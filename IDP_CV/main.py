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
import commands
import planner

record = None
# source = "table2.avi"
source = 1


if type(source) == int:
    fl = frame_loader.CameraFrameLoader(source, record)
else:
    fl = frame_loader.VideoFrameLoader(source, record)

md = mine_detection.MineDetection()
rd = robot_detection.RobotDetection()
c = commands.Commander()
p = planner.Planner(md, rd, c)

try:
    while True:
        p.wait()
except KeyboardInterrupt:
    print("great")

# keep looping
while True:
    try:
        frame = fl.get_frame_cropped().copy()
        frame, robot_mask, cleared_mask, rd_mask = rd.find_circles(frame)
        mine_mask = md.get_mines_mask(frame, robot_mask * cleared_mask)
        frame = md.get_mine_positions(frame)
        frame = p.run(frame)
        # mask = robot_mask * 255
        # mask = mine_mask
        # mask = cleared_mask * 255
        mask = cleared_mask * 127 + mine_mask // 2
        # maskL =

        # show the frame to our screen
        display = np.concatenate((frame, np.stack((mask, mask, mask), 2)), 1)
        cv2.imshow("Frame", display)
        key = cv2.waitKey(1) & 0xFF

        # if the 'q' key is pressed, stop the loop
        if key == ord("q"):
            break
    except KeyboardInterrupt:
        print("great")
        md = mine_detection.MineDetection()
        rd = robot_detection.RobotDetection()
        c = commands.Commander()
        p = planner.Planner(md, rd, c)

# close all windows
cv2.destroyAllWindows()
del fl
del md
