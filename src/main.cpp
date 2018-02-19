#include <iostream>
#include <stdlib.h> //Needed for "exit" function
#include <math.h>
#include <random>
#include <pthread.h>

//Include OpenGL header files, so that we can use OpenGL
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "ball.hpp"
#include "messageQueue.h"

using namespace std;

#define NUMSEGMENTS 30
vector<Ball> Balls; //vector containing numBalls Ball objects
GLfloat Cr=0.8f;//Translation speed of the ball.
GLfloat Cr1=1.0f;

float radius=0.3f;
int numBalls = 10;
int numThreads = 10;
int timingInterval = 10;
int threadCount = 3; //so that timer can know when all threads are done
int prevIndex = numBalls;
float prevGreen = 0.0;
float prevRed = 0.0;
float prevBlue = 0.0;
float xrotate = 0.0;
float yrotate = 0.0;
float zrotate = 0.0;
pthread_mutex_t parentCondMutex = PTHREAD_MUTEX_INITIALIZER; //!< mutex for timer function (parent)
pthread_cond_t parentConditional = PTHREAD_COND_INITIALIZER; //!< conditional for timer function (parent)
vector<pthread_mutex_t> childCondMutex; //!< vector of mutexes for ball threads (children)
vector<pthread_cond_t> childConditional; //!< vector of conditionals for ball threads (children)
vector<bool> startThreadComp; //!< boolean vector for start thread condition
vector< messageQueue<Ball> > messageQueues; //!<  describes position of the button on the screen.
vector<Vector3d<float> > old_speed;
vector<float> old_z_coordinates(numBalls);

class objects
{
    public :
    
    float X[NUMSEGMENTS + 1][NUMSEGMENTS + 1]; //!<  global 2D array of floats for drawing a circle
    float Y[NUMSEGMENTS + 1][NUMSEGMENTS + 1]; //!<  global 2D array of floats for drawing a circle
    
    void operator()()
    {
        cout<<"HIi";
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glEnable(GL_DEPTH_TEST);
        randInitBalls();
    }
    
    void randInitBalls()
    {
        Balls.reserve(numBalls);//reserves memory for the number of balls
        
        //mersenne_twister_engine
        mt19937 ran((random_device())());
        
        //Used to generate the center coordinates of the ball(X,Y)
        //Our box width is 2.0f*2.0f but we will generate the balls between (1.5f-radius)*(1.5f-radius)
        uniform_real_distribution<> xposGen(-1.5f+radius, 1.5f-radius);
        uniform_real_distribution<> yposGen(-1.5f+radius, 1.5f-radius);
        
        uniform_real_distribution<> velGen(-0.15f, 0.15f);
        
        uniform_real_distribution<> colorGen(0.2, 1.0);
        
        for (int i = 0; i < numBalls; i++)
        {
            Ball newBall;
            newBall.initBall(xposGen, yposGen, velGen, colorGen, ran );
            Balls.push_back(newBall);
        }
        for (int i = 0; i <= NUMSEGMENTS; i++)
        {
            // Last vertex same as first vertex
            float angle1 = i *2* M_PI / NUMSEGMENTS;  // 360 deg for all segments
            for (int j = 0; j <= NUMSEGMENTS; j++)
            {
                float angle2 = j *2* M_PI / NUMSEGMENTS;
                X[i][j] = cos(angle1) * sin(angle2);
                Y[i][j] = sin(angle1) * sin(angle2);
            }
        }
    }
    
