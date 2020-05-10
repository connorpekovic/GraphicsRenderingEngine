/* File name: rd_direct.cc
 * Programmer: Connor Pekovic
 * Date: 2/28/2020
 * Purpose: 3D Drawings, assignment 2
 *************************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <unistd.h> // for sleep function
#include <vector>
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
int translate(xform1& m1, float tx, float ty, float tz);

int bresenhams_line_algorithm(int starting_x, int starting_y,  int ending_x, int ending_y );
int bresenhams_circle_drawer(int x, int y, int cx, int cy);

//Assign2 prototpyes
int line_pipeline(pointh p, bool flag);
int point_pipeline(pointh& p);
void draw_cube();
void draw_sphere(const float radius);
void index_to_coords(int row, int column, pointh& p);
int findVerticesPerFace(const int face[]);
int line_clipping(pointh p, bool flag);

/* In geometric function, these control the wireframe */
int rowCount;
int columnCount;

using std::cout;
using std::endl;

// Global variables
static int frameNumber = 0;
float rgb_global[3] = {1, 1, 1};

// Global variables pertaining to assignment 2 are declared
// in matrix.h.
struct xform1 current_transform; // current Transformation
struct xform1 world_to_cameraM;
struct xform1 camera_to_clipM;
struct xform1 world_to_clippingM; // Are you suggesting combining world_to_cameraM & camera_to_clipM
struct xform1 clipping_to_deviceM;
stack<pointh> move_draw_stack;
struct pointh tracePoints[4];
int count1 = 0;

// Declared in line matrix.h line 50
//stack<xform1> transform_stack;

// Homogenius points for eyepoint and look up camera
struct pointh eye = { 0.0, 0.0, 0.0, 1.0 };// homogenus point
struct pointh at = { 0.0, 0.0, -1.0, 1.0 };
struct vec up = { 0.0, 1.0, 0.0 };
double CameraFOV = 90;
double near = 1.0;
double far = 100000000;
int drawPathSize = 0;

// Line pipeline
struct pointh prev_vertex;
struct pointh temp_p;
struct pointh temp_prev_p;

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
{
	// Set aspect ratio.
	double aspect_ratio = (double)display_xSize / (double)display_ySize;

	// 2. The current transform should be set to the identity.
	identity(current_transform);

	// 3. The world to camera transformation can be computed using the camera eyepoint
	world_to_camera(world_to_cameraM, eye, at, up);

	// 4. Uses the near and far clipping for depths and the field of view.
	camera_to_clip(camera_to_clipM, CameraFOV, near, far, aspect_ratio);

	// 5. These last two transformations can be combined and stored as the world to clipping coordinate matrix.
	multiply(world_to_clippingM, world_to_cameraM, camera_to_clipM); // world to clip = world to camera * camera to clip

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
	usleep(80000);
	rd_disp_end_frame();
	return 0;
}

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
		return RD_OK;
	}

    // If the color at (x, y) does not match the seed color, exit
	float cursor_rgb[3] = {0, 0, 0};
	rd_read_pixel(x, y, cursor_rgb);

	if (cursor_rgb[0] != seedColor[0] || cursor_rgb[1] != seedColor[1] || cursor_rgb[2] != seedColor[2])
	{
		return RD_OK;
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
		fff4(sx, y_coord, seedColor);
		sx++;
	}

	// 6. Go down
	sx = leftwards_x;
	ex = rightwards_x;
	y_coord = y;
	y_coord--;

	while(sx <= ex)
	{
		fff4(sx, y_coord, seedColor);
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

	// Turn input into homogeneous
	struct pointh point;
	point.x = p[0]; point.y = p[1]; point.z = p[2];

	// rd_write_pixel(x_coord, y_coord, rgb_global);

	// Send into pipeline, defined just below.
	point_pipeline(point);

	return 0;
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
	point1.z = start[2];

	line_pipeline(point1, 0); // false = move

	// 2nd point goes into line pipeline
	point2.x = end[0];
	point2.y = end[1];
	point2.z = end[2];

    line_pipeline(point2, 1);  // true = draw

	return 0;
}

