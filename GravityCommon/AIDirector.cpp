#include "AIDirector.h"
#include "Physics.h"
#include "Builders.h"

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
            entity.behavior.lifestate = Behavior::LifeCycleState::BIRTH;
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


float rotateTowards(sf::Vector2f source, sf::Vector2f target)
{
    float deltarad;
    float deltadeg;


    deltarad = atan2(target.y, target.x) - atan2(source.y, source.x);
    /*if (deltarad < 0) { deltarad += PI; }
    else { deltarad -= PI; }*/
    //return deltarad;

    deltadeg = deltarad * (180.f / PI);
    //if (deltadeg < 0) { deltadeg += 180.0f; }
    //else { deltadeg -= 180.0f; }
    return deltadeg;
}

void
AIDirector::godirection(
    Entity & entity,
    Entity* otherentity,
    sf::Vector2f ngap,
    float dist
)
{


    sf::Vector2f jumpDir = ngap;
    sf::Vector2f moveDir = ngap;

    //// This flips them around if/when they overshoot their mark.
    //if (vec::dot(entity.proto.body.vel, ngap) < 0.0f)
    //{
    //    //entity.proto.body.vel /= 2.f;
    //}

    if (vec::mag(entity.proto.body.vel) > 275)
    {
        // We need to slow this guy down so he doesn't get ejected into orbit.
        CommandQueue::postModifyVelocity(entity.proto.body, entity.proto.body.vel / 2.0f, true);
    }

    if (vec::mag(entity.collider.surfaceNormal) > 0.0)
    {
        sf::Vector2f rv = physics::rightVector(entity.proto.body.angle);
        sf::Vector2f lv = physics::leftVector(entity.proto.body.angle);
        if (vec::dot(ngap, rv) > 0.5f)
        {
            moveDir = vec::normal(entity.collider.surfaceNormal) * -1.0f;// physics::rightVector(entities[ei].proto.body.angle);
            CommandQueue::postMove(entity.proto.body, 1.0f, moveDir, true);
        }
        else if (vec::dot(ngap, lv) > 0.5f)
        {
            moveDir = vec::normal(entity.collider.surfaceNormal) * 1.0f;// physics::leftVector(entities[ei].proto.body.angle);
            CommandQueue::postMove(entity.proto.body, 1.0f, moveDir, true);
        }
        else
        {
            if (dist > 64)
            {
                CommandQueue::postJump(entity.proto.body, 1.0f, ngap);
                float delta = rotateTowards(physics::downVector(entity.proto.body.angle), ngap);
                entity.proto.body.angle += delta;
            }
        }
    }
    else
    {
        if (otherentity != NULL)
        {
            //float delta = rotateTowards(physics::downVector(entity.proto.body.angle), ngap);
            entity.proto.body.angle = otherentity->proto.body.angle;
        }
    }
}

bool
AIDirector::handleHunting(
    Behavior::Disposition disp,
    Vec<Waypoint> & waypoints,
    Player & player,
    Entity & entity
)
{
    if (disp != Behavior::Disposition::HUNTING)
    {
        std::cout << "Changed to HUNTING\n";
        entity.waypointpath.clear();
    }

    sf::Vector2f gap = GetCentroid(player.entity->proto.shapes[0]) - GetCentroid(entity.proto.shapes[0]);// +physics::downVector(entity.proto.body.angle) * entity.proto.shapes[0].getGlobalBounds().height / 2.f);
    sf::Vector2f ngap = vec::norm(gap);
    float mgap= vec::mag(gap);

    godirection(entity, player.entity, ngap, mgap);
    return true;
}

bool
createPath(
    sf::Vector2f p1,
    sf::Vector2f p2,
    Vec<Waypoint> & waypoints,
    Entity & entity
)
{
    GetClosestWaypointResult res1 = GetClosestWaypoint(waypoints, p1);
    GetClosestWaypointResult res2 = GetClosestWaypoint(waypoints, p2);
    std::vector<size_t> wpis = searchastar(waypoints, res1.wpi, res2.wpi);
    entity.waypointpath.clear();
    entity.waypointpath.assign(wpis.begin(), wpis.end());

    entity.seekwaypoint = 1;
    entity.currentwaypoint = 0;
    return true;
}
#define WAYPOINT_ARRIVED_THRESHOLD 64.f
bool
AIDirector::handleSeeking(
    Behavior::Disposition disp,
    Vec<Waypoint> & waypoints,
    Player & player,
    Entity & entity
)
{
    if (disp != Behavior::Disposition::SEEKING)
    {
        std::cout << "Changed to Seeking\n";
        createPath(entity.proto.body.pos, player.entity->proto.body.pos, waypoints, entity);
    }


    size_t wpi = entity.waypointpath[entity.seekwaypoint];
    sf::Vector2f gap = waypoints[wpi].location - entity.proto.body.pos;
    sf::Vector2f ngap = vec::norm(gap);
    float mgap = vec::mag(gap);

    if (mgap < WAYPOINT_ARRIVED_THRESHOLD)
    {
        //entity.currentwaypoint = entity.seekwaypoint;
        //entity.seekwaypoint = (entity.seekwaypoint + 1) % entity.waypointpath.size();

        // Every waypoint, recaculate path
        createPath(entity.proto.body.pos, player.entity->proto.body.pos, waypoints, entity);
    }

    // Since we may have arrived, recalc gaps.
    wpi = entity.waypointpath[entity.seekwaypoint];
    gap = waypoints[wpi].location - GetCentroid(entity.proto.shapes[0]);
    ngap = vec::norm(gap);
    mgap = vec::mag(gap);

    godirection(entity, NULL, ngap, mgap);
    return true;
}



