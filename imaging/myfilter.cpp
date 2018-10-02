#include "myfilter.h"

QStringList MyFilter::filterStringList = QStringList()
    << "Copy (Default)"
    << "Copy (Negative)"
    << "Copy 3x3"
    << "Copy 5x5"
    << "Copy 11x11"
    << "Min Reset"
    << "Max Reset"
    << "Median 3x3"
    << "Median 5x5"
    << "Average (Box)"
    << "Average (Weighted)"
    << "Laplacian 1"
    << "Laplacian 2"
    << "Laplacian 3"
    << "Laplacian 4"
    << "Gradient (Horizontal)"
    << "Gradient (Vertical)"
    << "Gradient (Absolute Value)"
    << "Gradient (Square Root)"
    << "Orientation Check";

// ----- Constructor / Destructor ---------------------------------------------
MyFilter::MyFilter()
{
    loadFilter();
}

MyFilter::~MyFilter()
{}

// ----- Accessors ------------------------------------------------------------
QString MyFilter::name()
{
    return filterName;
}

unsigned int MyFilter::rows()
{
    return maskRows;
}

unsigned int MyFilter::cols()
{
    return maskCols;
}

unsigned int MyFilter::size()
{
    return maskSize;
}

unsigned int MyFilter::type()
{
    return filterType;
}

double MyFilter::numerator()
{
    return scalarNum;
}

double MyFilter::denominator()
{
    return scalarDen;
}
double MyFilter::scalar()
{
    setScalarValue();
    return scalarValue;
}

QVector<double> MyFilter::data()
{
    return filterArray;
}

void MyFilter::adjustFilter(double A)
{
    unsigned int tmpIndex = (maskSize - 1) / 2;
    double tmpValue = 0;

    if (filterArray.at(tmpIndex)>0)
    {
        tmpValue = filterArray.at(tmpIndex) + A;
        filterArray.replace(tmpIndex,tmpValue);
    }
    if (filterArray.at(tmpIndex)<0)
    {
        tmpValue = filterArray.at(tmpIndex) - A;
        filterArray.replace(tmpIndex,tmpValue);
    }
}

// ----- -----
void MyFilter::setMaskSize()
{
    maskSize = maskRows * maskCols;

    padRows = maskRows - 1;
    padCols = maskCols - 1;

    a = floor(padRows/2);
    b = floor(padCols/2);
}

void MyFilter::setScalarValue()
{
    scalarValue = scalarNum / scalarDen;
}

void MyFilter::setFilterMaxScaling()
{
    double tmpPos = 0;
    double tmpNeg = 0;

    for (int i=0; i<filterArray.size(); i++)
    {
        if (filterArray.at(i)>0)
        {
            tmpPos = tmpPos + filterArray.at(i);
        }
        else if (filterArray.at(i)<0)
        {
            tmpNeg = tmpNeg + filterArray.at(i);
        }
    }

    if (std::abs(tmpNeg)>tmpPos)
    {
        filterMaxScaling = tmpNeg;
    }
    else
    {
        filterMaxScaling = tmpPos;
    }

    if (filterType == 6)
    {
        filterMaxScaling = 2 * filterMaxScaling;
    }
    else if (filterType == 7)
    {
        filterMaxScaling = sqrt(2) * filterMaxScaling;
    }
}

// ----- Load Filter ----------------------------------------------------------
void MyFilter::loadFilter()
{
    filterType = 0;

    maskRows = 1;
    maskCols = 1;
    setMaskSize();

    scalarNum = 1;
    scalarDen = 1;
    setScalarValue();

    filterArray.clear();
    filterArray.append(1);

    filterArray2.clear();
    filterArray2.append(1);

    subArray.clear();
    subArray.append(1);
}

