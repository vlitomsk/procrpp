#include <stdlib.h>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <libfreenect_sync.h>
#include <libfreenect.h>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#define u8 unsigned char
#define u32 unsigned int

using namespace std;

int width = 618, height = 476;

u8* getsetaddr(IplImage* img, int x, int y) {
    return &(((u8*)(img->imageData + y * img->widthStep))[x]);
}

void setarr(IplImage* img, int x, int y, u8 val) {
    (*getsetaddr(img, x, y)) = val;
}

int getarr(IplImage* img, int x, int y) {
    return *getsetaddr(img, x, y);
}

double average(IplImage* arr, int x0, int y0, int w, int h) {
    double s;
    int sz = w * h;
    for (int i = x0; i < x0 + w; ++i) {
        for (int j = y0; j < y0 + h; ++j)
            if ((getarr(arr, i, j) == 0xff) || (getarr(arr, i, j) == 0)) {
                sz--;
            } else {
                s += getarr(arr, i, j);
            }
    }

    return s / sz;
}

const int dxmin = 0, dxmax = 5;
double get_line_average(IplImage* arr, int y0, int lineheight) {
    return average(arr, 200, y0, 200, lineheight);
}

const int ddmin = 0, ddmax = 2; 
const int dy0min = 0, dy0max = 2;
const int minplanesize = 5; // пикселей

template <typename T>
bool in_bounds(T x, T min, T max) {
    if (x >= min && x <= max)
        return true;

    return false;
}

#define dddnormal(ideal, cur) in_bounds(abs((ideal) - (cur)), ddmin, ddmax)
#define dy0normal(ideal, cur) in_bounds(abs((ideal) - (cur)), dy0min, dy0max)

inline int get_y0(IplImage* img, int x) {
    return getarr(img, x, height - 1);
}
void make_blackbottom(IplImage* img, int x, int h) {

    while (h >= 0) {
        setarr(img, x, height - 1 - h, 0);
        --h;
    }
}

struct otrez {
    int start, finish;
};

vector<otrez> draw_deltas(IplImage* img) {
    int last_y0; // последнее значение нижнего пикселя
    int last_dd; // последнее значение крутизны

    vector<otrez> res;

    int last_x1 = -1;
    int last_x2 = -1;

    bool has_seed = false; // выбраны ли начальные last_y0 и last_dd
    for (int i = 0; i < width; ++i) {
        int planesize = 0; // высота "белого" пространства снизу

        int dd = getarr(img, i, height - 1) - getarr(img, i, height - 2); // начальная крутизна
        if (i == 0) { 
            last_dd = dd;
            last_y0 = get_y0(img, 0);
        }

        double averdd = 0;
        int nonwhite = 0;
        int white = 0; 
        for (int j = height - 1; j >= 1; --j) {            
            int pixa = getarr(img, i, j), 
                pixb = getarr(img, i, j - 1);


            if (pixa == 0xff || pixb == 0xff) { 
                white++;
                planesize++;
                continue;
            }

            averdd += abs(pixa - pixb);
            
            if (!dddnormal(dd, pixa - pixb)) {
                //printf("a{%d}", planesize);
                break;
            } else {
                planesize++;
                if (white != 0) {
                    if (white > 20)
                        break;
                }
                nonwhite++;
            }
        }
        averdd /= nonwhite;             
        //printf("%f\n", averdd);

        int magic =0;

        if (planesize >= minplanesize && dddnormal(dd, last_dd) && i != 0 && dy0normal(last_y0, get_y0(img, i)) && averdd >= 0.05) {
            last_y0 = get_y0(img, i);
            make_blackbottom(img, i, nonwhite);
            if (last_x1 < 0) 
                last_x1 = last_x2 = i;
            else 
                last_x2++;
            
        } else {
            for (int j = height - 1; j >= 1; --j)
                if ((last_y0 = get_y0(img, i)) != 0xff)
                    break;

            if (last_x1 != -1) {
                otrez otr = {last_x1, last_x2};
                res.push_back(otr);
                last_x1 = -1;
            }
        }
    }

    return res;
}

