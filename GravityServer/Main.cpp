#include <SFML/Graphics.hpp>
#include <memory>

#include "GameServer.h"
#include "GameServerContext.h"
#include "Network/Network.h"

bali::Network net;
bali::Socket readerSocket;
bali::Socket writerSocket;

void IOHandler(bali::Data & data, bali::Overlapped::IOType ioType, uint64_t id)
{
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);
    if (ioType == bali::Overlapped::IOType::READ)
    {
    }
    else
    {
    }
}

int main()
{
    bali::Network::Result result(bali::Network::ResultType::SUCCESS);
    bali::GameServerContext gsc;
    bali::GameServer game(&gsc);

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

    bali::Game* pGame = &game;
    pGame->initialize();
    while (!pGame->isDone())
    {
        pGame->doProcessing();
    }
    pGame->cleanup();
    net.cleanup();
    return 0;
}

