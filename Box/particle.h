
#ifndef particle_h
#define particle_h

#include "v3f.h"
#include "quad.h"
#include <utility>

#define RHO 0.9
#define MU 0.1

using namespace std;

class particle {
private:
    v3f pos, vel, accel;
    unsigned int blink_on, blink_off;
    float age;
    bool active, caught;
    
public:
    particle(const v3f& p = v3f(), const v3f& v = v3f(), const float t = 0.0, const unsigned int b_on = 1, const unsigned int b_off = 2, const bool a = false)
    : pos(p), vel(v), age(t), blink_on(b_on), blink_off(b_off), active(a), caught(false) { }
    
    particle(const particle& p)
    : pos(p.pos), vel(p.vel), age(p.age), blink_on(p.blink_on), blink_off(p.blink_off), active(p.active) { }
    
    float get_age() const { return age; }
    v3f get_pos() const { return pos; }
    v3f get_vel() const { return vel; }
    v3f get_accel() const { return accel; }
    
    void set_vel(const v3f& v) { vel = v; }
    void set_accel(const v3f& a) { accel = a; }
    void integrate(const float timestep, const std::vector<quad>& planes) {
        v3f new_v = vel + accel * timestep;
        v3f new_p = pos + (vel + new_v)/2 * timestep;
        
        for (auto &plane : planes){
            if (plane.cross_plane(pos, new_p)){
                v3f intersect = plane.find_intersect(pos, new_p);
                if (plane.projection_intersection(intersect)){
                    v3f d = plane.normal * plane.dist_from_plane(new_p);
                    new_p = new_p - (d * (1 + RHO));
                    
                    v3f v_norm = plane.normal * (new_v.dot(plane.normal));
                    v3f v_tan = new_v - v_norm;
                    new_v = (v_norm * -RHO) + (v_tan * (1 - MU));
                }
            }
        }
        
        vel = new_v;
        pos = new_p;
    }
    
    bool isinbounds(const float xl, const float xh, const float zl, const float zh) {
        if (pos.x < xl)
            return false;
        if (pos.x > xh)
            return false;
        if (pos.z < zl)
            return false;
        if (pos.z > zh)
            return false;
        return true;
    }
    
    bool visible(const float t) const {
        if (!active)
            return false;
        return (int)(t - age) % (blink_on + blink_off) < blink_on;
    }
    
    void draw(const float t) {
        if(visible(t)) {
            glBegin(GL_POINTS);
            glVertex3f(pos.x, pos.y, pos.z);
            glEnd();
        }
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
