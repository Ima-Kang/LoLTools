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
#include <QThread>
#include <chrono>
#include <QHash>
#include <functional>

class Script{
    private:
        QHash<int, bool> keys;
        QThread* keyThread;
        QThread* hotkeyThread;
        QThread* screenCapture;
        cv::Mat* frame;

        void monitorKeys();
    public:
        enum type{
            Accept,
            Report
        };
        bool enabled;
        QHash<int, QThread*> script;
        QHash<int, bool> enabledScripts;

        Script();
        cv::Point processFrame(QString object);
        cv::Mat QImageToMat(QImage image);
        cv::Mat captureScreenMat(HWND hwnd);
        BITMAPINFOHEADER createBitmapHeader(int width, int height);

        void exec(type script);
        void genThread(type __type);

        void accept();
        void acceptTrigger();
        void report();
        void reportTrigger();
        void reportPlayer(cv::Point p);

        void monitorHotkeys();
        void captureScreen(bool* e);
};

#endif // SCRIPT_H