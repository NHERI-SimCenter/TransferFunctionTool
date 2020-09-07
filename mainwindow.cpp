#define _USE_MATH_DEFINES

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <mkl_dfti.h>

#include <QRect>
#include <QGuiApplication>
#include <QTabWidget>
#include <QScreen>
#include <QVBoxLayout>
#include <HeaderWidget.h>
#include <FooterWidget.h>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDoubleSpinBox>
#include <QtDebug>
#include <QLabel>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "qwt_plot.h"
#include "qwt_picker.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_shapeitem.h"
#include "qwt_plot_curve.h"
#include "Utils/dialogabout.h"
#include <algorithm>

#define MIN(vec) *std::min_element(vec.constBegin(), vec.constEnd())
#define MAX(vec) *std::max_element(vec.constBegin(), vec.constEnd())


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = this->height()<int(0.80*rec.height())?int(0.80*rec.height()):this->height();
    int width  = this->width()<int(0.60*rec.width())?int(0.60*rec.width()):this->width();
    this->resize(width, height);

    // ------------------------------------------------------------------------
    // set up plot for figure tab
    ui->TransferFunctionFig_TabLayer->showAxisControls(false);
    // ui->TransferFunctionFig->setMinimumHeight(150);
    ui->TransferFunctionFig_TabLayer->setXLabel("Freq. [Hz]");
    ui->TransferFunctionFig_TabLayer->setYLabel("[H]");

    // ------------------------------------------------------------------------
    // set up plot for layers
    plot = new QwtPlot(this);
    QGridLayout *plotLayout = new QGridLayout();
    plotLayout->addWidget(plot, 0, 0);
    ui->layerGroupBox->setLayout(plotLayout);

    plot->setCanvasBackground(QBrush(Qt::white));
    plot->setAxisScale(QwtPlot::xBottom, 0, 1, 1);
    plot->setAxisScale(QwtPlot::yLeft, 0, m_defaultThickness, 0.5);
    plot->enableAxis(QwtPlot::xBottom, false);

    QwtText text;
    text.setText("Thickness (m)");
    QFont layerPlotfont;
    layerPlotfont.setPointSize(10);
    text.setFont(layerPlotfont);
    plot->setAxisTitle(QwtPlot::yLeft, text);

    //Picker
    QwtPicker *picker = new QwtPicker(plot->canvas());
    picker->setStateMachine(new QwtPickerClickPointMachine);
    picker->setTrackerMode(QwtPicker::AlwaysOff);
    picker->setRubberBand(QwtPicker::RectRubberBand);
    connect(picker, SIGNAL(appended(const QPoint &)), this, SLOT(on_picker_appended(const QPoint &)));

    // ------------------------------------------------------------------------
    // Add figures to figures tab
    ui->AccOutputFig->showAxisControls(false);
    ui->AccOutputFig->setXLabel("Time [s]");
    ui->AccOutputFig->setYLabel("Accel. [g]");
    int sz = int(0.6 * ui->AccOutputFig->labelFontSize());
    ui->AccOutputFig->setMaximumHeight(0.15 * rec.height());
    ui->AccOutputFig->grid(true,false);
    ui->AccOutputFig->setTickFontSize(sz);

    ui->FourierOutputFig->showAxisControls(false);
    ui->FourierOutputFig->setXLabel("Freq. [Hz]");
    ui->FourierOutputFig->setYLabel("FA [g-s]");
    ui->FourierOutputFig->setMaximumHeight(0.15 * rec.height());
    ui->FourierOutputFig->grid(true,false);
    ui->FourierOutputFig->setTickFontSize(sz);

    ui->TransferFunctionFig->showAxisControls(false);
    ui->TransferFunctionFig->setXLabel("Freq. [Hz]");
    ui->TransferFunctionFig->setYLabel("[H]");
    ui->TransferFunctionFig->setXLim(0, 25);
    ui->TransferFunctionFig->setMaximumHeight(0.15 * rec.height());
    ui->TransferFunctionFig->grid(true,false);
    ui->TransferFunctionFig->setTickFontSize(sz);

    ui->FourierInputFig->showAxisControls(false);
    ui->FourierInputFig->setXLabel("Freq. [Hz]");
    ui->FourierInputFig->setYLabel("FA [g-s]");
    ui->FourierInputFig->setMaximumHeight(0.15 * rec.height());
    ui->FourierInputFig->grid(true,false);
    ui->FourierInputFig->setTickFontSize(sz);

    ui->AccInputFig->showAxisControls(false);
    ui->AccInputFig->setXLabel("Time [s]");
    ui->AccInputFig->setYLabel("Accel. [g]");
    ui->AccInputFig->setMaximumHeight(0.15 * rec.height());
    ui->AccInputFig->grid(true,false);
    ui->AccInputFig->setTickFontSize(sz);

    m_xUpLimits.resize(4);
    m_xLowLimits.resize(4);
    m_yUpLimits.resize(4);
    m_yLowLimits.resize(4);

    // initial values
    double max_damping = 30;
    double max_Hs = 200;
    double max_Vs = 1000;
    double max_frequency = 10.0;
    double frequency = 1.0;
    double max_density = 5.0;

    // ------------------------------------------------------------------------
    // Controls Boxes

    ui->vsSpinBox->setRange(1, max_Vs);
    ui->vsSpinBox->setValue(m_defaultVs);
    ui->vsSpinBox->setSingleStep(0.1);

    ui->vsSlider->setRange(10, max_Vs * 10);
    ui->vsSlider->setValue(m_defaultVs * 10);

    connect(ui->vsSpinBox, SIGNAL(valueChanged(double)),this,SLOT(notifyVsDoubleValueChanged(double)));
    connect(this, SIGNAL(intVsValueChanged(int)), ui->vsSlider, SLOT(setValue(int)));
    connect(ui->vsSlider,SIGNAL(valueChanged(int)),this,SLOT(notifyVsIntValueChanged(int)));
    connect(this, SIGNAL(doubleVsValueChanged(double)), ui->vsSpinBox, SLOT(setValue(double)));

    ui->thicknessSpinBox->setRange(0.1, max_Hs);
    ui->thicknessSpinBox->setValue(m_defaultThickness);
    ui->thicknessSpinBox->setSingleStep(0.1);

    ui->thicknessSlider->setRange(1, max_Hs * 10);
    ui->thicknessSlider->setValue(m_defaultThickness * 10);

    connect(ui->thicknessSpinBox, SIGNAL(valueChanged(double)),this,SLOT(notifyHsDoubleValueChanged(double)));
    connect(this, SIGNAL(intHsValueChanged(int)), ui->thicknessSlider, SLOT(setValue(int)));
    connect(ui->thicknessSlider,SIGNAL(valueChanged(int)),this,SLOT(notifyHsIntValueChanged(int)));
    connect(this, SIGNAL(doubleHsValueChanged(double)), ui->thicknessSpinBox, SLOT(setValue(double)));

    ui->dampingSpinBox->setRange(0.01, max_damping);
    ui->dampingSpinBox->setValue(m_defaultDamping);
    ui->dampingSpinBox->setSingleStep(0.01);

    ui->dampingSlider->setRange(1, max_damping * 100);
    ui->dampingSlider->setValue(m_defaultDamping * 100);

    connect(ui->dampingSpinBox, SIGNAL(valueChanged(double)),this,SLOT(notifyDampingDoubleValueChanged(double)));
    connect(this, SIGNAL(intDampingValueChanged(int)), ui->dampingSlider, SLOT(setValue(int)));
    connect(ui->dampingSlider,SIGNAL(valueChanged(int)),this,SLOT(notifyDampingIntValueChanged(int)));
    connect(this, SIGNAL(doubleDampingValueChanged(double)), ui->dampingSpinBox, SLOT(setValue(double)));

    ui->frequencySpinBox->setRange(0.01, max_frequency);
    ui->frequencySpinBox->setValue(frequency);
    ui->frequencySpinBox->setSingleStep(0.01);
    ui->frequencySpinBox->setReadOnly(true);
    ui->frequencySpinBox->setStyleSheet("QDoubleSpinBox {background-color: lightGray; color: white;}");

    ui->frequencySlider->setRange(50, max_frequency * 100);
    ui->frequencySlider->setValue(frequency * 100);
    ui->frequencySlider->setEnabled(false);

    connect(ui->frequencySpinBox, SIGNAL(valueChanged(double)),this,SLOT(notifyFrequencyDoubleValueChanged(double)));
    connect(this, SIGNAL(intFrequencyValueChanged(int)), ui->frequencySlider, SLOT(setValue(int)));
    connect(ui->frequencySlider,SIGNAL(valueChanged(int)),this,SLOT(notifyFrequencyIntValueChanged(int)));
    connect(this, SIGNAL(doubleFrequencyValueChanged(double)), ui->frequencySpinBox, SLOT(setValue(double)));
    connect(this, SIGNAL(doubleFrequencyValueChanged(double)), this, SLOT(harmonicRecord(double)));


    ui->densitySpinBox->setRange(0.1, max_density);
    ui->densitySpinBox->setValue(m_defaultDensity);
    ui->densitySpinBox->setSingleStep(0.1);

    ui->densitySlider->setRange(1, max_density * 10);
    ui->densitySlider->setValue(m_defaultDensity * 10);

    connect(ui->densitySpinBox, SIGNAL(valueChanged(double)),this,SLOT(notifyDensityDoubleValueChanged(double)));
    connect(this, SIGNAL(intDensityValueChanged(int)), ui->densitySlider, SLOT(setValue(int)));
    connect(ui->densitySlider,SIGNAL(valueChanged(int)),this,SLOT(notifyDensityIntValueChanged(int)));
    connect(this, SIGNAL(doubleDensityValueChanged(double)), ui->densitySpinBox, SLOT(setValue(double)));

    // ------------------------------------------------------------------------
    // add some connections
    // connect(ui->tableView->m_sqlModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)), this, SLOT(onTableViewUpdated()));
    connect(ui->tableView, SIGNAL(cellClicked(const QModelIndex&)), this, SLOT(setActiveLayer(const QModelIndex&)));
    // connect(ui->tableView->itemDelegate(),SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)),SLOT(updateSpinBox()));


    ui->loadMotion->setEnabled(false);
    ui->userMotionFile->setReadOnly(true);
    ui->userMotionFile->setStyleSheet("QLineEdit {background-color: lightGray; color: white;}");

    this->initialTableView();

    ui->MotionSelectioncomboBox->addItems({"Motion 1", "RSN766", "RSN963", "RSN1203", "ElCentro", "Rinaldi"});
    ui->MotionSelectioncomboBox->setCurrentIndex(0);

    ui->btn_earthquake->setChecked(true);
    this->loadFile(":/resources/motions/motion1.json");
    this->onTableViewUpdated();
    this->updatePlots();
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialTableView()
{
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setMinimumHeight(30);
    QList<QVariant> valueListLayer;
    valueListLayer << QString ("Layer %1").arg(ui->tableView->m_sqlModel->rowCount() + 1) << m_defaultThickness << m_defaultDensity << m_defaultVs << DefaultEType << m_defaultDamping;
    ui->tableView->insertAt(valueListLayer,0);
    ui->tableView->setTotalHeight(m_defaultThickness);
    ui->totalLayerLineEdit->setText("1");
    ui->totalHeight->setText(QString::number(ui->tableView->totalHeight()));
    ui->activeLayerlineEdit->setText(QString::number(m_activeLayer));
}

