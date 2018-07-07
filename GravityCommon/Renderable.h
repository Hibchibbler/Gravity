#ifndef Renderable_h_
#define Renderable_h_

#include "Polygons.h"

namespace bali
{

//
// Renderable Types
//


class Renderable
{
public:
    uint32_t                    eid;
    std::vector<CONVEXSHAPE>    shapes;

};


}

#endif

