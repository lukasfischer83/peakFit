#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMap>
#include <QListWidgetItem>
#include <QFileDialog>
#include <masslibrary.h>

#include "../libs/qcustomplot/qcustomplot.h"
//#include "../libs/eigen-eigen-b30b87236a1b/Eigen/Dense"
#include <Dense>
#include <iostream>
//#include "E:\Devel\Apps\libs\HDF5-1.8.16-win32-mingw\HDF5-1.8.16-win32\include\H5Cpp.h"
//#include "E:\Devel\Apps\libs\HDF5-1.8.16-win32-mingw\HDF5-1.8.16-win32\include\hdf5.h"
//#include "E:\Devel\Apps\libs\HDF5-1.8.16-win32-mingw\HDF5-1.8.16-win32\include\hdf5_hl.h"
#include <H5Cpp.h>

using namespace Eigen;
using namespace H5;
namespace Ui {
class MainWindow;
}

class myQListWidgetItem : public QListWidgetItem
{
    //Q_OBJECT
public:
    explicit myQListWidgetItem(double mass);
    explicit myQListWidgetItem(const QListWidgetItem&);

    double mass() const;
    void setMass(double mass);

signals:

public slots:

private:
    double m_mass;

    // QListWidgetItem interface
public:
    bool operator <(const QListWidgetItem &other) const;
};


class myQCP : public QCustomPlot
{
    Q_OBJECT
    QCPRange getVisibleValueRange(QCPGraph *graph);
public:
    explicit myQCP(QWidget *parent = 0);
    virtual ~myQCP();
    // QWidget interface

    QCPRange rescaleToLargestVisibleValueRange();
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
signals:
    void markerRemoved(double position);
};

class massMarker : public QObject
{
    Q_OBJECT
public:
    massMarker(double mass, QCPItemStraightLine* plotMarker, QListWidgetItem* listItem, molecule_t* molecule = NULL);
    ~massMarker();

private:
    double m_mass;
    molecule_t* m_molecule;
    QCPItemStraightLine* m_plotMarker;
    QListWidgetItem* m_listItem;
public:

    QListWidgetItem *listItem() const;
    void setListItem(QListWidgetItem *listItem);
    QCPItemStraightLine *plotMarker() const;
    void setMarker(QCPItemStraightLine *plotMarker);
    double mass() const;
    double lastFitCoeff;
    void setMass(double mass);
    void updateMass();
    molecule_t* molecule();
    QList<massMarker*> linkedIsotopes;
    massMarker* isotopeParent;
};


class fitterResult_t {
public:
    QVector<double> fittedSpectrum;
    QVector<double> massCoefficients;
    double selectedMassCoefficient;
};

class fitter_t : public QObject
{
    Q_OBJECT
public:
    fitter_t();
    ~fitter_t();

private:
    int peakshapeMode = 0;
    QMap<double,double> m_peakshape;
    QMap<double,double> m_resolution;
    QMap<double,QVector<double> > m_massDepPeakshape;
    QVector<double> m_massList;
    QVector<double> m_dataToFit;
    QVector<double> generateSinglePeak(double mass, const QVector<double> *massAxis, double coefficient=1.0);
    QVector<double> addVectors(const QVector<double> a, const QVector<double> b);
    QVector<double> substractVectors(const QVector<double> a, const QVector<double> b);
    double interpolateMap(double key, QMap<double,double>* map);

public:
    void setData(QVector<double> peakshape, QVector<double> peakshapeXAxis, QVector<double> resolution, QVector<double> resolutionXAxis);
    void setData(QMap<double,QVector<double> > massDepPeakshape);
    fitterResult_t fit(QVector<double> massAxis, QVector<double> values, QVector<double> masslist, QVector<double> fixedMasses=QVector<double>(), QVector<double> fixedMassesCoeffs=QVector<double>());
};

