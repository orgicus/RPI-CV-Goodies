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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define QESP_NO_PORTABILITY_WARN true

#include <QMainWindow>

#include <QtCore>
#include <QtGui>

#include "CMUcam4.h"
#include "CMUcom4.h"

#include "qextserialport.h"
#include "qextserialenumerator.h"

#include "QCustomPlot/qcustomplot.h"

// System Menu Defaults

#define CMUCAM4_DEFAULT_GUI_TAB 0

#define CMUCAM4_DEFAULT_SAVE_SF false
#define CMUCAM4_DEFAULT_SAVE_SB false

#define CMUCAM4_DEFAULT_CLEAR_SF false
#define CMUCAM4_DEFAULT_CLEAR_SB false

// Sent Frame Boxes

#define CMUCAM4_DEFAULT_SF_HRES 0
#define CMUCAM4_DEFAULT_SF_VRES 0

// Dump Frame Boxes

#define CMUCAM4_DEFAULT_DF_HRES 0
#define CMUCAM4_DEFAULT_DF_VRES 0

// Opacity Control

#define CMUCAM4_DEFAULT_BITMAP_OPACITY 0.5
#define CMUCAM4_DEFAULT_FRAME_OPACITY 1.0

#define CMUCAM4_DEFAULT_BO false
#define CMUCAM4_DEFAULT_FO false

// Selection Control

#define CMUCAM4_DEFAULT_TS false
#define CMUCAM4_DEFAULT_CS false

// Tracking Information

#define CMUCAM4_DEFAULT_TC_STATE false
#define CMUCAM4_DEFAULT_TW_STATE false

#define CMUCAM4_DEFAULT_TW_RED_RANGE 30
#define CMUCAM4_DEFAULT_TW_GREEN_RANGE 30
#define CMUCAM4_DEFAULT_TW_BLUE_RANGE 30

#define CMUCAM4_DEFAULT_TC_MX 0
#define CMUCAM4_DEFAULT_TC_MY 0
#define CMUCAM4_DEFAULT_TC_X1 0
#define CMUCAM4_DEFAULT_TC_Y1 0
#define CMUCAM4_DEFAULT_TC_X2 0
#define CMUCAM4_DEFAULT_TC_Y2 0
#define CMUCAM4_DEFAULT_TC_PIXELS 0
#define CMUCAM4_DEFAULT_TC_CONFIDENCE 0

// Statistics Information

#define CMUCAM4_DEFAULT_X_AT false
#define CMUCAM4_DEFAULT_X_ATL false
#define CMUCAM4_DEFAULT_X_R -90.0

#define CMUCAM4_DEFAULT_X_TL 0
#define CMUCAM4_DEFAULT_X_STL 0

#define CMUCAM4_DEFAULT_Y_AT true
#define CMUCAM4_DEFAULT_Y_ATL true
#define CMUCAM4_DEFAULT_Y_R +00.0

#define CMUCAM4_DEFAULT_X2_VISIBLE true
#define CMUCAM4_DEFAULT_X2_T false
#define CMUCAM4_DEFAULT_X2_TL false

#define CMUCAM4_DEFAULT_Y2_VISIBLE true
#define CMUCAM4_DEFAULT_Y2_T false
#define CMUCAM4_DEFAULT_Y2_TL false

#define CMUCAM4_DEFAULT_X_TICKS false
#define CMUCAM4_DEFAULT_X_LABELS false
#define CMUCAM4_DEFAULT_X_GRID false
#define CMUCAM4_DEFAULT_X_SUBGRID false

#define CMUCAM4_DEFAULT_Y_TICKS false
#define CMUCAM4_DEFAULT_Y_LABELS false
#define CMUCAM4_DEFAULT_Y_GRID false
#define CMUCAM4_DEFAULT_Y_SUBGRID false

#define CMUCAM4_DEFAULT_DRAG_INTERACTION false
#define CMUCAM4_DEFAULT_ZOOM_INTERACTION false

