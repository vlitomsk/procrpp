#include "ObjectsFinder.h"
#include <cstdlib>
#include <exception>
#include "Errors.h"
#include <iostream>
using namespace std;

#define Hate

void ObjectsFinder::init(int cam, double realrad, double dist_coef) {
    this->cam = cam;
    this->t_rrad = realrad;
    capture = NULL;
    capture = cvCreateCameraCapture(cam);
    tDistanceCoeff = dist_coef;
    if (!capture) {
    	Errors::errorExit(Errors::ErrorCamNF);
    }
    classflags.calibrate = 0;

    img = NULL;
    gray = NULL;
    g_storage = NULL;
    contours = NULL;

    refresh();
}

ObjectsFinder::ObjectsFinder(int cam, double realrad) {
    init(cam, realrad, tDefaultDistCoeff);
}

ObjectsFinder::ObjectsFinder(int cam, double realrad, double dist_coef) {
    init(cam, realrad, dist_coef);
}

ObjectsFinder::~ObjectsFinder() {
	cvReleaseCapture(&capture);
}

#define max(a, b) ((a) >= (b) ? (a) : (b))

void ObjectsFinder::refresh() {
	/** ПОПРОБУЙ ТОЛЬКО ЗАРЕЛИЗИТЬ img!!!1!!11 **/
    img = cvQueryFrame(capture);

    //cvSmooth(img, img, CV_GAUSSIAN, BlurSize, BlurSize);

    gray = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );

    if(!g_storage)
        g_storage = cvCreateMemStorage(0);
     else
        cvClearMemStorage( g_storage );  

    if (classflags.calibrate) {
    	IplImage* chanr = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    	IplImage* chang = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    	IplImage* chanb = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
    	cvSplit(img, chanb, chang, chanr, NULL);

    	Hate {
			cvInRangeS(chanr, cvScalarAll(targetRange.r - targetRange.maxdelta),
					cvScalarAll(targetRange.r + targetRange.maxdelta), chanr);
			cvInRangeS(chang, cvScalarAll(targetRange.g - targetRange.maxdelta),
							cvScalarAll(targetRange.g + targetRange.maxdelta), chang);
			cvInRangeS(chanb, cvScalarAll(targetRange.b - targetRange.maxdelta),
							cvScalarAll(targetRange.b + targetRange.maxdelta), chanb);
			cvAnd(chanr, chang, gray);
			cvAnd(gray, chanb, gray);
    	};

    	cvReleaseImage(&chanr);
    	cvReleaseImage(&chang);
    	cvReleaseImage(&chanb);
    } else
    	cvCvtColor(img, gray, CV_BGR2GRAY);

    cvThreshold( gray, gray, 200, 255, CV_THRESH_BINARY );

    cvFindContours( gray, g_storage, &contours );
    cvZero( gray );
    CvSeq* c = NULL;

    for (c = contours; c != 0; c = c->h_next) {

        if (c->total >= 50) {
            CvBox2D ellipse = cvFitEllipse2(c);
            t_crad = (int)max(ellipse.size.width, ellipse.size.height);        
            t_centx = (int)ellipse.center.x;
            centy = (int)ellipse.center.y;

        }
    }    
    if( contours ){
        cvDrawContours(
        		gray,
                contours,
                cvScalarAll(255),
                cvScalarAll(255),
                100 );
    }

    cvRelease((void**)&contours);

    cvShowImage( "Original", img );
    cvShowImage("Contours", gray);
    
    cvReleaseImage(&gray);
}

double ObjectsFinder::getTargetDistance() {
    return t_crad * tDistanceCoeff;
}

int ObjectsFinder::getTargetCamRadius() {
    return t_crad;
}

void ObjectsFinder::setTargetDistanceCoef(double c) {
    tDistanceCoeff = c;
}

IntPoint ObjectsFinder::getTargetCenter() {
    IntPoint res;
    res.x = t_centx;
    res.y = centy;

    return res;
}

void ObjectsFinder::calibrate(int targetr, int targetg, int targetb, int maxdelta) {
	targetRange.r = targetr;
	targetRange.g = targetg;
	targetRange.b = targetb;
	targetRange.maxdelta = maxdelta;

	classflags.calibrate = 1;
}
