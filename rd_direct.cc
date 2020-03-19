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

int bresenhams_line_algorithm(int starting_x, int starting_y,  int ending_x, int ending_y );
int bresenhams_circle_drawer(int x, int y, int cx, int cy);

//Assign2 prototpyes
int point_pipeline(pointh& p);
int line_pipeline(pointh p, bool flag);

using std::cout;
using std::endl; 

// Global variables
static int frameNumber = 0;
float rgb_global[3] = {1, 1, 1};

// Global variables pertaining to assignment 2 are declared
// in matrix.h.
struct xform1 mainM; // current Transformation
struct xform1 world_to_cameraM;
struct xform1 cam_to_clipM; 
struct xform1 world_to_clippingM; // Are you suggesting combining world_to_cameraM & cam_to_clipM
struct xform1 clipping_to_deviceM;

// Declared in line matrix.h line 50

// Homogenius points for eyepoint and look up camera
struct pointh eye;
struct pointh at;
struct vec up;


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
	// Clipping depth
	double near = 1.0;
	double far = 10000000; //Should far be a long double?
	double aspect_ratio = display_xSize / display_ySize;
	eye.x = 0.0; eye.y = 0.0; eye.z = 0.0; eye.w = 0.0; // homogenus point
	at.x = 0.0; at.y = 0.0; at.z = -1.0; at.w = 0.0;	// homogenus point
	up.x = 0; up.y = 1; up.z = 0;						// vector
	
	// int fov = CameraFOV;
	// cout " Camera FOV is: " << fov << endl;

	// 2. The current transform should be set to the identity
	identity(mainM);

	// 3. The world to camera transformation can be computed using the camera eyepoint
	world_to_camera(mainM, eye, at, up);
	
	// 4. The camera to clipping coordinate transformation matrix can be computed using the near and far clipping 
	//		depths and the field of view.
	// int camera_to_clip(xform1& mainM, double fov, double near, double far, double aspect)
	// How do I get FOV and Aspect Ratio?
	camera_to_clip(cam_to_clipM, 45, near, far, aspect_ratio);
	
	// 5. These last two transformations can be combined and stored as the world to clipping coordinate matrix.
	// world to clip = world to camera * camera to clip
	multiply(world_to_clippingM, world_to_cameraM, cam_to_clipM);
	
	// 6. The clipping coordinate to device coordinate transform is also computed here. 
	// 	clip_to_device(mainM, width, height)
	clip_to_device(world_to_clippingM, display_xSize, display_ySize);
	
	//Declare frame number
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
	
	float oldColor[3] = {0, 0, 0};

	//Read what color was at that seed point before.
	rd_read_pixel(x, y, oldColor);
	
	//Pass these coordinates and colors to the decesion maker function.
	fillDecesionMaker(x, y, oldColor);
	
	return 0; 
}

