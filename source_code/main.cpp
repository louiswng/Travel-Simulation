#include "widget.h"
#include "log.h"
#include "schedule.h"

#include <QApplication>

/* 消息传递函数重定向log.txt */
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QDateTime time = QDateTime::currentDateTime();
    QString timestr = time.toString("yyyy-MM-dd hh:mm:ss ddd");

    QFile file("log.txt");
    file.open(QFile::WriteOnly| QIODevice::Append);
    QTextStream out(&file);

    switch (type) {
    case QtDebugMsg:
        out << "Debug: "  << msg << " (" << timestr << ") " << "\r\n";
        break;
    case QtWarningMsg:
        out << "warning: "<< msg << " (" << timestr << ") " << "\r\n";
        break;
    case QtCriticalMsg:
        out << "critical: "<< msg << " (" << timestr << ") " << "\r\n";
        break;
    case QtFatalMsg:
        out << "fatal: "<< msg << " (" << timestr << ") " << "\r\n";
        abort();
    }
}

/* 日志重定向 */
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    qInstallMessageHandler(myMessageOutput);

    Widget w;
    w.show();
    return a.exec();
}