void MyFilter::loadFilter(QString filePath)
{
    // --- Open File ---
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly)) {return;}

    // --- Create stream read variables ---
    QTextStream in(&file);
    QStringList tmpStrings;
    QString line;

    filterName = in.readLine();

    // --- Read the filter type ---
    line = in.readLine();
    tmpStrings = line.split("=");
    filterType = tmpStrings.at(1).toInt();

    // --- Read the scaling values ---
    line = in.readLine();
    tmpStrings = line.split("/");
    scalarNum = tmpStrings.at(0).toDouble();
    scalarDen = tmpStrings.at(1).toDouble();
    setScalarValue();

    // --- Read the mask size ---
    line = in.readLine();
    tmpStrings = line.split("x");
    maskRows = tmpStrings.at(0).toInt();
    maskCols = tmpStrings.at(1).toInt();
    setMaskSize();

    // --- Read the filter matrix values to array ---
    filterArray.clear();
    filterArray2.clear();
    for(unsigned int i=0; i<maskRows; i++)
    {
        line = in.readLine();
        QStringList values = line.split(",");

        for(unsigned int j=0; j<maskCols; j++)
        {
            double tmp = values.at(j).toDouble();
            filterArray.append(tmp);
        }
    }

    // --- Read the secondary filter matrix values to array ---
    if (filterType>5)
    {
        for(unsigned int i=0; i<maskRows; i++)
        {
            line = in.readLine();
            QStringList values = line.split(",");

            for(unsigned int j=0; j<maskCols; j++)
            {
                double tmp = values.at(j).toDouble();
                filterArray2.append(tmp);
            }
        }
    }

    subArray.clear();
    subArray.fill(0,filterArray.size());
}

void MyFilter::loadFilter(QString listString, int emptyValue)
{
    QString filterPath = QString(":/") + listString;
    loadFilter(filterPath);
    emptyValue++;
}

// ------ Apply Filter --------------------------------------------------------
void MyFilter::applyFilter(cv::Mat & inputImage, cv::Mat & outputImage)
{
    setMaxIntensity(inputImage);
    setFilterMaxScaling();

    cv::Mat paddedImage;
    padImage(inputImage,paddedImage,outputImage);

    // --- Process subArray using filterArray ---
    for (unsigned int row=0; row < (paddedImage.rows); row++)
    {
        for (unsigned int col=0; col < (paddedImage.cols); col++)
        {
            scanSubArray(row,col,paddedImage);

            double tmp = 0;

            // --- Process the filtered subArray based on type ---
            switch (filterType)
            {
            case (-1): // --- Negative Copy ---
                tmp = 255 - TdotProduct();
                break;
            case (0): // --- Testing Function ---
                tmp = scalarValue;
                break;
            case (1): // --- Positive Copy ---
                tmp = TdotProduct();
                break;
            case (2): // --- Median Average ---
                tmp = TmedianValue();
                break;
            case (3): // --- Mean Average ---
                tmp = scalarValue * TdotProduct();
                break;
            case (4): // --- Laplacian ---
                tmp = TlaplacianValue();
                break;
            case (5): // --- Gradient ---
                tmp = Tgradient1D();
                break;
            case (6):
                tmp = Tgradient2DAbsoluteValue();
                break;
            case (7):
                tmp = Tgradient2DSquareRoot();
                break;
            default:
                break;
            }
            setPixelValue(row+a,col+b,paddedImage,outputImage,tmp);
        }
    }

    if(filterType>2)
    {
        rebinImage(outputImage);
    }
}


// ----- Filtering Functions --------------------------------------------------
void MyFilter::setMaxIntensity(cv::Mat &inputImage)
{
    cv::minMaxLoc(inputImage,&currentMinIntensity,&currentMaxIntensity);
}

void MyFilter::padImage(cv::Mat &inputImage,
                        cv::Mat &paddedImage,
                        cv::Mat &outputImage)
{

    outputImage = cv::Mat::zeros((inputImage.rows),
                                 (inputImage.cols),
                                  inputImage.type());

    paddedImage = cv::Mat::zeros((inputImage.rows + 2 * padRows),
                                 (inputImage.cols + 2 * padCols),
                                  inputImage.type());

    for (unsigned int row=0; row < paddedImage.rows; row++)
    {
        for (unsigned int col=0; col < paddedImage.cols; col++)
        {
            if (row < padRows || (paddedImage.rows - maskRows) < row ||
                col < padCols || (paddedImage.cols - maskCols) < col )
            {
                paddedImage.at<uchar>(row,col) = 0;
            }
            else {
                paddedImage.at<uchar>(row,col) = inputImage.at<uchar>(row-padRows,col-padCols);
            }
        }
    }
}

