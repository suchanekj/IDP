import time
import numpy as np
import cv2


def _angle_between(v1, v2):
    unit_vector = lambda vector: vector / np.linalg.norm(vector)
    v1_u = unit_vector(v1)
    v1_r = np.asarray((v1_u[1], -v1_u[0]), np.float32)
    v2_u = unit_vector(v2)
    angle_cos = np.arccos(np.clip(np.dot(v1_u, v2_u), -1.0, 1.0)) * 180 / np.pi
    r_cos = np.dot(v1_r, v2_u)
    if r_cos > 0:
        return angle_cos
    else:
        return -angle_cos


class Planner(object):
    def __init__(self, md, rd, c):
        self.md = md
        self.rd = rd
        self.c = c
        self.last_time = time.time()
        self.target = None

    def run(self, frame):
        if not self.rd.detected:
            return frame
        mines = self.md.mine_positions
        r_center = self.rd.center
        r_forward = self.rd.forward
        if self.target is not None and np.linalg.norm(self.target - r_center) < 75:
            self.target = None
        if self.target is None:
            if len(mines) == 0:
                return frame
            mine = mines[0]
            for m in mines:
                if np.linalg.norm(m) < np.linalg.norm(mine): # - r_center
                    mine = m
            self.target = mine
        # angle = vg.angle(mine - r_center, r_forward)
        cv2.line(frame, tuple(self.target), tuple(r_center), (255, 255, 0), 1)
        angle = _angle_between(self.target - r_center, r_forward)
        if time.time() - self.last_time > 0.2:
            self.c.turn(angle)
            # self.c.forward()
            self.last_time = time.time()
        return frame

    def wait(self):
        if time.time() - self.last_time > 0.2:
            self.c.halt()
            self.last_time = time.time()

