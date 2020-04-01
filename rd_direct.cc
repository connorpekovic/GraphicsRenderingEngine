/* File name: rd_direct.cc
 * Programmer: Connor Pekovic
 * Date: 2/28/2020
 * Purpose: 3D Drawings, assignment 2
 *************************************/
 
#include <iostream>
#include <string>
#include <cstdlib>
#include "matrix.h"
#include "rd_direct.h"
#include "rd_display.h"
#include "rd_error.h"

// Function prototypes
int fff4(int x, int y, const float seedColor[]);
int fill_span(int xs, int ex, int y);
int fillDecesionMaker(int x, int y, const float oldColor[]);
int scan_up(int x, int y, const float oldColor[]);
int scan_down(int x, int y, const float oldColor[]);
int scan_right(int x, int y, const float seedColor[]);
int scan_left(int x, int y, const float seedColor[]);
int fill_up(int x, int y, int top_y);
int fill_down(int x, int y, int bottom_y);
int fill_right(int x, int y, int right_x);
int fill_left(int x_coord, int y_coord, int left_x);
int camera_to_clip(xform1& result, double fov, double near, double far, double aspect);
int clip_to_device(xform1& result, int width, int height);
int boundry_checks(pointh& p);

int bresenhams_line_algorithm(int starting_x, int starting_y,  int ending_x, int ending_y );
int bresenhams_circle_drawer(int x, int y, int cx, int cy);

//Assign2 prototpyes
int line_pipeline(pointh& p, bool flag);
int point_pipeline(pointh& p);

using std::cout;
using std::endl; 

// Global variables
static int frameNumber = 0;
float rgb_global[3] = {1, 1, 1};

// Global variables pertaining to assignment 2 are declared
// in matrix.h.
struct xform1 current_transform; // current Transformation
struct xform1 world_to_cameraM;
struct xform1 cam_to_clipM; 
struct xform1 world_to_clippingM; // Are you suggesting combining world_to_cameraM & cam_to_clipM
struct xform1 clipping_to_deviceM;
stack<pointh> move_draw_stack;

// Declared in line matrix.h line 50
// stack<xform1> transform_stack;

// Homogenius points for eyepoint and look up camera
struct pointh eye;
struct pointh at;
struct vec up;
int CameraFOV;
double near;
double far;
double aspect_ratio = display_xSize / display_ySize;

// Globals for fill 
int last_known = -1;


int REDirect::rd_display(const string& name, const string& type, const string& mode)
{/* The major work for this function is done behind the scenes. */
	return RD_OK;
}
	
int REDirect::rd_format(int xresolution, int yresolution)
{		
	if (display_xSize && display_ySize >= 0)
		return 0;
	else
		return -2;
}

int REDirect::rd_frame_begin(int frame_no)
{/* Prompt: Store the frame number in a global variable somewhere. */
	frameNumber = frame_no;
	return 0;
}

int REDirect::rd_world_begin(void)
{ // Comments indicate excerpts from the prompt for assignment 2

	// 1. Declaring values for the group of global variables to keep track of.
	near = 1.0;
	far = 10000000; //Should far be a long double?
	eye.x = 0.0; eye.y = 0.0; eye.z = 0.0; eye.w = 0.0; // homogenus point
	at.x = 0.0; at.y = 0.0; at.z = -1.0; at.w = 0.0;	// homogenus point
	up.x = 0; up.y = 1; up.z = 0;						// vector

	// Clipping depth
	// 2. The current transform should be set to the identity.
	identity(current_transform);

	// 3. The world to camera transformation can be computed using the camera eyepoint
	world_to_camera(current_transform, eye, at, up);
	
	// 4. The camera to clipping coordinate transformation matrix can be computed using the near and far clipping 
	//	depths and the field of view.
	camera_to_clip(cam_to_clipM, 45, near, far, aspect_ratio);
	
	// 5. These last two transformations can be combined and stored as the world to clipping coordinate matrix.
	multiply(world_to_clippingM, world_to_cameraM, cam_to_clipM); // world to clip = world to camera * camera to clip
	
	// Q: Boundry coordinate checks?
	
	// 6. The clipping coordinate to device coordinate transform is also computed here. 
	clip_to_device(clipping_to_deviceM, display_xSize, display_ySize);
	
	// Declare frame number
	rd_disp_init_frame(frameNumber);
	return 0;
}

