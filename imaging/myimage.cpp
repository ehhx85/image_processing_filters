#include "myimage.h"

// ----- Constructor / Destructor ---------------------------------------------
MyImage::MyImage(QString input)
{
    qTitle = input;
}

MyImage::~MyImage()
{
    // destructor call goes here
}

// ----- Image Set Methods ----------------------------------------------------
// --- Set image to data read from file ---
void MyImage::setImage(QString filePath)
{
    cv::destroyAllWindows();
    image.release();
    image = cv::imread(filePath.toStdString(), intensityColorMap);
    setIntensityHistograms();
}

// --- Set image to data read from default file or to default flat intensity ---
void MyImage::setImage(QString filePath, int intensityValue)
{
    if(intensityValue<0) {
        QFile file(filePath);
        cv::destroyAllWindows();
        image.release();

        if(file.open(QIODevice::ReadOnly)) {
            qint64 imageFileSize = file.size();
            std::vector<uchar> buf(imageFileSize);

            file.read((char*)buf.data(), imageFileSize);

            image = cv::imdecode(buf, intensityColorMap);
        }
        setIntensityHistograms();
    }
    else {
        image.release();
        image = cv::Mat::ones(200, 200, intensityColorMap) * intensityValue;
        setIntensityHistograms();
    }
}

// --- Set image to input image size with the same intensity ---
void MyImage::setImage(MyImage inputImage)
{
    image.release();
    image = cv::Mat::zeros(inputImage.image.rows,
                           inputImage.image.cols,
                           inputImage.image.type());

    image = inputImage.image;
    setIntensityHistograms();
}

// --- Set image to input image size with a flat intensity of given value---
void MyImage::setImage(MyImage inputImage, int intensityValue)
{
    if (intensityValue<0) {
       setImage(inputImage);
    }
    else {
        image.release();
        image = cv::Mat::ones(inputImage.image.rows,
                              inputImage.image.cols,
                              inputImage.image.type()) * intensityValue;

        setIntensityHistograms();
    }
}

// ----- -----------------------------------------------
uint8_t MyImage::getIntensity(uint32_t row, uint32_t col)
{
    return image.at<uchar>(row,col);
}

void MyImage::setIntensityHistograms()
{
    intensityBins.clear();
    intensityDistribution.clear();

    intensityBins.fill(0,numberBins);
    intensityDistribution.fill(0,numberBins);

    buildIntensityBins();
    buildIntensityDistribution();
}

void MyImage::buildIntensityBins()
{
    for(uint16_t i=0; i<numberBins; i++)
    {
        intensityBins.replace(i,i);
    }
}

void MyImage::buildIntensityDistribution()
{
    int i = -1;

    for(uint16_t row=0; row < image.rows; row++) {
        for (uint16_t col=0; col < image.cols; col++)
        {
            i = getIntensity(row,col);
            intensityDistribution.replace(i,intensityDistribution.at(i)+1);
        }
    }
}

QImage MyImage::getQImage()
{
    QImage dest((const uchar *) image.data,
                image.cols,
                image.rows,
                image.step,
                QImage::Format_Grayscale8);
    dest.bits();
    return dest;
}

void MyImage::saveImageToPNG(QString outputPath)
{
    std::vector<int> compression_parameters;
    compression_parameters.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_parameters.push_back(3);

    cv::imwrite(outputPath.toStdString(), image, compression_parameters);
}
