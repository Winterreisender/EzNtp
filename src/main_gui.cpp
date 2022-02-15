// coding: UTF-8
// License: AGPL-3.0

#include <iostream>
#include <windows.h>
#include <string>
#include <time.h>
#include "EzNtp.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Display.H>

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

    fl_beep(FL_BEEP_MESSAGE);
    fl_message("已设置服务器为%s:%d",ntpServerIP.c_str(),ntpServerPort);
}

void btnRefreshTimeClicked(Fl_Widget* _, void* __)
{
    auto ezNtp = EzNtp();
    ezNtp.ntpServerIP = ntpServerIP;
    ezNtp.ntpServerPort = ntpServerPort;
    ezNtp.initSocket();

    try {
        timeb ntpTime = ezNtp.getNtpTime();
        serverTimeOutput->value(Utils::timebToString(ntpTime).c_str());
        ezNtp.closeSocket();
    } catch(const AppException& err) {
        fl_beep(FL_BEEP_ERROR);
        fl_alert(err.message.c_str());
        return;
    }

    timeb sysTimeb;
    ftime(&sysTimeb);
    sysTimeOutput->value(Utils::timebToString(sysTimeb).c_str());

    fl_beep(FL_BEEP_MESSAGE);
}

void btnSyncTimeClicked(Fl_Widget* _, void* __)
{
    DWORD result = -1;

    try {
        auto ezNtp = EzNtp();
        ezNtp.ntpServerIP = ntpServerIP;
        ezNtp.ntpServerPort = ntpServerPort;
        ezNtp.initSocket();

        result = ezNtp.syncTime();
        ezNtp.closeSocket();
    } catch(const AppException& err) {
        fl_beep(FL_BEEP_ERROR);
        fl_alert(err.message.c_str());
        return;
    }
    if(result != 0) {
        fl_beep(FL_BEEP_ERROR);
        fl_alert("同步失败! 错误码为%ld\n%s",result, result==1314?"请以管理员权限运行":"");
    }else{
        fl_beep(FL_BEEP_MESSAGE);
        fl_message("同步成功");
    }

}

void fltk_main(int argc, char** argv)
{
    constexpr int WINDOW_WIDTH = 280, WINDOW_HEIGHT = 140;
    
    window_main = new Fl_Double_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "EzNtp校时");
    window_main->align(Fl_Align(FL_ALIGN_CLIP | FL_ALIGN_INSIDE));
    window_main->hotspot(WINDOW_WIDTH/2,WINDOW_HEIGHT/2); //始终显示在鼠标处

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
"EzNtp GUI v1.0.0 \"1096\"\n"
"主页: https://gitee.com/winter_reisender/ez-ntp\n\n"
"常用NTP服务器IP地址\n"
"国家授时中心: 114.118.7.163或114.118.7.161\n"
"Apple: 17.253.114.125\n"
"阿里云: 203.107.6.88\n\n"
"版权与许可声明:\n"
"Copyright 2022 Winterreisender.\n"
"This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, only version 3 of the License.\n"
"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.\n"
"You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.\n"
"本程序使用了以下开源软件\n"
"FLTK: FLTK is provided under the terms of the GNU Library Public License, Version 2 with exceptions that allow for static linking. See https://www.fltk.org/COPYING.php\n"
"鸣谢:\n"
"mingw-w64: https://www.mingw-w64.org/"
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
//    TCHAR exeName[256];
//    
//    GetModuleFileName(NULL, exeName,256);
//    dbg(exeName);
//    HICON hIcon = ExtractIconW(GetModuleHandle(NULL),(LPCWSTR) exeName,0);

    Fl::scheme("gtk+");
    fltk_main(argc, argv);
    return Fl::run();
}
