#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QElapsedTimer>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    // MassList
    element* carbon = new element("C",12.0);
    element* carbon13 = new element("C(13)",13.003355);
    element* oxygen = new element("O",15.994915);
    element* nitrogen = new element("N",14.003074);
    element* sulfur = new element("S",31.97207);
    element* hydrogen = new element("H",1.007825);
    element* proton = new element("H+",1.007276);

    // Primary Ions
    molecule_t* m = new molecule_t();
    m->addElement(carbon13,0);
    m->addElement(carbon,0);
    m->addElement(oxygen,1);
    m->addElement(hydrogen,2);
    m->addElement(nitrogen,0);
    m->addElement(sulfur,0);
    m->addElement(proton,1);
    masslib.append(m);

    m = new molecule_t();
    m->addElement(carbon13,0);
    m->addElement(carbon,0);
    m->addElement(oxygen,2);
    m->addElement(hydrogen,4);
    m->addElement(nitrogen,0);
    m->addElement(sulfur,0);
    m->addElement(proton,1);
    masslib.append(m);

    m = new molecule_t();
    m->addElement(carbon13,0);
    m->addElement(carbon,0);
    m->addElement(oxygen,3);
    m->addElement(hydrogen,6);
    m->addElement(nitrogen,0);
    m->addElement(sulfur,0);
    m->addElement(proton,1);
    masslib.append(m);


    double Cmax = 22;
    double OtoCmax = 1;
    double HtoCmax = 2.5;
    double HtoCmin = 0.8;
    double Nmax = 3;
    double Smax = 0;

    for (int carb = 1; carb<=Cmax; carb++)
    {
        for (int sulf = 0; sulf<=Smax;sulf++)
        {
            for (int oxy = 0; oxy<=qCeil(carb*OtoCmax); oxy++)
            {
                for (int hyd = qFloor(((carb)*HtoCmin/2))*2; hyd <= qCeil((carb)*HtoCmax)+2;hyd+=2)
                {
                    // Hydrocarbons

                    molecule_t* m = new molecule_t();
                    m->addElement(carbon13,0);
                    m->addElement(carbon,carb);
                    m->addElement(oxygen,oxy);
                    m->addElement(hydrogen,hyd);
                    m->addElement(nitrogen,0);
                    m->addElement(sulfur,sulf);
                    m->addElement(proton,1);
                    masslib.append(m);

                    // Hydrocarbon Isotopes
                    m = new molecule_t();
                    m->addElement(carbon13,1);
                    m->addElement(carbon,carb-1);
                    m->addElement(oxygen,oxy);
                    m->addElement(hydrogen,hyd);
                    m->addElement(nitrogen,0);
                    m->addElement(sulfur,sulf);
                    m->addElement(proton,1);
                    masslib.append(m);

                    // NH4+ Clusters
                    m = new molecule_t();
                    m->addElement(carbon13,0);
                    m->addElement(carbon,carb);
                    m->addElement(oxygen,oxy);
                    m->addElement(hydrogen,hyd+3);
                    m->addElement(nitrogen,1);
                    m->addElement(sulfur,sulf);
                    m->addElement(proton,1);
                    masslib.append(m);


                    // Organonitrates
                    m = new molecule_t();
                    m->addElement(carbon13,0);
                    m->addElement(carbon,carb);
                    m->addElement(oxygen,oxy+3);
                    m->addElement(hydrogen,hyd-1);
                    m->addElement(nitrogen,1);
                    m->addElement(sulfur,sulf);
                    m->addElement(proton,1);
                    masslib.append(m);

                    // Organonitrates Isotopes
                    m = new molecule_t();
                    m->addElement(carbon13,1);
                    m->addElement(carbon,carb-1);
                    m->addElement(oxygen,oxy+3);
                    m->addElement(hydrogen,hyd-1);
                    m->addElement(nitrogen,1);
                    m->addElement(sulfur,sulf);
                    m->addElement(proton,1);
                    masslib.append(m);

                }
            }
        }
    }


    qDebug() << "Masses: " << masslib.count();
    qDebug() << "Elements:";
    for (elementLibrary_t::const_iterator it = masslib.elements().constBegin(); it != masslib.elements().constEnd(); it++)
    {
        qDebug() << it.value()->name() << ": " << it.value()->mass();
    }
    spectrumFile = NULL;
    ui->setupUi(this);
    ui->plot->setInteraction(QCP::iSelectItems,true);
    ui->plot->setInteraction(QCP::iMultiSelect,false);
    ui->plot->setInteraction(QCP::iRangeDrag,true);
    ui->plot->setFocusPolicy(Qt::ClickFocus);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plot->setInteraction(QCP::iRangeZoom,true);
    ui->plot->axisRect()->setRangeZoomAxes(ui->plot->xAxis,NULL);
    connect(ui->plot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(plotDoubleClicked(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)),this,SLOT(plotDragStart(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(plotDragMove(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(plotDragEnd(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseWheel(QWheelEvent*)), this,SLOT(plotMouseWheel(QWheelEvent*)));
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange,QCPRange)),this,SLOT(plotXRangeChanged(QCPRange,QCPRange)));
    connect(ui->plot,SIGNAL(markerRemoved(double)), this, SLOT(markerRemoved(double)));
    connect(ui->plot, SIGNAL(selectionChangedByUser()),this, SLOT(plotSelectionChanged()));
    connect(ui->plot, SIGNAL(legendClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(plotLegendClicked(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)));
    connect(ui->horizontalSlider, SIGNAL(valueChanged(int)),this, SLOT(sliderMoved(int)));
    connect(ui->btnAddMass, SIGNAL(clicked()),this,SLOT(addMassToLib()));


    connect(ui->massList, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(massListClicked(QListWidgetItem*)));
    connect(ui->actionLoad_Spectrum,SIGNAL(triggered()),this,SLOT(loadSpectrum()));
    connect(ui->actionSave_Masses_To_HDF5, SIGNAL(triggered()),this, SLOT(saveToH5()));
    connect(ui->actionSave_Masslist_to_CSV, SIGNAL(triggered()), this, SLOT(saveToCSV()));

    //    totalSumSpectrum = ui->plot->addGraph();
    //    fittedSpectrum = ui->plot->addGraph();

    ui->peakshapePlot->addGraph();
    ui->peakshapePlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->peakshapePlot->hide();
    //    for (double i=0;i<10000; i+=1)
    //    {
    //        addMarker(i);
    //    }
    ui->plot->replot();
    draggedItem = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeGraphs()
{
    hintPen.setColor(QColor(0,0,0,32));
    identifiedPen.setColor(QColor(0,192,0));
    unidentifiedPen.setColor(QColor(0,0,0));

    // ################ Total sum spectrum ##############################
    totalSumSpectrum = ui->plot->addGraph();
    totalSumSpectrum->valueAxis()->setScaleType(QCPAxis::stLogarithmic);
    QPen graphPen;
    graphPen.setColor(QColor("red"));
    totalSumSpectrum->setPen(graphPen);
    totalSumSpectrum->setAntialiased(true);
    totalSumSpectrum->setName("Total Average");

    // ################ Min Max ##############################
    minSumSpectrum = ui->plot->addGraph();
    QPen graphPenMin;
    graphPenMin.setColor(QColor(0,0,0,64));
    minSumSpectrum->setPen(graphPenMin);
    minSumSpectrum->setName("Total Min");

    maxSumSpectrum = ui->plot->addGraph();
    QPen graphPenMax;
    graphPenMax.setColor(QColor(0,150,0,64));
    maxSumSpectrum->setPen(graphPenMax);
    maxSumSpectrum->setName("Total Max");

    // ################ Fit ##############################

    fittedSpectrum = ui->plot->addGraph();
    QPen graphPenFit;
    graphPenFit.setColor(QColor("blue"));
    fittedSpectrum->setPen(graphPenFit);
    fittedSpectrum->setName("Local Fit");

    // ############# Subspectra ###########################
    subspectraGraph = ui->plot->addGraph();
    QPen graphPenS;
    graphPenS.setColor(QColor(150,0,0,150));
    subspectraGraph->setPen(graphPenS);
    subspectraGraph->setName("Sub Spectrum");

    spectrumToFitTo = totalSumSpectrum;
    ui->plot->legend->setVisible(true);
    ui->plot->setToolTip("Scroll with Mouse Wheel\nDouble Click to add mass\nClick and drag mass to move\nClick mass to select\nPress DEL to remove selected mass\nClick element in legend to change fitting source");

    ui->horizontalSlider->setToolTip("Drag slider to scroll through individual average spectra for inspection");

    ui->massList->setToolTip("Click mass to select and zoom plot to selected mass");

}

void MainWindow::addMarker(double x, bool sort, bool convertFromPixel, molecule_t* molecule)
{
    QCPItemStraightLine* lineMarker = new QCPItemStraightLine(ui->plot);
    ui->plot->addItem(lineMarker);
    lineMarker->point1->setAxes(totalSumSpectrum->keyAxis(),totalSumSpectrum->valueAxis());
    lineMarker->point2->setAxes(totalSumSpectrum->keyAxis(),totalSumSpectrum->valueAxis());
    double xpos = x;
    if (convertFromPixel)
        xpos = totalSumSpectrum->keyAxis()->pixelToCoord(x);
    lineMarker->point1->setCoords(xpos,1e-8);
    lineMarker->point2->setCoords(xpos,1e8);
    lineMarker->setSelectable(true);
    myQListWidgetItem* listItem = new myQListWidgetItem(xpos);
    if (molecule == NULL)
    {
        //listItem->setBackgroundColor(QColor(255,0,0,20));
        lineMarker->setPen(unidentifiedPen);
    } else {
        listItem->setBackgroundColor(QColor(0,255,0,20));
        lineMarker->setPen(identifiedPen);
    }
    //    QListWidgetItem* listItem = new QListWidgetItem(QString::number(xpos));
    ui->massList->addItem(listItem);
    if (sort)
    {
        //ui->massList->sortItems();
    }

    massMarker* marker = new massMarker(xpos,lineMarker,listItem,molecule);

    markersMap.insert(xpos,marker);
}



void MainWindow::plotDoubleClicked(QMouseEvent *mouseEvent)
{
    addMarker(mouseEvent->localPos().x(),true,true);
}

void MainWindow::plotDragStart(QMouseEvent *mouseEvent)
{
    QCPAbstractItem* itemUnderMouse = ui->plot->itemAt(mouseEvent->pos(),true);
    if(itemUnderMouse != 0 \
            && itemUnderMouse->visible()\
            && markersMap.contains(((QCPItemStraightLine*) itemUnderMouse)->point1->coords().x())\
            && markersMap.find(((QCPItemStraightLine*) itemUnderMouse)->point1->coords().x()).value()->plotMarker() == itemUnderMouse )
    {
        draggedItem = (QCPItemStraightLine*) itemUnderMouse;
        markerRemoved(draggedItem->point1->coords().x());
        ui->plot->setInteraction(QCP::iRangeDrag,false);
    }
}

void MainWindow::plotDragMove(QMouseEvent *mouseEvent)
{
    if (draggedItem != 0)
    {
        // Do drag calculation stuff
        double xpos = totalSumSpectrum->keyAxis()->pixelToCoord(mouseEvent->pos().x());
        double closestMarkerX = closestMarkerPosition(xpos);
        if ((qAbs(xpos - closestMarkerX) < 0.1/xpos))
        {
            draggedItem->point1->setCoords(closestMarkerX,1e-8);
            draggedItem->point2->setCoords(closestMarkerX,1e8);
        } else {
            draggedItem->point1->setCoords(xpos,1e-8);
            draggedItem->point2->setCoords(xpos,1e8);
        }
        fitPrepareAndCall(ui->plot->xAxis->range());
        ui->plot->replot();
    }
    else // ########### Range Drag --> rescale ###############
    {
        if (mouseEvent->button() == Qt::LeftButton)
            rescaleToLargestVisibleValueRange();
    }
}

void MainWindow::plotDragEnd(QMouseEvent *mouseEvent)
{
    if (draggedItem != 0)
    {
        // Do drop calculation stuff and end drag process
        double xpos = totalSumSpectrum->keyAxis()->pixelToCoord(mouseEvent->pos().x());
        double closestMarkerX = closestMarkerPosition(xpos);
        double newpos;
        molecule_t* molecule;
        if ((qAbs(xpos - closestMarkerX) < 0.1/xpos))
        {
            newpos = closestMarkerX;
            molecule = masslib.find(closestMarkerX).value();
        } else {
            newpos = xpos;
            molecule = NULL;
        }
        draggedItem->point1->setCoords(newpos,1e-8);
        draggedItem->point2->setCoords(newpos,1e8);

        if (markersMap.contains(newpos))
        {
            ui->plot->removeItem(draggedItem);
        } else {
            ui->plot->removeItem(draggedItem);
            addMarker(newpos,false,false,molecule);
//            QListWidgetItem* listItem = new QListWidgetItem(QString::number(newpos));
//            ui->massList->addItem(listItem);
//            listItem->setSelected(draggedItem->selected());
//            massMarker* marker = new massMarker(newpos,draggedItem,listItem,molecule);
//            markersMap.insert(newpos,marker);
        }
        draggedItem = 0;
        ui->plot->setInteraction(QCP::iRangeDrag,true);
    }
    else // ########### Range Drag --> rescale ###############
    {
        rescaleToLargestVisibleValueRange();
    }
    //qDebug() << markersMap;
}

void MainWindow::plotMouseWheel(QWheelEvent *mouseWheelEvent)
{
    rescaleToLargestVisibleValueRange();
}

void MainWindow::plotSelectionChanged()
{
    if (ui->plot->selectedItems().count() > 0)
    {
        QCPItemStraightLine* item = ((QCPItemStraightLine*)ui->plot->selectedItems().at(0));
        double x = item->point1->coords().x();
        if (markersMap.contains(x))
        {
            markersMap.find(x).value()->listItem()->setSelected(true);
            ui->massList->setCurrentItem( markersMap.find(x).value()->listItem());
        } else {
            ui->massList->clearSelection();
        }
    } else {
        ui->massList->clearSelection();
    }
}

void MainWindow::plotLegendClicked(QCPLegend *legend, QCPAbstractLegendItem *legendItem, QMouseEvent *mouseEvent)
{
    for(int i=0; i<legend->parentPlot()->graphCount(); i++)
    {
        if(legend->parentPlot()->graph(i)->name() == ((QCPPlottableLegendItem*)legendItem)->plottable()->name())
        {
            QCPGraph* chosenGraph = legend->parentPlot()->graph(i);
            if (chosenGraph != fittedSpectrum)
            {
                if (chosenGraph == subspectraGraph && subspectraGraph->data()->count() < 10)
                {
                }
                else
                {
                    spectrumToFitTo = legend->parentPlot()->graph(i);
                    fitPrepareAndCall(ui->plot->xAxis->range());
                }
            }
        }
        else
        {
            //legend->parentPlot()->graph(i)->setSelected(false);
        }
    }
}


bool markersEnabled = true;
bool hintsEnabled = false;

void MainWindow::plotXRangeChanged(QCPRange newRange, QCPRange oldRange)
{
    if (newRange.upper-newRange.lower < 10.0) // when zoomed in enough, show markers
    {
        if (markersEnabled == false)
        {
            for(int i=0;i<ui->plot->itemCount();i++)
            {
                ui->plot->item(i)->setVisible(true);
            }
            markersEnabled = true;
        }

    } else {
        if (markersEnabled == true)
        {
            for(int i=0;i<ui->plot->itemCount();i++)
            {
                ui->plot->item(i)->setVisible(false);
            }
            markersEnabled = false;
        }
    }


    if (newRange.upper-newRange.lower < 1.0) // when zoomed in enough, do the fitting shit
    {
        fitPrepareAndCall(newRange);
    } else
    {
        fittedSpectrum->clearData();
    }


    if (newRange.upper-newRange.lower < 0.3) // when zoomed in enough, show mass hints
    {
        if (hintsEnabled == false)
        {
            for (massLibrary::iterator it = masslib.lowerBound(newRange.lower); it != masslib.lowerBound(newRange.upper);it++)
            {
                QCPItemStraightLine* line = new QCPItemStraightLine(ui->plot);
                line->setSelectable(false);
                line->setPen(hintPen);
                line->point1->setCoords(it.value()->mass(),1e-6);
                line->point2->setCoords(it.value()->mass(),1e6);


                QCPItemText* text = new QCPItemText(ui->plot);
                text->setSelectable(false);
                text->setPen(Qt::NoPen);
                text->setText(it.value()->name());
                text->setRotation(270);
                text->setPositionAlignment(Qt::AlignRight);
                text->position->setCoords(it.value()->mass(),ui->plot->yAxis->range().upper);

                ui->plot->addItem(text);
                ui->plot->addItem(line);

                massHints.append(text);
                massHints.append(line);
                massHintPositions.append(it.value()->mass());
            }
            hintsEnabled = true;
            ui->plot->replot();
        }
    }
    else
    {
        if (hintsEnabled == true)
        {
            for (int i=0;i<massHints.count();i++)
            {
                ui->plot->removeItem(massHints.at(i));
            }
            massHints.clear();
            massHintPositions.clear();
            hintsEnabled = false;
        }
        ui->plot->replot();
    }
}

void MainWindow::sliderMoved(int i)
{
    if (spectrumFile != NULL)

    {
        if(i>1)
        {
            QElapsedTimer timer;
            timer.start();
            QVector<double> datS = get2DSliceFromH5("SumSpecs", i-1, 0, -1);
            QVector<double> baseline = get2DSliceFromH5("BaseLines", i-1, 0, -1);
            qDebug() << "Loading Spectrum took " << timer.elapsed() << "ms";
            subspectraGraph->setData(totalSumSpectrum->data()->keys().toVector(),substractVectors(datS,baseline));

            subspectraGraph->setVisible(true);
        }
        else
        {
            subspectraGraph->clearData();
            subspectraGraph->setVisible(false);
            spectrumToFitTo = totalSumSpectrum;
        }
        fitPrepareAndCall(ui->plot->xAxis->range());
        ui->plot->replot();
    }
}

void MainWindow::massListClicked(QListWidgetItem *currentItem)
{
    massMarker* marker = NULL;
    markersMap_t::iterator it = markersMap.begin();
    while (it != markersMap.end())
    {
        if (it.value()->listItem() == currentItem)
        {
            marker = it.value();
            break;
        }
        it++;
    }
    if (marker != NULL)
    {
        foreach (QCPAbstractItem* item ,ui->plot->selectedItems())
        {
            item->setSelected(false);
        }

        ui->plot->xAxis->setRange(it.value()->mass(),1,Qt::AlignCenter);
        marker->plotMarker()->setSelected(true);
    }
    rescaleToLargestVisibleValueRange();
    ui->plot->setFocus();
    ui->plot->replot();
}

void MainWindow::markerRemoved(double position)
{
    ui->massList->clearSelection();
    ui->massList->removeItemWidget( markersMap.find(position).value()->listItem());
    delete markersMap.find(position).value()->listItem();
    markersMap.remove(position);
    fitPrepareAndCall(ui->plot->xAxis->range());
}


QCPRange MainWindow::getVisibleValueRange(QCPGraph *graph)
{
    // ############################### STILL BUGGY AND SLOW ########################
    if (graph != NULL && graph->data()->count() > 0)
    {
        QCPDataMap* data = graph->data();
        QCPRange rng = graph->keyAxis()->range();
        QCPDataMap::iterator it, itUp;
        if (rng.lower < data->first().key)
            it = data->begin();
        else
            it = data->lowerBound(rng.lower);

        if (rng.upper > data->last().key)
            itUp = data->end();
        else
            itUp = data->lowerBound(rng.upper);

        double min = 1e10;
        double max = 1e-10;
        while (it++ != itUp) {
            if (it.value().value > max)
                max=it.value().value;
            if (it.value().value < min && it.value().value>1e-6)
                min=it.value().value;
        }
        qDebug() << "Rescale Range: Lower: " << min << " Upper: " << max;
        return  QCPRange(min,max);
    }
    return QCPRange(0,0);
}

QCPRange MainWindow::rescaleToLargestVisibleValueRange()
{
    double globalMin = 1e10;
    double globalMax = -1e10;
    //    for (int i = 0; i<ui->plot->graphCount()-1;i++)
    //    {
    //        QCPRange rng = getVisibleValueRange(ui->plot->graph(i));
    //        if (rng.lower<globalMin)
    //            globalMin = rng.lower;
    //        if (rng.upper>globalMax)
    //            globalMax = rng.upper;
    //    }
    QCPRange rng = getVisibleValueRange(maxSumSpectrum);
    ui->plot->yAxis->setRange(QCPRange(rng.lower/2,rng.upper*2).sanitizedForLogScale());
    ui->plot->replot();
    return QCPRange(globalMin,globalMax);
}


void MainWindow::fitPrepareAndCall(QCPRange range)
{
    QVector<double> massAxis,values,massesAll, masses;
    double margin = 0.5; // include margin outside visible range in calculations
    QCPDataMap::iterator it, itUp;
    //    it = totalSumSpectrum->data()->lowerBound(range.lower-margin);
    //    itUp = totalSumSpectrum->data()->lowerBound(range.upper+margin);
    it = spectrumToFitTo->data()->lowerBound(range.lower-margin);
    itUp = spectrumToFitTo->data()->lowerBound(range.upper+margin);

    double min = 1e10;
    double max = 1e-10;
    while (it++ != itUp) {
        values.append(it.value().value);
        massAxis.append(it.value().key);
    }
    massesAll = markersMap.keys().toVector();
    for (int i=0;i<massesAll.count();i++)
    {
        if (massesAll.at(i)>range.lower-margin && massesAll.at(i)<range.upper+margin)
            masses.append(massesAll.at(i));
    }
    if (draggedItem != 0)
    {
        masses.append((draggedItem->point1->coords().x()));
    }
    QVector<double> fittedValues = fitter.fit(massAxis,values,masses);
    fittedSpectrum->setData(massAxis, fittedValues);
    ui->plot->replot();
}

double MainWindow::closestMarkerPosition(double x)
{
    double distance = 1e6;
    double closestMarker = 0;
    for(int i=0;i<massHintPositions.count();i++)
    {
        if (qAbs( massHintPositions.at(i)-x) <distance )
        {
            distance = qAbs( massHintPositions.at(i)-x);
            closestMarker = massHintPositions.at(i);
        }

    }
    return closestMarker;
}

QVector<double> MainWindow::addVectors(const QVector<double> firstVector, const QVector<double> secondVector)
{
    int max;
    if (firstVector.count() < secondVector.count())
        max = firstVector.count();
    else
        max = secondVector.count();
    QVector<double> ret(max);
    for(int i=0;i<max;i++)
    {
        ret.replace(i,firstVector.at(i)+secondVector.at(i));
    }
    return ret;
}

QVector<double> MainWindow::substractVectors(const QVector<double> firstVector, const QVector<double> secondVector)
{
    int max;
    if (firstVector.count() < secondVector.count())
        max = firstVector.count();
    else
        max = secondVector.count();
    QVector<double> ret(max);
    for(int i=0;i<max;i++)
    {
        ret.replace(i,firstVector.at(i)-secondVector.at(i));
    }
    return ret;
}

void MainWindow::loadSpectrum()
{
    const QString DEFAULT_DIR_KEY(".");
    QSettings loadFileSettings; // Will be using application informations for correct location of your settings

    QString filename = saveResultFileDialog.getOpenFileName(this,tr("Open Spectrum File"), loadFileSettings.value(DEFAULT_DIR_KEY).toString(),tr("Spectrum Files (*.hdf5)"));

    if (filename != "")
    {
        QDir CurrentDir;
        loadFileSettings.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(filename));

        qDebug() << "Opening File " << filename;
        H5std_string fn(filename.toStdString());
        spectrumFile = new H5File(fn,H5F_ACC_RDWR);

        ui->plot->clearGraphs();
        initializeGraphs();


        // #################### Get Tot. Avg. Spectrum and plot #################
        //        spectrumFile = new H5File(fn,H5F_ACC_RDONLY);
        QVector<double> massAxis = getMassAxisFromH5();
        //        QVector<double> dat = getSpecSliceFromH5(0,0,-1);
        QVector<double> dat = get1DSliceFromH5("AvgSpectrum");
        QVector<double> baseline = get1DSliceFromH5("AvgBaseline");
        totalSumSpectrum->setData(massAxis,substractVectors(dat,baseline));

        // #################### Get Min. Avg. Spectrum and plot #################
        QVector<double> dat1 = get1DSliceFromH5("SumSpecMin");
        minSumSpectrum->setData(massAxis,substractVectors(dat1,baseline));

        // #################### Get Max. Avg. Spectrum and plot #################
        QVector<double> dat2 = get1DSliceFromH5("SumSpecMax");
        maxSumSpectrum->setData(massAxis,substractVectors(dat2,baseline));


        // #################### Load preliminary masslist ######################
        ui->massList->clear();
        markersMap.clear();
        ui->plot->clearItems();
        QVector<double> masses = get1DSliceFromH5("MassList");


        QVector<QString> elementNamesInFile;
        elementNamesInFile.append("C");
        elementNamesInFile.append("C(13)");
        elementNamesInFile.append("H");
        elementNamesInFile.append("H+");
        elementNamesInFile.append("N");
        elementNamesInFile.append("O");
        elementNamesInFile.append("S");

        QVector<element*> elementsInFile;

        for (int i=0;i<elementNamesInFile.count();i++)
        {
            if (masslib.elements().contains(elementNamesInFile.at(i)))
            {
                elementsInFile.append(masslib.elements().find(elementNamesInFile.at(i)).value());
            } else {
                // need to create new element !
                qDebug() << "Add new element on demand - Not implemented yet!";
            }
        }

        bool compositionsFoundInFile = H5Lexists(spectrumFile->getId(), "ElementalCompositions", H5P_DEFAULT);

        for (int i=0;i<masses.count();i++)
        {
            molecule_t* closeMolecule = masslib.findClosestMolecule(masses.at(i));
            if ((qAbs(masses.at(i) - closeMolecule->mass()) < 0.002))
            {
                //qDebug() << "Found very close mass in lib --> snapping " << QString::number(masses.at(i)) << " to " << QString::number(closeMolecule->mass()) << " (" << closeMolecule->name() << ")";
                addMarker(closeMolecule->mass(),false,false,closeMolecule);
            } else {
                if (compositionsFoundInFile )
                {
                    QVector<int> compositionCoefs = get2DIntSliceFromH5("ElementalCompositions",i);
                    molecule_t* m = new molecule_t();
                    for (int i=0;i<elementsInFile.count();i++)
                    {
                        m->addElement(elementsInFile.at(i),compositionCoefs.at(i));
                    }
                    if (!masslib.contains(m->mass()))
                    {
                        masslib.append(m);
                        addMarker(masses.at(i),false,false,m);

                    } else {
                        delete m;
                        addMarker(masses.at(i),false,false);
                    }
                }
                else // just add unknown marker if no compositions found and no close match
                {
                    addMarker(masses.at(i),false,false);
                }
            }
        }

        qDebug() << "Elements in masslib after file loaded: " <<  masslib.elements().count();
        qDebug() << "Masses in masslib after file loaded: " <<  masslib.count();


        // #################### Get Sub Spectra #######################
        int NbrSpecs = getSumSpecCountFromH5();
        ui->horizontalSlider->setMinimum(1);
        ui->horizontalSlider->setMaximum(NbrSpecs);
        ui->horizontalSlider->setTickInterval(1);


        // #################### Get Peakshape  #########################

        QVector<double> peakshapeAxis = getPeakShapeMassAxisFromH5();
        QVector<double> peakshapeVals = getPeakShapeValuesFromH5();
        peakshape.clear();
        if (peakshapeAxis.count() == peakshapeVals.count())
        {
            for (int i=0;i<peakshapeAxis.count();i++)
            {
                peakshape.insert(peakshapeAxis.at(i),QCPData(peakshapeAxis.at(i),peakshapeVals.at(i)));
            }
        }
        ui->peakshapePlot->graph(0)->clearData();
        ui->peakshapePlot->graph(0)->setData(&peakshape,true);
        ui->peakshapePlot->rescaleAxes(false);
        ui->peakshapePlot->replot();

        ui->plot->rescaleAxes(false);
        ui->plot->replot();

        // #################### Get Resolution vs Mass #################
        QVector<double> resolutionAxis = getResolutionAxisFromH5();
        QVector<double> resolution = getResolutionFromH5();
        fitter.setData(peakshapeVals,peakshapeAxis,resolution,resolutionAxis);
        ui->actionSave_Masses_To_HDF5->setEnabled(true);
        ui->actionSave_Masslist_to_CSV->setEnabled(true);
        qDebug() << "Success.";
    }
}


QVector<double> MainWindow::getSpecSliceFromH5(int spectrumId, int start, int end)
{
    return get2DSliceFromH5("SumSpecs", spectrumId, start, end);
}


QVector<double> MainWindow::getMassAxisFromH5()
{
    return get1DSliceFromH5("MassAxis");
}

QVector<double> MainWindow::getResolutionFromH5()
{
    return get1DSliceFromH5("Resolution");
}

QVector<double> MainWindow::getResolutionAxisFromH5()
{
    QVector<double> resolution = get1DSliceFromH5("Resolution");
    for (int i=0;i<resolution.count();i++)
    {
        resolution.replace(i,i); // Resolution is stored 1 Value per AMU
    }
    return resolution;
}

QVector<double> MainWindow::getPeakShapeValuesFromH5()
{
    return get1DSliceFromH5("Peakshape");
}

QVector<double> MainWindow::getPeakShapeMassAxisFromH5()
{
    return get1DSliceFromH5("PeakshapeX");
}

int MainWindow::getSumSpecCountFromH5()
{
    return getSecondDimCountFromH5("SumSpecs");
}

int MainWindow::getSecondDimCountFromH5(QString datasetName)
{
    int len = 0;

    try
    {
        DataSet sumSpectra = spectrumFile->openDataSet(datasetName.toStdString());
        hsize_t dimsOut[2];
        sumSpectra.getSpace().getSimpleExtentDims( dimsOut, NULL);
        len = dimsOut[0];
    }
    catch( FileIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataTypeIException error )
    {
        error.printError();
    }
    return len;
}



QVector<double> MainWindow::get1DSliceFromH5(QString datasetName, int start, int end)
{
    QVector<double> dat ;

    try
    {
        int len = 0;
        DataSet dataset = spectrumFile->openDataSet(datasetName.toStdString());
        hsize_t dimsOut[1];
        dataset.getSpace().getSimpleExtentDims( dimsOut, NULL);
        if(end==-1)
        {
            len = dimsOut[0];
        }
        else
        {
            len = end-start;
        }


        DataSpace dataspace = dataset.getSpace();


        /*
         * Define hyperslab in the dataset; implicitly giving strike and
         * block NULL.
         */
        hsize_t offset[1];
        hsize_t count[1];
        offset[0] = start;

        count[0] = len;

        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        /*
         * Define the memory dataspace.
         */
        hsize_t dimsm[1];              /* memory space dimensions */
        dimsm[0] = len;
        DataSpace memspace( 1, dimsm );
        memspace.selectAll();

        float* x = new float[len];
        dataset.read( x, PredType::NATIVE_FLOAT, memspace, dataspace );

        for (int i=0;i<len;i++)
        {
            dat.append(x[i]);
        }

        delete[] x;
    }
    catch( FileIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataTypeIException error )
    {
        error.printError();
    }

    return dat;

}

QVector<double> MainWindow::get2DSliceFromH5(QString datasetName, int secondDimIndex, int start, int end)
{
    QVector<double> dat ;

    try
    {
        int len = 0;
        DataSet sumSpectra = spectrumFile->openDataSet(datasetName.toStdString());
        hsize_t dimsOut[2];
        sumSpectra.getSpace().getSimpleExtentDims( dimsOut, NULL);
        if(end==-1)
        {
            len = dimsOut[1];
        }
        else
        {
            len = end-start;
        }


        DataSpace dataspace = sumSpectra.getSpace();


        /*
         * Define hyperslab in the dataset; implicitly giving strike and
         * block NULL.
         */
        hsize_t offset[2];
        hsize_t count[2];
        offset[0] = secondDimIndex;
        offset[1] = start;

        count[0] = 1;
        count[1] = len;

        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        hssize_t selSize = dataspace.getSelectNpoints();

        /*
         * Define the memory dataspace.
         */
        hsize_t dimsm[2];              /* memory space dimensions */
        dimsm[0] = 1;
        dimsm[1] = len;
        DataSpace memspace( 2, dimsm );
        memspace.selectAll();

        float* x = new float[len];
        sumSpectra.read( x, PredType::NATIVE_FLOAT, memspace, dataspace );

        for (int i=0;i<len;i++)
        {
            dat.append(x[i]);
        }

        delete[] x;
    }
    catch( FileIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataTypeIException error )
    {
        error.printError();
    }

    return dat;
}


QVector<int> MainWindow::get2DIntSliceFromH5(QString datasetName, int secondDimIndex, int start, int end)
{
    QVector<int> dat ;

    try
    {
        int len = 0;
        DataSet sumSpectra = spectrumFile->openDataSet(datasetName.toStdString());
        hsize_t dimsOut[2];
        sumSpectra.getSpace().getSimpleExtentDims( dimsOut, NULL);
        if(end==-1)
        {
            len = dimsOut[1];
        }
        else
        {
            len = end-start;
        }

        DataSpace dataspace = sumSpectra.getSpace();


        /*
         * Define hyperslab in the dataset; implicitly giving strike and
         * block NULL.
         */
        hsize_t offset[2];
        hsize_t count[2];
        offset[0] = secondDimIndex;
        offset[1] = start;

        count[0] = 1;
        count[1] = len;

        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        hssize_t selSize = dataspace.getSelectNpoints();

        /*
         * Define the memory dataspace.
         */
        hsize_t dimsm[2];              /* memory space dimensions */
        dimsm[0] = 1;
        dimsm[1] = len;
        DataSpace memspace( 2, dimsm );
        memspace.selectAll();

        int* x = new int[len];
        sumSpectra.read( x, PredType::NATIVE_INT, memspace, dataspace );

        for (int i=0;i<len;i++)
        {
            dat.append(x[i]);
        }

        delete[] x;
    }
    catch( FileIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataTypeIException error )
    {
        error.printError();
    }

    return dat;
}

QCPItemStraightLine *massMarker::plotMarker() const
{
    return m_plotMarker;
}

void massMarker::setMarker(QCPItemStraightLine *plotMarker)
{
    m_plotMarker = plotMarker;
}
massMarker::massMarker(double mass, QCPItemStraightLine *marker, QListWidgetItem *listItem, molecule_t* molecule)
{
    setMass(mass);
    setListItem(listItem);
    setMarker(marker);
    if (molecule != NULL)
    {
        m_molecule = molecule;
    } else {
        m_molecule = new molecule_t();
    }
}

massMarker::~massMarker()
{

}
double massMarker::mass() const
{
    return m_mass;
}

void massMarker::setMass(double mass)
{
    m_mass = mass;
}

molecule_t *massMarker::molecule()
{
    return m_molecule;
}


QListWidgetItem *massMarker::listItem() const
{
    return m_listItem;
}

void massMarker::setListItem(QListWidgetItem *listItem)
{
    m_listItem = listItem;
}


fitter_t::fitter_t()
{

}

fitter_t::~fitter_t()
{

}

void fitter_t::setData(QVector<double> peakshape, QVector<double> peakshapeXAxis, QVector<double> resolution, QVector<double> resolutionXAxis)
{
    if(peakshape.count() == peakshapeXAxis.count() && resolution.count() == resolutionXAxis.count())
    {
        for(int i=0;i<peakshape.count();i++)
        {
            m_peakshape.insert(peakshapeXAxis.at(i),peakshape.at(i));
        }
        for(int i=0;i<resolution.count();i++)
        {
            m_resolution.insert(resolutionXAxis.at(i),resolution.at(i));
        }
    }
}

QVector<double> fitter_t::generateSinglePeak(double mass, const QVector<double>* massAxis)
{
    double localResolution = interpolateMap(mass,&m_resolution);
    QVector<double> values;
    for (int i=0; i<massAxis->count();i++)
    {
        double val=0;
        double peakshapeLookupIndex = (massAxis->at(i)-mass)*localResolution/mass;
        if (peakshapeLookupIndex < m_peakshape.firstKey() || peakshapeLookupIndex > m_peakshape.lastKey())
            val=0;
        else
            val=m_peakshape.lowerBound(peakshapeLookupIndex).value();
        values.append(val);
    }
    return values;
}

double fitter_t::interpolateMap(double key, QMap<double, double> *map)
{
    if (key < map->firstKey())
        return map->first();
    if (key > map->lastKey())
        return map->last();
    QMap<double,double>::iterator high = map->begin();
    while(high.key()<key && high != map->end())
        high++;
    QMap<double,double>::iterator low = high;
    low--;
    qDebug() << "Low:  " << low.key() ;
    qDebug() << "High: " << high.key();
    double distance = high.key() -low.key();
    double contribHigh = (key-low.key())/distance;
    double contribLow = 1-contribHigh;
    return low.value()*contribLow + high.value()*contribHigh;
}



QVector<double> fitter_t::fit(QVector<double> massAxis, QVector<double> values, QVector<double> masslist)
{
    if (massAxis.count() > 2 && values.count() == massAxis.count() && masslist.count() > 0)
    {
        QVector<double> ret;
        qDebug() << "fitter.fit() called";
        //    // Solve Ax=b for x
        //    MatrixXd A = MatrixXd::Random(values.count(),masslist.count());
        //    MatrixXd b = MatrixXd::Random(values.count(),1);
        //        MatrixXd x = A.fullPivLu().solve(b);
        //    MatrixXd x = A.householderQr().solve(b);
        //        std::cout << x << "\n\n";

        MatrixXd A(values.count(),masslist.count());
        for (int j=0;j<masslist.count();j++)
        {
            QVector<double> singlePeak = generateSinglePeak(masslist.at(j),&massAxis);
            for (int i=0; i<values.count();i++)
            {
                A(i,j) = singlePeak.at(i);
            }
        }
        MatrixXd b(values.count(),1);

        for(int i=0; i< values.count(); i++)
        {
            b(i,0) = values.at(i);
        }

        MatrixXd x = A.householderQr().solve(b);
        MatrixXd c = A*x;
        for(int i=0;i<values.count(); i++)
        {
            ret.append(c(i,0));
        }
        return ret;
    } else
    {
        return QVector<double>();
    }
}


myQCP::myQCP(QWidget *parent) : QCustomPlot(parent)
{

}

myQCP::~myQCP()
{

}

void myQCP::keyPressEvent(QKeyEvent * event)
{
    qDebug() << "Key was pressed: " << event->key();
    if (event->key() == Qt::Key_Delete)
    {
        if (this->selectedItems().count() > 0)
        {
            //delete markersMap.find(draggedItem->point1->coords().x()).value()->listItem();
            QCPItemStraightLine* item = ((QCPItemStraightLine*)selectedItems().at(0));
            double removeposition = item->point1->coords().x();
            removeItem(item);
            replot();
            emit markerRemoved(removeposition);
        }
    }
}

void MainWindow::saveToH5()
{
    QVector<double> dat = markersMap.keys().toVector();
    //QVector<double> dat;

    try
    {

        QList<QString> elementNames;
        for (elementLibrary_t::const_iterator it = masslib.elements().constBegin(); it != masslib.elements().constEnd(); it++)
        {
            elementNames.append(it.value()->name());
        }


        // ################################# Masslist #########################################################
        if (H5Lexists(spectrumFile->getId(), "MassList", H5P_DEFAULT) == true)
        {
            qDebug() << "Dataset exists, overwriting!";
            H5Ldelete(spectrumFile->getId(),"MassList",NULL);
        }
        hsize_t dims[1];
        dims[0] = dat.count();
        DataSpace dataspace(1,dims);
        DataSet* dataset = new DataSet(spectrumFile->createDataSet("MassList",PredType::NATIVE_FLOAT,dataspace));

        float* x = new float[dims[0]];
        for (int i=0;i<dims[0];i++)
        {
            x[i] = dat.at(i);
        }

        dataset->write(x,PredType::NATIVE_FLOAT,dataspace,NULL);
        delete[] x;



        // ################################## Elements ########################################################

        if (H5Lexists(spectrumFile->getId(), "ElementMasses", H5P_DEFAULT) == true)
        {
            qDebug() << "ElementMasses exists, overwriting!";
            H5Ldelete(spectrumFile->getId(),"ElementMasses",NULL);
        }
        hsize_t dimsElementMasses[1];
        dimsElementMasses[0] = elementNames.count();
        DataSpace dataspaceElementMasses(1,dimsElementMasses);
        DataSet* datasetElementMasses = new DataSet(spectrumFile->createDataSet("ElementMasses",PredType::NATIVE_FLOAT,dataspaceElementMasses));

        float* x1 = new float[dimsElementMasses[0]];
        for (int i=0;i<dimsElementMasses[0];i++)
        {
            x1[i] = masslib.elements().find(elementNames.at(i)).value()->mass();
        }

        datasetElementMasses->write(x1,PredType::NATIVE_FLOAT,dataspaceElementMasses,NULL);
        delete[] x1;


        if (H5Lexists(spectrumFile->getId(), "ElementNames", H5P_DEFAULT) == true)
        {
            qDebug() << "ElementNames exists, overwriting!";
            H5Ldelete(spectrumFile->getId(),"ElementNames",NULL);
        }

        std::string datasetname;
        datasetname = "ElementNames";
        std::vector<std::string> strings;

        for (int i=0;i<elementNames.count();i++)
        {
            strings.push_back(elementNames.at(i).toStdString());
        }

        hdf5WriteStrings(*spectrumFile,datasetname, strings);

        // ############################### Compositions #######################################################

        std::vector<int>* d = new std::vector<int>();
        d->reserve(dat.count()*elementNames.count());

        for (int i=0;i<dat.count();i++)
        {
            for (int j=0;j<elementNames.count();j++)
            {

                d->push_back( markersMap.find(dat.at(i)).value()->molecule()->elementCountByName(elementNames.at(j)) );
            }
        }
        datasetname = "ElementalCompositions";
        if (H5Lexists(spectrumFile->getId(), "ElementalCompositions", H5P_DEFAULT) == true)
        {
            qDebug() << "ElementalCompositions exists, overwriting!";
            H5Ldelete(spectrumFile->getId(),"ElementalCompositions",NULL);
        }

        hdf5WriteMatrixInt(*spectrumFile,datasetname,d,dat.count(),elementNames.count());


        spectrumFile->flush(H5F_SCOPE_GLOBAL);
    }
    catch( FileIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSet operations
    catch( DataSetIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataSpaceIException error )
    {
        error.printError();
    }
    // catch failure caused by the DataSpace operations
    catch( DataTypeIException error )
    {
        error.printError();
    }

}

void MainWindow::saveToCSV()
{
    const QString DEFAULT_DIR_KEY("masslist.csv");
    QSettings csvSavePath; // Will be using application informations for correct location of your settings
    saveResultFileDialog.setDefaultSuffix(".csv");
    QString filename = saveResultFileDialog.getSaveFileName(this,tr("Save masslist to CSV"), csvSavePath.value(DEFAULT_DIR_KEY).toString(),tr("CSV File (*.csv)"));

    if (filename != "")
    {
        QDir CurrentDir;
        csvSavePath.setValue(DEFAULT_DIR_KEY, CurrentDir.absoluteFilePath(filename));


        QFile csvFile(filename);
        if (csvFile.open(QFile::WriteOnly))
        {
            QTextStream stream(&csvFile);
//            QVector<double> masses = markersMap.keys().toVector();
//            for (int i=0;i<masses.count();i++)
//            {
//                stream << QString::number(masses.at(i),'g',8) << "\n";
//            }
            QList<QString> MassesNames;
            for (elementLibrary_t::const_iterator it = masslib.elements().constBegin(); it != masslib.elements().constEnd(); it++)
            {
                stream << it.value()->name() << "\t";
                MassesNames.append(it.value()->name());
            }
            stream << "Mass\tName\n";

            for (markersMap_t::iterator it = markersMap.begin();it != markersMap.end();it++)
            {
                for (int i= 0; i< MassesNames.count();i++)
                {
                    stream << QString::number(it.value()->molecule()->elementCountByName(MassesNames.at(i)),'g',3) << "\t";
                }
                stream << QString::number(it.key(),'g',8) << "\t";
                stream << it.value()->molecule()->name() << "\n";
            }
            csvFile.flush();
            csvFile.close();
        }
    }
}

void MainWindow::addMassToLib()
{
    molecule_t* m = new molecule_t();
    m->addElement(masslib.elements().find("C").value(),ui->spinC->value());
    m->addElement(masslib.elements().find("C(13)").value(),ui->spinC13->value());
    m->addElement(masslib.elements().find("H").value(),ui->spinH->value());
    m->addElement(masslib.elements().find("O").value(),ui->spinO->value());
    m->addElement(masslib.elements().find("N").value(),ui->spinN->value());
    m->addElement(masslib.elements().find("S").value(),ui->spinS->value());
    m->addElement(masslib.elements().find("H+").value(),ui->spinProt->value());

    if (!masslib.contains(m->mass()))
    {
        masslib.append(m);
    }
}


void MainWindow::hdf5WriteStrings(H5::H5File file, const std::string& data_set_name,
                const std::vector<std::string>& strings )
{
    //H5::Exception::dontPrint();

    try
    {
        // HDF5 only understands vector of char* :-(
        std::vector<const char*> arr_c_str;
        for (unsigned ii = 0; ii < strings.size(); ++ii)
            arr_c_str.push_back(strings[ii].c_str());

        //
        //  one dimension
        //
        hsize_t     str_dimsf[1] {arr_c_str.size()};
        H5::DataSpace   dataspace(1, str_dimsf);

        // Variable length string
        H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE);
        H5::DataSet str_dataset = file.createDataSet(data_set_name, datatype, dataspace);

        str_dataset.write(arr_c_str.data(), datatype);
    }
    catch (H5::Exception& err)
    {
       err.printError();
    }
}

void MainWindow::hdf5WriteMatrixInt(H5::H5File file, const std::string& data_set_name,
                const std::vector<int>* data, int dimX, int dimY)
{
    qDebug() << "Writing " << dimX << " mass compositions of " << dimY << " elements to HDF5";
    if (data->size() > 0)
    {
        try
        {

            hsize_t dims[2];
            dims[0] = dimX;
            dims[1] = dimY;

            H5::DataSpace dataspace(2, dims);
            H5::DataSet dataset = file.createDataSet(data_set_name, PredType::NATIVE_INT, dataspace);
            dataset.write(data->data(), PredType::NATIVE_INT);
        }
        catch (H5::Exception& err)
        {
            err.printError();
        }
    }

}



myQListWidgetItem::myQListWidgetItem(double mass) :
    QListWidgetItem()
{
    setMass(mass);
}

myQListWidgetItem::myQListWidgetItem(const QListWidgetItem &)
{
    setMass(0);
}
double myQListWidgetItem::mass() const
{
    return m_mass;
}

void myQListWidgetItem::setMass(double mass)
{
    this->setText(QString::number(mass,'g',7));
    m_mass = mass;
}

bool myQListWidgetItem::operator <(const QListWidgetItem &other) const
{
    //return m_mass > ((myQListWidgetItem) other).mass();
    //return true;
    //qDebug() << this->text().toDouble() << " < " << other.text().toDouble() << "??";
    return this->text().toDouble() < other.text().toDouble();
}

