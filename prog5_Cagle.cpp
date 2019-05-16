/*******************************
Name: Bryan Cagle
Class: CSCE 4230.001
Due Date: 04-07-2016
Description: The purpose of this program
is to plot a polygonal mesh surface using
a bivariate function. As well as apply lighting
effects and camera functions

Algorithm based off pseudo code in
"Polygonal Surfaces" PowerPoint
*******************************/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
# else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <math.h>

#define K 50
#define nt 2*K*K
#define nv (K+1)*(K+1)

static int 	indv=0,
			xrot=-70,
			yrot=0,
			ltri[nt][3],
			width=0,
			height=0;
			static int zoom = 40; //after experimenting, having the zoom start at 50 has the best starting camera for the perspective matrix

static float 	v[nv][3], 
				vn[nv][3];

//stores the verticies at each coordinate using the bivariate function for the z value
void storeVerts()
{
	float h = 1.0/K;
	indv=0;
	for(int j=0; j<=K; j++){
		float y = j*h;
		for(int i=0; i<=K; i++){
			float x = i*h;
			v[indv][0]=x;
			v[indv][1]=y;
			v[indv][2]= .5*exp(-.04*sqrt(pow(80*x-40, 2) + pow(90*y-45, 2))) *
                        cos(0.15*sqrt(pow(80*x-40, 2) + pow(90*y-45, 2)));
			indv++;
		}
	}
}

//stores triangles - triples of vertex indivies
void storeTris()
{
	int indt=0;
	for(int j=1; j<=K; j++){
		for(int i=1; i<=K; i++){
			indv=j*(K+1)+i;
			
			//indexes top right corner of cell
			ltri[indt][0]=indv-K-2;
			ltri[indt][1]=indv-K-1;
			ltri[indt][2]=indv;
			ltri[indt+1][0]=indv-K-2;
			ltri[indt+1][1]=indv;
			ltri[indt+1][2]=indv-1;
			indt+=2;
		}
	}
	
}

//calculates the vertex normal through averaging triangle normals that share vertex
void normalize(float v[])
{
	float length = 0.0;
	for(int i=0;i<3;++i){
		length+=v[i]*v[i];
	}
	length=sqrt(length);
	for(int i=0;i<3;++i){
		v[i] = v[i]/length;
	}
}
void normVerts()
{
	//initizalizes the normals
	for(indv=0;indv<=nv-1;indv++){
		vn[indv][0]=0;
		vn[indv][1]=0;
		vn[indv][2]=0;
	}
	
	//adds the triangle normals to the vertex normals
	for(int indt = 0; indt<=nt-1; indt++){
		int i1=ltri[indt][0], i2=ltri[indt][1], i3=ltri[indt][2];
		float tn[3];
		
		tn[0] = (v[i2][1]-v[i1][1])*(v[i3][2]-v[i1][2]) -
                (v[i2][2]-v[i1][2])*(v[i3][1]-v[i1][1]);
        tn[1] = (v[i2][2]-v[i1][2])*(v[i3][0]-v[i1][0]) -
                (v[i2][0]-v[i1][0])*(v[i3][2]-v[i1][2]);
        tn[2] = (v[i2][0]-v[i1][0])*(v[i3][1]-v[i1][1]) -
                (v[i2][1]-v[i1][1])*(v[i3][0]-v[i1][0]);
        normalize (tn);
        vn[i1][0] += tn[0];
        vn[i2][0] += tn[0];
        vn[i3][0] += tn[0];
		vn[i1][1] += tn[1];
        vn[i2][1] += tn[1];
        vn[i3][1] += tn[1];
		vn[i1][2] += tn[2];
        vn[i2][2] += tn[2];
        vn[i3][2] += tn[2];
	}
	
	//normalizes each vertex normal
	for(indv=0; indv<=nv-1; indv++){
		normalize(vn[indv]);
	}
	
}

//bundles functions up into a single function
void BivariateFunction()
{
	storeVerts();
	storeTris();
	normVerts();
}

//sets the lighting and smooth shading, then calls the bivariate function to create the triangles
void init(void)
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_DEPTH_TEST); //depth buffer for hidden surface removal
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    BivariateFunction();
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
		glEnableClientState (GL_VERTEX_ARRAY);
		glEnableClientState (GL_NORMAL_ARRAY);
		glVertexPointer (3, GL_FLOAT, 0, v);
		glNormalPointer(GL_FLOAT, 0, vn);
		glDrawElements(GL_TRIANGLES, nt*3, GL_UNSIGNED_INT, ltri); //draws triangles
    glPopMatrix();
    glutSwapBuffers();
}

void reshape (int w, int h)
{
	//saves width and height for when menu call requires a reshape
	width=w;
	height=h;
	
	glViewport(0,0,w,h); //keeps the aspect ratio
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(zoom,(GLfloat)w/(GLfloat)h,1,20);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-2.5); //moves back far enough to not get cut off from rendering
	glRotatef(xrot,1,0,0);
	glRotatef(yrot,0,1,0);
	glTranslatef(-0.5,-0.5,0); //centers the model
}

//menu actions
void MenuSelect(int idCommand)
{
	switch(idCommand)
	{
	case 'x': //rotates the camera on the X-axis down
		xrot=(xrot+10)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'X': //rotates the camera on the X-axis up
		xrot=(xrot-10)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'y': //rotates the camera on the Y-axis right
		yrot=(yrot+10)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'Y': //rotates the camera on the Y-axis left
		yrot=(yrot-10)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'z': //zooms in by three unit
		zoom-=3;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'Z': //zooms out by three unit
		zoom+=3;
		glutPostRedisplay();
		reshape(width,height);
		break;	
	case 27: exit(0); break; //ends program
	default: break;
	}
}

//builds the menu 
int BuildMenu(void)
{
	int menu;
	menu=glutCreateMenu(MenuSelect);
	glutAddMenuEntry("Rotate Camera X-Axis + (x)", 'x');
	glutAddMenuEntry("Rotate Camera X-Axis - (X)", 'X');
	glutAddMenuEntry("Rotate Camera Y-Axis + (y)", 'y');
	glutAddMenuEntry("Rotate Camera Y-Axis - (Y)", 'Y');
	glutAddMenuEntry("Zoom In (z)", 'z');
	glutAddMenuEntry("Zoom Out (Z)", 'Z');
	glutAddMenuEntry("Exit Program (ESC)", 27);
}

//redirects keyboard inputs to the menu inputs
void keyboard(unsigned char key, int  x, int y)
{
	switch(key){
	case 'x':
		MenuSelect('x');
		break;
	case 'X':
		MenuSelect('X');
		break;
	case 'y':
		MenuSelect('y');
		break;
	case 'Y':
		MenuSelect('Y');
		break;
	case 'z':
		MenuSelect('z');
		break;
	case 'Z':
		MenuSelect('Z');
		break;
	case 27:
		MenuSelect(27);
      		break;
      	default:
         	break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(550, 550);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Program 5 - Bivariate Function");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    BuildMenu();
	glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}