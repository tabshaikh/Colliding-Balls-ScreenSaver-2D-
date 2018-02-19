//
//  ball.hpp
//  dreamonway
//
//  Created by Kunal Parihar on 15/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

//#ifndef ball_hpp
//#define ball_hpp

//#include <stdio.h>

//#endif /* ball_hpp */
//
//  ball.hpp
//  dreamOnGo
//
//  Created by Kunal Parihar on 14/02/18.
//  Copyright © 2018 Tabish. All rights reserved.
//

#ifndef ball_hpp
#define ball_hpp

#include "vector.h"
#include "ball.hpp"
#include <random>
using namespace std;

typedef uniform_real_distribution<> urd;

class Ball
{
public:
    Ball();
    Ball(const Ball& a);
    Vector3d<float> centre;
    Vector3d<float> velocity;
    float red;
    float green;
    float blue;
    float radius=0.3;
    float mass = radius * radius * radius;
    
    Ball& operator=(const Ball& a);
    
    void initBall(urd& xposGen, urd& yposGen,urd& velGen,urd& colorGen, mt19937& ran);
};

#endif // BALLS _H

