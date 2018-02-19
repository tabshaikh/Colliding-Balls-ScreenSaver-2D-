//
//  ball.cpp
//  dreamonway
//
//  Created by Kunal Parihar on 15/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

//#include "ball.hpp"
//
//  ball.cpp
//  dreamOnGo
//
//  Created by Kunal Parihar on 14/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

#include "ball.hpp"
#include <stdio.h>
#include <iostream>

using namespace std;

Ball::Ball()
{
    red = green = blue = 0.0;
}

Ball::Ball(const Ball &a)
{
    centre =a.centre;
    centre.z = 0;
    velocity = a.velocity;
    velocity.z = 0;
    red = a.red;
    green = a.green;
    blue = a.blue;
    radius=0.3;
    mass = 2;
}

Ball& Ball::operator=(const Ball& a) //assigns all the x,y,z coordinates.
{
    centre = a.centre;
    centre.z = 0;
    velocity = a.velocity;
    velocity.z = 0;
    red = a.red;
    green = a.green;
    blue = a.blue;
    radius=0.3;
    mass = 2;
    return *this;
}

void Ball::initBall(urd& xposGen, urd& yposGen,urd& velGen, urd& colorGen, mt19937& ran)
{   //generate random velocity
    centre.x = xposGen(ran);
    centre.y = yposGen(ran);
    centre.z = 0;
    velocity.x = velGen(ran);
    velocity.y = velGen(ran);
    velocity.z = 0;
    red = colorGen(ran);
    radius=0.3;
    mass = 2;
    green = colorGen(ran);
    blue = colorGen(ran);
}
