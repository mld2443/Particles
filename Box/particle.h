//
//  particle.h
//  Particles
//
//  Created by Matthew Dillard on 10/1/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef particle_h
#define particle_h

#include "v3f.h"
#include <utility>

class particle {
private:
    v3f pos, vel, accel;
    unsigned int blink_on, blink_off;
    float age;
    bool active;
    
public:
    particle(const v3f& p = v3f(), const v3f& v = v3f(), const float t = 0.0, const unsigned int b_on = 0, const unsigned int b_off = 0, const bool a = false)
    : pos(p), vel(v), age(t), blink_on(b_on), blink_off(b_off), active(a) { }
    
    particle(const particle& p)
    : pos(p.pos), vel(p.vel), age(p.age), blink_on(p.blink_on), blink_off(p.blink_off), active(p.active) { }
    
    float get_age() const { return age; }
    v3f get_pos() const { return pos; }
    v3f get_vel() const { return vel; }
    v3f get_accel() const { return accel; }
    
    void set_vel(const v3f& v) { vel = v; }
    void set_accel(const v3f& a) { accel = a; }
    v3f integrate(const float t) {
        v3f nvel = vel + accel * t;
        pos = pos + (vel + nvel)/2 * t;
        vel = nvel;
        return pos;
    }
    
    void kill() { active = false; }
    
    explicit operator bool() const { return active; }
    
    particle& operator =(const particle& p) {
        pos = p.pos;
        vel = p.vel;
        age = p.age;
        blink_on = p.blink_on;
        blink_off = p.blink_off;
        active = p.active;
        return *this;
    }
};

#endif /* particle_h */
