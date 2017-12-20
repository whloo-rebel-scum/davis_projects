//Wesley Loo
//ECS175, Project 2

//Sources consulted:
//http://paulbourke.net/geometry/rotate/

#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<list>
#include<string>
#include<cmath>

#ifdef WIN32
#include <windows.h>
#endif

#if defined (__APPLE__) || defined(MACOSX)
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#else //linux
#include <GL/gl.h>
#include <GL/glut.h>
#endif

using namespace std;

/**STRUCT PROTOS**/
struct point;
struct edge;
/**WINDOW VAR**/
int grid_width;
int grid_height;
float pixel_size;
int win_height;
int win_width;

/**KEY VARS**/
char obj_sel = '1'; //default is object #1
char func_sel = 't'; //default is translate

/**OTHER VARS**/
string filename;
int num_obj; //number of objects
int *num_point; //number of points for each object
int *num_edge; //number of edges for each object
static list<point> *obj_list; //array of lists for obj points
static list<edge> *edge_list; //array of lists for edges
#define PI atan(1.0)*4
int choice = 1;

/**ROTATION AXIS VARS**/
double xx1 = 0.0;
double xx2 = 1;
double yy1 = 0.5;
double yy2 = 0.5;
double zz1 = 0;
double zz2 = 0;
double a = 2; //angle of rotation in degrees

/**BOUNDING VARS**/
double xmin = 0;
double xmax = 1;
double ymin = 0;
double ymax = 1;
double zmin = -10;
double zmax = 10;

/**FUNCS**/
void init();
void idle();
void display();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void check();
void read_in(string file);
void draw_lines(); //adapted from func from Garrett
void transl(double x, double y, double z);
void axis_rotate(double ang);
void scale(char c);
void axis_edit();
void write_out();
void ortho_proj_sel();
int rebound();

struct point
{
	double x;
	double y;
	double z;
};

struct edge
{
	int from;
	int to;
};

int main(int argc, char **argv)
{
	/**FILE READ-IN**/
	if(argv[1] == NULL)
	{
		cout << "No file specified!" << endl;
		exit(0);
	} //if no argument
	read_in(argv[1]);

	/**TEST PRINTS**/
	//for(int i = 0; i < num_obj; i++)
	//{
	//	for( list<point>::iterator it = obj_list[i].begin(), end = obj_list[i].end(); it != end; ++it)
	//	{
	//		cout << it->x << " " << it->y << " " << it->z << endl;
	//	} //iterating through each list of points
	//	for(list<edge>::iterator it = edge_list[i].begin(), end = edge_list[i].end(); it != end; ++it)
	//	{
	//		cout << it->from << " " << it->to << endl;
	//	} //iterating through list of edges
	//} //testprint of coordinates


	/**WINDOW SETUP**/
		//the number of pixels in the grid
    	grid_width = 150;
    	grid_height = 150;
    	pixel_size = 5;

    	win_height = grid_height*pixel_size;
    	win_width = grid_width*pixel_size;

	/*Set up glut functions*/
    	/** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    	/*initialize variables, allocate memory, create buffers, etc. */
   	//create window of size (win_width x win_height
    	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("whloo_p2");

	glutDisplayFunc(display); //rendering calls here
	glutReshapeFunc(reshape); //update GL on window size change
	//glutMouseFunc(mouse);     //mouse button events
	//glutMotionFunc(motion);   //mouse movement events
	glutKeyboardFunc(key);    //Keyboard events
	glutIdleFunc(idle);       //Function called while program is sitting "idle"

    	//initialize opengl variables
    	init();
	glutMainLoop();

	return 0;

} //main

void read_in(string file)
{
	ifstream inf(file);
	filename = file;
	inf >> num_obj; //read-in number of objects
	obj_list = new list<point>[num_obj]; //initialize list of points
	edge_list = new list<edge>[num_obj]; //initialize list of edges
	num_point = new int[num_obj]; //initialize array of number of points for each obj
	num_edge = new int[num_obj];

	for(int i = 0; i < num_obj; i++)
	{
		inf >> num_point[i];
		for(int j = 0; j < num_point[i]; j++)
		{
			double tempx, tempy, tempz;
			inf >> tempx; inf >> tempy; inf >> tempz;
			//offset for negative coordinates?
			obj_list[i].push_back(point{tempx,tempy,tempz});

		}//for, storing point information
		inf >> num_edge[i];
		for(int j = 0; j < num_edge[i]; j++)
		{
			int tempfrom, tempto;
			inf >> tempfrom; inf >> tempto;
			edge_list[i].push_back(edge{tempfrom,tempto});

		}//for, storing edge information
	}//for, take input from file for each object
	inf.close();
	return;
} //file read-in