void MainWindow::on_actionProvide_feedback_triggered()
{
    QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}

void MainWindow::on_actionVersion_triggered()
{
    QString versionText("Version 1.0");
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(versionText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindow::on_actionCopyright_triggered()
{
    QMessageBox msgBox;
    QString copyrightText = QString("\
                                    <p>\
                                    The source code is licensed under a BSD 2-Clause License:<p>\
                                    \"Copyright (c) 2017-2019, The Regents of the University of California (Regents).\"\
                                    All rights reserved.<p>\
                                    <p>\
                                    Redistribution and use in source and binary forms, with or without \
                                    modification, are permitted provided that the following conditions are met:\
                                    <p>\
                                    1. Redistributions of source code must retain the above copyright notice, this\
                                    list of conditions and the following disclaimer.\
                                    \
                                    \
                                    2. Redistributions in binary form must reproduce the above copyright notice,\
                                    this list of conditions and the following disclaimer in the documentation\
                                    and/or other materials provided with the distribution.\
                                    <p>\
                                    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\
                                    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\
                                    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\
                                    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\
                                    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\
                                    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\
                                    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\
            ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\
            SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\
            <p>\
            The views and conclusions contained in the software and documentation are those\
            of the authors and should not be interpreted as representing official policies,\
            either expressed or implied, of the FreeBSD Project.\
            <p>\
            REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \
            THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\
            THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS \
            PROVIDED \"AS IS\". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,\
            UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            The compiled binary form of this application is licensed under a GPL Version 3 license.\
            The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
            included in the packaging of this application. \
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            This software makes use of the QT packages (unmodified): core, gui, widgets and network\
                                                                     <p>\
                                                                     QT is copyright \"The Qt Company Ltd&quot; and licensed under the GNU Lesser General \
                                                                     Public License (version 3) which references the GNU General Public License (version 3)\
      <p>\
      The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
      included in the packaging of this application. \
      <p>\
      ");

      QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(copyrightText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}

void MainWindow::on_action_License_triggered()
{

}

void MainWindow::on_action_About_triggered()
{
    DialogAbout *dlg = new DialogAbout();
    QString aboutTitle("A SimCenter Tool For Transfer Function Calculation");
    QString aboutSource = ":/resources/textAboutthis.html";
    dlg->setTitle(aboutTitle);
    dlg->setTextSource(aboutSource);

    //
    // adjust size of application window to the available display
    //
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = 0.50*rec.height();
    int width  = 0.50*rec.width();
    dlg->resize(width, height);

    dlg->exec();
    delete dlg;
}


void MainWindow::on_btn_earthquake_clicked()
{
    ui->MotionSelectioncomboBox->setEnabled(true);
    ui->frequencySpinBox->setReadOnly(true);
    ui->frequencySpinBox->setStyleSheet("QDoubleSpinBox {background-color: lightGray; color: white;}");
    ui->frequencySlider->setEnabled(false);
    ui->loadMotion->setEnabled(false);
    ui->userMotionFile->setReadOnly(true);
    ui->userMotionFile->setStyleSheet("QLineEdit {background-color: lightGray; color: white;}");
    ui->MotionSelectioncomboBox->currentIndex();
    this->loadFile(QString (":/resources/motions/motion%1.json").arg(ui->MotionSelectioncomboBox->currentIndex() + 1));
    this->onTableViewUpdated();
    this->updatePlots(true);
    ui->tabWidget->setCurrentIndex(1);
}


void MainWindow::on_MotionSelectioncomboBox_currentIndexChanged(int index)
{
    this->loadFile(QString (":/resources/motions/motion%1.json").arg(index + 1));
    this->onTableViewUpdated();
    this->updatePlots();
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_sine_clicked()
{
    ui->MotionSelectioncomboBox->setEnabled(false);
    ui->frequencySpinBox->setReadOnly(false);
    ui->frequencySpinBox->setStyleSheet("QDoubleSpinBox {background-color: lightGray; color: black;}");
    ui->frequencySlider->setEnabled(true);
    ui->loadMotion->setEnabled(false);
    ui->userMotionFile->setReadOnly(true);
    ui->userMotionFile->setStyleSheet("QLineEdit {background-color: lightGray; color: white;}");
    this->harmonicRecord(ui->frequencySpinBox->value());
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_sweep_clicked()
{
    ui->MotionSelectioncomboBox->setEnabled(false);
    ui->frequencySpinBox->setReadOnly(true);
    ui->frequencySpinBox->setStyleSheet("QDoubleSpinBox {background-color: lightGray; color: white;}");
    ui->frequencySlider->setEnabled(false);
    ui->loadMotion->setEnabled(false);
    ui->userMotionFile->setReadOnly(true);
    ui->userMotionFile->setStyleSheet("QLineEdit {background-color: lightGray; color: white;}");
    this->sweepRecord();
    this->updatePlots();
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_loadMotion_clicked()
{
    ui->MotionSelectioncomboBox->setEnabled(false);
    ui->loadMotion->setEnabled(true);
    ui->userMotionFile->setReadOnly(false);
    ui->userMotionFile->setReadOnly(false);
    ui->userMotionFile->setStyleSheet("QLineEdit {background-color: lightGray; color: black;}");
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_loadMotion_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    ui->frequencySpinBox->setReadOnly(true);
    ui->frequencySpinBox->setStyleSheet("QDoubleSpinBox {background-color: lightGray; color: white;}");
    ui->frequencySlider->setEnabled(false);
    if (!fileName.isEmpty())
        this->loadFile(fileName);
    this->onTableViewUpdated();
    this->updatePlots();
    ui->userMotionFile->setText(fileName);
    ui->tabWidget->setCurrentIndex(1);
}

// ------------------------------------------------------------------------
// define motions
void MainWindow::harmonicRecord(double f)
{
    int nPoints = 4000;
    double s = 0;
    m_dt = 0.01;
    m_accInput.resize(nPoints);

    for (int i=0; i < nPoints; i++) {
        s+=m_dt;
        m_accInput[i] = 0.4 * sin(2 * f * M_PI * s);
    }

    this->setTime();
    this->setFreq();
    this->updateSoilTF();
    this->calculate();
    this->updatePlots(true);
}

void MainWindow::sweepRecord()
{
    int nPoints = 8000;
    double time;
    m_dt = 0.001;
    m_accInput.resize(nPoints);
    m_time.resize(nPoints);

    for (int i=0; i<m_time.size();i++){
        time = i * m_dt;
        m_time[i]= time;
        m_accInput[i] = sin(25.0 * time + 100.0 * (time * time / 2.0) / 8.0);
    }
    this->setFreq();
    this->updateSoilTF();
    this->calculate();
}

void MainWindow::setFreq()
{
    m_freq.resize(m_accInput.size()/2+1);
    // double dfreq = 1 / ( 2 * m_dt * (freq.size() - 1 ) );
    double sampleFreq = 1.0 / m_dt;
    for (int i = 0; i < m_freq.size(); i++ ) {
        m_freq[i] = i * sampleFreq / m_accInput.size();
    }
}

void MainWindow::setTime()
{
    //Define time QVector
    m_time.resize(m_accInput.size());
    for (int i=0; i<m_time.size();i++){
        m_time[i]=  i * m_dt;
    }
}


void MainWindow::loadFile(const QString &fileName)
{
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // place contents of file into json object
    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    QJsonArray events = jsonObj["Events"].toArray();

    if (events.size()>0)
    {
        QJsonArray patterns = events[0].toObject()["pattern"].toArray();
        QJsonArray timeseries = events[0].toObject()["timeSeries"].toArray();
        QString type = patterns[0].toObject()["type"].toString();
        QString tsname = patterns[0].toObject()["timeSeries"].toString();

        QJsonObject units = events[0].toObject()["units"].toObject();
        double accUnit = 1.0;
        if(!units.isEmpty()){
            QString accType = units["acc"].toString();
            if (accType=="g")
                accUnit = 1.0;
            else if (accType=="m/s2")
                accUnit = 1.0 / 9.81;
            else if (accType=="cm/s2" || accType=="gal" || accType=="Gal")
                accUnit = 1.0 / 981.0;
        }
        double dT = timeseries[0].toObject()["dT"].toDouble();
        QJsonArray accTH = timeseries[0].toObject()["data"].toArray();
        this->readGM(accTH, dT, accUnit);
    }
}

void MainWindow::readGM(QJsonArray accTH, double dT, double accUnit)
{
    int nPoints = accTH.size();
    m_dt = dT;
    m_accInput.clear();
    for (int ii = 0; ii < nPoints; ii++){
        m_accInput.append(accTH[ii].toDouble() * accUnit);
    }
    if (nPoints % 2 == 0) m_accInput.append(0.0);
    this->setTime();
    this->setFreq();
};

void MainWindow::updatePlots(bool updateInputMotionFlag)
{
    ui->AccOutputFig->clear();
    int sz = int(0.7 * ui->AccOutputFig->labelFontSize());
    ui->AccOutputFig->setLabelFontSize(sz);

    ui->FourierOutputFig->clear();
    ui->FourierOutputFig->setLabelFontSize(sz);

    ui->TransferFunctionFig->clear();
    ui->TransferFunctionFig->setLabelFontSize(sz);

    ui->TransferFunctionFig_TabLayer->clear();
    // ui->TransferFunctionFig_TabLayer->setLabelFontSize(int(0.8 * ui->TransferFunctionFig_TabLayer->labelFontSize()));

    ui->AccOutputFig->plot(m_time, m_accOutput, SimFigure::LineType::Solid, Qt::blue);
    ui->FourierOutputFig->plot(m_freq, m_absIFft, SimFigure::LineType::Solid, Qt::black);
    ui->TransferFunctionFig->plot(m_freq, m_soilTF, SimFigure::LineType::Solid, Qt::red);
    ui->TransferFunctionFig_TabLayer->plot(m_freq, m_soilTF, SimFigure::LineType::Solid, Qt::red);

    if (updateInputMotionFlag) {
        ui->FourierInputFig->clear();
        ui->FourierInputFig->setLabelFontSize(sz);

        ui->AccInputFig->clear();
        ui->AccInputFig->setLabelFontSize(sz);

        ui->FourierInputFig->plot(m_freq, m_absFft, SimFigure::LineType::Solid, Qt::black);
        ui->AccInputFig->plot(m_time, m_accInput, SimFigure::LineType::Solid, Qt::blue);
    }

    if (!m_lockAxesFlag) {
        ui->AccOutputFig->setXlimits(0, m_time.back());
        ui->FourierOutputFig->setXlimits(0, 25);
        ui->TransferFunctionFig->setXlimits(0, 25);
        ui->TransferFunctionFig_TabLayer->setXlimits(0, 25);
        if (updateInputMotionFlag) {
            ui->FourierInputFig->setXlimits(0, 25);
            ui->AccInputFig->setXlimits(0, m_time.back());
        }
    } else {
        ui->TransferFunctionFig_TabLayer->setXlimits(m_xLowLimits[0], m_xUpLimits[0]);
        ui->TransferFunctionFig_TabLayer->setYlimits(m_yLowLimits[0], m_yUpLimits[0]);
        qDebug() << m_yUpLimits[0];
        ui->TransferFunctionFig->setXlimits(m_xLowLimits[1], m_xUpLimits[1]);
        ui->TransferFunctionFig->setYlimits(m_yLowLimits[1], m_yUpLimits[1]);
        ui->FourierOutputFig->setXlimits(m_xLowLimits[2], m_xUpLimits[2]);
        ui->FourierOutputFig->setYlimits(m_yLowLimits[2], m_yUpLimits[2]);
        ui->AccOutputFig->setXlimits(m_xLowLimits[3], m_xUpLimits[3]);
        ui->AccOutputFig->setYlimits(m_yLowLimits[3], m_yUpLimits[3]);
    }
}

void MainWindow::on_addRowBtn_clicked()
{
    QList<QVariant> valueListLayer;
    valueListLayer << QString ("Layer %1").arg(ui->tableView->m_sqlModel->rowCount() + 1) << m_defaultThickness << m_defaultDensity << m_defaultVs << DefaultEType << m_defaultDamping;
    emit ui->tableView->insertAbove(valueListLayer);
    ui->totalLayerLineEdit->setText(QString::number(ui->tableView->m_sqlModel->rowCount()));
    this->updateLayerID();
    this->onTableViewUpdated();
    this->updatePlots(false);
    // plotLayers();
}

void MainWindow::on_delRowBtn_clicked()
{
    if (ui->tableView->m_sqlModel->rowCount() > 1) {
        emit ui->tableView->removeOneRow(1);
        ui->totalLayerLineEdit->setText(QString::number(ui->tableView->m_sqlModel->rowCount()));
        ui->totalHeight->setText(QString::number(ui->tableView->m_sqlModel->getTotalHeight()));
        this->updateLayerID();
        this->onTableViewUpdated();
        this->updatePlots(false);
        // plotLayers();
    }
}

void MainWindow::updateLayerID()
{
    for (int i = 1; i <= ui->tableView->m_sqlModel->rowCount();i++) {
        ui->tableView->m_sqlModel->editData(i-1, LAYERNAME, QString ("Layer %1").arg(i));
    }
    ui->totalHeight->setText(QString::number(ui->tableView->totalHeight()));
    ui->tableView->update();
}

void MainWindow::onTableViewUpdated()
{

    // updateSpinBox();
    // emit ui->tableView->onCellSingleClicked(ui->tableView->model()->index(m_activeLayer - 1, 1));
    this->plotLayers();
    this->updateSoilTF();
    this->calculate();
}

void MainWindow::setActiveLayer(const QModelIndex &index)
{
    m_activeLayer = index.row() + 1;
    ui->activeLayerlineEdit->setText(QString::number(m_activeLayer));
    ui->tableView->setFocus();
    ui->tableView->selectionModel()->select(index, QItemSelectionModel::Select);
    this->plotLayers();
    this->updateSpinBox();
}

void MainWindow::setActiveLayer(int index)
{
    m_activeLayer = index;
    emit ui->tableView->onCellSingleClicked(ui->tableView->model()->index(m_activeLayer - 1, 1));
    // ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_vsSpinBox_valueChanged(double arg1)
{
    if (ui->tableView->m_sqlModel->rowCount() >= m_activeLayer) {
        ui->tableView->m_sqlModel->editData(m_activeLayer - 1, VS, arg1);
        ui->tableView->update();
        this->onTableViewUpdated();
        this->updatePlots(false);
    }
}

void MainWindow::on_densitySpinBox_valueChanged(double arg1)
{
    if (ui->tableView->m_sqlModel->rowCount() >= m_activeLayer) {
        ui->tableView->m_sqlModel->editData(m_activeLayer - 1, DENSITY, arg1);
        ui->tableView->update();
        this->onTableViewUpdated();
        this->updatePlots(false);
    }
}

void MainWindow::on_thicknessSpinBox_valueChanged(double arg1)
{
    if (ui->tableView->m_sqlModel->rowCount() >= m_activeLayer) {
        ui->tableView->m_sqlModel->editData(m_activeLayer - 1, THICKNESS, arg1);
        ui->tableView->update();
        this->onTableViewUpdated();
        this->updatePlots(false);
    }
}

void MainWindow::on_dampingSpinBox_valueChanged(double arg1)
{
    if (ui->tableView->m_sqlModel->rowCount() >= m_activeLayer) {
        ui->tableView->m_sqlModel->editData(m_activeLayer - 1, ESIZE, arg1); // element size for damping
        ui->tableView->update();
        this->onTableViewUpdated();
        this->updatePlots(false);
    }
}

void MainWindow::updateSpinBox()
{
    ui->vsSpinBox->setValue(ui->tableView->m_sqlModel->getVS(m_activeLayer-1));
    ui->densitySpinBox->setValue(ui->tableView->m_sqlModel->getDensity(m_activeLayer-1));
    ui->dampingSpinBox->setValue(ui->tableView->m_sqlModel->getESize(m_activeLayer-1));
    ui->thicknessSpinBox->setValue(ui->tableView->m_sqlModel->getThickness(m_activeLayer-1));
}

// ------------------------------------------------------------------------
// calculate transfer function
void MainWindow::updateSoilTF()
{
    double Psi, Psi1, HH, Vs, Vs1, Rho, Rho1, aux;
    int nPt = m_freq.size();
    double dfreq = m_freq[1] - m_freq[0];
    QVector<double> freq(nPt), omega(nPt);
    QVector<std::complex<double> > mAA(nPt), mBB(nPt);
    QVector<std::complex<double> > mAA1(nPt), mBB1(nPt);
    QVector<std::complex<double> > mAAaux(nPt), mBBaux(nPt);
    QVector<double> Nfreq(nPt);

    m_soilTF.resize(nPt);

    std::complex<double> kstar;
    std::complex<double> Vsstar;
    std::complex<double> kHstar;
    std::complex<double> alphastar;
    std::complex<double> mExpA;
    std::complex<double> mExpB;
    std::complex<double> One;
    std::complex<double> AAA;

    One = std::complex<double>(1,0);
    freq[0]=0.0;
    for (int i=1; i<m_freq.size();i++){
        freq[i]  = freq[i-1]+dfreq;
        omega[i] = 2.0*M_PI*freq[i];
        mAAaux[i]=std::complex<double>(1,0);
        mBBaux[i]=std::complex<double>(1,0);
    }
    m_freq = freq;
    mAA = mAAaux; mBB = mBBaux;

    // set dummy value for rock layer
    int numLayers = ui->tableView->m_sqlModel->rowCount() + 1;
    QVector<QString> vsVec = ui->tableView->m_sqlModel->getvsVec();
    vsVec.append(QString::number(DefaultVs));
    QVector<QString> dampingVec = ui->tableView->m_sqlModel->getesizeVec();  // damping
    dampingVec.append("2");
    QVector<QString> thicknessVec = ui->tableView->m_sqlModel->getthicknessVec();
    thicknessVec.append(QString::number(DefaultThickness));
    QVector<QString> rhoVec = ui->tableView->m_sqlModel->getdensityVec();
    rhoVec.append(QString::number(DefaultDensity));

    if (numLayers == 0)
        return;

    for(int i = 0; i<numLayers-1; i++){
        Psi = dampingVec[i].toDouble() /100.0;
        Psi1 = dampingVec[i+1].toDouble() /100.0;
        HH  = thicknessVec[i].toDouble();
        Vs = vsVec[i].toDouble();
        Vs1 = vsVec[i+1].toDouble();
        Rho = rhoVec[i].toDouble();
        Rho1 = rhoVec[i+1].toDouble();
        mAAaux = mAA; mBBaux = mBB;

        aux = Rho*Vs/(Rho1*Vs1)/(1+Psi1*Psi1);
        alphastar = std::complex<double>(aux*(1+Psi1*Psi1), -aux*(Psi1-Psi));

        for (int ii = 0; ii < freq.size(); ii++ ){
            double km = 2.0* M_PI *freq[ii]/Vs/(1.0+Psi*Psi);
            double kmHH = km*HH;
            kstar = std::complex<double>(km, -km*Psi);
            Vsstar = std::complex<double>(Vs, Vs*Psi);
            kHstar = std::complex<double>(km*HH*Psi, km*HH);
            mExpA = std::exp(kHstar);
            mExpB = std::exp(-kHstar);
            mAA[ii] = 0.5*mAAaux[ii]*(One+alphastar)*mExpA + 0.5*mBBaux[ii]*(One-alphastar)*mExpB;
            mBB[ii] = 0.5*mAAaux[ii]*(One-alphastar)*mExpA + 0.5*mBBaux[ii]*(One+alphastar)*mExpB;
        }
        if (i == 0){
            mAA1 = mAA;
            mBB1 = mBB;
        }
    }

    m_soilTF[0] = 1.0;

    for (int i=1; i<m_freq.size();i++){
        AAA = One/(mAA[i]+mBB[i]);
        m_soilTF[i]= 2*abs(AAA);
    }
    // calculateNatFreq(nPt, maxfreq);
}

// ------------------------------------------------------------------------
// calcualte FFT and IFFT
void MainWindow::fft(QVector<double> ts, QVector<std::complex<double>>& fas)
{

    DFTI_DESCRIPTOR_HANDLE descriptor;
    MKL_LONG status;

    status = DftiCreateDescriptor(&descriptor, DFTI_DOUBLE, DFTI_REAL, 1, ts.size()); //Specify size and precision
    status = DftiSetValue(descriptor, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    status = DftiSetValue(descriptor, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
    status = DftiCommitDescriptor(descriptor); //Finalize the descriptor
    status = DftiComputeForward(descriptor, ts.data(), fas.data()); //Compute the Forward FFT
    status = DftiFreeDescriptor(&descriptor); //Free the descriptor
}

void MainWindow::ifft(QVector<std::complex<double>> fas, QVector<double>& ts)
{
    DFTI_DESCRIPTOR_HANDLE descriptor;
    MKL_LONG status;

    status = DftiCreateDescriptor(&descriptor, DFTI_DOUBLE, DFTI_REAL, 1, (fas.size() - 1) * 2); //Specify size and precision
    status = DftiSetValue(descriptor, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    status = DftiSetValue(descriptor, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX);
    status = DftiSetValue(descriptor, DFTI_BACKWARD_SCALE, 1.0f / fas.size()); //Scale down the output
    status = DftiCommitDescriptor(descriptor); //Finalize the descriptor
    status = DftiComputeBackward(descriptor, fas.data(), ts.data()); //Compute the Forward FFT
    status = DftiFreeDescriptor(&descriptor); //Free the descriptor
}

// Calculate soil transfer function and surface soil response
void MainWindow::calculate()
{
    QVector<double> absSoilTF(m_freq.size());

    // Compute the Fourier amplitude spectrum
    QVector<std::complex<double>> fas(m_freq.size());
    fft(m_accInput, fas);
    QVector<double> absfas(fas.size());
    for ( int i = 0; i < fas.size(); i++ ) {
        absfas[i] = abs(fas[i]);
    }
    m_absFft = absfas;

    for ( int i = 0; i < m_freq.size(); i++ ) {
        absSoilTF[i] = abs(m_soilTF[i]);
    }
    m_absSoilTF = absSoilTF;

    // Compute surface soil response
    QVector<std::complex<double>> ifas(m_freq.size());
    QVector<double> absfas2(ifas.size());
    for (int i = 0; i < m_freq.size(); i++) {
        ifas[i] = fas[i] * m_soilTF[i];
        absfas2[i] = abs(ifas[i]);
    }
    m_absIFft = absfas2;
    QVector<double> accT(m_accInput.size());
    ifft(ifas, accT);
    m_accOutput = accT;
}

// The following part of code is modified from PGT
void MainWindow::plotLayers()
{
    foreach (PLOTOBJECT item, plotItemList) {
        item.itemPtr->detach();
        delete item.itemPtr;
    }
    plotItemList.clear();
    //
    // Plot Ground Layers
    //
    plot->detachItems();
    double currentBase = 0;
    for (int iLayer = ui->tableView->m_sqlModel->rowCount(); iLayer >= 1; iLayer--) {
        QPolygonF groundCorners;
        groundCorners << QPointF(0, currentBase)
                      << QPointF(1, currentBase)
                      << QPointF(1, currentBase + ui->tableView->m_sqlModel->getThickness(iLayer-1))
                      << QPointF(0, currentBase + ui->tableView->m_sqlModel->getThickness(iLayer-1))
                      << QPointF(0, currentBase);
        currentBase += ui->tableView->m_sqlModel->getThickness(iLayer-1);
        QwtPlotShapeItem *layerII = new QwtPlotShapeItem();
        if (iLayer == m_activeLayer) {
            layerII->setPen(QPen(Qt::red, 2));
            layerII->setBrush(QBrush(BRUSH_COLOR[3+iLayer]));
        }
        else {
            layerII->setPen(QPen(BRUSH_COLOR[iLayer], 1));
            layerII->setBrush(QBrush(BRUSH_COLOR[iLayer]));
        }

        layerII->setPolygon(groundCorners);
        layerII->attach(plot);

        PLOTOBJECT var;
        var.itemPtr = layerII;
        var.index   = iLayer;
        plotItemList.append(var);
    }
    plot->setAxisScale(QwtPlot::yLeft, 0, currentBase);

    plot->replot();
}


void MainWindow::on_picker_appended (const QPoint &pos)
{
    PLOTOBJECT    obj = itemAt(pos);
    if (obj.index >= 1) {
        setActiveLayer(obj.index);
        ui->tableView->m_sqlModel->setActive(obj.index-1);
        ui->tableView->selectRow(obj.index-1);
    }
}


PLOTOBJECT MainWindow::itemAt( const QPoint& pos ) const
{
    PLOTOBJECT emptyObj;
    emptyObj.itemPtr = nullptr;
    emptyObj.index   = -1;

    if ( plot == nullptr )
        return emptyObj;

    // translate pos into the plot coordinates
    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    for ( int i = plotItemList.size() - 1; i >= 0; i-- )
    {
        PLOTOBJECT obj = plotItemList[i];
        QwtPlotItem *item = obj.itemPtr;
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            double dist;

            QwtPlotCurve *curveItem = static_cast<QwtPlotCurve *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( curveItem->boundingRect().contains( p ) )
            {
                // trace curves ...
                dist = 1000.;
                for (size_t line=0; line < curveItem->dataSize() - 1; line++)
                {
                    QPointF pnt;
                    double x, y;

                    pnt = curveItem->sample(line);
                    x = plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x0(x,y);

                    pnt = curveItem->sample(line+1);
                    x = plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x1(x,y);

                    QPointF r  = pos - x0;
                    QPointF s  = x1 - x0;
                    double s2  = QPointF::dotProduct(s,s);

                    if (s2 > 1e-6)
                    {
                        double xi  = QPointF::dotProduct(r,s) / s2;

                        if ( 0.0 <= xi && xi <= 1.0 )
                        {
                            QPointF t(-s.y()/sqrt(s2), s.x()/sqrt(s2));
                            double d1 = QPointF::dotProduct(r,t);
                            if ( d1 < 0.0 )  { d1 = -d1; }
                            if ( d1 < dist ) { dist = d1;}
                        }
                    }
                    else
                    {
                        dist = sqrt(QPointF::dotProduct(r,r));
                        //QPointF r2 = pos - x1;
                        double d2  = sqrt(QPointF::dotProduct(r,r));
                        if ( d2 < dist ) { dist = d2; }
                    }
                }

                qWarning() << "curve dist =" << dist;

                if ( dist <= 5 ) return obj;
            }
        }
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *shapeItem = static_cast<QwtPlotShapeItem *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( shapeItem->boundingRect().contains( p ) && shapeItem->shape().contains( p ) )
            {
                return obj;
            }
        }
    }

    return emptyObj;
}

void MainWindow::on_resetFigureBtn_clicked()
{
    ui->lockAxischeckBox->setChecked(false);
    this->updatePlots();
}

void MainWindow::on_lockAxischeckBox_stateChanged(int arg1)
{
    m_lockAxesFlag = arg1;
    // record current axis limits
    m_xUpLimits[0] = ui->TransferFunctionFig_TabLayer->maxX();
    m_xLowLimits[0] = ui->TransferFunctionFig_TabLayer->minX();
    m_yUpLimits[0] = ui->TransferFunctionFig_TabLayer->maxY();
    m_yLowLimits[0] = ui->TransferFunctionFig_TabLayer->minY();
    m_xUpLimits[1] = ui->TransferFunctionFig->maxX();
    m_xLowLimits[1] = ui->TransferFunctionFig->minX();
    m_yUpLimits[1] = ui->TransferFunctionFig->maxY();
    m_yLowLimits[1] = ui->TransferFunctionFig->minY();
    m_xUpLimits[2] = ui->FourierOutputFig->maxX();
    m_xLowLimits[2] = ui->FourierOutputFig->minX();
    m_yUpLimits[2] = ui->FourierOutputFig->maxY();
    m_yLowLimits[2] = ui->FourierOutputFig->minY();
    m_xUpLimits[3] = ui->AccOutputFig->maxX();
    m_xLowLimits[3] = ui->AccOutputFig->minX();
    m_yUpLimits[3] = ui->AccOutputFig->maxY();
    m_yLowLimits[3] = ui->AccOutputFig->minY();
}

