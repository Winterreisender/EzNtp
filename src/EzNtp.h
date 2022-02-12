// 许可: AGPL-3.0
#pragma once

#include <iostream>
#include <exception>
#include <windows.h>

#include <assert.h>
#include <string>
#include <sys/timeb.h>
#include <winsock.h>
using namespace std;

// #pragma comment(lib, "ws2_32.lib") //加载 ws2_32.dll 用于msvc

constexpr auto NTP_TIMESTAMP_DELTA = 2208988800ull;

class AppException {
public:
  string message;
  AppException(string_view msg = "AppException") : message{msg} {}
  string_view what() const { return message; }
};

inline void assert_throw(int expression, string error_msg) {
  if(!expression) {
    throw AppException(error_msg);
  }
}
/*
NTP时间戳的起点为: 1900/1/1
C语言标准/UNIX时间戳的起点为: 1970/1/1 统一使用
Windows API时间戳起点为: 1601/1/1
*/


#define dbg(x) cout << #x << " = " << x << endl;
#define show_line() cout << __FILE__ << ":" << __LINE__ << endl;





/*
               +-----------+------------+-----------------------+
               | Name      | Formula    | Description           |
               +-----------+------------+-----------------------+
               | leap      | leap       | leap indicator (LI)   |
               | version   | version    | version number (VN)   |
               | mode      | mode       | mode                  |
               | stratum   | stratum    | stratum               |
               | poll      | poll       | poll exponent         |
               | precision | rho        | precision exponent    |
               | rootdelay | delta_r    | root delay            |
               | rootdisp  | epsilon_r  | root dispersion       |
               | refid     | refid      | reference ID          |
               | reftime   | reftime    | reference timestamp   |
               | org       | T1         | origin timestamp      |
               | rec       | T2         | receive timestamp     |
               | xmt       | T3         | transmit timestamp    |
               | dst       | T4         | destination timestamp |
               | keyid     | keyid      | key ID                |
               | dgst      | dgst       | message digest        |
               +-----------+------------+-----------------------+
*/
typedef struct {

  uint8_t leap_version_mode;

  uint8_t stratum;
  uint8_t poll;
  uint8_t precision;

  uint32_t rootDelay;
  uint32_t rootDispersion;
  uint32_t refId;

  uint32_t refTimeSec;
  uint32_t refTimeFrac;

  uint32_t orgSec;
  uint32_t orgFrac;

  uint32_t recSec;
  uint32_t recFrac;

  uint32_t xmtSec;

  uint32_t xmtFrac;

} NtpPacket;

inline int64_t calculateTimeOffset(int64_t c1, int64_t s2, int64_t s3,
                                   int64_t c4) {
  return (s2 + s3 - c1 - c4) / 2;
}

namespace Utils {

inline uint64_t timbToMilsec(timeb time) {
  return time.time * 1000ull + time.millitm;
}

inline timeb ntp2timeb(uint32_t sec, uint32_t frac) {
  return timeb{(time_t)(sec - NTP_TIMESTAMP_DELTA),
               (unsigned short)(frac * 1000 / MAXUINT32)};
}

inline void printTimeB(const timeb &time) {
  if (ctime(&time.time) == NULL) {
      throw AppException("Wrong timeb format");
  }
  string timeSecString = ctime(&time.time);
  cout << timeSecString.substr(0, timeSecString.length() - 1)
       << ", Milsec:" << time.millitm << endl;
}

string timebToString(const timeb time) {
  tm *plocal = localtime(&time.time);
  assert_throw(plocal != NULL, "Unable to convert timeb to localtime");

  char str[256] = "";
  sprintf(str, "%4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d:%3.3d", plocal->tm_year + 1900,
          plocal->tm_mon + 1, plocal->tm_mday, plocal->tm_hour, plocal->tm_min,
          plocal->tm_sec,time.millitm);
  string result = str;
  return result;
}

}; // namespace Utils

class Config {
public:
  string ntpServerAddress;
};

class EzNtp {

private:
  SOCKET sock;
  sockaddr_in socketAddress;

public:
  string ntpServerIP = "114.118.7.163"; //国家授时中心
  int ntpServerPort = 123;

    // 设置NTP域名
    /*
    auto& setNtpServer(string domain) {
        dbg(domain);
        hostent* phost = gethostbyname(domain.c_str());  //gethostbyname function has been deprecated
        assert(phost != NULL);
        ntpServerIP = phost->h_name;
        return *this;
    }
    */