// The Line pipeline
int line_pipeline(pointh p, bool flag)
{
  int sX, sY, eX, eY;

	// Current transform.
	multiply(p, current_transform, p);

	//World to Camera
	multiply(p, world_to_cameraM, p);

	// Camera to clip
	multiply(p, camera_to_clipM, p);

	// Point * Translation matrix
	struct xform1 transM;  // produce 4x4
	identity(transM);
	translate(transM, 1.0, 1.0, 0.0); // load 4x4
	multiply(p, transM, p); // 4x4 x 1x4 multiplication

	// Point * Scale matrix
	struct xform1 scaleM;
	identity(scaleM);
	scale(scaleM, .5, .5, 0);
	multiply(p, scaleM, p);

//	line_clipping(p, flag);

  // Move
  if (flag == 0)
  {
    // TracePoints[] contains pointh's that are to be drawn
    copy(tracePoints[count1], p);

    count1++;
  }
  // Draw
  else
  {
    // Prepare the first end point of the line.
    // Start point already in variable 'p'.
    copy(prev_vertex, tracePoints[count1 - 1]);
    // copy(into, from)

    count1++;

    while (count1) {

      // Clipping to Device
      multiply(temp_prev_p, clipping_to_deviceM, prev_vertex);
      multiply(temp_p, clipping_to_deviceM, p);

      // Normalize by dividing through by w
      struct cartePoint start, end;
      normalize_homog_to_carte(end, temp_prev_p);
      normalize_homog_to_carte(start, temp_p);

      // Float to Int
      sX = start.x;
      sY = start.y;
      eX = end.x;
      eY = end.y;

      // Plot line
      bresenhams_line_algorithm(sX, sY, eX, eY);

      //Move down to next point.
      count1--;

      // Prepare variables for next iteration.
      copy(p, prev_vertex);
      copy(prev_vertex, tracePoints[count1 - 1]);
    }
    count1 = 0;
  }
  return 0;


}


int line_clipping(pointh p, bool flag)
{
  
	return 0;
}

