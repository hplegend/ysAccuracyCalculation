#include <windows.h>
#include <GL\glut.h>
#include <sys/time.h>
#include <time.h>
#include "SpaceColonizationHeader.h"
#include "OCtree.h"

float centerx,centery,centerz;

GLfloat g_rotx = 10.0, g_roty = 30.0, g_rotz = 0.0, g_angle=0.0;
GLfloat g_modelPos[3] = {0, 0, -20};


SpaceColonization colonization;
CTree myTree;
OCTree octree;

int Point_Skeleton_flag=1;

char g_optimizationFile[]=".\\model\\OptimizationTreeBranch01.txt";
char g_strFileName[] = ".\\model\\middlebranch.txt";
int g_currLevel = 0;
int g_maxLevel = 0;
bool g_bShowLeaves = false;
double g_thickness = 0.02;

int PointNum;   //这个参数，注意，可能要改掉。全部翻新代码

double precision=0.0;



double InfluenceValue=2.5;
double DeleteValue = 1.0;  //0.3-0.7
double NodeSpanValue=0.2;   //0.08-0.3   Tree limit

/*
double InfluenceValue=1.0;
double DeleteValue = 0.2;  //0.3-0.7
double NodeSpanValue=0.1;   //0.08-0.3  tree 07
*/
/*
double InfluenceValue=1.4;
double DeleteValue = 0.5;  //0.3-0.7
double NodeSpanValue=0.1  ; //0.08-0.3  tree else
*/

FILE *fobj;
int lastIndex;


unsigned int get_msec(void)    //获得的时间为毫秒
{
    static struct timeval timeval, first_timeval;

    gettimeofday(&timeval, 0);

    if(first_timeval.tv_sec == 0)
    {
        first_timeval = timeval;
        return 0;
    }
    return (timeval.tv_sec - first_timeval.tv_sec) * 1000 + (timeval.tv_usec - first_timeval.tv_usec) / 1000;
}






static  void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);  //让图像与窗口大小相等
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60 ,ar, 0.2, 150);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;


}

static  void key(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'w':
            g_rotx += 2;
            break;
        case 's':
            g_rotx -= 2;
            break;
        case 'a':
            g_roty += 2;
            break;
        case 'd':
            g_roty -= 2;
            break;
        case 'q':
            g_modelPos[1] += 1;
            break;
        case 'e':
            g_modelPos[1] -= 1;
            break;
        case 'z':
            g_modelPos[2] += 0.4;
            break;
        case 'x':
            g_modelPos[2] -= 1;
            break;
        case 'o':
            g_rotz+=5;
            break;
        case 'p':
            g_rotz-=5;
            break;
        case '1':
            Point_Skeleton_flag= 1;
            break;
        case '2':
            Point_Skeleton_flag= 2;
            break;
        case'3':
            Point_Skeleton_flag= 3;
            break;
        case'4':
            Point_Skeleton_flag= 4;
            break;
        case'5':
            Point_Skeleton_flag= 5;
            break;
        case'6':
            Point_Skeleton_flag= 6;
            break;
        case'7':
            Point_Skeleton_flag= 7;
            break;
        case' ':
            g_bShowLeaves=!g_bShowLeaves;
            myTree.Update();
            break;
        case ',':
            if(g_currLevel>0)
            {
                g_currLevel--;
                myTree.Update();
                cout << "Current max level:" << g_currLevel << endl;
            }
            break;
        case '.':
            if(g_currLevel<g_maxLevel)
            {
                g_currLevel++;
                myTree.Update();
                cout << "Current max level:" << g_currLevel << endl;
            }
            break;
    }
    glutPostRedisplay();
}


static void idle(void)
{
    glutPostRedisplay();
}



static  void display()
{
    //  cout<<"----dispaly\n";

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    // glTranslatef(centerx,centery,centerz);
    glTranslatef(g_modelPos[0],g_modelPos[1],g_modelPos[2]);
/*
glTranslatef(centerx,0.0f,0.0f);
glTranslatef(0.0f,centery,0.0f);
glTranslatef(0.0f,0.0f,centerz);
*/
    glRotatef(g_rotx, 1.0, 0.0, 0.0);
    glRotatef(g_roty, 0.0, 1.0, 0.0);
    glRotatef(g_rotz, 0.0, 0.0, 1.0);

    if(Point_Skeleton_flag==1)
        colonization.drawPoint();
    if(Point_Skeleton_flag==2)
        colonization.drawSkeleton();
    if(Point_Skeleton_flag==3)
    {
        myTree.DrawTree();
        //  colonization.drawPointLine();

    }
    if(Point_Skeleton_flag==4)
        myTree.DrawColorTree();
    if(Point_Skeleton_flag==5)
    {
        myTree.DrawTree();
        colonization.drawPoint();
    }
    if(Point_Skeleton_flag==6)
    {
        colonization.drawPoint();
        colonization.drawSkeleton();
    }
    if(Point_Skeleton_flag==7)
    {
        colonization.drawPoint();
        octree.CallCubicList();

        //colonization.drawProcess();
    }
    glutSwapBuffers();   //why?不用swapBuffer就出问题了

}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };



int main (int argv , char * argc[])
{

    glutInit(&argv,argc);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowPosition(10,10);
    glutInitWindowSize(1024,768);
    glutCreateWindow("ImplementBy CAU DOC-YangSi");

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(1, 1,1,1);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    //  glEnable(GL_LIGHTING);    //在需要的时候开启光照，而不是设置全局光照

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glShadeModel(GL_SMOOTH);

    colonization.LoadPointCloud();

    glutMainLoop();
    return 0;

}
