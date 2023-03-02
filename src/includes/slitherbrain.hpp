#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <err.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>
#include <filesystem>
#include <memory>
#include <array>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex>
#include <seccomp.h>
#include <thread>
#include <signal.h>


#define NEWLINE "\n"
#define QUOTE '"'
#define ESCAPE '\\'
#define FSLASH '/'
#define NULLCHAR '\0'
#define ALNUM 25
#define TMPSZ 10
#define RE_IPV4 "^(((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.|:|$)){4}\\d{2,5})"
#define RE_IPV6 "^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))$"
#define BUFFSIZE 4096
#define ENDPOINT_EXEC "/execute"
#define METHOD "POST"
#define VC "HTTP/1.1"
#define CRLF "\r\n"
#define CRLFX2 "\r\n\r\n"
#define STATUS_OK "200 Ok"
#define STATUS_NF "404 Not Found"
#define STATUS_MNA "405 Method Not Allowed"
#define STATUS_SE "500 Internal Sever Error"
#define STATUS_UE "422 Unprocessable Entity"
#define STATUS_BR "400 Bad Request"
#define TIME_FORMAT "%a, %d %b %Y %I:%M:%S GMT"
#define SERVER_NAME "Slitherbrain Python Code Runner Revision 1"
#define CONT_TYPE "text/plain"
#define CHECKSUM_HN "Checksum: "
#define SERVER_NAME_HN "Server: "
#define DATE_HN "Date: "
#define CONT_LEN_HN "Content-Length: "
#define CONT_TYPE_HN "Content-Type: "
#define BODY_ERR "Error Ocurred"

#define ERR_EXIT(message)                              \
                                                       \
    do                                                 \
    {                                                  \
        cout << "\033[1;31mError: \033[0m" << message; \
        exit(1);                                       \
    } while (0)


#define POX_PRIMNUM 32
#define POX_BLOCKNUM 64
#define POX_PORTNUM 16
#define POX_FACTNUM 4

#define ROT16LEFT4(num) ((num << 4) | (num >> (16 - 4)))
#define ROT16RIGHT4(num) ((num >> 4) | (num << (16 - 4)))

#define POX_ALPHA(a) a |= ROT16LEFT4(a) | ROT16RIGHT4(a * 2)
#define POX_DELTA(a, b) a = ROT16LEFT4(b) | 0xffcd
#define POX_THETA(a, b, c) c = (a * (ROT16RIGHT4(b + a))) >> 2
#define POX_OMEGA(a, b, c, d) a = ((a >> 2) * (b >> 4) * (c >> 6)) | ROT16LEFT4(d)


namespace slitherbrain {
    using namespace std;

    namespace args {
        vector<string> parseArgsAndRun(int argc, char **argv);
        string hasFlagIfSoRemoveMarker(char *arg, const char *flag);
    }
    namespace process {
        string runSlitherRunProcess(string slitherrun_path, string python_path, string disallowed_calls, string code);
        string execCommand(string cmd);
        static void sandboxProcess(vector<string> to_disallow);
    }
    namespace net {
        typedef enum NetworkAddressType
        {
            AddrTypeIPV4,
            AddrTypeIPV6,
        } netAddr_t;

        typedef struct sockaddr_in sockAddrIn_t;
        typedef struct sockaddr *pSockAddr_t;
        
        sockAddrIn_t newSocketAddress(string ip, uint16_t port, netAddr_t addr_type);
        netAddr_t getAddrType(string addr);
        int listenToSocket(sockAddrIn_t &socket_addr, netAddr_t addr_type);
        int accepetNewConnection(sockAddrIn_t &socket_addr, int listener);
        string readClientConnection(int clientsock);
        void readSocketExecuteAndSendBack(int clientsock, string slitherrun_path, string python_path, string disallowed_calls);
        void serveHttpForever(string ip, int port, string slitherrun_path, string python_path, string disallowed_calls);
        
        
    }
    namespace http {
        typedef enum HttpParseError
        {
            ParseEmptyRequest,
            ParseWrongProtocol,
            ParseWrongMethod,
            ParseWrongEndpont,
            ParseWrongVC,
            ParseNoBody,
            ParseReqLineOk,
            ParseBadCode,
        } parseActonStat_t;
        typedef struct tm *pTime_t;

        string composeResponseLine(parseActonStat_t action_stat);
        string composeResponseHeaders(size_t clen);
        string composeResponse(parseActonStat_t action_state, string response_body);
        parseActonStat_t validateReqestLine(string requestLine);
        parseActonStat_t parseRequest(string request, string &code);
        string getCheckSum(vector<string> headers);
        bool checkCodeIntegrity(string code, string checksum);

   
    }
    namespace utils {
        string getCurrentUtcFormattedTime();
        size_t randomNum(size_t min, size_t max);
        string randomString(size_t size);
        string readConfigFile(string fpath);

    }
    namespace strtools {
        bool charAtRightIs(string s, char c);
        void trimCharRight(string &str, char c);        
        void trimCharLeft(string &str, char c);
        void trimChar(string &str, vector<char> chars);
        vector<string> splitStr(string str, const string delimiter);
        string joinStrVector(vector<string> strs, const string delemiter);
        void replaceChar(string &str, const char c, char r);
        void escapeSequence(string &str);   
    }
    namespace filetools {
        string pathJoinTmp(string path);
        void writeStringToFile(string fpath, string contents);
        string processCodeAndSaveTemp(string code);
        string removeScriptFile(string fpath);
        
    }
    namespace pox {
        void poxRound(uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d);
        void poxProcessBlock(const char block[POX_BLOCKNUM], uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d);
        void padString(string &txt);
        string integerToHex(uint16_t a, uint16_t b, uint16_t c, uint16_t d);
    }
}

