//Wesley Loo
//ECS175, Project 4

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
struct dpoint;
/**WINDOW VAR**/
int grid_width;
int grid_height;
float pixel_size;
int win_height;
int win_width;


/**OTHER VARS**/
list<int> num_point; //a list of the number of points for each curve
list<list<point>> curve_list; //list of lists of control points on curve
int num_curves;
char func_sel;
int curve_sel = 1; //1 is bezier, 2 is b-spline
list< list<dpoint> > master_curve_list; //list of lists of points on curve
int korder = 3;
list< list<dpoint> > knots; //knots list, where x is the knot label, and y is the knot value

/**POINT Selection VARS**/
int selp_cl; //curve_list position
int selp_pl; //point_list position

/**FUNC**/
void init();
void idle();
void display();
void reshape(int width, int height);
void key(unsigned char ch, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void check();
void read_in();
void write_out();
void draw_lines();
void draw_points();
void mark_point();
void delete_point();
bool point_select(int x, int y);
void move_point(int x, int y);
void new_curve(int x, int y);
void new_p_before(int x, int y);
void new_p_after(int x, int y);
void draw_curve();
void bezier();
void bspline();
void bspline_setup();
void bspline_edit();
void knot_display();

struct point
{
	int x;
	int y;
};

struct dpoint
{
	double x;
	double y;
};

int main(int argc, char** argv) //no command line argument needed, always opens points.txt if it exists
{

	/**FILE READ-IN**/
	read_in();

	/**WINDOW SETUP**/
	//the number of pixels in the grid
    	grid_width = 200;
    	grid_height = 200;
    	pixel_size = 3;

    	win_height = grid_height*pixel_size;
    	win_width = grid_width*pixel_size;

	/*Set up glut functions*/
    	/** See https://www.opengl.org/resources/libraries/glut/spec3/spec3.html ***/
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    	/*initialize variables, allocate memory, create buffers, etc. */
   	//create window of size (win_width x win_height
    	glutInitWindowSize(win_width,win_height);
	glutCreateWindow("whloo_p4");

	glutDisplayFunc(display); //rendering calls here
	glutReshapeFunc(reshape); //update GL on window size change
	glutMouseFunc(mouse);     //mouse button events
	glutMotionFunc(motion);   //mouse movement events
	glutKeyboardFunc(key);    //Keyboard events
	glutIdleFunc(idle);       //Function called while program is sitting "idle"

    	//initialize opengl variables
    	init();
	glutMainLoop();

	return 0;

} //main

void read_in()
{

	if(ifstream("points.txt"))
	{

		cout << "File exists, proceed to read contents" << endl;
	}
	else
	{
		/**initalize variables and lists to empty**/

		return;
	}

	/**READ POINTS.TXT**/
	ifstream inf("points.txt");
	int points;
	int tempx;
	int tempy;
	inf >> num_curves; //read in the number of curves

	/**initalize variables and lists based on num_curves**/

	for(int i = 0; i < num_curves; i++)
	{ //for each curve
		//read in the number of points for a curve
		inf >> points;
		num_point.push_back(points); //record the number of points on the curve
		list<point> l;
		for(int j = 0; j < points; j++)
		{//for all points on one curve
			inf >> tempx; inf >> tempy;
			l.push_back(point{tempx, tempy});
		}
		curve_list.push_back(l); //push the completed list of points onto the bigger list
	}

	inf.close();


	/**TEST, accessing list of lists**/

	cout << num_curves << endl;
	list<int>::iterator p = num_point.begin();
	for(int i = 0; i < num_curves; i++)
	{
		/**print the number of points in the curve**/
		cout << *p << endl;
		p++;

		/**print the points**/
		list< list<point> >::iterator itr;
		int a = 0;
	       	for(itr = curve_list.begin(); itr != curve_list.end() && a < i; itr++, a++)
		{//navigate to correct list of points

		}
		list<point> lp = *itr;
		list<point>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++)
		{//print out all point coordinates
			cout << (*it).x << " " << (*it).y << endl;
		}


	}

	return;
}//read-in points.txt if it exists

void init()
{
    //set clear color (Default background to white)
	glClearColor(1.0,1.0,1.0,1.0);
    //checks for OpenGL errors
	check();
}