#define CMUCAM4_DEFAULT_GM_STATE false
#define CMUCAM4_DEFAULT_GH_STATE false

#define CMUCAM4_DEFAULT_GH_CHANNEL 0
#define CMUCAM4_DEFAULT_GH_BINS 0

#define CMUCAM4_DEFAULT_GM_R_MEAN 0
#define CMUCAM4_DEFAULT_GM_R_MEDIAN 0
#define CMUCAM4_DEFAULT_GM_R_MODE 0
#define CMUCAM4_DEFAULT_GM_R_STDEV 0
#define CMUCAM4_DEFAULT_GM_G_MEAN 0
#define CMUCAM4_DEFAULT_GM_G_MEDIAN 0
#define CMUCAM4_DEFAULT_GM_G_MODE 0
#define CMUCAM4_DEFAULT_GM_G_STDEV 0
#define CMUCAM4_DEFAULT_GM_B_MEAN 0
#define CMUCAM4_DEFAULT_GM_B_MEDIAN 0
#define CMUCAM4_DEFAULT_GM_B_MODE 0
#define CMUCAM4_DEFAULT_GM_B_STDEV 0

#define CMUCAM4_DEFAULT_NTR_GM true
#define CMUCAM4_DEFAULT_NTR_GH true

// Camera Auto Control Box

#define CMUCAM4_DEFAULT_AGC true
#define CMUCAM4_DEFAULT_AWB true
#define CMUCAM4_DEFAULT_HM false
#define CMUCAM4_DEFAULT_VF false
#define CMUCAM4_DEFAULT_BW false
#define CMUCAM4_DEFAULT_NG false
#define CMUCAM4_DEFAULT_TM false
#define CMUCAM4_DEFAULT_CT false

// Camera Manual Control Box

#define CMUCAM4_DEFAULT_BRIGHTNESS 0
#define CMUCAM4_DEFAULT_CONTRAST 0

// Servo Control Box

#define CMUCAM4_DEFAULT_PAN_ENABLED false
#define CMUCAM4_DEFAULT_TILT_ENABLED false

#define CMUCAM4_DEFAULT_PAN_PULSE_LENGTH 1500
#define CMUCAM4_DEFAULT_TILT_PULSE_LENGTH 1500

// Tracking Control Box

#define CMUCAM4_DEFAULT_RGB_MIN 0
#define CMUCAM4_DEFAULT_RGB_MAX 255

// Monitor Control Box

#define CMUCAM4_DEFAULT_M0 false
#define CMUCAM4_DEFAULT_M1 true

#define CMUCAM4_DEFAULT_MF true
#define CMUCAM4_DEFAULT_MS false

// Automatic Control Box

#define CMUCAM4_DEFAULT_AP false
#define CMUCAM4_DEFAULT_AT false

#define CMUCAM4_DEFAULT_AP_REV false
#define CMUCAM4_DEFAULT_AT_REV false

#define CMUCAM4_DEFAULT_PAN_P_GAIN 400
#define CMUCAM4_DEFAULT_PAN_D_GAIN 100

#define CMUCAM4_DEFAULT_TILT_P_GAIN 400
#define CMUCAM4_DEFAULT_TILT_D_GAIN 100

// Pin Control Box

#define CMUCAM4_DEFAULT_PO false
#define CMUCAM4_DEFAULT_TO false

#define CMUCAM4_DEFAULT_PO_LOW true
#define CMUCAM4_DEFAULT_PO_HIGH false

#define CMUCAM4_DEFAULT_TO_LOW true
#define CMUCAM4_DEFAULT_TO_HIGH false

// LED Control Box

#define CMUCAM4_DEFAULT_LED_STATE true

// Filter Control Box

#define CMUCAM4_DEFAULT_IF_OFF true
#define CMUCAM4_DEFAULT_IF_ON false

#define CMUCAM4_DEFAULT_NOISE_FILTER 0

// Mode Control Box

