#include "TMXReader.h"

int main()
{
    bali::TMX::MapContext mapCtx;
    bali::TMX::TMXReader::load("level1.tmx", mapCtx);

    return 0;
}