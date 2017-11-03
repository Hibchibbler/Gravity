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
bali::Socket readerSocket;
bali::Socket writerSocket;
bali::Mutex ioHandleLock;
void IOHandler(bali::Data & data, Overlapped::IOType ioType)
{
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);
    if (ioType == Overlapped::IOType::READ)
    {
        ioHandleLock.lock();
        // Print payload
        std::string payloadAscii((PCHAR)data.payload, data.size);
        std::cout << "[" << data.size << "]" << payloadAscii.c_str() << std::endl;
        ioHandleLock.unlock();
        // Prepare response
        memcpy(data.payload, "BAD Dog", 8);
        data.size = 8;
        // Send response
        net.write(writerSocket, data);

        // Prepare another Read to keep this perpetuating
        result = net.read(readerSocket);
        if (result.type == Network::ResultType::FAILED_SOCKET_NOMOREOVERLAPPED)
            std::cout << "NMO ";


        //do
        //{
        //    result = net.readSocket(readerSocket);
        //} while (result.type == bali::Network::ResultType::SUCCESS);
    }
    else if (ioType == Overlapped::IOType::WRITE)
    {
    }
    return;
}

int main()
{
    int a;
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);

    ioHandleLock.create();

    result = net.initialize(8, 8967, IOHandler);
    result = net.createPort(net.getIOCPort());
    result = net.createWorkerThreads();
    result = net.createSocket(writerSocket, COMPLETION_KEY_IO);
    result = net.createSocket(readerSocket, COMPLETION_KEY_IO);
    result = net.bindSocket(readerSocket);
    result = net.associateSocketWithIOCPort(net.getIOCPort(), writerSocket);
    result = net.associateSocketWithIOCPort(net.getIOCPort(), readerSocket);
    result = net.registerReaderSocket(readerSocket);
    result = net.registerWriterSocket(writerSocket);
    result = net.startWorkerThreads();

    /////////////////////////////////////////////////////////////
    std::cin >> a;  ///      press "any" key to continue      ///
    /////////////////////////////////////////////////////////////

    std::cout << "Bye bye." << std::endl;

    writerSocket.cleanup();
    readerSocket.cleanup();
    net.cleanup();
    ioHandleLock.destroy();
    return 0;
}