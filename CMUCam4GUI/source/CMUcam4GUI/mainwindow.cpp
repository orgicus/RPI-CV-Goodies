/***************************************************************************//**
* @file
* CMUcam4 Graphical User Interface
*
* @version @n 1.0
* @date @n 2/20/2013
*
* @author @n Kwabena W. Agyeman
* @copyright @n (c) 2013 Kwabena W. Agyeman
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v1.0 - Original release - 2/20/2013
*******************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

CMUcam4 CMUcam;
QextSerialPort CMUcom;

QMutex CMUcamMutex;
QMutex CMUcomMutex;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Setup Ui

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(CMUCAM4_DEFAULT_GUI_TAB);

    // Setup Future Watchers

    connect(&statusFutureWatcher, SIGNAL(finished()),
            this, SLOT(statusUpdate()));

    connect(&streamFutureWatcher, SIGNAL(finished()),
            this, SLOT(getTypeDataPacket()));

    connect(&GUIFutureWatcher, SIGNAL(finished()),
            &GUIEventLoop, SLOT(quit()));

    // Setup Status Bar

    CMUcam4State = new QLabel();
    CMUcam4GUIState = new QLabel(tr("Ready "));

    CMUcam4Version = new QLabel();
    CMUcam4GUIVersion = new QLabel(tr("CMUcam4GUI Version 1.00 "));

    ui->StatusBar->addWidget(CMUcam4State); // Leftmost
    ui->StatusBar->addWidget(CMUcam4GUIState); // Left

    ui->StatusBar->addPermanentWidget(CMUcam4Version); // Right
    ui->StatusBar->addPermanentWidget(CMUcam4GUIVersion); // Rightmost

    // Setup Viewing Window

    SFSBView = new MyQGraphicsScene;
    SFSBView->addItem(SFPixmap = new QGraphicsPixmapItem); // Bottom
    SFSBView->addItem(SBPixmap = new QGraphicsPixmapItem); // Top
    SFSBView->addItem(SFRect = new QGraphicsRectItem);
    ui->imageView->setScene(SFSBView);

    SFPath = SFSBView->selectionArea();

    connect(SFSBView, SIGNAL(selectionChanged()),
            this, SLOT(changedSelection()));

    // Setup Color Tracking View

    FTView = new QGraphicsScene;
    FTView->addItem(TPixmap = new QGraphicsPixmapItem);
    FPixmap = QPixmap(CMUCAM4_NATIVE_H_RES, CMUCAM4_NATIVE_V_RES);
    ui->colorTrackingView->setScene(FTView);

    // Setup Color Statistics View

    ui->colorStatisticsView->addPlottable(RPlot = new QCPStatisticalBox(
    ui->colorStatisticsView->xAxis, ui->colorStatisticsView->yAxis));

    ui->colorStatisticsView->addPlottable(GPlot = new QCPStatisticalBox(
    ui->colorStatisticsView->xAxis, ui->colorStatisticsView->yAxis));

    ui->colorStatisticsView->addPlottable(BPlot = new QCPStatisticalBox(
    ui->colorStatisticsView->xAxis, ui->colorStatisticsView->yAxis));

    ui->colorStatisticsView->addPlottable(HPlot = new QCPBars(
    ui->colorStatisticsView->xAxis, ui->colorStatisticsView->yAxis));

    ui->colorStatisticsView->xAxis->setAutoTicks(CMUCAM4_DEFAULT_X_AT);
    ui->colorStatisticsView->xAxis->setAutoTickLabels(CMUCAM4_DEFAULT_X_ATL);
    ui->colorStatisticsView->xAxis->setTickLabelRotation(CMUCAM4_DEFAULT_X_R);

    ui->colorStatisticsView->xAxis->setTickLength(CMUCAM4_DEFAULT_X_TL);
    ui->colorStatisticsView->xAxis->setSubTickLength(CMUCAM4_DEFAULT_X_STL);

    ui->colorStatisticsView->yAxis->setAutoTicks(CMUCAM4_DEFAULT_Y_AT);
    ui->colorStatisticsView->yAxis->setAutoTickLabels(CMUCAM4_DEFAULT_Y_ATL);
    ui->colorStatisticsView->yAxis->setTickLabelRotation(CMUCAM4_DEFAULT_Y_R);

    ui->colorStatisticsView->xAxis2->setVisible(CMUCAM4_DEFAULT_X2_VISIBLE);
    ui->colorStatisticsView->xAxis2->setTicks(CMUCAM4_DEFAULT_X2_T);
    ui->colorStatisticsView->xAxis2->setTickLabels(CMUCAM4_DEFAULT_X2_TL);

    ui->colorStatisticsView->yAxis2->setVisible(CMUCAM4_DEFAULT_Y2_VISIBLE);
    ui->colorStatisticsView->yAxis2->setTicks(CMUCAM4_DEFAULT_Y2_T);
    ui->colorStatisticsView->yAxis2->setTickLabels(CMUCAM4_DEFAULT_Y2_TL);

    ui->colorStatisticsView->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->colorStatisticsView->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    ui->colorStatisticsView->xAxis->setBasePen(
    QApplication::palette().color(QPalette::Shadow));

    ui->colorStatisticsView->yAxis->setBasePen(
    QApplication::palette().color(QPalette::Shadow));

    ui->colorStatisticsView->xAxis2->setBasePen(
    QApplication::palette().color(QPalette::Shadow));

    ui->colorStatisticsView->yAxis2->setBasePen(
    QApplication::palette().color(QPalette::Shadow));

    ui->colorStatisticsView->yAxis->setTickPen(
    QApplication::palette().color(QPalette::Shadow));

    ui->colorStatisticsView->yAxis->setSubTickPen(
    QApplication::palette().color(QPalette::Shadow));

    RPlot->setBrush(QBrush(Qt::red));
    RPlot->setPen(QPen(QColor(Qt::darkRed)));
    RPlot->setMedianPen(QPen(QColor(Qt::darkRed)));
    RPlot->setWhiskerPen(QPen(QColor(Qt::darkRed)));
    RPlot->setWhiskerBarPen(QPen(QColor(Qt::darkRed)));
    RPlot->setOutlierPen(QPen(QColor(Qt::darkRed)));
    RPlot->setOutlierStyle(QCP::ssDisc);

    GPlot->setBrush(QBrush(Qt::green));
    GPlot->setPen(QPen(QColor(Qt::darkGreen)));
    GPlot->setMedianPen(QPen(QColor(Qt::darkGreen)));
    GPlot->setWhiskerPen(QPen(QColor(Qt::darkGreen)));
    GPlot->setWhiskerBarPen(QPen(QColor(Qt::darkGreen)));
    GPlot->setOutlierPen(QPen(QColor(Qt::darkGreen)));
    GPlot->setOutlierStyle(QCP::ssDisc);

    BPlot->setBrush(QBrush(Qt::blue));
    BPlot->setPen(QPen(QColor(Qt::darkBlue)));
    BPlot->setMedianPen(QPen(QColor(Qt::darkBlue)));
    BPlot->setWhiskerPen(QPen(QColor(Qt::darkBlue)));
    BPlot->setWhiskerBarPen(QPen(QColor(Qt::darkBlue)));
    BPlot->setOutlierPen(QPen(QColor(Qt::darkBlue)));
    BPlot->setOutlierStyle(QCP::ssDisc);

    // Reset Ui

    disableGraphicalUserInterace();
}

MainWindow::~MainWindow()
{
    CMUcom.close();

    delete FTView;
    delete SFSBView;

    delete ui;
}

void MainWindow::begin()
{
    if(ui->actionEnd->isEnabled())
    {
        if(lockGraphicalUserInterface(tr("Reset Error")))
        {
            if(statusUpdateEnable)
            {
                statusUpdateEnable = CMUCAM4_DEFAULT_SC;
                ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
                statusEventLoop.exec();
            }

            GUIFuture = QtConcurrent::run(resetSystemWrapper);

            if(ranGraphicalUserInterface(tr("Reset Error")))
            {
                enableGraphicalUserInterace();
            }
            else
            {
                disableGraphicalUserInterace();
                CMUcom.close();
            }

            unlockGraphicalUserInterface();
        }
    }
    else
    {
        if(lockGraphicalUserInterface(tr("Begin Error")))
        {
            QMap<int, QString> com; // Support COM* - Windows port
            QMap<int, QString> acm; // Support ttyACM* - Linux port
            QMap<int, QString> usb; // Support ttyUSB* - Linux port
            QMap<int, QString> ser; // Support ttyS* - Linux port

            QStringList cu; // Support cu.* - Macintosh port
            QStringList tty; // Support tty.* - Macintosh port

            QStringList generic;

            foreach(QextPortInfo info, QextSerialEnumerator::getPorts())
            {
                if(info.portName.contains(QRegExp(QString("COM\\d"))))
                {
                    com.insertMulti(info.portName.mid(info.portName.indexOf(
                    QRegExp(QString("\\d")))).toInt(), info.portName);
                }
                else if(info.portName.contains(QRegExp(QString("ttyACM\\d"))))
                {
                    acm.insertMulti(info.portName.mid(info.portName.indexOf(
                    QRegExp(QString("\\d")))).toInt(), info.portName);
                }
                else if(info.portName.contains(QRegExp(QString("ttyUSB\\d"))))
                {
                    usb.insertMulti(info.portName.mid(info.portName.indexOf(
                    QRegExp(QString("\\d")))).toInt(), info.portName);
                }
                else if(info.portName.contains(QRegExp(QString("ttyS\\d"))))
                {
                    ser.insertMulti(info.portName.mid(info.portName.indexOf(
                    QRegExp(QString("\\d")))).toInt(), info.portName);
                }
                else if(info.portName.startsWith(QString("cu.")))
                {
                    cu.append(info.portName);
                }
                else if(info.portName.startsWith(QString("tty.")))
                {
                    tty.append(info.portName);
                }
                else
                {
                    generic.append(info.portName);
                }
            }

            generic = tty + generic;
            generic = cu + generic;

            generic = QStringList(ser.values()) + generic;
            generic = QStringList(usb.values()) + generic;
            generic = QStringList(acm.values()) + generic;
            generic = QStringList(com.values()) + generic;

            if(generic.isEmpty())
            {
                QMessageBox::critical(this, tr("Begin Error"),
                tr("No Serial Ports found"));
            }
            else
            {
                bool okay;

                QString item = QInputDialog::getItem(this,
                tr("Select a Serial Port"), tr("Available Ports"), generic, +0,
                false, &okay, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

                if(okay)
                {
                    CMUcom.setPortName(item);
                    CMUcom.setQueryMode(QextSerialPort::Polling);

                    if(!CMUcom.open(QIODevice::ReadWrite))
                    {
                        QMessageBox::critical(this, tr("Begin Error"),
                        tr("Unable to open Selected Serial Port"));
                    }
                    else
                    {
                        CMUcom.setDataBits(DATA_8);
                        CMUcom.setFlowControl(FLOW_OFF);
                        CMUcom.setParity(PAR_NONE);
                        CMUcom.setStopBits(STOP_1);
                        CMUcom.setTimeout(-1);

                        CMUcom.setDtr(false);
                        CMUcom.setRts(false);

                        /* NOTE: Added the below code to qextserialport_win
                        between lines 108 and 109 to disable DTR and RTS.

                        Win_CommConfig.dcb.fDtrControl = DTR_CONTROL_DISABLE;
                        Win_CommConfig.dcb.fRtsControl = RTS_CONTROL_DISABLE;

                        */

                        /* NOTE: Added the below code to qextserialport_unix
                        between lines xxx and xxx to disable DTR and RTS.

                        ...
                        ...

                        */

                        GUIFuture = QtConcurrent::run(beginWrapper);

                        if(ranGraphicalUserInterface(tr("Begin Error")))
                        {
                            enableGraphicalUserInterace();
                        }
                        else
                        {
                            disableGraphicalUserInterace();
                            CMUcom.close();
                        }
                    }
                }
            }

            unlockGraphicalUserInterface();
        }
    }
}

