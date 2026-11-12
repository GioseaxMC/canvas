# Canvas

A C++ utility library built on top of [Allegro 5](https://liballeg.org/), providing reusable components for 2D application and game development.

## Requirements

- C++ compiler (GCC or Clang)
- Python 3.12+ (for building)
- Allegro 5.2.12+
  - Allegro core
  - Image addon
  - Primitives addon
  - Audio addon
  - Audio codec addon
  - Font addon
  - TTF addon

## Building

Canvas uses a custom Python build script.

Configure your build using `buildpy.ini`, then run:

```bash
python3.12 build.py --build
```

To install the library:

```bash
python3.12 build.py --install
```

The installation prefix and compiler settings can be configured in `buildpy.ini`.

## Linking

When linking against Canvas, include its Allegro dependencies:

```bash
g++ main.cpp -o app -lcanvas \
    -lallegro \
    -lallegro_image \
    -lallegro_primitives \
    -lallegro_audio \
    -lallegro_acodec \
    -lallegro_font \
    -lallegro_ttf
```

## Examples

See the [`examples/`](examples/) directory for usage examples.

## License

See [LICENSE](LICENSE).