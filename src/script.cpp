#include "../headers/script.h"

void monitorKeys(Script* s);
Script::Script(){
    enabled = bool{false};

    genThread(type::Accept);
    genThread(type::Report);

    keyThread = QThread::create([this](){monitorKeys();});
    keyThread -> start();
}

void Script::acceptTrigger(){
    auto accept = script[Script::type::Accept];
    if(accept->isFinished()){
        genThread(Script::type::Accept);
    }

    if(enabledScripts[Script::type::Accept]){
        enabledScripts[Script::type::Accept] = false;
        accept -> wait();
    }
    else if(!enabledScripts[Script::type::Accept]){
        exec(Script::type::Accept);
    }
}

void Script::reportTrigger(){
    auto report = script[Script::type::Report];
    if(report->isFinished()){
        genThread(Script::type::Report);
    }

    if(enabledScripts[Script::type::Report]){
        enabledScripts[Script::type::Report] = false;
        report -> wait();
        qDebug() << "STOP";
    }
    else if(!enabledScripts[Script::type::Report]){
        qDebug() << "START";
        exec(Script::type::Report);
    }
}

void Script::genThread(type __type){
    switch(__type){
        case type::Accept:
            script[type::Accept] = QThread::create([this](){accept();});
        case type::Report:
            script[type::Report] = QThread::create([this](){report();});
        break;
    }
}

void Script::monitorKeys(){
    while(true){
        keys['A'] = GetAsyncKeyState('A');
        keys['R'] = GetAsyncKeyState('R');
        keys[VK_LCONTROL] = GetAsyncKeyState(VK_LCONTROL);
        keys[VK_LSHIFT] = GetAsyncKeyState(VK_LSHIFT);
    }
}

void Script::exec(type __script){
    //  threading here
    enabled = true;
    enabledScripts[__script] = true;
    script[__script] -> start();
}
void Script::accept(){
    cv::Point p;
    keys[VK_LCONTROL] = keys[VK_LSHIFT] = keys['A'] = false;

    while(!keys['A'] || !keys[VK_LCONTROL] || !keys[VK_LSHIFT]){
        p = processFrame(":/imgs/accept.png");
        if(p != cv::Point{-1, -1}){
            p += cv::Point{100, 30};
            SetCursorPos(p.x, p.y);

            auto key = new INPUT{};
            key -> type = INPUT_MOUSE;
            key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, key, sizeof(INPUT));
            key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, key, sizeof(INPUT));
            delete key;
        }
        if(!enabledScripts[type::Accept])
            break;
    }
    enabledScripts[type::Accept] = false;
}

void Script::reportPlayer(cv::Point p){
    cv::Point rel{438, 232};
    auto key = new INPUT{};
    key -> type = INPUT_MOUSE;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel += p;
    SetCursorPos(rel.x, rel.y);
    key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, key, sizeof(INPUT));
    key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, key, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 277;
    SetCursorPos(rel.x, rel.y);
    key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, key, sizeof(INPUT));
    key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, key, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 353;
    SetCursorPos(rel.x, rel.y);
    key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, key, sizeof(INPUT));
    key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, key, sizeof(INPUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 600;
    rel.x += 200;
    SetCursorPos(rel.x, rel.y);
    key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, key, sizeof(INPUT));
    key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, key, sizeof(INPUT));
    delete key;
}

void Script::report(){
    cv::Point p, rel;
    keys[VK_LCONTROL] = keys[VK_LSHIFT] = keys['R'] = false;

    while(!keys['R'] || !keys[VK_LCONTROL] || !keys[VK_LSHIFT]){
        rel = p = processFrame(":/imgs/pgl.png");
        if(p != cv::Point{-1,-1}){
            rel += cv::Point{165, 156}; // +35
            for(int i = 0; i < 10; i++){
                SetCursorPos(
                    rel.x,
                    rel.y + i*35 + (i > 3 ? 8 : 0)
                );
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                auto key = new INPUT{};
                key -> type = INPUT_MOUSE;
                key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, key, sizeof(INPUT));
                key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, key, sizeof(INPUT));
                reportPlayer(p);
                delete key;
            }
            break;
        }
        if(!enabledScripts[type::Report])
            break;
    }
    enabledScripts[type::Report] = false;
}

cv::Mat Script::QImageToMat(QImage image){
    cv::Mat mat;

    switch(image.format()){
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

BITMAPINFOHEADER Script::createBitmapHeader(int width, int height){
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

cv::Mat Script::captureScreenMat(HWND hwnd){
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

    StretchBlt(hwindowCompatibleDC,
        0, 0, width, height,
        hwindowDC, screenx, screeny, width, height, SRCCOPY
    );
    GetDIBits(hwindowCompatibleDC,
        hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS
    );

    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

cv::Point Script::processFrame(QString object){
    cv::Mat currentFrame, templ, result, img_display;  //  gonna need to move frame
    QImage f2(object);

    currentFrame = captureScreenMat(HWND{GetDesktopWindow()});
    templ = QImageToMat(f2);

    currentFrame.copyTo(img_display);

    if(currentFrame.empty() || templ.empty()){
        qDebug() << "Can't read one of the images\n";
        return cv::Point{-1, -1};
    }

    cv::Point minLoc, maxLoc, matchLoc;
    double minVal, maxVal, threshold = (object == ":/imgs/pgl.png") ? 0.3 : 0.99;

    cv::matchTemplate(currentFrame, templ, result, cv::TM_CCOEFF_NORMED);

    cv::threshold(result, result, threshold, 1., cv::THRESH_TOZERO);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
    matchLoc = maxLoc;

    return maxVal >= threshold ? matchLoc : cv::Point{-1, -1};
}
