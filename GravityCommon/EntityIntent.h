#ifndef EntityIntent_H_
#define EntityIntent_H_

namespace bali
{

class EntityIntent
{
public:
    EntityIntent()
    {
        movingright = movingleft = jumping = charging = false;
    }

    bool IsIdle()
    {
        return !(movingright || movingleft || charging);
    }

    bool movingright;
    bool movingleft;
    bool jumping;
    bool charging;
};

}

#endif