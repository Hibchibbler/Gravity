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
#include <queue>
#include "RingBuffer.h"

#define COMPLETION_KEY_UNKNOWN      0
#define COMPLETION_KEY_IO           1
#define COMPLETION_KEY_SHUTDOWN     2

namespace bali
{
    const ULONG MAX_PACKET_SIZE = 1024;

    class Mutex
    {
    public:
        void create()
        {
            InitializeCriticalSection(&critSection);
            init++;
        }

        void destroy()
        {
            DeleteCriticalSection(&critSection);
            init--;
        }

        void lock()
        {
            EnterCriticalSection(&critSection);
        }

        void unlock()
        {
            LeaveCriticalSection(&critSection);
        }

        USHORT isInit()
        {
            return init;
        }
    private:
        USHORT init;
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

        enum IOType
        {
            READ,
            WRITE
        };

        Overlapped()
        {
            ZeroMemory(&srcAddr, sizeof(srcAddr));
            srcAddrLen = 0;
            dstAddrLen = 0;
            this->Internal = 0;
            this->InternalHigh = 0;
            this->Offset = 0;
            this->OffsetHigh = 0;
            this->hEvent = 0;
            inuse = false;

            ZeroMemory(&srcAddr, sizeof(srcAddr));
            srcAddrLen = sizeof(srcAddr);

            ZeroMemory(&dstAddr, sizeof(dstAddr));
            dstAddrLen = sizeof(dstAddr);
        }

        SOCKADDR_STORAGE    srcAddr;
        INT                 srcAddrLen;
        SOCKADDR_STORAGE    dstAddr;
        INT                 dstAddrLen;
        IOType              ioType;
        uint32_t            index;
        UCHAR               buffer[MAX_PACKET_SIZE];
        BOOL                inuse;
    };

    class OverlapCollection
    {
    public:
#define JACK 1
        uint32_t index;
        HANDLE readMutex;
        Overlapped overlaps[JACK];

        OverlapCollection()
        {
            index = 0;
            readMutex = INVALID_HANDLE_VALUE;
            for (uint32_t i = 0; i < JACK; ++i)
            {
                overlaps[i].inuse = false;
                overlaps[i].index = i;
            }
        }

        bool getOverlapped(Overlapped** overlapped, Mutex* m)
        {
            bool status = false;
            m->lock();
            for (uint32_t i = 0; i <JACK; ++i)
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
            if (i < JACK && overlaps[i].inuse == true)
            {
                overlaps[i].inuse = false;
                status = true;
            }
            m->unlock();
            return status;
        }
    private:
    };
    class Address
    {
    public:
        SOCKADDR_STORAGE addr;
        uint32_t addrLen;
    };

    class Socket
    {
    public:
        Socket()
        {
            handle = INVALID_SOCKET;
            ZeroMemory(&addr, sizeof(addr));
        }

        void cleanup()
        {
            overlapMutex.destroy();
            closesocket(handle);
        }

        SOCKET handle;
        SOCKADDR_STORAGE addr;

        const uint8_t  MAX_OUTSTANDING = 10;
        Mutex overlapMutex;
        OverlapCollection overCollection;
        ULONG_PTR completionKey;
    };

    class Data
    {
    public:
        Data() = default;
        Data(const Data & _data)
        {
            size = _data.size;
            memcpy(payload, _data.payload, _data.size);
            srcAddrLen = _data.srcAddrLen;
            memcpy(&srcAddr, &_data.srcAddr, _data.srcAddrLen);
        }

        Data(uint32_t _size, void* _buffer, SOCKADDR_STORAGE & addr, int addrLen)
        {
            size = _size;
            memcpy(payload, _buffer, MAX_PACKET_SIZE);
            srcAddrLen = addrLen;
            memcpy(&srcAddr, &addr, addrLen);
        }

        Data & operator=(const Data & _data)
        {
            if (this != &_data)
            {
                this->size = _data.size;
                memcpy(this->payload, _data.payload, MAX_PACKET_SIZE);
                srcAddrLen = _data.srcAddrLen;
                memcpy(&this->srcAddr, &_data.srcAddr, _data.srcAddrLen);
            }
            return *this;
        }
        SOCKADDR_STORAGE srcAddr;
        int              srcAddrLen;
        uint32_t size;
        uint8_t payload[1024];
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
            INFO_CLIENT_RXQUEUE_EMPTY,
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
            FAILED_SOCKET_CLEANUP,
            INFO_SOCKET_READ_COMPLETED
        };

        class Result
        {
        public:
            Result(ResultType t) { type = t; code = 0; }
            Result(ResultType t, uint32_t c) { type = t; code = c; }
            ResultType type;
            uint32_t code;
        };
        typedef void(*IOHandler)(Data &data, Overlapped::IOType ioType);

        Network::Result initialize(uint32_t maxThreads, uint16_t port, IOHandler handler);
        Network::Result cleanup();
        Network::Result createWorkerThreads();
        Network::Result startWorkerThreads(Socket & rSock);
        //Network::Result stopWorkerThreads();
        Network::Result createPort(HANDLE & iocport);
        Network::Result associateSocketWithIOCPort(HANDLE iocport, Socket & s);
        Network::Result registerReaderSocket(Socket & s);
        Network::Result registerWriterSocket(Socket & s);
        Network::Result createSocket(Socket & s, ULONG_PTR completionKey);
        Network::Result bindSocket(Socket & s);
        Network::Result writeSocket(Socket & s, Data & data);
        Network::Result readSocket(Socket & s);
        Network::Result popRxData(Data & data);
        Network::Result pushTxData(Socket & s, Data & data);
        bool GetLocalAddressInfo(Socket & s);
        HANDLE & getIOCPort()
        {
            return ioPort;
        }
    private:
        Socket* readerSocket;
        Socket* writerSocket;
        HANDLE ioPort;
        std::list<Thread> threads;
        uint32_t maxThreads;
        uint16_t port;
        Mutex rxDataMutex;
        std::queue<Data> rxData;
        IOHandler ioHandler;
    private:
        static void* WorkerThread(Network* context);
    };
}
