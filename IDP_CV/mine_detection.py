import cv2
import numpy as np


# blurSizeLims = (0, 5)
# tresholdSizeLims = (5, 15)
# erodesLims = (0, 3)
# treshold = 2
# blurSize = blurSizeLims[0]
# tresholdSize = tresholdSizeLims[0]
# erodes = erodesLims[0]
# step = 1
#
# erodes = 1
# treshold = 3
# blurSize = 5
# tresholdSize = 10
# step += 1
# if step % 64 == 0:
#     blurSize += 1
#     if blurSize > blurSizeLims[1]:
#         blurSize = blurSizeLims[0]
#         tresholdSize += 5
#         if tresholdSize > tresholdSizeLims[1]:
#             tresholdSize = tresholdSizeLims[0]
#             erodes += 1
#             if erodes > erodesLims[1]:
#                 erodes = erodesLims[0]
#                 treshold += 1
#     print("er", erodes, "tres", treshold, "bs", blurSize, "ts", tresholdSize)

def _draw_grid(img, line_color=0, thickness=1, type_=cv2.LINE_AA, pxstep=100):
    '''(ndarray, 3-tuple, int, int) -> void
    draw gridlines on img
    line_color:
        BGR representation of colour
    thickness:
        line thickness
    type:
        8, 4 or cv2.LINE_AA
    pxstep:
        grid line frequency in pixels
    '''
    x = pxstep
    y = pxstep
    while x < img.shape[1]:
        cv2.line(img, (x, 0), (x, img.shape[0]), color=line_color, lineType=type_, thickness=thickness)
        x += pxstep

    while y < img.shape[0]:
        cv2.line(img, (0, y), (img.shape[1], y), color=line_color, lineType=type_, thickness=thickness)
        y += pxstep


class MineDetection(object):
    def __init__(self):
        self.erodes = 1
        self.treshold = 3
        self.blurSize = 5
        self.tresholdSize = 10
        self.limits = [(50, 150), (-1, 170) , (-1, -32), (50, -32)]
        self.mine_mask = None
        self.momentum = 0.9
        self.mine_positions = []
        self.remaining_mines = 7
        self.mask_remaining = None

    def get_mines_mask(self, frame, robot_mask):
        blurred = cv2.GaussianBlur(frame, (2 * self.blurSize + 1, 2 * self.blurSize + 1), 0)
        grey = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)

        mask = cv2.adaptiveThreshold(grey, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY,
                                     1 + 2 * self.tresholdSize, self.treshold)
        if self.erodes > 0:
            # kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (1 + 2 * erodes, 1 + 2 * erodes))
            mask = cv2.dilate(mask, None, iterations=self.erodes)
            mask = cv2.erode(mask, None, iterations=self.erodes)

        blurred = cv2.GaussianBlur(mask, (7, 7), 0)
        mask = (blurred > 100).astype(np.uint8) * 255

        limit_mask = np.zeros(mask.shape, mask.dtype)
        h, w = mask.shape
        limits = np.asarray([(a if a >= 0 else w + a, b if b >= 0 else h + b) for b, a in self.limits], np.int32)
        cv2.fillPoly(limit_mask, [limits], 1)

        frame_hsv = cv2.cvtColor(cv2.GaussianBlur(frame, (5, 5), 0), cv2.COLOR_BGR2HSV)
        colour_mask = cv2.inRange(frame_hsv, (0, 0, 0), (255, 80, 255)) // 255
        colour_mask = cv2.erode(colour_mask, None, iterations=30)
        self.mask_remaining = limit_mask * colour_mask * robot_mask
        mask = ((255 - mask) * self.mask_remaining).astype(np.float32)

        if self.mine_mask is None:
            self.mine_mask = mask
        else:
            self.mine_mask = self.mine_mask * self.momentum + (1 - self.momentum) * mask
        return self.mine_mask.astype(np.uint8)

    def get_mine_positions(self, frame):
        contours, _ = cv2.findContours(np.greater(self.mine_mask, 10).astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        mines = []
        find_mines = self.remaining_mines
        for cnt in contours:
            x, y, w, h = cv2.boundingRect(cnt)
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            mines.append((np.sum((self.mine_mask[y:y+w, x:x+w])), (x + w//2, y + h//2)))
        if len(mines) == 0:
            mask_remaining = self.mask_remaining.copy()
            _draw_grid(mask_remaining)
            contours, _ = cv2.findContours(mask_remaining, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            mines = []
            for cnt in contours:
                x, y, w, h = cv2.boundingRect(cnt)
                cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                mines.append((np.sum((mask_remaining[y:y+w, x:x+w])), (x + w//2, y + h//2)))
            find_mines = 1
        mines.sort(reverse=True)
        self.mine_positions =[pos for _, pos in mines[0:find_mines]]
        for x, y in self.mine_positions:
            cv2.circle(frame, (x, y), 3, (255, 255, 0), -1)
        return frame
