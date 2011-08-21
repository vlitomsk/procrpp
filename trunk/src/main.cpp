#include "ObjectsFinder.h"
#include <cstdlib>
#include <cv.h>
#include <highgui.h>
#include <unistd.h>

#define DEFAULT_CAMERA_ID 0

int main(int argc, char** argv) {
	int CAMERAID = 1;
	if (argc < 5) {
		printf("Usage: %s <TargetRed> <TargetGreen> <TargetBlue> <TargetMaxColorDelta> [CameraId]\n", argv[0]);
		return 42;
	}
	int tred, tgreen, tblue, tdelta;
	tred = atoi(argv[1]);
	tgreen = atoi(argv[2]);
	tblue = atoi(argv[3]);
	tdelta = atoi(argv[4]);
	if (argc >= 6)
		CAMERAID = atoi(argv[5]);

    cvNamedWindow("Original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Contours", CV_WINDOW_AUTOSIZE);

    ObjectsFinder finder(CAMERAID, 0.338, 25.012);
    finder.calibrate(tred, tgreen, tblue, tdelta);

    IntPoint objcenter;

    while (true) {
        finder.refresh();
        //objcenter = finder.getTargetCenter();
        double distance = finder.getTargetDistance();
        printf("Distance, m: %f\n", distance);
        if (cvWaitKey(1) == 27)
            break;
    }
    finder.~ObjectsFinder();

    cvDestroyWindow("Original");
    cvDestroyWindow("Contours");

    return 0;
}
