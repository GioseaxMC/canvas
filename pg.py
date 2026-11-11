import pygame as pg
import copy
from math import *

pg.joystick.init()

flags = 0
screen = 0
frames = 0
sprites = []
joypads = []
left_clicked = 0
right_clicked = 0
left_released = 0
right_released = 0
wheel_up = 0
wheel_down = 0
text_list = []
key_pres = 0
ascii = {chr(i): i for i in range(128)}
clock = pg.time.Clock()
updating_sizes = 0
minimum_sizes = (0,0)
fullscreen = 0
running = 1
resizable = 0
delta = 0
to_blit = []
WIDTH = 0
HEIGHT = 0
FPS = 0
locked = 0
kill_request = 0
focus_lost = 0
focus_gained = 0
draw_events = []

def add_draw_event(x):
    draw_events.append(x)

def sprites_kill_all():
    global kill_request; kill_request = 1

def quit():
    global running
    running = 0
    sprites_kill_all()

def blit(image, coords):
    to_blit.append((image, coords))
    return image, coords

def get_lock():
    return locked

def get_screen():
    return screen

def get_all():
    return sprites

def is_running():
    return running

def get_delta():
    return delta if delta else 1

def get_frames():
    return frames

def get_left_clicked():
    return left_clicked

def get_left_released():
    return left_released

def get_right_clicked():
    return right_clicked

def get_right_released():
    return right_released

def screen_size():
    return screen.get_size()

def get_wheel():
    return wheel_up - wheel_down

def get_FPS():
    return FPS

def get_key_press():
    return key_pres

def is_updating_sizes():
    return updating_sizes

def get_text_surface(text, size = 25, color = (0,0,0), font = None, antialiasing = True):
    fontText = pg.font.Font(font, size)
    text_surface = fontText.render(str(text), antialiasing, color)
    return text_surface

