#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <math.h>
#include <deque>
#include "generator.h"

using namespace std;

const int steps_per_frame = 10;

v3f gravity, wind;
double windc, cr, mu;    // ball mass, wind coefficient, coefficient of restistuion, coefficient of friction

double boxxl, boxxh, boxyl, boxyh, boxzl, boxzh;  // The low and high x, y, z values for the box sides
double grndhght, ldepth, lwidth, rdepth, rwidth, bankwidth, treex, treez;
float genheight, genradius;

int rotateon;
vector<generator> pgens;

//double xmin, xmax, ymin, ymax, zmin, zmax;
//double maxdiff;
double fps, timestep, elapsed;
GLUquadricObj *quadric;

//unsigned int path_length;

int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

void draw_lights(void) {
    // Set up lights
    //GLfloat light0color[] = { 0.3, 0.1, 0.3 };
    GLfloat light0color[] = { 1, 1, 1 };
    GLfloat light0pos[] = { 500, 500, 0 };
    GLfloat light1color[] = { 0.1, 0.05, 0.1 };
    GLfloat light1pos[] = { 303, 300, 300 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1color);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1color);
}

void draw_box(void) {
    //Draw the box
    GLfloat littlergb[] = { 0.25, 0.25, 0.25 };
    GLfloat box_ambient[] = { 0.1, 0.1, 0.1 };
    GLfloat box_diffuse[] = { 0.7, 0.7, 0.7 };
    GLfloat box_specular[] = { 0.1, 0.1, 0.1 };
    GLfloat box_shininess[] = { 0.5 };
    //set material parameters
    glMaterialfv(GL_FRONT, GL_AMBIENT, box_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, box_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, box_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, box_shininess);
    
    glBegin(GL_LINE_STRIP);
    //bottom face
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxh, boxyl, boxzh);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    //top face
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxh, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    glVertex3f(boxxh, boxyh, boxzh);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    //back left
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyh, boxzl);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    //back right
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    //front right
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyh, boxzh);
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    //front left
    glMaterialfv(GL_FRONT, GL_AMBIENT, littlergb);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glEnd();
}

void draw_ground(void) {
    GLfloat gnd_ambient[] = { 0.1, 0.15, 0.1 };
    GLfloat gnd_diffuse[] = { 0.1, 0.15, 0.1 };
    GLfloat gnd_specular[] = { 0.0, 0.15, 0.0 };
    GLfloat gnd_shininess[] = { 1.0 };
    
    GLfloat water_ambient[] = { 0.1, 0.1, 0.15 };
    GLfloat water_diffuse[] = { 0.0, 0.0, 0.3 };
    GLfloat water_specular[] = { 0.0, 0.0, 0.3 };
    GLfloat water_shininess[] = { 10.0 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, gnd_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gnd_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gnd_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, gnd_shininess);
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(boxxl, grndhght, boxzh);
    glVertex3f(boxxh, grndhght, boxzh);
    
    glVertex3f(boxxl, grndhght, ldepth + lwidth + bankwidth);
    glVertex3f(boxxh, grndhght, rdepth + rwidth + bankwidth);
    
    glVertex3f(boxxl, boxyl, ldepth + lwidth);
    glVertex3f(boxxh, boxyl, rdepth + rwidth);
    glEnd();
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, water_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, water_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, water_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, water_shininess);
    
    glBegin(GL_QUADS);
    glVertex3f(boxxl, boxyl, ldepth + lwidth);
    glVertex3f(boxxh, boxyl, rdepth + rwidth);
    glVertex3f(boxxh, boxyl, rdepth - rwidth);
    glVertex3f(boxxl, boxyl, ldepth - lwidth);
    glEnd();
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, gnd_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gnd_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gnd_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, gnd_shininess);
    
    glBegin(GL_QUAD_STRIP);
    glVertex3f(boxxl, boxyl, ldepth - lwidth);
    glVertex3f(boxxh, boxyl, rdepth - rwidth);
    
    glVertex3f(boxxl, grndhght, ldepth - lwidth - bankwidth);
    glVertex3f(boxxh, grndhght, rdepth - rwidth - bankwidth);
    
    glVertex3f(boxxl, grndhght, boxzl);
    glVertex3f(boxxh, grndhght, boxzl);
    glEnd();
}