bool
AIDirector::handleWandering(
    Behavior::Disposition disp,
    Vec<Waypoint> & waypoints,
    Entity & entity
)
{
    if (disp != Behavior::Disposition::WANDERING)
    {
        // Only set rail when beginning to wander.
        SetRailAndClickToNearestWaypoint(waypoints, entity);
        std::cout << "Changed to Wandering\n";
    }
    size_t wpi = entity.waypointpath[entity.seekwaypoint];
    sf::Vector2f gap = waypoints[wpi].location - entity.proto.body.pos;
    sf::Vector2f ngap = vec::norm(gap);
    float mgap = vec::mag(gap);

    if (mgap < WAYPOINT_ARRIVED_THRESHOLD)
    {
        entity.currentwaypoint = entity.seekwaypoint;
        entity.seekwaypoint = (entity.seekwaypoint + 1) % entity.waypointpath.size();

        if (entity.seekwaypoint < entity.currentwaypoint)
        {
            if (!waypoints[entity.waypointpath[entity.currentwaypoint]].looped)
            {
                std::reverse(entity.waypointpath.begin(), entity.waypointpath.end());
            }
        }
    }

    // Since we may have arrived, recalc gaps.
    wpi = entity.waypointpath[entity.seekwaypoint];
    gap = waypoints[wpi].location - entity.proto.body.pos;
    ngap = vec::norm(gap);
    mgap = vec::mag(gap);

    godirection(entity, NULL, ngap, mgap);
    return true;
}


#define PLAYER_FAR_THRESHOLD 256
#define PLAYER_NEAR_THRESHOLD 64

bool
AIDirector::handleDispositions(
    Vec<Waypoint> & waypoints,
    Vec<Player> & players,
    Entity & entity
)
{
    Player player;
    Behavior::Disposition orgDisp;
    Behavior::Disposition modDisp;
    modDisp = orgDisp = entity.behavior.disposition;
    
    if (getNearestPlayer(players, entity.proto.body.pos, player))
    {
        sf::Vector2f toplayer = player.entity->proto.body.pos - entity.proto.body.pos;
        float mtoplayer = vec::mag(toplayer);

        if (mtoplayer < PLAYER_NEAR_THRESHOLD)
        {
            modDisp = Behavior::Disposition::HUNTING;
        }
        else if (mtoplayer < PLAYER_FAR_THRESHOLD * (orgDisp == Behavior::Disposition::SEEKING ? 2 : 1))
        {
            modDisp = Behavior::Disposition::SEEKING;
        }
        else
        {
            modDisp = Behavior::Disposition::WANDERING;
        }
    }
    else
    {
        modDisp = Behavior::Disposition::WANDERING;
    }

    switch (modDisp)
    {
    case Behavior::Disposition::WANDERING:
        handleWandering(orgDisp, waypoints, entity);
        break;
    case Behavior::Disposition::SEEKING:
        handleSeeking(orgDisp, waypoints, player, entity);
        break;
    case Behavior::Disposition::HUNTING:
        handleHunting(orgDisp, waypoints, player, entity);
        break;
    }
    entity.behavior.disposition = modDisp;

    return false;
}

bool
AIDirector::handleAlive(
    Vec<Waypoint> & waypoints,
    Vec<Player> & players,
    Entity & entity
)
{
    // Zero Health?
    if (entity.health <= 0) {
        entity.behavior.lifestate = Behavior::LifeCycleState::DYING;
    }
    else
    {
        handleDispositions(waypoints, players, entity);
    }

    return false;
}