    void operator()(Ball& ball)
    {
        //cout<<Z[0][0]<<endl;
        glColor3f(ball.red, ball.green, ball.blue);
        for (int i = 0; i < NUMSEGMENTS; i++)
        {
            for (int j = 0; j < NUMSEGMENTS; j++)
            {
                glBegin(GL_POLYGON);
                glNormal3f(X[i][j], Y[i][j], 0.0);
                glVertex3f(ball.centre.x + ball.radius * X[i][j], ball.centre.y + ball.radius * Y[i][j], 0.0f);
                glNormal3f(X[i][j + 1], Y[i][j + 1], 0.0f);
                glVertex3f(ball.centre.x + ball.radius * X[i][j + 1], ball.centre.y + ball.radius * Y[i][j + 1], 0.0f);
                glNormal3f(X[i + 1][j + 1], Y[i + 1][j + 1], 0.0f);
                glVertex3f(ball.centre.x + ball.radius * X[i + 1][j + 1], ball.centre.y + ball.radius * Y[i + 1][j + 1], 0.0f);
                glNormal3f(X[i + 1][j], Y[i + 1][j], 0.0f);
                glVertex3f(ball.centre.x + ball.radius * X[i + 1][j], ball.centre.y + ball.radius * Y[i + 1][j], 0.0f);
                glEnd();
            }
        }
    }
};

objects object;

class Box
{
public:
    float boxWidth=2.0f;
    float boxHeight=2.0f;
};

Box box;

void wallCollision(Ball& ball)
{
    if(ball.centre.x + ball.radius > box.boxWidth)
    {
        ball.centre.x = box.boxWidth - ball.radius;
        ball.velocity.x *= -1;
    }
    if(ball.centre.x - ball.radius < -box.boxWidth)
    {
        ball.centre.x = -box.boxWidth + ball.radius;
        ball.velocity.x *= -1;
    }
    if(ball.centre.y + ball.radius > box.boxHeight)
    {
        ball.centre.y = box.boxHeight - ball.radius;
        ball.velocity.y *= -1;
    }
    if(ball.centre.y - ball.radius < -box.boxHeight)
    {
        ball.centre.y = -box.boxHeight + ball.radius;
        ball.velocity.y *= -1;
    }
}

void updateBalls(Ball &a, Ball& b, float Cr)
{
    Vector3d<float> t(b.centre);
    t.subtract(a.centre);
    Vector3d<float> velocityA(a.velocity);
    Vector3d<float> velocityB(b.velocity);
    if(t.mod() > ((a.radius + b.radius)))
        return;
    t.normalize();
    Vector3d<float> t1(t),t2(t),t3(t),t4(t);
    float u1 = t.dot(velocityA);
    float u2 = t.dot(velocityB);
    float mass1 = a.mass;
    float mass2 = mass1;
    float v1 =( mass2*(u2-u1) + mass1*u1 + mass2*u2)/(mass1 + mass2);
    float v2 =( mass1*(u1 - u2) + mass2*u2 + mass1*u1)/(mass1 + mass2);
    t.scale(u1);
    velocityA.subtract(t);
    t1.scale(v1);
    velocityA.add(t1);
    t2.scale(u2);
    velocityB.subtract(t2);
    t3.scale(v2);
    velocityB.add(t3);
    velocityA.comp(a.velocity);
    velocityB.comp(b.velocity);
    Vector3d<float> fpos(a.centre);
    t4.scale(a.radius + b.radius + 0.000001);
    fpos.add(t4);
    fpos.comp(b.centre);
}

void* ballControl(void* args)
{
    long i = (long) args;
    int threadRatio = numBalls / numThreads;
    int left = numBalls % numThreads;
    int lessBalls = numThreads - left;
    while(true)
    {
        //wait for signal from timer
        pthread_mutex_lock(&childCondMutex[i]);
        while(!startThreadComp[i])
            pthread_cond_wait(&childConditional[i], &childCondMutex[i]);
        pthread_mutex_unlock(&childCondMutex[i]);
        //after signal update ball position and check wall collisions
        int startIndex = i * threadRatio;
        int endIndex = startIndex + threadRatio;
        if(i >= lessBalls)
        {
            startIndex += (i - lessBalls);
            endIndex = startIndex + threadRatio + 1;
        }
        for(int j = startIndex; j < endIndex; j++)
        {
            Balls[j].centre.add(Balls[j].velocity);
            wallCollision(Balls[j]);
        }
        
        //ball collision
        for(int j = startIndex; j < endIndex; j++)
        {
            for(int k = j + 1; k < endIndex; k++)
            {
                updateBalls(Balls[j], Balls[k], Cr);
            }
        }
        for (int j = 0; j < numThreads; j++)
        {
            if(j != i)
            {
                for(int k = startIndex; k < endIndex; k++)
                    messageQueues[j].addItem(Balls[k]);
            }
        }
        int doneSofar = 0;
        while(doneSofar != numBalls - (endIndex - startIndex))
        {
            Ball m = messageQueues[i].remove();
            for(int j = startIndex; j < endIndex; j++)
                updateBalls(m, Balls[j], Cr);
            doneSofar++;
        }
        pthread_mutex_lock(&parentCondMutex);
        threadCount += 1;
        startThreadComp[i] = false;
        pthread_cond_signal(&parentConditional);
        pthread_mutex_unlock(&parentCondMutex);
    }
}

