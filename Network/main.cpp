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

using namespace bali;

bali::Network net;
bali::Socket recvSocket;
bali::Socket sendSocket;

void IOHandler(bali::Data & data, Overlapped::IOType ioType)
{
    if (ioType == Overlapped::IOType::READ)
    {
        std::string payloadAscii((PCHAR)data.payload, data.size);
        std::cout << "[" << data.size << "]" << payloadAscii.c_str() << std::endl;

        memcpy(data.payload, "BAD Dog", 8);
        data.size = 8;
        net.writeSocket(sendSocket, data);
    }
    else if (ioType == Overlapped::IOType::WRITE)
    {
        std::cout << "Write went through   ";
    }
    return;
}

int main()
{
    int a;
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);

    net.initialize(8, 8967, IOHandler);
    result = net.createPort(net.getIOCPort());
    result = net.createWorkerThreads();
    result = net.createSocket(sendSocket, COMPLETION_KEY_IO);
    result = net.createSocket(recvSocket, COMPLETION_KEY_IO);
    result = net.bindSocket(recvSocket);
    result = net.associateSocketWithIOCPort(net.getIOCPort(), sendSocket);
    result = net.associateSocketWithIOCPort(net.getIOCPort(), recvSocket);
    result = net.registerReaderSocket(recvSocket);
    result = net.registerWriterSocket(sendSocket);
    result = net.startWorkerThreads(recvSocket);

    /////////////////////////////////////////////////////////////
    std::cin >> a;  ///      press "any" key to continue      ///
    /////////////////////////////////////////////////////////////

    std::cout << "Bye bye." << std::endl;

    sendSocket.cleanup();
    recvSocket.cleanup();
    net.cleanup();

    return 0;
}