bool
AIDirector::handleBirth(
    Vec<Waypoint> & waypoints,
    Entity & entity
)
{
    //
    // Setup behavior
    //
    entity.behavior.lifestate = Behavior::LifeCycleState::ALIVE;
    entity.behavior.disposition = Behavior::Disposition::WANDERING;
    SetRailAndClickToNearestWaypoint(waypoints, entity);
    return true;
}


bool
AIDirector::handleDying(
    Entity & entity
)
{
    entity.behavior.lifestate = Behavior::LifeCycleState::DEAD;
    return false;
}

bool
AIDirector::handleDead(
    Entity & entity
)
{
    // Damn B
    return false;
}



/*
    each entity maintains a list of waypoints(waypointpath) that it must travel.
    The waypointpath list doesn't contain any information about waypoints. It does, however,
    contain indices into the context->waypoints list, where actual waypoint data is stored.


*/
void
AIDirector::update(
    sf::Time elapsed,
    Vec<Player> & players,
    Vec<Entity> & entities,
    Vec<Waypoint> & waypoints
)
{
    const float DETECT_WAYPOINT_RADIUS_PIXEL = 64.f;
    const float DETECT_PLAYER_ENTRY_RADIUS_PIXEL = 512.f;
    const float DETECT_PLAYER_EXIT_RADIUS_PIXEL = 512;

    timeaccumulator += elapsed;

    if (timeaccumulator.asSeconds() > 0)//0.01)
    {
        timeaccumulator -= sf::seconds(0.05);
        for (auto ei : registeredentities)
        {
            Entity & entity = entities[ei];
            switch (entity.behavior.lifestate)
            {
            case Behavior::LifeCycleState::ALIVE:
            {
                handleAlive(waypoints, players, entity);
                break;
            }
            case Behavior::LifeCycleState::BIRTH:
            {
                // Prepare this entity for subservience
                handleBirth(waypoints, entity);
                break;
            }
            case Behavior::LifeCycleState::DYING:
                // 
                handleDying(entity);
                break;
            case Behavior::LifeCycleState::DEAD:
                // 
                handleDead(entity);
                break;
            default:
                break;
            }
        }
    }
}

bool
AIDirector::PopulateEntityPath(
    std::vector<Waypoint> & waypoints,
    Entity & entity,
    uint32_t pathid
)
{
    entity.waypointpath.clear();
    for (size_t wi = 0;
        wi < waypoints.size();
        wi++)
    {
        Waypoint & wp = waypoints[wi];
        if (wp.pathid == pathid)
        {
            entity.pathid = pathid;
            entity.waypointpath.push_back(wi);
        }
    }

    std::sort(entity.waypointpath.begin(),
              entity.waypointpath.end());

    return true;
}


void
AIDirector::SetRail(
    Vec<Waypoint> & waypoints,
    size_t wpi,
    Entity & entity
)
{
     if (PopulateEntityPath(waypoints, entity, waypoints[wpi].pathid))
    {
        entity.seekwaypoint = 0;
        entity.currentwaypoint = 0;

        do {
            entity.currentwaypoint = entity.seekwaypoint;
            entity.seekwaypoint = (entity.seekwaypoint + 1) % entity.waypointpath.size();
        } while (waypoints[entity.waypointpath[entity.currentwaypoint]].id != waypoints[wpi].id);
    }
}

void
AIDirector::SetRailAndClickToNearestWaypoint(
    Vec<Waypoint> & waypoints,
    Entity & entity
)
{
    GetClosestWaypointResult res = GetClosestWaypoint(waypoints, entity.proto.body.pos);

    if (PopulateEntityPath(waypoints, entity, waypoints[res.wpi].pathid))
    {
        entity.seekwaypoint = 0;
        entity.currentwaypoint = 0;

        do {
            entity.currentwaypoint = entity.seekwaypoint;// entity.waypointpath[entity.seekwaypoint];
            entity.seekwaypoint = (entity.seekwaypoint + 1) % entity.waypointpath.size();
        } while (waypoints[entity.waypointpath[entity.currentwaypoint]].id != waypoints[res.wpi].id);
    }
}



sf::Vector2f GetDistance(Player & player, sf::Vector2f curpos)
{
    sf::Vector2f d = GetCentroid(player.entity->proto.shapes[0]) - curpos;
    return d;
}

bool 
AIDirector::getNearestPlayer(Vec<Player> & players, sf::Vector2f mypos, Player & player)
{
    float minmag = 9999999.f;
    sf::Vector2f mind;
    bool found = false;
    for (auto & p : players)
    {
        if (!p.entity->huntable)
            continue;

        sf::Vector2f d = GetCentroid(p.entity->proto.shapes[0]) - mypos;
        float mag = vec::mag(d);
        if (mag < minmag)
        {
            minmag = mag;
            player = p;
            found = true;
        }
    }

    return found;
}


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