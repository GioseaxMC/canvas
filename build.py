import argparse
import configparser
import os
import platform
import shlex
import shutil
import subprocess
import sys
from pathlib import Path

INI_TEMPLATE = """\
[project]
name =
type =

source_dir =
sources =

header_dir =

include_dirs =

library_dirs =

libraries =

c_flags =

cxx_flags =

link_flags =
"""

def run(cmd):
    print("+", subprocess.list2cmdline([str(x) for x in cmd]))
    subprocess.run(cmd, check=True)


def git_root():
    result = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"],
        capture_output=True, text=True, check=True
    )
    return Path(result.stdout.strip())


def find_compiler(preference):
    compilers = {
        "gcc": [
            ("gcc", "g++"),
        ],
        "clang": [
            ("clang", "clang++"),
            *[
                (f"clang-{v}", f"clang++-{v}")
                for v in range(20, 9, -1)
            ],
        ],
    }

    order = ["gcc", "clang"] if preference == "auto" else [preference]

    for compiler in order:
        for cc_name, cxx_name in compilers[compiler]:
            cc_path = shutil.which(cc_name)
            cxx_path = shutil.which(cxx_name)

            if cc_path and cxx_path:
                print(f"Using {cc_name} / {cxx_name}")
                return (
                    compiler,
                    Path(cc_path).resolve(),
                    Path(cxx_path).resolve(),
                )

    raise RuntimeError(f"No complete compiler toolchain found: {preference}")


def compiler_prefix(cxx):
    return cxx.parent.parent


def entries(value):
    return [
        item.strip()
        for line in value.splitlines()
        for item in line.split(";")
        if item.strip()
    ]