#define CMUCAM4_DEFAULT_PM false
#define CMUCAM4_DEFAULT_SM false
#define CMUCAM4_DEFAULT_LM false
#define CMUCAM4_DEFAULT_HM false

// Register Control Box

#define CMUCAM4_DEFAULT_REG_ADDRESS 0
#define CMUCAM4_DEFAULT_REG_VALUE 0
#define CMUCAM4_DEFAULT_REG_MASK 255

// Status Control Box

#define CMUCAM4_DEFAULT_SC false

#define CMUCAM4_DEFAULT_SC_UBS 0
#define CMUCAM4_DEFAULT_SC_UBD 0
#define CMUCAM4_DEFAULT_SC_UBP 0
#define CMUCAM4_DEFAULT_SC_UBR 0
#define CMUCAM4_DEFAULT_SC_PI 0
#define CMUCAM4_DEFAULT_SC_TI 0
#define CMUCAM4_DEFAULT_SC_PSP 0
#define CMUCAM4_DEFAULT_SC_TSP 0

// Disk Information Box

#define CMUCAM4_DEFAULT_DI_VL 0
#define CMUCAM4_DEFAULT_DI_FST 0
#define CMUCAM4_DEFAULT_DI_DS 0
#define CMUCAM4_DEFAULT_DI_VI 0
#define CMUCAM4_DEFAULT_DI_CODS 0
#define CMUCAM4_DEFAULT_DI_SPC 0
#define CMUCAM4_DEFAULT_DI_BPS 0
#define CMUCAM4_DEFAULT_DI_COC 0

// Disk Space Box

#define CMUCAM4_DEFAULT_DS_FSC 0
#define CMUCAM4_DEFAULT_DS_USC 0

// Attributes Box

#define CMUCAM4_DEFAULT_R_ATTRIB false
#define CMUCAM4_DEFAULT_H_ATTRIB false
#define CMUCAM4_DEFAULT_S_ATTRIB false
#define CMUCAM4_DEFAULT_A_ATTRIB false

// Selected Items Box

#define CMUCAM4_DEFAULT_SI false

// Generic Constants

#define CMUCAM4_DV_NAME 0
#define CMUCAM4_DV_SIZE 1
#define CMUCAM4_DV_ATTRIBUTES 2
#define CMUCAM4_DV_NUMBER 65536

#define CMUCAM4_FP_SIZE 1048576
#define CMUCAM4_FP_OFFSET 0

#define CMUCAM4_ST_R_MIN 0
#define CMUCAM4_ST_G_MIN 1
#define CMUCAM4_ST_B_MIN 2
#define CMUCAM4_ST_R_MAX 0
#define CMUCAM4_ST_G_MAX 1
#define CMUCAM4_ST_B_MAX 2

#define CMUCAM4_TC_R_MIN 0
#define CMUCAM4_TC_G_MIN 1
#define CMUCAM4_TC_B_MIN 2
#define CMUCAM4_TC_R_MAX 0
#define CMUCAM4_TC_G_MAX 1
#define CMUCAM4_TC_B_MAX 2

#define CMUCAM4_SF_H_OFFSET 0
#define CMUCAM4_SF_V_OFFSET 0

namespace Ui
{
    class MainWindow;
    class MyQGraphicsScene;
}

class MainWindow;
class MyQGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = NULL);
    ~MainWindow();

