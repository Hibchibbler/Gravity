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
#include <iostream>

#define COMPLETION_KEY_UNKNOWN      0
#define COMPLETION_KEY_IO           1
#define COMPLETION_KEY_SHUTDOWN     2

namespace bali
{
    class Mutex
    {
    public:
        void create()
        {
            InitializeCriticalSection(&critSection);
        }

        void destroy()
        {
            DeleteCriticalSection(&critSection);
        }

        void lock()
        {
            EnterCriticalSection(&critSection);
        }

        void unlock()
        {
            LeaveCriticalSection(&critSection);
        }
    private:
        CRITICAL_SECTION critSection;
    };

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
            inuse = false;

            ZeroMemory(&srcAddr, sizeof(srcAddr));
            srcAddrLen = sizeof(srcAddr);
        }

        SOCKADDR_STORAGE    srcAddr;
        INT                 srcAddrLen;

        IOType              ioType;
        DWORD               bytesReceived;
        uint32_t            index;
        UCHAR               buffer[MAX_DGRAM_SIZE];
        BOOL                inuse;
    };

    class OverlapCollection
    {
    public:
        //std::vector<Overlapped> outstanding;
        Overlapped overlaps[10];
        uint32_t index;
        HANDLE readMutex;
        OverlapCollection()
        {
            index = 0;
            readMutex = INVALID_HANDLE_VALUE;

            for (uint32_t i = 0; i < 10; ++i)
            {
                overlaps[i].inuse = false;
                overlaps[i].index = i;
            }
        }

        bool getOverlapped(Overlapped** overlapped, Mutex* m)
        {
            bool status = false;
            m->lock();
            for (uint32_t i = 0; i <10; ++i)
            {
                *overlapped = &overlaps[i];
                if ((*overlapped)->inuse == false)
                {
                    (*overlapped)->inuse = true;
                    status = true;
                    break;
                }
            };
            m->unlock();
            return status;
        }

        bool expireOverlapped(uint32_t i, Mutex* m)
        {
            bool status = false;
            m->lock();
            if (i < 10 && overlaps[i].inuse == true)
            {
                overlaps[i].inuse = false;
                status = true;
            }
            m->unlock();
            return status;
        }
    private:
    };

    class Socket
    {
    public:
        Socket()
        {
            handle = INVALID_SOCKET;
            ZeroMemory(&addr, sizeof(addr));
        }

        SOCKET handle;
        SOCKADDR_STORAGE addr;

        const uint8_t  MAX_OUTSTANDING = 10;
        Mutex overlapMutex;
        OverlapCollection overCollection;
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
            FAILED_SYNCHR_CREATE_READER_MUTEX,
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
        HANDLE ioPort;
        Socket recvSocket;
        std::list<Thread> threads;

        uint32_t maxThreads;
        uint16_t port;
    private:
        static void* WorkerThread(Network* context);
    };
}