def rounded_rectangle(width, height, roundness, color):
    width = max(width*5, 0)
    height = max(height*5, 0)
    roundness = max(roundness*5, 0)
    surface = pg.Surface((width, height), pg.SRCALPHA)
    # new method
    # scircle = pg.Surface((roundness*5, roundness*5), pg.SRCALPHA)
    # pg.draw.circle(scircle, color, (roundness//2*5, roundness//2*5), roundness//2*5, 0)
    # scircle = pg.transform.smoothscale_by(scircle, 0.2)
    # surface.blit(scircle, (0,0))
    # surface.blit(scircle, (width-roundness, 0))
    # surface.blit(scircle, (width-roundness, height-roundness))
    # surface.blit(scircle, (0, height-roundness))

    # old method++
    pg.draw.circle(surface, color, (roundness//2, roundness//2), roundness//2, 0)
    pg.draw.circle(surface, color, (width-roundness//2, roundness//2), roundness//2, 0)
    pg.draw.circle(surface, color, (width-roundness//2, height-roundness//2), roundness//2, 0)
    pg.draw.circle(surface, color, (roundness//2, height-roundness//2), roundness//2, 0)
    pg.draw.rect(surface, color, pg.Rect(0, roundness//2, width, height-roundness))
    pg.draw.rect(surface, color, pg.Rect(roundness//2, 0, width-roundness, height))
    return pg.transform.smoothscale_by(surface, 0.2)

def rectangle(width, height, color):
    """color must be type _color (tuple of R,G,B)"""
    surface = pg.Surface((max(width,0), max(height,0)))
    surface.fill(color)
    return surface

class joystick:
    def __init__(self, id) -> None:
        if pg.joystick.get_count() >= id+1:
            self.joypad = pg.joystick.Joystick(id)
            self.joypad.init()
            self.button_press = ""
            self.button_up = ""
            if len(joypads) < id+1:
                joypads.append(self)
            else:
                joypads[id] = self

    def button_clicked(self, button):
        return self.button_press == button

    def button_released(self, button):
        return self.button_up == button
    
    def get_axis(self, multiplier = 1):
        num_axes = self.joypad.get_numaxes()
        axis_values = [round(self.joypad.get_axis(i), 2) * multiplier for i in range(num_axes)]
        return axis_values
    
    def buttons_clicked(self, *buttons):
        condition = 0
        for button in buttons:
            condition = condition or self.button_press == button
        return condition
    
    def buttons_released(self, *buttons):
        condition = 0
        for button in buttons:
            condition = condition or self.button_up == button
        return condition
    
    def get_lever(self, lever):
        num_axes = self.joypad.get_numaxes()
        axis_values = [round(self.joypad.get_axis(i), 2) for i in range(num_axes)]
        if "r" in lever.lower():
            return axis_values[-1] > 0
        elif "l" in lever.lower():
            return axis_values[-2] > 0
        
    def button_down(self, button):
        return self.joypad.get_button(button)

    def buttons_down(self, *buttons):
        condition = 0
        for butt in buttons:
            condition = condition or self.joypad.get_button(butt)
        return condition

    def get_sticks_direction(self, side, direction, multiplier = 1):
        if "l" in side.lower():
            side = 0
        elif "r" in side.lower():
            side = 2
        if "x" in direction.lower():
            direction = 0
        elif "y" in direction.lower():
            direction = 1
        axis_values = self.get_axis(multiplier)
        return axis_values[direction+side]
    
    def get_stick(self, side, multiplier = 1):
        return self.get_sticks_direction(side, "x", multiplier), self.get_sticks_direction(side, "y", multiplier)
    
    def get_d_pad(self, direction):
        conv = {"up" : 11,
                "down" : 12,
                "left" : 13,
                "right" : 14}
        return self.joypad.get_button(conv[direction.lower()])

def window(width = 480,height = 360,title = "window", can_resize=1, smallest_window_sizes = (480,360), icon = None, full_Screen = 0, do_draws = 1, hide_window_border = 0):
    global call_draws; call_draws = do_draws
    global resizable; resizable = can_resize
    global minimum_sizes; minimum_sizes = smallest_window_sizes
    global fullscreen; fullscreen = full_Screen
    global no_frame; no_frame = hide_window_border
    global flags
    global updating_sizes; updating_sizes = 1
    pg.init()
    global screen
    flags = 0
    if resizable:
        flags = flags | pg.RESIZABLE
    if fullscreen:
        flags = flags | pg.FULLSCREEN
    if no_frame:
        flags = flags | pg.NOFRAME
    screen = pg.display.set_mode((width,height), flags)
    if icon:
        pg.display.set_icon(pg.image.load(icon))

    pg.display.set_caption(title)
    return screen

def set_title(title):
    pg.display.set_caption(title)

def draw_calls(fps = None):
    global delta, frames, to_blit, sprites, text_list, locked; locked = 1
    if frames > (1 << 16):
        frames = 0
    for sprite in sprites:
        sprite.draw()
    for blit in to_blit:
        screen.blit(blit[0],blit[1])
    for text in text_list:
        screen.blit(text[0],text[1])
    for event in draw_events:
        event()
    draw_events.clear()
    to_blit.clear()
    text_list.clear()
    pg.display.flip()
    if fps:
        delta = clock.tick(fps) / (1000 / fps)
    locked = 0

def loop(fps_goal = None, background = (255,255,255)):
    global running
    global frames
    global FPS; FPS = int(clock.get_fps())
    frames += 1
    for joy in joypads:
        joy.button_press = ""
        joy.button_up = ""
    global screen; screen.fill(background)
    global left_clicked, right_clicked; left_clicked = 0 ; right_clicked = 0
    global right_released, left_released; right_released = 0; left_released = 0
    global wheel_up, wheel_down; wheel_down = 0; wheel_up = 0
    global text_list, key_pres, key_unicode; key_pres = 0; key_unicode = ""
    global updating_sizes; updating_sizes = 0
    global minimum_sizes
    global flags
    global fullscreen
    global WIDTH; global HEIGHT
    global resizable
    global delta
    global to_blit
    global call_draws
    global kill_request
    global focus_lost; focus_lost = 0
    global focus_gained; focus_gained = 0

    if kill_request:
        sprites.clear()
        kill_request = 0
    # draw_calls(fps_goal)

    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = 0
        elif event.type == pg.MOUSEBUTTONDOWN:
            if event.button == 1:
                left_clicked = 1
            elif event.button == 3:
                right_clicked = 1
            elif event.button == 4:
                wheel_up = 1
            elif event.button == 5:
                wheel_down = 1
        elif event.type == pg.MOUSEBUTTONUP:
            if event.button == 1:
                left_released = 1
            elif event.button == 3:
                right_released = 1
        elif event.type == pg.KEYDOWN:
            key_unicode = event.unicode
            key_pres = event.key
            if key_pres == pg.K_F11:
                if resizable:
                    if fullscreen:
                        fullscreen = 0
                        flags &= ~pg.FULLSCREEN
                        screen = pg.display.set_mode(minimum_sizes, flags)
                    else:
                        fullscreen = 1
                        flags |= pg.FULLSCREEN
                        screen = pg.display.set_mode((0,0), flags)
                    WIDTH,HEIGHT = screen_size()
                    updating_sizes = 1   
        elif event.type == pg.VIDEORESIZE:
            updating_sizes = 1
            screen = pg.display.set_mode((max(event.w,minimum_sizes[0]),max(event.h,minimum_sizes[1])), flags)
            WIDTH,HEIGHT = screen_size()
        elif event.type == pg.JOYBUTTONDOWN:
            joypads[event.joy].button_press = event.button
        elif event.type == pg.JOYBUTTONUP:
            joypads[event.joy].button_up = event.button
        elif event.type == pg.ACTIVEEVENT:
            if event.gain:
                focus_gained = 1
            else:
                focus_lost = 1
    return running

def get_clicked_key():
    return key_pres

def get_clicked_unicode():
    return key_unicode

def gained_focus():
    return focus_gained

def lost_focus():
    return focus_lost

def mouse_down(button = "left"):
    if button == "left" or button == 0:
        return pg.mouse.get_pressed()[0]
    elif button == "right" or button == 1:
        return pg.mouse.get_pressed()[2]

def mouse_position():
    return pg.mouse.get_pos()

def global_mouse_position():
    return tuple(x+y for x, y in zip(mouse_position(), pg.display.get_window_position()))

def fill(*color_value):
    global color
    if len(color_value) == 1:
        color = color_value[0]
    else:
        color = (color_value[0],color_value[1],color_value[2])

def set_smoothly(value,target,smoothness):
    return value + (target - value) / smoothness

def key_pressed(key):
    global ascii
    try:
        return pg.key.get_pressed()[ascii[key]]
    except:
        return pg.key.get_pressed()[key]
    
def key_clicked(key):
    global ascii
    try:
        return key_pres == ascii[key]
    except:
        return key_pres == key
    
def keys_pressed(*keys):
    pressed = 0
    for key in keys:
        pressed = key_pressed(key) or pressed
    return pressed

def keys_clicked(*keys):
    pressed = 0
    for key in keys:
        pressed = key_clicked(key) or pressed
    return pressed

def ctrl():
    return keys_pressed(pg.K_LCTRL , pg.K_RCTRL)

def shift():
    keys = pg.key.get_mods()
    return keys & pg.KMOD_SHIFT

def text(text, position = (0,0), size = 25, color = (0,0,0), font = None, antialiasing = 1, topLeft = True):
    if type(font) == pg.font.Font:
        text_surface = font.render(str(text), antialiasing, color)
    else:
        fontText = pg.font.Font(font, size)
        text_surface = fontText.render(str(text), antialiasing, color)
    if topLeft:
        drawPos = (position[0],position[1])
    else:
        drawPos = (position[0] - (text_surface.get_width() // 2),position[1] - (text_surface.get_height() // 2))
    text_list.append((text_surface, drawPos))

def cool_text(prompt, position = (0,0), size = 25, offset = 0.1, colors = ("white", "black"), font = None, antialiasing = True, topleft = True):
    text(prompt, (position[0] + (size * offset), position[1] + (size * offset)), size, colors[1], font, antialiasing, topleft)
    text(prompt, position, size, colors[0], font, antialiasing, topleft)

def debug_list(*list, color = (255,255,255), position = (0,0), font):
    
    if not type(font) == pg.font.Font:
        font = pg.font.Font(font, 25)

    i = 0
    for item in list:
        text(str(item), (position[0],position[1]+i*20),color=color, font=font)
        i += 1

def adjust_brightness(image_surface, brightness_factor):
    black_surface = pg.Surface(image_surface.get_size())
    black_surface.fill([brightness_factor]*3)
        
    adjusted_surface = image_surface.copy()
    adjusted_surface.blit(black_surface, (0, 0), special_flags = pg.BLEND_RGBA_MULT)

    return adjusted_surface

class sprite:
    def __init__(self, images = [], pos = (0,0), direction = 0, scale = 100, depth = 0, hide = 0, alpha = 255, frame = 0, sounds = [], top_left = True, smooth = 0):
        """images can be surface.
        images must be list, even if only one item is given.
        
        ATTENTION: Never change sprite position from sprite.pos_Y, sprite.pos_X or sprite.position, they are read only."""    
        try:
            self.sprite_images = [(pg.image.load(image).convert_alpha()) for image in images]
        except TypeError:
            self.sprite_images = [(image.convert_alpha()) for image in images]
        self.sound_name = [(sound) for sound in sounds]
        self.sounds = [(pg.mixer.Sound(sound)) for sound in sounds]
        for sound in self.sounds:
            sound.set_volume(.2)
        self.pos_X = pos[0]
        self.pos_Y = pos[1]
        self.position = (pos[0],pos[1])
        self.direction = direction
        self.scale = scale
        sprites.append(self)
        self.frame = frame
        self.appearence = self.sprite_images[self.frame]
        self.hitbox = pg.transform.scale(self.appearence, (self.appearence.get_width() / 5, self.appearence.get_height() / 5))
        self.is_clone = 0
        self.depth = depth
        self.hide = hide
        self.alpha = alpha
        self.brightness = 255
        rect = self.sprite_images[self.frame].get_rect()
        self.drawPos = (self.pos_X - (rect.width >> 1),self.pos_Y - (rect.height >> 1))
        self.ld = None
        self.sc = None
        self.lf = None
        self.br = None
        self.top_left = top_left
        self.smooth = smooth
        # leave last
        self.update()

    def play_sound(self,index_or_file):
        try:
            self.sounds[index_or_file]
            return self.sounds[index_or_file].play()
        except TypeError:
            self.sounds[self.sound_name.index(index_or_file)]
            return self.sounds[self.sound_name.index(index_or_file)].play()

    def update(self, forced = 0):
        if forced or not (self.direction == self.ld and self.scale == self.sc and self.frame == self.lf and self.brightness == self.br):
            self.lf = self.frame
            rect = self.sprite_images[self.frame % len(self.sprite_images)].get_rect()
            sprite_image = self.sprite_images[self.frame % len(self.sprite_images)]
            if self.smooth:
                scaled_image =  pg.transform.smoothscale_by(sprite_image, self.scale / 100)
            else:
                scaled_image =  pg.transform.scale_by(sprite_image, self.scale / 100)
            self.appearence = pg.transform.rotate(scaled_image, self.direction)
            self.appearence = adjust_brightness(self.appearence, self.brightness)
            self.appearence.set_alpha(self.alpha)
            if self.top_left:
                self.drawPos = self.get_position()
            else:
                rect = self.appearence.get_rect()
                self.drawPos = (self.pos_X - (rect.width >> 1),self.pos_Y - (rect.height >> 1))
            self.ld , self.sc , self.br = self.direction, self.scale, self.brightness
            self.hitbox = pg.transform.scale(self.appearence, (self.appearence.get_width() // 5, self.appearence.get_height() // 5))
            self.mask = pg.mask.from_surface(self.hitbox)
        else:
            if self.top_left:
                self.drawPos = self.get_position()
            else:
                rect = self.appearence.get_rect()
                self.drawPos = (self.pos_X - (rect.width >> 1),self.pos_Y - (rect.height >> 1))

    def get_sizes(self):
        return self.appearence.get_size()

    def set_position(self,*pos):
        if len(pos) == 1:
            self.pos_X,self.pos_Y = pos[0][0],pos[0][1]
        else:
            self.pos_X, self.pos_Y = pos[0],pos[1]
        self.update()

    def set_scale(self, value: int):
        self.scale = value
        self.update()

    def set_direction(self, direction: int):
        self.direction = direction
        self.update()

    def change_position(self,*pos):
        if len(pos) == 1:
            self.pos_X,self.pos_Y = self.pos_X+pos[0][0],self.pos_Y+pos[0][1]
        else:
            self.pos_X, self.pos_Y = self.pos_X+pos[0],self.pos_Y+pos[1]
        self.update()

    def get_position(self):
        return (self.pos_X, self.pos_Y)

    def draw(self):
        if not self.hide:
            screen.blit(self.appearence, self.drawPos)
    
    def move(self,speed,offset = 0, direction = ""):
        if direction == "":
            direction = self.direction + offset
        self.pos_X += cos(direction * pi / 180) * speed
        self.pos_Y -= sin(direction * pi / 180) * speed
        self.update()
    
    def point_towards(self, *pos, offset=0):
        if len(pos) == 1:
            try:
                posx,posy = pos[0][0],pos[0][1]
            except:
                posx,posy = pos[0].pos_X,pos[0].pos_Y
        else:
            posx,posy = pos[0],pos[1]
        
        self.direction = degrees(atan2(self.pos_Y - posy , posx - self.pos_X)) + offset
        self.update()

    def direction_to(self, *pos):
        if len(pos) == 1:
            try:
                posx,posy = pos[0][0],pos[0][1]
            except:
                posx,posy = pos[0].pos_X,pos[0].pos_Y
        else:
            posx,posy = pos[0],pos[1]
        
        return degrees(atan2(posy - self.pos_Y, posx - self.pos_X))

    def slide_to(self, smooth, *pos):
        if len(pos) == 1:
            posx,posy = pos[0][0],pos[0][1]
        else:
            posx,posy = pos[0],pos[1]
        if not (posx,posy) == (self.pos_X,self.pos_Y):
            self.pos_X += (posx - self.pos_X) / smooth
            self.pos_Y += (posy - self.pos_Y) / smooth
            self.update()

    def touching_px(self, sprite):
        self.update()
        return self.mask.overlap(sprite.mask, ((sprite.drawPos[0] - self.drawPos[0])//5, (sprite.drawPos[1] - self.drawPos[1])//5)) != None

    def touching_point_px(self, point):
        self.update()
        try:
            posx = (point[0] - self.pos_X) // 5 + self.hitbox.get_width() // 2
            posy = (point[1] - self.pos_Y) // 5 + self.hitbox.get_height() // 2
            if self.mask.get_at((posx,posy)):
                return True
        except:
            pass
        return False
    
    def touching_mouse_px(self):
        return self.touching_point_px(pg.mouse.get_pos())
    
    def touching_point(self, point):
        if self.top_left:
            rect = self.appearence.get_rect(topleft=self.get_position())
        else:
            rect = self.appearence.get_rect(center=self.get_position())
        return rect.collidepoint(point)
    
    def touching_mouse(self):
        return self.touching_point(mouse_position())

    def distance_to(self,*pos):
        if len(pos) == 1:
            return sqrt(((self.pos_X - pos[0][0]) ** 2) + ((self.pos_Y - pos[0][1]) ** 2))
        else:
            return sqrt(((self.pos_X - pos[0]) ** 2) + ((self.pos_Y - pos[1]) ** 2))
        
    def clone(self):
        self.is_clone += 1
        clone = copy.copy(self)
        sprites.insert(sprites.index(self),clone)
        self.is_clone -= 1
        return clone
        
    def kill_clone(self):
        if self.is_clone:
            sprites.remove(self)
            return self
        
    def kill(self):
        sprites.remove(self)
        return self
    
    def set_depth(self,z):
        global sprites
        if z == "first" or z == 0:
            sprites.remove(self)
            sprites.append(self)
        elif z == "last":
            sprites.remove(self)
            sprites.insert(0, self)
        else:
            self.depth = z
            sprites = sorted(sprites, key=lambda x: x.depth)

    def animate(self, frames_quantity, first_frame, speed):
        self.frame = (frames * speed//60) % frames_quantity + first_frame

    def clicked(self,mouse_button = 0):
        if mouse_button in ["right", 1] :
            return right_clicked and self.touching_mouse() and not self.hide
        elif mouse_button in ["left", 0]:
            return left_clicked and self.touching_mouse() and not self.hide
        
    def released(self, mouse_button = 0):
        if mouse_button in ["right", 1] :
            return right_released and self.touching_mouse()
        elif mouse_button in ["left", 0]:
            return left_released and self.touching_mouse()
        
def default_template(fileName):
    """
    fileName should be: __file__

    functions loads default template for pg2.
    """
    with open(fileName, "w") as file:
        file.write("""import pg_canvas as c

c.window(title = "game")

# sprite declaration here:
object = c.sprite((c.rectangle(50,50,(0,200,255)),),(240,180))

while c.loop(60):
    
    # your code here:
    pass
""")

def is_in(amax,amin,cmax,cmin):
    return cmin < amax and amin < cmax

def get_collisions(list = sprites):
    
    collisions = []
    potentials = []
    aMax = 0
    aMin = 0
    item = 0

    for list in sorted(list, key=lambda spr: spr.pos_X):
        cmax = list.pos_X + list.appearence.get_width() // 2
        cmin = cmax - list.appearence.get_width()
        if aMax == 0 and aMin == 0:
            aMax = cmax
            aMin = cmin
            potentials.append([list,])
        elif is_in(aMax,aMin,cmax,cmin):
            aMax = max(aMax,cmax)
            aMin = min(aMin,cmin)
            potentials[item].append(list)
        else:
            item += 1
            potentials.append([list,])
            aMax = cmax
            aMin = cmin
    for possibles in potentials:
        for i in range(len(possibles)):
            possible = possibles[i]
            for j in range(i+1,len(possibles)):
                if possible.touching(possibles[j]):
                    collisions.append((possible,possibles[j]))
    return collisions

def physics(precision = 10, *args):
    global sprites
    toCheck = [item for item in sprites if not item in args]
    for _ in range(precision // 2):
        x = get_collisions(toCheck)
        if not x:
            break
        for collider in x:
            collider[0].move(collider[0].distance_to(collider[1].get_position()) / -precision, direction = collider[0].direction_to(collider[1]))
            collider[1].move(collider[1].distance_to(collider[0].get_position()) / -precision, direction = collider[1].direction_to(collider[0]))

def flick(condition, variable):
    if condition:
        return (not variable), True
    return variable, False
