#include "AIDirector.h"
#include "Physics.h"

namespace bali
{

void
AIDirector::initialize(
    Vec<Entity> & entities
)
{
    // Register entities that have requested it.
    for (auto e = 0; e < entities.size(); e++)
    {
        Entity & entity = entities[e];
        if (entity.registerwithaidirector)
        {
            entity.behavior.state = Behavior::State::BIRTH;
            registeredentities.push_back(e);
        }
    }
}

sf::Vector2f rotatePoint(sf::Vector2f v, sf::Vector2f origin, float angle)
{
    //std::cout << angle << " ";
    angle = angle*(PI / 180.0f);

    v -= origin;
    v.x = v.x * cos(angle) - v.y * sin(angle);
    v.y = v.x * sin(angle) + v.y * cos(angle);
    v += origin;
    return v;
}

void
AIDirector::update(
    sf::Time elapsed,
    Vec<Entity> & entities,
    Vec<Waypoint> & waypoints
)
{
    timeaccumulator += elapsed;

    if (timeaccumulator.asSeconds() > 0.05)
    {
        timeaccumulator -= sf::seconds(0.05);

        for (auto ei : registeredentities)
        {
            entities[ei].update(elapsed);

            switch (entities[ei].behavior.state)
            {
            case Behavior::State::BIRTH: {
                //
                // Setup behavior
                //

                uint32_t jk = 0;
                for (auto & w : waypoints)
                {
                    entities[ei].waypointpath.push_back(jk);
                    jk++;
                }
                /*int g = rand() % 2;
                if (g == 0)
                {*/
                    //entities[ei].waypointpath.push_back(15);
                    //entities[ei].waypointpath.push_back(14);
                    //entities[ei].waypointpath.push_back(13);
                    //entities[ei].waypointpath.push_back(12);
                    //entities[ei].waypointpath.push_back(11);
                    //entities[ei].waypointpath.push_back(10);
                    //entities[ei].waypointpath.push_back(9);
                    //entities[ei].waypointpath.push_back(8);
                    //entities[ei].waypointpath.push_back(7);
                    /*entities[ei].waypointpath.push_back(6);
                    entities[ei].waypointpath.push_back(5);
                    entities[ei].waypointpath.push_back(4);
                    entities[ei].waypointpath.push_back(3);
                    entities[ei].waypointpath.push_back(2);
                    entities[ei].waypointpath.push_back(1);
                    entities[ei].waypointpath.push_back(0);*/
                /*}
                else
                {
                    entities[ei].waypointpath.push_back(7);
                    entities[ei].waypointpath.push_back(6);
                    entities[ei].waypointpath.push_back(5);
                    entities[ei].waypointpath.push_back(4);
                }*/
                entities[ei].seekwaypoint = 1;
                entities[ei].currentwaypoint = 0;

                entities[ei].behavior.state = Behavior::State::IDLE;
                entities[ei].behavior.disposition = Behavior::Disposition::AGGRESSIVE;

                break;
            }
            case Behavior::State::DEATH:
                break;
            case Behavior::State::IDLE:
            {
                // Continue seeking waypoint
                size_t wpi = entities[ei].waypointpath[entities[ei].seekwaypoint];
                sf::Vector2f gap = waypoints[wpi].location - entities[ei].proto.body.pos;
                float dist = vec::mag(gap);
                if (dist < 128.0f)
                {
                    // We found the waypoint
                    entities[ei].currentwaypoint = entities[ei].waypointpath[entities[ei].seekwaypoint];
                    entities[ei].seekwaypoint = (entities[ei].seekwaypoint + 1) % entities[ei].waypointpath.size();
                    //std::cout << "[" << ei << "] Found " << entities[ei].currentwaypoint << std::endl;
                    //std::cout << "[" << ei << "] Seeking " << entities[ei].seekwaypoint << std::endl;
                }
                else if (dist > 0)
                {
                    float str = 0.20f;
                    sf::Vector2f ngap = vec::norm(gap);
                    sf::Vector2f track = waypoints[entities[ei].waypointpath[entities[ei].seekwaypoint]].location - 
                                         waypoints[entities[ei].waypointpath[entities[ei].currentwaypoint]].location;

                    sf::Vector2f jumpDir;
                    sf::Vector2f moveDir;

                    Waypoint & swp = waypoints[entities[ei].seekwaypoint];

                    // This flips them around if/when they overshoot their mark.
                    if (vec::dot(entities[ei].proto.body.vel, ngap) < -0.1f)
                    {
                        entities[ei].proto.body.vel = vec::Zero();
                    }

                    if (vec::dot(ngap, physics::upVector(entities[ei].proto.body.angle)) > 0.10f)
                    {
                        //if (entities[ei].collider.jumpNormal != vec::Zero())
                        {
                            str = 1.0f;
                            jumpDir = ngap;
                            CommandQueue::postJump(entities[ei].proto.body, str, jumpDir);
                            entities[ei].collider.jumpNormal = vec::Zero();
                        }
                    }
                    else
                    {
                        str = 0.4f;
                        //if (vec::dot(ngap, physics::rightVector(entities[ei].proto.body.angle)) > 0.0f)
                        //{
                        //    moveDir = vec::normal(entities[ei].collider.surfaceNormal);// physics::rightVector(entities[ei].proto.body.angle);
                        //}
                        //else
                        //{
                        //    moveDir = vec::normal(entities[ei].collider.surfaceNormal) * -1.0f;// physics::leftVector(entities[ei].proto.body.angle);
                        //}
                        moveDir = ngap;// physics::rightVector(entities[ei].proto.body.angle) + physics::upVector(entities[ei].proto.body.angle) / 4.0f;//vec::normal(entities[ei].collider.surfaceNormal) * -1.0f;//
                        assert(moveDir != vec::Zero());
                        CommandQueue::postMove(entities[ei].proto.body, str, moveDir, true);

                    }
                }
                break;
            }case Behavior::State::WALKING:
                break;
            case Behavior::State::RUNNING:
                break;
            case Behavior::State::JUMPING:
                break;
            case Behavior::State::FALLING:
                break;
            case Behavior::State::ATTACKING:
                break;
            default:
                break;
            }

        }
    }
}

void AIDirector::SeekEntity(Entity & from, Entity & to) {}
void AIDirector::SeekWaypoint(Entity & from, Waypoint & to) {}
void AIDirector::FoundWaypoint(Entity & entity) {}
void AIDirector::FoundEntity(Entity & entity) {}

}

