///////////////////////////////////////////////////////////////////////////////
// Daniel J Ferguson
// 2017
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Utility.h"
#include <list>
#include <sstream>

namespace bali
{
    namespace physics
    {
        //bool IgnoreEdge(vec::VECTOR2 edge, std::vector<vec::VECTOR2> sharedEdges)
        //{
        //    bool ret = false;
        //    for (auto e = sharedEdges.begin(); e != sharedEdges.end(); ++e)
        //    {
        //        if (edge.x == e->x && edge.y == e->y)
        //        {
        //            ret = true;
        //            break;
        //        }
        //    }
        //    return ret;
        //}

        //bool IsBigger(SAT::MTV a, SAT::MTV b)
        //{
        //    if (a.overlap < b.overlap)
        //        return true;
        //    return false;
        //}

        //template <typename T> int sgn(T val) {
        //    return (T(0) < val) - (val < T(0));
        //}

        bool ResolveCollisions(std::vector<SAT::Shape> & shapes, std::vector<SAT::Shape> & playerShapes, Player & player, bali::TileLayer & tileLayer, GameContext* ctx)
        {
            // compare player shape to all other shapes for collision        
            bool isCollided = false;

            float a_deg =180+ player.angle;
            float a = DEG_TO_RAD(a_deg);
            std::list<SAT::MTV> mtvs;

            vec::VECTOR2 original_velocity = player.velocity;
            double original_magnitude = original_velocity.mag();

            //for (auto pshape = playerShapes.begin(); pshape != playerShapes.end(); pshape++)
            //{
            for (auto shape = shapes.begin(); shape != shapes.end(); ++shape)
            {
                SAT::MTV mtv1;
                bool collision = playerShapes.back().collision(*shape, mtv1, tileLayer);
                if (collision)
                {
                    mtvs.push_back(mtv1);
                }
            }
            //}
            vec::VECTOR2  newPos(0,0);
            vec::VECTOR2  newVel(0,0);
            //mtvs.sort(IsBigger);
            for (auto mtv = mtvs.begin();mtv != mtvs.end();++mtv)
            {
                //if (IgnoreEdge(mtv->smallest.edge, ctx->sharedEdges))
                //{
                //    cout << "-ie-";
                //    //continue;
                //}
                SAT::Axis collision_normal = mtv->smallest;

                //// -R =  2*(V dot N)*N - V
                ////  R = -2*(V dot N)*N + V
                vec::VECTOR2 new_velocity = collision_normal*(original_velocity.dot(collision_normal)) * -2 + original_velocity;
                newPos += mtv->smallest;
                newVel += new_velocity;

                std::stringstream ss;
                ss << ">>>";
                ss << "<CN " << collision_normal.x << ", " << collision_normal.y << ">, ";
                ss << "<OV " << original_velocity.x << ", " << original_velocity.y << ">, ";
                ss << "<NV " << new_velocity.x << ", " << new_velocity.y << "> ";
                ss << "<EDGE " << mtv->smallest.edge.x << ", " << mtv->smallest.edge.y << "> ";
                ss << "<A " << player.angle << ">, ";
                ss << "<NP " << newPos.x << ", " << newPos.y << "> ";
                ss << "<NV " << newVel.x << ", " << newVel.y << "> ";
                ss << "<<< " << std::endl;
                cout << ss.str();
            }

            player.position.x += newPos.x * 1.0;
            if (newVel.x != 0)
            {
                player.velocity.x = newVel.norm().x * original_magnitude* 0.20;
                //player.velocity.x = 0;
            }

            player.position.y += newPos.y * 1.0;
            if (newVel.y != 0)
            {
                //player.velocity.y = 0;
                player.velocity.y = newVel.norm().y * original_magnitude * 0.20;
            }
            
            return isCollided;
        }
    }
}
