import socket
import time
import struct
import threading
import math
from Animations import *
running = True

UDP_PORT = 8888
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# sock.bind(('', UDP_PORT))
sock.connect(('192.168.0.112', UDP_PORT))
# st = '1'*96

led_count = 21
servo_count = 3
leds = [0 for i in range(led_count*3)]
servos = [0 for i in range(servo_count)]


def set_color(n, r, g, b):
    global leds
    leds[n*3] = r
    leds[n*3+1] = g
    leds[n*3+2] = b


def sender():
    global leds, servos, running
    while running:
        buf = struct.pack('%sB' % (led_count*3+servo_count+1+1),
                          servo_count, *servos, led_count, *leds)
        # print(len(buf))
        # client.publish("/LedController/leds", buf, 0)
        # client.publish("/LedController/servo", json.dumps({"servos":[j, j, j], "count": 3, "i":0}))
        sock.send(buf)
        time.sleep(0.006)


s_th = threading.Thread(target=sender)
s_th.daemon = True
s_th.start()
brit = 0.1
a_type = "none"



rgb_f = rgb_fading(3, leds)
def anim():
    global a_type, leds, servos, running
    while True:
        # if a_type == "rgb_fading":
        #     rgb_f.tick()
        #     time.sleep(0.01)
        if a_type == "rgb_fading_old":
            # servos[2] = 0
            for j in range(0, 360, 1):
                if a_type != "rgb_fading_old":
                    break
                for i in range(led_count):
                    # set_color(i, int((255-j)*brit), int(j*brit), 0)
                    set_color(i, check((math.sin(j/57.0)*128+128) * brit),
                            check((math.cos(j/57.0)*128+128) * brit),
                            check(((i/led_count*255) - (math.sin(j/57.0)*128+128)*0.8)*brit))
                # servos[1] = j
                # print(leds)
                time.sleep(0.01)
        if a_type == "rg_fading":
            # servos[2] = 0
            for j in range(0, 360, 2):
                if a_type != "rg_fading":
                    break
                for i in range(led_count):
                    # set_color(i, int((255-j)*brit), int(j*brit), 0)
                    set_color(i, check((math.sin(j/57)*128+128) * brit),
                              check((math.sin(j/57)*128+128) * brit), 0)
                time.sleep(0.01)
            time.sleep(0.05)
            # servos[2] = 255
            for j in range(360, 0, -2):
                if a_type != "rg_fading":
                    break
                for i in range(led_count):
                    # set_color(i, int((255-j)*brit), int(j*brit), 0)
                    set_color(i, check((math.sin(j/57)*128+128) * brit),
                              check((math.sin(j/57)*128+128) * brit), 0)
                    time.sleep(0.01)
            time.sleep(0.05)
        


anim_th = threading.Thread(target=anim)
anim_th.daemon = True
anim_th.start()
# set_color()
while True:
    # a_type = "none"
    # for i in range(led_count):
    #     set_color(i, 0, 0, 0)
    time.sleep(1)

    a_type = "rgb_fading_old"

    time.sleep(1)
    a_type = "rgb_fading_old"
    time.sleep(1)
    # time.sleep(2)
running = False