/*
//entities[ei].upper = vec::norm(ngap + physics::upVector(entities[ei].proto.body.angle)/ 0.7f);
//entities[ei].ahead = ngap;
//entities[ei].lower = vec::norm(ngap + physics::downVector(entities[ei].proto.body.angle) / 0.7f);

//Intersection upperinter;
//Intersection aheadinter;
//Intersection lowerinter;

//entities[ei].ahead = ngap;// entities[ei].proto.body.vel;//entities[ei].proto.body.pos
//entities[ei].upper = rotatePoint(entities[ei].ahead, vec::Zero(), entities[ei].proto.body.angle + 45);
//entities[ei].lower = rotatePoint(entities[ei].ahead, vec::Zero(), entities[ei].proto.body.angle - 45);
//physics::RayCast(entities[ei].upper,
//    entities[ei].proto.body.pos,
//    entities[ei].visiblesegments,
//    upperinter);
//entities[ei].distupper = upperinter.distance;

//physics::RayCast(entities[ei].ahead,
//    entities[ei].proto.body.pos,
//    entities[ei].visiblesegments,
//    aheadinter);
//entities[ei].distahead = aheadinter.distance;

//physics::RayCast(entities[ei].lower,
//    entities[ei].proto.body.pos,
//    entities[ei].visiblesegments,
//    lowerinter);
//entities[ei].distlower = lowerinter.distance;

////if (!aheadinter.expired)
////{
////    float magah = vec::mag(aheadinter.rayPoint);
////    if (magah < 2.0f)
////    {
////        float magup=0;
////        float maglo=0;

////        if (upperinter.expired && lowerinter.expired)
////            moveDir = entities[ei].upper;
////        else if (upperinter.expired && !lowerinter.expired)
////            moveDir = entities[ei].upper;
////        else  if (!upperinter.expired && lowerinter.expired)
////            moveDir = entities[ei].lower;

////        //if (magup < maglo)
////        //{
////        //    moveDir = entities[ei].upper;//physics::upVector(entities[ei].proto.body.angle);
////        //    std::cout << "u ";
////        //}
////        //else
////        //{
////        //    moveDir = entities[ei].lower;//physics::downVector(entities[ei].proto.body.angle);
////        //    std::cout << "d ";
////        //}
////    }
////}

*/