int REDirect::rd_world_end(void)
{/* Finish off the display frame here. See rd_display.h. */
	
	//rd_clear() is a good canidate too
	rd_disp_end_frame();
	
	return 0;
}

int REDirect::rd_frame_end(void)
{/* Prompt: Same as rd_world_end() at this point. But don't just have one call the other. They will change. */
	rd_disp_end_frame();
	return 0;
}

/* Welcome to the Lighting and Shading section. */

int REDirect::rd_color(const float color[])
{/* Use the function to change the global drawing color */
	rgb_global[0] = color[0];
	rgb_global[1] = color[1];
	rgb_global[2] = color[2];
	
	return RD_OK;  
}

/* Store the color values to be used as the screen background color. */
int REDirect::rd_background(const float color[])
{
	rd_set_background(color);
	return 0;
}

int REDirect::rd_fill(const float seed_point[3])
{	//Calls your flood fill routine using the given seed point and the current drawing color as the fill color.
	
	int x = seed_point[0];
	int y = seed_point[1];
	
	float seedColor[3] = {0, 0, 0};

	//Read what color was at that seed point before.
	rd_read_pixel(x, y, seedColor);
	
	//Pass these coordinates and colors to the decesion maker function.
	fff4(x, y, seedColor);
	
	return 0; 
}

int fff4(int x, int y, const float seedColor[])
{
	// 1. Check for stopping conditions
    // 2. Loop decrementing xs until xs is outside of the image boundaries
	// 3. Loop incrementing xe until xe is outside of the image boundaries
    // 4. Fill the span
	// 5. Go Up
	// 5a. Pre recursive checks for boundries and color
	// 6. Go Down
	// 6a. Pre recursive checks for boundries and color

	// 1. Check for stopping conditions
    // If (x, y) is outside the image boudaries, exit
	if ( x >= display_xSize || x <= 0 || y >= display_ySize || y <= 0 )
	{
		return 0;
	}
	
    // If the color at (x, y) does not match the seed color, exit
	float cursor_rgb[3] = {0, 0, 0};
	rd_read_pixel(x, y, cursor_rgb);
	
	if (cursor_rgb[0] != seedColor[0] || cursor_rgb[1] != seedColor[1] || cursor_rgb[2] != seedColor[2])
	{
		return 0;
	}

    // 2. Loop decrementing xs until xs is outside of the image boundaries
	int leftwards_x = scan_left(x, y, seedColor);
	
	// 3. Loop incrementing xe until xe is outside of the image boundaries
	int rightwards_x = scan_right(x, y, seedColor);

    // 4. Fill the span
	fill_span(leftwards_x, rightwards_x, y);

    // 5. Go up
   
    // Set working variables
	int sx = leftwards_x;
	int ex = rightwards_x;
	int y_coord = y;
	y_coord++;
	
	 // Loop x from xs to xe, calling fff4(x, y+1, seedColor)
	while(sx <= ex)
	{	
		
		// 5a. Pre recursive check
		// Boundries
		if ( x < display_xSize || x > 0 || y < display_ySize || y > 0 )
		{
			// Color
			rd_read_pixel(x, y_coord, cursor_rgb);
			if (cursor_rgb[0] == seedColor[0] && cursor_rgb[1] == seedColor[1] && cursor_rgb[2] == seedColor[2])
			{
				int tempY = y_coord;
				fff4(sx, tempY, seedColor);
				y_coord++;
			}
		}
		
		sx++;
	}
	
	// 6. Go down
	sx = leftwards_x;
	ex = rightwards_x;
	y_coord = y;
	y_coord--;
	
	while(sx <= ex)
	{		
		// 6a. Pre recursive check
		// Boundries
		if ( x < display_xSize || x > 0 || y < display_ySize || y > 0 )
		{
			// Color
			rd_read_pixel(x, y_coord, cursor_rgb);
			if (cursor_rgb[0] == seedColor[0] && cursor_rgb[1] == seedColor[1] && cursor_rgb[2] == seedColor[2])
			{
				last_known = y_coord;
				fff4(sx, y_coord, seedColor);
				y_coord = last_known;
				y_coord--;
				
			}
		}
		sx++;
	}
	
	return RD_OK;
}

