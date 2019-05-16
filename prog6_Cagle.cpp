/*******************************
Name: Bryan Cagle
Class: CSCE 4230.001
Due Date: 04-21-2016
Description: The purpose of this program
is to add more functionality to the bivariate
function graph

Modified from the Program 5 Assignment
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

#define K 75
#define nt 2*K*K
#define nv (K+1)*(K+1)

static int 	indv=0,
			xrot=-70,
			yrot=0,
			ltri[nt][3],
			width=0,
			height=0;
			static int zoom = 40, //after experimenting, having the zoom start at 40 has the best starting camera for the perspective matrix
			bBoxTog=0,
			wireTog=0,
			shadeTog=0;
static float 	v[nv][3], 
				vn[nv][3],
				scale=1.0,
				scaleShine=50.0,
				mat[]={1,1,1,1},
				lightPos[]={-1,-1,1,0},
				maxHeight,
				minHeight;
				

//finds the smallest Z value in the produced function
float minZ()
{
	float small=v[0][2]; 
    for (int i = 0; i < nv; i++) 
    {
         if(v[i][2]<small) 
        {
            small=v[i][2];
        }
    }
	return small;
}

//finds the largest Z value in the produced function
float maxZ()
{
	float big=v[0][2];
    for (int i = 0; i < nv; i++)
    {
         if(v[i][2]>big)
        {
            big=v[i][2];
        }
    }
	return big;
}

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
			v[indv][2]= (float) scale*.5*exp(-.04*sqrt(pow(80*x-40, 2) + pow(90*y-45, 2))) *
                        cos(0.15*sqrt(pow(80*x-40, 2) + pow(90*y-45, 2)));
			indv++;
		}
	}
	minHeight=minZ();
	maxHeight=maxZ();
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
	if(shadeTog==0)
    glShadeModel(GL_SMOOTH);
	else
	glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST); //depth buffer for hidden surface removal
    BivariateFunction();
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(bBoxTog==1){ //draws the bounding box
	glPushMatrix(); 
	glTranslatef(0.5,0.5,-minHeight/1.75);
	glScalef(1,1,maxHeight-minHeight);
	glutWireCube(1.0);
	glPopMatrix();
	}
    glPushMatrix();
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		if(wireTog==0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		glEnableClientState (GL_VERTEX_ARRAY);
		glEnableClientState (GL_NORMAL_ARRAY);
		glVertexPointer (3, GL_FLOAT, 0, v);
		glNormalPointer(GL_FLOAT, 0, vn);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,scaleShine);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		if(shadeTog==0)
		glDrawElements(GL_TRIANGLES, nt*3, GL_UNSIGNED_INT, ltri); //draws triangles when using smooth shading
		else{
                for (int k = 0; k < nt; k++) { //draw triangles when using flat shading
					glBegin(GL_TRIANGLES);
						glNormal3f(vn[ltri[k][0]][0], vn[ltri[k][0]][1], vn[ltri[k][0]][2]);
						glVertex3f(v[ltri[k][0]][0], v[ltri[k][0]][1], v[ltri[k][0]][2]);
						glNormal3f(vn[ltri[k][1]][0], vn[ltri[k][1]][1], vn[ltri[k][1]][2]);
						glVertex3f(v[ltri[k][1]][0], v[ltri[k][1]][1], v[ltri[k][1]][2]);
						glNormal3f(vn[ltri[k][2]][0], vn[ltri[k][2]][1], vn[ltri[k][2]][2]);
						glVertex3f(v[ltri[k][2]][0], v[ltri[k][2]][1], v[ltri[k][2]][2]);
					glEnd();
				 }
            
		}
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
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

//sets all the changable variables to what the program initializes to, then redraws the graph
void restoreDefaults()
{
			indv=0,
			xrot=-70,
			yrot=0,
			zoom = 40,
			bBoxTog=0,
			scale=1,
			scaleShine=50,
			wireTog=0,
			shadeTog=0;
			init();
			BivariateFunction();
}

//menu actions
void MenuSelect(int idCommand)
{
	switch(idCommand)
	{
	case 'x': //rotates the camera on the X-axis down
		xrot=(xrot+5)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'X': //rotates the camera on the X-axis up
		xrot=(xrot-5)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'y': //rotates the camera on the Y-axis right
		yrot=(yrot+5)%360;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'Y': //rotates the camera on the Y-axis left
		yrot=(yrot-5)%360;
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
	case 'b':
		if(bBoxTog==0)
		bBoxTog=1;
		else
		bBoxTog=0;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'd':
		if(scale!=0.5)
		scale*=0.5;
		BivariateFunction();
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'D':
		if(scale!=2.0)
		scale*=2.0;
		BivariateFunction();
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'E':
		if(scaleShine!=25)
		scaleShine*=0.5;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'e':
		if(scaleShine!=100)
		scaleShine*=2;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'f': //toggle wireframe/polygon-fill mode
		if(wireTog==0)
		wireTog=1;
		else
		wireTog=0;
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 'r': //restores defaults
		restoreDefaults();
		glutPostRedisplay();
		reshape(width,height);
		break;
	case 's':
		if(shadeTog==0)
		shadeTog=1;
		else
		shadeTog=0;
		init();
		BivariateFunction();
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
	glutAddMenuEntry("Toggle Bounding Box (b)", 'b');
	glutAddMenuEntry("Scale function values by 0.5 (d)", 'd');
	glutAddMenuEntry("Scale function values by 2.0 (D)", 'D');
	glutAddMenuEntry("Scale Scale shininess exponent by 0.5 (e)", 'e');
	glutAddMenuEntry("Scale Scale shininess exponent by 2.0 (E)", 'E');
	glutAddMenuEntry("Toggle wireframe/polygon-fill mode (f)", 'f');
	glutAddMenuEntry("Restore defaults for all options (r)", 'r');
	glutAddMenuEntry("Toggle flat/smooth shading (s)", 's');
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
	case 'b':
		MenuSelect('b');
		break;
	case 'd':
		MenuSelect('d');
		break;
	case 'D':
		MenuSelect('D');
		break;
	case 'e':
		MenuSelect('e');
		break;
	case 'E':
		MenuSelect('E');
		break;
	case 'f':
		MenuSelect('f');
		break;
	case 'r':
		MenuSelect('r');
		break;
	case 's':
		MenuSelect('s');
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
    glutCreateWindow("Program 6 - Bivariate Function 2");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    BuildMenu();
	glutAttachMenu(GLUT_RIGHT_BUTTON);
    glutMainLoop();
    return 0;
}