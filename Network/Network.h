///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <memory>
#include <list>
#include <vector>
#include <stdint.h>

namespace bali
{
    class Thread
    {
    public:
        HANDLE handle;
        DWORD id;
    };

    class Overlapped : public OVERLAPPED
    {
    public:
        typedef std::unique_ptr<Overlapped> Ptr;

        enum IOType
        {
            READ,
            WRITE
        };

        std::vector<UCHAR> buffer;
        IOType ioType;
        int bytesReceived;
    };

    class Socket
    {
    public:
        Socket()
        {
            handle = INVALID_SOCKET;
            ZeroMemory(&addr, sizeof(addr));
            key = NULL;
        }

        SOCKET handle;
        SOCKADDR_STORAGE addr;
        ULONG_PTR key;
        std::list<Overlapped::Ptr> outstanding;
    };

    class Network
    {
    public:
        
        Network();
        ~Network();

        enum ResultType
        {
            SUCCESS,
            FAILED,
            FAILED_IOPORT_ASSOCIATION,
            FAILED_IOPORT_CREATE,
            FAILED_THREAD_CREATE,
            FAILED_THREAD_START,
            FAILED_SOCKET_RECVFROM,
            FAILED_SOCKET_SENDTO,
            FAILED_SOCKET_BIND,
            FAILED_SOCKET_CREATE,
            FAILED_SOCKET_STARTUP,
            FAILED_SOCKET_CLEANUP
        };

        class Result
        {
        public:
            Result(ResultType t) { type = t; code = 0; }
            Result(ResultType t, uint32_t c) { type = t; code = c; }
            ResultType type;
            uint32_t code;
        };

        Network::Result initialize(uint32_t maxThreads, uint16_t port);
        Network::Result cleanup();

        Network::Result createWorkerThreads();
        Network::Result startWorkerThreads();
        Network::Result createPort();
        Network::Result associate(Socket & s);

        Network::Result createSocket(Socket & s);
        Network::Result bindSocket(Socket & s);
        Network::Result writeSocket(Socket & s);
        Network::Result readSocket(Socket & s);

        bool GetLocalAddressInfo(Socket & s);
        Socket & getReceiverSock()
        {
            return recvSocket;
        }
    private:
        HANDLE ioPort;
        Socket recvSocket;
        std::list<Thread> threads;
        std::list<Socket> sockets;

        uint32_t maxThreads;
        uint16_t port;
    private:
        static void* WorkerThread(Network* context);
    };
}
