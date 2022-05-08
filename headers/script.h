#ifndef SCRIPT_H
#define SCRIPT_H

#include "profile.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "tesseract/baseapi.h"
#include "leptonica/allheaders.h"
#include "opencv2/photo.hpp"
#include "settings.h"

#include <Windows.h>
#include <QString>
#include <QImage>
#include <QTransform>
#include <QDebug>
#include <QThread>
#include <chrono>
#include <QHash>
#include <functional>
#include <QMutex>

class Script{
    private:
        QHash<int, bool> keys;
        QThread* keyThread;
        const static unsigned int maxDepth = 3;
        void monitorKeys();
    public:
        enum type{
            Accept,
            Report,
            Select
        };
        Settings* settings;
        bool enabled;
        QHash<int, QThread*> script;
        QHash<int, bool> enabledScripts;

        Script(Settings* __settings);
        cv::Point processFrame(QString object);
        cv::Mat QImageToMat(QImage image);
        cv::Mat captureScreenMat(HWND hwnd);
        BITMAPINFOHEADER createBitmapHeader(int width, int height);


        void click(INPUT* key, cv::Point p);
        void exec(type script);
        void genThread(type __type);

        void accept();
        void report();
        void reportPlayer(cv::Point p);
        void select();
        void typeKeys(QString selectedChamp);
        void trigger(type __type);
        QString getTextFromFrame(cv::Point p, cv::Mat, int depth=0);
};

#endif // SCRIPT_H
