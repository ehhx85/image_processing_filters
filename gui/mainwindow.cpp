// ----- Headers --------------------------------------------------------------
#include "mainwindow.h"
#include "ui_mainwindow.h"

// ----- Constructor / Destructor ---------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rawImage(MyImage("")),
    filteredImage(MyImage("")),
    filterObject(MyFilter())
{
    ui->setupUi(this);

    buildMenu();
    buildComboBoxes();
    buildSliderBar();
    buildGraphics();
    buildFilterMatrix();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ----- Build Methods --------------------------------------------------------
void MainWindow::buildMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    helpMenu = menuBar()->addMenu(tr("&Help"));

    // --- New File Menu Actions ---
    openDefaultAction = new QAction(tr("Open &Default Image"), this);
    openAction = new QAction(tr("&Open from File"), this);
    resetAction = new QAction(tr("&Reset Image"), this);
    saveAction = new QAction(tr("&Save"), this);
    saveAsAction = new QAction(tr("Save &As..."), this);
    closeAction = new QAction(tr("&Close"), this);
    exitAction = new QAction(tr("&Exit"), this);

    // --- New Help Menu Actions ---
    aboutAction = new QAction(tr("&About This Application"), this);
    aboutQtAction = new QAction(tr("&About Qt"), this);
    aboutAuthorAction = new QAction(tr("&About Author"), this);

    // --- Set File Menu Shortcuts ---
    openDefaultAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    openAction->setShortcut(QKeySequence::Open);
    resetAction->setShortcut(QKeySequence::Refresh);
    saveAction->setShortcut(QKeySequence::Save);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    closeAction->setShortcut(QKeySequence::Close);
    exitAction->setShortcut(QKeySequence::Quit);

    // --- Connect file menu actions to slots triggered at menu ---
    connect(openDefaultAction, SIGNAL(triggered()), this, SLOT(openDefault()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(resetAction, SIGNAL(triggered()), this, SLOT(reset()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));

    // --- Connect help menu actions to slots triggered at menu ---
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(aboutAuthorAction, SIGNAL(triggered()), this, SLOT(aboutAuthor()));

    // --- Add the file menu actions to the file menu bar---
    fileMenu->addAction(openDefaultAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(resetAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    // --- Add the help menu actions to the help menu bar ---
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
    helpMenu->addAction(aboutAuthorAction);
}

void MainWindow::buildComboBoxes()
{
    imageFileList.clear();
    filterFileList.clear();

    populateImageList();

    ui->comboBox_imageSelection->addItems(imageFileList);
    ui->comboBox_filterSelection->addItems(MyFilter::filterStringList);

    connect(ui->comboBox_imageSelection, SIGNAL(activated(int)), this, SLOT(openDefault()));
    connect(ui->comboBox_filterSelection, SIGNAL(activated(int)), this, SLOT(getFilterMatrix()));
}

void MainWindow::buildSliderBar()
{
    connect(ui->horizontalSlider_filterAdjust, SIGNAL(valueChanged(int)), this, SLOT(updateSliderLCD()));

    ui->horizontalSlider_filterAdjust->setRange(0,100);
    ui->horizontalSlider_filterAdjust->setValue(0);
    ui->lcdNumber_sliderValue->setDigitCount(5);
}

void MainWindow::buildGraphics()
{
    item = new QGraphicsPixmapItem;

    rawScene = new QGraphicsScene(this);
    ui->graphicsView_rawImage->setScene(rawScene);

    filteredScene = new QGraphicsScene(this);
    ui->graphicsView_filteredImage->setScene(filteredScene);

    filteredHistogram = new QCPBars(ui->qCustomPlotHistogram->xAxis, ui->qCustomPlotHistogram->yAxis);
    filteredHistogram->setName("Filtered Image Histogram");
    filteredHistogram->setPen(QColor("#000000"));
    ui->qCustomPlotHistogram->xAxis->setRange(0,10);
    ui->qCustomPlotHistogram->yAxis->setRange(0,1);
}
void MainWindow::buildFilterMatrix()
{
    ui->label_filterSelection->setText("Select a Filter");
    filterModel = new QStandardItemModel(1,1,this);
    ui->tableView_filterMatrix->setModel(filterModel);
    ui->lcdNumber_filterNumerator->setDigitCount(5);
    ui->lcdNumber_filterDenominator->setDigitCount(5);
    ui->lcdNumber_filterScalar->setDigitCount(5);
}

// ----- File Menu Action Slots -----------------------------------------------
void MainWindow::openDefault()
{
    printToStatusFromMenuCommand("File","Open from Image List");
    QString tmp = ui->comboBox_imageSelection->currentText();
    appendToStatusBar(QString("Loading from default list ... ") + tmp);

    tmp = QString(":/") +  tmp;
    rawImage.setImage(tmp, -1);
    filteredImage.setImage(rawImage,0);
    updateGraphics();
}

void MainWindow::open()
{
    printToStatusFromMenuCommand("File","Open");

    QFileDialog dialog(this);

    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Image (*.png)"));
    dialog.setDefaultSuffix("png");

    if (dialog.exec()) {
        QStringList filePath = dialog.selectedFiles();
        QString tmp = filePath.at(0);

        appendToStatusBar(QString("Loading from file ... ") + tmp);

        rawImage.setImage(tmp);
        filteredImage.setImage(rawImage,0);
        updateGraphics();
    }
    else {
        appendToStatusBar(" ... open canceled");
    }
}

void MainWindow::reset()
{
    if(rawImage.intensityBins.size() == 0)
    {
        appendToStatusBar(QString("No data loaded."));
        return;
    }

    printToStatusFromMenuCommand("File","Reset");

    rawImage.setImage("",0);
    filteredImage.setImage("",0);
    updateGraphics();

    appendToStatusBar(QString("Images cleared"));
}

void MainWindow::save()
{
    printToStatusFromMenuCommand("File","Save");

    if(rawImage.intensityBins.size() == 0)
    {
        appendToStatusBar(QString("No data loaded, save canceled."));
        return;
    }

    outputDirectory = QDir::home();
    QString tmp = "/_tmp.png";
    tmp = outputDirectory.path() + tmp;

    appendToStatusBar(QString("Saving to file ... ") + tmp);
    filteredImage.saveImageToPNG(tmp);
}

void MainWindow::saveAs()
{
    printToStatusFromMenuCommand("File","Save As...");

    if(rawImage.intensityBins.size() == 0)
    {
        appendToStatusBar(QString("No data loaded, save canceled."));
        return;
    }

    QFileDialog dialog(this);

    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);

    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Image (*.png)"));
    dialog.setDefaultSuffix("png");

    if (dialog.exec()) {
        QStringList filePath = dialog.selectedFiles();
        appendToStatusBar(QString("Saving to file ... ") + filePath.at(0));
        filteredImage.saveImageToPNG(filePath.at(0));
    }
    else {
        appendToStatusBar(" ... save canceled");
    }
}

void MainWindow::close()
{
    printToStatusFromMenuCommand("File","Close");

    rawScene->clear();
    filteredScene->clear();
}

void MainWindow::quit()
{
    printToStatusFromMenuCommand("File","Quit");
    QApplication::quit();
}

// ----- Help Menu Action Slots -----------------------------------------------
void MainWindow::about()
{
    printToStatusFromMenuCommand("Help","About Application");
}

void MainWindow::aboutQt()
{}

void MainWindow::aboutAuthor()
{
    printToStatusFromMenuCommand("Help","About Author");
}

// ---- Other Actions and Slots -----------------------------------------------
void MainWindow::updateGraphics()
{
    rawScene->clear();
    filteredScene->clear();

    rawScene->addPixmap(QPixmap::fromImage(rawImage.getQImage()));
    filteredScene->addPixmap(QPixmap::fromImage(filteredImage.getQImage()));

    filteredHistogram->setData(filteredImage.intensityBins, filteredImage.intensityDistribution);
    ui->qCustomPlotHistogram->xAxis->rescale();
    ui->qCustomPlotHistogram->yAxis->rescale();
    ui->qCustomPlotHistogram->replot();
}

void MainWindow::updateSliderLCD()
{
    ui->lcdNumber_sliderValue->display(ui->horizontalSlider_filterAdjust->value());
    QString tmp = QString::number(ui->horizontalSlider_filterAdjust->value());
    statusBar()->showMessage("Slider bar value adjusted | A = " + tmp);

    if (filterObject.type() == 4) // sloppy implementation
    {
        getFilterMatrix();
    }
}

void MainWindow::getFilterMatrix()
{
    timer.start();

    filterObject.loadFilter(ui->comboBox_filterSelection->currentText(),0);
    statusBar()->showMessage("Applying filter | " + filterObject.name());

    if (filterObject.type() == 4) // sloppy implementation
    {
        double tmp = 1.0 * ui->horizontalSlider_filterAdjust->value() / ui->horizontalSlider_filterAdjust->maximum();
        filterObject.adjustFilter(5*tmp);
    }

    ui->label_filterSelection->setText(filterObject.name());
    ui->lcdNumber_filterNumerator->display(filterObject.numerator());
    ui->lcdNumber_filterDenominator->display(filterObject.denominator());
    ui->lcdNumber_filterScalar->display(filterObject.scalar());

    updateFilterMatrix();

    if(rawImage.intensityBins.size() == 0) {
        appendToStatusBar(QString(" ... no data loaded, filter not applied."));
        return;
    }
    else {
        filterObject.applyFilter(rawImage.image, filteredImage.image);
        filteredImage.setIntensityHistograms();
        updateGraphics();
    }

    QString tmp = QString::number(timer.elapsed());
    appendToStatusBar(QString(" ... processed filter in ") + tmp + QString(" mSec"));
}

void MainWindow::updateFilterMatrix()
{
    unsigned int i = 0;
    double tmp = 0;
    QString tmpString;

    filterModel->clear();

    for (unsigned int row = 0; row < filterObject.rows(); ++row) {
        for (unsigned int column = 0; column < filterObject.cols(); ++column)
        {
            tmp = filterObject.data().at(i);
            tmpString.setNum(tmp,'g',3);

            QStandardItem *item = new QStandardItem(tmpString);
            filterModel->setItem(row, column, item);
            i++;
        }
    }
}

// ----- Status Bar -----------------------------------------------------------
void MainWindow::printToStatusFromMenuCommand(QString menuString,
                                              QString subMenuString)
{
    QString S = " | ";
    QString tmp = menuString + S + subMenuString + S;
    statusBar()->showMessage(tmp);
}

void MainWindow::appendToStatusBar(QString newString)
{
    statusBar()->showMessage(statusBar()->currentMessage()+newString);
}

// ----- Data From Resource Files -----------
void MainWindow::populateImageList()
{
    imageFileList.append("astronaut");
    imageFileList.append("chart_gray");
    imageFileList.append("elephants");
    imageFileList.append("Lenna");
    imageFileList.append("pollen");
    imageFileList.append("moon");
    imageFileList.append("moon (blurred)");
    imageFileList.append("test_pattern");
    imageFileList.append("woman");
}