int scan_right(int x, int y, const float seedColor[])
{    // Loop incrementing xe until xe is outside of the image boundaries
    // or the color at (xe, y) does not match the seed color

	// Working variables
	int x_coord = x + 1;
	int y_coord = y;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
	if ( cursor_rgb[0] != seedColor[0] || cursor_rgb[1] != seedColor[1] || cursor_rgb[2] != seedColor[2] )
	{
		return x_coord - 1;
	}
	
	// While x is within bounds and the cursor at the seed color.
		while ( x_coord < display_xSize &&
				cursor_rgb[0] == seedColor[0] && cursor_rgb[1] == seedColor[1] && cursor_rgb[2] == seedColor[2] )
		{	
			// Increment the length.
			x_coord++;
			
			// Read at the bottom of the loop.
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	
	x_coord--;
	
	return x_coord;
}

int scan_left(int x, int y, const float seedColor[])
{    // Loop decrementing xs until xs is outside of the image boundaries
    // or the color at (xs, y) does not match the seed color
	
	// Working variables
	int x_coord = x - 1;
	int y_coord = y;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
	// if ( cursor_rgb[0] != seedColor[0] || cursor_rgb[1] != seedColor[1] || cursor_rgb[2] != seedColor[2] )
	// {
		// flagUpRight = 0; // Set flag for not calling recursion
		// return x_coord - 1; 
	// }
	
	// While x is within bounds and the cursor at the seed color.
		while ( x_coord > 0 &&
				cursor_rgb[0] == seedColor[0] && cursor_rgb[1] == seedColor[1] && cursor_rgb[2] == seedColor[2] )
		{	
			// Increment the length.
			x_coord--;
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	
	x_coord++;
	
	return x_coord;
}


int fill_span(int xs, int ex, int y)
{ // Fills a span with pixels from the start to end.
	int x_coord = xs;
	int y_coord = y;

	while(x_coord <= ex)
	{
		rd_write_pixel(x_coord, y_coord, rgb_global);
		x_coord++;
	}
	
	return 0;
}



int REDirect::rd_point(const float p[3])
{ // Write a single pixel using the current drawing color. */	
  // 1st extract x and cooridnates from the argument 'p'.
	float x_coord = p[0];
	float y_coord = p[1];
	float z_coord = p[2];
	
	// Turn input into homogeneous 
	struct pointh point;
	point.x = p[0]; point.y = p[1]; point.z = p[2];
	
	rd_write_pixel(x_coord, y_coord, rgb_global);
	
	// Send into pipeline, defined just below.
	//point_pipeline(point);
	
	return 0;
}

// The point pipeline   q: How do I test point pipeline?
int point_pipeline(pointh& p)
{	
	//  1. Take a homogeneous point and transform it by the current
	//  transform and the world to clipping transform.
	
	// p = p * main trandformation matrix
	multiply(p, current_transform, p);
	
	// 2. The world to clipping transform.
	// p = p * main trandformation matrix
	multiply(p, world_to_clippingM, p);
	
	// 3. At this stage the resulting point (in normalized clipping coordinates) 
	// should be tested to see if it lies in the canonical view volume (from 0 
	// to 1 in x, y, and z).
	// Q: What's part about the 0 to 1 about?
	
	// prepare boundry coordinates
	double x = p.x;
	double y = p.y;
	double z = p.z;
	double w = p.w;
	double wx = w - x;
	double wy = w - y;
	double wz = w - z;	

	// If any of these are negative, then we are outside the boundries.
	// If it passes this test, the point is converted to device coordinates and plotted.
	if ( x > 0  ||
		 y > 0  ||
		 z > 0  ||
		 wx > 0 ||
		 wy > 0 ||
		 wz > 0  )
		 {
			// To plot these points, typecast to int's before calling rd_write_pixel
			int X = (int)p.x;
			int Y = (int)p.y;
			// What about Z?
			
			rd_write_pixel(X, Y, rgb_global);
		}
		
	return 0;	

	// If it passes this test, the point is converted to device coordinates and plotted.
}

int REDirect::rd_line(const float start[3], const float end[3])
{ // This function recived 2 points, representing the begining and
  // the end of the line.  Convert the input to a struct that represents
  // a homogeneous coordinate, then pass them to the line pipeline one 
  // at a time.
  
	// 1st point going into line pipeline
	struct pointh point1, point2;
	point1.x = start[0];
	point1.y = start[1];
	// int point1.w = start[2];	
	
	//line_pipeline(point1, 0); // false = move
	
	// 2nd point goes into line pipeline
	point2.x = end[0];
	point2.y = end[1];
	// int point2.w = end[2];
	
//	line_pipeline(point2, 1);
	
	bresenhams_line_algorithm(point1.x, point1.y, point2.x, point2.y);
	
	return 0;
}

	// Q: Where are all my translations, roations, and scales going to happen?
	// A: rd_translate, rd_rotate, rd_scale	

// The Line pipeline
int line_pipeline(pointh& p, bool flag)
{ // 1. The pipeline for lines should take two arguments, a single
  // homogeneous point and a flag that indicates whether the point 
  // should be moved to or drawn to.

	struct pointh old_vertex;
	
	// 2. Regardless of the flag, the point should be transformed 
	// through object coordinates to world coordinates to clipping 
	// coordinates.
	
	// Object to world 
	multiply(p, current_transform, p);
	
	// World to clipping
	multiply(p, world_to_clippingM, p);
		
	// Boundry coordinate checks
	int result = boundry_checks(p);
	
	if (result == 0)
	{
		return 0;
	}
	
	// 3.  At this step if the flag is set for moving only, the point 
	// should be stored (in clipping coordinates) in a static or global
	// variable.
	
	multiply(p, clipping_to_deviceM, p);
	
	// Move only
	if (flag == 0) 
	{
		old_vertex = p;
		//  with a poly set, use the move_draw_stack
		//	store points, use .top() to pop off values.
	}
	// Draw
	else		  
	{
		//  4. If the line is to be drawn, then both the old_vertex (stored 
		//	in the global variable) and the new vertex are transformed 
		// 	to device coordinates and a line drawn between the resulting 
		//	coordinates. 
		
		// Prepare better variable names for bresenhams line 
		// drawing algorithm.
		int sX = old_vertex.x;// Start  X
		int sY = old_vertex.y;// Start  Y
		int eX = p.x;         // Ending X
		int eY = p.y;         // Ending Y
		
		// Envoke bresenhams line drawing algorithim.
		bresenhams_line_algorithm(sX, sY, eX, eY);
	}
	
	//  5. At the end of this process, the clipping coordinate version 
	//	of the new point should be stored as the old vertex ( in the 
	// 	global variable ). 
	old_vertex = p;
	
	return 0;
}

int bresenhams_line_algorithm(int starting_x, int starting_y,  int ending_x, int ending_y ) 
{ 
	int sX = starting_x, sY = starting_y; //Starting point
	int eX = ending_x, eY = ending_y;     //Ending poiint
	int x = starting_x, y = starting_y;	  //Used in loading workingXyz.
	int dX = std::abs(eX - sX); 		  //std::cout << "Change in X is " << dX << std::endl;
	int dY = std::abs(eY - sY); 		  //std::cout << "Change in Y is " << dY << std::endl;
	int p_wider = (2 * dY) - dX;		  // Octants 2, 3, 6, and 7.
	int p_taller = (2 * dX) - dY;		  // Octants 1, 4, 5, and 8.

	
	// std::cout << std::endl << "  LINE NUMBER : " << counter << std::endl;
	// counter++;
	// std::cout << "starting_x " << starting_x << std::endl;
	// std::cout << "starting_y " << starting_y << std::endl;
	// std::cout << "ending_x " << ending_x << std::endl;
	// std::cout << "ending_y " << ending_y << std::endl;
	// std::cout << "dX " << dX << std::endl;
	// std::cout << "dY " << dY << std::endl;
	// std::cout << "P value is : " << p << std::endl;
	
	/* Decesion tree: First, decide which of the four quadrants the line is. Them,
		picking one of the tow octants of the quadrant. */
	
	// If X increases and Y increases.
	if ( sX <= eX && sY <= eY ) 		 // Qudrant 1
	{	
		// If taller than it is wide.
		if ( dX < dY ) 		 // Octant 1
		{	
			for ( int y = sY; y <= eY; y++ )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if (p_taller > 0 )
				{
					p_taller += 2*dX - 2*dY;
					// std::cout << " P value: " << p << std::endl;
					
					x++;
				}
				else
				{
					p_taller += 2*dX;
					// std::cout << " P value: " << p << std::endl;
				}
			}
		
			return RD_OK;
		}
		//It's wider than it is tall or is 45 degrees.
		else	 				// Octant 2
		{	
			// std::cout << "I'm in Octant 2." << std::endl;
			
			
			for ( int x = sX; x <= eX; x++ )
			{
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_wider > 0 )
				{		
					p_wider += (2 * dY) - (2 * dX);
					y++;
				}
				else
				{
					p_wider += 2 * dY;
				}
			}
			return RD_OK;
		}
	}
	//If X decreses and Y increases
	else if ( sX >= eX && sY <= eY )  // Quandrant 2
	{
		//If it's taller than it is wide
		if ( dX <= dY ) 		 // Octant 8 
		{	
			for ( int y = sY; y <= eY; y++ )
			{
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_taller >= 0 )
				{
					p_taller += (2 * dX) - (2 * dY);
					x--;	
				}
				else
				{
					p_taller += (2 * dX);
				}
			}
			return RD_OK;
		}
		else  				  		 // Octant 7
		{	
			for ( int x = sX; x > eX; x-- )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_wider >= 0 )
				{	
					p_wider += (2 * dY) - (2 * dX);
					y++;
				}
				else
				{
					p_wider += (2 * dY);
					
				}
			}
			return RD_OK;
		}
	}
	// If X decreases or Y is flat or decreases
	else if ( sX > eX && sY >= eY )  // Quadrant 3
	{
		if ( dX < dY )		 // Octant 5
		{	
			for ( int y = sY; y > eY; y-- )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_taller >= 0 )
				{
					p_taller += (2 * dX) - (2 * dY);
					x--;
				}
				else
				{
					p_taller += (2 * dX);
				}
			}
			return RD_OK;
		}
		else						 // Octant 6
		{	
		// std::cout << "I'm in Octant 6." << std::endl;
			for ( int x = sX; x > eX; x-- )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_wider >= 0 )
				{
					p_wider += (2 * dY) - (2 * dX);
					y--;
				}
				else
				{
					p_wider += (2 * dY);
				}
			}
		}
		return RD_OK;
	}
	// If X increases and Y is flat or decreases
	else if ( sX <= eX && sY > eY ) 	// Quadrant 4
	{
		// std::cout << "I'm in quadrant 4." << std::endl;
		if ( dX <= dY )					// Octant 4
		{	
			// std::cout << "I'm in octant 4." << std::endl;
			for ( int y = sY; y > eY; y-- )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_taller >= 0 )
				{
					p_taller += (2 * dX) - (2 * dY);
					x++;
				}
				else
				{
					p_taller += (2 * dX);
				}
			}
		}
		else						// Octant 3
		{
			// std::cout << "I'm in octant 3.";
			for ( int x = sX; x < eX; x++ )
			{	
				rd_write_pixel(x, y, rgb_global);
				
				if ( p_wider >= 0 )
				{		
					p_wider += (2 * dY) - (2 * dX);
					y--;
				}
				else
				{
					p_wider += (2 * dY);
				}
			}
		}
		return RD_OK;
	}
	else
	{
		std::cout << "Line coordinates don't fit into any of the 4 quadrants.";	
	}
	
	return 0;
}

