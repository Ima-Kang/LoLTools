#ifndef SCRIPT_H
#define SCRIPT_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <Windows.h>
#include <QString>
#include <QImage>
#include <QTransform>
#include <QDebug>

class Script
{
private:
    QString script;
public:
    Script(QString script);
    void setScript(QString script);
    void processFrame();
};

#endif // SCRIPT_H