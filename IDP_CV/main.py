# import socket
# import time
# from multiprocessing.connection import Client
#
# TCP_IP = '192.168.43.160' #Arduino WiFi IP
# TCP_PORT = 2390             #Arudino WiFi Port
# BUFFER_SIZE = 1024
#
#
# ard_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# # ard_sock.connect((TCP_IP, TCP_PORT))
# for i in range(1000):
#     ard_sock.sendto(bytes("a", 'utf-8'), (TCP_IP, TCP_PORT))
#     print("a")
#     time.sleep(1)


# import the necessary packages
from collections import deque
from imutils.video import VideoStream
import numpy as np
import cv2
import imutils
import time
import shutil
import os
import pyqrcodeng as pyqrcode

import frame_loader
import mine_detection
import robot_detection

record = None
# source = "table2.avi"
source = 1

if type(source) == int:
    fl = frame_loader.CameraFrameLoader(source, record)
else:
    fl = frame_loader.VideoFrameLoader(source, record)

md = mine_detection.MineDetection()
rd = robot_detection.RobotDetection()

# keep looping
while True:
    frame = fl.get_frame_cropped().copy()
    frame, robot_mask, mask = rd.find_circles(frame)
    _ = md.get_mines_mask(frame, robot_mask)
    frame = md.get_mine_positions(frame)

    # show the frame to our screen
    display = np.concatenate((frame, np.stack((mask, mask, mask), 2)), 1)
    cv2.imshow("Frame", display)
    key = cv2.waitKey(1) & 0xFF

    # if the 'q' key is pressed, stop the loop
    if key == ord("q"):
        break

# close all windows
cv2.destroyAllWindows()
del fl
del md
