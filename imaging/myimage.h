#ifndef MYIMAGE_H
#define MYIMAGE_H

#include <iostream>
#include <math.h>
#include <QFile>
#include <QImage>
#include <QString>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "myfilter.h"

class MyImage
{
private:

    // THIS IS SLOPPY!

public:

    QString qTitle;
    cv::Mat image;

    double intensityMin;
    double intensityMax;

    static const int intensityColorMap = CV_8UC1;
    static const uint16_t numberBins = 256;
    static const uint8_t maxBin = 255;

    MyImage(QString input);
    ~MyImage();

    void setImage(QString filePath);
    void setImage(QString filePath, int intensityValue);
    void setImage(MyImage inputImage);
    void setImage(MyImage inputImage, int intensityValue);

    uint8_t getIntensity(uint32_t row, uint32_t col);

    QVector<double> intensityBins;
    QVector<double> intensityDistribution;

    void setIntensityHistograms();
    void buildIntensityBins();
    void buildIntensityDistribution();

    QImage getQImage();
    void saveImageToPNG(QString outputPath);

};

#endif // MYIMAGE_H
