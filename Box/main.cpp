#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <deque>
using namespace std;

struct v3d{
    double x,y,z;
    
    v3d& operator =(const v3d& a) {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }
    
    v3d operator +(const v3d& a) {
        return {x+a.x, y+a.y, z+a.z};
    }
    
    v3d operator -(const v3d& a) {
        return {x-a.x, y-a.y, z-a.z};
    }
    
    v3d operator *(const double& a) {
        return {x*a, y*a, z*a};
    }
    
    v3d operator /(const double& a) {
        return {x/a, y/a, z/a};
    }
};

v3d ball, ballv, gravity, wind; //The position of the ball - you can set this in your code
double ballm, windc, cr, mu;    // ball mass, wind coefficient, coefficient of restistuion, coefficient of friction
int radius;

double boxxl, boxxh, boxyl, boxyh, boxzl, boxzh;  // The low and high x, y, z values for the box sides

int rotateon;

double xmin, xmax, ymin, ymax, zmin, zmax;
double maxdiff;
double fps;
int steps_per_frame;

deque<v3d> ball_path;
unsigned int path_length;

int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

void display(void) {
    GLfloat box_ambient[] = { 0.1, 0.1, 0.1 };
    GLfloat smallr00[] = { 0.1, 0.0, 0.0 };
    GLfloat small0g0[] = { 0.0, 0.1, 0.0 };
    GLfloat small00b[] = { 0.0, 0.0, 0.1 };
    GLfloat smallrg0[] = { 0.1, 0.1, 0.0 };
    GLfloat smallr0b[] = { 0.1, 0.0, 0.1 };
    GLfloat small0gb[] = { 0.0, 0.1, 0.1 };
    GLfloat smallrgb[] = { 0.1, 0.1, 0.1 };
    GLfloat bigrgb[] = { 1.0, 1.0, 1.0 };
    
    GLfloat box_diffuse[] = { 0.7, 0.7, 0.7 };
    GLfloat box_specular[] = { 0.1, 0.1, 0.1 };
    GLfloat box_shininess[] = { 1.0 };
    GLfloat ball_ambient[] = { 0.4, 0.0, 0.0 };
    GLfloat ball_diffuse[] = { 0.3, 0.0, 0.0 };
    GLfloat ball_specular[] = { 0.3, 0.3, 0.3 };
    GLfloat ball_shininess[] = { 10.0 };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    //rotate the view
    glRotatef(spinup, 1.0, 0.0, 0.0);
    glRotatef(spin, 0.0, 1.0, 0.0);
    
    /*
     8 vertices:
     glVertex3f(boxxl, boxyl, boxzl);
     glVertex3f(boxxh, boxyl, boxzl);
     glVertex3f(boxxh, boxyh, boxzl);
     glVertex3f(boxxl, boxyh, boxzl);
     glVertex3f(boxxl, boxyl, boxzh);
     glVertex3f(boxxh, boxyl, boxzh);
     glVertex3f(boxxh, boxyh, boxzh);
     glVertex3f(boxxl, boxyh, boxzh);
     */
    
    //Draw the box
    //set material parameters
    glMaterialfv(GL_FRONT, GL_AMBIENT, box_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, box_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, box_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, box_shininess);
    
    glBegin(GL_QUADS);
    //back face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallrgb);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    glVertex3f(boxxl, boxyh, boxzl);
    
    //left face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyh, boxzl);
    glVertex3f(boxxl, boxyh, boxzh);
    
    //right face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small00b);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyh, boxzh);
    glVertex3f(boxxh, boxyh, boxzl);
    glVertex3f(boxxh, boxyl, boxzl);
    
    //bottom face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallrg0);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzh);
    
    //top face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallr0b);
    glVertex3f(boxxh, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    
    //front face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small0gb);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glVertex3f(boxxh, boxyh, boxzh);
    
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glMaterialfv(GL_FRONT, GL_AMBIENT, bigrgb);
    for (deque<v3d>::iterator it = ball_path.begin(); it != ball_path.end(); it++)
        glVertex3f(it->x, it->y, it->z);
    glEnd();
    
    //draw the ball
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ball_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ball_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ball_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ball_shininess);
    glPushMatrix();
    glTranslatef(ball.x, ball.y, ball.z);
    glutSolidSphere(radius, 10, 10);
    glPopMatrix();
    
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
    
    // Set up lights
    GLfloat light0color[] = { 1.0, 1.0, 1.0 };
    GLfloat light0pos[] = { 0, 500, 300 };
    GLfloat light1color[] = { 1.0, 1.0, 1.0 };
    GLfloat light1pos[] = { 300, 300, 300 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1color);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1color);
    
    //Initialize ball position and velocity
    radius = 5;
    ball = { 0.0, 0.0, 0.0};
    ballv = { 120.0, 400.0, 200.0};
    gravity = { 0.0, -196.2, 0.0};
    wind = { -80.0, 0.0, 0.0};
    
    ball_path.clear();
    ball_path.push_back(ball);
    path_length = 5000;
    
    ballm = 3.0;
    windc = 0.2;
    cr = 0.95;
    
    
    fps = 60.0;
    steps_per_frame =  10;
    
    //Initialize box boundaries
    boxxl = -100;
    boxxh = 100;
    boxyl = -100;
    boxyh = 100;
    boxzl = -100;
    boxzh = 100;
}

