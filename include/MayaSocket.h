#ifndef __MAYASOCKET_H
#define __MAYASOCKET_H
#include <vector>
#include <iostream>
#include "Socket.h"
#include "TrackingData.h"

class MayaSocket
{
	public:	
		MayaSocket();
        virtual ~MayaSocket();

        bool send(TrackingData &data);
        bool isConnected();

	private:
        void connect();

        Socket internal_socket;
        bool connected;
};

#endif 	/* MAYASOCKET_H */
