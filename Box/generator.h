//
//  generator.h
//  Particles
//
//  Created by Matthew Dillard on 9/30/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef generator_h
#define generator_h

#define PI 3.1415926535897932

#include <random>
#include "v3f.h"

class generator {
private:
    std::default_random_engine rand;
    v3f center, normal, w;
    float radius, delta;
    v3f w1, w2, w3;
    v3f n1, n2, n3;
    
    v3f WdotV(const v3f v) {
        return {(w1.x*v.x + w2.x*v.y + w3.x*v.z), (w1.y*v.x + w2.y*v.y + w3.y*v.z), (w1.z*v.x + w2.z*v.y + w3.z*v.z)};
    }
    
    v3f NdotV(const v3f v) {
        return {(n1.x*v.x + n2.x*v.y + n3.x*v.z), (n1.y*v.x + n2.y*v.y + n3.y*v.z), (n1.z*v.x + n2.z*v.y + n3.z*v.z)};
    }
    
public:
    generator(const v3f c, const v3f n, const float r, const v3f w_, const float d)
    : center(c), normal(n), radius(r), w(w_), delta(d) {
        w3 = w.normalize();
        v3f a = {1, 0, 0};
        if (w.y == 0.0 && w.z == 0.0)
            a = {0, 1, 0};
        w1 = (a.cross(w3)).normalize();
        w2 = w3.cross(w1);
        
        n3 = normal.normalize();
        a = {1, 0, 0};
        if (normal.y == 0.0 && normal.z == 0.0)
            a = {0, 1, 0};
        n1 = (a.cross(n3)).normalize();
        n2 = n3.cross(n1);
    }
    
    v3f u_angle_to_disc() {
        std::uniform_real_distribution<float> U01(0.0, 1.0);
        float f = U01(rand);
        float phi = sqrt(f) * delta;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f vP = v3f(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
        
        return WdotV(vP);
    }
    
    v3f g_angle_to_disc() {
        std::normal_distribution<double> G0d(0.0, delta/3);
        float f = G0d(rand);
        float phi = sqrt(f) * delta;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f vP = v3f(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
        
        return WdotV(vP);
    }
    
    v3f u_pos_on_disc() {
        std::uniform_real_distribution<float> U01(0.0, 1.0);
        float f = U01(rand);
        float r = sqrt(f) * radius;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f pP = v3f(r * cos(theta), r * sin(theta), 0);
        
        return NdotV(pP);
    }
    
    v3f g_pos_on_disc() {
        std::normal_distribution<double> G0d(0.0, radius/3);
        float f = G0d(rand);
        float r = sqrt(f) * radius;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f pP = v3f(r * cos(theta), r * sin(theta), 0);
        
        return NdotV(pP);
    }
};

#endif /* generator_h */
