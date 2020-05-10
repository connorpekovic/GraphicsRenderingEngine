#ifndef PNM_DISPLAY
#define PNM_DISPLAY

/* 
 *  File name: pnm_display.cc
 *	
 * Purpose: Functions that access and can manipulate display array which is implimented neatly with
 *	as a pseudo 3 dimentional array. */

 #include "rd_error.h"
 #include "pnm_display.h"
 #include "rd_display.h"

 #include <algorithm> 
 #include <string>
 #include <iostream>
 #include <fstream>

using namespace std; 
/*
List of concerns about this page.
1. Why must I use extern for display_xSize to be recognixed, it's initalized in pnm_display.h.
2. int pnm_read_pixel(); We can't return the pixle value as an int? It needs to be a float.
*/
	static int frameNumber = 0;
	
	//const char * display_name;

	// Declarations
	float * displayArray; 	   // Display array
	
	extern int display_xSize;  // Only recognized when 'extern' is used, very odd.
	extern int display_ySize;  //
	int rgbStorage = 3; 	   // I just hard code a 3 everywhere anyways.
	float backgroundColor[3] = {0, 0, 0};  // Initalized in pnm_display.h

/* ]dynamically allocateing memory sufficiently large to hold the image of the given size. */
  int pnm_init_display(void)
  {
	  // Is it okay hardcode the third operand as 3?
	displayArray = new float [display_xSize * display_ySize * 3];

	return RD_OK;
  }

  /* This reads value of a red, green or blue value from a pixels. */
  int index(int x, int y, int rgbIndex)
  { 
	  int rgbSize = 3;
	  int index = (((x * display_ySize) + y) * rgbSize + rgbIndex);
	  //int index = ((( x * display_xSize ) + y * display_ySize) + rgbIndex);
	  //int index = ((x * y) + rgbIndex);
  	  return index;
  } 

 /* Called when the input file is finished. Free up the dynamically allocated image memory here.*/
  int pnm_end_display(void)
  {
	delete [] displayArray;
	return RD_OK;
  }

  /* Purpose:  Get ready for a new image by setting every pixel to the desingated background color.
	 Save the frame number into a global location by choosing to incriment it here. */
  int pnm_init_frame(int globalFrameNumber)
  {
	//Loopty loop and close.
	for (int y = 0; y < display_ySize; y++)
	{
		for (int x = 0; x < display_xSize; x++)
		{
			
			int red = index(x, y, 0);
			int green = index(x, y, 1);
			int blue = index(x, y, 2);
			
			displayArray[red] = backgroundColor[0];
			displayArray[green] = backgroundColor[1];
			displayArray[blue] = backgroundColor[2];
		}
	}
	
	globalFrameNumber = frameNumber;
	
	frameNumber++;

	 return RD_OK;
  } 
  
  /* Surprisingly, this is the most involved routine. When this is called, the image is
		finished and can be written out to storage. The file name is found in the global
		variable display_name which must be modified by the frame number in any way you
		see fit to create a unique file name. Appending works well. Output a standard PPM
		file from the global image data. */

 /* We are going to output a new file here from the contents of the displayArray, I think.*/
  int pnm_end_frame(void)
  {
	/* Manipulating the file name using the working variable 'fileName' and appending the frame number 
	   using the to_string function from the <string> library. */
	string fileName = display_name;
	fileName += to_string(frameNumber);
	fileName += ".ppm";
	
	// Output file name is 'out'.
	ofstream out;
	out.open(fileName, ios::binary);
	
	/* Writing out to a .pmm */
	/* Start out with the magic, then putting the rgb information in it. */
	out << "P6\n" << display_xSize << " " << display_ySize << "\n255\n";

	//Loopty loop and close.
	for (int y = 0; y < display_ySize; y++)
	{
		for (int x = 0; x < display_xSize; x++)
		{
			int red_index = index(x, y, 0);
			int green_index = index(x, y, 1);
			int blue_index = index(x, y, 2);
			
			float red = displayArray[red_index];
			float green = displayArray[green_index];
			float blue = displayArray[blue_index];
			
			red = red * 255.99;
			green = green * 255.99;
			blue = blue * 255.99;
			
			// Does it need to be display
			out.put(red);
			out.put(green);
			out.put(blue);
			
		}
	}

	out.close();

	return RD_OK;
  }
  
  /* Set the image value at location(x, y) to the value of the current color. Don't 
	 forget any necessary intensity range conversion. */
  int pnm_write_pixel(int x, int y, const float rgb [])
  {		
	/* Take index of RGB I want to write to */
	int red_index = index(x, y, 0);
	int green_index = index(x, y, 1);
	int blue_index = index(x, y, 2);
	
	
	/* Load those locations with input RGB.*/
	displayArray[red_index] = rgb[0];
	displayArray[green_index] = rgb[1];
	displayArray[blue_index] = rgb[2];
	
	return RD_OK;
  }

  /* Read the value from location(x, y) in the array to the red, green, and blue values
  passed in. Make sure that the output values are in the range 0.0 to 1.0.*/
  int pnm_read_pixel(int x, int y, float rgb[])
  {  
	
	// The index location is an int
	int red_index = index(x, y, 0);
	int green_index = index(x, y, 1);
	int blue_index = index(x, y, 2);
	
	// The value as a float
	float r = displayArray[red_index];
	float g = displayArray[green_index];
	float b = displayArray[blue_index];
	
	// Storing our answer in the parray that was passed in.
	rgb[0] = r;
    rgb[1] = g;
	rgb[2] = b;
	
	return RD_OK;
  }

  /* Store the color values in some global location as the current background color. 
	 The default background color should be black. */
  int pnm_set_background(const float rgb [])
  {
	backgroundColor[0] = rgb[0];
	backgroundColor[1] = rgb[1];
	backgroundColor[2] = rgb[2];
	
	return RD_OK;

  }	

  /*(Re)initialize the image array to the background color.*/
  int pnm_clear(void)
  {
	//Loop through the display array.
	for (int y = 0; y < display_ySize; y++)
	{
		for (int x = 0; x < display_xSize; x++)
		{
			int red = index(x, y, 0);
			int green = index(x, y, 1);
			int blue = index(x, y, 2);
			
			displayArray[red] = backgroundColor[0];
			displayArray[green] = backgroundColor[1];
			displayArray[blue] = backgroundColor[2];
			
		}
	}
	return RD_OK;
  }


#endif