void draw_tree(const float x, const float y, const float z) {
    GLfloat tree_ambient[] = { 0.05, 0.05, 0.05 };
    GLfloat tree_diffuse[] = { 0.3, 0.3, 0.3 };
    GLfloat tree_specular[] = { 0.3, 0.3, 0.3 };
    GLfloat tree_shininess[] = { 1.0 };
    
    GLfloat leaf_ambient[] = { 0.05, 0.075, 0.05 };
    GLfloat leaf_diffuse[] = { 0.1, 0.3, 0.1 };
    GLfloat leaf_specular[] = { 0.1, 0.3, 0.1 };
    GLfloat leaf_shininess[] = { 1.0 };
    
    glPushMatrix(); {
        glTranslatef(x, y, z);
        
        glMaterialfv(GL_FRONT, GL_AMBIENT, tree_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, tree_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
        
        glPushMatrix(); {
            glRotatef(-90, 1, 0, 0);
            gluCylinder(quadric, 20, 15, 60, 32, 32);
            
            glTranslatef(0, 0, 60);
            gluCylinder(quadric, 15, 15, 100, 32, 32);
        } glPopMatrix();
    
        glTranslatef(0, 160, 0);
    
        glMaterialfv(GL_FRONT, GL_AMBIENT, leaf_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, leaf_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, leaf_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, leaf_shininess);
        
        for (int i = 0; i < 360; i += 360/6) {
            glPushMatrix(); {
                glRotatef(i, 0, 1, 0);
                glTranslatef(40, 0, 0);
                glutSolidSphere(60, 64, 64);
            } glPopMatrix();
        }
        glTranslatef(0, 40, 0);
        glutSolidSphere(60, 64, 64);
    
    } glPopMatrix();
}

void display(void) {
    GLfloat bigrgb[] = { 1.0, 1.0, 1.0 };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    //rotate the view
    glRotatef(spinup, 1.0, 0.0, 0.0);
    glRotatef(spin, 0.0, 1.0, 0.0);
    
    draw_lights();
    draw_box();
    draw_ground();
    draw_tree(treex, grndhght, treez);
    
    for (auto &g : pgens) {
        g.draw_particles();
    }
        
    glPopMatrix();
    glutSwapBuffers();
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // Enable Z-buffering, backface culling, and lighting
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set eye point and lookat point
    gluLookAt(0, 225, 300, 0, 0, 0, 0, 1, 0);
    quadric = gluNewQuadric();
    
    gravity = { 0.0, 9.81, 0.0 };
    wind = { 10.0, 0.0, 0.0}; windc = 0.1;
    
    // simulation timing
    fps = 60.0;
    elapsed = 0.0;
    timestep = 1.0/(fps * steps_per_frame);
    
    //Initialize scene variables
    boxxl = -100;
    boxxh = 100;
    boxyl = -100;
    boxyh = 100;
    boxzl = -100;
    boxzh = 100;
    grndhght = -90;
    ldepth = -30;
    lwidth = 10;
    rdepth = -0;
    rwidth = 20;
    bankwidth = 10;
    treex = 70;
    treez = 55;
    genheight = 20;
    genradius = 18;
    
    // invoke the generator(s)
    generator g(v3f(boxxl,genheight,ldepth), v3f(1,0,0), genradius, v3f(6,3,1), 0.5, 15);
    g.set_part_prop(15, 3, 2, 100);
    pgens.push_back(g);
}

void reshapeFunc(GLint newWidth, GLint newHeight) {
    if (newWidth > newHeight) // Keep a square viewport
        glViewport((newWidth - newHeight) / 2, 0, newHeight, newHeight);
    else
        glViewport(0, (newHeight - newWidth) / 2, newWidth, newWidth);
    init();
    glutPostRedisplay();
}

void newFrame(const int id) {
    //glutTimerFunc(1000.0/fps, newFrame, 1);
    
    unsigned int step = 0;
    
    while (step < steps_per_frame) {
        double time_remaining = timestep;
        double t_current = time_remaining;
        
        for (auto &g : pgens) {
            g.spawn_particles(elapsed, timestep);
            g.cull_particles(elapsed);
            g.compute_accel(gravity, wind, windc);
        }
        
        step++;
        elapsed += timestep;
    }
    
    glutPostRedisplay();
}

void rotateview(void) {
    if (rotateon) {
        spin += xchange / 250.0;
        if (spin >= 360.0) spin -= 360.0;
        if (spin < 0.0) spin += 360.0;
        spinup -= ychange / 250.0;
        if (spinup > 89.0) spinup = 89.0;
        if (spinup < -89.0) spinup = -89.0;
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                lastx = x;
                lasty = y;
                xchange = 0;
                ychange = 0;
                rotateon = 1;
            }
            else if (state == GLUT_UP) {
                xchange = 0;
                ychange = 0;
                rotateon = 0;
            }
            break;
            
        default:
            break;
    }
}

void motion(int x, int y) {
    xchange = x - lastx;
    ychange = y - lasty;
}

void key(unsigned char c, int x, int y) {
    switch (c) {
        case ' ':
            break;
            
        default:
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Ball in Cube Demo");
    init();
    rotateon = 0;
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);
    //glutTimerFunc(1000.0/fps, newFrame, 0);
    glutIdleFunc(rotateview);
    glutReshapeFunc(reshapeFunc);
    
    glutMainLoop();
    return 0;
}