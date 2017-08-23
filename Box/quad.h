//
//  quad.h
//  Particles
//
//  Created by Matthew Dillard on 10/1/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef quad_h
#define quad_h

#include "v3f.h"

class quad {
private:
    v3f pick_axis_proj() const {
        if (normal.x > normal.y && normal.x > normal.z)
            return v3f(0,1,1);
        else if (normal.y > normal.x && normal.y > normal.z)
            return v3f(1,0,1);
        return v3f(1,1,0);
    }
    
public:
    v3f c1, c2, c3, c4, normal;
    
    quad(const v3f& c11 = v3f(), const v3f& c22 = v3f(), const v3f& c33 = v3f(), const v3f& c44 = v3f())
    : c1(c11), c2(c22), c3(c33), c4(c44), normal(((c3 - c2).cross(c1 - c2)).normalize()) { }
    
    quad(const quad& q): c1(q.c1), c2(q.c2), c3(q.c3), c4(q.c4), normal(q.normal) { }
    
    bool cross_plane(const v3f& x1, const v3f& x2) const {
        float sign1 = (x1 - c1).dot(normal);
        float sign2 = (x2 - c1).dot(normal);
        if (sign1 * sign2 < 0.0)
            return true;
        return false;
    }
    
    v3f find_intersect(const v3f& x1, const v3f& x2) const {
        v3f l = (x2 - x1).normalize();
        float d = ((x1 - c1).dot(normal))/(l.dot(normal));
        return x1 + l * d;
    }
    
    bool projection_intersection(const v3f& x) const {
        v3f proj = pick_axis_proj(), p_i = v3f(1,1,1)-proj;
        v3f v1 = (c2 - c1) * proj, x1 = (x - c1) * proj, cp1 = (v1.cross(x1) * p_i).normalize();
        v3f v2 = (c3 - c2) * proj, x2 = (x - c2) * proj, cp2 = (v2.cross(x2) * p_i).normalize();
        v3f v3 = (c4 - c3) * proj, x3 = (x - c3) * proj, cp3 = (v3.cross(x3) * p_i).normalize();
        v3f v4 = (c1 - c4) * proj, x4 = (x - c4) * proj, cp4 = (v4.cross(x4) * p_i).normalize();
        
        if (cp1 == cp2 && cp2 == cp3 && cp3 == cp4)
            return true;
        return false;
    }
    
    float dist_from_plane(const v3f& x) const {
        return (x - c1).dot(normal);
    }
};

#endif /* quad_h */
