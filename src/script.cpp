#include "../headers/script.h"

void monitorKeys(Script* s);
Script::Script(Profile& __profile): profile{__profile}{
    enabled = bool{false};

    genThread(type::Accept);
    genThread(type::Report);

    keyThread = QThread::create([this](){monitorKeys();});
    keyThread -> start();
}

void Script::genThread(type __type){
    switch(__type){
        case type::Accept:
            script[type::Accept] = QThread::create([this](){accept();});
        case type::Report:
            script[type::Report] = QThread::create([this](){report();});
        case type::Select:
            script[type::Select] = QThread::create([this](){select();});
        break;
    }
}

void Script::monitorKeys(){
    while(true){
        keys['A'] = GetAsyncKeyState('A');
        keys['R'] = GetAsyncKeyState('R');
        keys['S'] = GetAsyncKeyState('S');
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

void Script::trigger(type __type){
    auto trigger = script[__type];
    if(trigger->isFinished()){
        genThread(__type);
    }

    if(enabledScripts[__type]){
        enabledScripts[__type] = false;
        trigger -> wait();
    }
    else if(!enabledScripts[__type]){
        exec(__type);
    }
}

void Script::click(INPUT* key, cv::Point p){
    SetCursorPos(p.x, p.y);
    key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, key, sizeof(INPUT));
    key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, key, sizeof(INPUT));
}

void Script::typeKeys(QString selectedChamp){
    auto ip = new INPUT{};
    ip->type = INPUT_KEYBOARD;
    ip->ki.dwFlags = KEYEVENTF_UNICODE;
    ip->ki.time = 0;
    ip->ki.wVk = 0;
    ip->ki.dwExtraInfo = 0;

    for(char c : selectedChamp.toStdString()){
        ip->ki.wScan = c;
        SendInput(1, ip, sizeof(INPUT));
    }
    delete ip;
}

void Script::select(){
    cv::Point p, q;
    bool banned = false, first = true;
    auto key = new INPUT{};
    int prioChamp = 0, prioBanChamp = 0;
    QString selectedChamp;

    keys[VK_LCONTROL] = keys[VK_LSHIFT] = keys['S'] = false;
    key -> type = INPUT_MOUSE;

    while(!keys['S'] || !keys[VK_LCONTROL] || !keys[VK_LSHIFT]){
//        p = processFrame(":/imgs/none.png");
//        if(p != cv::Point{-1, -1}){
//            p += cv::Point{25, 25};
//            click(key, p);
//            banned = true;
//        }

        //  banning
        if(!banned){
            p = processFrame(":/imgs/search.png");
            if(p != cv::Point{-1, -1} && first){
                std::this_thread::sleep_for(std::chrono::milliseconds(20000));
                first = false;
            }
            if(p != cv::Point{-1, -1} && !first){
                p += cv::Point{100, 10};
                click(key, p);
                click(key, p);

                if(prioBanChamp < profile.banChamps->size()){
                    typeKeys(profile.banChamps->at(prioBanChamp));
                    prioBanChamp += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    click(key, p - cv::Point{450, -80});
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }

                p = processFrame(":/imgs/ban.png");
                if(p != cv::Point{-1, -1}){
                    p += cv::Point{100, 30};
                    click(key, p);
                    banned = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
                }
            }
        }

        if(banned){
            p = processFrame(":/imgs/search.png");
            if(p != cv::Point{-1, -1}){
                p += cv::Point{100, 10};
                click(key, p);
                click(key, p);

                if(prioChamp < profile.champs->size()){
                    typeKeys(profile.champs->at(prioChamp));
                    prioChamp += 1;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    click(key, p - cv::Point{450, -80});
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            }

            p = processFrame(":/imgs/lock_in.png");
            if(p != cv::Point{-1, -1}){
                p += cv::Point{100, 30};
                click(key, p);
            }
        }

        //  if player dodges
        p = processFrame(":/imgs/accept.png");
        q = processFrame(":/imgs/in_queue.png");
        if(p != cv::Point{-1, -1} || q != cv::Point{-1, -1}){
            banned = false;
            first = true;
            prioChamp = prioBanChamp = 0;
        }
        if(!enabledScripts[type::Select])
            break;
    }
    enabledScripts[type::Select] = false;
    delete key;
}

void Script::accept(){
    cv::Point p;
    auto key = new INPUT{};

    keys[VK_LCONTROL] = keys[VK_LSHIFT] = keys['A'] = false;
    key -> type = INPUT_MOUSE;

    while(!keys['A'] || !keys[VK_LCONTROL] || !keys[VK_LSHIFT]){
        p = processFrame(":/imgs/accept.png");
        if(p != cv::Point{-1, -1}){
            p += cv::Point{100, 30};
            click(key, p);
        }
        if(!enabledScripts[type::Accept])
            break;
    }
    enabledScripts[type::Accept] = false;
    delete key;
}

void Script::reportPlayer(cv::Point p){
    cv::Point rel{438, 232};
    auto key = new INPUT{};
    key -> type = INPUT_MOUSE;

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel += p;
    click(key, rel);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 277;
    click(key, rel);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 353;
    click(key, rel);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    rel.y = p.y + 600;
    rel.x += 200;
    click(key, rel);
    delete key;
}

void Script::report(){
    cv::Point p, rel;
    auto key = new INPUT{};

    keys[VK_LCONTROL] = keys[VK_LSHIFT] = keys['R'] = false;
    key -> type = INPUT_MOUSE;

    bool pgl = false;
    while(!keys['R'] || !keys[VK_LCONTROL] || !keys[VK_LSHIFT]){
        rel = p = processFrame(":/imgs/pgl.png");
        if(p != cv::Point{-1,-1} && !pgl){
            // get player names
            rel += cv::Point{105, 120};
            for(int i = 0; i < 10; i++){
                //  process player name
                QString name = getTextFromFrame(rel);
                if(i == 4){
                    rel += cv::Point{0, 8};
                    continue;
                }
                rel += cv::Point{0, 35};
            }

            //  set rel loc for report buttons
            rel = p + cv::Point{165, 156}; // first report loc
            for(int i = 0; i < 10; i++){
                SetCursorPos(
                    rel.x,
                    rel.y + i*35 + (i > 3 ? 8 : 0)
                );
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, key, sizeof(INPUT));
                key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, key, sizeof(INPUT));
                reportPlayer(p);
            }
            pgl = true;
        }
        p = processFrame(":/imgs/find_match.png");
        pgl &= p == cv::Point{-1,-1};
        p = processFrame(":/imgs/in_queue.png");
        pgl &= p == cv::Point{-1,-1};

        if(!enabledScripts[type::Report])
            break;
    }
    enabledScripts[type::Report] = false;
    delete key;
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
    double minVal, maxVal, threshold;

    if(object == ":/imgs/pgl.png")
        threshold = 0.45;
    else if(object == ":/imgs/find_match.png")
        threshold = 0.90;
    else
        threshold = 0.99;

    cv::matchTemplate(currentFrame, templ, result, cv::TM_CCOEFF_NORMED);

    cv::threshold(result, result, threshold, 1., cv::THRESH_TOZERO);
    cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
    matchLoc = maxLoc;

    return maxVal >= threshold ? matchLoc : cv::Point{-1, -1};
}

QString Script::getTextFromFrame(cv::Point p){
    cv::Mat currentFrame = captureScreenMat(HWND{GetDesktopWindow()});

    cv::Mat croppedImage = currentFrame(cv::Rect{p.x, p.y, 135, 20});// +105x, +120y

}
