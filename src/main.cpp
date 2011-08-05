#include <ObjectsFinder.h>
#include <cstdlib>
#include <cv.h>
#include <highgui.h>

using namespace std;

int main(int argc, char** argv) {
    cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Contours", CV_WINDOW_AUTOSIZE);

    ObjectsFinder finder(0, 1, 42);

    while (true) {
        finder.refresh();
        IntPoint objcenter = finder.getTargetCenter(); 
        printf("Center coordinates: (%d; %d)\n", objcenter.x, objcenter.y);
        if (cvWaitKey(1) == 27)
            break;
    }

    cvDestroyWindow("Original");
    cvDestroyWindow("Contours");

    return 0;
}
