// coding: UTF-8
// License: AGPL-3.0

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>

#include "EzNtp.h"
#include <time.h>

using namespace std;

auto window_main = (Fl_Double_Window*)0;
auto btnRefreshTime = (Fl_Button*)0;
auto serverTimeOutput = (Fl_Input*)0;
auto sysTimeOutput = (Fl_Input*)0;
auto btnSyncTime = (Fl_Button*)0;
auto btnShowAbout = (Fl_Button*)0;
auto inputServerIP = (Fl_Input*)0;
auto inputServerPort = (Fl_Input*)0;

string ntpServerIP = "114.118.7.161";
int ntpServerPort = 123;

void show_about_windows();

void btnShowAboutClicked(Fl_Widget* _, void* __) {
    show_about_windows();
}

void btnSetServerInfoClicked(Fl_Widget* _, void* __) {
    dbg(inputServerIP->value())
    dbg(inputServerPort->value());
    
    ntpServerIP = inputServerIP->value();
    ntpServerPort = atoi(inputServerPort->value());
}

void btnRefreshTimeClicked(Fl_Widget* _, void* __)
{
    auto ezNtp = EzNtp();
    ezNtp.ntpServerIP = ntpServerIP;
    ezNtp.ntpServerPort = ntpServerPort;
    ezNtp.initSocket();

    timeb ntpTime = ezNtp.getNtpTime();
    serverTimeOutput->value(Utils::timebToString(ntpTime).c_str());
    ezNtp.closeSocket();

    timeb sysTimeb;
    ftime(&sysTimeb);
    sysTimeOutput->value(Utils::timebToString(sysTimeb).c_str());
}

void btnSyncTimeClicked(Fl_Widget* _, void* __)
{
    btnSyncTime->label("同步中...");
    btnSyncTime->deactivate();

    auto ezNtp = EzNtp();
    ezNtp.ntpServerIP = ntpServerIP;
    ezNtp.ntpServerPort = ntpServerPort;
    ezNtp.initSocket();

    auto result = ezNtp.syncTime();
    ezNtp.closeSocket();

    btnSyncTime->label(((result == 0) ? "同步成功" : "请重试"));
    btnSyncTime->set_active();
}

void fltk_main(int argc, char** argv)
{

    window_main = new Fl_Double_Window(280, 140, "EzTimeSync");
    window_main->align(Fl_Align(FL_ALIGN_CLIP | FL_ALIGN_INSIDE));

    serverTimeOutput = new Fl_Input(85, 22, 185, 25, "服务器时间");
    serverTimeOutput->value("点击\"刷新\"以获取时间");

    sysTimeOutput = new Fl_Input(85, 57, 185, 25, "系统时间");
    sysTimeOutput->value("点击\"刷新\"以获取时间");

    btnRefreshTime = new Fl_Button(10, 95, 90, 25, "刷新");
    btnRefreshTime->callback((Fl_Callback*)btnRefreshTimeClicked, NULL);

    btnSyncTime = new Fl_Button(110, 95, 90, 25, "同步");
    btnSyncTime->callback((Fl_Callback*)btnSyncTimeClicked, NULL);

    btnShowAbout = new Fl_Button(210, 95, 58, 25, "设置");
    btnShowAbout->callback((Fl_Callback*)btnShowAboutClicked, NULL);


    window_main->end();
    window_main->show(argc, argv);
}

void show_about_windows() {
    constexpr int WINDOW_SIZE_X = 400;
    constexpr int WINDOW_SIZE_Y = 300;

    window_main = new Fl_Double_Window(WINDOW_SIZE_X, WINDOW_SIZE_Y, "About");
    window_main->align(Fl_Align(FL_ALIGN_CLIP | FL_ALIGN_INSIDE));

    auto textBuff = new Fl_Text_Buffer();
    auto aboutTextDisplay = new Fl_Text_Display(0,0,WINDOW_SIZE_X,190);
    aboutTextDisplay->buffer(textBuff);
    textBuff->text(
"EzNtp GUI v0.1.2\n"
"简单的手动校时软件\n"
"Copyright 2022 Winterreisender.\n"
"Licensed under AGLP-3.0\n"
"Homepage: https://gitee.com/winter_reisender/ez-ntp\n"

"若同步时出现错误,请尝试以管理员身份运行并检查网络连接。\n"
"常用NTP服务器IP地址\n"
"国家授时中心(默认): 114.118.7.161或114.118.7.163\n"
"Apple: 17.253.114.125\n"
"阿里云: 203.107.6.88"
    ); 

    inputServerIP = new Fl_Input(100,195,200,25, "IP地址");
    inputServerIP->value(ntpServerIP.c_str());
    inputServerPort = new Fl_Input(100,230,200,25, "端口号");
    inputServerPort->type(FL_INT_INPUT);
    inputServerPort->value(to_string(ntpServerPort).c_str());

    auto btnSetServerInfo = new Fl_Button(120,265,150,25, "应用");
    btnSetServerInfo->callback((Fl_Callback*)btnSetServerInfoClicked, NULL);



    window_main->end();
    window_main->show(0, NULL);

}

int main(int argc, char** argv)
{
    Fl::scheme("gtk+");
    fltk_main(argc, argv);
    return Fl::run();
}
