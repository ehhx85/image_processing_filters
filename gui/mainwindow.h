#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// --- Header Definitions ---
#include <QMainWindow>

#include <QAction>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QLCDNumber>
#include <QList>
#include <QSlider>
#include <QStandardItemModel>
#include <QString>
#include <QTableView>

#include "qcustomplot.h"
#include "myimage.h"
#include "myfilter.h"


// --- Main Window Class ---
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // --- File Menu Slots ---
    void openDefault();
    void open();
    void reset();
    void save();
    void saveAs();
    void close();
    void quit();

    // --- Help Menu Slots ---
    void about();
    void aboutQt();
    void aboutAuthor();

    // --- Other Slots ---
    void getFilterMatrix();

    void updateGraphics();
    void updateSliderLCD();
    void updateFilterMatrix();

private:
    // --- Main Window ---
    Ui::MainWindow *ui;
    QElapsedTimer timer;

    // --- Directories ---
    QDir inputDirectory;
    QDir outputDirectory;

    // --- Menus ---
    QMenu *fileMenu;
    QMenu *helpMenu;

    // --- Actions ---
    QAction *openDefaultAction;
    QAction *openAction;
    QAction *resetAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *aboutAuthorAction;

    // --- Graphics ---
    QGraphicsPixmapItem *item;

    QGraphicsScene *rawScene;
    QGraphicsScene *filteredScene;

    // --- Images ---
    QList<QString> imageFileList;
    QList<QString> filterFileList;

    MyImage rawImage;
    MyImage filteredImage;

    QCPBars *filteredHistogram;

    // --- Image Filter ---
    MyFilter filterObject;
    QStandardItemModel *filterModel;

    // --- Methods ---
    void buildComboBoxes();
    void buildFilterMatrix();
    void buildGraphics();
    void buildMenu();
    void buildSliderBar();

    void populateImageList();

    void appendToStatusBar(QString newString);
    void printToStatusFromMenuCommand(QString menuString, QString subMenuString);


};

#endif // MAINWINDOW_H