class MainWindow : public QMainWindow
{
    typedef QMap<double, massMarker*>  markersMap_t;
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    fitter_t fitter;
    QFileDialog loadSpecFileDialog;
    QFileDialog saveResultFileDialog;
    QCPGraph* totalSumSpectrum;
    QCPGraph* minSumSpectrum;
    QCPGraph* maxSumSpectrum;
    QCPGraph* spectrumToFitTo;
    QCPGraph* fittedSpectrum;
    QCPGraph* subspectraGraph;
    //QCPItemStraightLine* draggedItem;
    massMarker* draggedMarker; // replaces draggedItem
    QList<QCPAbstractItem*> massHints;
    QList<double> massHintPositions;
    QPen hintPen;
    QPen identifiedPen;
    QPen autoPen;
    QPen unidentifiedPen;

    markersMap_t markersMap;
    markersMap_t isotopeMarkersMap;

    massLibrary masslib;

    //QList<QCPGraph*> subSpectraGraphs;
    QCPDataMap* currentWorkingSpectrum;
    QCPDataMap peakshape;
    void initializeGraphs(void);
    massMarker *addMarker(double x, bool sort=true, bool convertFromPixel=false, molecule_t *molecule = NULL, massMarker *isotopeParent=NULL);
    QList<massMarker*> addIsotopesToMarker(massMarker* marker);
    void setMarkersVisible(bool visibility);
    H5File* spectrumFile;
    QVector<double> get1DSliceFromH5(QString datasetName, int start=0, int end=-1);
    QVector<double> get2DSliceFromH5(QString datasetName, int secondDimIndex, int start=0, int end=-1);
    QVector<int> get2DIntSliceFromH5(QString datasetName, int secondDimIndex, int start=0, int end=-1);
    void hdf5WriteStrings(H5::H5File file, const std::string& data_set_name,const std::vector<std::string>& strings);
    void hdf5WriteMatrixInt(H5::H5File file, const std::string& data_set_name, const std::vector<int> *data , int dimX, int dimY);
    QVector<double> getSpecSliceFromH5(int spectrumId, int start=0, int end=-1);
    QVector<double> getMassAxisFromH5();
    QVector<double> getPeakShapeValuesFromH5();
    QVector<double> getPeakShapeMassAxisFromH5();
    QVector<double> getResolutionFromH5();
    QVector<double> getResolutionAxisFromH5();
    QMap<double, QVector<double> > getMassDepPeakshapeFromH5();
    QVector<QDateTime> spectrumTimes;

    int getSecondDimCountFromH5(QString datasetName);
    int getSumSpecCountFromH5();
    //QCPRange getVisibleValueRange(QCPGraph* graph);
    //QCPRange rescaleToLargestVisibleValueRange();
    void reloadSubSpectrum();
    int getIndexOfValueInSortedVector(const QVector<double> *vec, double val);

    double fitPrepareAndCall(QCPRange range);
    double closestMarkerPosition(double x);
    QVector<double> addVectors(const QVector<double> firstVector,const QVector<double> secondVector);
    QVector<double> substractVectors(const QVector<double> firstVector,const QVector<double> secondVector);

private slots:
    void plotDoubleClicked(QMouseEvent* mouseEvent);
    void plotDragStart(QMouseEvent* mouseEvent);
    void plotDragMove(QMouseEvent* mouseEvent);
    void plotDragEnd(QMouseEvent* mouseEvent);
    void plotMouseWheel(QWheelEvent* mouseWheelEvent);
    void plotSelectionChanged();
    void plotLegendClicked(QCPLegend* legend,QCPAbstractLegendItem* legendItem,QMouseEvent* mouseEvent);
    void plotXRangeChanged(QCPRange newRange, QCPRange oldRange);
    void sliderMoved(int i);
    void massListClicked(QListWidgetItem* currentItem);
    void removeMarker(double position);
    void removeIsotopeMarker(double position);
    void loadSpectrum();
    void loadMassFromCSV();
    void saveToH5();
    void saveToCSV();
    void saveToPDF();
    void addMassToLib();
    void clearMasslist();
    void clearMasslistButtonPress();
};

#endif // MAINWINDOW_H
