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

int main()
{
    bali::Network net;
    bali::Socket recvSock;

    net.initialize(1, 8967);
    net.createPort();
    net.createWorkerThreads();
    
    net.createSocket(net.getReceiverSock());
    net.bindSocket(net.getReceiverSock());
    net.associate(net.getReceiverSock());
    net.startWorkerThreads();
    int g;
    std::cin >> g;
    net.cleanup();
    
    return 0;
}


///*  RANDOM SCRATCH
//SOCKET RecvSocket;
//sockaddr_in RecvAddr;
//*/
//
//
//
//class Connection
//{
//public:
//    static ULONG nextKey;
//    SOCKET socket;
//    ULONG key;
//};
//
//ULONG Connection::nextKey = 0;
//
//
//class IOCPWorkerThreads;
//class Thread
//{
//public:
//    typedef std::unique_ptr<Thread> Ptr;
//
//    HANDLE h;           // Handle of thread
//    bool isDone;        // Tells the thread to stop
//    uint32_t status;    // This can be read
//    HANDLE port;        // Associated IO Compl Port
//    std::weak_ptr<IOCPWorkerThreads> owner;
//};
//
//class IOCP
//{
//public:
//
//    bool create()
//    {
//        port =
//        CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
//        if (port != NULL)
//        {
//        }
//        else
//        {
//            wprintf(L"CreateIoCompletionPort() failed\n");
//
//        }
//    }
//
//    bool associate(SOCKET s)
//    {
//        Connection conn;
//        if (CreateIoCompletionPort((HANDLE)conn.socket, port, (ULONG_PTR)&Connection::nextKey++, 0) != NULL)
//        {
//            wprintf(L"socket associated with io completion port.\n");
//            connections.push_back(conn);
//        }
//        else
//        {
//            wprintf(L"socket FAILED to associate with io completion port.\n");
//        }
//    }
//
//private:
//    HANDLE port;
//    std::list<Connection> connections;
//    std::list<Thread::Ptr> threads;
//};
//class IOCPWorkerThreads
//{
//public:
//    bool specifyContext(IOCP* context)
//    {
//        this->context = context;
//    }
//
//    bool createWorkerThreads(int nThreads)
//        /*
//        CreateWorkThreads is responsible for starting nThread number of threads.
//        Each thread is created in suspended mode
//        We can specify an arbitrary context to be passed
//        to each thread. All threads get the same context.
//        */
//    {
//        for (int t = 0; t < nThreads; ++t)
//        {
//            Thread::Ptr newThread = std::make_unique<Thread>();
//            
//            newThread->h = CreateThread(NULL, 0,
//                (LPTHREAD_START_ROUTINE)IOCPWorkerThreads::WorkerThread,
//                this->context,
//                CREATE_SUSPENDED,
//                NULL);
//            if (newThread->h != NULL)
//            {
//                wprintf(L"Created Thread.\n");
//                threads.push_back(std::move(newThread));
//            }
//            else
//            {
//                wprintf(L"Failed to Create Thread.\n");
//            }
//        }
//    }
//
//    //LPTHREAD_START_ROUTINE
//    static DWORD WorkerThread(void* context)
//    {
//        DWORD numberOfBytes = 0;
//        BOOL ret = GetQueuedCompletionStatus(context->port, &numberOfBytes, &completionKey, &overlapped, INFINITE);
//
//
//        return 0;
//    }
//    IOCP* context;
//    std::list<Thread::Ptr> threads;
//};
//
//
//
//
//
//
//
//
//int main()
//{
//
//
//    const int nThreads = 1;
//    ULONG completionKey;
//    int iResult = 0;
//
//    WSADATA wsaData;
//
//    SOCKET RecvSocket;
//    sockaddr_in RecvAddr;
//
//    unsigned short Port = 27015;
//
//    char RecvBuf[1024];
//    int BufLen = 1024;
//
//    sockaddr_in SenderAddr;
//    int SenderAddrSize = sizeof(SenderAddr);
//    //-----------------------------------------------
//    // Initialize Winsock
//    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//    if (iResult != NO_ERROR) {
//        wprintf(L"WSAStartup failed with error %d\n", iResult);
//        return 1;
//    }
//
//    /*HANDLE CreateIoCompletionPort(
//        HANDLE FileHandle,
//        HANDLEExistingCompletionPort,
//        ULONG_PTR  CompletionKey,
//        DWORD NumberOfConcurrentThreads
//    );*/
//
//    
//    std::list<Connection> connections;
//
//    HANDLE port = 
//        CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL, 0);
//    if (port != NULL)
//    {
//        wprintf(L"IO completion port Created.\n");
//
//        Connection connection;
//        connection.key = 1;
//        connection.socket =
//            WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP,
//                NULL, 0,
//                WSA_FLAG_OVERLAPPED);
//        if (connection.socket != INVALID_SOCKET)
//        {
//            wprintf(L"socket Created.\n");
//            if (CreateIoCompletionPort((HANDLE)connection.socket, port, (ULONG_PTR)&connection.key, 0) != NULL)
//            {
//                wprintf(L"socket associated with io completion port.\n");
//                connections.push_back(connection);
//            }
//            else
//            {
//                wprintf(L"socket FAILED to associate with io completion port.\n");
//            }
//        }
//        else
//        {
//            wprintf(L"WSASocket() returned an invalid socket.\n");
//        }
//    }
//    else
//    {
//        wprintf(L"CreateIoCompletionPort() returned a NULL handle.\n");
//    }
//
//    //-----------------------------------------------
//    // Clean up and exit.
//    for (auto c = connections.begin(); c != connections.end(); ++c)
//    {
//        wprintf(L".");
//        //CloseHandle((HANDLE)c->socket);
//        closesocket(c->socket);
//    }
//    wprintf(L"*\n");
//    CloseHandle(port);
//
//    wprintf(L"Exiting.\n");
//    WSACleanup();
//    return 0;
//}
//
///*
//#include "stdafx.h"
//#ifndef UNICODE
//#define UNICODE
//#endif
//
//#define WIN32_LEAN_AND_MEAN
//
//#include <winsock2.h>
//#include <Ws2tcpip.h>
//#include <stdio.h>
//
//// Link with ws2_32.lib
//#pragma comment(lib, "Ws2_32.lib")
//
//int main()
//{
//    int iResult = 0;
//
//    WSADATA wsaData;
//
//    SOCKET RecvSocket;
//    sockaddr_in RecvAddr;
//
//    unsigned short Port = 27015;
//
//    char RecvBuf[1024];
//    int BufLen = 1024;
//
//    sockaddr_in SenderAddr;
//    int SenderAddrSize = sizeof(SenderAddr);
//    //-----------------------------------------------
//    // Initialize Winsock
//    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//    if (iResult != NO_ERROR) {
//        wprintf(L"WSAStartup failed with error %d\n", iResult);
//        return 1;
//    }
//
//    //-----------------------------------------------
//    // Create a receiver socket to receive datagrams
//    RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//    if (RecvSocket == INVALID_SOCKET) {
//        wprintf(L"socket failed with error %d\n", WSAGetLastError());
//        return 1;
//    }
//    //-----------------------------------------------
//    // Bind the socket to any address and the specified port.
//    RecvAddr.sin_family = AF_INET;
//    RecvAddr.sin_port = htons(Port);
//    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//    iResult = bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
//    if (iResult != 0) {
//        wprintf(L"bind failed with error %d\n", WSAGetLastError());
//        return 1;
//    }
//    //-----------------------------------------------
//    // Call the recvfrom function to receive datagrams
//    // on the bound socket.
//    wprintf(L"Receiving datagrams...\n");
//    iResult = recvfrom(RecvSocket,
//        RecvBuf, BufLen, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
//    if (iResult == SOCKET_ERROR) {
//        wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());
//    }
//
//    //-----------------------------------------------
//    // Close the socket when finished receiving datagrams
//    wprintf(L"Finished receiving. Closing socket.\n");
//    iResult = closesocket(RecvSocket);
//    if (iResult == SOCKET_ERROR) {
//        wprintf(L"closesocket failed with error %d\n", WSAGetLastError());
//        return 1;
//    }
//
//    //-----------------------------------------------
//    // Clean up and exit.
//    wprintf(L"Exiting.\n");
//    WSACleanup();
//    return 0;
//}
//*/
///*
//
//class Network
//{
//public:
//    enum State
//    {
//
//        UNINITIALIZED,
//        INITIALIZED,
//        FAILED_INITIALIZATION,
//        CREATED_RECEIVER,
//        FAILED_CREATE_RECEIVER,
//        RECEIVER_BOUND,
//        FAILED_RECEIVER_BINDING,
//        FAILED_RECEIVER_CLOSING
//    };
//    Network(unsigned short port)
//    {
//        this->port = port;
//        osResult = 0;
//        initialize();
//    }
//
//    ~Network()
//    {
//        uninitialize();
//    }
//
//    bool initialize()
//    {
//        bool status = true;
//
//        //-----------------------------------------------
//        // Initialize Winsock
//        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//        if (iResult == NO_ERROR) 
//        {
//            state = Network::State::INITIALIZED;
//            //-----------------------------------------------
//            // Create a receiver socket to receive datagrams
//            recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//            if (recvSocket != INVALID_SOCKET) 
//            {
//                state = Network::State::CREATED_RECEIVER;
//                //-----------------------------------------------
//                // Bind the socket to any address and the specified port.
//                recvAddr.sin_family = AF_INET;
//                recvAddr.sin_port = htons(port);
//                recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//                iResult = bind(recvSocket, (SOCKADDR *)& recvAddr, sizeof(recvAddr));
//                if (iResult == 0) 
//                {
//                    state = Network::State::RECEIVER_BOUND;
//
//                }
//                else
//                {
//                    osResult = WSAGetLastError();
//                    state = Network::State::FAILED_RECEIVER_BINDING;
//                    status = false;
//                    wprintf(L"bind failed with error %d\n", osResult);
//                }
//            }
//            else
//            {
//                osResult = WSAGetLastError();
//                state = Network::State::FAILED_CREATE_RECEIVER;
//                status = false;
//                wprintf(L"socket failed with error %d\n", WSAGetLastError());
//            }
//        }
//        else
//        {
//            osResult = iResult;
//            state = Network::State::FAILED_INITIALIZATION;
//            status = false;
//            wprintf(L"WSAStartup failed with error %d\n", iResult);
//        }
//
//        return status;
//    }
//
//    bool uninitialize()
//    {
//        bool status = true;
//
//        //-----------------------------------------------
//        // Close the socket when finished receiving datagrams
//        wprintf(L"Finished receiving. Closing socket.\n");
//        int iResult = closesocket(recvSocket);
//        if (iResult == SOCKET_ERROR) {
//            osResult = WSAGetLastError();
//            state = Network::State::FAILED_RECEIVER_CLOSING;
//            status = false;
//            wprintf(L"closesocket failed with error %d\n", osResult);
//        }
//        else
//        {
//            state = Network::State::UNINITIALIZED;
//        }
//
//
//        WSACleanup();
//        return status;
//    }
//
//
//private:
//    int osResult;
//    State state;
//    WSADATA wsaData;
//    unsigned short port;
//    SOCKET recvSocket;
//    sockaddr_in recvAddr;
//
//};
//
//*/