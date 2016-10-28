#include <QWidget>
#include <QPainter>
#include <QImage>

#pragma once

class PaintPanel:public QWidget {
	Q_OBJECT
public:
	PaintPanel(QWidget *parent=0);

	QImage image;

	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.drawImage(0, 0, image);
	}

public slots:
    void setImage(QImage);    
};