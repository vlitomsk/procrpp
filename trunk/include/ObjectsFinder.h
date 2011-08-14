#ifndef OBJECTS_FINDER_H
#define OBJECTS_FINDER_H

#include <cv.h>
#include <highgui.h>
#include "types.h"

struct IntPoint {
    int x, y;
};

class ObjectsFinder {
    public:
		/*
		 * Конструкторы.
		 * cam - id камеры, с которой снимать видеопоток
		 * target_realrad - радиус мишени в метрах
		 * dist_coef - Коэффициент пропорциональности = Distance/CamRadius
		 */
        ObjectsFinder(int cam, double target_realrad);
        ObjectsFinder(int cam, double realrad, double dist_coef);
        ~ObjectsFinder();

        /* Обновить данные о мишени и луче */
        void refresh();

        /* Установить коэффициент пропорциональности расстояния радиусу на камере */
        void setTargetDistanceCoef(double c);

        /* Получить  расстояние до мишени в метрах*/
        double getTargetDistance();

        /* Получить радиус мишени, кажущийся камере, пикс */
        int getTargetCamRadius();

        /* Получить координаты центра мишени в камере, пикс */
        IntPoint getTargetCenter();

        /* Получить координаты луча в камере, пикс */
        IntPoint getLuchCenter();

        /* Калибровать камеру на поиск мишени и заданного цвета с заданным +-ом для него */
        void calibrate(int targetr, int targetg, int targetb, int maxdelta);

    private:
        static const double tDefaultDistCoeff = 42; // дефолтный tDistanceCoeff
        double tDistanceCoeff; // отношение реальных размеров к размерам на камерe    
        static const int g_thresh = 100; // порог обнаружения контуров
        static const int BlurSize = 51; // Размах Гауссова размытия в пикс

        struct __classflags {
        	u8 calibrate : 1;
        } classflags;
        int cam; // id камеры, с которой снимается видеопоток.
        double t_rrad; // реальный радиус мишени в метрах
        int t_crad; // радиус на камере(берется большая полуось)
        int t_centx, centy; // координаты центра цели на камере
        double dist; // дистанция до мишени в метрах
        CvCapture* capture; // капчур с камеры.
        struct RGBDrange {
        	int r, g, b;
        	int maxdelta;
        } targetRange;

        IplImage *img, *gray;
        CvMemStorage* g_storage;
        CvSeq* contours;

        void init(int cam, double realrad, double dist_coef);
};

#endif