int REDirect::rd_circle(const float center[3], float radius)
{// Calls your circle drawing routine and draws a circle using the current drawing color.

	int cx = center[0]; // Center X
	int cy = center[1]; // Center Y
	int r = radius;
	int x = 0;
	int y = radius;
	
	// Initalization for decesion variable.
	int p = 3 - ( 2 * r );
	
	bresenhams_circle_drawer(x, y, cx, cy);
	
	while(x < y)
	{
		if(p <= 0)
		{
			p += (4*x) + 6;
			bresenhams_circle_drawer(++x,y,cx,cy);
		}
		else
		{
			p += (4*(x-y)) + 10;
			bresenhams_circle_drawer(++x,--y,cx,cy);
		}
	}
	return 0;
}

int bresenhams_circle_drawer(int x, int y, int cx, int cy)
{
		rd_write_pixel(x+cx,y+cy, rgb_global);
		rd_write_pixel(-x+cx,y+cy, rgb_global);
		rd_write_pixel(x+cx, -y+cy, rgb_global);
		rd_write_pixel(-x+cx, -y+cy, rgb_global);
		rd_write_pixel(y+cx, x+cy, rgb_global);
		rd_write_pixel(y+cx, -x+cy, rgb_global);
		rd_write_pixel(-y+cx, x+cy, rgb_global);
		rd_write_pixel(-y+cx, -x+cy, rgb_global);
	return 0;
} 

