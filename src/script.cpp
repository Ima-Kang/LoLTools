#include "../headers/script.h"

int editDist(QString str1, QString str2){
    int n = str1.size(), m = str2.size();
    int **dp = new int *[n + 1];

    for (int i = 0; i <= n; i++)
        dp[i] = new int[m + 1];

    for (int i = 0; i <= n; i++){
        for (int j = 0; j <= m; j++){
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (str1[i - 1] == str2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else{
                dp[i][j] = 1 + min(
                    min(dp[i][j - 1],dp[i - 1][j]),
                    dp[i - 1][j - 1]
                );
            }
        }
    }
    int ans = dp[n][m];
    for (int i = 0; i <= n; i++)
        delete[] dp[i];
    delete[] dp;
    return ans;
}
Script::Script(Settings* __settings, QList<QAction*> __actions):
    settings{__settings}, actions{__actions}{
    enabled = bool{false};

    genThread(type::Accept);
    genThread(type::Report);
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
    Profile profile = *settings->currentProfile;
    bool banned = false, first = true;
    auto key = new INPUT{};
    int prioChamp = 0, prioBanChamp = 0;

    key -> type = INPUT_MOUSE;

    while(true){
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
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

    for(auto& a: actions){
        if(a->objectName() == "actionEnableSelect")
            a->setText("Enable (Ctrl + Shift + S)");
    }

    delete key;
}

void Script::accept(){
    cv::Point p;
    auto key = new INPUT{};

    key -> type = INPUT_MOUSE;

    while(true){
        p = processFrame(":/imgs/accept.png");
        if(p != cv::Point{-1, -1}){
            p += cv::Point{100, 30};
            click(key, p);
        }
        if(!enabledScripts[type::Accept])
            break;
    }

    for(auto& a: actions){
        if(a->objectName() == "actionEnableAccept")
            a->setText("Enable (Ctrl + Shift + A)");
    }

    enabledScripts[type::Accept] = false;
    delete key;
}

void Script::reportPlayer(cv::Point p){
    cv::Point rel{448, 232};
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
    auto whitelist = *settings->whitelist;

    key -> type = INPUT_MOUSE;

    while(true){
        rel = p = processFrame(":/imgs/pgl.png");
        if(p != cv::Point{-1,-1}){
            // get player names
            QMutex mu;
            cv::Mat currentFrame = captureScreenMat(HWND{GetDesktopWindow()});
            QHash<QString, int> names;
            QVector<QThread* > tThreads;

            rel += cv::Point{165, 136};
            //  process every player name
            for(int i = 0; i < 11; i++){
                if(i == 5){
                    rel += cv::Point{0, 36};
                    continue;
                }
                getTextFromFrame(rel, currentFrame);
                tThreads.push_back(QThread::create([this, &names, &mu]
                    (cv::Point q, cv::Mat frame, int i){
                    QString name{getTextFromFrame(q, frame)}; name.chop(1);
                    mu.tryLock(); names[name] = i; mu.unlock();
                }, rel, currentFrame, i));
                tThreads.back() -> start();

                rel += cv::Point{0, 41};
            }

            for(auto& t : tThreads){
                while(!t->isFinished());
            }
            auto guessNames = names.keys();

            for(auto& name : whitelist){
                if(names.contains(name))
                    names.remove(name);
                else{
                    for(auto gname : guessNames){
                        if(gname.isEmpty())
                            continue;
                        auto entropy{
                            editDist(name, gname)
                        };
                        if(entropy < 5)
                            names.remove(gname);
                    }
                }
            }

            //  set rel loc for report buttons
            rel = p + cv::Point{245, 156}; // first report loc
            for(auto i : names.values()){
                SetCursorPos(
                    rel.x,
                    rel.y + i*41 - (i > 4 ? 3 : 0)
                );
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                key -> mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, key, sizeof(INPUT));
                key -> mi.dwFlags = MOUSEEVENTF_LEFTUP;
                SendInput(1, key, sizeof(INPUT));
                reportPlayer(p);
            }
            break;
        }
        if(!enabledScripts[type::Report])
            break;
    }
    enabledScripts[type::Report] = false;

    for(auto& a: actions){
        if(a->objectName() == "actionEnableReport")
            a->setText("Enable (Ctrl + Shift + R)");
    }
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
    cv::Mat currentFrame, templ, result, img_display;
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

QString Script::getTextFromFrame(cv::Point p, cv::Mat currentFrame, int depth){
    int multiplier = 2 + depth;
    if(currentFrame.empty())
        return "";
    cv::Size size{135, 16};

    cv::Mat croppedImage = currentFrame(
        cv::Rect{p.x, p.y, size.width, size.height}
    );// +105x, +120y
    cv::resize(croppedImage, croppedImage,
        cv::Size(size.width*multiplier, size.height*multiplier), cv::INTER_LINEAR
    );
    //croppedImage.convertTo(croppedImage, -1, 2, 0);

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    //  Include training data
    api->Init("./", "eng_fast", tesseract::OEM_LSTM_ONLY);
    api->SetPageSegMode(tesseract::PSM_AUTO);
    api->SetImage(croppedImage.data, croppedImage.cols,
        croppedImage.rows, 4, croppedImage.step
    );
    api->SetSourceResolution(300);

    if (api->MeanTextConf() < 50 && depth < maxDepth){
        return getTextFromFrame(p, currentFrame, depth + 1);
    }
    auto text = QString{api->GetUTF8Text()};

    api->End();
    delete api;

//    cv::namedWindow("test");
//    cv::moveWindow("test", 100, 100);
//    cv::imshow("test", croppedImage);
//    cv::waitKey();
    return text;
}