int fillDecesionMaker(int x, int y, const float oldColor[])
{
//	It starts by filling the given seed point with the global drawing color.
//	Then it shoots out flood fill lines like a cross (+), filling up, down
//  left, and right as they go. When this first layer of fill decesion maker
//  is complete, the function recursivly itself at the most NE, SE, SW, and NW  
//  coordinates to try and go farther.

//  Filling a line is broken up into two parts to achive O(n) computation.
// 	1st, a scan function returns how far the span goes with the seed color.
//  2nd, a fill functions fills that span for the given distance.

// This function goes:
//  1. Fill seed point
// 	2. Fill rightwards main line, filling up and down as it goes.  
// 	3. Fill leftwards main line, filling up and down as it goes.
// 	4. Fill above main line, filling left and right as it goes.
// 	5. Fill below main line, filling left and right as it goes.

//  The reconstruction:
// 1. Change these If to while loops as a main decesion maker that shoots out like a cross at a given 
//		x y coordinate
// 2. Break up the flood lines into two parts.
//	2a. Scan line - returns the length that cosecutive pixels coords to fill
//  2b. Flood fill - Fills that legnth of pixels with color.
// 3. At the end of all 4 while loops for fillDecesionMaker, call more fill desesion makers
// At the last known NE SE SW NW points.
	
	std::cout << std::endl << "Fill decesion maker has been called" << std::endl;
	
	//Declaring working variables
	int x_coord = x;
	int y_coord = y;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Keeps track of inital flood cross +
	int rightmost_x = 0, left_x = 0;
	int top_y = 0, bottom_y = 0;
	
	// A set of working variables that stores where I values used in flood recrusion.
	int rightUp_x, rightUp_y;
	int rightDown_x, rightDown_y;
	int leftUp_x, leftUp_y;
	int leftDown_x, leftDown_y;
	
	int upRight_x, upRight_y;
	int upLeft_x, upLeft_y;
	int downRight_x, downRight_y;
	int downLeft_x, downLeft_y;
	
	// 1. Fill the given seed point.
	rd_write_pixel(x, y, rgb_global);	
	
	// 2. Fill rightwards main line, filling up and down as it goes.  
	
	// Priming read of right side.
	x_coord = x + 1;
	rd_read_pixel(x_coord, y_coord, cursor_rgb);
	
	//If the rightwards pixel is the old color that needs to get filled.
	if (cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] && cursor_rgb[2] == oldColor[2])
	{
		// While x is within bounds and the cursor is the same color as the old color.
		while ( x_coord < display_xSize &&		// Within x bounds
			cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] &&	cursor_rgb[2] == oldColor[2] )
		{
			
			//  Write new pixel.
			rd_write_pixel(x_coord, y_coord, rgb_global); 
			
			//  Fill span above
			top_y = scan_up(x_coord, y_coord, oldColor);
			fill_up(x_coord, y_coord, top_y);
			rightUp_x = x_coord;
			rightUp_y = y_coord + top_y; // Wouldn't be dumb to check these values.

			// Fill span below
			bottom_y = scan_down(x_coord, y_coord, oldColor);
			fill_down(x_coord, y_coord, bottom_y);
			rightDown_x = x_coord;
			rightDown_y = y_coord - bottom_y;
			
			x_coord++;
			
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	}		
	
	// 	3. Fill leftwards main line, filling up and down as it goes.
	
	//Priming read for left side.
	x_coord = x - 1;
	y_coord = y;
	rd_read_pixel(x_coord, y_coord, cursor_rgb);
	
	//If the rightwards pixel is the old color that needs to get filled.
	if (cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] && cursor_rgb[2] == oldColor[2])
	{
		// While x is within bounds and the cursor is the same color as the old color.
		while ( x_coord > 0 &&		// Within x bounds
			cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] &&	cursor_rgb[2] == oldColor[2] )
		{
			
			// Write new pixel.
			rd_write_pixel(x_coord, y_coord, rgb_global); 
			
			// Fill span above
			top_y = scan_up(x_coord, y_coord, oldColor);
			fill_up(x_coord, y_coord, top_y);
			leftUp_x = x_coord;
			leftUp_y = y_coord + top_y;

			// Fill span below
			bottom_y = scan_down(x_coord, y_coord, oldColor);
			fill_down(x_coord, y_coord, bottom_y);
			leftDown_x = x_coord;
			leftDown_y = y_coord - bottom_y;
			
			x_coord--;
			
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	}	
	
	// 	4. Fill above main line, filling left and right as it goes.

	//Priming read for left side.
	x_coord = x;
	y_coord = y + 1;
	rd_read_pixel(x_coord, y_coord, cursor_rgb);
	
	//If the rightwards pixel is the old color that needs to get filled.
	if (cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] && cursor_rgb[2] == oldColor[2])
	{
		// While x is within bounds and the cursor is the same color as the old color.
		while ( y_coord < display_ySize &&
			cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] &&	cursor_rgb[2] == oldColor[2] )
		{
			rd_write_pixel(x_coord, y_coord, rgb_global); 
			
			// Fill rightwards
			rightmost_x = scan_right(x_coord, y_coord, oldColor);
			fill_right(x_coord, y_coord, rightmost_x);
			upRight_x = x_coord + rightmost_x;
			upRight_y = y_coord;

			// Fill leftwards
			left_x = scan_left(x_coord, y_coord, oldColor);
			fill_left(x_coord, y_coord, left_x);
			upLeft_x = x_coord - left_x;
			upLeft_y = y_coord;
			
			y_coord++;
			
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	}
	
	
	// 	5. Fill below main line, filling left and right as it goes.
	
	//Priming read for left side.
	x_coord = x;
	y_coord = y - 1;
	rd_read_pixel(x_coord, y_coord, cursor_rgb);
	
	//If the rightwards pixel is the old color that needs to get filled.
	if (cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] && cursor_rgb[2] == oldColor[2])
	{
		// While x is within bounds and the cursor is the same color as the old color.
		while ( y_coord > 0 &&
			cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] &&	cursor_rgb[2] == oldColor[2] )
		{
			rd_write_pixel(x_coord, y_coord, rgb_global); 
			
			// Fill rightwards
			rightmost_x = scan_right(x_coord, y_coord, oldColor);
			fill_right(x_coord, y_coord, rightmost_x);
			downRight_x = x_coord + rightmost_x;
			downRight_y = y_coord;

			// Fill leftwards
			left_x = scan_left(x_coord, y_coord, oldColor);
			fill_left(x_coord, y_coord, left_x);
			downLeft_x = x_coord - left_x;
			downLeft_y = y_coord;
			
			y_coord--;
			
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	}
	// int rightUp_x, rightUp_y;
	// int rightDown_x, rightDown_y;
	// int leftUp_x, leftUp_y;
	// int leftDown_x, leftDown_y;
	
	// int upRight_x, upRight_y;
	// int upLeft_x, upLeft_y;
	// int downRight_x, downRight_y;
	// int downLeft_x, downLeft_y;
	
	// Recursion
	
	// Impliment Pre recursive checks
	
	// fillDecesionMaker(rightUp_x, rightUp_y, oldColor);
	// fillDecesionMaker(rightDown_x, rightDown_y, oldColor);
	// fillDecesionMaker(leftUp_x, leftUp_y, oldColor);
	// fillDecesionMaker(leftDown_x, leftDown_y, oldColor);
	
	// fillDecesionMaker(upRight_x, upRight_y, oldColor);
	// fillDecesionMaker(upLeft_x, upLeft_y, oldColor);
	// fillDecesionMaker(downRight_x, downRight_y, oldColor);
	// fillDecesionMaker(downLeft_x, downLeft_y, oldColor);


	
	
	
	/* Use information functions above to call Fill decesion maker 4 more times at the NE, SE, SW, NW coordinates 
		to finish filling */
	
	return RD_OK;
}