void idle()
{
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
		case 's':
			if(curve_sel == 2)
				knot_display();
			break;
		case 'b':
			if(curve_sel == 1)
			{
				curve_sel = 2;
				cout << "B-SPLINE CURVE" << endl;
				bspline_setup();
			}
			else
			{
				curve_sel = 1;
				cout << "BEZIER CURVE" << endl;
			}
			break;
		case 'e':
			if(curve_sel == 2)
			{
				bspline_edit();
			}
			break;
		case 'm':
			func_sel = 'm';
			cout << "MODIFY MODE ENABLED" << endl;
			break;
		case 'd':
			if(func_sel == 'm' || func_sel == 'i')
				delete_point();
			if(curve_sel == 2)
				bspline_setup();
			break;
		case 'i':
			func_sel = 'i';
			cout << "POINT INSERTION MODE" << endl;
			break;
		case 'n':
			func_sel = 'n';
			cout << "NEW CURVE MODE" << endl;
			break;
		case 'k':
			cout << "Program closed." << endl;
			exit(0);
			break;
		case 'o':
			write_out();
			cout << "SCENE SAVED" << endl;
			break;
		default:
			break;
	}

	//redraw the scene after keyboard input
	glutPostRedisplay();
}

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
    glOrtho(0,grid_width,0,grid_height,-10,10);

    //clear the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //check for opengl errors
    check();

}

void display()
{
    //clears the screen
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //clears the opengl Modelview transformation matrix
	glLoadIdentity();

    draw_lines();
    draw_points();

    if(func_sel == 'm' || func_sel == 'i')
    { //mark selected point
	mark_point();
    }

    if(curve_sel == 1)
    { //bezier
	master_curve_list.clear();
	bezier();
	draw_curve();
    }
    else
    { //b-spline
	master_curve_list.clear();
	//bspline_setup();
	bspline();
	draw_curve();
    }

    //blits the current opengl framebuffer on the screen
    glutSwapBuffers();
    //checks for opengl errors
	check();
}

void bezier()
{
	if(curve_list.empty())
		return;

	glPointSize(4);
	glBegin(GL_POINTS);
	glColor3f(0, 0.5, 0);
	list<int>::iterator p = num_point.begin();
	list< list<point> >::iterator itr = curve_list.begin();
	list<point> lp = *itr;
	list<point>::iterator it = lp.begin();
	double t;
	list<dpoint> temp1; //= *itr; //used for calculation of next generation
	list<dpoint> temp2; //all calculations intially stored in here, before being moved to temp1
	list<dpoint>::iterator it1 = temp1.begin();
	double tempx;
	double tempy;
	list<dpoint> master_curve_subset;
	for(int i = 0; i < num_curves; i++)
	{
		lp = *itr;
		for(t = 0; t < 1; t += 0.01)
		{//over the whole interval
			it = lp.begin();
			for(int z = 0; z < *p; z++)
			{
				temp1.push_back(dpoint{(double)(*it).x, (double)(*it).y});
				it++;
			}
			for(int j = 1; j < *p; j++)
			{//for generations
				it1 = temp1.begin();
				for(int i = 0; i < *p - j; i++)
				{
					//calculate x and y
					tempx = (1.0-t) * (double)(*it1).x;
					tempy = (1.0-t) * (double)(*it1).y;
				      	it1++;
					tempx += t * (double)(*it1).x;
					tempy += t * (double)(*it1).y;
					temp2.push_back(dpoint{tempx, tempy});
				}
				//copy and replace lists
				temp1 = temp2;
				temp2.clear();
			}
			//store in master list of final curve points?
			it1 = temp1.begin();
			//glVertex2f((*it1).x, (*it1).y);
			master_curve_subset.push_back({(*it1).x, (*it1).y});
			temp1.clear();
		}
		master_curve_list.push_back(master_curve_subset);
		master_curve_subset.clear();
		p++;
		itr++;
	}

	glEnd();

} //bezier curve

void knot_display()
{
	list< list<dpoint> >::iterator itr = knots.begin();
	list<dpoint> knot = *itr;
	list<dpoint>::iterator it;
	cout << "order k = " << korder << endl;
	for(int i = 0; i < num_curves; i++)
	{
		knot = *itr;
		cout << "Curve #" << i + 1 << ":" << endl;
		for(it = knot.begin(); it != knot.end(); it++)
		{
			cout << "u" << (*it).x << "(" << (*it).y << ")" << "|";
		}
		cout << endl;
		itr++;
	}

} //display knots