def main():
    parser = argparse.ArgumentParser(description="Generic C/C++ build tool")

    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--build", action="store_true")
    action.add_argument("--install", action="store_true")

    parser.add_argument("--compiler", choices=["auto", "gcc", "clang"], default="auto")
    parser.add_argument("--build-dir", default="buildpy")
    parser.add_argument("--config", default="buildpy.ini")
    parser.add_argument("--prefix")
    action.add_argument("--init", action="store_true", help="Generate buildpy.ini template")

    args = parser.parse_args()

    root = git_root()
    build_dir = root / args.build_dir
    config_path = root / args.config

    if args.init:
        if config_path.exists():
            raise RuntimeError("buildpy.ini already exists")

        config_path.write_text(INI_TEMPLATE, encoding="utf-8")
        print(f"Generated {config_path}")
        return
    
    if not config_path.exists():
        raise RuntimeError(f"Configuration not found: {config_path}")

    config = configparser.ConfigParser()
    config.read(config_path)

    if "project" not in config:
        raise RuntimeError("Missing [project] section")

    project = config["project"]

    # Project configuration
    name = project["name"]
    library_type = project["type"].lower()
    source_dir = root / project["source_dir"]
    header_dir = root / project["header_dir"]

    if library_type not in ("static", "shared"):
        raise RuntimeError("Project type must be static or shared")

    sources = [source_dir / f for f in entries(project["sources"])]

    if not sources:
        raise RuntimeError("No source files configured")

    for source in sources:
        if not source.is_file():
            raise RuntimeError(f"Source file not found: {source}")

    include_dirs = [
        (root / p).resolve()
        for p in entries(project.get("include_dirs", ""))
    ]

    library_dirs = [
        Path(p).resolve() if Path(p).is_absolute() else (root / p).resolve()
        for p in entries(project.get("library_dirs", ""))
    ]

    libraries = entries(project.get("libraries", ""))

    # Build
    if args.build:
        compiler_name, cc, cxx = find_compiler(args.compiler)

        prefix = (
            Path(args.prefix).resolve()
            if args.prefix
            else compiler_prefix(cxx)
        )

        build_dir.mkdir(parents=True, exist_ok=True)
        object_dir = build_dir / "obj"
        object_dir.mkdir(exist_ok=True)

        system = platform.system()
        windows = system == "Windows"
        macos = system == "Darwin"

        objects = []

        for source in sources:
            relative = source.relative_to(source_dir)
            obj = object_dir / relative.parent / (relative.name + ".o")
            obj.parent.mkdir(parents=True, exist_ok=True)

            compiler = cc if source.suffix.lower() == ".c" else cxx

            command = [str(compiler), "-c", str(source), "-o", str(obj)]

            for directory in include_dirs:
                command.extend(["-I", str(directory)])

            if library_type == "shared" and not windows:
                command.append("-fPIC")

            flags = (
                project.get("c_flags", "")
                if source.suffix.lower() == ".c"
                else project.get("cxx_flags", "")
            )

            command.extend(shlex.split(flags, posix=not windows))

            run(command)
            objects.append(obj)

        # Determine output files
        if library_type == "static":
            output = build_dir / f"lib{name}.a"
        elif windows:
            output = build_dir / f"{name}.dll"
        elif macos:
            output = build_dir / f"lib{name}.dylib"
        else:
            output = build_dir / f"lib{name}.so"

        if library_type == "static":
            ar = shutil.which("ar") or shutil.which("llvm-ar")

            if not ar:
                raise RuntimeError("Could not find ar or llvm-ar")

            run([ar, "rcs", str(output), *map(str, objects)])

        else:
            command = [str(cxx)]

            if windows:
                command.append("-shared")
                import_lib = build_dir / f"lib{name}.dll.a"
                command.append(f"-Wl,--out-implib,{import_lib}")
            elif macos:
                command.append("-dynamiclib")
            else:
                command.append("-shared")

            command.extend(["-o", str(output)])
            command.extend(map(str, objects))

            for directory in library_dirs:
                command.extend(["-L", str(directory)])

            command.extend(f"-l{lib}" for lib in libraries)
            command.extend(
                shlex.split(project.get("link_flags", ""), posix=not windows)
            )

            run(command)

        # Save build metadata for --install
        state = configparser.ConfigParser()
        state["build"] = {
            "prefix": str(prefix),
            "output": str(output),
            "library_type": library_type,
            "windows": str(windows),
            "import_lib": str(build_dir / f"lib{name}.dll.a")
                if library_type == "shared" and windows else "",
        }

        with open(build_dir / "build-info.ini", "w") as file:
            state.write(file)

        print(f"\nBuilt {output}")
        print(f"Install prefix: {prefix}")

    # Install
    elif args.install:
        state_path = build_dir / "build-info.ini"

        if not state_path.exists():
            raise RuntimeError("No build metadata. Run --build first.")

        state = configparser.ConfigParser()
        state.read(state_path)
        info = state["build"]

        prefix = Path(args.prefix).resolve() if args.prefix else Path(info["prefix"])

        output = Path(info["output"])
        library_type = info["library_type"]
        windows = info.getboolean("windows")

        if not output.exists():
            raise RuntimeError(f"Built library not found: {output}")

        lib_dir = prefix / "lib"
        bin_dir = prefix / "bin"
        include_dir = prefix / "include"

        lib_dir.mkdir(parents=True, exist_ok=True)
        bin_dir.mkdir(parents=True, exist_ok=True)
        include_dir.mkdir(parents=True, exist_ok=True)

        if library_type == "static":
            shutil.copy2(output, lib_dir / output.name)

        elif windows:
            shutil.copy2(output, bin_dir / output.name)

            import_lib = Path(info["import_lib"])
            if import_lib.exists():
                shutil.copy2(import_lib, lib_dir / import_lib.name)

        else:
            shutil.copy2(output, lib_dir / output.name)

        # Copy public headers, preserving directory structure
        if not header_dir.is_dir():
            raise RuntimeError(f"Header directory not found: {header_dir}")

        shutil.copytree(header_dir, include_dir, dirs_exist_ok=True)

        print(f"\nInstalled {output.name}")
        print(f"Library directory: {lib_dir}")
        print(f"Header directory: {include_dir}")


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, subprocess.CalledProcessError, KeyError) as error:
        print(f"Error: {error}", file=sys.stderr)
        sys.exit(1)