void MyFilter::scanSubArray(unsigned int pixelRow,
                            unsigned int pixelCol,
                            cv::Mat &paddedImage)
{
    subArray.clear();
    for (unsigned int row=pixelRow; row<(pixelRow+maskRows); row++)
    {
        for (unsigned int col=pixelCol; col<(pixelCol+maskCols); col++)
        {
            double tmp = paddedImage.at<uchar>(row,col);
            subArray.append(tmp);
        }
    }
}

void MyFilter::setPixelValue(unsigned int row,
                             unsigned int col,
                             cv::Mat &paddedImage,
                             cv::Mat &outputImage,
                             double pixelValue)
{
    if (row < padRows || (paddedImage.rows - maskRows) < row ||
        col < padCols || (paddedImage.cols - maskCols) < col )
    {
        return;
    }
    else {
        outputImage.at<uchar>(row-padRows,col-padCols) = pixelValue;
    }
}

void MyFilter::rebinImage(cv::Mat &outputImage)
{
    double tmpMin = 0;
    double tmpMax = 0;

    cv::minMaxLoc(outputImage,&tmpMin,&tmpMax);

    for (unsigned int row=0; row < outputImage.rows; row++)
    {
        for (unsigned int col=0; col < outputImage.cols; col++)
        {
            double tmp = outputImage.at<uchar>(row,col);
            tmp = tmp - tmpMin; // scales to [0,tmpMax-tmpMin]
            tmp = tmp / (tmpMax-tmpMin); // scales to [0,1]
            tmp = tmp * 255; // scales to [0,255]

            outputImage.at<uchar>(row,col) = tmp;
        }
    }
}

// ----- Filter Transformations -----------------------------------------------
void MyFilter::filterSubArray()
{
    for (unsigned int i=0; i<maskSize ;i++)
    {
        double tmp = subArray.at(i) * filterArray.at(i);
        subArray.replace(i,tmp);
    }
}

double MyFilter::TdotProduct()
{
    filterSubArray();

    double tmp = 0;
    for (unsigned int i=0; i<maskSize ;i++)
    {
        tmp = tmp + subArray.at(i);
    }
    return tmp;
}

double MyFilter::TmedianValue()
{
    filterSubArray();

    QVector<double> tmp = subArray;
    std::sort(tmp.begin(),tmp.end());

    unsigned int tmpIndex = (maskSize - 1) / 2;
    return tmp.at(tmpIndex);
}

double MyFilter::TlaplacianValue()
{
    double tmp = TdotProduct();

    tmp = scaleFilteredValue(tmp);

    return tmp;
}

double MyFilter::Tgradient1D()
{
    double tmp = TdotProduct();

    tmp = scaleFilteredValue(tmp);

    return tmp;
}

void MyFilter::Tgradient2D(double &tmpX, double &tmpY)
{
    tmpX = 0;
    tmpY = 0;

    for (unsigned int i=0; i<maskSize ;i++)
    {
        tmpX = tmpX + subArray.at(i) * filterArray.at(i);
        tmpY = tmpY + subArray.at(i) * filterArray2.at(i);
    }
}

double MyFilter::Tgradient2DAbsoluteValue()
{
    double tmpX;
    double tmpY;

    Tgradient2D(tmpX, tmpY);

    double tmp = (std::abs(tmpX) + std::abs(tmpY));

    tmp = scaleFilteredValue(tmp);
    return tmp;
}

double MyFilter::Tgradient2DSquareRoot()
{
    double tmpX;
    double tmpY;

    Tgradient2D(tmpX, tmpY);

    double tmp = sqrt(pow(tmpX, 2) + pow(tmpY, 2));

    tmp = scaleFilteredValue(tmp);
    return tmp;
}

double MyFilter::scaleFilteredValue(double value)
{
    // function mapping [ -C*max, +C*max ]

    double tmp = value / filterMaxScaling;
    // new mapping [ -max, +max ]

    tmp = tmp + currentMaxIntensity;
    // new mapping [ 0, 2*max ]

    tmp = tmp / (2 * currentMaxIntensity);
    // new mapping [ 0, 1 ]

    tmp = tmp * 255;
    // final mapping [ 0, 255 ]

    return tmp;
}
