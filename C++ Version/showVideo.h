#include <QObject>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QImage>
#include <QString>
#include <QFileDialog>
#include <QDir>

#pragma once

#define blue CV_RGB(0, 0, 255)
#define green CV_RGB(0, 255, 0)
#define red CV_RGB(255, 0, 0)
#define white CV_RGB(255, 255, 255)
#define black CV_RGB(0, 0, 0)

class ShowVideo:public QObject {
	Q_OBJECT
public:
	ShowVideo(QObject *parent = 0);

	CvScalar lower_bound, upper_bound, line_color;
	int line_thickness, confirm_close, confirm_clear, x_pos, y_pos;
	double area_limit;
	IplImage *image_panel, *drawing, *scribble, *frame, *edit_image;
	CvFont font, big_font;
	CvCapture *capture;
	bool run_video;
	bool edit_mode;
	QString file_name;
	char msg[50];

	IplImage *get_threshold_image(IplImage *img, CvScalar &lower_bound, CvScalar &upper_bound)
	{
	    IplImage *image_hsv = cvCreateImage(cvGetSize(img), 8, 3);
	    cvCvtColor(img, image_hsv, CV_BGR2HSV);
	    IplImage *threshold_image = cvCreateImage(cvGetSize(img), 8, 1);
	    cvInRangeS(image_hsv, lower_bound, upper_bound, threshold_image);

	    return threshold_image;
	}

	void clear_screen(IplImage *scribble, IplImage *drawing)
	{
	    cvSet(scribble, black);
	    cvSet(drawing, white);
	}

	bool compare(CvScalar scalar1, CvScalar scalar2)
	{
	    bool l = true;
	    for(int i = 0; i < 4; i++)
	        if(scalar1.val[i] != scalar2.val[i])
	            l = false;
	    return l;
	}

	QImage IplImagetoQImage(IplImage *image)
	{
		int height = image->height;
		int width = image->width;
		
		const uchar *qImageBuffer =(const uchar*)image->imageData;
		
		QImage result(qImageBuffer, width, height, QImage::Format_RGB888);
		
		return result.rgbSwapped();
	}

public slots:
	void startVideo();
	void stopVideo();
	void saveImage();

signals:
	void sendFeed(QImage);
	void sendImage(QImage);
	void close();
};