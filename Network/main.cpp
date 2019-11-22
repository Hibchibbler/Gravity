            /////////////////////////////////////////////////////////////////////////////////
//// Daniel J Ferguson
//// 2017
/////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <iostream>
#include <conio.h>

#ifndef UNICODE
#define UNICODE
#endif


#include "Network.h"

using namespace bali;

bali::Network net;
bali::Socket readerSocket;
bali::Socket writerSocket;
bali::Mutex ioHandleLock;

struct Event {
    DWORD tid;
    uint64_t id;
    Overlapped::IOType iotype;
};

Event* g_EventArray;// [10000];
uint64_t g_EventIndex;
const uint64_t MAX_EVENTS = 10000;

void IOHandler(bali::Data & data, Overlapped::IOType ioType, uint64_t id)
{
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);
    if (ioType == Overlapped::IOType::READ)
    {


        // Prepare another Read to keep this perpetuating.
        // Hell, prepare two!
        const uint64_t prepare_max = 1;
        uint64_t prepared = 0;
        do
        {
            result = net.read(readerSocket);
            if (result.type == Network::ResultType::FAILED_SOCKET_NOMOREOVERLAPPED)
            {
            }
        } while ((result.type == bali::Network::ResultType::SUCCESS) && prepared++ < prepare_max);
        
        // Print payload
        std::string payloadAscii((PCHAR)data.payload, data.size);
        //ioHandleLock.lock();


        std::cout << "<" << prepared << ">" << "[" << id << "][" << data.size << "]" << payloadAscii.c_str() << std::endl;
        //std::cout << "ioHandler for thread=" << GetCurrentThreadId() << ", id=" << id << ", ioType=" << ioType << "\n";


        //ioHandleLock.unlock();
        Event anEvent;
        anEvent.id = id;
        anEvent.tid = GetCurrentThreadId();
        anEvent.iotype = ioType;

        g_EventIndex = InterlockedIncrement(&g_EventIndex);
        g_EventArray[(g_EventIndex - 1) % MAX_EVENTS] = anEvent;
        //// Prepare response
        //memcpy(data.payload, "BAD Dog", 8);
        //data.size = 8;

        //// Send response
        //net.write(writerSocket, data);
    }
    else if (ioType == Overlapped::IOType::WRITE)
    {
    }
    return;
}


int main()
{
    bool done = false;

    bali::Network::Result result(bali::Network::ResultType::SUCCESS);
    ioHandleLock.create();
    g_EventArray = new Event[sizeof(Event) * MAX_EVENTS];
    g_EventIndex = 0;
    do {
        std::cout << ">";
        char val = (char)_getch(); /// press any key to continue ///
        switch (val)
        {
        case '0':
            std::cout << "0\nCreate IOCP and Sockets and Workers...";
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
            std::cout << "Done." << std::endl;
            break;
        case '1'://Exit
            std::cout << "1\nShutting down worker threads...";
            net.shutdownWorkerThreads();
            std::cout << "Done." << std::endl;
            break;
        case '2':
            std::cout << "2\nClosing sockets, and ports.\n";
            writerSocket.cleanup();
            readerSocket.cleanup();
            net.cleanup();
            ioHandleLock.destroy();
            break;
        case '3':
            std::cout << "3\nExiting app.\n";
            done = true;
            break;
        default:
            break;
        }
    } while (!done);

    delete [] g_EventArray;

    return 0;
}