void reshapeFunc(GLint newWidth, GLint newHeight) {
    if (newWidth > newHeight) // Keep a square viewport
        glViewport((newWidth - newHeight) / 2, 0, newHeight, newHeight);
    else
        glViewport(0, (newHeight - newWidth) / 2, newWidth, newWidth);
    init();
    glutPostRedisplay();
}

enum surface {none, left, right, bottom, top, front, back};

surface collision(v3d newpos, double& lowest_f) {
    surface s = none;
    double current_f;
    lowest_f = 1.0;
    if (newpos.x < (-100.0 + radius)) {
        current_f = (-100.0 + radius - ball.x)/(newpos.x - ball.x);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = left;
        }
    }
    if (newpos.x > (100.0 - radius)) {
        current_f = (100.0 - radius - ball.x)/(newpos.x - ball.x);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = right;
        }
    }
    if (newpos.y < (-100.0 + radius)) {
        current_f = (-100.0 + radius - ball.y)/(newpos.y - ball.y);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = bottom;
        }
    }
    if (newpos.y > (100.0 - radius)) {
        current_f = (100.0 - radius - ball.y)/(newpos.y - ball.y);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = top;
        }
    }
    if (newpos.z < (-100.0 + radius)) {
        current_f = (-100.0 + radius - ball.z)/(newpos.z - ball.z);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = front;
        }
    }
    if (newpos.z > (100.0 - radius)) {
        current_f = (100.0 - radius - ball.z)/(newpos.z - ball.z);
        if (current_f < lowest_f){
            lowest_f = current_f;
            s = back;
        }
    }
    return s;
}

v3d bounce(v3d velocity, surface side) {
    double vx, vy, vz;
    switch (side) {
        case left:
        case right:
            vx = - cr * velocity.x;
            vy = velocity.y;
            vz = velocity.z;
            break;
            
        case bottom:
        case top:
            vx = velocity.x;
            vy = - cr * velocity.y;
            vz = velocity.z;
            break;
            
        case front:
        case back:
            vx = velocity.x;
            vy = velocity.y;
            vz = - cr * velocity.z;
            break;
            
        default:
            break;
    }
    return {vx, vy, vz};
}

void newFrame(int id) {
    glutTimerFunc(1000.0/fps, newFrame, 1);
    
    double timestep = 1.0/(fps * steps_per_frame);
    double elapsed = 0.0;
    unsigned int step = 0;
    
    while (step < steps_per_frame) {
        double time_remaining = timestep;
        double t_current = time_remaining;
        
        while (time_remaining > 0.0) {
            v3d accel = gravity + ((wind - ballv) * (windc/ballm));
            v3d newv = ballv + accel * t_current;
            v3d newpos = ball + (ballv + newv)/2 * t_current;
            
            double f;
            surface side;
            if ((side = collision(newpos, f))) {
                t_current = f*t_current;
                newpos = ball + (ballv + newv)/2 * t_current;
                newv = bounce(newv, side);
            }
            
            time_remaining -= t_current;
            ballv = newv;
            ball = newpos;
        }
        
        step++;
        elapsed += timestep;
    }
    
    ball_path.push_back(ball);
    if (ball_path.size() >= path_length)
        ball_path.pop_front();
    
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

int main(int argc, char** argv) {
    GLint SubMenu1, SubMenu2, SubMenu3, SubMenu4;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ball in Cube Demo");
    init();
    rotateon = 0;
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(1000.0/fps, newFrame, 0);
    glutIdleFunc(rotateview);
    glutReshapeFunc(reshapeFunc);
    
    glutMainLoop();
    return 0;
}