#include "Game.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

bool foot_contact(float thighAngle, float calfAngle){
    //note for some reason max angle for thighAngle is 10 and calfAngle is 20
    //visually, thighs go to about 60 degrees on each side.
    //calfs to go to about 120 degrees on one side.
    float thighLength = 1.0f;
    float calfLength = 1.0f;

    float thighDegree = abs(thighAngle/10.0f)*60.0f;
    float calfDegree = abs(calfAngle/20.0)*60.f;
    thighDegree = thighDegree/180.0f*M_PI;
    calfDegree = calfDegree/180.0f*M_PI;
    float thigh_adj = thighLength*cosf(thighDegree);
    float calf_adj = calfLength*cosf(calfDegree);
    if(thigh_adj+calf_adj>1.9f){
        return true;
    }
    return false;
}


void Game::update(float time, char playerNum) {
    if(playerNum == '0'){
        body_pos.y += time*(body_velocity.y+time*-4.9f);
        body_pos.x += time*body_velocity.x;
        body_velocity.y += time*-4.9;
        if(body_pos.y<0.03f){
            body_pos.y = 0.0f;
            body_velocity.x = 0.0f;
            body_velocity.y = 0.0f;
        }
        if(foot_contact(thighR_angle, calfR_angle)&&body_pos.y<0.001f){
            body_velocity.x += (calfR_old-calfR_angle)*4.0f;
            body_velocity.y += 2.0f;
        }
        if(foot_contact(thighL_angle, calfL_angle)&&body_pos.y<0.001f){
            body_velocity.x += (calfL_old-calfL_angle)*4.0f;
            body_velocity.y += 2.0f;
        }
        body_velocity.x *= (body_velocity.x<0 ? -1.0f : 1.0f);
    }else{
        body_pos2.y += time*(body_velocity2.y+time*-4.9f);
        body_pos2.x += time*body_velocity2.x;
        body_velocity2.y += time*-4.9;
        if(body_pos2.y<0.03f){
            body_pos2.y = 0.0f;
            body_velocity2.x = 0.0f;
            body_velocity2.y = 0.0f;
        }
        if(foot_contact(thighR_angle2, calfR_angle2)&&body_pos2.y<0.001f){
            body_velocity2.x += (calfR_old2-calfR_angle2)*4.0f;
            body_velocity2.y += 2.0f;
        }
        if(foot_contact(thighL_angle2, calfL_angle2)&&body_pos2.y<0.001f){
            body_velocity2.x += (calfL_old2-calfL_angle2)*4.0f;
            body_velocity2.y += 2.0f;
        }
        body_velocity2.x *= (body_velocity2.x<0 ? -1.0f : 1.0f);
    }

}
