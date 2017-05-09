///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#ifndef XMLReader_h_
#define XMLReader_h_

//This is a convienance macro for assigning attributes while parsing XML
#define ASSIGNIFMATCHESSTR(a1, a2)                  \
        if (std::string(attribute[i]) == a1){       \
            a2 = attribute[i+1];                    \
            continue;                               \
        }
//             cout << attribute[i] << " = " << attribute[i+1] << endl;    \

#define ASSIGNIFMATCHESINT(a1, a2)                  \
        if (std::string(attribute[i]) == a1){       \
            a2 = atoi(attribute[i+1]);              \
            continue;                               \
        }
//             cout << attribute[i] << " = " << attribute[i+1] << endl;    \

#define ASSIGNIFMATCHESFLT(a1, a2)                  \
        if (std::string(attribute[i]) == a1){       \
            a2 = (float)atof(attribute[i+1]);       \
            continue;                               \
        }
//             cout << attribute[i] << " = " << attribute[i+1] << endl;    \

#define ASSIGNIFMATCHESBLN(a1, a2)                 \
        if (std::string(attribute[i]) == a1){       \
            a2 = (atoi(attribute[i+1]) > 0 ? true : \
                                             false);\
            continue;                               \
        }
//            cout << attribute[i] << " = " << attribute[i+1] << endl;    \

namespace bali 
{
    class XMLReader
    {
    public:
        XMLReader() {}
        ~XMLReader() {}

        typedef void(*StartElementHandler) (void* data, const char* element, const char** attribute);
        typedef void(*EndElementHandler) (void* data, const char* el);
        typedef void(*CharacterDataHandler)(void *data, const char *content, int length);

    public:
        static bool load(const char *fn, void *ud, StartElementHandler se, EndElementHandler ee, CharacterDataHandler cdh);

    private:

    };//end class LoadXML


}//end namespace bali


#endif
