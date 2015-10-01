//
//  v3f.h
//  Particles
//
//  Created by Matthew Dillard on 9/30/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef v3f_h
#define v3f_h

#include <math.h>

class v3f {
public:
    float x,y,z;
    
    v3f(const float xx = 0.0, const float yy = 0.0, const float zz = 0.0) : x(xx), y(yy), z(zz) {}
    
    v3f& operator =(const v3f& a) {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }
    
    v3f operator +(const v3f& a) const {
        return {x+a.x, y+a.y, z+a.z};
    }
    
    v3f operator -(const v3f& a) const {
        return {x-a.x, y-a.y, z-a.z};
    }
    
    v3f operator *(const v3f& a) const {
        return {x*a.x, y*a.y, z*a.z};
    }
    
    v3f operator *(const float& a) const {
        return {x*a, y*a, z*a};
    }
    
    v3f operator /(const float& a) const {
        return {x/a, y/a, z/a};
    }
    
    float abs() const {
        return sqrt(x*x + y*y + z*z);
    }
    
    v3f normalize() const {
        float len = abs();
        return {x/len, y/len, z/len};
    }
    
    float dot(const v3f& a) {
        return x*a.x + y*a.y + z*a.z;
    }
    
    v3f cross(const v3f& a) {
        return {y*a.z - z*a.y, z*a.x - x*a.z, x*a.y - y*a.x};
    }
};

#endif /* v3f_h */
