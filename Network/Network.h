///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <WinSock2.h>
#include <windows.h>
//#include <memory>
//#include <array>
#include <list>
//#include <vector>
#include <cassert>
#include <stdint.h>
#include <iostream>
//#include <queue>

//
// Network, Socket, Overlap, and OverlapPool are used together
// to create a UDP IOCP based Server.
// Network 
//  Used to coordinate everything. The flow is as follows:
//  
// 
//

#define COMPLETION_KEY_UNKNOWN      0
#define COMPLETION_KEY_IO           1
#define COMPLETION_KEY_SHUTDOWN     2

#define INUSE       1
#define NOT_INUSE   0

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
        DWORD started;
    };

    class Address
    {
    public:
        Address() 
        {
            ZeroMemory(&addr, sizeof(SOCKADDR_STORAGE));
            len = sizeof(SOCKADDR_STORAGE);
        }
        SOCKADDR_STORAGE    addr;
        INT                 len;
    };

    class Overlapped
    {
    public:
        enum IOType
        {
            READ,
            WRITE
        };

        Overlapped()
        {
            this->wsaoverlapped.Internal = 0;
            this->wsaoverlapped.InternalHigh = 0;
            this->wsaoverlapped.Offset = 0;
            this->wsaoverlapped.OffsetHigh = 0;
            this->wsaoverlapped.hEvent = 0;
            inuse = NOT_INUSE;
        }
        // WSAOVERLAPPED must be first member
        WSAOVERLAPPED       wsaoverlapped;
        Address             remote;
        IOType              ioType;
        uint32_t            index;
        UCHAR               inuse;
        WSABUF              wsabuf;
        UCHAR               buffer[MAX_PACKET_SIZE];
        
    };
    class OverlapPool
    {
    public:
#define JACK 16
        uint32_t index;
        Overlapped pool[JACK];

        OverlapPool()
        {
            index = 0;
            for (uint32_t i = 0; i < JACK; ++i)
            {
                pool[i].inuse = NOT_INUSE;
                pool[i].index = i;
            }
        }

        ~OverlapPool()
        {
        }

        //
        // Acquire an unused overlap
        // Return false when there is not an unused overlap available.
        //
        bool acquire(Overlapped** overlapped)
        {
            bool acquired = false;
            for (uint32_t i = 0; i <JACK; ++i)
            {
                *overlapped = &pool[i];
                if (InterlockedExchange8((char*)&(*overlapped)->inuse, INUSE) == NOT_INUSE)
                {//Wasn't already in use. So, we want it, and we have it.
                    acquired = true;
                    break;
                }
                else
                {//Was already in use. So we don't want it,
                 // We didn't change anything that wasn't already true;
                    continue;
                }
            };
            return acquired;
        }

        //
        // Release a used overlap
        //
        void release(uint32_t i)
        {
            InterlockedExchange8((char*)&(pool[i].inuse), NOT_INUSE);
        }
    private:
    };
    class Socket
    {
    public:
        Socket()
        {
            handle = INVALID_SOCKET;
        }

        void cleanup()
        {
            closesocket(handle);
        }

        SOCKET handle;
        Address local;
        OverlapPool overlapPool;
        ULONG_PTR completionKey;
    };
    class Data
    {
    public:
        Data(const Data & _data)
        {
            *this = _data;
        }

        Data(uint32_t _size, void* _buffer, SOCKADDR_STORAGE & addr, int addrLen)
        {
            size = _size;
            memcpy(payload, _buffer, MAX_PACKET_SIZE);
            remote.len = addrLen;
            memcpy(&remote.addr, &addr, sizeof(SOCKADDR_STORAGE));
        }

        Data & operator=(const Data & _data)
        {
            if (this != &_data)
            {
                this->size = _data.size;
                memcpy(this->payload, _data.payload, MAX_PACKET_SIZE);
                remote.len = _data.remote.len;
                memcpy(&this->remote.addr, &_data.remote.addr, sizeof(SOCKADDR_STORAGE));
            }
            return *this;
        }
        Address remote;
        uint32_t size;
        uint8_t payload[1024];n
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
        typedef void(*IOHandler)(Data &data, Overlapped::IOType ioType, uint64_t id);

        Network::Result shutdownWorkerThreads();

        Network::Result initialize(uint32_t maxThreads, uint16_t port, IOHandler handler);
        Network::Result cleanup();
        Network::Result createWorkerThreads();
        Network::Result startWorkerThreads();
        Network::Result createPort(HANDLE & iocport);
        Network::Result associateSocketWithIOCPort(HANDLE iocport, Socket & s);
        Network::Result registerReaderSocket(Socket & s);
        Network::Result registerWriterSocket(Socket & s);
        Network::Result createSocket(Socket & s, ULONG_PTR completionKey);
        Network::Result bindSocket(Socket & s);
        // write posts a write-request to the IOCP
        Network::Result write(Socket & s, Data & data);
        // read posts a read-request to the IOCP
        Network::Result read(Socket & s);
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
        IOHandler ioHandler;
        uint64_t threadidmax;
    private:
        static void* WorkerThread(Network* context);
    };
}