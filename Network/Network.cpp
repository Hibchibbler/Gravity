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
    nextOID = 1;
    nextSID = 1;
}

Network::~Network()
{

}

Network::Result Network::initialize(uint32_t maxThreads, uint16_t port)
{
    Network::Result result(Network::ResultType::SUCCESS);
    this->maxThreads = maxThreads;
    this->port = port;
    this->ioPort = INVALID_HANDLE_VALUE;

    WSADATA wsaData;
    //-----------------------------------------------
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
    //-----------------------------------------------
    // Clean up and exit.
    closesocket(recvSocket.handle);
    CloseHandle(ioPort);
    
    wprintf(L"Exiting.\n");
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
    }

    return result;
}

Network::Result Network::createPort()
{
    Network::Result result(Network::ResultType::SUCCESS);
    ioPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
    if (ioPort == NULL)
    {
        result.type = Network::ResultType::FAILED_IOPORT_CREATE;
        result.code = GetLastError();
    }
    return result;
}

Network::Result Network::associate(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    s.sid = InterlockedIncrement(&nextSID);
    HANDLE temp = CreateIoCompletionPort((HANDLE)s.handle, ioPort, COMPLETION_KEY_IO, 0);
    if (temp == NULL)
    {
        result.type = Network::ResultType::FAILED_IOPORT_ASSOCIATION;
        result.code = GetLastError();
    }

    return result;
}

Network::Result Network::createSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    s.handle = WSASocket(AF_UNSPEC, SOCK_DGRAM, IPPROTO_UDP, NULL , 0 , WSA_FLAG_OVERLAPPED);
    if (s.handle == INVALID_SOCKET)
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

    int ret = bind(s.handle, (SOCKADDR*)&s.addr, sizeof(s.addr));
    if (ret == SOCKET_ERROR)
    {
        result.type = Network::ResultType::FAILED_SOCKET_BIND;
        result.code = WSAGetLastError();
        std::cout << result.code << std::endl;
    }

    return result;
}
//Overlapped::Iter getNewOverlapped()
//{
//
//}

Network::Result Network::writeSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    return result;
}



Network::Result Network::readSocket(Socket & s)
{
    Network::Result result(Network::ResultType::SUCCESS);
    if (s.oIndex != 5)
    {
        Overlapped::Ptr o = std::make_unique<Overlapped>();
     
        o->ioType = Overlapped::IOType::READ;
        o->oid = InterlockedIncrement(&nextOID);

        ZeroMemory(&o->srcAddr, sizeof(o->srcAddr));
        o->srcAddrLen = sizeof(SOCKADDR_STORAGE);
        
        WSABUF wsaBuf;
        wsaBuf.buf = (CHAR*)&o->buffer;
        wsaBuf.len = Overlapped::Limits::MAX_DGRAM_SIZE;

        DWORD flags = 0;
        int ret = 0;
        ret =
            WSARecvFrom(s.handle,
                &wsaBuf,
                1,
                NULL,// This can be NULL because lpOverlapped parameter is not NULL.
                &flags,
                (SOCKADDR*)&o->srcAddr,
                &o->srcAddrLen,
                o.get(),
                NULL);
        if (ret == 0)
        {
            // No Problem
            assert(false);
            s.addOutstanding(o);
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
                s.addOutstanding(o);
            }
        }
    }
    else
    {
        result.type = Network::ResultType::FAILED_SOCKET_NOMOREOVERLAPPED;
    }
    return result;
}

void* Network::WorkerThread(Network* context)
{
    DWORD bytesTrans = 0;
    ULONG_PTR ckey = COMPLETION_KEY_UNKNOWN;

    LPWSAOVERLAPPED pOver;
    bool done = false;
    while (!done)
    {
        BOOL ret = GetQueuedCompletionStatus(context->ioPort, &bytesTrans, &ckey, (LPOVERLAPPED*)&pOver, INFINITE);
        if (ret == TRUE)
        {
            // Ok
            assert(pOver != NULL);
            if (pOver != NULL)
            {
                Overlapped* overlapped = reinterpret_cast<Overlapped*>(pOver);
                if (ckey == COMPLETION_KEY_IO)
                {
                    std::string s((PCHAR)overlapped->buffer, bytesTrans);
                    std::cout << s.c_str() << std::endl;
                    context->recvSocket.removeOutstanding(overlapped);
                    // Issue another Read to keep this perpetuating
                    context->readSocket(context->recvSocket);
                }
                else if (ckey == COMPLETION_KEY_SHUTDOWN)
                {

                }
            }
        }
        else
        {
            // Problem
            return NULL;
        }
    }
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
    /* Setting AI_PASSIVE will give you a wildcard address if addr is NULL */
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV | AI_PASSIVE;

    if ((ret = GetAddrInfo(NULL, port_buffer, &hints, &res)) != 0)
    {
        //wprintf(stderr, L"getaddrinfo: %s\n", gai_strerror(status));
        status = false;
    }
    else
    {
        /* Note, we're taking the first valid address, there may be more than one */
        memcpy(&s.addr, res->ai_addr, res->ai_addrlen);
        FreeAddrInfo(res);
    }

    return status;
}
