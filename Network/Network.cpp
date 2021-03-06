///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#include "Network.h"
#include <Ws2tcpip.h>
#include <iostream>
#include <cassert>
using namespace bali;

#pragma comment(lib, "Ws2_32.lib")

Network::Network()
{

}

Network::~Network()
{

}

Network::Result Network::shutdownWorkerThreads()
{
    Network::Result result(Network::ResultType::SUCCESS);
    for (auto t = threads.begin(); t != threads.end(); ++t)
    {
        PostQueuedCompletionStatus(getIOCPort(), 0, COMPLETION_KEY_SHUTDOWN, NULL);
    }

    uint32_t threadsdone = 0;
    while (threads.size() > threadsdone)
    {
        for (auto t = threads.begin(); t != threads.end(); ++t)
        {
            if (t->started)
            {
                DWORD ret = WaitForSingleObject(t->handle, 100);
                if (ret == WAIT_OBJECT_0)
                {
                    t->started = 0;
                    threadsdone++;
                    std::cout << "Thread joined\n";
                }
                else if (ret == WAIT_TIMEOUT)
                {
                    std::cout << "Still Waiting\n";
                }
                else
                {
                    std::cout << "WEIRD shutdownworkerthreads\n";
                }
            }
        }
    }


    
    return result;
}

Network::Result Network::initialize(uint32_t maxThreads, uint16_t port, IOHandler ioHandler)
{
    Network::Result result(Network::ResultType::SUCCESS);
    this->maxThreads = maxThreads;
    this->port = port;
    this->ioPort = INVALID_HANDLE_VALUE;
    this->ioHandler = ioHandler;

    WSADATA wsaData;
    // Initialize Winsock
    int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (r != NO_ERROR) {
        result.type = Network::ResultType::FAILED_SOCKET_STARTUP;
        result.code = r;
    }
    return result;
}

Network::Result Network::cleanup()
{
    Network::Result result(Network::ResultType::SUCCESS);
    // Clean up and exit.
    CloseHandle(ioPort);

    wprintf(L"Cleanup.\n");
    WSACleanup();
    return result;
}

Network::Result  Network::createWorkerThreads()
{
    Network::Result result(Network::ResultType::SUCCESS);
    for (uint32_t t = 0; t < maxThreads; ++t)
    {
        Thread thread;
        thread.handle = CreateThread(NULL, 0,
            (LPTHREAD_START_ROUTINE)Network::WorkerThread,
            this,
            CREATE_SUSPENDED,
            &thread.id);

        if (thread.handle == INVALID_HANDLE_VALUE)
        {// Bad thread, bail
            result.type = Network::ResultType::FAILED_THREAD_CREATE;
            result.code = GetLastError();
            break;
        }
        threads.push_back(thread);
    }
    return result;
}
Network::Result Network::startWorkerThreads()
{
    Network::Result result(Network::ResultType::SUCCESS);
    for (auto t = threads.begin(); t != threads.end(); ++t)
    {
        DWORD ret = ResumeThread(t->handle);
        if (ret == -1)
        {
            result.type = Network::ResultType::FAILED_THREAD_START;
            result.code = GetLastError();
            break;
        }
        t->started = true;
    }

    // Kick off the perpetual reads
    if (result.type == Network::ResultType::SUCCESS)
    {
        result = read(*readerSocket);
    }
    return result;
}

//
// createPort creates an IO Completion Port, and returns a handle to it.
//
Network::Result Network::createPort(HANDLE & iocport)
{
    Network::Result result(Network::ResultType::SUCCESS);
    iocport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (iocport == NULL)
    {
        result.type = Network::ResultType::FAILED_IOPORT_CREATE;
        result.code = GetLastError();
    }
    return result;
}

//
// associateSocketWithIOCPort associates a Socket with an IO completion port
//
Network::Result Network::associateSocketWithIOCPort(HANDLE iocport, Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    HANDLE temp = CreateIoCompletionPort((HANDLE)s.handle, ioPort, s.completionKey, 0);
    if (temp == NULL)
    {
        result.type = Network::ResultType::FAILED_IOPORT_ASSOCIATION;
        result.code = GetLastError();
    }

    return result;
}

Network::Result Network::createSocket(Socket & s, ULONG_PTR completionKey)
{
    Network::Result result(Network::ResultType::SUCCESS);
    s.handle = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (s.handle != INVALID_SOCKET)
    {
        s.completionKey = completionKey;
    }
    else
    {
        result.type = Network::ResultType::FAILED_SOCKET_CREATE;
        result.code = WSAGetLastError();
    }
    return result;
}

Network::Result Network::bindSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    GetLocalAddressInfo(s);

    int ret = bind(s.handle, (SOCKADDR*)&s.local.addr, sizeof(s.local.addr));
    if (ret == SOCKET_ERROR)
    {
        result.type = Network::ResultType::FAILED_SOCKET_BIND;
        result.code = WSAGetLastError();
        std::cout << result.code << std::endl;
    }

    return result;
}