void bspline_edit()
{
	cout << "Edit knot info[1] or edit k-value (order)[2]? ";
	int choice;
	int curve_choice;
	int knot_choice;
	double knot_val_choice;
	cin >> choice;
	if(choice == 1)
	{
		cout << "Current knot values: " << endl;
		//display knots
		knot_display();
		//ask what knot to edit
		cout << "Which curve would you like to edit? ";
		cin >> curve_choice;
		if(curve_choice > num_curves)
		{
			cout << "Curve selection not valid!" << endl;
			return;
		}
		curve_choice--; //offset for zero start
		cout << "Which knot would you like to edit (enter the integer only, u_)? ";
		cin >> knot_choice;
		cout << "Enter a new knot value for curve #" << curve_choice << ", knot u" << knot_choice << ": ";
		cin >> knot_val_choice;
		list< list<dpoint> >::iterator itr = knots.begin();
		list<dpoint> knot;
		list<dpoint>::iterator it;
		//navigate to correct curve
		for(int i = 0; i < curve_choice; i++) itr++;
		//navigate to correct knot within curve
		knot = *itr;
		it = knot.begin();
		for(int i = 0; i < knot_choice; i++) it++;
		//edit the knot value
		(*it).y = knot_val_choice;
		//push remove the old list and replace with the new
		itr = knots.erase(itr);
		knots.insert(itr, knot);

		cout << "New knot values: " << endl;
		//display new knot
		knot_display();
	}
	else if (choice == 2)
	{
		cout << "Enter a new value for k: ";
		cin >> korder;
		bspline_setup();
	}
	else
	{
		cout << "Incorrect selection!" << endl;
	}
}

void bspline_setup()
{
	list<int>::iterator p = num_point.begin();
	//setup for knots, how to keep consistent: have a bspline setup function, have it called again everytime a point is removed or added
	knots.clear();
	list<dpoint> temp_knot;
	for(int i = 0; i < num_curves; i++)
	{ //for each curve
		for(int j = 0; j < *p + korder; j++)
		{
			temp_knot.push_back(dpoint{(double)j,(double)j});
		}
		knots.push_back(temp_knot);
		temp_knot.clear();
		p++;
	}

	return;


} //redo the knots when a point is removed or added (including new curve)

void bspline()
{
	if(curve_list.empty())
		return;

	glPointSize(4);
	glBegin(GL_POINTS);
	glColor3f(0, 0.5, 0);
	list<int>::iterator p = num_point.begin();
	list< list<point> >::iterator itr = curve_list.begin();
	list<point> lp = *itr;
	list<point>::iterator it = lp.begin();
	double ubar;
	list<dpoint> temp1; //= *itr; //used for calculation of next generation
	list<dpoint> temp2; //all calculations intially stored in here, before being moved to temp1
	list<dpoint>::iterator it1 = temp1.begin();
	double tempx;
	double tempy;
	list<dpoint> master_curve_subset;

	list< list<dpoint> >::iterator kitr = knots.begin();
	list<dpoint> knot_sublist = *kitr;
	list<dpoint>::iterator kit = knot_sublist.begin();

	for(int n = 0; n < num_curves; n++)
	{
		lp = *itr;
		//knots
		knot_sublist = *kitr;

		//another for loop to account for different segments?, increment by one knot at a time
		for(ubar = korder - 1 ; ubar < *p /*korder*/; ubar += 0.05)
		{
			int index = 0;
			kit = knot_sublist.begin();

			/**NEEDS WORK**/
			while(ubar >= (*kit).y)
			{
				index = (*kit).x;
				kit++;
			}
			/****/

			it = lp.begin();
			for(int z = 0; z < *p; z++)
			{//copy stuff into temp1
				temp1.push_back(dpoint{(double)(*it).x, (double)(*it).y});
				it++;
			}

			/**pop values off of the initial list**/
			for(int e = 0; e < index - (korder - 1); e++)
			{
				temp1.pop_front();
			}

			for(int j = 1; j < korder; j++)
			{//for generations
				it1 = temp1.begin();

				for(int i = index - (korder - 1); i < index - j + 1; i++)
				{
					//get ui+j, ui+k
					//calculate x and y
					kit = knot_sublist.begin();
					double uik = 0; double uij = 0;

					while((*kit).x <= i + korder)
					{//finding the right uik
						uik = (*kit).y;
						kit++;
					}
					kit = knot_sublist.begin();
					while((*kit).x <= i + j)
					{//finding the right uij
						uij = (*kit).y;
						kit++;
					}

					tempx = (uik - ubar)/(uik - uij) * (double)(*it1).x;
					tempy = (uik - ubar)/(uik - uij) * (double)(*it1).y;
				      	it1++;
					tempx += (ubar-uij)/(uik - uij) * (double)(*it1).x;
					tempy += (ubar-uij)/(uik - uij) * (double)(*it1).y;
					temp2.push_back(dpoint{tempx, tempy});
				}
				//copy and replace lists
				temp1 = temp2;
				temp2.clear();

			}
			//store in master list of final curve points?
			it1 = temp1.begin();
			//cout << (*it1).x << " " << (*it1).y << endl;
			//glVertex2f((*it1).x, (*it1).y);
			master_curve_subset.push_back({(*it1).x, (*it1).y});
			temp1.clear();
		} //outside ubar loop
		master_curve_list.push_back(master_curve_subset);
		master_curve_subset.clear();
		p++;
		itr++;
		kitr++;

	}

	glEnd();
} //b-spline curve

