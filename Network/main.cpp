/////////////////////////////////////////////////////////////////////////////////
//// Daniel J Ferguson
//// 2017
/////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#ifndef UNICODE
#define UNICODE
#endif


#include "Network.h"
void ProcessResult(std::string input)
{

}
int main()
{
    bali::Network net;
    bali::Socket recvSock;

    net.initialize(5, 8967);
    net.createPort();
    net.createWorkerThreads();
    
    net.createSocket(net.getReceiverSock());
    net.bindSocket(net.getReceiverSock());
    net.associate(net.getReceiverSock());
    net.startWorkerThreads();
    net.readSocket(net.getReceiverSock());
    int g;
    std::cin >> g;
    net.cleanup();
    
    return 0;
}