public slots:

    void begin();
    void end();

    void sleepLightly();
    void sleepDeeply();

    void dumpBitmap();
    void dumpFrame();
    void sendBitmap();
    void sendFrame();

    void open();

    void clearBitmap();
    void clearFrame();
    void saveBitmap();
    void saveFrame();

    void sentBitmapOpacity(double opacity);
    void sentFrameOpacity(double opacity);

    void trackSelection();
    void clearSelection();
    void changedSelection();

    void automaticGainControl();
    void automaticWhiteBalance();
    void horizontalMirror();
    void verticalFlip();
    void blackAndWhiteMode();
    void negativeMode();
    void testMode();
    void colorTracking();

    void brightnessApply();
    void brightnessReset();
    void contrastApply();
    void contrastReset();

    void panDialEnable();
    void panDialApply();
    void panDialReset();
    void tiltDialEnable();
    void tiltDialApply();
    void tiltDialReset();

    void setTrackingParametersApply();
    void setTrackingParametersReset();
    void getTrackingParameters();
    void setTrackingWindowApply();
    void setTrackingWindowReset();
    void getTrackingWindow();

    void monitorOff();
    void monitorOn();
    void monitorFreeze();
    void monitorSignal();

    void automaticPan();
    void automaticPanReversed();
    void automaticPanApply();
    void automaticPanReset();
    void automaticTilt();
    void automaticTiltReversed();
    void automaticTiltApply();
    void automaticTiltReset();

    void panPinOutput();
    void panPinLow();
    void panPinHigh();
    void tiltPinOutput();
    void tiltPinLow();
    void tiltPinHigh();

    void LEDPower();
    void LEDFrequencyApply();
    void LEDFrequencyReset();

    void invertedFilter();
    void noiseFilterApply();
    void noiseFilterReset();

    void pollMode();
    void switchingMode();
    void lineMode();
    void histogramTracking();

    void registerRead();
    void registerWrite();

    void statusEnable();
    void statusUpdate();

    void trackColor();
    void trackWindow();
    void getMean();
    void getHistogram();
    void idleCamera();
    void getTypeDataPacket();

    void diskInformation();
    void diskSpace();
    void formatDisk();
    void unmountDisk();

    void itemActivated();
    void itemActivated(QTreeWidgetItem *item, int column);
    void currentItemChanged(QTreeWidgetItem *current,
                            QTreeWidgetItem *previous);

    void removeEntry();
    void moveEntry();
    void changeAttributes();

    void listDirectory();
    void changeDirectory(QTreeWidgetItem *item);
    void filePrint(QTreeWidgetItem *item);
    void removeEntry(QTreeWidgetItem *item);
    void moveEntry(QTreeWidgetItem *item);
    void changeAttributes(QTreeWidgetItem *item);
    void printLine();
    void makeDirectory();

    void documentsHelp();
    void wikiHelp();
    void forumsHelp();
    void filesHelp();
    void aboutCMUcam4GUI();
    void aboutQt();