void write_out()
{
	ofstream outf("points.txt");
	if (!outf)
	{
		cout << "File could not be created, you may not have write permissions" << endl;
	}

	/**WRITE DATA BACK OUT, SAME AS TEST PRINTS in READ-IN**/
	outf << num_curves << endl;
	list<int>::iterator p = num_point.begin();
	for(int i = 0; i < num_curves; i++)
	{
		/**print the number of points in the curve**/
		outf << *p << endl;
		p++;

		/**print the points**/
		list< list<point> >::iterator itr;
		int a = 0;
	       	for(itr = curve_list.begin(); itr != curve_list.end() && a < i; itr++, a++)
		{//navigate to correct list of points

		}
		list<point> lp = *itr;
		list<point>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++)
		{//print out all point coordinates
			outf << (*it).x << " " << (*it).y << endl;
		}


	}

	outf.close();


}

//gets called when a mouse button is pressed
void mouse(int button, int state, int x, int y)
{
    //print the pixel location, and the grid location
    /*printf ("MOUSE AT PIXEL: %d %d, GRID: %d %d\n",(x/3), (600 - y)/3,(int)(x/pixel_size),(int)(y/pixel_size));
	switch(button)
	{
		case GLUT_LEFT_BUTTON: //left button
            printf("LEFT ");
            break;
		case GLUT_RIGHT_BUTTON: //right button
            printf("RIGHT ");
		default:
            printf("UNKNOWN "); //any other mouse button
			break;
	}
    if(state !=GLUT_DOWN)  //button released
        printf("BUTTON UP\n");
    else
        printf("BUTTON DOWN\n");  //button clicked
   */
    //function to select a point
    //button pressed, checks against list of lists
    //sets info that points to the appropriate element in the list for later modification
    //returns true if a point is selected
    //otherwise returns false, which means that the point selected will instead move to the new area that was clicked on

    if(func_sel == 'm')
    { //modify mode
    	if(!point_select((x/3), (600-y)/3))
    	{ //move point to new area
		//cout << "Point not selected!" << endl;
		move_point((x/3), (600-y)/3);
		//make that the new selected point
   	}
    }

    if(func_sel == 'n' && state == GLUT_DOWN)
    {//new curve mode
	if(!point_select((x/3), (600-y)/3))
	{
		new_curve((x/3), (600-y)/3);
		bspline_setup();
	}
	else
	{
		cout << "There's a point already there!" << endl;
	}
    }

    if(func_sel == 'i' && state == GLUT_DOWN)
    { //insertion mode
	if(!point_select((x/3), (600-y)/3))
	{

		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				new_p_before((x/3), (600-y)/3);
				break;
			case GLUT_RIGHT_BUTTON:
				new_p_after((x/3), (600-y)/3);
				break;
		}
		bspline_setup();
	}
    }

    //redraw the scene after mouse click
    glutPostRedisplay();
}

void new_curve(int x, int y)
{
	list<point> l;
	l.push_back(point{x,y});
	curve_list.push_back(l);
	//update global info
	num_curves++;
	num_point.push_back(1);
	return;
} //creates a curve via a new control point, not connected to any other point

