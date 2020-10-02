#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <complex>
#include <QMainWindow>
#include "simfigure.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct PLOTOBJECT {
    QwtPlotItem *itemPtr;   // pointer to the PlotItem
    int          index;     // layer index
};

class QwtPlot;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    PLOTOBJECT itemAt( const QPoint& pos ) const;
    ~MainWindow();

signals:
    void doubleVsValueChanged(double doubleValue);
    void intVsValueChanged(int intValue);
    void doubleHsValueChanged(double doubleValue);
    void intHsValueChanged(int intValue);
    void doubleDampingValueChanged(double doubleValue);
    void intDampingValueChanged(int intValue);
    void doubleFrequencyValueChanged(double doubleValue);
    void intFrequencyValueChanged(int intValue);
    void doubleDensityValueChanged(double doubleValue);
    void intDensityValueChanged(int intValue);

private slots:
    void on_loadMotion_clicked();
    void on_btn_earthquake_clicked();
    void on_btn_sine_clicked();
    void on_btn_sweep_clicked();
    void on_btn_loadMotion_clicked();
    void updateSpinBox();
    void harmonicRecord(double f);

    void notifyVsIntValueChanged(int value) {
        double doubleValue = value / 10.0;
        emit doubleVsValueChanged(doubleValue);
    }

    void notifyVsDoubleValueChanged(double value) {
        int intValue = value * 10;
        emit intVsValueChanged(intValue);
    }

    void notifyHsIntValueChanged(int value) {
        double doubleValue = value / 10.0;
        emit doubleHsValueChanged(doubleValue);
    }

    void notifyHsDoubleValueChanged(double value) {
        int intValue = value * 10;
        emit intHsValueChanged(intValue);
    }

    void notifyDampingIntValueChanged(int value) {
        double doubleValue = value / 100.0;
        emit doubleDampingValueChanged(doubleValue);
    }

    void notifyDampingDoubleValueChanged(double value) {
        int intValue = value * 100;
        emit intDampingValueChanged(intValue);
    }

    void notifyFrequencyIntValueChanged(int value) {
        double doubleValue = value / 100.0;
        emit doubleFrequencyValueChanged(doubleValue);
    }

    void notifyFrequencyDoubleValueChanged(double value) {
        int intValue = value * 100;
        emit intFrequencyValueChanged(intValue);
    }

    void notifyDensityIntValueChanged(int value) {
        double doubleValue = value / 10.0;
        emit doubleDensityValueChanged(doubleValue);
    }

    void notifyDensityDoubleValueChanged(double value) {
        int intValue = value * 10;
        emit intDensityValueChanged(intValue);
    }

    void on_MotionSelectioncomboBox_currentIndexChanged(int index);
    void on_addRowBtn_clicked();
    void on_delRowBtn_clicked();
    void onTableViewUpdated();
    void updateLayerID();
    void on_picker_appended(const QPoint &pos);
    void setActiveLayer(const QModelIndex &index);
    void setActiveLayer(int index);
    void on_vsSpinBox_valueChanged(double arg1);
    void on_densitySpinBox_valueChanged(double arg1);
    void on_thicknessSpinBox_valueChanged(double arg1);
    void on_dampingSpinBox_valueChanged(double arg1);
    void on_resetFigureBtn_clicked();
    void on_lockAxischeckBox_stateChanged(int arg1);

    void on_actionProvide_feedback_triggered();
    void on_actionVersion_triggered();
    void on_actionCopyright_triggered();
    void on_action_License_triggered();
    void on_action_About_triggered();
    void on_actionDocumentation_triggered();

private:
    Ui::MainWindow *ui;

    void initialTableView();
    void updatePlots(bool updateInputMotionFlag = true);
    void loadFile(const QString &fileName);
    void plotLayers();

    void updateSoilTF();
    void calculate();
    void fft(QVector<double> ts, QVector<std::complex<double> >& fas);
    void ifft(QVector<std::complex<double>> fas, QVector<double>& ts);
    void setFreq();
    void setTime();
    void sweepRecord();
    void readGM(QJsonArray accTH, double dT, double accUnit);

    bool m_lockAxesFlag = false;
    int m_activeLayer = 1;
    double m_defaultThickness = 5.0;
    double m_defaultVs = 200.0;
    double m_defaultDamping = 3.0;
    double m_defaultDensity = 2.0;
    double m_dt;
    int m_labelFont;

    QVector<double> m_accInput;
    QVector<double> m_accOutput;
    QVector<double> m_absFft;
    QVector<double> m_absIFft;
    QVector<double> m_time;
    QVector<double> m_freq;
    QVector<std::complex<double>> m_soilTF;
    QVector<double> m_absSoilTF;
    QVector<double> m_xUpLimits;
    QVector<double> m_xLowLimits;
    QVector<double> m_yUpLimits;
    QVector<double> m_yLowLimits;


    QwtPlot *m_layerPlot;
    QList<SimFigure*> m_figureList;

    QMenu *fileMenu;
    QMenu *editMenu;
    QWidget *centralWidget;
    QWidget *LayerWidget;

    QString currentFile;
    QList<PLOTOBJECT> plotItemList;
};

static QVector<QColor> BRUSH_COLOR({QColor(255, 127, 0, 127),
                                    QColor(191,  95, 0, 127),
                                    QColor(127,  63, 0, 127),
                                    QColor(127, 255, 0, 127),
                                    QColor( 95, 191, 0, 127),
                                    QColor( 63, 127, 0, 127),
                                    QColor(127, 255, 255, 255),
                                    QColor( 95, 191, 255, 255),
                                    QColor( 63, 127, 255, 255),
                                    QColor(127, 255, 255, 127),
                                    QColor( 95, 191, 255, 127),
                                    QColor( 63, 127, 255, 127),
                                    QColor(255, 127, 0, 63),
                                    QColor(191,  95, 0, 63),
                                    QColor(127,  63, 0, 63),
                                    QColor(127, 255, 0, 63),
                                    QColor( 95, 191, 0, 63),
                                    QColor( 63, 127, 0, 63)
                                   });

#endif // MAINWINDOW_H