int scan_up(int x, int y, const float seedColor[])
{ // This function measures the how many consecutive pixels
	// are the seed color
	
	// Working variables
	int x_coord = x;
	int y_coord = y + 1;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
	// While x is within bounds and the cursor at the seed color.
		while ( y_coord < display_ySize &&
				cursor_rgb[0] == seedColor[0] && cursor_rgb[1] == seedColor[1] && cursor_rgb[2] == seedColor[2] )
		{	
			// Increment the length.
			y_coord++;
			
			// Read at the bottom of the loop.
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
	
	y_coord--;
	
	return y_coord;
}

int scan_down(int x, int y, const float oldColor[])
{ // This function measures the how many consecutive pixels
  // are the seed color
	
	// Working variables
	int x_coord = x;
	int y_coord = y - 1;
	float cursor_rgb[3] = {0, 0, 0};

	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
	// While x is within bounds and the cursor at the seed color.
		while ( y_coord > 0 &&
				cursor_rgb[0] == oldColor[0] && cursor_rgb[1] == oldColor[1] &&	cursor_rgb[2] == oldColor[2] )
		{	
			// Increment the length.
			y_coord--;
			
			// Read at the bottom of the loop.
			rd_read_pixel(x_coord, y_coord, cursor_rgb); 
		}
		
		y_coord++;
	
	return y_coord;
}

int scan_right(int x, int y, const float seedColor[])
{ // This function measures the how many consecutive pixels
	// are the seed color
	
	// Working variables
	int x_coord = x + 1;
	int y_coord = y;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
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
{ // This function measures the how many consecutive pixels
	// are the seed color
	
	// Working variables
	int x_coord = x - 1;
	int y_coord = y;
	float cursor_rgb[3] = {0, 0, 0};
	
	// Priming read.
	rd_read_pixel(x_coord, y_coord, cursor_rgb); 
	
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

int fill_up(int x, int y, int top_y)
{ // This function fills a predetermined length of pixels in a line upwards.
  // X needs to be passed as the x + 1 value.
  
	int x_coord = x;
	int y_coord = y;

	while(y_coord <= top_y)
	{
		rd_write_pixel(x_coord, y_coord, rgb_global);
		y_coord++;
	}
	
	return 0;
}

int fill_down(int x, int y, int bottom_y)
{ // This function fills a predetermined length of pixels in a line upwards.
  // X needs to be passed as the x + 1 value.
  
	int x_coord = x;
	int y_coord = y;

	while(y_coord >= bottom_y)
	{
		rd_write_pixel(x_coord, y_coord, rgb_global);
		y_coord--;
	}
	
	return 0;
}

int fill_right(int x, int y, int rightmost_x)
{
	int x_coord = x;
	int y_coord = y;

	while(x_coord <= rightmost_x)
	{
		rd_write_pixel(x_coord, y_coord, rgb_global);
		x_coord++;
	}
	
	return 0;
}

int fill_left(int x, int y, int left_x)
{
	int x_coord = x;
	int y_coord = y;

	while(x_coord >= left_x)
	{
		rd_write_pixel(x_coord, y_coord, rgb_global);
		x_coord--;
	}
	
	return 0;
}

int REDirect::rd_point(const float p[3])
{ //  Write a single pixel using the current drawing color. */	
	// 1st extract x and cooridnates from the argument 'p'.
	int x_coord = p[0];
	int y_coord = p[1];
	//int z-coord =p[2];
	
	rd_write_pixel(x_coord, y_coord, rgb_global);
	
	return 0;
	
}

int REDirect::rd_line(const float start[3], const float end[3])
{ // Calls bresenhams_line_algorithm
	/* Take the input and output points from the arguments.*/
	int starting_x = start[0];
	int starting_y = start[1];
	int starting_z = start[2];
	int ending_x = end[0];
	int ending_y = end[1];
	int ending_z = end[2];
	
	// Put line drawing pieline here.
	
	// Does bresenhams line drawing algo need 
	
	// Envoke bresenhams line drawing algorithim
	bresenhams_line_algorithm(starting_x, starting_y, ending_x, ending_y);
	
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

int point_pipeline(pointh& p)
{	// 1st plot the points

	//  The point pipeline should take a homogeneous point and transform it by the
	//  current transform and the world to clipping transform.
	
	struct pointh temp1;
	struct pointh temp2;
	
	// Homogenius point multiplied by the current transform
	//multiply(temp1, mainM, p)
	
	// We have mainM, how to we derive points from that?
}

