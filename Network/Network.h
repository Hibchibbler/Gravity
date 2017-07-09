///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <memory>
#include <array>
#include <list>
#include <vector>
#include <cassert>
#include <stdint.h>

#define COMPLETION_KEY_UNKNOWN      0
#define COMPLETION_KEY_IO           1
#define COMPLETION_KEY_SHUTDOWN     2

namespace bali
{
    class Thread
    {
    public:
        HANDLE handle;
        DWORD id;
    };

    class Overlapped : public WSAOVERLAPPED
    {
    public:
        typedef std::unique_ptr<Overlapped> Ptr;

        enum Limits
        {
            MAX_DGRAM_SIZE = 1024
        };

        enum IOType
        {
            READ,
            WRITE
        };

        Overlapped()
        {
            ZeroMemory(&srcAddr, sizeof(srcAddr));
            srcAddrLen = 0;
            bytesReceived = 0;
            this->Internal = 0;
            this->InternalHigh = 0;
            this->Offset = 0;
            this->OffsetHigh = 0;
            this->hEvent = 0;
        }

        bool operator==(uint64_t oid)
        {
            if (this->oid == oid)
                return true;
            return false;
        }

        SOCKADDR_STORAGE    srcAddr;
        INT                 srcAddrLen;

        IOType              ioType;
        int                 bytesReceived;
        uint64_t            oid;
        UCHAR               buffer[MAX_DGRAM_SIZE];
    };

    class Socket
    {
    public:
        Socket()
        {
            handle = INVALID_SOCKET;
            ZeroMemory(&addr, sizeof(addr));
            sid = 0;
            oIndex = 0;
        }

        SOCKET handle;
        SOCKADDR_STORAGE addr;
        ULONG_PTR sid;

        const uint8_t  MAX_OUTSTANDING = 5;
        std::array<Overlapped::Ptr, 5> outstanding;
        uint32_t oIndex;

        bool addOutstanding(Overlapped::Ptr & ptr)
        {
            bool status = false;
            for (auto o = outstanding.begin(); o != outstanding.end(); ++o)
            {
                if (!(*o))
                {
                    *o = std::move(ptr);
                    status = true;
                    break;
                }
            }
            return status;
        }

        bool removeOutstanding(Overlapped* overlapped)
        {
            bool status = false;
            for (int o = 0; o < outstanding.size(); ++o)
            {
                if (outstanding[o] && outstanding[o]->oid == overlapped->oid)
                {
                    status = true;
                    outstanding[o].reset(nullptr);
                    break;
                }
            }
            return status;
        }
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
            FAILED_SOCKET_NOMOREOVERLAPPED,
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
        uint64_t nextOID; // Next Overlapped ID
        uint64_t nextSID; // Next Socket ID
        HANDLE ioPort;
        Socket recvSocket;
        std::list<Thread> threads;
        std::list<Socket> sockets;

        uint32_t maxThreads;
        uint16_t port;

        SOCKADDR_STORAGE them;
        INT themSize;
        char rBuffer[1024];
    private:
        static void* WorkerThread(Network* context);
    };
}
