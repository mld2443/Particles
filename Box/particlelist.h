//
//  particlelist.h
//  Particles
//
//  Created by Matthew Dillard on 10/1/15.
//  Copyright © 2015 Matthew Dillard. All rights reserved.
//

#ifndef particlelist_h
#define particlelist_h

#include <vector>
#include <stack>
#include "particle.h"

using namespace std;

class particlelist {
private:
    vector<particle> particles;
    stack<unsigned long> inactivestack;
    unsigned long inactivecount;
    float old_age, p_mass;
    
public:
    particlelist(const float old = 1.0, const float m = 0.01, const unsigned long size = 256): old_age(old), p_mass(m), inactivecount(size) {
        particles.resize(size);
        
        for (unsigned long i = size; i > 0; i--)
            inactivestack.push(i-1);
    }
    
    void addparticle(const particle& p) {
        particles[inactivestack.top()] = p;
        inactivestack.pop();
        inactivecount--;
    }
    
    void cull_particles(const float t) {
        for (unsigned int i = 0; i < particles.size(); i++)
            if (particles[i]) {
                if (particles[i].get_age() > old_age) {
                    particles[i].kill();
                    inactivestack.push(i);
                    inactivecount++;
                }
            }
    }
    
    void compute_accel(const v3f& g, const v3f& w, const float wc) {
        for (auto &p : particles)
            if (p) p.set_accel(g + ((w - p.get_vel()) * (wc/p_mass)));
    }
    
    void integrate(const float t) {
        for (auto &p : particles)
            if (p) p.integrate(t);
    }
};

#endif /* particlelist_h */