private:

    int latchedChannel;
    int latchedBins;

    int latchedBrightnessValue;
    int latchedContrastValue;

    int latchedPanPulseLengthValue;
    int latchedTiltPulseLengthValue;

    int latchedRedMinValue;
    int latchedRedMaxValue;
    int latchedGreenMinValue;
    int latchedGreenMaxValue;
    int latchedBlueMinValue;
    int latchedBlueMaxValue;

    int latchedXMinValue;
    int latchedYMinValue;
    int latchedXMaxValue;
    int latchedYMaxValue;

    bool latchedM0State;
    bool latchedM1State;

    int latchedPanPGainValue;
    int latchedPanDGainValue;

    int latchedTiltPGainValue;
    int latchedTiltDGainValue;

    bool latchedP0State;
    bool latchedP1State;

    bool latchedT0State;
    bool latchedT1State;

    int latchedLEDFrequencyValue;

    bool latchedFilterOffState;
    bool latchedFilterOnState;

    int latchedNoiseFilterValue;

    bool latchedPMState;
    bool latchedSMState;
    bool latchedLMState;
    bool latchedHTState;

    int latchedRegAddress;
    int latchedRegValue;

    bool statusUpdateEnable;

    enum {GB, GD, GP, GR, PI, TI, GTS, GPS} statusUpdateRRobin;

    enum {TYPE_F, TYPE_H, TYPE_S, TYPE_T} getType, dataPacket;

    bool needToRescaleGetMean;
    bool needToRescaleGetHistogram;

    Ui::MainWindow *ui;

    QLabel *CMUcam4State;
    QLabel *CMUcam4GUIState;
    QLabel *CMUcam4Version;
    QLabel *CMUcam4GUIVersion;

    MyQGraphicsScene *SFSBView;
    QGraphicsPixmapItem *SBPixmap;
    QGraphicsPixmapItem *SFPixmap;
    QGraphicsRectItem *SFRect;
    QPainterPath SFPath;

    QGraphicsScene *FTView;
    QGraphicsPixmapItem *TPixmap;
    QPixmap FPixmap;

    QCPStatisticalBox *RPlot;
    QCPStatisticalBox *GPlot;
    QCPStatisticalBox *BPlot;
    QCPBars *HPlot;

    CMUcam4_tracking_data_t trackingData;
    CMUcam4_statistics_data_t statisticsData;
    CMUcam4_histogram_data_1_t histogramData1;
    CMUcam4_histogram_data_2_t histogramData2;
    CMUcam4_histogram_data_4_t histogramData4;
    CMUcam4_histogram_data_8_t histogramData8;
    CMUcam4_histogram_data_16_t histogramData16;
    CMUcam4_histogram_data_32_t histogramData32;
    CMUcam4_histogram_data_64_t histogramData64;
    CMUcam4_image_data_t imageData;

    QEventLoop statusEventLoop;
    QFutureWatcher<int> statusFutureWatcher;
    QFuture<int> statusFuture;
    int statusResult;

    QEventLoop streamEventLoop;
    QFutureWatcher<int> streamFutureWatcher;
    QFuture<int> streamFuture;
    int streamResult;

    QEventLoop GUIEventLoop;
    QFutureWatcher<int> GUIFutureWatcher;
    QFuture<int> GUIFuture;
    int GUIResult;

    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

    bool ranGraphicalUserInterface(const QString message);
    bool lockGraphicalUserInterface(const QString message);
    void unlockGraphicalUserInterface();

    void disableGraphicalUserInterace();
    void enableGraphicalUserInterace();
    void resetGraphicalUserInterace();

    QString errorString(int lastError);
};

class MyQGraphicsScene : public QGraphicsScene
{
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        QGraphicsScene::mousePressEvent(event);

        emit selectionChanged();
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        QGraphicsScene::mouseReleaseEvent(event);

        emit selectionChanged();
    }
};

/*******************************************************************************
* State Functions
*******************************************************************************/

int beginWrapper();
int endWrapper();

/*******************************************************************************
* System Level Commands
*******************************************************************************/

int getVersionWrapper();
int resetSystemWrapper();
int sleepDeeplyWrapper();
int sleepLightlyWrapper();

/*******************************************************************************
* Camera Module Commands
*******************************************************************************/

int cameraBrightnessWrapper(int brightness);
int cameraContrastWrapper(int contrast);
int cameraRegisterReadWrapper(int reg);
int cameraRegisterWriteWrapper(int reg, int value, int mask);

/*******************************************************************************
* Camera Sensor Auto Control Commands
*******************************************************************************/

int autoGainControlWrapper(int active);
int autoWhiteBalanceWrapper(int active);

/*******************************************************************************
* Camera Format Commands
*******************************************************************************/

int horizontalMirrorWrapper(int active);
int verticalFlipWrapper(int active);

/*******************************************************************************
* Camera Effect Commands
*******************************************************************************/

int blackAndWhiteModeWrapper(int active);
int negativeModeWrapper(int active);

/*******************************************************************************
* Auxiliary I/O Commands
*******************************************************************************/

int getButtonStateWrapper();
int getButtonDurationWrapper();
int getButtonPressedWrapper();
int getButtonReleasedWrapper();
int panInputWrapper();
int panOutputWrapper(int direction, int output);
int tiltInputWrapper();
int tiltOutputWrapper(int direction, int output);
int getInputsWrapper();
int setOutputsWrapper(int directions, int outputs);
int LEDOffWrapper();
int LEDOnWrapper(int frequency);

/*******************************************************************************
* Servo Commands
*******************************************************************************/

