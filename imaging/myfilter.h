#ifndef MYFILTER_H
#define MYFILTER_H

#include <iostream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class MyFilter
{
private:
    // --- Members ---
    QString filterName;
    int filterType = 0;

    unsigned int maskRows = 0;
    unsigned int maskCols = 0;
    unsigned int maskSize = 0;
    unsigned int padRows = 0;
    unsigned int padCols = 0;
    unsigned int a = 0;
    unsigned int b = 0;

    double scalarNum = 0;
    double scalarDen = 0;
    double scalarValue = 1;

    double currentMinIntensity = 255;
    double currentMaxIntensity = 0;
    double filterMaxScaling = 0;

    QVector<double> filterArray;
    QVector<double> filterArray2;
    QVector<double> subArray;

    // --- Mutators ---
    void setScalarValue();
    void setMaskSize();
    void setFilterMaxScaling();

    // --- Filtering ---
    void setMaxIntensity(cv::Mat &inputImage);
    void padImage(cv::Mat &inputImage,
                  cv::Mat &paddedImage,
                  cv::Mat &outputImage);
    void scanSubArray(unsigned int Pixelrow,
                      unsigned int pixelCol,
                      cv::Mat &paddedImage);
    void setPixelValue(unsigned int Pixelrow,
                       unsigned int pixelCol,
                       cv::Mat &paddedImage,
                       cv::Mat &outputImage,
                       double pixelValue);

    void rebinImage(cv::Mat &outputImage);

    // --- Transforms ---
    void filterSubArray();
    double TdotProduct();
    double TmedianValue();
    double TlaplacianValue();
    double Tgradient1D();
    void Tgradient2D(double &tmpX,
                     double &tmpY);
    double Tgradient2DAbsoluteValue();
    double Tgradient2DSquareRoot();

    double scaleFilteredValue(double value);

public:
    // --- Constructor / Destructor ---
    MyFilter();
    ~MyFilter();

    // --- Static Class Members---
    static QStringList filterStringList;

    // --- Accessors ---
    QString name();
    unsigned int rows();
    unsigned int cols();
    unsigned int size();
    unsigned int type();
    double numerator();
    double denominator();
    double scalar();
    QVector<double> data();
    void adjustFilter(double A);

    // --- Filters ---
    void loadFilter();
    void loadFilter(QString filePath);
    void loadFilter(QString filePath,
                    int emptyValue);
    void applyFilter(cv::Mat &inputImage,
                     cv::Mat &outputImage);
};

#endif // MYFILTER_H
