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

bool MayaSocket::send(TrackingData &data) {
    /*
    std::string cmd = "";
    try {
        if(internal_socket.send(cmd) < cmd.length()) {
            std::cout << "full message not sent!" << std::endl;
        }
        return true;
    } catch (int e) {
        connect();
        return false;
    }
    */
    return true;
}

bool MayaSocket::isConnected() {
    return connected;
}