    void initSocket() {
        dbg(ntpServerIP);

        WSADATA wsaData;
        assert_throw(
          WSAStartup(MAKEWORD(2, 2), &wsaData) == 0,
          "Error in WSAStartup"
          );
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        assert_throw(sock, "Can not create socket");

        memset(&socketAddress, 0, sizeof(socketAddress));
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_addr.s_addr = inet_addr(ntpServerIP.c_str());
        socketAddress.sin_port = htons(ntpServerPort);

        int sockTimeout = 5000; // 5 sec
        setsockopt( sock, SOL_SOCKET, SO_SNDTIMEO, ( char * )&sockTimeout, sizeof( int ) );
        setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, ( char * )&sockTimeout, sizeof( int ) );
    }

  // milsecs
  int64_t getNtpOffset() {
    NtpPacket packet = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    memset(&packet, 0, sizeof(NtpPacket));
    packet.leap_version_mode = 0x1b;

    assert_throw(
      connect(sock, (SOCKADDR *)&socketAddress, sizeof(SOCKADDR)) != SOCKET_ERROR,
      "Network Error"
      );

    // --------时间敏感区开始-------

    timeb org;
    ftime(&org);

    assert_throw(
      send(sock, (const char *)&packet, sizeof(packet), 0) != SOCKET_ERROR,
      "Network Error"
      );

    assert_throw(
      recv(sock, (char *)&packet, sizeof(packet), 0) != SOCKET_ERROR,
      "Network Error"
    );

    timeb dst;
    ftime(&dst);

    //---------时间敏感区结束------

    //转换大小端
    auto xmtSec = ntohl(packet.xmtSec);
    auto xmtFrac = ntohl(packet.xmtFrac);
    auto recSec = ntohl(packet.recSec);
    auto recFrac = ntohl(packet.recFrac);

    //提取时间
    timeb xmt = Utils::ntp2timeb(xmtSec, xmtFrac);
    timeb rec = Utils::ntp2timeb(recSec, recFrac);

    return calculateTimeOffset(
        Utils::timbToMilsec(org), Utils::timbToMilsec(rec),
        Utils::timbToMilsec(xmt), Utils::timbToMilsec(dst));
  }

  timeb getNtpTime() {
    auto offset = getNtpOffset();
    dbg(offset);

    time_t offsetSec = offset / 1000;
    short int offsetMilsec = offset % 1000;
    timeb localTime;

    //---时间敏感区开始---
    ftime(&localTime);
    localTime.time += offsetSec;
    localTime.millitm += offsetMilsec;
    localTime.time += localTime.millitm / 1000;
    localTime.millitm %= 1000;

    return localTime;
  }

    auto syncTime() { return applyOffset(getNtpOffset()); }

    // retunt 0 if success or Win32 Error Code
    static DWORD applyOffset(const int64_t offset) {
        time_t offsetSec = offset / 1000;
        short int offsetMilsec = offset % 1000;
        timeb localTime;

        //---时间敏感区开始---
        ftime(&localTime);
        localTime.time += offsetSec;
        localTime.millitm += offsetMilsec;
        localTime.time += localTime.millitm / 1000;
        localTime.millitm %= 1000;

        tm t = *localtime(&localTime.time);
        SYSTEMTIME newSysTime;
        newSysTime.wYear = 1900 + t.tm_year, newSysTime.wMonth = 1 + t.tm_mon,
        newSysTime.wDayOfWeek = t.tm_wday, newSysTime.wDay = t.tm_mday,
        newSysTime.wHour = t.tm_hour, newSysTime.wMinute = t.tm_min,
        newSysTime.wSecond = t.tm_sec,
        newSysTime.wMilliseconds = (int)localTime.millitm;

        bool isSetSysTimeSuccess = SetLocalTime(&newSysTime);
        //------时间敏感区结束--------

        return isSetSysTimeSuccess ? 0 : GetLastError();
    }

    void closeSocket() {
        closesocket(sock);
        WSACleanup();
    }
};

/*
int main() {
  auto ezNtp = EzNtp();
  ezNtp.initSocket();
  Utils::printTimeB(ezNtp.getNtpTime());
  DWORD result = ezNtp.syncTime();
  cout << ((result == 0) ? "Success" : "Error") << " : " << result << endl;
  return 0;
}
*/