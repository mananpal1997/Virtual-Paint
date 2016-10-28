#include "paintPanel.h"

PaintPanel::PaintPanel(QWidget* parent): QWidget(parent)
{
	this->image = QImage();
	this->setGeometry(300, 300, 650, 480);
}

void PaintPanel::setImage(QImage i)
{
	image = i;
	this->repaint();
}