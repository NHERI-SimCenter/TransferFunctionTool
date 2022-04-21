#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <GoogleAnalytics.h>

static QString logFilePath;


QString openStyleFiles()
{
    QString ret;
    QFile mainStyleFile(":/resources/styleSheets/stylesheet.qss");

#ifdef Q_OS_WIN
    QFile appendedStyle(":/resources/styleSheets/Win.qss");
#endif

#ifdef Q_OS_MACOS
    QFile appendedStyle(":/resources/styleSheets/Mac.qss");
#endif

#ifdef Q_OS_LINUX
    QFile appendedStyle(":/resources/styleSheets/Linux.qss");
#endif

    if (!mainStyleFile.open(QFile::ReadOnly))
    {
        return ret;
    }

    if (!appendedStyle.open(QFile::ReadOnly))
    {
        return ret;
    }

    ret = ret.append(mainStyleFile.readAll());
    ret = ret.append(appendedStyle.readAll());

    mainStyleFile.close();
    appendedStyle.close();

    return ret;
}


int main(int argc, char *argv[])
{
    GoogleAnalytics::SetTrackingId("UA-178896295-1");
    GoogleAnalytics::StartSession();
    GoogleAnalytics::ReportStart();

    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("NHERI-TFT");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("1.0.1");
    // window scaling
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    MainWindow w;
    w.setWindowTitle("Transfer Function Tool");
    QApplication::setWindowIcon(QIcon(":/resources/NHERI-TFT-Icon.icns"));
    w.show();

    // set the global stylesheet
    a.setStyleSheet(openStyleFiles());

    GoogleAnalytics::EndSession();

    return a.exec();
}
