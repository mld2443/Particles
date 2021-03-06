
#ifndef generator_h
#define generator_h

#define PI 3.1415926535897932

#include <random>
#include <stdlib.h>
#include "particlelist.h"

class generator {
private:
    std::default_random_engine rand;
    v3f center, normal, w, attract;
    float radius, delta;
    v3f w1, w2, w3;
    v3f n1, n2, n3;
    
    particlelist list;
    bool areatype, vectortype;
    float p_speed;
    unsigned int blink_on, blink_off, pps;
    GLenum light;
    
    v3f WdotV(const v3f v) {
        return {(w1.x*v.x + w2.x*v.y + w3.x*v.z), (w1.y*v.x + w2.y*v.y + w3.y*v.z), (w1.z*v.x + w2.z*v.y + w3.z*v.z)};
    }
    
    v3f NdotV(const v3f v) {
        return {(n1.x*v.x + n2.x*v.y + n3.x*v.z), (n1.y*v.x + n2.y*v.y + n3.y*v.z), (n1.z*v.x + n2.z*v.y + n3.z*v.z)};
    }
    
    v3f Du() {
        std::uniform_real_distribution<float> U01(0.0, 1.0);
        float f = U01(rand);
        float phi = sqrt(f) * delta;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f vP = v3f(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
        
        return WdotV(vP);
    }
    
    v3f Dg() {
        std::normal_distribution<double> G0d(0.0, delta/3);
        float f = G0d(rand);
        float phi = sqrt(f) * delta;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f vP = v3f(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
        
        return WdotV(vP);
    }
    
    v3f Cu() {
        std::uniform_real_distribution<float> U01(0.0, 1.0);
        float f = U01(rand);
        float r = sqrt(f) * radius;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f pP = v3f(r * cos(theta), r * sin(theta), 0);
        
        return NdotV(pP);
    }
    
    v3f Cg() {
        std::normal_distribution<double> G0d(0.0, radius/3);
        float f = G0d(rand);
        float r = sqrt(f) * radius;
        
        std::uniform_real_distribution<float> Upp(-PI,PI);
        float theta = Upp(rand);
        
        v3f pP = v3f(r * cos(theta), r * sin(theta), 0);
        
        return NdotV(pP);
    }
    
    particle gen_particle(const float t){
        std::uniform_real_distribution<float> U01(0.0, 1.0);
        float f = U01(rand);
        
        v3f p, v;
        if (areatype)
            p = Cu();
        else
            p = Cg();
        
        if (vectortype)
            v = Du();
        else
            v = Dg();
        
        p = center + p + v * f * p_speed;
        
        return particle(p, v * p_speed, t, blink_on, blink_off, true);
    }
    
public:
    generator(const v3f c, const v3f n, const float r, const v3f w_, const float d, const float old_age, const float m = 0.01, const bool C = true, const bool D = true)
    : center(c), normal(n), radius(r), w(w_), delta(d), areatype(C), vectortype(D) {
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
        
        list = particlelist(old_age, m, 10000);
    }
    
    void set_part_prop(const float speed, const unsigned int b_on, const unsigned int b_off, const unsigned int ptspersec, const GLenum l, const v3f& attr = v3f()) {
        p_speed = speed;
        blink_on = b_on;
        blink_off = b_off;
        pps = ptspersec;
        light = l;
        attract = attr;
    }
    
    void spawn_particles(const float t, const float elapsed) {
        int number = pps * elapsed;
        if(!number)
            number = 1;
        
        for (int i = 0; i < number; i++)
            list.addparticle(gen_particle(t));
    }
    
    void set_pps(const unsigned int p) {
        pps = p;
    }
    
    void cull_particles(const float t, const float xl, const float xh, const float zl, const float zh) {
        list.cull_particles(t, xl, xh, zl, zh);
    }
    
    void compute_accel(const v3f& g, const v3f& w, const float wc, const float c = 0.0){
        list.compute_accel(g, w, wc, attract, c);
    }
    
    void integrate(const float timestep, const vector<quad>& planes) { list.integrate(timestep, planes); }
    
    void draw_particles(const float t) {
        list.draw_particles(t);
        
        if (list.visible(t)) {
            glEnable(light);
            v3f m = list.get_midpoint(t);
            
            GLfloat lightcolor[] = { 0.15, 0.15, 0.025 };
            GLfloat lightpos[] = { m.x, m.y, m.z };
            glLightfv(light, GL_POSITION, lightpos);
            glLightfv(light, GL_AMBIENT, lightcolor);
            glLightfv(light, GL_DIFFUSE, lightcolor);
            glLightfv(light, GL_SPECULAR, lightcolor);
        }
        else
            glDisable(light);
    }
};

#endif /* generator_h */
