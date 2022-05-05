#include "../headers/script.h"

Script::Script(QString __script): script{__script}{}

void Script::setScript(QString __script){
    script = __script;
}

cv::Mat QImageToMat(QImage image){
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(
            image.height(), image.width(),
            CV_8UC4, (void*)image.constBits(),
            image.bytesPerLine()
        );
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(
            image.height(), image.width(),
            CV_8UC3, (void*)image.constBits(),
            image.bytesPerLine()
        );
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        break;
    case QImage::Format_Grayscale8:
        mat = cv::Mat(
            image.height(), image.width(),
            CV_8UC1, (void*)image.constBits(),
            image.bytesPerLine()
        );
        break;
    default:
        break;
    }
    return mat;
}

BITMAPINFOHEADER createBitmapHeader(int width, int height){
   BITMAPINFOHEADER  bi;

     bi.biSize = sizeof(BITMAPINFOHEADER);
     bi.biWidth = width;
     bi.biHeight = -height;
     bi.biPlanes = 1;
     bi.biBitCount = 32;
     bi.biCompression = BI_RGB;
     bi.biSizeImage = 0;
     bi.biXPelsPerMeter = 0;
     bi.biYPelsPerMeter = 0;
     bi.biClrUsed = 0;
     bi.biClrImportant = 0;

     return bi;
}

cv::Mat captureScreenMat(HWND hwnd){
    cv::Mat src;

    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    src.create(height, width, CV_8UC4);

    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = createBitmapHeader(width, height);

    SelectObject(hwindowCompatibleDC, hbwindow);

    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

void Script::processFrame(){
    cv::Mat frame, templ, result, img_display;
    QImage f2(":/imgs/accept.png");

    frame = captureScreenMat(HWND{GetDesktopWindow()});
    templ = QImageToMat(f2);

    frame.copyTo(img_display);

    if(frame.empty() || templ.empty()){
        qDebug() << "Can't read one of the images\n";
        return;
    }

    cv::Point minLoc, maxLoc, matchLoc;
    double minVal, maxVal;

    cv::matchTemplate(frame, templ, result, cv::TM_CCOEFF_NORMED);

    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
    matchLoc = maxLoc;  //  take higher val for TM_COEFF_NORMED

    cv::Scalar colorScalar = cv::Scalar(0, 255, 0);
    cv::Point rec{matchLoc.x + templ.cols , matchLoc.y + templ.rows};
    rectangle(img_display, matchLoc, rec, colorScalar, 2, 8, 0 );
    imshow("", img_display);
}