void new_p_before(int x, int y)
{
	if(curve_list.empty())
	{
		cout << "Create a curve before adding more control points!" << endl;
		return;
	}
	//navigate according to selp_cl and selp_pl
	int a = 0; int b = 0;
	list< list<point> >::iterator itr;
	list<int>::iterator itr2;
	itr = curve_list.begin();
	itr2 = num_point.begin();
	while(itr != curve_list.end() && a < selp_cl)
	{//navigate through curve list
		itr++; a++; itr2++;
	}
	list<point> lp = *itr;
	list<point>::iterator it;
	it = lp.begin();
	while(it != lp.end() && b < selp_pl)
	{
		it++; b++;
	}

	*itr2 += 1; //modify num_point list
	//insert before
	lp.insert(it, point{x, y});
	itr = curve_list.erase(itr);
	curve_list.insert(itr, lp);

} //creates a new control point before an existing control point in sequence

void new_p_after(int x, int y)
{
	if(curve_list.empty())
	{
		cout << "Create a curve before adding more control points!" << endl;
		return;
	}
	//navigate according to selp_cl and selp_pl
	int a = 0; int b = 0;
	list< list<point> >::iterator itr;
	list<int>::iterator itr2;
	itr = curve_list.begin();
	itr2 = num_point.begin();
	while(itr != curve_list.end() && a < selp_cl)
	{//navigate through curve list
		itr++; a++; itr2++;
	}
	list<point> lp = *itr;
	list<point>::iterator it;
	it = lp.begin();
	while(it != lp.end() && b < selp_pl)
	{
		it++; b++;
	}

	*itr2 += 1; //modify num_point list
	//insert after
	lp.insert(++it, point{x, y});
	selp_pl++;
	itr = curve_list.erase(itr);
	curve_list.insert(itr, lp);

} //creates a new control point after an existing control point

bool point_select(int x, int y)
{

	int i; //curve_position
	int a; //to reach correct curve after every iteration
	int j; //point_position
	for(i = 0; i < num_curves; i++)
	{
		list< list<point> >::iterator itr;
		a = 0;
	       	for(itr = curve_list.begin(); itr != curve_list.end() && a < i; itr++, a++)
		{//navigate to correct list of points

		}

		j = 0;
		list<point> lp = *itr;
		list<point>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++, j++)
		{//find correct point, if any
			/**Check if mouse press is within bounds of the coordinate (1 off error allowed)**/
			if(x <= ((*it).x + 1) && x >= ((*it).x - 1) && y <= ((*it).y + 1) && y >= ((*it).y - 1))
			{ //if within the bounds of the point
				selp_cl = i; //curve_list position
				selp_pl = j; //point_list position
				//cout << "Point selected!" << endl;
				return true;
			}
		}


	}

	return false;
} //selects a point

void move_point(int x, int y)
{
	if(curve_list.empty())
	{
		cout << "No points to move!" << endl;
		return;
	}

	int a = selp_cl; //to reach correct curve
	int b = selp_pl; //point_list_position
	int i = 0; int j = 0;
	list< list<point> >::iterator itr;
	for(itr = curve_list.begin(); itr != curve_list.end() && i < a; itr++, i++)
	{//navigate to correct list of points

	}

	list<point> lp = *itr;
	list<point>::iterator it;
	for(it = lp.begin(); it != lp.end() && j < b; it++, j++)
	{//find correct point position

	}

	//assign new point location in local point_list, erase point list from curve list and reinsert
	//erase from point list, reinsert
	it = lp.erase(it);
	lp.insert(it, point{x,y});
	cout << "New position: ";
	cout << "x: " << x << " y: " << y << endl;
	//cout << (*it).y << endl;
	//erase from curve_list, reinsert (IMPORTANT)
	itr = curve_list.erase(itr);
	curve_list.insert(itr, lp);
	return;

}//move a point