void MainWindow::end()
{
    if(lockGraphicalUserInterface(tr("End Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(endWrapper);

        if(ranGraphicalUserInterface(tr("End Error")))
        {
            disableGraphicalUserInterace();
            CMUcom.close();
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::sleepLightly()
{
    if(lockGraphicalUserInterface(tr("Sleep Lightly Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(sleepLightlyWrapper);

        if(ranGraphicalUserInterface(tr("Sleep Lightly Error")))
        {
            // Camera Auto Control Box

            ui->agcCheckBox->setChecked(CMUCAM4_DEFAULT_AGC);
            ui->awbCheckBox->setChecked(CMUCAM4_DEFAULT_AWB);
            ui->hmCheckBox->setChecked(CMUCAM4_DEFAULT_HM);
            ui->vfCheckBox->setChecked(CMUCAM4_DEFAULT_VF);
            ui->bwCheckBox->setChecked(CMUCAM4_DEFAULT_BW);
            ui->ngCheckBox->setChecked(CMUCAM4_DEFAULT_NG);
            ui->tmCheckBox->setChecked(CMUCAM4_DEFAULT_TM);
            ui->ctCheckBox->setChecked(CMUCAM4_DEFAULT_CT);

            // Camera Manual Control Box

            ui->brightnessSlider->setValue(CMUCAM4_DEFAULT_BRIGHTNESS);
            ui->contrastSlider->setValue(CMUCAM4_DEFAULT_CONTRAST);

            latchedBrightnessValue = CMUCAM4_DEFAULT_BRIGHTNESS;
            latchedContrastValue = CMUCAM4_DEFAULT_CONTRAST;

            // Register Control Box

            ui->regAddressSpinBox->setValue(CMUCAM4_DEFAULT_REG_ADDRESS);
            ui->regValueSpinBox->setValue(CMUCAM4_DEFAULT_REG_VALUE);

            latchedRegAddress = CMUCAM4_DEFAULT_REG_ADDRESS;
            latchedRegValue = CMUCAM4_DEFAULT_REG_VALUE;

            // Status Control Box

            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);

            ui->userButtonStateValue->setNum(CMUCAM4_DEFAULT_SC_UBS);
            ui->userButtonDurationValue->setNum(CMUCAM4_DEFAULT_SC_UBD);
            ui->userButtonPressedValue->setNum(CMUCAM4_DEFAULT_SC_UBP);
            ui->userButtonReleasedValue->setNum(CMUCAM4_DEFAULT_SC_UBR);
            ui->panInputValue->setNum(CMUCAM4_DEFAULT_SC_PI);
            ui->tiltInputValue->setNum(CMUCAM4_DEFAULT_SC_TI);
            ui->panServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_PSP);
            ui->tiltServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_TSP);

            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::sleepDeeply()
{
    if(lockGraphicalUserInterface(tr("Sleep Deeply Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(sleepDeeplyWrapper);

        if(ranGraphicalUserInterface(tr("Sleep Deeply Error")))
        {
            // Camera Auto Control Box

            ui->agcCheckBox->setChecked(CMUCAM4_DEFAULT_AGC);
            ui->awbCheckBox->setChecked(CMUCAM4_DEFAULT_AWB);
            ui->hmCheckBox->setChecked(CMUCAM4_DEFAULT_HM);
            ui->vfCheckBox->setChecked(CMUCAM4_DEFAULT_VF);
            ui->bwCheckBox->setChecked(CMUCAM4_DEFAULT_BW);
            ui->ngCheckBox->setChecked(CMUCAM4_DEFAULT_NG);
            ui->tmCheckBox->setChecked(CMUCAM4_DEFAULT_TM);
            ui->ctCheckBox->setChecked(CMUCAM4_DEFAULT_CT);

            // Camera Manual Control Box

            ui->brightnessSlider->setValue(CMUCAM4_DEFAULT_BRIGHTNESS);
            ui->contrastSlider->setValue(CMUCAM4_DEFAULT_CONTRAST);

            latchedBrightnessValue = CMUCAM4_DEFAULT_BRIGHTNESS;
            latchedContrastValue = CMUCAM4_DEFAULT_CONTRAST;

            // Register Control Box

            ui->regAddressSpinBox->setValue(CMUCAM4_DEFAULT_REG_ADDRESS);
            ui->regValueSpinBox->setValue(CMUCAM4_DEFAULT_REG_VALUE);

            latchedRegAddress = CMUCAM4_DEFAULT_REG_ADDRESS;
            latchedRegValue = CMUCAM4_DEFAULT_REG_VALUE;

            // Status Control Box

            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);

            ui->userButtonStateValue->setNum(CMUCAM4_DEFAULT_SC_UBS);
            ui->userButtonDurationValue->setNum(CMUCAM4_DEFAULT_SC_UBD);
            ui->userButtonPressedValue->setNum(CMUCAM4_DEFAULT_SC_UBP);
            ui->userButtonReleasedValue->setNum(CMUCAM4_DEFAULT_SC_UBR);
            ui->panInputValue->setNum(CMUCAM4_DEFAULT_SC_PI);
            ui->tiltInputValue->setNum(CMUCAM4_DEFAULT_SC_TI);
            ui->panServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_PSP);
            ui->tiltServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_TSP);

            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::dumpBitmap()
{
    if(lockGraphicalUserInterface(tr("Dump Bitmap Error")))
    {
        GUIFuture = QtConcurrent::run(dumpBitmapWrapper);

        bool okay = ranGraphicalUserInterface(tr("Dump Bitmap Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::dumpFrame()
{
    if(lockGraphicalUserInterface(tr("Dump Frame Error")))
    {
        GUIFuture = QtConcurrent::run(dumpFrameWrapper,
                                      ui->dfHResComboBox->currentIndex(),
                                      ui->dfVResComboBox->currentIndex());

        bool okay = ranGraphicalUserInterface(tr("Dump Frame Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::sendBitmap()
{
    if(lockGraphicalUserInterface(tr("Send Bitmap Error")))
    {
        CMUcam4_image_data_t buffer;

        GUIFuture = QtConcurrent::run(sendBitmapWrapper,
                                      &buffer);

        if(ranGraphicalUserInterface(tr("Send Bitmap Error")))
        {
            QPixmap pixmap(CMUCAM4_BINARY_H_RES, CMUCAM4_BINARY_V_RES);
            QPainter painter(&pixmap);

            for(int y = CMUCAM4_MIN_BINARY_ROW;
            y <= CMUCAM4_MAX_BINARY_ROW; y++)
            {
                for(int x = CMUCAM4_MIN_BINARY_COLUMN;
                x <= CMUCAM4_MAX_BINARY_COLUMN; x++)
                {
                    painter.setPen(QColor(CMUCAM4_GET_PIXEL(&buffer,
                    y, x) ? Qt::blue : Qt::black));
                    painter.drawPoint(x, y);
                }
            }

            painter.end();
            pixmap.setMask(pixmap.createMaskFromColor(QColor(Qt::black)));

            SBPixmap->setPixmap(pixmap);
            ui->actionSave_Bitmap->setEnabled(true);
            ui->actionClear_Bitmap->setEnabled(true);
            ui->sentBitmapOpacitySpinBox->setEnabled(true);

            if(!SFPixmap->pixmap().isNull())
            {
                SBPixmap->setTransform(QTransform::fromScale(
                SFPixmap->boundingRect().width() /
                SBPixmap->boundingRect().width(),
                SFPixmap->boundingRect().height() /
                SBPixmap->boundingRect().height()));
            }
            else
            {
                SBPixmap->setTransform(QTransform());
            }

            SFSBView->setSceneRect(SBPixmap->sceneBoundingRect());
            ui->imageView->fitInView(SFSBView->sceneRect(),
                                     Qt::KeepAspectRatio);
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::sendFrame()
{
    if(lockGraphicalUserInterface(tr("Send Frame Error")))
    {
        int horizontalScale = ui->sfHResComboBox->currentIndex();
        int verticalScale = ui->sfVResComboBox->currentIndex();

        int horizontalResolution = (CMUCAM4_FRAME_H_RES >> horizontalScale);
        int verticalResolution = (CMUCAM4_FRAME_V_RES >> verticalScale);

        uint16_t buffer[verticalResolution][horizontalResolution];

        GUIFuture = QtConcurrent::run(sendFrameWrapper,
                                      horizontalScale,
                                      verticalScale,
                                      (uint16_t *) buffer,
                                      (size_t) horizontalResolution,
                                      (size_t) verticalResolution);

        if(ranGraphicalUserInterface(tr("Send Frame Error")))
        {
            QPixmap pixmap(horizontalResolution, verticalResolution);
            QPainter painter(&pixmap);

            for(int y = 0; y < verticalResolution; y++)
            {
                for(int x = 0; x < horizontalResolution; x++)
                {
                    painter.setPen(QColor(
                    ((((buffer[y][x] >> 11) & 0x1F) * 255) / 31),
                    ((((buffer[y][x] >> 05) & 0x3F) * 255) / 63),
                    ((((buffer[y][x] >> 00) & 0x1F) * 255) / 31)));
                    painter.drawPoint(x, y);
                }
            }

            SFPixmap->setPixmap(pixmap);
            ui->actionSave_Frame->setEnabled(true);
            ui->actionClear_Frame->setEnabled(true);
            ui->sentFrameOpacitySpinBox->setEnabled(true);

            SFRect->setRect(QRect());
            ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
            ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);
            ui->imageView->setDragMode(QGraphicsView::RubberBandDrag);

            if(!SBPixmap->pixmap().isNull())
            {
                SBPixmap->setTransform(QTransform::fromScale(
                SFPixmap->boundingRect().width() /
                SBPixmap->boundingRect().width(),
                SFPixmap->boundingRect().height() /
                SBPixmap->boundingRect().height()));
            }

            SFSBView->setSceneRect(SFPixmap->sceneBoundingRect());
            ui->imageView->fitInView(SFSBView->sceneRect(),
                                     Qt::KeepAspectRatio);
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),
    QDir::cleanPath(QDir::homePath() + QDir::separator() + tr("untitled.bmp")),
    tr("Image Format (*.bmp *.jpg *.jpeg *.png *.ppm *.tiff *.xbm *.xpm)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Open Image Error"),
            tr("Invalid filename"));
        }
        else
        {
            QPixmap pixmap;

            if(pixmap.load(fileName))
            {
                SFPixmap->setPixmap(pixmap);
                ui->actionSave_Frame->setEnabled(true);
                ui->actionClear_Frame->setEnabled(true);
                ui->sentFrameOpacitySpinBox->setEnabled(true);

                SFRect->setRect(QRect());
                ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
                ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);
                ui->imageView->setDragMode(QGraphicsView::RubberBandDrag);

                if(!SBPixmap->pixmap().isNull())
                {
                    SBPixmap->setTransform(QTransform::fromScale(
                    SFPixmap->boundingRect().width() /
                    SBPixmap->boundingRect().width(),
                    SFPixmap->boundingRect().height() /
                    SBPixmap->boundingRect().height()));
                }

                SFSBView->setSceneRect(SFPixmap->sceneBoundingRect());
                ui->imageView->fitInView(SFSBView->sceneRect(),
                                         Qt::KeepAspectRatio);
            }
            else
            {
                QMessageBox::critical(this, tr("Open Image Error"),
                tr("An unknown error occured"));
            }
        }
    }
}

void MainWindow::clearBitmap()
{
    SBPixmap->setPixmap(QPixmap());
    ui->actionSave_Bitmap->setEnabled(CMUCAM4_DEFAULT_SAVE_SB);
    ui->actionClear_Bitmap->setEnabled(CMUCAM4_DEFAULT_CLEAR_SB);
    ui->sentBitmapOpacitySpinBox->setEnabled(CMUCAM4_DEFAULT_BO);
}

void MainWindow::clearFrame()
{
    SFPixmap->setPixmap(QPixmap());
    ui->actionSave_Frame->setEnabled(CMUCAM4_DEFAULT_SAVE_SF);
    ui->actionClear_Frame->setEnabled(CMUCAM4_DEFAULT_CLEAR_SF);
    ui->sentFrameOpacitySpinBox->setEnabled(CMUCAM4_DEFAULT_FO);

    SFRect->setRect(QRect());
    ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
    ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);
    ui->imageView->setDragMode(QGraphicsView::NoDrag);

    if(!SBPixmap->pixmap().isNull())
    {
        SBPixmap->setTransform(QTransform());
        SFSBView->setSceneRect(SBPixmap->sceneBoundingRect());
        ui->imageView->fitInView(SFSBView->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::saveBitmap()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Bitmap"),
    QDir::cleanPath(QDir::homePath() + QDir::separator() + tr("untitled.bmp")),
    tr("Image Format (*.bmp *.jpg *.jpeg *.png *.ppm *.tiff *.xbm *.xpm)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save Bitmap Error"),
            tr("Invalid filename"));
        }
        else if(!SBPixmap->pixmap().save(fileName))
        {
            QMessageBox::critical(this, tr("Save Bitmap Error"),
            tr("An unknown error occured"));
        }
    }
}

void MainWindow::saveFrame()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Frame"),
    QDir::cleanPath(QDir::homePath() + QDir::separator() + tr("untitled.bmp")),
    tr("Image Format (*.bmp *.jpg *.jpeg *.png *.ppm *.tiff *.xbm *.xpm)"));

    if(!fileName.isNull())
    {
        if(fileName.isEmpty())
        {
            QMessageBox::critical(this, tr("Save Frame Error"),
            tr("Invalid filename"));
        }
        else if(!SFPixmap->pixmap().save(fileName))
        {
            QMessageBox::critical(this, tr("Save Frame Error"),
            tr("An unknown error occured"));
        }
    }
}

void MainWindow::sentBitmapOpacity(double opacity)
{
    SBPixmap->setOpacity(opacity);
}

void MainWindow::sentFrameOpacity(double opacity)
{
    SFPixmap->setOpacity(opacity);
}

void MainWindow::trackSelection()
{
    bool okay;

    qreal ci = QInputDialog::getDouble(this, tr("Enter the Confidence Level"),
    tr("Confidence Interval Level coefficient"), 50, 0, 99, 0, &okay,
    Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    if(okay)
    {
        QPixmap pixmap = SFPixmap->pixmap();
        QImage image = pixmap.toImage().copy(SFRect->rect().toRect());

        // Calculate color bounds using Chebyshev's Inequality

        qreal width = ((qreal) image.width());
        qreal height = ((qreal) image.height());

        qreal k = qSqrt(1.00 / (1.00 - (ci / 100.0)));
        qreal n = width * height;

        qreal redMean = 0.0;
        qreal greenMean = 0.0;
        qreal blueMean = 0.0;

        qreal redStdev = 0.0;
        qreal greenStdev = 0.0;
        qreal blueStdev = 0.0;

        // Calculate arthimetic means

        for(int y = 0; y < ((int) height); y++)
        {
            for(int x = 0; x < ((int) width); x++)
            {
                int pixel = image.pixel(x, y);

                redMean += ((qreal) qRed(pixel));
                greenMean += ((qreal) qGreen(pixel));
                blueMean += ((qreal) qBlue(pixel));
            }
        }

        redMean /= n;
        greenMean /= n;
        blueMean /= n;

        // Calculate standard deviations

        for(int y = 0; y < ((int) height); y++)
        {
            for(int x = 0; x < ((int) width); x++)
            {
                int pixel = image.pixel(x, y);

                redStdev += qPow(((qreal) qRed(pixel)) - redMean, 2.0);
                greenStdev += qPow(((qreal) qGreen(pixel)) - greenMean, 2.0);
                blueStdev += qPow(((qreal) qBlue(pixel)) - blueMean, 2.0);
            }
        }

        redStdev = qSqrt(redStdev / n);
        greenStdev = qSqrt(greenStdev / n);
        blueStdev = qSqrt(blueStdev / n);

        // Calculate Chebyshev's Inequality

        int redMin = qMax(qFloor(redMean - (k * redStdev)),
                          CMUCAM4_DEFAULT_RGB_MIN);

        int redMax = qMin(qCeil(redMean + (k * redStdev)),
                          CMUCAM4_DEFAULT_RGB_MAX);

        int greenMin = qMax(qFloor(greenMean - (k * greenStdev)),
                            CMUCAM4_DEFAULT_RGB_MIN);

        int greenMax = qMin(qCeil(greenMean + (k * greenStdev)),
                            CMUCAM4_DEFAULT_RGB_MAX);

        int blueMin = qMax(qFloor(blueMean - (k * blueStdev)),
                           CMUCAM4_DEFAULT_RGB_MIN);

        int blueMax = qMin(qCeil(blueMean + (k * blueStdev)),
                           CMUCAM4_DEFAULT_RGB_MAX);

        ui->redMinSpinBox->setValue(redMin);
        ui->redMaxSpinBox->setValue(redMax);
        ui->greenMinSpinBox->setValue(greenMin);
        ui->greenMaxSpinBox->setValue(greenMax);
        ui->blueMinSpinBox->setValue(blueMin);
        ui->blueMaxSpinBox->setValue(blueMax);

        setTrackingParametersApply();
        sendBitmap();
    }
}

void MainWindow::clearSelection()
{
    clearBitmap();

    SFRect->setRect(QRect());
    ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
    ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);
}

void MainWindow::changedSelection()
{
    if(SFPath != SFSBView->selectionArea())
    {
        SFPath = SFSBView->selectionArea();

        SFRect->setRect(QRectF(SFPixmap->mapFromScene(SFPath).boundingRect().
        normalized().toAlignedRect().intersected(SFPixmap->boundingRect().
        normalized().toAlignedRect())));

        ui->trackSelectionButton->setEnabled(true);
        ui->clearSelectionButton->setEnabled(true);
    }
    else
    {
        SFRect->setRect(QRect());
        ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
        ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);
    }
}

void MainWindow::automaticGainControl()
{
    if(lockGraphicalUserInterface(tr("Automatic Gain Control Error")))
    {
        GUIFuture = QtConcurrent::run(autoGainControlWrapper,
                                      ui->agcCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic Gain Control Error")))
        {
            ui->agcCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->agcCheckBox->toggle();
    }
}

void MainWindow::automaticWhiteBalance()
{
    if(lockGraphicalUserInterface(tr("Automatic White Balance Error")))
    {
        GUIFuture = QtConcurrent::run(autoWhiteBalanceWrapper,
                                      ui->awbCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic White Balance Error")))
        {
            ui->awbCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->awbCheckBox->toggle();
    }
}

void MainWindow::horizontalMirror()
{
    if(lockGraphicalUserInterface(tr("Horizontal Mirror Error")))
    {
        GUIFuture = QtConcurrent::run(horizontalMirrorWrapper,
                                      ui->hmCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Horizontal Mirror Error")))
        {
            ui->hmCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->hmCheckBox->toggle();
    }
}

void MainWindow::verticalFlip()
{
    if(lockGraphicalUserInterface(tr("Vertical Flip Error")))
    {
        GUIFuture = QtConcurrent::run(verticalFlipWrapper,
                                      ui->vfCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Vertical Flip Error")))
        {
            ui->vfCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->vfCheckBox->toggle();
    }
}

void MainWindow::blackAndWhiteMode()
{
    if(lockGraphicalUserInterface(tr("Black And White Mode Error")))
    {
        GUIFuture = QtConcurrent::run(blackAndWhiteModeWrapper,
                                      ui->bwCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Black And White Mode Error")))
        {
            ui->bwCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->bwCheckBox->toggle();
    }
}

void MainWindow::negativeMode()
{
    if(lockGraphicalUserInterface(tr("Negative Mode Error")))
    {
        GUIFuture = QtConcurrent::run(negativeModeWrapper,
                                      ui->ngCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Negative Mode Error")))
        {
            ui->ngCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ngCheckBox->toggle();
    }
}

void MainWindow::testMode()
{
    if(lockGraphicalUserInterface(tr("Test Mode Error")))
    {
        GUIFuture = QtConcurrent::run(testModeWrapper,
                                      ui->tmCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Test Mode Error")))
        {
            ui->tmCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->tmCheckBox->toggle();
    }
}

void MainWindow::colorTracking()
{
    if(lockGraphicalUserInterface(tr("Color Tracking Error")))
    {
        GUIFuture = QtConcurrent::run(colorTrackingWrapper,
                                      ui->ctCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Color Tracking Error")))
        {
            ui->ctCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ctCheckBox->toggle();
    }
}

void MainWindow::brightnessApply()
{
    if(lockGraphicalUserInterface(tr("Camera Brightness Apply Error")))
    {
        int tempLatchedBrightnessValue = latchedBrightnessValue;
        latchedBrightnessValue = ui->brightnessSlider->value();

        GUIFuture = QtConcurrent::run(cameraBrightnessWrapper,
                                      ui->brightnessSlider->value());

        if(!ranGraphicalUserInterface(tr("Camera Brightness Apply Error")))
        {
            ui->brightnessSlider->setValue(tempLatchedBrightnessValue);
            latchedBrightnessValue = tempLatchedBrightnessValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->brightnessSlider->setValue(latchedBrightnessValue);
    }
}

void MainWindow::brightnessReset()
{
    if(lockGraphicalUserInterface(tr("Camera Brightness Reset Error")))
    {
        ui->brightnessSlider->setValue(CMUCAM4_DEFAULT_BRIGHTNESS);

        int tempLatchedBrightnessValue = latchedBrightnessValue;
        latchedBrightnessValue = ui->brightnessSlider->value();

        GUIFuture = QtConcurrent::run(cameraBrightnessWrapper,
                                      ui->brightnessSlider->value());

        if(!ranGraphicalUserInterface(tr("Camera Brightness Reset Error")))
        {
            ui->brightnessSlider->setValue(tempLatchedBrightnessValue);
            latchedBrightnessValue = tempLatchedBrightnessValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->brightnessSlider->setValue(latchedBrightnessValue);
    }
}

void MainWindow::contrastApply()
{
    if(lockGraphicalUserInterface(tr("Camera Contrast Apply Error")))
    {
        int tempLatchedContrastValue = latchedContrastValue;
        latchedContrastValue = ui->contrastSlider->value();

        GUIFuture = QtConcurrent::run(cameraContrastWrapper,
                                      ui->contrastSlider->value());

        if(!ranGraphicalUserInterface(tr("Camera Contrast Apply Error")))
        {
            ui->contrastSlider->setValue(tempLatchedContrastValue);
            latchedContrastValue = tempLatchedContrastValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->contrastSlider->setValue(latchedContrastValue);
    }
}

void MainWindow::contrastReset()
{
    if(lockGraphicalUserInterface(tr("Camera Contrast Reset Error")))
    {
        ui->contrastSlider->setValue(CMUCAM4_DEFAULT_CONTRAST);

        int tempLatchedContrastValue = latchedContrastValue;
        latchedContrastValue = ui->contrastSlider->value();

        GUIFuture = QtConcurrent::run(cameraContrastWrapper,
                                      ui->contrastSlider->value());

        if(!ranGraphicalUserInterface(tr("Camera Contrast Reset Error")))
        {
            ui->contrastSlider->setValue(tempLatchedContrastValue);
            latchedContrastValue = tempLatchedContrastValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->contrastSlider->setValue(latchedContrastValue);
    }
}

void MainWindow::panDialEnable()
{
    if(lockGraphicalUserInterface(tr("Set Pan Servo Position Error")))
    {
        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_PAN_SERVO,
                                      ui->panGroupBox->isChecked(),
                                      ui->panDial->value());

        if(!ranGraphicalUserInterface(tr("Set Pan Servo Position Error")))
        {
            ui->panGroupBox->setChecked(!ui->panGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->panGroupBox->setChecked(!ui->panGroupBox->isChecked());
    }
}

void MainWindow::panDialApply()
{
    if(lockGraphicalUserInterface(tr("Pan Servo Position Apply Error")))
    {
        int tempLatchedPanPulseLengthValue = latchedPanPulseLengthValue;
        latchedPanPulseLengthValue = ui->panDial->value();

        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_PAN_SERVO,
                                      ui->panGroupBox->isChecked(),
                                      ui->panDial->value());

        if(!ranGraphicalUserInterface(tr("Pan Servo Position Apply Error")))
        {
            ui->panDial->setValue(tempLatchedPanPulseLengthValue);
            latchedPanPulseLengthValue = tempLatchedPanPulseLengthValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->panDial->setValue(latchedPanPulseLengthValue);
    }
}

void MainWindow::panDialReset()
{
    if(lockGraphicalUserInterface(tr("Pan Servo Position Reset Error")))
    {
        ui->panDial->setValue(CMUCAM4_DEFAULT_PAN_PULSE_LENGTH);

        int tempLatchedPanPulseLengthValue = latchedPanPulseLengthValue;
        latchedPanPulseLengthValue = ui->panDial->value();

        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_PAN_SERVO,
                                      ui->panGroupBox->isChecked(),
                                      ui->panDial->value());

        if(!ranGraphicalUserInterface(tr("Pan Servo Position Reset Error")))
        {
            ui->panDial->setValue(tempLatchedPanPulseLengthValue);
            latchedPanPulseLengthValue = tempLatchedPanPulseLengthValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->panDial->setValue(latchedPanPulseLengthValue);
    }
}

void MainWindow::tiltDialEnable()
{
    if(lockGraphicalUserInterface(tr("Set Tilt Servo Position Error")))
    {
        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_TILT_SERVO,
                                      ui->tiltGroupBox->isChecked(),
                                      ui->tiltDial->value());

        if(!ranGraphicalUserInterface(tr("Set Tilt Servo Position Error")))
        {
            ui->tiltGroupBox->setChecked(!ui->tiltGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->tiltGroupBox->setChecked(!ui->tiltGroupBox->isChecked());
    }
}

void MainWindow::tiltDialApply()
{
    if(lockGraphicalUserInterface(tr("Tilt Servo Position Apply Error")))
    {
        int tempLatchedTiltPulseLengthValue = latchedTiltPulseLengthValue;
        latchedTiltPulseLengthValue = ui->tiltDial->value();

        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_TILT_SERVO,
                                      ui->tiltGroupBox->isChecked(),
                                      ui->tiltDial->value());

        if(!ranGraphicalUserInterface(tr("Tilt Servo Position Apply Error")))
        {
            ui->tiltDial->setValue(tempLatchedTiltPulseLengthValue);
            latchedTiltPulseLengthValue = tempLatchedTiltPulseLengthValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->tiltDial->setValue(latchedTiltPulseLengthValue);
    }
}

void MainWindow::tiltDialReset()
{
    if(lockGraphicalUserInterface(tr("Tilt Servo Position Reset Error")))
    {
        ui->tiltDial->setValue(CMUCAM4_DEFAULT_TILT_PULSE_LENGTH);

        int tempLatchedTiltPulseLengthValue = latchedTiltPulseLengthValue;
        latchedTiltPulseLengthValue = ui->tiltDial->value();

        GUIFuture = QtConcurrent::run(setServoPositionWrapper,
                                      CMUCAM4_TILT_SERVO,
                                      ui->tiltGroupBox->isChecked(),
                                      ui->tiltDial->value());

        if(!ranGraphicalUserInterface(tr("Tilt Servo Position Reset Error")))
        {
            ui->tiltDial->setValue(tempLatchedTiltPulseLengthValue);
            latchedTiltPulseLengthValue = tempLatchedTiltPulseLengthValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->tiltDial->setValue(latchedTiltPulseLengthValue);
    }
}


void MainWindow::setTrackingParametersApply()
{
    if(lockGraphicalUserInterface(tr("Tracking Parameters Apply Error")))
    {
        int tempLatchedRedMinValue = latchedRedMinValue;
        int tempLatchedRedMaxValue = latchedRedMaxValue;
        int tempLatchedGreenMinValue = latchedGreenMinValue;
        int tempLatchedGreenMaxValue = latchedGreenMaxValue;
        int tempLatchedBlueMinValue = latchedBlueMinValue;
        int tempLatchedBlueMaxValue = latchedBlueMaxValue;
        latchedRedMinValue = ui->redMinSpinBox->value();
        latchedRedMaxValue = ui->redMaxSpinBox->value();
        latchedGreenMinValue = ui->greenMinSpinBox->value();
        latchedGreenMaxValue = ui->greenMaxSpinBox->value();
        latchedBlueMinValue = ui->blueMinSpinBox->value();
        latchedBlueMaxValue = ui->blueMaxSpinBox->value();

        int min[] = {ui->redMinSpinBox->value(),
                     ui->greenMinSpinBox->value(),
                     ui->blueMinSpinBox->value()};

        int max[] = {ui->redMaxSpinBox->value(),
                     ui->greenMaxSpinBox->value(),
                     ui->blueMaxSpinBox->value()};

        GUIFuture = QtConcurrent::run(setTrackingParametersWrapper,
                                      (int *) min,
                                      (int *) max);

        if(!ranGraphicalUserInterface(tr("Tracking Parameters Apply Error")))
        {
            ui->redMinSpinBox->setValue(tempLatchedRedMinValue);
            ui->redMaxSpinBox->setValue(tempLatchedRedMaxValue);
            ui->greenMinSpinBox->setValue(tempLatchedGreenMinValue);
            ui->greenMaxSpinBox->setValue(tempLatchedGreenMaxValue);
            ui->blueMinSpinBox->setValue(tempLatchedBlueMinValue);
            ui->blueMaxSpinBox->setValue(tempLatchedBlueMaxValue);
            latchedRedMinValue = tempLatchedRedMinValue;
            latchedRedMaxValue = tempLatchedRedMaxValue;
            latchedGreenMinValue = tempLatchedGreenMinValue;
            latchedGreenMaxValue = tempLatchedGreenMaxValue;
            latchedBlueMinValue = tempLatchedBlueMinValue;
            latchedBlueMaxValue = tempLatchedBlueMaxValue;
        }
        else
        {
            ui->mfCheckBox->setChecked(false);
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->redMinSpinBox->setValue(latchedRedMinValue);
        ui->redMaxSpinBox->setValue(latchedRedMaxValue);
        ui->greenMinSpinBox->setValue(latchedGreenMinValue);
        ui->greenMaxSpinBox->setValue(latchedGreenMaxValue);
        ui->blueMinSpinBox->setValue(latchedBlueMinValue);
        ui->blueMaxSpinBox->setValue(latchedBlueMaxValue);
    }
}

void MainWindow::setTrackingParametersReset()
{
    if(lockGraphicalUserInterface(tr("Tracking Parameters Reset Error")))
    {
        ui->redMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
        ui->redMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);
        ui->greenMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
        ui->greenMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);
        ui->blueMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
        ui->blueMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);

        int tempLatchedRedMinValue = latchedRedMinValue;
        int tempLatchedRedMaxValue = latchedRedMaxValue;
        int tempLatchedGreenMinValue = latchedGreenMinValue;
        int tempLatchedGreenMaxValue = latchedGreenMaxValue;
        int tempLatchedBlueMinValue = latchedBlueMinValue;
        int tempLatchedBlueMaxValue = latchedBlueMaxValue;
        latchedRedMinValue = ui->redMinSpinBox->value();
        latchedRedMaxValue = ui->redMaxSpinBox->value();
        latchedGreenMinValue = ui->greenMinSpinBox->value();
        latchedGreenMaxValue = ui->greenMaxSpinBox->value();
        latchedBlueMinValue = ui->blueMinSpinBox->value();
        latchedBlueMaxValue = ui->blueMaxSpinBox->value();

        GUIFuture = QtConcurrent::run(setTrackingParametersWrapper);

        if(!ranGraphicalUserInterface(tr("Tracking Parameters Reset Error")))
        {
            ui->redMinSpinBox->setValue(tempLatchedRedMinValue);
            ui->redMaxSpinBox->setValue(tempLatchedRedMaxValue);
            ui->greenMinSpinBox->setValue(tempLatchedGreenMinValue);
            ui->greenMaxSpinBox->setValue(tempLatchedGreenMaxValue);
            ui->blueMinSpinBox->setValue(tempLatchedBlueMinValue);
            ui->blueMaxSpinBox->setValue(tempLatchedBlueMaxValue);
            latchedRedMinValue = tempLatchedRedMinValue;
            latchedRedMaxValue = tempLatchedRedMaxValue;
            latchedGreenMinValue = tempLatchedGreenMinValue;
            latchedGreenMaxValue = tempLatchedGreenMaxValue;
            latchedBlueMinValue = tempLatchedBlueMinValue;
            latchedBlueMaxValue = tempLatchedBlueMaxValue;
        }
        else
        {
            ui->mfCheckBox->setChecked(false);
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->redMinSpinBox->setValue(latchedRedMinValue);
        ui->redMaxSpinBox->setValue(latchedRedMaxValue);
        ui->greenMinSpinBox->setValue(latchedGreenMinValue);
        ui->greenMaxSpinBox->setValue(latchedGreenMaxValue);
        ui->blueMinSpinBox->setValue(latchedBlueMinValue);
        ui->blueMaxSpinBox->setValue(latchedBlueMaxValue);
    }
}

void MainWindow::getTrackingParameters()
{
    if(lockGraphicalUserInterface(tr("Get Tracking Parameters Error")))
    {
        int tempLatchedRedMinValue = latchedRedMinValue;
        int tempLatchedRedMaxValue = latchedRedMaxValue;
        int tempLatchedGreenMinValue = latchedGreenMinValue;
        int tempLatchedGreenMaxValue = latchedGreenMaxValue;
        int tempLatchedBlueMinValue = latchedBlueMinValue;
        int tempLatchedBlueMaxValue = latchedBlueMaxValue;
        latchedRedMinValue = ui->redMinSpinBox->value();
        latchedRedMaxValue = ui->redMaxSpinBox->value();
        latchedGreenMinValue = ui->greenMinSpinBox->value();
        latchedGreenMaxValue = ui->greenMaxSpinBox->value();
        latchedBlueMinValue = ui->blueMinSpinBox->value();
        latchedBlueMaxValue = ui->blueMaxSpinBox->value();

        CMUcam4_tracking_parameters_t parameters;

        GUIFuture = QtConcurrent::run(getTrackingParametersWrapper,
                                      &parameters);

        if(!ranGraphicalUserInterface(tr("Get Tracking Parameters Error")))
        {
            ui->redMinSpinBox->setValue(tempLatchedRedMinValue);
            ui->redMaxSpinBox->setValue(tempLatchedRedMaxValue);
            ui->greenMinSpinBox->setValue(tempLatchedGreenMinValue);
            ui->greenMaxSpinBox->setValue(tempLatchedGreenMaxValue);
            ui->blueMinSpinBox->setValue(tempLatchedBlueMinValue);
            ui->blueMaxSpinBox->setValue(tempLatchedBlueMaxValue);
            latchedRedMinValue = tempLatchedRedMinValue;
            latchedRedMaxValue = tempLatchedRedMaxValue;
            latchedGreenMinValue = tempLatchedGreenMinValue;
            latchedGreenMaxValue = tempLatchedGreenMaxValue;
            latchedBlueMinValue = tempLatchedBlueMinValue;
            latchedBlueMaxValue = tempLatchedBlueMaxValue;
        }
        else
        {
            ui->redMinSpinBox->setValue(parameters.redMin);
            ui->redMaxSpinBox->setValue(parameters.redMax);
            ui->greenMinSpinBox->setValue(parameters.greenMin);
            ui->greenMaxSpinBox->setValue(parameters.greenMax);
            ui->blueMinSpinBox->setValue(parameters.blueMin);
            ui->blueMaxSpinBox->setValue(parameters.blueMax);
            latchedRedMinValue = parameters.redMin;
            latchedRedMaxValue = parameters.redMax;
            latchedGreenMinValue = parameters.greenMin;
            latchedGreenMaxValue = parameters.greenMax;
            latchedBlueMinValue = parameters.blueMin;
            latchedBlueMaxValue = parameters.blueMax;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->redMinSpinBox->setValue(latchedRedMinValue);
        ui->redMaxSpinBox->setValue(latchedRedMaxValue);
        ui->greenMinSpinBox->setValue(latchedGreenMinValue);
        ui->greenMaxSpinBox->setValue(latchedGreenMaxValue);
        ui->blueMinSpinBox->setValue(latchedBlueMinValue);
        ui->blueMaxSpinBox->setValue(latchedBlueMaxValue);
    }
}

void MainWindow::setTrackingWindowApply()
{
    if(lockGraphicalUserInterface(tr("Tracking Window Apply Error")))
    {
        int tempLatchedXMinValue = latchedXMinValue;
        int tempLatchedYMinValue = latchedYMinValue;
        int tempLatchedXMaxValue = latchedXMaxValue;
        int tempLatchedYMaxValue = latchedYMaxValue;
        latchedXMinValue = ui->xMinSpinBox->value();
        latchedYMinValue = ui->yMinSpinBox->value();
        latchedXMaxValue = ui->xMaxSpinBox->value();
        latchedYMaxValue = ui->yMaxSpinBox->value();

        GUIFuture = QtConcurrent::run(setTrackingWindowWrapper,
                                      ui->xMinSpinBox->value(),
                                      ui->yMinSpinBox->value(),
                                      ui->xMaxSpinBox->value(),
                                      ui->yMaxSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Tracking Window Apply Error")))
        {
            ui->xMinSpinBox->setValue(tempLatchedXMinValue);
            ui->yMinSpinBox->setValue(tempLatchedYMinValue);
            ui->xMaxSpinBox->setValue(tempLatchedXMaxValue);
            ui->yMaxSpinBox->setValue(tempLatchedYMaxValue);
            latchedXMinValue = tempLatchedXMinValue;
            latchedYMinValue = tempLatchedYMinValue;
            latchedXMaxValue = tempLatchedXMaxValue;
            latchedYMaxValue = tempLatchedYMaxValue;
        }
        else
        {
            ui->mfCheckBox->setChecked(false);
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->xMinSpinBox->setValue(latchedXMinValue);
        ui->yMinSpinBox->setValue(latchedYMinValue);
        ui->xMaxSpinBox->setValue(latchedXMaxValue);
        ui->yMaxSpinBox->setValue(latchedYMaxValue);
    }
}

void MainWindow::setTrackingWindowReset()
{
    if(lockGraphicalUserInterface(tr("Tracking Window Reset Error")))
    {
        ui->xMinSpinBox->setValue(CMUCAM4_MIN_NATIVE_COLUMN);
        ui->yMinSpinBox->setValue(CMUCAM4_MIN_NATIVE_ROW);
        ui->xMaxSpinBox->setValue(CMUCAM4_MAX_NATIVE_COLUMN);
        ui->yMaxSpinBox->setValue(CMUCAM4_MAX_NATIVE_ROW);

        int tempLatchedXMinValue = latchedXMinValue;
        int tempLatchedYMinValue = latchedYMinValue;
        int tempLatchedXMaxValue = latchedXMaxValue;
        int tempLatchedYMaxValue = latchedYMaxValue;
        latchedXMinValue = ui->xMinSpinBox->value();
        latchedYMinValue = ui->yMinSpinBox->value();
        latchedXMaxValue = ui->xMaxSpinBox->value();
        latchedYMaxValue = ui->yMaxSpinBox->value();

        GUIFuture = QtConcurrent::run(setTrackingWindowWrapper);

        if(!ranGraphicalUserInterface(tr("Tracking Window Reset Error")))
        {
            ui->xMinSpinBox->setValue(tempLatchedXMinValue);
            ui->yMinSpinBox->setValue(tempLatchedYMinValue);
            ui->xMaxSpinBox->setValue(tempLatchedXMaxValue);
            ui->yMaxSpinBox->setValue(tempLatchedYMaxValue);
            latchedXMinValue = tempLatchedXMinValue;
            latchedYMinValue = tempLatchedYMinValue;
            latchedXMaxValue = tempLatchedXMaxValue;
            latchedYMaxValue = tempLatchedYMaxValue;
        }
        else
        {
            ui->mfCheckBox->setChecked(false);
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->xMinSpinBox->setValue(latchedXMinValue);
        ui->yMinSpinBox->setValue(latchedYMinValue);
        ui->xMaxSpinBox->setValue(latchedXMaxValue);
        ui->yMaxSpinBox->setValue(latchedYMaxValue);
    }
}

void MainWindow::getTrackingWindow()
{
    if(lockGraphicalUserInterface(tr("Get Tracking Window Error")))
    {
        int tempLatchedXMinValue = latchedXMinValue;
        int tempLatchedYMinValue = latchedYMinValue;
        int tempLatchedXMaxValue = latchedXMaxValue;
        int tempLatchedYMaxValue = latchedYMaxValue;
        latchedXMinValue = ui->xMinSpinBox->value();
        latchedYMinValue = ui->yMinSpinBox->value();
        latchedXMaxValue = ui->xMaxSpinBox->value();
        latchedYMaxValue = ui->yMaxSpinBox->value();

        CMUcam4_tracking_window_t window;

        GUIFuture = QtConcurrent::run(getTrackingWindowWrapper,
                                      &window);

        if(!ranGraphicalUserInterface(tr("Get Tracking Window Error")))
        {
            ui->xMinSpinBox->setValue(tempLatchedXMinValue);
            ui->yMinSpinBox->setValue(tempLatchedYMinValue);
            ui->xMaxSpinBox->setValue(tempLatchedXMaxValue);
            ui->yMaxSpinBox->setValue(tempLatchedYMaxValue);
            latchedXMinValue = tempLatchedXMinValue;
            latchedYMinValue = tempLatchedYMinValue;
            latchedXMaxValue = tempLatchedXMaxValue;
            latchedYMaxValue = tempLatchedYMaxValue;
        }
        else
        {
            ui->xMinSpinBox->setValue(window.topLeftX);
            ui->yMinSpinBox->setValue(window.topLeftY);
            ui->xMaxSpinBox->setValue(window.bottomRightX);
            ui->yMaxSpinBox->setValue(window.bottomRightY);
            latchedXMinValue = window.topLeftX;
            latchedYMinValue = window.topLeftY;
            latchedXMaxValue = window.bottomRightX;
            latchedYMaxValue = window.bottomRightY;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->xMinSpinBox->setValue(latchedXMinValue);
        ui->yMinSpinBox->setValue(latchedYMinValue);
        ui->xMaxSpinBox->setValue(latchedXMaxValue);
        ui->yMaxSpinBox->setValue(latchedYMaxValue);
    }
}

void MainWindow::monitorOff()
{
    if(lockGraphicalUserInterface(tr("Monitor Off Error")))
    {
        bool tempLatchedM0State = latchedM0State;
        bool tempLatchedM1State = latchedM1State;
        latchedM0State = ui->m0RadioButton->isChecked();
        latchedM1State = ui->m1RadioButton->isChecked();

        GUIFuture = QtConcurrent::run(monitorOffWrapper);

        if(!ranGraphicalUserInterface(tr("Monitor Off Error")))
        {
            ui->m0RadioButton->setChecked(tempLatchedM0State);
            ui->m1RadioButton->setChecked(tempLatchedM1State);
            latchedM0State = tempLatchedM0State;
            latchedM1State = tempLatchedM1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->m0RadioButton->setChecked(latchedM0State);
        ui->m1RadioButton->setChecked(latchedM1State);
    }
}

void MainWindow::monitorOn()
{
    if(lockGraphicalUserInterface(tr("Monitor On Error")))
    {
        bool tempLatchedM0State = latchedM0State;
        bool tempLatchedM1State = latchedM1State;
        latchedM0State = ui->m0RadioButton->isChecked();
        latchedM1State = ui->m1RadioButton->isChecked();

        GUIFuture = QtConcurrent::run(monitorOnWrapper);

        if(!ranGraphicalUserInterface(tr("Monitor On Error")))
        {
            ui->m0RadioButton->setChecked(tempLatchedM0State);
            ui->m1RadioButton->setChecked(tempLatchedM1State);
            latchedM0State = tempLatchedM0State;
            latchedM1State = tempLatchedM1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->m0RadioButton->setChecked(latchedM0State);
        ui->m1RadioButton->setChecked(latchedM1State);
    }
}

void MainWindow::monitorFreeze()
{
    if(lockGraphicalUserInterface(tr("Monitor Freeze Error")))
    {
        GUIFuture = QtConcurrent::run(monitorFreezeWrapper,
                                      ui->mfCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Monitor Freeze Error")))
        {
            ui->mfCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->mfCheckBox->toggle();
    }
}

void MainWindow::monitorSignal()
{
    if(lockGraphicalUserInterface(tr("Monitor Signal Error")))
    {
        GUIFuture = QtConcurrent::run(monitorSignalWrapper,
                                      ui->msCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Monitor Signal Error")))
        {
            ui->msCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->msCheckBox->toggle();
    }
}

void MainWindow::automaticPan()
{
    if(lockGraphicalUserInterface(tr("Automatic Pan Error")))
    {
        GUIFuture = QtConcurrent::run(automaticPanWrapper,
                                      ui->APGroupBox->isChecked(),
                                      ui->APReversedCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic Pan Error")))
        {
            ui->APGroupBox->setChecked(!ui->APGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->APGroupBox->setChecked(!ui->APGroupBox->isChecked());
    }
}

void MainWindow::automaticPanReversed()
{
    if(lockGraphicalUserInterface(tr("Automatic Pan Reversed Error")))
    {
        GUIFuture = QtConcurrent::run(automaticPanWrapper,
                                      ui->APGroupBox->isChecked(),
                                      ui->APReversedCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic Pan Reversed Error")))
        {
            ui->APReversedCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->APReversedCheckBox->toggle();
    }
}

void MainWindow::automaticPanApply()
{
    if(lockGraphicalUserInterface(tr("Automatic Pan Apply Error")))
    {
        int tempLatchedPanPGainValue = latchedPanPGainValue;
        int tempLatchedPanDGainValue = latchedPanDGainValue;
        latchedPanPGainValue = ui->APPGainSpinBox->value();
        latchedPanDGainValue = ui->APDGainSpinBox->value();

        GUIFuture = QtConcurrent::run(autoPanParameters,
                                      ui->APPGainSpinBox->value(),
                                      ui->APDGainSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Automatic Pan Apply Error")))
        {
            ui->APPGainSpinBox->setValue(tempLatchedPanPGainValue);
            ui->APDGainSpinBox->setValue(tempLatchedPanDGainValue);
            latchedPanPGainValue = tempLatchedPanPGainValue;
            latchedPanDGainValue = tempLatchedPanDGainValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->APPGainSpinBox->setValue(latchedPanPGainValue);
        ui->APDGainSpinBox->setValue(latchedPanDGainValue);
    }
}

void MainWindow::automaticPanReset()
{
    if(lockGraphicalUserInterface(tr("Automatic Pan Reset Error")))
    {
        ui->APPGainSpinBox->setValue(CMUCAM4_DEFAULT_PAN_P_GAIN);
        ui->APDGainSpinBox->setValue(CMUCAM4_DEFAULT_PAN_D_GAIN);

        int tempLatchedPanPGainValue = latchedPanPGainValue;
        int tempLatchedPanDGainValue = latchedPanDGainValue;
        latchedPanPGainValue = ui->APPGainSpinBox->value();
        latchedPanDGainValue = ui->APDGainSpinBox->value();

        GUIFuture = QtConcurrent::run(autoPanParameters,
                                      ui->APPGainSpinBox->value(),
                                      ui->APDGainSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Automatic Pan Reset Error")))
        {
            ui->APPGainSpinBox->setValue(tempLatchedPanPGainValue);
            ui->APDGainSpinBox->setValue(tempLatchedPanDGainValue);
            latchedPanPGainValue = tempLatchedPanPGainValue;
            latchedPanDGainValue = tempLatchedPanDGainValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->APPGainSpinBox->setValue(latchedPanPGainValue);
        ui->APDGainSpinBox->setValue(latchedPanDGainValue);
    }
}

void MainWindow::automaticTilt()
{
    if(lockGraphicalUserInterface(tr("Automatic Tilt Error")))
    {
        GUIFuture = QtConcurrent::run(automaticTiltWrapper,
                                      ui->ATGroupBox->isChecked(),
                                      ui->ATReversedCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic Tilt Error")))
        {
            ui->ATGroupBox->setChecked(!ui->ATGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ATGroupBox->setChecked(!ui->ATGroupBox->isChecked());
    }
}

void MainWindow::automaticTiltReversed()
{
    if(lockGraphicalUserInterface(tr("Automatic Tilt Reversed Error")))
    {
        GUIFuture = QtConcurrent::run(automaticTiltWrapper,
                                      ui->ATGroupBox->isChecked(),
                                      ui->ATReversedCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Automatic Tilt Reversed Error")))
        {
            ui->ATReversedCheckBox->toggle();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ATReversedCheckBox->toggle();
    }
}

void MainWindow::automaticTiltApply()
{
    if(lockGraphicalUserInterface(tr("Automatic Tilt Apply Error")))
    {
        int tempLatchedTiltPGainValue = latchedTiltPGainValue;
        int tempLatchedTiltDGainValue = latchedTiltDGainValue;
        latchedTiltPGainValue = ui->ATPGainSpinBox->value();
        latchedTiltDGainValue = ui->ATDGainSpinBox->value();

        GUIFuture = QtConcurrent::run(autoTiltParameters,
                                      ui->ATPGainSpinBox->value(),
                                      ui->ATDGainSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Automatic Tilt Apply Error")))
        {
            ui->ATPGainSpinBox->setValue(tempLatchedTiltPGainValue);
            ui->ATDGainSpinBox->setValue(tempLatchedTiltDGainValue);
            latchedTiltPGainValue = tempLatchedTiltPGainValue;
            latchedTiltDGainValue = tempLatchedTiltDGainValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ATPGainSpinBox->setValue(latchedTiltPGainValue);
        ui->ATDGainSpinBox->setValue(latchedTiltDGainValue);
    }
}

void MainWindow::automaticTiltReset()
{
    if(lockGraphicalUserInterface(tr("Automatic Tilt Reset Error")))
    {
        ui->ATPGainSpinBox->setValue(CMUCAM4_DEFAULT_TILT_P_GAIN);
        ui->ATDGainSpinBox->setValue(CMUCAM4_DEFAULT_TILT_D_GAIN);

        int tempLatchedTiltPGainValue = latchedTiltPGainValue;
        int tempLatchedTiltDGainValue = latchedTiltDGainValue;
        latchedTiltPGainValue = ui->ATPGainSpinBox->value();
        latchedTiltDGainValue = ui->ATDGainSpinBox->value();

        GUIFuture = QtConcurrent::run(autoTiltParameters,
                                      ui->ATPGainSpinBox->value(),
                                      ui->ATDGainSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Automatic Tilt Reset Error")))
        {
            ui->ATPGainSpinBox->setValue(tempLatchedTiltPGainValue);
            ui->ATDGainSpinBox->setValue(tempLatchedTiltDGainValue);
            latchedTiltPGainValue = tempLatchedTiltPGainValue;
            latchedTiltDGainValue = tempLatchedTiltDGainValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->ATPGainSpinBox->setValue(latchedTiltPGainValue);
        ui->ATDGainSpinBox->setValue(latchedTiltDGainValue);
    }
}

void MainWindow::panPinOutput()
{
    if(lockGraphicalUserInterface(tr("Pan Output Error")))
    {
        GUIFuture = QtConcurrent::run(panOutputWrapper,
                                      ui->PPOGroupBox->isChecked(),
                                      ui->PPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Pan Output Error")))
        {
            ui->PPOGroupBox->setChecked(!ui->PPOGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->PPOGroupBox->setChecked(!ui->PPOGroupBox->isChecked());
    }
}

void MainWindow::panPinLow()
{
    if(lockGraphicalUserInterface(tr("Pan Output Low Error")))
    {
        bool tempLatchedP0State = latchedP0State;
        bool tempLatchedP1State = latchedP1State;
        latchedP0State = ui->PPOLowRadioButton->isChecked();
        latchedP1State = ui->PPOHighRadioButton->isChecked();

        GUIFuture = QtConcurrent::run(panOutputWrapper,
                                      ui->PPOGroupBox->isChecked(),
                                      ui->PPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Pan Output Low Error")))
        {
            ui->PPOLowRadioButton->setChecked(tempLatchedP0State);
            ui->PPOHighRadioButton->setChecked(tempLatchedP1State);
            latchedP0State = tempLatchedP0State;
            latchedP1State = tempLatchedP1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->PPOLowRadioButton->setChecked(latchedP0State);
        ui->PPOHighRadioButton->setChecked(latchedP1State);
    }
}

void MainWindow::panPinHigh()
{
    if(lockGraphicalUserInterface(tr("Pan Output High Error")))
    {
        bool tempLatchedP0State = latchedP0State;
        bool tempLatchedP1State = latchedP1State;
        latchedP0State = ui->PPOLowRadioButton->isChecked();
        latchedP1State = ui->PPOHighRadioButton->isChecked();

        GUIFuture = QtConcurrent::run(panOutputWrapper,
                                      ui->PPOGroupBox->isChecked(),
                                      ui->PPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Pan Output High Error")))
        {
            ui->PPOLowRadioButton->setChecked(tempLatchedP0State);
            ui->PPOHighRadioButton->setChecked(tempLatchedP1State);
            latchedP0State = tempLatchedP0State;
            latchedP1State = tempLatchedP1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->PPOLowRadioButton->setChecked(latchedP0State);
        ui->PPOHighRadioButton->setChecked(latchedP1State);
    }
}

void MainWindow::tiltPinOutput()
{
    if(lockGraphicalUserInterface(tr("Tilt Output Error")))
    {
        GUIFuture = QtConcurrent::run(tiltOutputWrapper,
                                      ui->TPOGroupBox->isChecked(),
                                      ui->TPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Tilt Output Error")))
        {
            ui->TPOGroupBox->setChecked(!ui->TPOGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->TPOGroupBox->setChecked(!ui->TPOGroupBox->isChecked());
    }
}

void MainWindow::tiltPinLow()
{
    if(lockGraphicalUserInterface(tr("Tilt Output Low Error")))
    {
        bool tempLatchedT0State = latchedT0State;
        bool tempLatchedT1State = latchedT1State;
        latchedT0State = ui->TPOLowRadioButton->isChecked();
        latchedT1State = ui->TPOHighRadioButton->isChecked();

        GUIFuture = QtConcurrent::run(tiltOutputWrapper,
                                      ui->TPOGroupBox->isChecked(),
                                      ui->TPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Tilt Output Low Error")))
        {
            ui->TPOLowRadioButton->setChecked(tempLatchedT0State);
            ui->TPOHighRadioButton->setChecked(tempLatchedT1State);
            latchedT0State = tempLatchedT0State;
            latchedT1State = tempLatchedT1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->TPOLowRadioButton->setChecked(latchedT0State);
        ui->TPOHighRadioButton->setChecked(latchedT1State);
    }
}

void MainWindow::tiltPinHigh()
{
    if(lockGraphicalUserInterface(tr("Tilt Output High Error")))
    {
        bool tempLatchedT0State = latchedT0State;
        bool tempLatchedT1State = latchedT1State;
        latchedT0State = ui->TPOLowRadioButton->isChecked();
        latchedT1State = ui->TPOHighRadioButton->isChecked();

        GUIFuture = QtConcurrent::run(tiltOutputWrapper,
                                      ui->TPOGroupBox->isChecked(),
                                      ui->TPOHighRadioButton->isChecked());

        if(!ranGraphicalUserInterface(tr("Tilt Output High Error")))
        {
            ui->TPOLowRadioButton->setChecked(tempLatchedT0State);
            ui->TPOHighRadioButton->setChecked(tempLatchedT1State);
            latchedT0State = tempLatchedT0State;
            latchedT1State = tempLatchedT1State;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->TPOLowRadioButton->setChecked(latchedT0State);
        ui->TPOHighRadioButton->setChecked(latchedT1State);
    }
}

void MainWindow::LEDPower()
{
    if(lockGraphicalUserInterface(ui->LEDCGroupBox->isChecked() ?
    tr("LED On Error") : tr("LED Off Error")))
    {
        if(ui->LEDCGroupBox->isChecked())
        {
            GUIFuture = QtConcurrent::run(LEDOnWrapper,
                                          ui->LEDFrequencySpinBox->value());
        }
        else
        {
            GUIFuture = QtConcurrent::run(LEDOffWrapper);
        }

        if(!ranGraphicalUserInterface(ui->LEDCGroupBox->isChecked() ?
        tr("LED On Error") : tr("LED Off Error")))
        {
            ui->LEDCGroupBox->setChecked(!ui->LEDCGroupBox->isChecked());
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->LEDCGroupBox->setChecked(!ui->LEDCGroupBox->isChecked());
    }
}

void MainWindow::LEDFrequencyApply()
{
    if(lockGraphicalUserInterface(tr("LED Frequency Apply Error")))
    {
        int tempLatchedLEDFrequencyValue = latchedLEDFrequencyValue;
        latchedLEDFrequencyValue = ui->LEDFrequencySpinBox->value();

        GUIFuture = QtConcurrent::run(LEDOnWrapper,
                                      ui->LEDFrequencySpinBox->value());

        if(!ranGraphicalUserInterface(tr("LED Frequency Apply Error")))
        {
            ui->LEDFrequencySpinBox->setValue(tempLatchedLEDFrequencyValue);
            latchedLEDFrequencyValue = tempLatchedLEDFrequencyValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->LEDFrequencySpinBox->setValue(latchedLEDFrequencyValue);
    }
}

void MainWindow::LEDFrequencyReset()
{
    if(lockGraphicalUserInterface(tr("LED Frequency Reset Error")))
    {
        ui->LEDFrequencySpinBox->setValue(CMUCAM4_LED_ON);

        int tempLatchedLEDFrequencyValue = latchedLEDFrequencyValue;
        latchedLEDFrequencyValue = ui->LEDFrequencySpinBox->value();

        GUIFuture = QtConcurrent::run(LEDOnWrapper,
                                      ui->LEDFrequencySpinBox->value());

        if(!ranGraphicalUserInterface(tr("LED Frequency Reset Error")))
        {
            ui->LEDFrequencySpinBox->setValue(tempLatchedLEDFrequencyValue);
            latchedLEDFrequencyValue = tempLatchedLEDFrequencyValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->LEDFrequencySpinBox->setValue(latchedLEDFrequencyValue);
    }
}

void MainWindow::invertedFilter()
{
    if(lockGraphicalUserInterface(ui->invertedFilterOnButton->isChecked() ?
    tr("Inverted Filter On Error") : tr("Inverted Filter Off Error")))
    {
        bool tempLatchedFilterOffState = latchedFilterOffState;
        bool tempLatchedFilterOnState = latchedFilterOnState;
        latchedFilterOffState = ui->invertedFilterOffButton->isChecked();
        latchedFilterOnState = ui->invertedFilterOnButton->isChecked();

        GUIFuture = QtConcurrent::run(invertedFilterWrapper,
                                      ui->invertedFilterOnButton->isChecked());

        if(!ranGraphicalUserInterface(ui->invertedFilterOnButton->isChecked() ?
        tr("Inverted Filter On Error") : tr("Inverted Filter Off Error")))
        {
            ui->invertedFilterOffButton->setChecked(tempLatchedFilterOffState);
            ui->invertedFilterOnButton->setChecked(tempLatchedFilterOnState);
            latchedFilterOffState = tempLatchedFilterOffState;
            latchedFilterOnState = tempLatchedFilterOnState;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->invertedFilterOffButton->setChecked(latchedFilterOffState);
        ui->invertedFilterOnButton->setChecked(latchedFilterOnState);
    }
}

void MainWindow::noiseFilterApply()
{
    if(lockGraphicalUserInterface(tr("Noise Filter Apply Error")))
    {
        int tempLatchedNoiseFilterValue = latchedNoiseFilterValue;
        latchedNoiseFilterValue = ui->noiseFilterSpinBox->value();

        GUIFuture = QtConcurrent::run(noiseFilterWrapper,
                                      ui->noiseFilterSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Noise Filter Apply Error")))
        {
            ui->noiseFilterSpinBox->setValue(tempLatchedNoiseFilterValue);
            latchedNoiseFilterValue = tempLatchedNoiseFilterValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->noiseFilterSpinBox->setValue(latchedNoiseFilterValue);
    }
}

void MainWindow::noiseFilterReset()
{
    if(lockGraphicalUserInterface(tr("Noise Filter Reset Error")))
    {
        ui->noiseFilterSpinBox->setValue(CMUCAM4_DEFAULT_NOISE_FILTER);

        int tempLatchedNoiseFilterValue = latchedNoiseFilterValue;
        latchedNoiseFilterValue = ui->noiseFilterSpinBox->value();

        GUIFuture = QtConcurrent::run(noiseFilterWrapper,
                                      ui->noiseFilterSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Noise Filter Reset Error")))
        {
            ui->noiseFilterSpinBox->setValue(tempLatchedNoiseFilterValue);
            latchedNoiseFilterValue = tempLatchedNoiseFilterValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->noiseFilterSpinBox->setValue(latchedNoiseFilterValue);
    }
}

void MainWindow::pollMode()
{
    if(lockGraphicalUserInterface(tr("Poll Mode Error")))
    {
        latchedPMState = ui->pollModeCheckBox->isChecked();

        GUIFuture = QtConcurrent::run(pollModeWrapper,
        ui->pollModeCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Poll Mode Error")))
        {
            ui->pollModeCheckBox->toggle();
            latchedPMState = !latchedPMState;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->pollModeCheckBox->toggle();
    }
}

void MainWindow::switchingMode()
{
    if(lockGraphicalUserInterface(tr("Switching Mode Error")))
    {
        latchedSMState = ui->switchingModeCheckBox->isChecked();

        GUIFuture = QtConcurrent::run(switchingModeWrapper,
        ui->switchingModeCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Switching Mode Error")))
        {
            ui->switchingModeCheckBox->toggle();
            latchedSMState = !latchedSMState;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->switchingModeCheckBox->toggle();
    }
}

void MainWindow::lineMode()
{
    if(lockGraphicalUserInterface(tr("Line Mode Error")))
    {
        latchedLMState = ui->lineModeCheckBox->isChecked();

        GUIFuture = QtConcurrent::run(lineModeWrapper,
        ui->lineModeCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Line Mode Error")))
        {
            ui->lineModeCheckBox->toggle();
            latchedLMState = !latchedLMState;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->lineModeCheckBox->toggle();
    }
}

void MainWindow::histogramTracking()
{
    if(lockGraphicalUserInterface(tr("Histogram Tracking Error")))
    {
        latchedHTState = ui->histogramTrackingCheckBox->isChecked();

        GUIFuture = QtConcurrent::run(histogramTrackingWrapper,
        ui->histogramTrackingCheckBox->isChecked());

        if(!ranGraphicalUserInterface(tr("Histogram Tracking Error")))
        {
            ui->histogramTrackingCheckBox->toggle();
            latchedHTState = !latchedHTState;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->histogramTrackingCheckBox->toggle();
    }
}

void MainWindow::registerRead()
{
    if(lockGraphicalUserInterface(tr("Register Read Error")))
    {
        int tempLatchedRegAddress = latchedRegAddress;
        int tempLatchedRegValue = latchedRegValue;
        latchedRegAddress = ui->regAddressSpinBox->value();
        latchedRegValue = ui->regValueSpinBox->value();

        GUIFuture = QtConcurrent::run(cameraRegisterReadWrapper,
                                      ui->regAddressSpinBox->value());

        if(!ranGraphicalUserInterface(tr("Register Read Error")))
        {
            ui->regAddressSpinBox->setValue(tempLatchedRegAddress);
            ui->regValueSpinBox->setValue(tempLatchedRegValue);
            latchedRegAddress = tempLatchedRegAddress;
            latchedRegValue = tempLatchedRegValue;
        }
        else
        {
            ui->regValueSpinBox->setValue(GUIResult);
            latchedRegValue = GUIResult;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->regAddressSpinBox->setValue(latchedRegAddress);
        ui->regValueSpinBox->setValue(latchedRegValue);
    }
}

void MainWindow::registerWrite()
{
    if(lockGraphicalUserInterface(tr("Register Write Error")))
    {
        int tempLatchedRegAddress = latchedRegAddress;
        int tempLatchedRegValue = latchedRegValue;
        latchedRegAddress = ui->regAddressSpinBox->value();
        latchedRegValue = ui->regValueSpinBox->value();

        GUIFuture = QtConcurrent::run(cameraRegisterWriteWrapper,
                                      ui->regAddressSpinBox->value(),
                                      ui->regValueSpinBox->value(),
                                      CMUCAM4_DEFAULT_REG_MASK);

        if(!ranGraphicalUserInterface(tr("Register Write Error")))
        {
            ui->regAddressSpinBox->setValue(tempLatchedRegAddress);
            ui->regValueSpinBox->setValue(tempLatchedRegValue);
            latchedRegAddress = tempLatchedRegAddress;
            latchedRegValue = tempLatchedRegValue;
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->regAddressSpinBox->setValue(latchedRegAddress);
        ui->regValueSpinBox->setValue(latchedRegValue);
    }
}

void MainWindow::statusEnable()
{
    if(lockGraphicalUserInterface(tr("Get Status Update Error")))
    {
        if((statusUpdateEnable = ui->StatusCGroupBox->isChecked()))
        {
           statusUpdateRRobin = GB;
           statusFuture = QtConcurrent::run(getButtonStateWrapper);
           statusFutureWatcher.setFuture(statusFuture);
        }
        else
        {
           statusEventLoop.exec();
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->StatusCGroupBox->setChecked(statusUpdateEnable);
    }
}

void MainWindow::statusUpdate()
{
    if((statusResult = statusFuture.result()) >= CMUCAM4_RETURN_SUCCESS)
    {
        switch(statusUpdateRRobin)
        {

        case GB:
        ui->userButtonStateValue->setNum(statusResult);
        break;

        case GD:
        ui->userButtonDurationValue->setNum(statusResult);
        break;

        case GP:
        ui->userButtonPressedValue->setNum(statusResult);
        break;

        case GR:
        ui->userButtonReleasedValue->setNum(statusResult);
        break;

        case PI:
        ui->panInputValue->setNum(statusResult);
        break;

        case TI:
        ui->tiltInputValue->setNum(statusResult);
        break;

        case GPS:
        ui->panServoPositionValue->setNum(statusResult);
        break;

        case GTS:
        ui->tiltServoPositionValue->setNum(statusResult);
        break;

        }
    }
    else
    {
        QMessageBox::critical(this, tr("Get Status Update Error"),
        errorString(statusResult));
    }

    if(statusUpdateEnable && (statusResult >= CMUCAM4_RETURN_SUCCESS))
    {
        switch(statusUpdateRRobin)
        {
            case GB:
            statusUpdateRRobin = GD;
            statusFuture = QtConcurrent::run(getButtonDurationWrapper);
            break;

            case GD:
            statusUpdateRRobin = GP;
            statusFuture = QtConcurrent::run(getButtonPressedWrapper);
            break;

            case GP:
            statusUpdateRRobin = GR;
            statusFuture = QtConcurrent::run(getButtonReleasedWrapper);
            break;

            case GR:
            statusUpdateRRobin = PI;
            statusFuture = QtConcurrent::run(panInputWrapper);
            break;

            case PI:
            statusUpdateRRobin = TI;
            statusFuture = QtConcurrent::run(tiltInputWrapper);
            break;

            case TI:
            statusUpdateRRobin = GPS;
            statusFuture = QtConcurrent::run(getServoPositionWrapper,
            CMUCAM4_PAN_SERVO);
            break;

            case GPS:
            statusUpdateRRobin = GTS;
            statusFuture = QtConcurrent::run(getServoPositionWrapper,
            CMUCAM4_TILT_SERVO);
            break;

            case GTS:
            statusUpdateRRobin = GB;
            statusFuture = QtConcurrent::run(getButtonStateWrapper);
            break;
        }

        statusFutureWatcher.setFuture(statusFuture);
    }
    else
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
        }
        else
        {
            statusEventLoop.exit();
        }
    }
}

void MainWindow::trackColor()
{
    if(lockGraphicalUserInterface(tr("Track Color Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(trackColorWrapper);

        if(ranGraphicalUserInterface(tr("Track Color Error")))
        {
            streamFuture = QtConcurrent::run(getTypeTDataPacketWrapper,
                                             &trackingData);

            streamFutureWatcher.setFuture(streamFuture);
            getType = dataPacket = TYPE_T;

            ui->TCStopPushButton->setEnabled(!latchedPMState);
            ui->mfCheckBox->setChecked(false);
        }
        else
        {
            unlockGraphicalUserInterface();
        }
    }
}

void MainWindow::trackWindow()
{
    if(lockGraphicalUserInterface(tr("Track Window Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(trackWindowWrapper,
        ui->TWRedRangeSpinBox->value(), ui->TWGreenRangeSpinBox->value(),
        ui->TWBlueRangeSpinBox->value());

        if(ranGraphicalUserInterface(tr("Track Window Error")))
        {
            streamFuture = QtConcurrent::run(getTypeTDataPacketWrapper,
                                             &trackingData);

            streamFutureWatcher.setFuture(streamFuture);
            getType = dataPacket = TYPE_T;

            ui->TWStopPushButton->setEnabled(!latchedPMState);
            ui->mfCheckBox->setChecked(false);
        }
        else
        {
            unlockGraphicalUserInterface();
        }
    }
}

void MainWindow::getMean()
{
    if(lockGraphicalUserInterface(tr("Get Mean Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        GUIFuture = QtConcurrent::run(getMeanWrapper);

        if(ranGraphicalUserInterface(tr("Get Mean Error")))
        {
            streamFuture = QtConcurrent::run(getTypeSDataPacketWrapper,
                                             &statisticsData);

            streamFutureWatcher.setFuture(streamFuture);
            getType = dataPacket = TYPE_S;

            ui->GMStopPushButton->setEnabled(!latchedPMState);
        }
        else
        {
            unlockGraphicalUserInterface();
        }
    }
}

void MainWindow::getHistogram()
{
    if(lockGraphicalUserInterface(tr("Get Histogram Error")))
    {
        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;
            ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);
            statusEventLoop.exec();
        }

        latchedChannel = ui->GHChannelComboBox->currentIndex();
        latchedBins = ui->GHBinsComboBox->currentIndex();

        GUIFuture = QtConcurrent::run(getHistogramWrapper,
                                      ui->GHChannelComboBox->currentIndex(),
                                      ui->GHBinsComboBox->currentIndex());

        if(ranGraphicalUserInterface(tr("Get Histogram Error")))
        {
            switch(latchedBins)
            {
                case CMUCAM4_H1_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper1,
                                                 &histogramData1); } break;
                case CMUCAM4_H2_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper2,
                                                 &histogramData2); } break;
                case CMUCAM4_H4_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper4,
                                                 &histogramData4); } break;
                case CMUCAM4_H8_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper8,
                                                 &histogramData8); } break;
                case CMUCAM4_H16_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper16,
                                                 &histogramData16); } break;
                case CMUCAM4_H32_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper32,
                                                 &histogramData32); } break;
                case CMUCAM4_H64_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper64,
                                                 &histogramData64); } break;
            }

            streamFutureWatcher.setFuture(streamFuture);
            getType = dataPacket = TYPE_H;

            ui->GHStopPushButton->setEnabled(!latchedPMState);
        }
        else
        {
            unlockGraphicalUserInterface();
        }
    }
}

void MainWindow::idleCamera()
{
    if(getType != TYPE_F)
    {
        getType = TYPE_F;

        ui->TCStopPushButton->setEnabled(CMUCAM4_DEFAULT_TC_STATE);
        ui->TWStopPushButton->setEnabled(CMUCAM4_DEFAULT_TW_STATE);
        ui->GMStopPushButton->setEnabled(CMUCAM4_DEFAULT_GM_STATE);
        ui->GHStopPushButton->setEnabled(CMUCAM4_DEFAULT_GH_STATE);

        streamEventLoop.exec();

        statusUpdateEnable = CMUCAM4_DEFAULT_SC;

        if((!needToRescaleGetMean) || (!needToRescaleGetHistogram))
        {
            ui->colorStatisticsView->setInteraction(
            QCustomPlot::iRangeDrag, true);

            ui->colorStatisticsView->setInteraction(
            QCustomPlot::iRangeZoom, true);
        }

        needToRescaleGetMean = CMUCAM4_DEFAULT_NTR_GM;
        needToRescaleGetHistogram = CMUCAM4_DEFAULT_NTR_GH;

        GUIFuture = QtConcurrent::run(idleCameraWrapper);
        ranGraphicalUserInterface(tr("Idle Camera Error"));
        unlockGraphicalUserInterface();
    }
}

void MainWindow::getTypeDataPacket()
{
    /* Handle present data packet */

    if((streamResult = streamFuture.result()) >= CMUCAM4_RETURN_SUCCESS) {
    switch(dataPacket)
    {
        case TYPE_T: {

        ui->centroidMXValue->setNum(trackingData.mx);
        ui->centroidMYValue->setNum(trackingData.my);
        ui->boundingBoxX1Value->setNum(trackingData.x1);
        ui->boundingBoxY1Value->setNum(trackingData.y1);
        ui->boundingBoxX2Value->setNum(trackingData.x2);
        ui->boundingBoxY2Value->setNum(trackingData.y2);
        ui->pixelsValue->setNum(trackingData.pixels);
        ui->confidenceValue->setNum(trackingData.confidence);

        statusUpdateEnable = true;

        if(latchedLMState)
        {
            break;
        }

        }

        case TYPE_F: {

        FPixmap.fill(Qt::black);

        QPainter painter(&FPixmap);

        if(latchedLMState)
        {
            painter.setPen(Qt::blue);

            for(int y = CMUCAM4_MIN_BINARY_ROW;
            y <= CMUCAM4_MAX_BINARY_ROW; y++)
            {
                for(int x = CMUCAM4_MIN_BINARY_COLUMN;
                x <= CMUCAM4_MAX_BINARY_COLUMN; x++)
                {
                    if(CMUCAM4_GET_PIXEL(&imageData, y, x))
                    {
                        painter.drawRect(x * 2, y * 2, 1, 1);
                    }
                }
            }
        }

        if(statusUpdateEnable)
        {
            statusUpdateEnable = CMUCAM4_DEFAULT_SC;

            if(trackingData.pixels)
            {

            painter.setPen(QColor(Qt::red));
            painter.drawRect(trackingData.x1, trackingData.y1,
            trackingData.x2 - trackingData.x1 /* plus the pen width */,
            trackingData.y2 - trackingData.y1 /* plus the pen width */);

            painter.setPen(QColor(Qt::white));
            painter.drawPoint(trackingData.mx, trackingData.my);

            }
            else
            {

            QFont font = painter.font();
            font.setStyleStrategy(QFont::NoAntialias);
            painter.setFont(font);

            painter.setPen(QColor(Qt::green));
            painter.drawText(0, 0, FPixmap.width() - 1, FPixmap.height() - 1,
            Qt::AlignCenter | Qt::TextWordWrap, tr("Tracking Colors Lost"));

            }
        }

        TPixmap->setPixmap(FPixmap);

        FTView->setSceneRect(TPixmap->sceneBoundingRect());
        ui->colorTrackingView->fitInView(FTView->sceneRect(),
                                         Qt::KeepAspectRatio);

        } break;

        case TYPE_H: {

        if(needToRescaleGetHistogram)
        {
            needToRescaleGetHistogram = false;
            needToRescaleGetMean = CMUCAM4_DEFAULT_NTR_GM;

            ui->colorStatisticsView->xAxis->setLabel(tr("Bins"));
            ui->colorStatisticsView->yAxis->setLabel(tr("Percentage"));

            switch(latchedBins)
            {
                case CMUCAM4_H1_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_1_T_LENGTH); } break;

                case CMUCAM4_H2_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_2_T_LENGTH); } break;

                case CMUCAM4_H4_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_4_T_LENGTH); } break;

                case CMUCAM4_H8_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_8_T_LENGTH); } break;

                case CMUCAM4_H16_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_16_T_LENGTH); } break;

                case CMUCAM4_H32_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_32_T_LENGTH); } break;

                case CMUCAM4_H64_BINS: {
                ui->colorStatisticsView->yAxis->setRange(0.0,
                1.0 / CMUCAM4_HD_64_T_LENGTH); } break;
            }

            ui->colorStatisticsView->xAxis->setTicks(true);
            ui->colorStatisticsView->xAxis->setTickLabels(true);

            ui->colorStatisticsView->yAxis->setTicks(true);
            ui->colorStatisticsView->yAxis->setTickLabels(true);
            ui->colorStatisticsView->yAxis->setGrid(true);
            ui->colorStatisticsView->yAxis->setSubGrid(true);

            ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeDrag,
            CMUCAM4_DEFAULT_DRAG_INTERACTION);

            ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeZoom,
            CMUCAM4_DEFAULT_ZOOM_INTERACTION);
        }

        RPlot->clearData();
        GPlot->clearData();
        BPlot->clearData();
        HPlot->clearData();

        QVector<QString> labels;
        QVector<double> ticks;

        switch(latchedBins)
        {
            case CMUCAM4_H1_BINS: { int j = (256 / CMUCAM4_HD_1_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_1_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData1.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H2_BINS: { int j = (256 / CMUCAM4_HD_2_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_2_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData2.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H4_BINS: { int j = (256 / CMUCAM4_HD_4_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_4_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData4.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H8_BINS: { int j = (256 / CMUCAM4_HD_8_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_8_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData8.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H16_BINS: { int j = (256 / CMUCAM4_HD_16_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_16_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData16.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H32_BINS: { int j = (256 / CMUCAM4_HD_32_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_32_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData32.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;

            case CMUCAM4_H64_BINS: { int j = (256 / CMUCAM4_HD_64_T_LENGTH);
            for(int i = 0; i < CMUCAM4_HD_64_T_LENGTH; i++)
            {
                HPlot->addData((double) i,
                ((double) histogramData64.bins[i]) / CMUCAM4_DEFAULT_RGB_MAX);
                labels.append(QString(tr("%1 to %2")).arg(i * j).arg(((i + 1) *
                j) - 1)); ticks.append((double) i);
            }
            } break;
        }

        ui->colorStatisticsView->xAxis->setTickVectorLabels(labels);
        ui->colorStatisticsView->xAxis->setTickVector(ticks);

        switch(latchedChannel)
        {
            case CMUCAM4_RED_CHANNEL:
            HPlot->setPen(QPen(QColor(Qt::darkRed)));
            HPlot->setBrush(QBrush(Qt::red)); break;

            case CMUCAM4_GREEN_CHANNEL:
            HPlot->setPen(QPen(QColor(Qt::darkGreen)));
            HPlot->setBrush(QBrush(Qt::green)); break;

            case CMUCAM4_BLUE_CHANNEL:
            HPlot->setPen(QPen(QColor(Qt::darkBlue)));
            HPlot->setBrush(QBrush(Qt::blue)); break;
        }

        HPlot->rescaleKeyAxis(false);
        HPlot->rescaleValueAxis(true);

        ui->colorStatisticsView->replot();

        } break;

        case TYPE_S: {

        ui->redMeanValue->setNum(statisticsData.RMean);
        ui->redMedianValue->setNum(statisticsData.RMedian);
        ui->redModeValue->setNum(statisticsData.RMode);
        ui->redStdevValue->setNum(statisticsData.RStDev);
        ui->greenMeanValue->setNum(statisticsData.GMean);
        ui->greenMedianValue->setNum(statisticsData.GMedian);
        ui->greenModeValue->setNum(statisticsData.GMode);
        ui->greenStdevValue->setNum(statisticsData.GStDev);
        ui->blueMeanValue->setNum(statisticsData.BMean);
        ui->blueMedianValue->setNum(statisticsData.BMedian);
        ui->blueModeValue->setNum(statisticsData.BMode);
        ui->blueStdevValue->setNum(statisticsData.BStDev);

        if(needToRescaleGetMean)
        {
            needToRescaleGetMean = false;
            needToRescaleGetHistogram = CMUCAM4_DEFAULT_NTR_GH;

            ui->colorStatisticsView->xAxis->setLabel(tr("Channels"));
            ui->colorStatisticsView->yAxis->setLabel(tr("Value"));

            ui->colorStatisticsView->xAxis->setRange(0, 1);
            ui->colorStatisticsView->yAxis->setRange(0, 1);

            ui->colorStatisticsView->xAxis->setTicks(true);
            ui->colorStatisticsView->xAxis->setTickLabels(true);

            ui->colorStatisticsView->yAxis->setTicks(true);
            ui->colorStatisticsView->yAxis->setTickLabels(true);
            ui->colorStatisticsView->yAxis->setGrid(true);
            ui->colorStatisticsView->yAxis->setSubGrid(true);

            ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeDrag,
            CMUCAM4_DEFAULT_DRAG_INTERACTION);

            ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeZoom,
            CMUCAM4_DEFAULT_ZOOM_INTERACTION);
        }

        RPlot->clearData();
        GPlot->clearData();
        BPlot->clearData();
        HPlot->clearData();

        RPlot->setOutliers(QVector<double>() << statisticsData.RMode);
        GPlot->setOutliers(QVector<double>() << statisticsData.GMode);
        BPlot->setOutliers(QVector<double>() << statisticsData.BMode);

        RPlot->setData(0,
        qMax(statisticsData.RMean - (1.645 * statisticsData.RStDev), // 5%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        qMax(statisticsData.RMean - (0.674 * statisticsData.RStDev), // 25%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        statisticsData.RMedian,
        qMin(statisticsData.RMean + (0.674 * statisticsData.RStDev), // 50%
        (double) CMUCAM4_DEFAULT_RGB_MAX),
        qMin(statisticsData.RMean + (1.645 * statisticsData.RStDev), // 95%
        (double) CMUCAM4_DEFAULT_RGB_MAX));

        GPlot->setData(1,
        qMax(statisticsData.GMean - (1.645 * statisticsData.GStDev), // 5%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        qMax(statisticsData.GMean - (0.674 * statisticsData.GStDev), // 25%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        statisticsData.GMedian,
        qMin(statisticsData.GMean + (0.674 * statisticsData.GStDev), // 50%
        (double) CMUCAM4_DEFAULT_RGB_MAX),
        qMin(statisticsData.GMean + (1.645 * statisticsData.GStDev), // 95%
        (double) CMUCAM4_DEFAULT_RGB_MAX));

        BPlot->setData(2,
        qMax(statisticsData.BMean - (1.645 * statisticsData.BStDev), // 5%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        qMax(statisticsData.BMean - (0.674 * statisticsData.BStDev), // 25%
        (double) CMUCAM4_DEFAULT_RGB_MIN),
        statisticsData.BMedian,
        qMin(statisticsData.BMean + (0.674 * statisticsData.BStDev), // 50%
        (double) CMUCAM4_DEFAULT_RGB_MAX),
        qMin(statisticsData.BMean + (1.645 * statisticsData.BStDev), // 95%
        (double) CMUCAM4_DEFAULT_RGB_MAX));

        QVector<QString> labels;
        QVector<double> ticks;

        labels.append(tr("Red Box"));
        labels.append(tr("Green Box"));
        labels.append(tr("Blue Box"));

        ticks.append(0);
        ticks.append(1);
        ticks.append(2);

        ui->colorStatisticsView->xAxis->setTickVectorLabels(labels);
        ui->colorStatisticsView->xAxis->setTickVector(ticks);

        RPlot->rescaleAxes(true);
        GPlot->rescaleAxes(true);
        BPlot->rescaleAxes(true);

        ui->colorStatisticsView->replot();

        if(latchedSMState)
        {
            statusUpdateEnable = true;
        }

        } break;
    }
    } else { QMessageBox::critical(this, tr("Get Data Packet Error"),
    errorString(streamResult)); }

    /* Handle future data packet */

    if((dataPacket != TYPE_F) && latchedLMState &&
    (streamResult >= CMUCAM4_RETURN_SUCCESS))
    {
        streamFuture = QtConcurrent::run(getTypeFDataPacketWrapper,
                                         &imageData);

        streamFutureWatcher.setFuture(streamFuture);
        dataPacket = TYPE_F;
    }
    else
    {
        if((getType != TYPE_F) && (latchedPMState ||
        (streamResult <= CMUCAM4_RETURN_FAILURE)))
        {
            getType = TYPE_F;

            ui->TCStopPushButton->setEnabled(CMUCAM4_DEFAULT_TC_STATE);
            ui->TWStopPushButton->setEnabled(CMUCAM4_DEFAULT_TW_STATE);
            ui->GMStopPushButton->setEnabled(CMUCAM4_DEFAULT_GM_STATE);
            ui->GHStopPushButton->setEnabled(CMUCAM4_DEFAULT_GH_STATE);

            statusUpdateEnable = CMUCAM4_DEFAULT_SC;

            if((!needToRescaleGetMean) || (!needToRescaleGetHistogram))
            {
                ui->colorStatisticsView->setInteraction(
                QCustomPlot::iRangeDrag, true);

                ui->colorStatisticsView->setInteraction(
                QCustomPlot::iRangeZoom, true);
            }

            needToRescaleGetMean = CMUCAM4_DEFAULT_NTR_GM;
            needToRescaleGetHistogram = CMUCAM4_DEFAULT_NTR_GH;

            GUIFuture = QtConcurrent::run(idleCameraWrapper);
            ranGraphicalUserInterface(tr("Idle Camera Error"));
            unlockGraphicalUserInterface();
        }
        else
        {
            if(latchedSMState)
            {
                switch(getType)
                {
                    case TYPE_F: break;
                    case TYPE_H: break;
                    case TYPE_S: getType = TYPE_T; break;
                    case TYPE_T: getType = TYPE_S; break;
                }
            }

            switch(getType)
            {
                case TYPE_F: {

                streamEventLoop.exit();

                } break;

                case TYPE_H: {

                switch(latchedBins)
                {
                case CMUCAM4_H1_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper1,
                                                 &histogramData1); } break;
                case CMUCAM4_H2_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper2,
                                                 &histogramData2); } break;
                case CMUCAM4_H4_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper4,
                                                 &histogramData4); } break;
                case CMUCAM4_H8_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper8,
                                                 &histogramData8); } break;
                case CMUCAM4_H16_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper16,
                                                 &histogramData16); } break;
                case CMUCAM4_H32_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper32,
                                                 &histogramData32); } break;
                case CMUCAM4_H64_BINS: {
                streamFuture = QtConcurrent::run(getTypeHDataPacketWrapper64,
                                                 &histogramData64); } break;
                }

                streamFutureWatcher.setFuture(streamFuture);
                dataPacket = TYPE_H;

                } break;

                case TYPE_S: {

                streamFuture = QtConcurrent::run(getTypeSDataPacketWrapper,
                                                 &statisticsData);

                streamFutureWatcher.setFuture(streamFuture);
                dataPacket = TYPE_S;

                } break;

                case TYPE_T: {

                streamFuture = QtConcurrent::run(getTypeTDataPacketWrapper,
                                                 &trackingData);

                streamFutureWatcher.setFuture(streamFuture);
                dataPacket = TYPE_T;

                } break;
            }
        }
    }
}

void MainWindow::diskInformation()
{
    if(lockGraphicalUserInterface(tr("Disk Information Error")))
    {
        CMUcam4_disk_information_t information;

        GUIFuture = QtConcurrent::run(diskInformationWrapper, &information);

        if(ranGraphicalUserInterface(tr("Disk Information Error")))
        {
            ui->volumeLabelValue->setText(information.volumeLabel);
            ui->fileSystemTypeValue->setText(information.fileSystemType);

            ui->diskSignatureValue->setText(QString("0x") +
            QString::number(information.diskSignature, 16).toUpper());

            ui->volumeIdentificationValue->setText(QString("0x") +
            QString::number(information.volumeIdentification, 16).toUpper());

            ui->countOfDataSectorsValue->setText(
            QLocale().toString((unsigned int) information.countOfDataSectors));

            ui->sectorsPerClusterValue->setText(
            QLocale().toString((unsigned int) information.sectorsPerCluster));

            ui->bytesPerSectorValue->setText(
            QLocale().toString((unsigned int) information.bytesPerSector));

            ui->countOfClustersValue->setText(
            QLocale().toString((unsigned int) information.countOfClusters));
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::diskSpace()
{
    if(lockGraphicalUserInterface(tr("Disk Space Error")))
    {
        CMUcam4_disk_space_t space;

        GUIFuture = QtConcurrent::run(diskSpaceWrapper, &space);

        if(ranGraphicalUserInterface(tr("Disk Space Error")))
        {
            ui->freeSectorCountValue->setText(
            QLocale().toString((unsigned int) space.freeSectorCount));

            ui->usedSectorCountValue->setText(
            QLocale().toString((unsigned int) space.usedSectorCount));
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::formatDisk()
{
    if(lockGraphicalUserInterface(tr("Format Disk Error")))
    {
        GUIFuture = QtConcurrent::run(formatDiskWrapper);

        if(ranGraphicalUserInterface(tr("Format Disk Error")))
        {
            // Disk Information Box

            ui->volumeLabelValue->setNum(CMUCAM4_DEFAULT_DI_VL);
            ui->fileSystemTypeValue->setNum(CMUCAM4_DEFAULT_DI_FST);
            ui->diskSignatureValue->setNum(CMUCAM4_DEFAULT_DI_DS);
            ui->volumeIdentificationValue->setNum(CMUCAM4_DEFAULT_DI_VI);
            ui->countOfDataSectorsValue->setNum(CMUCAM4_DEFAULT_DI_CODS);
            ui->sectorsPerClusterValue->setNum(CMUCAM4_DEFAULT_DI_SPC);
            ui->bytesPerSectorValue->setNum(CMUCAM4_DEFAULT_DI_BPS);
            ui->countOfClustersValue->setNum(CMUCAM4_DEFAULT_DI_COC);

            // Disk Space Box

            ui->freeSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_FSC);
            ui->usedSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_USC);

            // New File Box

            ui->newFileNameEdit->setText(QString());
            ui->newTextToAppendEdit->setText(QString());

            // New Folder Box

            ui->newFolderNameEdit->setText(QString());

            // Name Box

            ui->entryNameEdit->setText(QString());

            // Attributes Box

            ui->readOnlyCheckBox->setChecked(CMUCAM4_DEFAULT_R_ATTRIB);
            ui->hiddenCheckBox->setChecked(CMUCAM4_DEFAULT_H_ATTRIB);
            ui->systemCheckBox->setChecked(CMUCAM4_DEFAULT_S_ATTRIB);
            ui->archiveCheckBox->setChecked(CMUCAM4_DEFAULT_A_ATTRIB);

            // Selected Item Box

            ui->selectedItemBox->setEnabled(CMUCAM4_DEFAULT_SI);

            // File Explorer Box

            ui->diskView->clear();
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::unmountDisk()
{
    if(lockGraphicalUserInterface(tr("Unmount Disk Error")))
    {
        GUIFuture = QtConcurrent::run(unmountDiskWrapper);

        if(ranGraphicalUserInterface(tr("Unmount Disk Error")))
        {
            // Disk Information Box

            ui->volumeLabelValue->setNum(CMUCAM4_DEFAULT_DI_VL);
            ui->fileSystemTypeValue->setNum(CMUCAM4_DEFAULT_DI_FST);
            ui->diskSignatureValue->setNum(CMUCAM4_DEFAULT_DI_DS);
            ui->volumeIdentificationValue->setNum(CMUCAM4_DEFAULT_DI_VI);
            ui->countOfDataSectorsValue->setNum(CMUCAM4_DEFAULT_DI_CODS);
            ui->sectorsPerClusterValue->setNum(CMUCAM4_DEFAULT_DI_SPC);
            ui->bytesPerSectorValue->setNum(CMUCAM4_DEFAULT_DI_BPS);
            ui->countOfClustersValue->setNum(CMUCAM4_DEFAULT_DI_COC);

            // Disk Space Box

            ui->freeSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_FSC);
            ui->usedSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_USC);

            // New File Box

            ui->newFileNameEdit->setText(QString());
            ui->newTextToAppendEdit->setText(QString());

            // New Folder Box

            ui->newFolderNameEdit->setText(QString());

            // Name Box

            ui->entryNameEdit->setText(QString());

            // Attributes Box

            ui->readOnlyCheckBox->setChecked(CMUCAM4_DEFAULT_R_ATTRIB);
            ui->hiddenCheckBox->setChecked(CMUCAM4_DEFAULT_H_ATTRIB);
            ui->systemCheckBox->setChecked(CMUCAM4_DEFAULT_S_ATTRIB);
            ui->archiveCheckBox->setChecked(CMUCAM4_DEFAULT_A_ATTRIB);

            // Selected Item Box

            ui->selectedItemBox->setEnabled(CMUCAM4_DEFAULT_SI);

            // File Explorer Box

            ui->diskView->clear();
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::itemActivated()
{
    itemActivated(ui->diskView->currentItem(), ui->diskView->currentColumn());
}

void MainWindow::itemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if(item->text(CMUCAM4_DV_ATTRIBUTES).contains(QChar('D')))
    {
        changeDirectory(item);
    }
    else
    {
        filePrint(item);
    }
}

void MainWindow::currentItemChanged(QTreeWidgetItem *current,
                                    QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    if(current == NULL)
    {
        ui->entryNameEdit->setText(QString());

        ui->readOnlyCheckBox->setChecked(CMUCAM4_DEFAULT_R_ATTRIB);
        ui->hiddenCheckBox->setChecked(CMUCAM4_DEFAULT_H_ATTRIB);
        ui->systemCheckBox->setChecked(CMUCAM4_DEFAULT_S_ATTRIB);
        ui->archiveCheckBox->setChecked(CMUCAM4_DEFAULT_A_ATTRIB);
    }
    else
    {
        ui->entryNameEdit->setText(current->text(CMUCAM4_DV_NAME));

        ui->readOnlyCheckBox->setChecked(current->text(CMUCAM4_DV_ATTRIBUTES).
                                         contains(QChar('R')));
        ui->hiddenCheckBox->setChecked(current->text(CMUCAM4_DV_ATTRIBUTES).
                                       contains(QChar('H')));
        ui->systemCheckBox->setChecked(current->text(CMUCAM4_DV_ATTRIBUTES).
                                       contains(QChar('S')));
        ui->archiveCheckBox->setEnabled(!current->text(CMUCAM4_DV_ATTRIBUTES).
                                        contains(QChar('D')));
        ui->archiveCheckBox->setChecked(current->text(CMUCAM4_DV_ATTRIBUTES).
                                        contains(QChar('A')));

        if(current->text(CMUCAM4_DV_ATTRIBUTES).contains(QChar('D')))
        {
            ui->newFolderNameEdit->setText(current->text(CMUCAM4_DV_NAME));
            ui->newFileNameEdit->setText(QString());
        }
        else
        {
            ui->newFileNameEdit->setText(current->text(CMUCAM4_DV_NAME));
            ui->newFolderNameEdit->setText(QString());
        }
    }

    ui->selectedItemBox->setEnabled(current != NULL);
}

void MainWindow::removeEntry()
{
    removeEntry(ui->diskView->currentItem());
}

void MainWindow::moveEntry()
{
    moveEntry(ui->diskView->currentItem());
}

void MainWindow::changeAttributes()
{
    changeAttributes(ui->diskView->currentItem());
}

void MainWindow::listDirectory()
{
    if(lockGraphicalUserInterface(tr("List Directory Error")))
    {
        CMUcam4_directory_entry_t entries[CMUCAM4_DV_NUMBER];

        GUIFuture = QtConcurrent::run(listDirectoryWrapper,
        (CMUcam4_directory_entry_t *) entries, CMUCAM4_DV_NUMBER, 0);

        if(ranGraphicalUserInterface(tr("List Directory Error")))
        {
            ui->diskView->clear();

            for(int i = 0; i < GUIFuture; i++)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() <<
                QString(entries[i].name).trimmed() <<
                (CMUcam.isDirectory(&entries[i]) ? QString() :
                QLocale().toString((unsigned int) entries[i].size)) <<
                QString(entries[i].attributes).trimmed());

                item->setIcon(CMUCAM4_DV_NAME, CMUcam.isDirectory(&entries[i])
                ? QFileIconProvider().icon(QFileIconProvider::Folder)
                : QFileIconProvider().icon(QFileIconProvider::File));

                ui->diskView->addTopLevelItem(item);
            }

            ui->diskView->sortItems(CMUCAM4_DV_NAME, Qt::AscendingOrder);

            ui->diskView->resizeColumnToContents(CMUCAM4_DV_NAME);
            ui->diskView->resizeColumnToContents(CMUCAM4_DV_SIZE);
            ui->diskView->resizeColumnToContents(CMUCAM4_DV_ATTRIBUTES);
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::changeDirectory(QTreeWidgetItem *item)
{
    if(lockGraphicalUserInterface(tr("Change Directory Error")))
    {
        QByteArray name = item->text(CMUCAM4_DV_NAME).toLatin1();

        GUIFuture = QtConcurrent::run(changeDirectoryWrapper,
                                      name.constData());

        bool okay = ranGraphicalUserInterface(tr("Change Directory Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::filePrint(QTreeWidgetItem *item)
{
    if(lockGraphicalUserInterface(tr("File Transfer Error")))
    {
        bool okay;

        qint64 size = (qint64) QLocale().toUInt(item->text(CMUCAM4_DV_SIZE),
                                                &okay);

        if(okay && (size <= CMUCAM4_FP_SIZE))
        {
            QByteArray name = item->text(CMUCAM4_DV_NAME).toLatin1();

            uint8_t buffer[size];

            GUIFuture = QtConcurrent::run(filePrintWrapper,
                                          name.constData(),
                                          (uint8_t *) buffer,
                                          (size_t) size,
                                          CMUCAM4_FP_OFFSET);

            if(ranGraphicalUserInterface(tr("File Transfer Error")))
            {
                QString fileName = QDir::cleanPath(QDir::tempPath() +
                                                   QDir::separator() +
                                                   QString(name.constData()));

                QFile file(fileName);

                if(file.open(QIODevice::WriteOnly | QIODevice::Truncate) &&
                (file.write((const char *) buffer, size) == size))
                {
                    file.close();

                    if(!QDesktopServices::openUrl(QUrl(fileName)))
                    {
                        QMessageBox::critical(this,
                        tr("File Transfer Error"),
                        tr("Unable to close file"));
                    }
                }
                else
                {
                    if(file.isOpen() && (!file.remove()))
                    {
                        QMessageBox::critical(this,
                        tr("File Transfer Error"),
                        tr("Unable to close file"));
                    }
                    else
                    {
                        QMessageBox::critical(this,
                        tr("File Transfer Error"),
                        tr("Unable to open file"));
                    }
                }
            }
        }
        else
        {
            QMessageBox::critical(this,
            tr("File Transfer Error"),
            tr("Unable to open file"));
        }

        unlockGraphicalUserInterface();
    }
}

void MainWindow::removeEntry(QTreeWidgetItem *item)
{
    if(lockGraphicalUserInterface(tr("Remove Entry Error")))
    {
        QByteArray name = item->text(CMUCAM4_DV_NAME).toLatin1();

        GUIFuture = QtConcurrent::run(removeEntryWrapper,
                                      name.constData());

        bool okay = ranGraphicalUserInterface(tr("Remove Entry Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::moveEntry(QTreeWidgetItem *item)
{
    if(lockGraphicalUserInterface(tr("Rename Entry Error")))
    {
        QByteArray oldName = item->text(CMUCAM4_DV_NAME).toLatin1();

        QByteArray newName = ui->entryNameEdit->text().replace(QChar('\"'),
        QChar('_')).toLatin1();

        GUIFuture = QtConcurrent::run(moveEntryWrapper,
                                      oldName.constData(),
                                      newName.constData());

        bool okay = ranGraphicalUserInterface(tr("Rename Entry Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
        else
        {
            ui->entryNameEdit->setText(item->text(CMUCAM4_DV_NAME));
        }
    }
    else
    {
        ui->entryNameEdit->setText(item->text(CMUCAM4_DV_NAME));
    }
}

void MainWindow::changeAttributes(QTreeWidgetItem *item)
{
    if(lockGraphicalUserInterface(tr("Change Attributes Error")))
    {
        QByteArray name = item->text(CMUCAM4_DV_NAME).toLatin1();

        QString attrib = QString() +
        (ui->readOnlyCheckBox->isChecked() ? 'R' : '_') +
        (ui->hiddenCheckBox->isChecked() ? 'H' : '_') +
        (ui->systemCheckBox->isChecked() ? 'S' : '_') +
        (item->text(CMUCAM4_DV_ATTRIBUTES).contains(QChar('V')) ? 'V' : '_') +
        (item->text(CMUCAM4_DV_ATTRIBUTES).contains(QChar('D')) ? 'D' : '_') +
        (ui->archiveCheckBox->isChecked() ? 'A' : '_');

        QByteArray attributes = attrib.toLatin1();

        GUIFuture = QtConcurrent::run(changeAttributesWrapper,
                                      name.constData(),
                                      attributes.constData());

        if(ranGraphicalUserInterface(tr("Change Attributes Error")))
        {
            item->setText(CMUCAM4_DV_ATTRIBUTES, attrib);
        }
        else
        {
            ui->readOnlyCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                             contains(QChar('R')));
            ui->hiddenCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                           contains(QChar('H')));
            ui->systemCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                           contains(QChar('S')));
            ui->archiveCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                            contains(QChar('A')));
        }

        unlockGraphicalUserInterface();
    }
    else
    {
        ui->readOnlyCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                         contains(QChar('R')));
        ui->hiddenCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                       contains(QChar('H')));
        ui->systemCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                       contains(QChar('S')));
        ui->archiveCheckBox->setChecked(item->text(CMUCAM4_DV_ATTRIBUTES).
                                        contains(QChar('A')));
    }
}

void MainWindow::printLine()
{
    if(lockGraphicalUserInterface(tr("New File Error")))
    {
        QByteArray name = ui->newFileNameEdit->text().replace(QChar('\"'),
        QChar('_')).toLatin1();

        QByteArray text = ui->newTextToAppendEdit->text().replace(QChar('\"'),
        QChar('_')).toLatin1();

        GUIFuture = QtConcurrent::run(printLineWrapper,
                                      name.constData(),
                                      text.constData());

        bool okay = ranGraphicalUserInterface(tr("New File Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::makeDirectory()
{
    if(lockGraphicalUserInterface(tr("New Folder Error")))
    {
        QByteArray name = ui->newFolderNameEdit->text().replace(QChar('\"'),
        QChar('_')).toLatin1();

        GUIFuture = QtConcurrent::run(makeDirectoryWrapper,
                                      name.constData());

        bool okay = ranGraphicalUserInterface(tr("New Folder Error"));
        unlockGraphicalUserInterface();

        if(okay)
        {
            listDirectory();
        }
    }
}

void MainWindow::documentsHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    tr("http://www.cmucam.org/projects/cmucam4/documents/"))))
    {
        QMessageBox::critical(this, tr("Open Documents URL Error"),
        tr("Unable to open the URL to the CMUcam4 documents web page"));
    }
}

void MainWindow::wikiHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    tr("http://www.cmucam.org/projects/cmucam4/wiki/"))))
    {
        QMessageBox::critical(this, tr("Open Wiki URL Error"),
        tr("Unable to open the URL to the CMUcam4 wiki web page"));
    }
}

void MainWindow::forumsHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    tr("http://www.cmucam.org/projects/cmucam4/boards/"))))
    {
        QMessageBox::critical(this, tr("Open Forums URL Error"),
        tr("Unable to open the URL to the CMUcam4 forums web page"));
    }
}

void MainWindow::filesHelp()
{
    if(!QDesktopServices::openUrl(QUrl(
    tr("http://www.cmucam.org/projects/cmucam4/files/"))))
    {
        QMessageBox::critical(this, tr("Open Files URL Error"),
        tr("Unable to open the URL to the CMUcam4 files web page"));
    }
}

void MainWindow::aboutCMUcam4GUI()
{
    QMessageBox::about(this, tr("About CMUcam4GUI"),
    tr("<h3>About CMUcam4GUI</h3>"
       ""
       "<p>Copyright 2013 by Kwabena W. Agyeman, Anthony Rowe, and Carnegie "
       "Mellon University. All rights reserved.</p>"
       ""
       "<p>GUI powered by <a href=\"http://www.qt-project.org/\">Qt</a>.</p>"
       ""
       "<p>Serial port library provided by "
       "<a href=\"http://code.google.com/p/qextserialport/\">QextSerialPort"
       "</a>.</p>"
       ""
       "<p>Qt plotting widget provided by "
       "<a href=\"http://www.workslikeclockwork.com/index.php/components/"
       "qt-plotting-widget/\">Works Like Clockwork</a>.</p>"
       ""
       "<p>Icons courtesy of <a href=\"http://www.iconshock.com/\">Iconshock"
       "</a>.</p>"
       ""
       "<h4>Acknowledgments</h4>"
       ""
       "<p>This project was supported through the generosity of "
       "<a href=\"http://www.parallax.com/\">Parallax</a>, "
       "<a href=\"http://www.lextronic.fr/\">Lextronic</a>, "
       "<a href=\"http://www.sparkfun.com/\">SparkFun</a>, and the "
       "<a href=\"http://www.src.org/\">Semiconductor Research Corporation</a>"
       ". This project is from <a href=\"http://www.ece.cmu.edu/\">The "
       "Electrical and Computer Engineering Department</a> and the "
       "<a href=\"http://www.cmu.edu/uro/\">Undergraduate Research Office</a> "
       "at <a href=\"http://www.cmu.edu/\">Carnegie Mellon University</a>.</p>"
       ""
       "<h4>Questions or Comments?</h4>"
       ""
       "<p>Contact us at "
       "<a href=\"mailto:cmucam@cs.cmu.edu\">cmucam@cs.cmu.edu</a>.</p>"));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);

    if(event->matches(QKeySequence::Delete) && ui->diskView->hasFocus() &&
      (ui->diskView->currentItem() != NULL))
    {
        removeEntry();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    ui->imageView->fitInView(SFSBView->sceneRect(), Qt::KeepAspectRatio);
    ui->colorTrackingView->fitInView(FTView->sceneRect(), Qt::KeepAspectRatio);
}

bool MainWindow::ranGraphicalUserInterface(const QString message)
{
    // Returns true on success and false on failure.

    GUIFutureWatcher.setFuture(GUIFuture);
    GUIEventLoop.exec();

    if((GUIResult = GUIFuture.result()) <= CMUCAM4_RETURN_FAILURE)
    {
        QMessageBox::critical(this, message, errorString(GUIResult));
        return false;
    }

    return true;
}

bool MainWindow::lockGraphicalUserInterface(const QString message)
{
    // Returns true on success and false on failure.

    if(CMUcomMutex.tryLock())
    {
        CMUcam4GUIState->setText(tr("Busy "));
        return true;
    }

    QMessageBox::critical(this, message,
    tr("Please wait until the graphical user interface is not busy"));
    return false;
}

void MainWindow::unlockGraphicalUserInterface()
{
    CMUcam4GUIState->setText(tr("Ready "));
    CMUcomMutex.unlock();
}

void MainWindow::disableGraphicalUserInterace()
{
    CMUcam4Version->setText(tr("CMUcam4 Version 0.00 "));
    CMUcam4State->setText(tr("Disconnected "));

    ui->actionOpen->setEnabled(false);

    ui->actionBegin->setText(tr("Begin"));
    ui->actionBegin->setIconText(tr("Begin"));
    ui->actionBegin->setToolTip(tr("Begin"));
    ui->actionEnd->setEnabled(false);

    ui->actionSleep_Lightly->setEnabled(false);
    ui->actionSleep_Deeply->setEnabled(false);

    ui->actionDump_Frame->setEnabled(false);
    ui->actionDump_Bitmap->setEnabled(false);
    ui->actionSend_Frame->setEnabled(false);
    ui->actionSend_Bitmap->setEnabled(false);

    ui->Viewing->setEnabled(false);
    ui->Tracking->setEnabled(false);
    ui->Options->setEnabled(false);
    ui->Disk->setEnabled(false);

    resetGraphicalUserInterace();

    QPixmap pixmap(ui->colorStatisticsView->viewport().size());
    pixmap.fill(QApplication::palette().color(QPalette::Disabled,
                                              QPalette::Base));

    ui->colorStatisticsView->setAxisBackground(pixmap);
    ui->colorStatisticsView->replot();
}

void MainWindow::enableGraphicalUserInterace()
{
    GUIFuture = QtConcurrent::run(getVersionWrapper);

    if(ranGraphicalUserInterface(tr("Get Version Error")))
    {
        switch(GUIFuture)
        {
            case CMUCAM4_FIRMWARE_V100:
                CMUcam4Version->setText(tr("CMUcam4 Version 1.00 ")); break;

            case CMUCAM4_FIRMWARE_V101:
                CMUcam4Version->setText(tr("CMUcam4 Version 1.01 ")); break;

            case CMUCAM4_FIRMWARE_V102:
                CMUcam4Version->setText(tr("CMUcam4 Version 1.02 ")); break;

            case CMUCAM4_FIRMWARE_V103:
                CMUcam4Version->setText(tr("CMUcam4 Version 1.03 ")); break;
        }
    }

    CMUcam4State->setText(tr("Connected "));

    ui->actionOpen->setEnabled(true);

    ui->actionBegin->setText(tr("Reset"));
    ui->actionBegin->setIconText(tr("Reset"));
    ui->actionBegin->setToolTip(tr("Reset"));
    ui->actionEnd->setEnabled(true);

    ui->actionSleep_Lightly->setEnabled(true);
    ui->actionSleep_Deeply->setEnabled(true);

    ui->actionDump_Frame->setEnabled(true);
    ui->actionDump_Bitmap->setEnabled(true);
    ui->actionSend_Frame->setEnabled(true);
    ui->actionSend_Bitmap->setEnabled(true);

    ui->Viewing->setEnabled(true);
    ui->Tracking->setEnabled(true);
    ui->Options->setEnabled(true);
    ui->Disk->setEnabled(true);

    resetGraphicalUserInterace();

    QPixmap pixmap(ui->colorStatisticsView->viewport().size());
    pixmap.fill(QApplication::palette().color(QPalette::Normal,
                                              QPalette::Base));

    ui->colorStatisticsView->setAxisBackground(pixmap);
    ui->colorStatisticsView->replot();
}

void MainWindow::resetGraphicalUserInterace()
{
    ui->actionSave_Frame->setEnabled(CMUCAM4_DEFAULT_SAVE_SF);
    ui->actionSave_Bitmap->setEnabled(CMUCAM4_DEFAULT_SAVE_SB);

    ui->actionClear_Frame->setEnabled(CMUCAM4_DEFAULT_CLEAR_SF);
    ui->actionClear_Bitmap->setEnabled(CMUCAM4_DEFAULT_CLEAR_SB);

    // Sent Frame Boxes

    SBPixmap->setPixmap(QPixmap());
    SFPixmap->setPixmap(QPixmap());

    ui->sfHResComboBox->setCurrentIndex(CMUCAM4_DEFAULT_SF_HRES);
    ui->sfVResComboBox->setCurrentIndex(CMUCAM4_DEFAULT_SF_VRES);

    // Dump Frame Boxes

    ui->dfHResComboBox->setCurrentIndex(CMUCAM4_DEFAULT_DF_HRES);
    ui->dfVResComboBox->setCurrentIndex(CMUCAM4_DEFAULT_DF_VRES);

    // Opacity Control

    SBPixmap->setOpacity(CMUCAM4_DEFAULT_BITMAP_OPACITY);
    SFPixmap->setOpacity(CMUCAM4_DEFAULT_FRAME_OPACITY);

    ui->sentBitmapOpacitySpinBox->setValue(CMUCAM4_DEFAULT_BITMAP_OPACITY);
    ui->sentFrameOpacitySpinBox->setValue(CMUCAM4_DEFAULT_FRAME_OPACITY);

    ui->sentBitmapOpacitySpinBox->setEnabled(CMUCAM4_DEFAULT_BO);
    ui->sentFrameOpacitySpinBox->setEnabled(CMUCAM4_DEFAULT_FO);

    // Selection Control

    SFRect->setRect(QRect());

    ui->trackSelectionButton->setEnabled(CMUCAM4_DEFAULT_TS);
    ui->clearSelectionButton->setEnabled(CMUCAM4_DEFAULT_CS);

    ui->imageView->setDragMode(QGraphicsView::NoDrag);

    // Tracking Information

    TPixmap->setPixmap(QPixmap());

    ui->TCStopPushButton->setEnabled(CMUCAM4_DEFAULT_TC_STATE);
    ui->TWStopPushButton->setEnabled(CMUCAM4_DEFAULT_TW_STATE);

    ui->TWRedRangeSpinBox->setValue(CMUCAM4_DEFAULT_TW_RED_RANGE);
    ui->TWGreenRangeSpinBox->setValue(CMUCAM4_DEFAULT_TW_GREEN_RANGE);
    ui->TWBlueRangeSpinBox->setValue(CMUCAM4_DEFAULT_TW_BLUE_RANGE);

    ui->centroidMXValue->setNum(CMUCAM4_DEFAULT_TC_MX);
    ui->centroidMYValue->setNum(CMUCAM4_DEFAULT_TC_MY);
    ui->boundingBoxX1Value->setNum(CMUCAM4_DEFAULT_TC_X1);
    ui->boundingBoxY1Value->setNum(CMUCAM4_DEFAULT_TC_Y1);
    ui->boundingBoxX2Value->setNum(CMUCAM4_DEFAULT_TC_X2);
    ui->boundingBoxY2Value->setNum(CMUCAM4_DEFAULT_TC_Y2);
    ui->pixelsValue->setNum(CMUCAM4_DEFAULT_TC_PIXELS);
    ui->confidenceValue->setNum(CMUCAM4_DEFAULT_TC_CONFIDENCE);

    // Statistics Information

    RPlot->rescaleAxes();
    GPlot->rescaleAxes();
    BPlot->rescaleAxes();
    HPlot->rescaleAxes();

    RPlot->clearData();
    GPlot->clearData();
    BPlot->clearData();
    HPlot->clearData();

    ui->colorStatisticsView->xAxis->setLabel(NULL);
    ui->colorStatisticsView->xAxis->setTicks(CMUCAM4_DEFAULT_X_TICKS);
    ui->colorStatisticsView->xAxis->setTickLabels(CMUCAM4_DEFAULT_X_LABELS);
    ui->colorStatisticsView->xAxis->setGrid(CMUCAM4_DEFAULT_X_GRID);
    ui->colorStatisticsView->xAxis->setSubGrid(CMUCAM4_DEFAULT_X_SUBGRID);

    ui->colorStatisticsView->yAxis->setLabel(NULL);
    ui->colorStatisticsView->yAxis->setTicks(CMUCAM4_DEFAULT_Y_TICKS);
    ui->colorStatisticsView->yAxis->setTickLabels(CMUCAM4_DEFAULT_Y_LABELS);
    ui->colorStatisticsView->yAxis->setGrid(CMUCAM4_DEFAULT_Y_GRID);
    ui->colorStatisticsView->yAxis->setSubGrid(CMUCAM4_DEFAULT_Y_SUBGRID);

    ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeDrag,
                                            CMUCAM4_DEFAULT_DRAG_INTERACTION);

    ui->colorStatisticsView->setInteraction(QCustomPlot::iRangeZoom,
                                            CMUCAM4_DEFAULT_ZOOM_INTERACTION);

    ui->GMStopPushButton->setEnabled(CMUCAM4_DEFAULT_GM_STATE);
    ui->GHStopPushButton->setEnabled(CMUCAM4_DEFAULT_GH_STATE);

    ui->GHChannelComboBox->setCurrentIndex(CMUCAM4_DEFAULT_GH_CHANNEL);
    ui->GHBinsComboBox->setCurrentIndex(CMUCAM4_DEFAULT_GH_BINS);

    ui->redMeanValue->setNum(CMUCAM4_DEFAULT_GM_R_MEAN);
    ui->redMedianValue->setNum(CMUCAM4_DEFAULT_GM_R_MEDIAN);
    ui->redModeValue->setNum(CMUCAM4_DEFAULT_GM_R_MODE);
    ui->redStdevValue->setNum(CMUCAM4_DEFAULT_GM_R_STDEV);
    ui->greenMeanValue->setNum(CMUCAM4_DEFAULT_GM_G_MEAN);
    ui->greenMedianValue->setNum(CMUCAM4_DEFAULT_GM_G_MEDIAN);
    ui->greenModeValue->setNum(CMUCAM4_DEFAULT_GM_G_MODE);
    ui->greenStdevValue->setNum(CMUCAM4_DEFAULT_GM_G_STDEV);
    ui->blueMeanValue->setNum(CMUCAM4_DEFAULT_GM_B_MEAN);
    ui->blueMedianValue->setNum(CMUCAM4_DEFAULT_GM_B_MEDIAN);
    ui->blueModeValue->setNum(CMUCAM4_DEFAULT_GM_B_MODE);
    ui->blueStdevValue->setNum(CMUCAM4_DEFAULT_GM_B_STDEV);

    latchedChannel = CMUCAM4_DEFAULT_GH_CHANNEL;
    latchedBins = CMUCAM4_DEFAULT_GH_BINS;

    needToRescaleGetMean = CMUCAM4_DEFAULT_NTR_GM;
    needToRescaleGetHistogram = CMUCAM4_DEFAULT_NTR_GH;

    // Camera Auto Control Box

    ui->agcCheckBox->setChecked(CMUCAM4_DEFAULT_AGC);
    ui->awbCheckBox->setChecked(CMUCAM4_DEFAULT_AWB);
    ui->hmCheckBox->setChecked(CMUCAM4_DEFAULT_HM);
    ui->vfCheckBox->setChecked(CMUCAM4_DEFAULT_VF);
    ui->bwCheckBox->setChecked(CMUCAM4_DEFAULT_BW);
    ui->ngCheckBox->setChecked(CMUCAM4_DEFAULT_NG);
    ui->tmCheckBox->setChecked(CMUCAM4_DEFAULT_TM);
    ui->ctCheckBox->setChecked(CMUCAM4_DEFAULT_CT);

    // Camera Manual Control Box

    ui->brightnessSlider->setValue(CMUCAM4_DEFAULT_BRIGHTNESS);
    ui->contrastSlider->setValue(CMUCAM4_DEFAULT_CONTRAST);

    ui->brightnessValue->setNum(CMUCAM4_DEFAULT_BRIGHTNESS);
    ui->contrastValue->setNum(CMUCAM4_DEFAULT_CONTRAST);

    latchedBrightnessValue = CMUCAM4_DEFAULT_BRIGHTNESS;
    latchedContrastValue = CMUCAM4_DEFAULT_CONTRAST;

    // Servo Control Box

    ui->panGroupBox->setChecked(CMUCAM4_DEFAULT_PAN_ENABLED);
    ui->tiltGroupBox->setChecked(CMUCAM4_DEFAULT_TILT_ENABLED);

    ui->panDial->setValue(CMUCAM4_DEFAULT_PAN_PULSE_LENGTH);
    ui->tiltDial->setValue(CMUCAM4_DEFAULT_TILT_PULSE_LENGTH);

    ui->panValue->setNum(CMUCAM4_DEFAULT_PAN_PULSE_LENGTH);
    ui->tiltValue->setNum(CMUCAM4_DEFAULT_TILT_PULSE_LENGTH);

    latchedPanPulseLengthValue = CMUCAM4_DEFAULT_PAN_PULSE_LENGTH;
    latchedTiltPulseLengthValue = CMUCAM4_DEFAULT_TILT_PULSE_LENGTH;

    // Tracking Control Box

    ui->redMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
    ui->redMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);
    ui->greenMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
    ui->greenMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);
    ui->blueMinSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MIN);
    ui->blueMaxSpinBox->setValue(CMUCAM4_DEFAULT_RGB_MAX);

    ui->xMinSpinBox->setValue(CMUCAM4_MIN_NATIVE_COLUMN);
    ui->yMinSpinBox->setValue(CMUCAM4_MIN_NATIVE_ROW);
    ui->xMaxSpinBox->setValue(CMUCAM4_MAX_NATIVE_COLUMN);
    ui->yMaxSpinBox->setValue(CMUCAM4_MAX_NATIVE_ROW);

    latchedRedMinValue = CMUCAM4_DEFAULT_RGB_MIN;
    latchedRedMaxValue = CMUCAM4_DEFAULT_RGB_MAX;
    latchedGreenMinValue = CMUCAM4_DEFAULT_RGB_MIN;
    latchedGreenMaxValue = CMUCAM4_DEFAULT_RGB_MAX;
    latchedBlueMinValue = CMUCAM4_DEFAULT_RGB_MIN;
    latchedBlueMaxValue = CMUCAM4_DEFAULT_RGB_MAX;

    latchedXMinValue = CMUCAM4_MIN_NATIVE_COLUMN;
    latchedYMinValue = CMUCAM4_MIN_NATIVE_ROW;
    latchedXMaxValue = CMUCAM4_MAX_NATIVE_COLUMN;
    latchedYMaxValue = CMUCAM4_MAX_NATIVE_ROW;

    // Monitor Control Box

    ui->m0RadioButton->setChecked(CMUCAM4_DEFAULT_M0);
    ui->m1RadioButton->setChecked(CMUCAM4_DEFAULT_M1);

    ui->mfCheckBox->setChecked(CMUCAM4_DEFAULT_MF);
    ui->msCheckBox->setChecked(CMUCAM4_DEFAULT_MS);

    latchedM0State = CMUCAM4_DEFAULT_M0;
    latchedM1State = CMUCAM4_DEFAULT_M1;

    // Automatic Control Box

    ui->APGroupBox->setChecked(CMUCAM4_DEFAULT_AP);
    ui->ATGroupBox->setChecked(CMUCAM4_DEFAULT_AT);

    ui->APReversedCheckBox->setChecked(CMUCAM4_DEFAULT_AP_REV);
    ui->ATReversedCheckBox->setChecked(CMUCAM4_DEFAULT_AT_REV);

    ui->APPGainSpinBox->setValue(CMUCAM4_DEFAULT_PAN_P_GAIN);
    ui->APDGainSpinBox->setValue(CMUCAM4_DEFAULT_PAN_D_GAIN);

    ui->ATPGainSpinBox->setValue(CMUCAM4_DEFAULT_TILT_P_GAIN);
    ui->ATDGainSpinBox->setValue(CMUCAM4_DEFAULT_TILT_D_GAIN);

    latchedPanPGainValue = CMUCAM4_DEFAULT_PAN_P_GAIN;
    latchedPanDGainValue = CMUCAM4_DEFAULT_PAN_D_GAIN;

    latchedTiltPGainValue = CMUCAM4_DEFAULT_TILT_P_GAIN;
    latchedTiltDGainValue = CMUCAM4_DEFAULT_TILT_D_GAIN;

    // Pin Control Box

    ui->PPOGroupBox->setChecked(CMUCAM4_DEFAULT_PO);
    ui->TPOGroupBox->setChecked(CMUCAM4_DEFAULT_TO);

    ui->PPOLowRadioButton->setChecked(CMUCAM4_DEFAULT_PO_LOW);
    ui->PPOHighRadioButton->setChecked(CMUCAM4_DEFAULT_PO_HIGH);

    ui->TPOLowRadioButton->setChecked(CMUCAM4_DEFAULT_TO_LOW);
    ui->TPOHighRadioButton->setChecked(CMUCAM4_DEFAULT_TO_HIGH);

    latchedP0State = CMUCAM4_DEFAULT_PO_LOW;
    latchedP1State = CMUCAM4_DEFAULT_PO_HIGH;

    latchedT0State = CMUCAM4_DEFAULT_TO_LOW;
    latchedT1State = CMUCAM4_DEFAULT_TO_HIGH;

    // LED Control Box

    ui->LEDCGroupBox->setChecked(CMUCAM4_DEFAULT_LED_STATE);

    ui->LEDFrequencySpinBox->setValue(CMUCAM4_LED_ON);

    latchedLEDFrequencyValue = CMUCAM4_LED_ON;

    // Filter Control Box

    ui->invertedFilterOffButton->setChecked(CMUCAM4_DEFAULT_IF_OFF);
    ui->invertedFilterOnButton->setChecked(CMUCAM4_DEFAULT_IF_ON);

    ui->noiseFilterSpinBox->setValue(CMUCAM4_DEFAULT_NOISE_FILTER);

    latchedFilterOffState = CMUCAM4_DEFAULT_IF_OFF;
    latchedFilterOnState = CMUCAM4_DEFAULT_IF_ON;

    latchedNoiseFilterValue = CMUCAM4_DEFAULT_NOISE_FILTER;

    // Mode Control Box

    ui->pollModeCheckBox->setChecked(CMUCAM4_DEFAULT_PM);
    ui->switchingModeCheckBox->setChecked(CMUCAM4_DEFAULT_SM);
    ui->lineModeCheckBox->setChecked(CMUCAM4_DEFAULT_LM);
    ui->histogramTrackingCheckBox->setChecked(CMUCAM4_DEFAULT_HM);

    latchedPMState = CMUCAM4_DEFAULT_PM;
    latchedSMState = CMUCAM4_DEFAULT_SM;
    latchedLMState = CMUCAM4_DEFAULT_LM;
    latchedHTState = CMUCAM4_DEFAULT_HM;

    // Register Control Box

    ui->regAddressSpinBox->setValue(CMUCAM4_DEFAULT_REG_ADDRESS);
    ui->regValueSpinBox->setValue(CMUCAM4_DEFAULT_REG_VALUE);

    latchedRegAddress = CMUCAM4_DEFAULT_REG_ADDRESS;
    latchedRegValue = CMUCAM4_DEFAULT_REG_VALUE;

    // Status Control Box

    ui->StatusCGroupBox->setChecked(CMUCAM4_DEFAULT_SC);

    ui->userButtonStateValue->setNum(CMUCAM4_DEFAULT_SC_UBS);
    ui->userButtonDurationValue->setNum(CMUCAM4_DEFAULT_SC_UBD);
    ui->userButtonPressedValue->setNum(CMUCAM4_DEFAULT_SC_UBP);
    ui->userButtonReleasedValue->setNum(CMUCAM4_DEFAULT_SC_UBR);
    ui->panInputValue->setNum(CMUCAM4_DEFAULT_SC_PI);
    ui->tiltInputValue->setNum(CMUCAM4_DEFAULT_SC_TI);
    ui->panServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_PSP);
    ui->tiltServoPositionValue->setNum(CMUCAM4_DEFAULT_SC_TSP);

    statusUpdateEnable = CMUCAM4_DEFAULT_SC;

    // Disk Information Box

    ui->volumeLabelValue->setNum(CMUCAM4_DEFAULT_DI_VL);
    ui->fileSystemTypeValue->setNum(CMUCAM4_DEFAULT_DI_FST);
    ui->diskSignatureValue->setNum(CMUCAM4_DEFAULT_DI_DS);
    ui->volumeIdentificationValue->setNum(CMUCAM4_DEFAULT_DI_VI);
    ui->countOfDataSectorsValue->setNum(CMUCAM4_DEFAULT_DI_CODS);
    ui->sectorsPerClusterValue->setNum(CMUCAM4_DEFAULT_DI_SPC);
    ui->bytesPerSectorValue->setNum(CMUCAM4_DEFAULT_DI_BPS);
    ui->countOfClustersValue->setNum(CMUCAM4_DEFAULT_DI_COC);

    // Disk Space Box

    ui->freeSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_FSC);
    ui->usedSectorCountValue->setNum(CMUCAM4_DEFAULT_DS_USC);

    // New File Box

    ui->newFileNameEdit->setText(QString());
    ui->newTextToAppendEdit->setText(QString());

    // New Folder Box

    ui->newFolderNameEdit->setText(QString());

    // Name Box

    ui->entryNameEdit->setText(QString());

    // Attributes Box

    ui->readOnlyCheckBox->setChecked(CMUCAM4_DEFAULT_R_ATTRIB);
    ui->hiddenCheckBox->setChecked(CMUCAM4_DEFAULT_H_ATTRIB);
    ui->systemCheckBox->setChecked(CMUCAM4_DEFAULT_S_ATTRIB);
    ui->archiveCheckBox->setChecked(CMUCAM4_DEFAULT_A_ATTRIB);

    // Selected Items Box

    ui->selectedItemBox->setEnabled(CMUCAM4_DEFAULT_SI);

    // File Explorer Box

    ui->diskView->clear();

    ui->diskView->resizeColumnToContents(CMUCAM4_DV_NAME);
    ui->diskView->resizeColumnToContents(CMUCAM4_DV_SIZE);
    ui->diskView->resizeColumnToContents(CMUCAM4_DV_ATTRIBUTES);
}

QString MainWindow::errorString(int lastError)
{
    switch(lastError)
    {
        case CMUCAM4_RETURN_SUCCESS: return NULL;
        case CMUCAM4_RETURN_FAILURE: return
        tr("Graphical user interface programing bug detected");
        case CMUCAM4_NOT_ACTIVATED: return
        tr("Already terminated serial communication with the CMUcam4");
        case CMUCAM4_NCK_RESPONCE: return
        tr("The CMUcam4 rejected the command");
        case CMUCAM4_UNSUPPORTED_VERSION: return
        tr("Unsupported CMUcam4 firmware detected");
        case CMUCAM4_UNEXPECTED_RESPONCE: return
        tr("The CMUcam4 responded unexpectedly");
        case CMUCAM4_COMMAND_OVERFLOW: return
        tr("The serial command buffer overflowed");
        case CMUCAM4_RESPONCE_OVERFLOW: return
        tr("The serial responce buffer overflowed");
        case CMUCAM4_STREAM_END: return
        tr("The CMUcam4 is not streaming data packets");
        case CMUCAM4_SERIAL_TIMEOUT: return
        tr("The CMUcam4 is not responding");
        case CMUCAM4_CAMERA_TIMEOUT_ERROR: return
        tr("An error occured with the camera module's parallel data bus");
        case CMUCAM4_CAMERA_CONNECTION_ERROR: return
        tr("An error occured with the camera module's serial data bus");
        case CMUCAM4_DISK_IO_ERROR: return
        tr("A disk input output error occured with the micro SD card");
        case CMUCAM4_FILE_SYSTEM_CORRUPTED: return
        tr("The micro SD card file system is corrupted");
        case CMUCAM4_FILE_SYSTEM_UNSUPPORTED: return
        tr("The micro SD card file system is unsupported");
        case CMUCAM4_CARD_NOT_DETECTED: return
        tr("A micro SD card was not detected");
        case CMUCAM4_DISK_MAY_BE_FULL: return
        tr("The disk may be full");
        case CMUCAM4_DIRECTORY_FULL: return
        tr("The directory is full");
        case CMUCAM4_EXPECTED_AN_ENTRY: return
        tr("Could not evaluate the name of the target file or directory");
        case CMUCAM4_EXPECTED_A_DIRECTORY: return
        tr("Could not evaluate the name of an intermediate directory");
        case CMUCAM4_ENTRY_NOT_ACCESSIBLE: return
        tr("Cannot access the file or directory");
        case CMUCAM4_ENTRY_NOT_MODIFIABLE: return
        tr("Cannot modify the file or directory");
        case CMUCAM4_ENTRY_NOT_FOUND: return
        tr("The file or directory was not found");
        case CMUCAM4_ENTRY_ALREADY_EXISTS: return
        tr("The file or directory already exists");
        case CMUCAM4_DIRECTORY_LINK_MISSING: return
        tr("The directory link is missing");
        case CMUCAM4_DIRECTORY_NOT_EMPTY: return
        tr("The directory is not empty");
        case CMUCAM4_NOT_A_DIRECTORY: return
        tr("Please select a directory instead of a file");
        case CMUCAM4_NOT_A_FILE: return
        tr("Please select a file instead of a directory");
        default: return NULL;
    }

    return NULL;
}

/*******************************************************************************
* State Functions
*******************************************************************************/

int beginWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.begin();
}

int endWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.end();
}

/*******************************************************************************
* System Level Commands
*******************************************************************************/

int getVersionWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getVersion();
}

int resetSystemWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.resetSystem();
}

int sleepDeeplyWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.sleepDeeply();
}

int sleepLightlyWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.sleepLightly();
}

/*******************************************************************************
* Camera Module Commands
*******************************************************************************/

int cameraBrightnessWrapper(int brightness)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.cameraBrightness(brightness);
}

int cameraContrastWrapper(int contrast)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.cameraContrast(contrast);
}

int cameraRegisterReadWrapper(int reg)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.cameraRegisterRead(reg);
}

int cameraRegisterWriteWrapper(int reg, int value, int mask)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.cameraRegisterWrite(reg, value, mask);
}

/*******************************************************************************
* Camera Sensor Auto Control Commands
*******************************************************************************/

int autoGainControlWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.autoGainControl(active);
}

int autoWhiteBalanceWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.autoWhiteBalance(active);
}

/*******************************************************************************
* Camera Format Commands
*******************************************************************************/

int horizontalMirrorWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.horizontalMirror(active);
}

int verticalFlipWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.verticalFlip(active);
}

/*******************************************************************************
* Camera Effect Commands
*******************************************************************************/

int blackAndWhiteModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.blackAndWhiteMode(active);
}

int negativeModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.negativeMode(active);
}

/*******************************************************************************
* Auxiliary I/O Commands
*******************************************************************************/

int getButtonStateWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getButtonState();
}

int getButtonDurationWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return (int) CMUcam.getButtonDuration();
}

int getButtonPressedWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getButtonPressed();
}

int getButtonReleasedWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getButtonReleased();
}

int panInputWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.panInput();
}

int panOutputWrapper(int direction, int output)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.panOutput(direction, output);
}

int tiltInputWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.tiltInput();
}

int tiltOutputWrapper(int direction, int output)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.tiltOutput(direction, output);
}

int getInputsWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getInputs();
}

int setOutputsWrapper(int directions, int outputs)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setOutputs(directions, outputs);
}

int LEDOffWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.LEDOff();
}

int LEDOnWrapper(int frequency)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.LEDOn((long) frequency);
}

/*******************************************************************************
* Servo Commands
*******************************************************************************/

int getServoPositionWrapper(int servo)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getServoPosition(servo);
}

int setServoPositionWrapper(int servo, int active, int pulseLength)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setServoPosition(servo, active, pulseLength);
}

int automaticPanWrapper(int active, int reverse)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.automaticPan(active, reverse);
}

int automaticTiltWrapper(int active, int reverse)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.automaticTilt(active, reverse);
}

int autoPanParameters(int proportionalGain, int derivativeGain)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.autoPanParameters(proportionalGain, derivativeGain);
}

int autoTiltParameters(int proportionalGain, int derivativeGain)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.autoTiltParameters(proportionalGain, derivativeGain);
}

/*******************************************************************************
* Television Commands
*******************************************************************************/

int monitorOffWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.monitorOff();
}

int monitorOnWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.monitorOn();
}

int monitorFreezeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.monitorFreeze(active);
}

int monitorSignalWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.monitorSignal(active);
}

/*******************************************************************************
* Color Tracking Commands
*******************************************************************************/

int getTrackingParametersWrapper(CMUcam4_tracking_parameters_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTrackingParameters(pointer);
}

int getTrackingWindowWrapper(CMUcam4_tracking_window_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTrackingWindow(pointer);
}

int setTrackingParametersWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setTrackingParameters();
}

int setTrackingParametersWrapper(int * min, int * max)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setTrackingParameters(min[CMUCAM4_ST_R_MIN],
                                        max[CMUCAM4_ST_R_MAX],
                                        min[CMUCAM4_ST_G_MIN],
                                        max[CMUCAM4_ST_G_MAX],
                                        min[CMUCAM4_ST_B_MIN],
                                        max[CMUCAM4_ST_B_MAX]);
}

int setTrackingWindowWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setTrackingWindow();
}

int setTrackingWindowWrapper(int topLeftX, int topLeftY,
                             int bottomRightX, int bottomRightY)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.setTrackingWindow(topLeftX, topLeftY,
                                    bottomRightX, bottomRightY);
}

int idleCameraWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.idleCamera();
}

int trackColorWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.trackColor();
}

int trackColorWrapper(int * min, int * max)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.trackColor(min[CMUCAM4_TC_R_MIN],
                             max[CMUCAM4_TC_R_MAX],
                             min[CMUCAM4_TC_G_MIN],
                             max[CMUCAM4_TC_G_MAX],
                             min[CMUCAM4_TC_B_MIN],
                             max[CMUCAM4_TC_B_MAX]);
}

int trackWindowWrapper(int redRange, int greenRange, int blueRange)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.trackWindow(redRange, greenRange, blueRange);
}

int getHistogramWrapper(int channel, int bins)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getHistogram(channel, bins);
}

int getMeanWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getMean();
}

int getTypeFDataPacketWrapper(CMUcam4_image_data_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeFDataPacket(pointer);
}

int getTypeHDataPacketWrapper1(CMUcam4_histogram_data_1_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper2(CMUcam4_histogram_data_2_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper4(CMUcam4_histogram_data_4_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper8(CMUcam4_histogram_data_8_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper16(CMUcam4_histogram_data_16_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper32(CMUcam4_histogram_data_32_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeHDataPacketWrapper64(CMUcam4_histogram_data_64_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeHDataPacket(pointer);
}

int getTypeSDataPacketWrapper(CMUcam4_statistics_data_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeSDataPacket(pointer);
}

int getTypeTDataPacketWrapper(CMUcam4_tracking_data_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.getTypeTDataPacket(pointer);
}

int pollModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.pollMode(active);
}

int lineModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.lineMode(active);
}

int switchingModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.switchingMode(active);
}

int testModeWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.testMode(active);
}

int colorTrackingWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.colorTracking(active);
}

int histogramTrackingWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.histogramTracking(active);
}

int invertedFilterWrapper(int active)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.invertedFilter(active);
}

int noiseFilterWrapper(int threshold)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.noiseFilter(threshold);
}

/*******************************************************************************
* File System Commands
*******************************************************************************/

int changeAttributesWrapper(const char * fileOrDirectoryPathName,
                            const char * attributes)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.changeAttributes(fileOrDirectoryPathName,
                                   attributes);
}

int changeDirectoryWrapper(const char * directoryPathAndName)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.changeDirectory(directoryPathAndName);
}

int diskInformationWrapper(CMUcam4_disk_information_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.diskInformation(pointer);
}

int diskSpaceWrapper(CMUcam4_disk_space_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.diskSpace(pointer);
}

int formatDiskWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.formatDisk();
}

int listDirectoryWrapper(CMUcam4_directory_entry_t * pointer,
                         size_t size, unsigned int offset)
{
    QMutexLocker lock(&CMUcamMutex);
    return (int) CMUcam.listDirectory(pointer,
                                      size, (unsigned long) offset);
}

int makeDirectoryWrapper(const char * directoryPathAndName)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.makeDirectory(directoryPathAndName);
}

int moveEntryWrapper(const char * oldEntryPathAndName,
                     const char * newEntryPathAndName)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.moveEntry(oldEntryPathAndName,
                            newEntryPathAndName);
}

int printLineWrapper(const char * filePathAndName, const char * textToAppend)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.printLine(filePathAndName, textToAppend);
}

int filePrintWrapper(const char * filePathAndName, uint8_t * buffer,
                     size_t size, unsigned int offset)
{
    QMutexLocker lock(&CMUcamMutex);
    return (int) CMUcam.filePrint(filePathAndName, buffer,
                                  size, (unsigned long) offset);
}

int removeEntryWrapper(const char * fileOrDirectoryPathAndName)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.removeEntry(fileOrDirectoryPathAndName);
}

int unmountDiskWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.unmountDisk();
}

/*******************************************************************************
* Image Capture Commands
*******************************************************************************/

int dumpBitmapWrapper()
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.dumpBitmap();
}

int dumpFrameWrapper(int horizontalResolution, int verticalResolution)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.dumpFrame(horizontalResolution, verticalResolution);
}

int sendBitmapWrapper(CMUcam4_image_data_t * pointer)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.sendBitmap(pointer);
}

int sendFrameWrapper(int horizontalResolution, int verticalResolution,
                     uint16_t * buffer,
                     size_t horizonalSize,
                     size_t verticalSize)
{
    QMutexLocker lock(&CMUcamMutex);
    return CMUcam.sendFrame(horizontalResolution, verticalResolution,
                            buffer,
                            horizonalSize, CMUCAM4_SF_H_OFFSET,
                            verticalSize, CMUCAM4_SF_V_OFFSET);
}

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
