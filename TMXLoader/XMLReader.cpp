#include "XMLReader.h"
#include <expat.h>
#include <fstream>
#include <string.h>
#include <iostream>

namespace bali {


    bool XMLReader::load(const char *fn, void *ud, StartElementHandler se, EndElementHandler ee, CharacterDataHandler cdh) {
        XML_Parser parser;
        char* xmlBuffer;

        parser = XML_ParserCreate(NULL);
        XML_SetUserData(parser, ud);
        XML_SetElementHandler(parser, se, ee);
        XML_SetCharacterDataHandler(parser, cdh);

        std::ifstream fin(fn, std::ios::binary);
        if (!fin.is_open()) {
            std::cout << "Unable to open " << fn << std::endl;
            return false;
        }
        fin.seekg(0, fin.end);
        int length = fin.tellg();
        fin.seekg(0, fin.beg);

        xmlBuffer = new char[length + 1];

        fin.read(xmlBuffer, length);
        fin.close();
        xmlBuffer[length] = '\0';

        if (XML_Parse(parser, xmlBuffer, length, XML_TRUE) == XML_STATUS_ERROR)
        {
            std::cout << "XMLLoader::load(" << fn << ") Error: " << XML_ErrorString(XML_GetErrorCode(parser)) << "@ " << XML_GetCurrentLineNumber(parser) << std::endl;
            XML_ParserFree(parser);
            delete[]xmlBuffer;
            return false;
        }
        XML_ParserFree(parser);
        delete[]xmlBuffer;
        return true;
    }


}//end namespace bali

