import cv2
import numpy as np
import pyzbar.pyzbar as pyzbar
import imutils


class RobotDetection(object):
    def __init__(self):
        # self.limits = [(100, 0), (200, 0), (200, 100), (100, 100)]
        self.limits = [(0, 0), (-50, 0), (-50, -50), (0, -50)]
        # self.limits = [(0, 0), (-1, 0), (-1, -1), (0, -1)]
        self.hues = (157, 37, 87)
        # self.hues = (160)
        self.robot_mask = None
        self.cleared_mask = None
        self.center = np.asarray([0, 0], np.int32)
        self.forward = np.asarray([1, 0], np.int32)
        self.detected = False

    def find_circles(self, frame):
        circles = [None, None, None]
        centers = []
        forwards = []
        mask_sum = None
        if self.cleared_mask is None or self.cleared_mask.shape[:2] != frame.shape[:2]:
            self.cleared_mask = np.ones(frame.shape[:2], frame.dtype)
        for i, hue in enumerate(self.hues):
            colourLower = (int(hue), 50, 50)
            colourUpper = (int(hue) + 20, 255, 255)
            # grab the current frame

            blurred = cv2.GaussianBlur(frame, (1, 1), 0)
            hsv = cv2.cvtColor(blurred, cv2.COLOR_BGR2HSV)

            # construct a mask for the color "green", then perform
            # a series of dilations and erosions to remove any small
            # blobs left in the mask
            mask = cv2.inRange(hsv, colourLower, colourUpper)

            limit_mask = np.zeros(mask.shape, mask.dtype)
            h, w = mask.shape
            limits = np.asarray([(a if a >= 0 else w + a, b if b >= 0 else h + b) for b, a in self.limits], np.int32)
            cv2.fillPoly(limit_mask, [limits], 1)
            mask *= limit_mask

            # mask = cv2.erode(mask, None, iterations=1)
            # mask = cv2.dilate(mask, None, iterations=1)

            # return frame, mask, mask * 255

            if mask_sum is None:
                mask_sum = mask
            else:
                mask_sum += mask

            # find contours in the mask and initialize the current
            # (x, y) center of the ball
            cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL,
                                    cv2.CHAIN_APPROX_SIMPLE)
            cnts = imutils.grab_contours(cnts)
            center = None

            # only proceed if at least one contour was found
            if len(cnts) > 0:
                # find the largest contour in the mask, then use
                # it to compute the minimum enclosing circle and
                # centroid

                def circular_target(x):
                    radius = cv2.minEnclosingCircle(x)[1]
                    if radius <= 4 or radius >= 12:
                        return -1
                    out = min(cv2.contourArea(x) / (radius**2 * np.pi), (radius**2 * np.pi) / max(cv2.contourArea(x), 1))
                    # print(radius, out)
                    return out

                c = max(cnts, key=circular_target)
                ((x, y), radius) = cv2.minEnclosingCircle(c)

                circles[i] = np.asarray((x, y), np.float32)
                # cv2.circle(frame, (int(x), int(y)), 5 + i*2, (0, 255, 255), -1)
        if circles[0] is not None and circles[1] is not None:
            vx = circles[1] - circles[0]
            if vx[0] ** 2 + vx[1] ** 2 < 25 ** 2:
                vy = np.asarray((vx[1], -vx[0]), np.float32)
                center = circles[0] + vx / 2 - vy * 0.9
                centers.append(center)
                forwards.append(vy)
                cv2.circle(frame, tuple(center), 3, (255, 0, 255), -1)
                cv2.line(frame, tuple(center), tuple(center + vy), (255, 0, 255), 1)
        if circles[1] is not None and circles[2] is not None:
            c = circles[2] - circles[1]
            if c[0] ** 2 + c[1] ** 2 < 25 ** 2:
                cn = np.asarray((c[1], -c[0]), np.float32)
                vx = - c / 2 + cn / np.sqrt(2)
                vy = np.asarray((vx[1], -vx[0]), np.float32)
                center = circles[2]
                centers.append(center)
                forwards.append(vy)
                cv2.circle(frame, tuple(center), 3, (255, 0, 255), -1)
                cv2.line(frame, tuple(center), tuple(center + vy), (255, 0, 255), 1)
        if circles[0] is not None and circles[2] is not None:
            c = circles[2] - circles[0]
            if c[0] ** 2 + c[1] ** 2 < 25 ** 2:
                cn = np.asarray((c[1], -c[0]), np.float32)
                vx = c / 2 + cn / np.sqrt(2)
                vy = np.asarray((vx[1], -vx[0]), np.float32)
                center = circles[2]
                centers.append(center)
                forwards.append(vy)
                cv2.circle(frame, tuple(center), 3, (255, 0, 255), -1)
                cv2.line(frame, tuple(center), tuple(center + vy), (255, 0, 255), 1)
        mask_sum = (mask_sum.astype(np.float32) / np.max(mask_sum) * 255).astype(np.uint8)
        print(len(centers), end="\t")
        if len(centers) > 0:
            centers = np.asarray(centers, np.int32)
            forwards = np.asarray(forwards, np.int32)
            self.center[0] = np.average(centers[:, 0])
            self.center[1] = np.average(centers[:, 1])

            self.forward[0] = np.average(forwards[:, 0])
            self.forward[1] = np.average(forwards[:, 1])
            self.detected = True
        else:
            self.detected = False
        self.robot_mask = np.ones(frame.shape[:2], np.uint8)
        right = np.asarray((-self.forward[1], self.forward[0]), np.int32)

        for mask, side_mult, len_mult in [[self.robot_mask, 2.6, 3.0], [self.cleared_mask, 1.3, 1.5]]:
            corners = np.asarray((self.center + self.forward * 0.9 + self.forward * len_mult + right * side_mult,
                                  self.center + self.forward * 0.9 + self.forward * len_mult - right * side_mult,
                                  self.center + self.forward * 0.9 - self.forward * len_mult - right * side_mult,
                                  self.center + self.forward * 0.9 - self.forward * len_mult + right * side_mult), np.int32)
            h, w = frame.shape[:2]
            for i in range(4):
                corners[i][0] = np.clip(corners[i][0], 0, w)
                corners[i][1] = np.clip(corners[i][1], 0, h)

            # print(self.center, self.forward, right)
            # print(corners)
            cv2.fillPoly(mask, [corners], 0)

        return frame, self.robot_mask, self.cleared_mask, mask_sum

    def display(self, frame):
        return self.find_circles(frame)

