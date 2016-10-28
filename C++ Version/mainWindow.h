#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QMainWindow>

#include "paintPanel.h"
#include "videoFeed.h"
#include "showVideo.h"

#pragma once

class MainWindow:public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QMainWindow *parent=0);

private:
	VideoFeed *video_feed;
	PaintPanel *paint_panel;
	ShowVideo *vid;
	QPushButton *start_button;
	QPushButton *stop_button;
	QPushButton *edit_button;
	QPushButton *save_button;
	QHBoxLayout *view_layout;
	QHBoxLayout *button_layout;
	QVBoxLayout *main_layout;
	QWidget *main_widget;

	void closeEvent(QCloseEvent *event)
	{
		this->vid->stopVideo();
	}

private slots:
    void start();
    void stop();
    void save();
    void edit();
};