int REDirect::rd_translate(const float p[3])
{
	// This function makes a 4x4 out of the three floats and   
	// by the current transform, storing the result back in the current transform
	translate(current_transform, p[0], p[1], p[2]);
	
	return 0;
}

int REDirect::rd_scale(const float p[3])
{
	// Takes an array of three floats, the scale factors in x, y, and z, creates 
	// a scale matrix and multiplies it by the current transform, storing the 
	// result back in the current transform.
	scale(current_transform, p[0], p[1], p[2]);
	
	return 0;
}

int REDirect::rd_rotate_xy(const float theta)
{
	// Takes a float which is the angle of rotation in degrees and creates a rotation 
	// matrix in the xy plane. The matrix is multiplied by the current transformation
	// matrix and the results stored back in the current transform.
	rotate_xy(current_transform, theta);
	
	return 0;
	
}

int REDirect::rd_rotate_yz(const float theta)
{
	// Ditto in the yz plane.
	rotate_yz(current_transform, theta);
	
	return 0;
	
}

int REDirect::rd_rotate_zx(const float theta)
{
	// Ditto in the yz plane.
	rotate_zx(current_transform, theta);
	
	return 0;
	
}

// Q: What should rd_camera...'s function header be called. 
//    'const float p[4]' is a guess.

int REDirect::rd_camera_eye(const float p[4])
{ // Store the values passed in into the global pointh struct 'eye'.

	eye.x = p[0];
	eye.y = p[1];
	eye.z = p[2];
	eye.w = p[3];

	return 0;
}

int REDirect::rd_camera_at(const float p[4])
{ // Store globally.
	
	at.x = p[0];
	at.y = p[1];
	at.z = p[2];
	at.w = p[3];
	
	return 0;
}

int REDirect::rd_camera_up(const float p[3])
{ // Store globally.
	
	up.x = p[0];
	up.y = p[1];
	up.z = p[2];
	
	return 0;
}

int REDirect::rd_camera_fov(const int fov)
{ // Store globally.

	CameraFOV = fov;
	
	return 0;
}

 int REDirect::rd_clipping(const float p[2])
 { // Store the values in the near and far global clipping depths respectively.
	 near = p[0];
	 far = p[1];
	 
	 return 0;
 }