void drawScene() {
    //Clear information from last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity(); //Reset the drawing perspective
    
    glTranslatef(0.0f,0.0f,-6.4f);//move forward 4 units
    
    for(int i=0;i<numBalls;i++)
        object(Balls[i]);
    
    glutSwapBuffers();//Send the 3D scene to the screen
}

void idle()
{
    drawScene();
}

void timer(int timerParam)
{
    
    //signal to threads to start computation
    for (int i = 0; i < numThreads; i++)
    {
        pthread_mutex_lock(&childCondMutex[i]);
        startThreadComp[i] = true;
        pthread_cond_signal(&childConditional[i]);
        pthread_mutex_unlock(&childCondMutex[i]);
    }
    //wait till threads are done
    pthread_mutex_lock(&parentCondMutex);
    while(threadCount < numThreads)
    {
        pthread_cond_wait(&parentConditional, &parentCondMutex);
    }
    pthread_mutex_unlock(&parentCondMutex);
    //put threadCount to zero for next cycle
    threadCount = 0;
    //display
    glutPostRedisplay();
    glutTimerFunc(timingInterval, timer, 0);
}

//Called when the window is resized
void handleResize(int w, int h)
{
    //Tell OpenGL how to convert from coordinates to pixel values
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
    //Set the camera perspective
    glLoadIdentity(); //Reset the camera
    gluPerspective(45.0,                  //The camera angle
                   (double)1000 / (double)1000, //The width-to-height ratio
                   1.0,                   //The near z clipping coordinate
                   200.0);                //The far z clipping coordinate
}

//Called when a key is pressed
void handleKeypress(unsigned char key,int x, int y)//The key that was pressed
{    //The current mouse coordinates
    switch (key) {
        case 27: //Escape key
            exit(0); //Exit the program
    }
}

void initRendering()
{
    //Makes 3D drawing work when something is in front of something else
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv)
{
    int n;
    cout<<"Enter the number of balls:";
    cin>>n;
    numThreads = numBalls=n;
    startThreadComp.resize(numThreads, false);
    //Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 1000); //Set the window size
    //initialisations to barrier, conditionals and mutexes
    vector<pthread_t> ballControlThreads(numThreads);
    childConditional.resize(numThreads);
    childCondMutex.resize(numThreads);
    messageQueues.resize(numThreads);
    for(int i = 0; i < numThreads; i++)
    {
        pthread_mutex_init(&childCondMutex[i], NULL);
        pthread_cond_init(&childConditional[i], NULL);
    }
    for (int i = 0; i < numThreads; i++)
    {
        int m = i;
        pthread_create(&ballControlThreads[i], NULL, ballControl, (void*) m);
    }
    
    //Create the window
    glutCreateWindow("Tabish");
    object();
    initRendering(); //Initialize rendering
    //Set handler functions for drawing, keypresses, and window resizes
    glutDisplayFunc(drawScene);
    glutTimerFunc(timingInterval, timer, 0);
    glutKeyboardFunc(handleKeypress);
    glutIdleFunc(idle);
    glutReshapeFunc(handleResize);
    
    glutMainLoop(); //Start the main loop.  glutMainLoop doesn't return.
    return 0;       //This line is never reached
}


