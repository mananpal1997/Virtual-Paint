#include "showVideo.h"
#include <math.h>
#include <iostream>
#include <QImage>

ShowVideo::ShowVideo(QObject *parent): QObject(parent)
{
    lower_bound = cvScalar(20, 100, 100);
    upper_bound = cvScalar(40, 225, 225);
    
    line_thickness = 2;
    area_limit = 700.00;

    image_panel = cvLoadImage("../resources/paint.png", CV_LOAD_IMAGE_COLOR);

    drawing = cvCreateImage(cvSize(640, 480), 8L, 3);

    scribble = cvCreateImage(cvSize(640, 480), 8L, 3);

    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1, .6, 0, 2, CV_AA);
    cvInitFont(&big_font, CV_FONT_HERSHEY_COMPLEX, 1, 1, 0, 3, CV_AA);

    x_pos=0, y_pos=0;

    edit_mode = false;
}

void ShowVideo::startVideo()
{
    run_video = true;

    line_color = blue;
    cvSet(drawing, white);

    capture = cvCaptureFromCAM(0);

    confirm_close = 20, confirm_clear=20;

    if(edit_mode)
    {
        QString filter = "(*jpg *png)";
        file_name = QFileDialog::getOpenFileName(NULL, "Select an image to open...", QDir::homePath(), filter);
        char *path = file_name.toLatin1().data();
        edit_image = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
        drawing = edit_image;
        edit_mode = false;
    }

    while(run_video)
    {
        frame = cvQueryFrame(capture);
        
        if(!frame)
            continue;

        cvFlip(frame, NULL, 1);
        cvSmooth(frame, frame, CV_MEDIAN, 5, 5);

        IplImage *threshold_image = get_threshold_image(frame, lower_bound, upper_bound);

        CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
        cvMoments(threshold_image, moments, 1);

        double moment10 = cvGetSpatialMoment(moments, 1, 0);
        double moment01 = cvGetSpatialMoment(moments, 0, 1);

        double area = cvGetCentralMoment(moments, 0, 0);
        if(area == 0)
            area = 0.000000000000000001;

        int last_x = x_pos, last_y = y_pos;
        x_pos = 0, y_pos = 0;

        if(moment10/area >= 0 && moment10/area < 640 && moment01/area >= 0 && moment01/area < 480 && area > area_limit)
        {
            x_pos = moment10/area;
            y_pos = moment01/area;
        }

        if(x_pos < 90 && y_pos > 400)
        {
            line_color = white;
            cvPutText(frame, "Eraser Selected", cvPoint(150, 30), &font, white);
            sprintf(msg, "Clearing the screen in %d",confirm_clear);
            cvPutText(frame, msg, cvPoint(150, 70), &font, red);

            confirm_clear -= 1;
            if(confirm_clear < 0)
            {
                confirm_clear = 20;
                clear_screen(scribble, drawing);
                cvPutText(frame, "Cleared the screen", cvPoint(150, 110), &font, white);
            }
        }

        else if(x_pos > 540 and y_pos > 360)
        {
            line_color = blue;
            cvPutText(frame, "Blue color selected", cvPoint(150, 30), &font, blue);
        }

        else if(x_pos > 540 and y_pos > 200 and y_pos < 280)
        {
            line_color = green;
            cvPutText(frame, "Green color selected", cvPoint(150, 30), &font, green);
        }

        else if(x_pos > 540 and y_pos < 120)
        {
            line_color = red;
            cvPutText(frame, "Red color selected", cvPoint(150, 30), &font, red);
        }

        else if(x_pos > 0 and x_pos < 90 and y_pos > 0 and y_pos < 120)
        {
            sprintf(msg, "Exiting in %d",confirm_close);
            cvPutText(frame, msg, cvPoint(150,30), &font, red);

            confirm_close -= 1;
            if(confirm_close < 0) {
                cvReleaseCapture(&capture);
                run_video = false;
                emit close();
                emit sendFeed(QImage());
                continue;
            }
        }

        else if(x_pos < 90 and y_pos > 130 and y_pos < 390)
        {
            line_thickness = 6 - (y_pos/60 - 1);
            if(line_thickness == 1)
                line_thickness = 2;
        }

        sprintf(msg, "%d", line_thickness);
        cvPutText(frame, msg, cvPoint(40, 255), &big_font, line_color);

        double x_diff = (double)(last_x - x_pos);
        double y_diff = (double)(last_y - y_pos);

        double magnitude = sqrt(pow(x_diff, 2) + pow(y_diff, 2));

        if(magnitude > 0 and magnitude < 100 and x_pos > 120 and x_pos < 530)
        {
            if(compare(line_color, white))
                cvLine(drawing, cvPoint(x_pos, y_pos), cvPoint(last_x, last_y), line_color, line_thickness+2, CV_AA);
            else
                cvLine(drawing, cvPoint(x_pos, y_pos), cvPoint(last_x, last_y), line_color, line_thickness, CV_AA);
        }

        // cvAdd(drawing, scribble, drawing);
        cvAnd(frame, drawing, frame);
        cvAnd(image_panel, frame, frame);

        if(compare(line_color, white))
        {
            cvRectangle(frame, cvPoint(x_pos-line_thickness*2, y_pos), cvPoint(x_pos+line_thickness*2, y_pos+4), black);
            cvRectangle(frame, cvPoint(x_pos-line_thickness*2, y_pos+4), cvPoint(x_pos+line_thickness*2, y_pos+4+line_thickness*8), black);
        }
        else
            cvCircle(frame, cvPoint(x_pos, y_pos), line_thickness+8, black);

        if(drawing)
            emit sendImage(IplImagetoQImage(drawing));

        emit sendFeed(IplImagetoQImage(frame));
        int c = cvWaitKey(10);
    }
}

void ShowVideo::stopVideo()
{
    cvReleaseCapture(&(capture));
    emit sendFeed(QImage());
    run_video = false;
}

void ShowVideo::saveImage()
{
    QString filter = "(*jpg *png)";
    file_name = QFileDialog::getSaveFileName(NULL, "Select an image to open...", QDir::homePath(), filter);
    char *path = file_name.toLatin1().data();
    cvSaveImage(path, drawing);
    emit sendImage(QImage());
    stopVideo();
}