int getServoPositionWrapper(int servo);
int setServoPositionWrapper(int servo, int active, int pulseLength);
int automaticPanWrapper(int active, int reverse);
int automaticTiltWrapper(int active, int reverse);
int autoPanParameters(int proportionalGain, int derivativeGain);
int autoTiltParameters(int proportionalGain, int derivativeGain);

/*******************************************************************************
* Television Commands
*******************************************************************************/

int monitorOffWrapper();
int monitorOnWrapper();
int monitorFreezeWrapper(int active);
int monitorSignalWrapper(int active);

/*******************************************************************************
* Color Tracking Commands
*******************************************************************************/

int getTrackingParametersWrapper(CMUcam4_tracking_parameters_t *pointer);
int getTrackingWindowWrapper(CMUcam4_tracking_window_t *pointer);
int setTrackingParametersWrapper();
int setTrackingParametersWrapper(int *min, int *max);
int setTrackingWindowWrapper();
int setTrackingWindowWrapper(int topLeftX, int topLeftY,
                             int bottomRightX, int bottomRightY);
int idleCameraWrapper();
int trackColorWrapper();
int trackColorWrapper(int *min, int *max);
int trackWindowWrapper(int redRange, int greenRange, int blueRange);
int getHistogramWrapper(int channel, int bins);
int getMeanWrapper();
int getTypeFDataPacketWrapper(CMUcam4_image_data_t *pointer);
int getTypeHDataPacketWrapper1(CMUcam4_histogram_data_1_t *pointer);
int getTypeHDataPacketWrapper2(CMUcam4_histogram_data_2_t *pointer);
int getTypeHDataPacketWrapper4(CMUcam4_histogram_data_4_t *pointer);
int getTypeHDataPacketWrapper8(CMUcam4_histogram_data_8_t *pointer);
int getTypeHDataPacketWrapper16(CMUcam4_histogram_data_16_t *pointer);
int getTypeHDataPacketWrapper32(CMUcam4_histogram_data_32_t *pointer);
int getTypeHDataPacketWrapper64(CMUcam4_histogram_data_64_t *pointer);
int getTypeSDataPacketWrapper(CMUcam4_statistics_data_t *pointer);
int getTypeTDataPacketWrapper(CMUcam4_tracking_data_t *pointer);
int pollModeWrapper(int active);
int lineModeWrapper(int active);
int switchingModeWrapper(int active);
int testModeWrapper(int active);
int colorTrackingWrapper(int active);
int histogramTrackingWrapper(int active);
int invertedFilterWrapper(int active);
int noiseFilterWrapper(int threshold);

/*******************************************************************************
* File System Commands
*******************************************************************************/

int changeAttributesWrapper(const char *fileOrDirectoryPathName,
                            const char *attributes);
int changeDirectoryWrapper(const char *directoryPathAndName);
int diskInformationWrapper(CMUcam4_disk_information_t *pointer);
int diskSpaceWrapper(CMUcam4_disk_space_t *pointer);
int formatDiskWrapper();
int listDirectoryWrapper(CMUcam4_directory_entry_t *pointer,
                         size_t size, unsigned int offset);
int makeDirectoryWrapper(const char *directoryPathAndName);
int moveEntryWrapper(const char *oldEntryPathAndName,
                     const char *newEntryPathAndName);
int printLineWrapper(const char *filePathAndName, const char *textToAppend);
int filePrintWrapper(const char *filePathAndName, uint8_t *buffer,
                     size_t size, unsigned int offset);
int removeEntryWrapper(const char *fileOrDirectoryPathAndName);
int unmountDiskWrapper();

/*******************************************************************************
* Image Capture Commands
*******************************************************************************/

int dumpBitmapWrapper();
int dumpFrameWrapper(int horizontalResolution, int verticalResolution);
int sendBitmapWrapper(CMUcam4_image_data_t *pointer);
int sendFrameWrapper(int horizontalResolution, int verticalResolution,
                     uint16_t *buffer,
                     size_t horizonalSize,
                     size_t verticalSize);

#endif // MAINWINDOW_H

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
