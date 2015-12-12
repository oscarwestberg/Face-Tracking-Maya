#include "MayaSocket.h"

#include <string>

#include "opencv2/core/core.hpp"


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

std::string transMelCmd(std::string name, Point2f pos) {
    //FULKOD! borde kanske basera skalning på något annat, typ ansiktsbredd istället för att bara dela med godtyckligt tal? kanske göra det möjligt att välja för olika delar av ansiktet?
    return "setAttr " + name + ".translateX " + std::to_string(pos.x/400) + ";\n setAttr " + name + ".translateY " + std::to_string(-pos.y/400) + ";\n";
}

int frm = 0;
bool MayaSocket::send(TrackingData &data) {
    std::string cmd = "";

    cmd += transMelCmd("lob",data.leftouterbrow);
    cmd += transMelCmd("lib",data.leftinnerbrow);
    cmd += transMelCmd("rib",data.rightinnerbrow);
    cmd += transMelCmd("rob",data.rightouterbrow);

    cmd += transMelCmd("lc",data.leftcheek);
    cmd += transMelCmd("rc",data.rightcheek);

    cmd += transMelCmd("ln",data.leftnose);
    cmd += transMelCmd("rn",data.rightnose);

    cmd += transMelCmd("ul",data.upperlip);

    cmd += transMelCmd("lm",data.leftmouth);
    cmd += transMelCmd("rm",data.rightmouth);
    
    cmd += transMelCmd("ll",data.lowerlip);
    
    //cmd += "currentTime " + std::to_string(frm++) + ";\n";
    //cmd += "currentTime (`currentTime -query` + " + std::to_string(data.timeStep*24) + ");\n";
    //cmd += "setKeyframe lob lib rib rob lc rc ln rn lm ul rm ll;\n";
    try {
        if(internal_socket.send(cmd) < cmd.length()) {
            std::cout << "full message not sent!" << std::endl;
        }
        return true;
    } catch (int e) {
        connect();
        return false;
    }
    
    return true;
}

bool MayaSocket::isConnected() {
    return connected;
}