// The Point Pipeline
int point_pipeline(pointh& p)
{
	// Current transform.
	multiply(p, current_transform, p);

	//World to Camera
	multiply(p, world_to_cameraM, p);

	// Camera to clip
	multiply(p, camera_to_clipM, p);

	// Point * Translation matrix
	struct xform1 transM;  // produce 4x4
	identity(transM);
	translate(transM, 1.0, 1.0, 0.0); // load 4x4
	multiply(p, transM, p); // 4x4 x 1x4 multiplication

	// Point * Scale matrix
	struct xform1 scaleM;
	identity(scaleM);
	scale(scaleM, .5, .5, 0);
	multiply(p, scaleM, p);

	// Checks aginst boundry coordinates
	int result = boundry_checks(p);
	if (result == 0){
		return 0;
	}

	// Clipping to Device.
	multiply(p, clipping_to_deviceM, p);

	// Normalize into cartesian coord's by dividing through by w.
	struct cartePoint coord;
	normalize_homog_to_carte(coord, p);

	// Write pixel
	int x = coord.x;
	int y = coord.y;
	rd_write_pixel(x, y, rgb_global);

	return RD_OK;
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

					x++;
				}
				else
				{
					p_taller += 2*dX;
				}
			}

			return RD_OK;
		}
		//It's wider than it is tall or is 45 degrees.
		else	 				// Octant 2
		{
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

int REDirect::rd_translate(const float t[3])
{
	translate(current_transform, t[0], t[1], t[2]);

	//transform_stack.push(current_transform);

	return 0;
}

int REDirect::rd_scale(const float s[3])
{
	// Takes an array of three floats, the scale factors in x, y, and z, creates
	// a scale matrix and multiplies it by the current transform, storing the
	// result back in the current transform.
	scale(current_transform, s[0], s[1], s[2]);

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

int REDirect::rd_camera_eye(const float p[3])
{ // Store the values passed in into the global pointh struct 'eye'.

	eye.x = p[0];
	eye.y = p[1];
	eye.z = p[2];

	return 0;
}

int REDirect::rd_camera_at(const float p[3])
{ // Store globally.

	at.x = p[0];
	at.y = p[1];
	at.z = p[2];

	return 0;
}

int REDirect::rd_camera_up(const float p[3])
{ // Store globally.

	up.x = p[0];
	up.y = p[1];
	up.z = p[2];

	return 0;
}

int REDirect::rd_camera_fov(const float fov)
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

 int REDirect::rd_cube()
 { // Draw each 6 faces of the cube with move-move-move-draw's.

	draw_cube();
	return 0;
 }

 void draw_cube()
 {

	 // Front face
	struct pointh start = { -1.0, -1.0, -1.0 };
	struct pointh next = { 1.0, -1.0, -1.0 };
	struct pointh next2 = { 1.0, 1.0, -1.0 };
	struct pointh next3 = { -1.0, 1.0, -1.0 };
	struct pointh end = { -1.0, -1.0, -1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

	// Back face
	start = { -1.0, -1.0, 1.0 };
	next = { 1.0, -1.0, 1.0 };
	next2 = { 1.0, 1.0, 1.0 };
	next3 = { -1.0, 1.0, 1.0 };
	end = { -1.0, -1.0, 1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

	// Right side
	start = { -1.0, -1.0, 1.0 };
	next = { -1.0, -1.0, -1.0 };
	next2 = { -1.0, 1.0, -1.0 };
	next3 = { -1.0, 1.0, 1.0 };
	end = { -1.0, -1.0, 1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

	// Left side
 	start = { 1.0, -1.0, 1.0 };
	next = { 1.0, -1.0, -1.0 };
	next2 = { 1.0, 1.0, -1.0 };
	next3 = { 1.0, 1.0, 1.0 };
	end = { 1.0, -1.0, 1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

	// Top
	start = { 1.0, -1.0, 1.0 };
	next = { 1.0, -1.0, -1.0 };
	next2 = { -1.0, -1.0, -1.0 };
	next3 = { -1.0, -1.0, 1.0 };
	end = { 1.0, -1.0, 1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

	// Bottom
	start = { -1.0, 1.0, 1.0 };
	next = { 1.0, 1.0, 1.0 };
	next2 = { 1.0, 1.0, -1.0 };
	next3 = { -1.0, 1.0, -1.0 };
	end = { -1.0, 1.0, 1.0 };

	line_pipeline(start, 0); // move
	line_pipeline(next, 0);
	line_pipeline(next2, 0);
	line_pipeline(next3, 0);
	line_pipeline(end, 1); // draw

 }

 int REDirect::rd_sphere(float radius, float zmin, float zmax, float theta)
 {
	 // radius = 1
	 draw_sphere(radius);
	 return 0;
 }

 void draw_sphere(const float radius)
 {
	struct pointh p;

	/* Controlls granularity of detail */
	rowCount = 8;
	columnCount = 8;

	/* Step across rows (x-axis) */
	for (int row = 0; row <= rowCount; row++)
	{
		/* Step across the columns (y-axis) */
		for (int column = 1; column <= columnCount; column++)
		{
			index_to_coords(row, column, p);
			line_pipeline(p, 0);
			p.x = 0; p.y = 0; p.x = 0; p.w = 1;//Reset w

			index_to_coords(row + 1, column, p);
			line_pipeline(p, 0);
			p.x = 0; p.y = 0; p.x = 0; p.w = 1;//Reset w

			index_to_coords(row + 1, column + 1, p);
			line_pipeline(p, 0);
			p.x = 0; p.y = 0; p.x = 0; p.w = 1;//Reset w

			index_to_coords(row, column + 1, p);
			line_pipeline(p, 1);
			p.x = 0; p.y = 0; p.x = 0; p.w = 1;//Reset w
		}
	}
 }

//Convert row and column indices, then store it in a point.
void index_to_coords(int row, int column, pointh& p)
{
	// 1. Calculate phi and theta
	// 2. Calculate x, y, and z
	// 3. Convert ranges of X Y Z to [ -pi/2, pi/2 ] or [ 0, pi/2 ]
	// 4. Return XYZ in the third argument, p

	// 1. Calculate phi and theta based on index's (indices)

	//Convert index's to float
	float col = (float)column;
	float row_f = (float)row;

	// Calculate for theta and phi
	float phi = M_PI * ( row_f / rowCount ) - ( M_PI / 2 ); // 1.57079637
	float theta = 2 * M_PI * ( col / columnCount); // 1.57079637

	// 3. Calculate x, y, and z
	float radius = 1; // hard coded for now.
	float x, y, z;

	x = (radius * cosf(phi)) * cosf(theta);	// 1.91068547e-15
	y = (radius * cosf(phi)) * sinf(theta); // -4.37113883e-08
	z = radius * sinf(phi); // 1

	// 4. Return XYZ in the third argument, p
	p.x = x;
	p.y = y;
	p.z = z;
}

 int REDirect::rd_xform_push()
 {
	 push(current_transform);
	 return 0;
 }

 int REDirect::rd_xform_pop()
 {
	 pop(current_transform);
	 return 0;
 }

 int REDirect::rd_clipping(const float near_input, const float far_input)
 {
	 near = near_input;
	 far = far_input;

	 return 0;
 }

int REDirect::rd_cone(float height, float radius, float thetamax)
{
	// 1. Calculate base circle with trigonometry method
	// 2. Draw the walls
	// 3. Draw the base

	// 1. Calculate base circle with trigonometry method

	// Trigonometry variables
	int numSteps = 10; // Controlls # of steps around the circle
	float degrees = 0;   // [0-360]
	float step = 360 / numSteps;

	// Vector that contains points on the circles.
	vector<pointh> circleBasePoints;
	int a = 0;

	for(int s = 0; s <= numSteps; s++) // d for degrees
	{
		// New point for every loop
		struct pointh p;

		// Tigonometry
		degree_to_radian(degrees);
		p.x = cos(degrees);
		p.y = sin(degrees);
		radian_to_degree(degrees);

		// Call line pipeline
		if (degrees < 360)
		{
			// Store move
			circleBasePoints.push_back(p);
			a++;
		}
		else
		{
			// Store draw
			circleBasePoints.push_back(p);
		}
		degrees = degrees + step;
	}

	// 2. Draw walls of cone

	//Get point for the tip and center of the base circle.
	struct pointh tip, center;
	tip.z = 1;
	struct pointh p1, p2, p3, p4, p5;

	// Drawing the faces of the sides of the cone.
	for(int i = 1; i <= numSteps; i++)
	{
		p1 = circleBasePoints[i];
		p2 = tip;
		p3 = tip;
		p4 = circleBasePoints[i - 1];
		p5 = p1;

		line_pipeline(p1, 0);
		line_pipeline(p2, 0);
		line_pipeline(p3, 0);
		line_pipeline(p4, 0);
		line_pipeline(p5, 1);
	}

	// 3. Draw base of cone

	line_pipeline(circleBasePoints[0], 0);
	line_pipeline(circleBasePoints[1], 0);
	line_pipeline(circleBasePoints[2], 0);
	line_pipeline(circleBasePoints[3], 0);
	line_pipeline(circleBasePoints[4], 0);
	line_pipeline(circleBasePoints[5], 0);
	line_pipeline(circleBasePoints[6], 0);
	line_pipeline(circleBasePoints[7], 0);
	line_pipeline(circleBasePoints[8], 0);
	line_pipeline(circleBasePoints[9], 0);
	line_pipeline(circleBasePoints[0], 1);

	return RD_OK;
}

int findVerticesPerFace(const int face[])
{	// Find how many vertices are in a face before the
	// end of line character (-1) is reached.

	int i = 0;
	int a = face[i];

	while ( a != -1)
	{
		i++;
		a = face[i];
	}

	return i + 1;
}

int REDirect::rd_polyset(const string & vertex_type,
                          int nvertex, const float * vertex_arr,
                          int nface,   const int * face)
{
	// Declare vector for vertices
	vector<vertex> vertices;

	// Declare vector for vertices
	vector<pointh> inFaces;

	struct pointh p1, p2, p3, p4; // Pipeline points
	int nv = 0; // next vertex

	// Build vetices vector from vertex_arr
	for (int i = 0; i < nvertex; i++)
	{
		struct vertex corner;

		corner.x = vertex_arr[nv];
		nv++;
		corner.y = vertex_arr[nv];
		nv++;
		corner.z = vertex_arr[nv];
		nv++;

		vertices.push_back(corner);
	}

	// Drawing vertices.
	int n = findVerticesPerFace(face);
	int faceListSize = (nface * n); // size of face[]
	int inputIndex = 0; // counts vertices

	for (int a = 0; a < faceListSize; a++ )
	{
		// The index of which point the the line
		int cursor = face[a];

		// Put vertex into out inFaces vector to plotting
		if ( cursor != -1 )
		{
			struct vertex v = vertices[cursor]; // Take vertex

			struct pointh p;	// convert to pointh
			copy_p_from_v(p, v);

			inFaces.push_back(p); // Push onto inFace

			inputIndex++;	// incriment counter
		}
		else
		{
			p1 = inFaces[inputIndex - 1];
			p2 = inFaces[inputIndex - 2];
			p3 = inFaces[inputIndex - 3];
			p4 = inFaces[inputIndex - 1]; //Back to the first point.

			line_pipeline(p1, 0);
			line_pipeline(p2, 0);
			line_pipeline(p3, 0);
			line_pipeline(p4, 1);
		}
	}
	return RD_OK;
}

int REDirect::rd_cylinder(float radius, float zmin, float zmax, float thetamax)
{
	// 1. Calculate the base points circle
	// 2. Calculate the roof points circle
	// 3. Draw base
	// 4. Draw roof
	// 5. Draw walls

	// 1. Calculate the base points circle

	// Trigonometry variables
	int numSteps = 10; // Controlls granularity
	float degrees = 0;   // [0-360]
	float step = 360 / numSteps;

	//Array containers and their iterators
	vector<pointh> basePoints;

	int a = 0;

	for(int s = 0; s <= numSteps; s++) // d for degrees
	{
		// Tigonometry
		struct pointh p;
		degree_to_radian(degrees);
		p.x = cos(degrees); // 	These are in radian mode and they need to be degree mode.
		p.y = sin(degrees);	// Same goes for rotation matrix's in matrix.h.
		radian_to_degree(degrees);

		// Call line pipeline
		if (degrees < 360)
		{
			// Store move
			basePoints.push_back(p);
			a++;
		}
		else
		{
			// Store draw
			basePoints.push_back(basePoints[0]);
		}
		degrees = degrees + step;
	}

	// 2. Calculate the roof points circle
	numSteps = 10; // Reset trigonometry variables
	degrees = 0;
	step = 360 / numSteps;

	vector<pointh> roofPoints;
	int b = 0;

	for(int s = 0; s <= numSteps; s++) // d for degrees
	{
		// Tigonometry
		struct pointh p2;
		degree_to_radian(degrees);
		p2.x = cos(degrees);
		p2.y = sin(degrees);
		radian_to_degree(degrees);

		// Store points
		if (degrees < 360)
		{
			// Store move
			roofPoints.push_back(p2);
			b++;
		}
		else
		{
			// Store draw
			roofPoints.push_back(roofPoints[0]);
		}
		degrees = degrees + step;
	}

	// 3. Draw base

	line_pipeline(basePoints[0], 0);
	line_pipeline(basePoints[1], 0);
	line_pipeline(basePoints[2], 0);
	line_pipeline(basePoints[3], 0);
	line_pipeline(basePoints[4], 0);
	line_pipeline(basePoints[5], 0);
	line_pipeline(basePoints[6], 0);
	line_pipeline(basePoints[7], 0);
	line_pipeline(basePoints[8], 0);
	line_pipeline(basePoints[9], 0);
	line_pipeline(basePoints[0], 1);


	// 3. Draw roof
	roofPoints[0].z += 1;
	roofPoints[1].z += 1;
	roofPoints[2].z += 1;
	roofPoints[3].z += 1;
	roofPoints[4].z += 1;
	roofPoints[5].z += 1;
	roofPoints[6].z += 1;
	roofPoints[7].z += 1;
	roofPoints[8].z += 1;
	roofPoints[9].z += 1;

	line_pipeline(roofPoints[0], 0);
	line_pipeline(roofPoints[1], 0);
	line_pipeline(roofPoints[2], 0);
	line_pipeline(roofPoints[3], 0);
	line_pipeline(roofPoints[4], 0);
	line_pipeline(roofPoints[5], 0);
	line_pipeline(roofPoints[6], 0);
	line_pipeline(roofPoints[7], 0);
	line_pipeline(roofPoints[8], 0);
	line_pipeline(roofPoints[9], 0);
	line_pipeline(roofPoints[0], 1);



	// 4. Draw walls
	struct pointh w1, w2, w3, w4, w5;
	for(int i = 0; i <= numSteps; i++)
	{
		if ( i < (numSteps - 1)) // every face
		{
			line_pipeline(basePoints[i], 0);
			line_pipeline(roofPoints[i], 0);
			line_pipeline(roofPoints[i + 1], 0);
			line_pipeline(basePoints[i + 1], 0);
			line_pipeline(basePoints[i], 1);

		}
		else // final face
		{
			line_pipeline(basePoints[0], 0);
			line_pipeline(basePoints[9], 0);
			line_pipeline(roofPoints[9], 0);
			line_pipeline(roofPoints[0], 0);
			line_pipeline(basePoints[0], 1);
		}
	}

	return RD_OK;
}

int countPointsetCalls = 0;

int REDirect::rd_pointset(const string & vertex_type, int nvertex,
		const float * vertex)
{
	countPointsetCalls++;

	// Iterate over the array vertex[] nvertex number of times.

	// Make an array list of points.
	int const nvertx = nvertex;
	// struct pointh pointContain[nvertx];
	vector<pointh> pointContain;

	int indx = 0;

	for(int i = 0; i < nvertx; i++)
	{
		struct pointh p;
		p.x = vertex[indx];
		indx++;
		p.y = vertex[indx];
		indx++;
		p.z = vertex[indx];
		indx++;

		pointContain.push_back(p);
	}

	//Should have a list of points
	cout << " ";
	// Run every point through point pipeline
	for (int i = 0; i < nvertex; i++)
	{
		point_pipeline(pointContain[i]);
	}

	return RD_OK;
}

int REDirect::rd_attribute_push(void)
{
	push(current_transform);
	return RD_OK;
}
int REDirect::rd_attribute_pop(void)
{
	pop(current_transform);
	return RD_OK;
}
// ##
