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
        bool ResolveCollisions(std::vector<SAT::Shape> & shapes, std::vector<SAT::Shape> & playerShapes, Player & player, bali::TileLayer & tileLayer, GameContext* ctx)
        {
            // compare player shape to all other shapes for collision
            bool isCollided = false;

            float a_deg =180+ player.angle;
            float a = DEG_TO_RAD(a_deg);
            std::list<SAT::MTV> mtvs;
            std::stringstream ss;

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
                    float dp = original_velocity.dot(mtv1.smallest);
                    if ( dp > 0)
                    {
                        mtv1.smallest *= -1;
                    }
                    mtvs.push_back(mtv1);
                }
            }
            //}
            vec::VECTOR2  newPos(0,0);
            vec::VECTOR2  newVel(0,0);
            //mtvs.sort(IsBigger);
            for (auto mtv = mtvs.begin();mtv != mtvs.end();++mtv)
            {
                if (original_velocity == vec::VECTOR2(0.0, 0.0))
                {
                    int a = 42;
                }
                SAT::Axis collision_normal = mtv->smallest;

                ////  R = -2*(V dot N)*N + V
                vec::VECTOR2 new_velocity = collision_normal*(original_velocity.dot(collision_normal)) * -2 + original_velocity;
                newPos += mtv->smallest;
                newVel += new_velocity;
                
                ss << "<CN " << collision_normal.x << ", " << collision_normal.y << ">, ";
                ss << "<OV " << original_velocity.x << ", " << original_velocity.y << ">, ";
                ss << "<NV " << new_velocity.x << ", " << new_velocity.y << "> ";
                ss << "<EDGE " << mtv->smallest.edge.x << ", " << mtv->smallest.edge.y << "> ";
                ss << "<A " << player.angle << ">, ";
                ss << "<NP " << newPos.x << ", " << newPos.y << "> ";
                ss << "<NV " << newVel.x << ", " << newVel.y << "> ";
                /*ss << "---" << endl;*/
            }

            // Normalize the aggregate new velocity vectors
           // if (newVel.x != 0.0 && newVel.y != 0.0)
                newVel = newVel.norm();

            if (!mtvs.empty())
            {

                player.posCorrection = newPos;
                player.velCorrection = newVel;
                player.position.x += newPos.x * 1.0;
                if (newVel.x != 0)
                {
                    float newVX = (newVel.x * original_magnitude) * 0.60f;
                    player.velocity.x = newVX;
                }

                player.position.y += newPos.y * 1.0f;


                if (newVel.y != 0)
                {
                    float newVY = (newVel.y * original_magnitude) * 0.60;
                    player.velocity.y = newVY;
                }
            }
            //ss << "<<< " << std::endl;
            if (!mtvs.empty())
                ss << "---" << endl;
            cout << ss.str();
            return isCollided;
        }
    }
}