void delete_point()
{
	if(curve_list.empty())
	{
		cout << "No points to delete!" << endl;
		return;
	}
	else
		cout << "Point deleted!" << endl;

	int a = selp_cl; //to reach correct curve
	int b = selp_pl; //point_list_position
	int i = 0; int j = 0;
	list< list<point> >::iterator itr;
	list<int>::iterator itr2; //for editing num_point
	itr = curve_list.begin();
	itr2 = num_point.begin();
	while(itr != curve_list.end() && i < a)
	{//navigate to correct list of points
		itr++; i++; itr2++;
	}
	*itr2 -= 1;

	list<point> lp = *itr;
	list<point>::iterator it;
	it = lp.begin();
	while(it != lp.end() && j < b)
	{//find correct point position
		it++; j++;
	}

	//assign new point location in local point_list, erase point list from curve list and reinsert
	//erase from point list
	it = lp.erase(it);
	//erase from curve_list, reinsert (IMPORTANT)
	itr = curve_list.erase(itr);
	if(!lp.empty())
		curve_list.insert(itr, lp);
	else
	{
		num_curves--;
		itr2 = num_point.erase(itr2);
	}

	if((signed)lp.size() <= selp_pl)
	{//to make sure selected point info does not go out of bounds
		while(((signed)lp.size() <= selp_pl) && selp_pl > 0)
			selp_pl--;
	}
	if((signed)curve_list.size() <= selp_cl)
	{//to make sure selected cuve info does not go out of bounds
		while(((signed)curve_list.size() <= selp_cl) && selp_cl > 0)
			selp_cl--;
	}
	//cout << selp_cl << " " << selp_pl << endl;

	return;

} //delete a point

//gets called when the curser moves accross the scene
void motion(int x, int y)
{
    //redraw the scene after mouse movement
	glutPostRedisplay();
}

void draw_curve()
{
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(0, 0.7, 0);

	/**DRAW LINES**/
	for(int i = 0; i < num_curves; i++)
	{
		list< list<dpoint> >::iterator itr;
		int a = 0;
	       	for(itr = master_curve_list.begin(); itr != master_curve_list.end() && a != i; itr++, a++)
		{//navigate to correct list of points
		}

		list<dpoint> lp = *itr;
		list<dpoint>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++)
		{//draw lines
			//cout << "First point: " << (*it).x << " " << (*it).y << endl;
			glVertex2f((*it).x, (*it).y);
			//cout << (*it).x << " " << (*it).y;
			it++;
			if(it == lp.end())
			{//if reached the end of the curve, stop line drawing
				it--;
				glVertex2f((*it).x, (*it).y);
				//cout << " to end of curve: " << (*it).x << " " << (*it).y << endl;
			}
			else
			{ //keep drawing as normal
				glVertex2f((*it).x, (*it).y);
				//cout << " to " << (*it).x << " " << (*it).y << endl;
				it--;
			}
		}

	}

	glEnd();

} //draws curve based on master_curve_list (points are double)

void draw_lines()
{
	glLineWidth(2); //sets the "width" of each line we are rendering
   	//tells opengl to interperate every two calls to glVertex as a line
    	glBegin(GL_LINES);
	glColor3f(.2,.2,1.0);

	/**DRAW LINES**/
	for(int i = 0; i < num_curves; i++)
	{
		list< list<point> >::iterator itr;
		int a = 0;
	       	for(itr = curve_list.begin(); itr != curve_list.end() && a != i; itr++, a++)
		{//navigate to correct list of points

		}
		list<point> lp = *itr;
		list<point>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++)
		{//draw lines
			glVertex2i((*it).x, (*it).y);
			it++;
			if(it == lp.end())
			{//if reached the end of the curve, stop line drawing
				it--;
				glVertex2i((*it).x, (*it).y);
			}
			else
			{ //keep drawing as normal
				glVertex2i((*it).x, (*it).y);
				it--;
			}
		}

	}

	glEnd();
}

void draw_points()
{
	glPointSize(8);
	glBegin(GL_POINTS);
	glColor3f(1, 0.2, 0.2);
	for(int i = 0; i < num_curves; i++)
	{

		/**display points**/
		list< list<point> >::iterator itr;
		int a = 0;
	       	for(itr = curve_list.begin(); itr != curve_list.end() && a != i; itr++, a++)
		{//navigate to correct list of points

		}
		list<point> lp = *itr;
		list<point>::iterator it;
		for(it = lp.begin(); it != lp.end(); it++)
		{//display points
			glVertex2i((*it).x, (*it).y);
		}


	}

	glEnd();
}

void mark_point()
{
	if(curve_list.empty())
		return;
	glPointSize(8);
	glBegin(GL_POINTS);
	glColor3f(0,0,0);
	list< list<point> >::iterator itr;
	int a = 0;
	itr = curve_list.begin();
	while(itr != curve_list.end() && a < selp_cl)
	{
		itr++; a++;
	}
	list<point> lp = *itr;
	list<point>::iterator it;
	it = lp.begin();
	int b = 0;
	while(it != lp.end() && b < selp_pl)
	{
		it++; b++;
	}
	glVertex2i((*it).x, (*it).y);

	glEnd();
} //marks the selected point black
