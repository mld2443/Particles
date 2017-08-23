
#ifndef particlelist_h
#define particlelist_h

#include <vector>
#include <stack>
#include "particle.h"

#define MPROD 200000.0

class particlelist {
private:
    vector<particle> particles;
    stack<unsigned long> inactivestack;
    unsigned long inactivecount, size;
    float old_age, p_mass;
    
    void kill(unsigned int i) {
        particles[i].kill();
        inactivestack.push(i);
        inactivecount++;
        size--;
    }
    
public:
    particlelist(const float old = 1.0, const float m = 0.01, const unsigned long s = 256): old_age(old), p_mass(m), inactivecount(s), size(0) {
        particles.resize(s);
        
        for (unsigned long i = s; i > 0; i--)
            inactivestack.push(i-1);
    }
    
    unsigned long get_size() const { return size; }
    
    void addparticle(const particle& p) {
        particles[inactivestack.top()] = p;
        inactivestack.pop();
        inactivecount--;
        size++;
    }
    
    void cull_particles(const float t, const float xl, const float xh, const float zl, const float zh) {
        for (unsigned int i = 0; i < particles.size(); i++)
            if (particles[i]) {
                if (t - particles[i].get_age() > old_age)
                    kill(i);
                else if (!particles[i].isinbounds(xl,xh,zl,zh))
                    kill(i);
            }
    }
    
    void compute_accel(const v3f& g, const v3f& w, const float wc, const v3f& attr, const float coef) {
        for (auto &p : particles) {
            v3f vb = attr - p.get_pos();
            v3f attract = (vb.normalize()) * (MPROD / (vb.abs() * vb.abs())) * coef;
            if (p) p.set_accel(g + ((w - p.get_vel()) * (wc/p_mass)) + attract);
        }
    }
    
    void integrate(const float timestep, const vector<quad>& planes) {
        for (auto &p : particles)
            if (p) p.integrate(timestep, planes);
    }
    
    void draw_particles(const float t) {
        for (auto &p : particles)
            if (p) p.draw(t);
    }
    
    bool visible(const float t) {
        for (auto &p : particles)
            if (p.visible(t))
                return true;
        return false;
    }
    
    v3f get_midpoint(const float t) {
        v3f m;
        for (auto &p : particles) {
            if (p.visible(t))
                m += p.get_pos();
        }
        m = m/size;
        return m;
    }
};

#endif /* particlelist_h */
