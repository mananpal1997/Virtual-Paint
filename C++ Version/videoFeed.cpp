#include "videoFeed.h"
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

VideoFeed::VideoFeed(QWidget* parent): QWidget(parent)
{
	this->image = QImage();
	this->setGeometry(300, 300, 650, 480);
}

void VideoFeed::setImage(QImage i)
{
	image = i;
	this->repaint();
}