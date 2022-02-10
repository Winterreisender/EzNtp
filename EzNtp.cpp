#include "EzNtp.h"
using namespace std;

int main() {
  auto ezNtp = EzNtp();
  ezNtp.initSocket();
  Utils::printTimeB(ezNtp.getNtpTime());
  DWORD result = ezNtp.syncTime();
  ezNtp.closeSocket();
  cout << "Set to system time..." << ((result == 0) ? "Success" : "Error") << " : " << result << endl;
  return 0;
}