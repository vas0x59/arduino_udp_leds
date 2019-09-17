import math

def check(a):
    if a < 0:
        return 0
    elif a > 255:
        return 255
    else:
        return int(a)

class rgb_fading:
    def __init__(self, delta, leds):
        self.j = 0
        self.delta = delta
        self.leds = leds

    def set_color(self, n, r, g, b):
        # global leds
        self.leds[n*3] = r
        self.leds[n*3+1] = g
        self.leds[n*3+2] = b
    def tick(self):
        # leds = []
        for i in range(led_count):
            # set_color(i, int((255-j)*brit), int(j*brit), 0)
            set_color(i, check((math.sin(self.j/57.0)*128+128) * brit),
                      check((math.cos(self.j/57.0)*128+128) * brit),
                      check(((i/led_count*255) - (math.sin(self.j/57.0)*128+128)*0.8)*brit))
        if j < 360:
            j += self.delta
        else:
            j = 0
        return self.leds
