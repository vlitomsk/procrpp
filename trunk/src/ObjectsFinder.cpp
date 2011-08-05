#include <ObjectsFinder.h>
#include <cstdlib>
#include <Errors.h>

void ObjectsFinder::init(int cam, double realrad, double dist_coef) {
    this->cam = cam;
    this->t_rrad = realrad;
    capture = NULL;
    capture = cvCreateCameraCapture(cam);
    tDistanceCoeff = dist_coef;
    if (!capture) {
    	errorExit(Errors::ErrorCamNF);
    }

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
    IplImage* g_image = NULL;
    IplImage* g_gray = NULL;
    CvMemStorage* g_storage = NULL;    
    CvSeq* contours = NULL;

    g_image = cvQueryFrame(capture);    
    cvSmooth(g_image, g_image, CV_GAUSSIAN, GaussBlurSize, GaussBlurSize);
    if(!g_storage) {
        g_gray = cvCreateImage( cvGetSize( g_image ), IPL_DEPTH_8U, 1 );
        g_storage = cvCreateMemStorage(0);
    } else 
        cvClearMemStorage( g_storage );
     
    cvCvtColor( g_image, g_gray, CV_BGR2GRAY );
    cvThreshold( g_gray, g_gray, g_thresh, 255, CV_THRESH_BINARY );
    cvFindContours( g_gray, g_storage, &contours );
    cvZero( g_gray );
    CvSeq* c = NULL;

    for (c = contours; c != 0; c = c->h_next) {

        if (c->total >= 100) {
            CvBox2D ellipse = cvFitEllipse2(c);
            t_crad = (int)max(ellipse.size.width, ellipse.size.height);        
            t_centx = (int)ellipse.center.x;
            centy = (int)ellipse.center.y;
            //printf("Center: (%d; %d)\n", (int)ellipse.center.x, (int)ellipse.center.y);
        }
    }
    printf("\n");
    if( contours ){
        cvDrawContours(
                g_gray,
                contours,
                cvScalarAll(255),
                cvScalarAll(255),
                100 );
    }

    cvShowImage( "Original", g_image );
    cvShowImage("Contours", g_gray);

    cvClearMemStorage(g_storage);

    // Из-за отсутствия этих строк сильно какается память, но с ними происходит Segfault.
    /*cvReleaseImage(&g_image);
    cvReleaseImage(&g_gray);*/
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