//
// write() posts a write-request to the IOCP
//
Network::Result Network::write(Socket & s, Data & data)
{
    Network::Result result(Network::ResultType::SUCCESS);
    Overlapped* o = nullptr;
    if (s.overlapPool.acquire(&o))
    {
        DWORD flags = 0;
        int ret = 0;
        o->ioType = Overlapped::IOType::WRITE;

        memcpy(&o->remote.addr, &data.remote.addr, sizeof(SOCKADDR_STORAGE));
        o->remote.len = data.remote.len;

        o->wsabuf.buf = (CHAR*)&data.payload;
        o->wsabuf.len = min(data.size, MAX_PACKET_SIZE);// Shit gets Truncated here.
        ret =
            WSASendTo(
                s.handle,
                &o->wsabuf,
                1,
                NULL,
                flags,
                (SOCKADDR*)&o->remote.addr,
                o->remote.len,
                (OVERLAPPED*)o,
                NULL);
        if (ret == 0)
        {//Good
         // SendTo finished now, but we'll let the worker thread handle the completion.
        }
        else
        {
            // Problem
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                result.type = Network::ResultType::FAILED_SOCKET_RECVFROM;
                result.code = err;
            }
            else
            {//Good
             // No Problem. The read has been posted.
            }
        }
    }
    else
    {
        result.type = Network::ResultType::FAILED_SOCKET_NOMOREOVERLAPPED;
    }
    return result;
}



//
// read() posts a read-request to the IOCP
//
// Result.type == Sucess
//  When a Read was posted to the IOCP
// Result.type == RecvFrom
//  When the API WSARecvFrom fails.
//
Network::Result Network::read(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    Overlapped* o = nullptr;
    if (s.overlapPool.acquire(&o))
    {
        o->ioType = Overlapped::IOType::READ;
        o->wsabuf.buf = (CHAR*)&o->buffer;
        o->wsabuf.len = MAX_PACKET_SIZE;

        DWORD flags = 0;
        int ret = 0;
        ret =
            WSARecvFrom(s.handle,
                &o->wsabuf,
                1,
                NULL,//&o->bytesReceived,// This can be NULL because lpOverlapped parameter is not NULL.
                &flags,
                (SOCKADDR*)&o->remote.addr,
                &o->remote.len,
                (OVERLAPPED*)o,
                NULL);
        if (ret == 0)
        {
            // No Problem - the read completed here and now
            // But we will just let a workerthread process it like normal.
            // keep things simple.
        }
        else
        {
            // Problem
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                result.type = Network::ResultType::FAILED_SOCKET_RECVFROM;
                result.code = err;
            }
            else
            {
                // No Problem - the read has been successfully posted
            }
        }
    }
    else
    {
        result.type = Network::ResultType::FAILED_SOCKET_NOMOREOVERLAPPED;
    }
    return result;
}

Network::Result Network::registerReaderSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    readerSocket = &s;
    return result;
}

Network::Result Network::registerWriterSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    writerSocket = &s;
    return result;
}

//
// Completes IO requests that were previously posted
// to the IO completion port.
// Processes Reads and Writes.
//
void* Network::WorkerThread(Network* context)
{
    Network::Result result(Network::ResultType::SUCCESS);
    DWORD bytesTrans = 0;
    ULONG_PTR ckey = COMPLETION_KEY_UNKNOWN;
    LPWSAOVERLAPPED pOver;

    uint64_t id = InterlockedIncrement(&context->threadidmax);

    bool done = false;
    while (!done)
    {
        BOOL ret = GetQueuedCompletionStatus(context->ioPort, &bytesTrans, &ckey, (LPOVERLAPPED*)&pOver, INFINITE);
        if (ret == TRUE)
        {// Ok
            if (ckey == COMPLETION_KEY_IO)
            {
                assert(pOver != NULL);
                if (pOver != NULL)
                {
                    Overlapped* overlapped = reinterpret_cast<Overlapped*>(pOver);
                    // Capture all the packet information
                    Data data(bytesTrans, overlapped->buffer, overlapped->remote.addr, overlapped->remote.len);
                    Overlapped::IOType ioType = overlapped->ioType;

                    if (ioType == Overlapped::IOType::READ)
                    {
                        // relinquish this overlapped structure
                        context->readerSocket->overlapPool.release(overlapped->index);
                    }
                    else if (ioType == Overlapped::IOType::WRITE)
                    {
                        // reqlinquish this overlapped structure
                        context->writerSocket->overlapPool.release(overlapped->index);
                    }

                    //Notify the user
                    // nothing is locked, data is copied, 
                    // alls we're doin' now is 

                    context->ioHandler(data, ioType, id);
                }
            }
            else if (ckey == COMPLETION_KEY_SHUTDOWN)
            {
                std::cout << "Worker: Shutdown Request Received!\n";
                done = true;
                continue;
            }
        }
        else
        {
            // Problem
            std::cout << "Worker: ERROR GetQueuedCompletionStatus()\n";
            return NULL;
        }
    }
    std::cout << "Worker: Shutdown Complete\n";
    return NULL;
}

bool Network::GetLocalAddressInfo(Socket & s)
{
    ADDRINFOW hints, *res;
    int ret;
    bool status = true;
    wchar_t port_buffer[6];

    wsprintf(port_buffer, L"%hu", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    // Setting AI_PASSIVE will give you a wildcard address if addr is NULL
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV | AI_PASSIVE;

    if ((ret = GetAddrInfo(NULL, port_buffer, &hints, &res)) != 0)
    {
        //(L"getaddrinfo: %s\n", gai_strerror(status));
        status = false;
    }
    else
    {
        // Note; we're hoping that the first valid address is the right address. glta.
        s.local.len = res->ai_addrlen;
        memcpy(&s.local.addr, res->ai_addr, res->ai_addrlen);
        FreeAddrInfo(res);
    }

    return status;
}