void init()
{
    //set clear color (Default background to white)
	glClearColor(1.0,1.0,1.0,1.0);
    //checks for OpenGL errors
	check();
}

void idle()
{
	/*if(rebound() == 1)
	{
		int i = 0;
		for(list<point>::iterator it = obj_list[obj_sel - '0' - 1].begin(); i < num_point[obj_sel-'0'-1]; it++, i++)
		{
			it->x -= 0.5;
			it->y -= 0.5;
			it->z -= 0.5;
		}

	}*/
	glutPostRedisplay();
}

void check()
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		printf("GLERROR: There was an error %s\n",gluErrorString(err) );
		exit(1);
	}
}

void key(unsigned char ch, int x, int y)
{
	switch(ch)
	{
	/**OBJECTION SELECTION**/
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(ch - '0' <= num_obj)
			{
				obj_sel = ch;
				cout << "object #" << ch << " selected" << endl;
			} //if the user chose a valid object
			break;
	/**FUNCTIONS**/
		case 'e':
			if(func_sel == 'r')
			{
				cout << "EDITING ROTATION PARAMETERS" << endl;
				axis_edit();
			}
			break;
		case 'r':
			cout << "ROTATE selected" << endl;
			func_sel = ch;
			break;
		case 't':
			cout << "TRANSLATE selected" << endl;
			func_sel = ch;
			break;
		case 'g':
			cout << "SCALING selected" << endl;
			func_sel = ch;
			break;
		case 'k':
			cout << "Program closed." << endl;
			exit(0);
			break;
		case 'o':
			write_out();
			cout << "SCENE SAVED" << endl;
			break;
		case 'p':
			//ortho projections onto planes
			/*if(func_sel == 'p')
			{
				cout << "Exiting projection, defaulting back to TRANSLATE" << endl;
				func_sel = 't';
				break;
			}*/
			func_sel = ch;
			cout << "PROJECTIONS" << endl;
			ortho_proj_sel();
			break;
	/**DIRECTIONAL CONTROLS**/
		case 'q':
			if(func_sel == 't'){transl(0,0,0.05);}
			break;
		case 'z':
			if(func_sel == 't'){transl(0,0,-0.05);}
			break;
		case 'w':
			if(func_sel == 't'){transl(0,0.05,0);}
			if(func_sel == 'g') scale('w');
			break;
		case 's':
			if(func_sel == 't'){transl(0,-0.05,0);}
			if(func_sel == 'g') scale('s');
			break;
		case 'a':
			if(func_sel == 't'){transl(-0.05,0,0);}
			if(func_sel == 'r') axis_rotate(-a);
			break;
		case 'd':
			if(func_sel == 't'){transl(0.05,0,0);}
			if(func_sel == 'r') axis_rotate(a);
			break;
		default:
            printf("Unknown command \"%c\" \n",ch);
			break;
	}
    	//redraw the scene after keyboard input
	glutPostRedisplay();
}

/*Gets called when display size changes, including initial craetion of the display*/
/*this needs to be fixed so that the aspect ratio of the screen is consistant with the orthographic projection*/

void reshape(int width, int height)
{
	/*set up projection matrix to define the view port*/
    //update the ne window width and height
	win_width = width;
	win_height = height;

    //creates a rendering area across the window
	glViewport(0,0,width,height);

    // uses an orthogonal projection matrix so that
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xmin, xmax, ymin, ymax, zmin, zmax);

    //clear the modelview matrix
    //the ModelView Matrix can be used in this project, to change the view on the projection
    //but you can also leave it alone and deal with changing the projection to a different view
    //for project 2, do not use the modelview matrix to transform the actual geometry, as you won't
    //be able to save the results
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //check for opengl errors
    check();
}

//this is where we render the screen
void display()
{
    //clears the screen
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
	glLoadIdentity();

    draw_lines();

    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
	check();
}

