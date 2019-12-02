import cv2
from imutils.video import VideoStream
import time


class FrameLoader(object):
    def __init__(self, source=None, record=None, infinite=True):
        self.vs = None
        self.record = record
        self.fourcc = cv2.VideoWriter_fourcc(*"MJPG")
        self.writer = None
        self.infinite = infinite
        self.source = source

    def _get_frame(self):
        raise NotImplemented

    def get_frame(self):
        frame = self._get_frame()
        if frame is None:
            return None
        if self.record is not None:
            if self.writer is None:
                # store the image dimensions, initialize the video writer,
                # and construct the zeros array
                self.dims = frame.shape[:2]
                self.writer = cv2.VideoWriter(self.record, self.fourcc, 64,
                                              self.dims, True)
            self.writer.write(frame)
        return frame

    def get_frame_cropped(self):
        return self.get_frame()[:, 0:-70]

    def __del__(self):
        if self.writer is not None:
            self.writer.release()


class VideoFrameLoader(FrameLoader):
    def __init__(self, source=None, record=None, infinite=True):
        super().__init__(source, record, infinite)
        self.vs = cv2.VideoCapture(source)
        print("Opened video file")
        time.sleep(2.)

    def _get_frame(self):
        for i in range(10):
            frame = self.vs.read()
        if frame is None:
            if not self.infinite:
                return None
            self.vs.release()
            self.vs = cv2.VideoCapture(self.source)
        frame = frame[1]
        return frame

    def __del__(self):
        self.vs.release()
        super().__del__()


class CameraFrameLoader(FrameLoader):
    def __init__(self, source=None, record=None, infinite=True):
        super().__init__(source, record, infinite)
        self.vs = VideoStream(src=source, resolution=(1080, 1920)).start()
        self.vs.stream.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.15)
        # self.vs.stream.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.04)

        print("Opened camera")
        time.sleep(2.)

    def _get_frame(self):
        frame = self.vs.read()
        return frame

    def __del__(self):
        self.vs.stop()
        super().__del__()

