#include "Socket.h"

#ifdef _WIN32
# include <winsock2.h>
#else
#include <sys/socket.h>
#endif

Socket::Socket() {
    #ifdef _WIN32
    WORD versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(versionWanted, &wsaData);
    #endif
}


Socket::~Socket() {
}