void imgdata_to_array(IplImage* img, u8** arr) {
    *arr = (u8*)malloc(sizeof(u8) * img->imageSize);
    
    printf("size = %d\n", img->imageSize);
    for (int i = 0; i < img->height; ++i) {
        for (int j = 0; j < img->width; ++j) {
            (*arr)[i*width + j] = getarr(img, j, i);
        }
    }
}   

void draw_graph(IplImage* data, int x) {
    IplImage* img = cvCreateImage(cvSize(height, 512), 8, 3);

    for (int i = height; i > 0; --i)  
        cvLine(img, cvPoint(height - i, (255 - getarr(data, x, i)) * 2), cvPoint(height - i + 1, (255 - getarr(data, x, i - 1)) * 2), cvScalar(0, 0, 255), 2, CV_AA, 0);    

    cvShowImage("Depth(height) graph", img);
    cvReleaseImage(&img);
}


#define STATIC_IMG
int main() {
    IplImage* image;
    //    reset_white_cache();

#ifdef STATIC_IMG
    IplImage* tmp = cvLoadImage("./5d.png");
    CvSize sz = cvGetSize(tmp);
    image = cvCreateImageHeader(sz, 8, 1);
    /*    height = sz.height;
          width = sz.width;*/
#endif

#ifndef STATIC_IMG
    image = cvCreateImageHeader(cvSize(640, 480), 8, 1);
    u8* data = (u8*) malloc(sizeof (char) * 640 * 480);
    IplImage* wcam = cvCreateImageHeader(cvSize(640, 480), 8, 1);
    memset(data, 0xff, 640 * 480);
    freenect_sync_set_tilt_degs(0, 0);
    for (int counter = 0;; counter++) {
        short *data_;
        char* vidata_;
        unsigned int timestamp;
        freenect_sync_get_depth((void**) (&data_), &timestamp, 0, FREENECT_DEPTH_11BIT);
        freenect_sync_get_video((void**)(&vidata_), &timestamp, 0, FREENECT_VIDEO_BAYER);
        for (int i = 0; i < 640 * 480; ++i) {
            //            if (data[i] == 0xff)
            data[i] = data_[i] >> 3;
        }

        cvSetData(wcam, data_, 640);

        cvSetData(image, data, 640);
        draw_graph(image, 610);
        //draw_graph(image, 200);
        draw_deltas(image);}
        cvShowImage("KinectFuck2", image);

        if (counter > 30) {
            //            draw_deltas(data);
        }

        if (cvWaitKey(10) == 27)
            break;

        usleep(10000);
}
free(data);
freenect_sync_stop();
cvFree(&image);
#endif

#ifdef STATIC_IMG

IplImage* tmp2 = cvCreateImage(cvGetSize(tmp), IPL_DEPTH_8U, 1);
cvCvtColor(tmp, tmp2, CV_BGR2GRAY);
printf("Size: (%d; %d)\n", tmp2->width, tmp2->height);
draw_graph(tmp2, 200);

u8* data;
imgdata_to_array(tmp2, &data);
printf("here\n");

cvSetData(image, data, width);
vector<otrez> otr = draw_deltas(image);
cout << "Отрезочки:\n";
for (int i = 0; i < otr.size(); ++i) {
    otrez tmp_struct = otr.at(i);
    cout << "(" << tmp_struct.start << "; " << tmp_struct.finish << ")\n";
}
/*    for (int i = 0; i < 100; ++i)
      for (int j = 0; j < 100; ++j)
      setarr(image, i, j, 0);*/
cvShowImage("Depth", image);
#endif

while (cvWaitKey(10) != 27) {}
#ifdef STATIC_IMG
cvReleaseImage(&tmp);
cvReleaseImage(&tmp2);
#endif
//free(data);

return 0;
}
