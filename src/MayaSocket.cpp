#include "MayaSocket.h"

void MayaSocket::connect() {
    try {
        internal_socket.connect("localhost", 5055);
        connected = true;
    } catch (int e) {
        connected = false;
    }
}

MayaSocket::MayaSocket() {
    connect();
}

MayaSocket::~MayaSocket() {

}

bool MayaSocket::send(const std::string& cmd) {
    try {
        internal_socket.send(cmd);
        return true;
    } catch (int e) {
        connect();
        return false;
    }
}

bool MayaSocket::isConnected() {
    return connected;
}