import cv2, sys, os
import numpy as np
from PyQt4 import QtGui, QtCore
from math import pow, sqrt

blue = cv2.cv.CV_RGB(0,0,255)
green = cv2.cv.CV_RGB(0,255,0)
red = cv2.cv.CV_RGB(255,0,0)
white = cv2.cv.CV_RGB(255,255,255)
black = cv2.cv.CV_RGB(0,0,0)

def get_threshold_img(img, lower_bound, upper_bound):
    image_hsv = cv2.cv.CreateImage(cv2.cv.GetSize(img), 8, 3)
    cv2.cv.CvtColor(img, image_hsv, cv2.cv.CV_BGR2HSV)
    threshold_image = cv2.cv.CreateImage(cv2.cv.GetSize(img), 8, 1)
    cv2.cv.InRangeS(image_hsv, lower_bound, upper_bound, threshold_image)

    return threshold_image

def clear_screen(scribble, drawing):
    cv2.cv.Set(scribble, black)
    cv2.cv.Set(drawing, white)


class ShowVideo(QtCore.QObject):
    lower_bound = cv2.cv.Scalar(20,100,100)
    upper_bound = cv2.cv.Scalar(40,255,255)

    line_thickness = 2
    area_limit = 700.00

    line_color = blue
    
    image_panel = cv2.cv.LoadImage('../resources/paint.png', cv2.cv.CV_LOAD_IMAGE_COLOR)

    edit_mode = False

    drawing = cv2.cv.CreateImage((640, 480), 8L, 3)
    cv2.cv.Set(drawing, white)

    font = cv2.cv.InitFont(cv2.cv.CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 3, 8)

    confirm_close = 20
    confirm_clear = 20

    x_pos, y_pos = 0, 0

    scribble = cv2.cv.CreateImage((640, 480), 8L, 3)

    videoSignal1 = QtCore.pyqtSignal(QtGui.QImage)
    videoSignal2 = QtCore.pyqtSignal(QtGui.QImage)
    videoSignal3 = QtCore.pyqtSignal()

    run_video = True

    def __init__(self, parent = None):
        super(ShowVideo, self).__init__(parent)

    @QtCore.pyqtSlot()
    def startVideo(self):

        self.run_video = True

        cv2.cv.Set(self.drawing, white)

        if self.edit_mode == True:
            file_name = QtGui.QFileDialog.getOpenFileName(None, 'Save Image', './paint.png', selectedFilter = '*jpg')
            file_name = str(file_name)
            self.edit_image = cv2.cv.LoadImage(file_name, cv2.cv.CV_LOAD_IMAGE_COLOR)
            self.drawing = self.edit_image

        self.capture = cv2.cv.CaptureFromCAM(0)

        while self.run_video:
            frame = cv2.cv.QueryFrame(self.capture)

            if frame is None:
                break

            cv2.cv.Flip(frame, None, 1)

            cv2.cv.Smooth(frame, frame, cv2.cv.CV_MEDIAN, 5, 5)
            
            threshold_image = get_threshold_img(frame, self.lower_bound, self.upper_bound)
            matrix = cv2.cv.GetMat(threshold_image)
            moments = cv2.cv.Moments(matrix)
            
            moment10 = cv2.cv.GetSpatialMoment(moments, 1, 0)
            moment01 = cv2.cv.GetSpatialMoment(moments, 0, 1)

            area = cv2.cv.GetCentralMoment(moments, 0, 0)
            if area == 0:
                area = 0.000000000000000001
            
            last_x, last_y = int(self.x_pos), int(self.y_pos)
            self.x_pos, self.y_pos = 0, 0

            if moment10/area >= 0 and moment10/area < 1280 and moment01/area >= 0 and moment01/area < 1280 and area > self.area_limit:
                self.x_pos = moment10//area
                self.y_pos = moment01//area

            if self.x_pos < 90 and self.y_pos > 400:
                self.line_color = white
                cv2.cv.PutText(frame, "Eraser selected", (150, 30), self.font, white)
                stream = "Clearing the screen in %d" % self.confirm_clear
                cv2.cv.PutText(frame, stream, (150, 70), self.font, red)
                
                self.confirm_clear -= 1
                if self.confirm_clear < 0:
                    self.confirm_clear = 20
                    clear_screen(self.scribble, self.drawing)
                    cv2.cv.PutText(frame, "Cleared the screen", (150, 110), self.font, white)

            elif self.x_pos > 540 and self.y_pos > 360:
                self.line_color = blue
                cv2.cv.PutText(frame, "Blue color selected", (150, 30), self.font, blue)

            elif self.x_pos > 540 and self.y_pos > 200 and self.y_pos < 280:
                self.line_color = green
                cv2.cv.PutText(frame, "Green color selected", (150, 30), self.font, green)

            elif self.x_pos > 540 and self.y_pos < 120:
                self.line_color = red
                cv2.cv.PutText(frame, "Red color selected", (150, 30), self.font, red)

            elif self.x_pos > 0 and self.x_pos < 90 and self.y_pos > 0 and self.y_pos < 120:
                stream = "Exiting in %d" % self.confirm_close
                cv2.cv.PutText(frame, stream, (150, 30), self.font, red)
                self.confirm_close -= 1
                if self.confirm_close < 0:
                    self.videoSignal3.emit()
                    qt_image = QtGui.QImage()
                    self.videoSignal1.emit(qt_image)
                    cv2.cv.DestroyAllWindows()
                    try:
                        del self.capture
                    except:
                        pass
                    self.run_video = False
                    continue

            elif self.x_pos < 90 and self.y_pos > 130 and self.y_pos < 390:
                self.line_thickness = 6 - (self.y_pos//60 - 1)

            stream = "%d" % self.line_thickness
            cv2.cv.PutText(frame, stream, (40, 255), self.font, self.line_color)

            x_diff = last_x - self.x_pos
            y_diff = last_y - self.y_pos

            magnitude = sqrt(pow(x_diff, 2) + pow(y_diff, 2))

            if magnitude > 0 and magnitude < 100 and self.x_pos > 120 and self.x_pos < 530:
                if self.line_color == white:
                    cv2.cv.Line(self.drawing, (int(self.x_pos), int(self.y_pos)), (int(last_x), int(last_y)), self.line_color, int(self.line_thickness)+6, cv2.cv.CV_AA)
                else:
                    cv2.cv.Line(self.drawing, (int(self.x_pos), int(self.y_pos)), (int(last_x), int(last_y)), self.line_color, int(self.line_thickness), cv2.cv.CV_AA)

            cv2.cv.Add(self.drawing, self.scribble, self.drawing)

            cv2.cv.And(frame, self.drawing, frame)
            cv2.cv.And(self.image_panel, frame, frame)
            if self.edit_mode == True:
                cv2.cv.And(self.edit_image, frame, frame)

            if self.drawing != None:
                self.drawing_matrix = cv2.cv.GetMat(self.drawing)
                temp_image = np.asarray(self.drawing_matrix[:,:])
                temp_image = cv2.cvtColor(temp_image, cv2.COLOR_BGR2RGB)
                height, width, byteValue = temp_image.shape
                byteValue *= width
                qt_image = QtGui.QImage(temp_image, width, height, byteValue, QtGui.QImage.Format_RGB888)
                self.videoSignal2.emit(qt_image)

                self.frame_matrix = cv2.cv.GetMat(frame)
                temp_image = np.asarray(self.frame_matrix[:,:])
                temp_image = cv2.cvtColor(temp_image, cv2.COLOR_BGR2RGB)
                height, width, byteValue = temp_image.shape
                byteValue *= width
                qt_image = QtGui.QImage(temp_image, width, height, byteValue, QtGui.QImage.Format_RGB888)
                self.videoSignal1.emit(qt_image)

            #cv2.cv.MoveWindow("Virtual Window", 0, 150)
            #cv2.cv.MoveWindow("Painter", 650, 150)

            c = cv2.cv.WaitKey(10)
            if c == 27:
                break

        cv2.cv.DestroyAllWindows()

    @QtCore.pyqtSlot()
    def stopVideo(self):
        qt_image = QtGui.QImage()
        self.videoSignal1.emit(qt_image)
        cv2.cv.DestroyAllWindows()
        try:
            del self.capture
        except:
            pass
        self.run_video = False

    def saveImage(self):
        file_name = QtGui.QFileDialog.getSaveFileName(None, 'Save Image', '/home/ghostman/Pictures/', selectedFilter = '*jpg')
        cv2.cv.SaveImage(str(file_name)+".jpg", self.drawing)
        self.videoSignal2.emit(QtGui.QImage())
        self.stopVideo()

class VideoFeed(QtGui.QWidget):
    def __init__(self, parent = None):
        super(VideoFeed, self).__init__(parent)
        self.image = QtGui.QImage()
        self.initUI()

    def paintEvent(self, _):
        painter = QtGui.QPainter(self)
        painter.drawImage(0,0, self.image)

    def initUI(self):
        self.setGeometry(300, 300, 650, 480)

    @QtCore.pyqtSlot(QtGui.QImage)
    def setImage(self, image):
        self.image = image
        self.repaint()

class PaintPanel(QtGui.QWidget):
    def __init__(self, parent = None):
        super(PaintPanel, self).__init__(parent)
        self.image = QtGui.QImage()
        self.initUI()

    def paintEvent(self, _):
        painter = QtGui.QPainter(self)
        painter.drawImage(0,0, self.image)

    def initUI(self):
        self.setGeometry(300, 300, 650, 480)

    @QtCore.pyqtSlot(QtGui.QImage)
    def setImage(self, image):
        self.image = image
        self.repaint()


class Main(QtGui.QMainWindow):
    def __init__(self, parent = None):
        super(Main, self).__init__(parent)

        self.video_feed = VideoFeed()
        self.paint_panel = PaintPanel()
        self.vid = ShowVideo()

        self.vid.videoSignal1.connect(self.video_feed.setImage)
        self.vid.videoSignal2.connect(self.paint_panel.setImage)
        self.vid.videoSignal3.connect(self.stop)

        self.start_button = QtGui.QPushButton("Start")
        self.start_button.setFixedHeight(30)
        self.start_button.setFixedWidth(100)
        self.start_button.clicked.connect(self.start)

        self.stop_button = QtGui.QPushButton("Stop")
        self.stop_button.setFixedHeight(30)
        self.stop_button.setFixedWidth(100)
        self.stop_button.setEnabled(False)
        self.stop_button.clicked.connect(self.stop)

        self.edit_button = QtGui.QPushButton("Edit")
        self.edit_button.setFixedHeight(30)
        self.edit_button.setFixedWidth(100)
        self.edit_button.clicked.connect(self.edit)

        self.save_button = QtGui.QPushButton("Save")
        self.save_button.setFixedHeight(30)
        self.save_button.setFixedWidth(100)
        self.save_button.clicked.connect(self.save)

        self.view_layout = QtGui.QGridLayout()
        
        self.button_layout = QtGui.QGridLayout()
        self.button_layout.setSpacing(300)
        
        self.main_layout = QtGui.QGridLayout()

        self.view_layout.addWidget(self.video_feed, 0 , 0)
        self.view_layout.addWidget(self.paint_panel, 0 , 1)

        self.button_layout.addWidget(self.start_button, 0, 0)
        self.button_layout.addWidget(self.stop_button, 0, 1)
        self.button_layout.addWidget(self.save_button, 0, 2)
        self.button_layout.addWidget(self.edit_button, 0, 3)

        self.main_layout.addItem(self.view_layout)
        self.main_layout.addItem(self.button_layout)

        self.layout_widget = QtGui.QWidget()
        self.layout_widget.setLayout(self.main_layout)

        self.setCentralWidget(self.layout_widget)

    def closeEvent(self, event):
        self.vid.stopVideo()

    def save(self):
        self.setUpdatesEnabled(False)
        self.vid.saveImage()

    def start(self):
        self.save_button.setEnabled(False)
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(True)
        self.vid.startVideo()

    def stop(self):
        self.stop_button.setEnabled(False)
        self.save_button.setEnabled(True)
        self.start_button.setEnabled(True)
        self.vid.stopVideo()

    def edit(self):
        if(self.vid.edit_mode == False):
            self.vid.edit_mode = True
        else:
            self.vid.edit_mode = False
        self.start()
    
if __name__ == '__main__':

    app = QtGui.QApplication(sys.argv)

    main_window = Main()
    main_window.resize(1300, 540)
    main_window.show()

    sys.exit(app.exec_())