void draw_lines(){
    glLineWidth(0.1); //sets the "width" of each line we are rendering

    //tells opengl to interperate every two calls to glVertex as a line
    glBegin(GL_LINES);

   if(func_sel == 'r')
   { //if rotation function selected, display axis of rotation
  	glColor3f(1.0,0.0,0.0);
	if(choice == 1)
	{ //XY proj
	glVertex3f(xx1, yy1, zz1);
	glVertex3f(xx2, yy2, zz2);
	}
	else if(choice == 2)
	{ //YZ proj
	glVertex3f(yy1, zz1, xx1);
	glVertex3f(yy2, zz2, xx2);
	}
	else if(choice == 3)
	{ //XZ proj
	glVertex3f(xx1, zz1, yy1);
	glVertex3f(xx2, zz2, yy2);
	}
   }
    glColor3f(0,0,1.0);
    for(int i = 0; i < num_obj; i++)
    {//iterating through all objects
	for(list<edge>::iterator it = edge_list[i].begin(), end = edge_list[i].end(); it != end; it++)
	{//iterating through all edges
		point from, to;
		int j = 0; //counter to reach the correct point
		for(list<point>::iterator it2 = obj_list[i].begin(); j < it->from; it2++, j++)
		{ //searches for the correct "from" vertex
			from.x = it2->x;
			from.y = it2->y;
			from.z = it2->z;
		}
		j = 0; //resetting counter
		for(list<point>::iterator it2 = obj_list[i].begin(); j < it->to; it2++, j++)
		{ //searches for the correct "to" vertex
			to.x = it2->x;
			to.y = it2->y;
			to.z = it2->z;
		}
		//connecting on the screen
		if(choice == 1)
		{ //XY proj
			glVertex3f(from.x, from.y, from.z);
			glVertex3f(to.x, to.y, to.z);
		}
		else if(choice == 2)
		{ //YZ proj
			glVertex3f(from.y, from.z, from.x);
			glVertex3f(to.y, to.z, to.x);
		}
		else if(choice == 3)
		{ //XZ proj
			glVertex3f(from.x, from.z, from.y);
			glVertex3f(to.x, to.z, to.y);
		}
	}
    } //for

    glEnd();
}

void transl(double x, double y, double z)
{
	int i = 0;
	for(list<point>::iterator it = obj_list[obj_sel - '0' - 1].begin(); i < num_point[obj_sel-'0'-1]; it++, i++)
	{ //translate all points /**WHY DOES IT INFINITELY LOOP WHEN USING END?**/
		it->x += x;
		it->y += y;
		it->z += z;
	}
	return;
} //3D translate function

void axis_rotate(double ang)
{
	int i = obj_sel - '0' - 1;
	double angle = ang*PI/180.0;
	for(list<point>::iterator it = obj_list[i].begin(), end = obj_list[i].end(); it != end; it++)
	{
	/**1. translate space so that the rotation axis passes through the origin**/
		it->x -= xx1;
		it->y -= yy1;
		it->z -= zz1;
	/**2. rotate space about the x-axis so that the rotation axis lies in the xz plane**/
		//x stays the same
		//calculate unit vector:
		double dx = xx2 - xx1;
		double dy = yy2 - yy1;
		double dz = zz2 - zz1;
		double a = dx/sqrt(dx*dx+dy*dy+dz*dz); //x
		double b = dy/sqrt(dx*dx+dy*dy+dz*dz); //y
		double c = dz/sqrt(dx*dx+dy*dy+dz*dz); //z
		//we now have the unit vector
		double tempx, tempy, tempz;
		double d = sqrt(b*b + c*c);
		if(d != 0)
		{
	       		tempy = (c/d)*it->y + (-b/d)*it->z;
			tempz = (b/d)*it->y + (c/d)*it->z;
			it->y = tempy;
			it->z = tempz;
		} //if d = 0, then rotation axis is already along x-axis

	/**3. rotate space about the y-axis so that the rotation axis lies along the z-axis**/
		tempx = d*it->x + -a*it->z;
		//y is unchanged
		tempz = a*it->x + d*it->z;
		it->x = tempx;
		it->z = tempz;
	/**4. perform the desired rotation by theta about the z-axis**/
		tempx = cos(angle)*it->x + (-sin(angle))*it->y;
		tempy = sin(angle)*it->x + cos(angle)*it->y;
		//z is unchanged
		it->x = tempx;
		it->y = tempy;
	/**apply inverse of step 3**/
		tempx = d*it->x + a*it->z;
		//y is unchanged
		tempz = -a*it->x + d*it->z;
		it->x = tempx;
		it->z = tempz;
	/**apply inverse of step 2**/
		if(d != 0)
		{
			tempy = (c/d)*it->y + (b/d)*it->z;
			tempz = (-b/d)*it->y + (c/d)*it->z;
			it->y = tempy;
			it->z = tempz;
		} //if d = 0, then no need to rotate back
	/**apply inverse of step 1**/
		it->x += xx1;
		it->y += yy1;
		it->z += zz1;
	}
	return;

} //3D rotation function

