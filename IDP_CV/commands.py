import socket
import time


class Commander(object):
    def __init__(self):
        self.TCP_IP = '192.168.43.161'
        self.TCP_PORT = 2390
        self.BUFFER_SIZE = 1024
        self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def turn(self, angle):
        angle = int(angle)
        num = 1
        while angle >= 180:
            angle -= 360
        while angle < -180:
            angle += 360
        if abs(angle) > 90:
            angle = angle // 2
            num = 2
        if angle < 0:
            angle = angle + 256
        for i in range(num):
            print(angle)
            msg = bytes([angle])
            self.s.sendto(msg, (self.TCP_IP, self.TCP_PORT))

    def halt(self):
        msg = b'h'
        print(msg)
        self.s.sendto(msg, (self.TCP_IP, self.TCP_PORT))

    def get_response(self):
        print(self.s.recv(1024))

if __name__ == "__main__":
    c = Commander()
    while 1:
        time.sleep(1)
        c.forward()
        # c.turn(67)
        # c.get_response()
