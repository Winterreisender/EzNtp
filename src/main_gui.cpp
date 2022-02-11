// coding: UTF-8
// License: AGPL-3.0

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

#include "EzNtp.h"
#include <time.h>

using namespace std;

Fl_Double_Window* window_main = (Fl_Double_Window*)0;
Fl_Button* btnRefreshTime = (Fl_Button*)0;
Fl_Input* serverTimeOutput = (Fl_Input*)0;
Fl_Input* sysTimeOutput = (Fl_Input*)0;
Fl_Button* btnSyncTime = (Fl_Button*)0;

void btnRefreshTimeClicked(Fl_Widget* _, void* __)
{
    auto ezNtp = EzNtp();
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
    auto ezNtp = EzNtp();
    ezNtp.initSocket();
    auto result = ezNtp.syncTime();
    btnSyncTime->label(((result == 0) ? "Success" : "Error"));
    ezNtp.closeSocket();
}

int fltk_main(int argc, char** argv)
{
    Fl::scheme("gtk+");

    window_main = new Fl_Double_Window(282, 140, "EzTimeSync");
    window_main->align(Fl_Align(FL_ALIGN_CLIP | FL_ALIGN_INSIDE));

    serverTimeOutput = new Fl_Input(85, 22, 170, 23, "服务器时间");

    sysTimeOutput = new Fl_Input(85, 57, 170, 23, "系统时间");

    btnSyncTime = new Fl_Button(155, 95, 100, 25, "同步");
    btnSyncTime->callback((Fl_Callback*)btnSyncTimeClicked, NULL);

    btnRefreshTime = new Fl_Button(35, 95, 100, 25, "刷新");
    btnRefreshTime->callback((Fl_Callback*)btnRefreshTimeClicked, NULL);

    window_main->end();
    window_main->show(argc, argv);
    return Fl::run();
}

int main(int argc, char** argv)
{
    return fltk_main(argc, argv);
}