void axis_edit()
{
	cout << "Current axis points: (" << xx1 << "," << yy1 << "," << zz1 << ") , (" << xx2 << "," << yy2 << "," << zz2 << ")" << endl;
        cout << "Current angle of rotation: " << a << " degrees" << endl;
	cout << "Specify the axis you would like to rotate on, based on two points:" << endl;
	cout << "1st x-coordinate: ";
	cin >> xx1;
	cout << "1st y-coordinate: ";
	cin >> yy1;
	cout << "1st z-coordinate: ";
	cin >> zz1;
	cout << "2nd x-coordinate: ";
	cin >> xx2;
	cout << "2nd y-coordinate: ";
	cin >> yy2;
	cout << "2nd z-coordinate: ";
	cin >> zz2;
	cout << "New angle of rotation (degrees): ";
	cin >> a;
	return;

}

void scale(char c)
{
	int i = obj_sel - '0' - 1;
	double coord[3];
	double temp[3];
	double xtotal = 0;
	double ytotal = 0;
	double ztotal = 0;
	double centerx;
	double centery;
	double centerz;
		for( list<point>::iterator it = obj_list[i].begin(), end = obj_list[i].end(); it != end; ++it)
		{
			xtotal +=it->x;
			ytotal +=it->y;
			ztotal +=it->z;
		} //iterating through each list

		centerx = 1.0/num_point[i] * xtotal;
		centery = 1.0/num_point[i] * ytotal;
		centerz = 1.0/num_point[i] * ztotal;
		for( list<point>::iterator it = obj_list[i].begin(), end = obj_list[i].end(); it != end; ++it)
		{
			/***CONSTRUCTING POINT ARRAY***/
			/**TRANSLATE TO ORIGIN***/
			coord[0] = it->x - centerx;
			coord[1] = it->y - centery;
			coord[2] = it->z - centerz;
			/**SCALE**/
			if( c == 'w')
			{
				temp[0] = coord[0]*1.05;
				temp[1] = coord[1]*1.05;
				temp[2] = coord[2]*1.05;
			} //scale up
			else if( c == 's')
			{
				temp[0] = coord[0]/1.05;
				temp[1] = coord[1]/1.05;
				temp[2] = coord[2]/1.05;
			} //scale down
			/**TRANSLATE BACK**/
			it->x = temp[0] + centerx;
			it->y = temp[1] + centery;
			it->z = temp[2] + centerz;

		}
	return;

} //scaling function

void write_out()
{

	ofstream outf(filename);
	outf << num_obj << endl;
	for(int i = 0; i < num_obj; i++)
	{
		outf << num_point[i] << endl;
		for( list<point>::iterator it = obj_list[i].begin(), end = obj_list[i].end(); it != end; it++)
		{
			outf << it->x << " " << it->y << " " << it->z << endl;
		} //iterating through each list of points
		outf << num_edge[i] << endl;
		for(list<edge>::iterator it = edge_list[i].begin(), end = edge_list[i].end(); it != end; it++)
		{
			outf << it->from << " " << it->to << endl;
		} //iterating through list of edges
	} //testprint of coordinates

	outf.close();

	return;
} //writing back to file

void ortho_proj_sel()
{
	choice = 0;
	do
	{
		cout << "Display ortho projection on the XY plane (1), YZ plane (2), or XZ plane (3)?" << endl;
		cin >> choice;
		if(choice <= 0 || choice > 3) cout << "Invalid plane chosen!" << endl;
	} while(choice <= 0 || choice > 3);

	return;
} //orthogonal projections, selecting which plane to display on

int rebound()
{
	int i = 0;
	for(list<point>::iterator it = obj_list[obj_sel - '0' - 1].begin(); i < num_point[obj_sel-'0'-1]; it++, i++)
	{
		if(it->x > xmax || it->y > ymax || it->z > zmax)
		{
			cout << "RE-BOUNDING VIEWING AREA" << endl;
			glMatrixMode(GL_PROJECTION);
    			glLoadIdentity();
			xmax += 1; ymax += 1; zmax += 1;
   			glOrtho(xmin, xmax, ymin, ymax, zmin, zmax);
			return 1;

		}
	}
	/*glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
   	glOrtho(xmin, xmax, ymin, ymax, zmin, zmax);*/

	return 0;
} //rebound function
