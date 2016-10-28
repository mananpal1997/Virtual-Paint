#include "mainWindow.h"
#include <iostream>

MainWindow::MainWindow(QMainWindow* parent): QMainWindow(parent)
{
	video_feed = new VideoFeed();
	paint_panel = new PaintPanel();
	vid = new ShowVideo();

	connect(vid, SIGNAL(sendFeed(QImage)), video_feed, SLOT(setImage(QImage)));
	connect(vid, SIGNAL(sendImage(QImage)), paint_panel, SLOT(setImage(QImage)));
	connect(vid, SIGNAL(close()), this, SLOT(stop()));

	start_button = new QPushButton("Start",this);
	start_button->setFixedHeight(30);
	start_button->setFixedWidth(100);
	connect(start_button, SIGNAL(clicked()), this, SLOT(start()));

	stop_button = new QPushButton("Stop",this);
	stop_button->setFixedHeight(30);
	stop_button->setFixedWidth(100);
	stop_button->setEnabled(false);
	connect(stop_button, SIGNAL(clicked()), this, SLOT(stop()));
	

	edit_button = new QPushButton("Edit", this);
	edit_button->setFixedHeight(30);
	edit_button->setFixedWidth(100);
	connect(edit_button, SIGNAL(clicked()), this, SLOT(edit()));

	save_button = new QPushButton("Save",this);
	save_button->setFixedHeight(30);
	save_button->setFixedWidth(100);
	save_button->setEnabled(false);
	connect(save_button, SIGNAL(clicked()), this, SLOT(save()));

	button_layout = new QHBoxLayout();
	// this->buttonLayout = new QGridLayout();
	//The above shit works
	button_layout->setSpacing(300);
	button_layout->addWidget(start_button);
	button_layout->addWidget(stop_button);
	button_layout->addWidget(edit_button);
	button_layout->addWidget(save_button);

	view_layout = new QHBoxLayout();
	view_layout->addWidget(video_feed);
	view_layout->addWidget(paint_panel);

	main_layout = new QVBoxLayout(this);
	main_layout->addLayout(view_layout);
	main_layout->addLayout(button_layout);

	main_widget = new QWidget(this);
	main_widget->setLayout(main_layout);

	this->setCentralWidget(main_widget);
	
}

void MainWindow::start()
{
	this->start_button->setEnabled(false);
	this->save_button->setEnabled(false);
	this->edit_button->setEnabled(false);
	this->stop_button->setEnabled(true);
	this->vid->startVideo();
}

void MainWindow::stop()
{
	this->start_button->setEnabled(true);
	this->save_button->setEnabled(true);
	this->edit_button->setEnabled(true);
	this->stop_button->setEnabled(false);
	this->vid->stopVideo();
}

void MainWindow::save()
{
	this->setUpdatesEnabled(false);
	this->vid->saveImage();
}

void MainWindow::edit()
{
	if(this->vid->edit_mode == false)
		this->vid->edit_mode = true;
	start();
}