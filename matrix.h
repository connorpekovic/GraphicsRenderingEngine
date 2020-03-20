#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <cmath>
#include <stack>

// Function prototypes


using namespace std;

// Define struts
// Represents a Cartesian coordinate point.
struct cartePoint {
	float x = 0;
	float y = 0;
	float z = 0;
};

// Represents a homogeneous coordinate point.
struct pointh {
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
};

// Represents a homogeneous coordinate point.
struct vec {
	float x = 0;
	float y = 0;
	float z = 0;
};

// Represents the transformation matrix.
// Read the nomenclature "number row column"
struct xform1 {
	float n00 = 0; float n01 = 0; float n02 = 0; float n03 = 0;
	float n10 = 0; float n11 = 0; float n12 = 0; float n13 = 0;
	float n20 = 0; float n21 = 0; float n22 = 0; float n23 = 0;
	float n30 = 0; float n31 = 0; float n32 = 0; float n33 = 1;
	// ? Does n33 need to be initalized to 1 or will the identity
	// take care of that when needed?
	// Came up with this when doing camera_to_clip.  I changed n33
	// to 0, so certinly walk though the program and look.
};

// Represnts the transformation stack.
stack<xform1> transform_stack;

//  1.
//  Multiplies a scalar by a point and returns a point.
int multiply(pointh &p_result, float scalar, pointh &p_input)
{
	p_result.x = p_input.x * scalar;
	p_result.y = p_input.y * scalar;
	p_result.z = p_input.z * scalar;
	p_result.w = p_input.w * scalar;
	
	return 0;	
}

//  2.
//  Multiplies a point by a scalar and returns a point. 
int multiply(pointh &p_result, pointh &p_input, float scalar)
{
	p_result.x = p_input.x * scalar;
	p_result.y = p_input.y * scalar;
	p_result.z = p_input.z * scalar;
	p_result.w = p_input.w * scalar;
	
	return 0;
}

// 3. 
// Multiplies a scalar by a vector and returns a vector.
int multiply(vec &result, float scalar, vec& input)
{
	result.x = input.x * scalar;
	result.y = input.y * scalar;
	result.z = input.z * scalar;
	
	return 0;
}

// 4.
// Multiplies a vector by a scalar and returns a vector.
int multiply(vec& result, vec& input, float scalar)
{
	result.x = input.x * scalar;
	result.y = input.y * scalar;
	result.z = input.z * scalar;

	return 0;
}

// 5.
// Copies a source point data structure into a destination point data structure.	
int copy(pointh& result, pointh& input)
{
	result.x = input.x;
	result.y = input.y;
	result.z = input.z;
	result.w = input.w;
	
	return 0;
}

// 6.
// Copies a source vector data structure into a destination vector data structure.	
int copy(vec& result, vec source)
{
	result.x = source.x;
	result.y = source.y;
	result.z = source.z;
	
	return 0;
}

// 6a.
// Copies a source 4x4 struct into a destination 4x4.	
int copy_matrix(xform1& result, xform1& source)
{
	// First row
	result.n00 = source.n00;
	result.n01 = source.n01;
	result.n02 = source.n02;
	result.n03 = source.n03;
	
	// Second row
	result.n10 = source.n10;
	result.n11 = source.n11;
	result.n12 = source.n12;
	result.n13 = source.n13;
	
	// Third row
	result.n20 = source.n20;
	result.n21 = source.n21;
	result.n22 = source.n22;
	result.n23 = source.n23;
	
	// Fourth row
	result.n30 = source.n30;
	result.n31 = source.n31;
	result.n32 = source.n32;
	result.n33 = source.n33;
	
	return 0;
}

// 7.
// Computes the dot product of v1 and v2 and passes the 
// product back by refrence in the first argument.
int dot(float* scalar, vec vec1, vec vec2)
{
	// Working variables
	float answer = 0;
	
	//How many numbers are in a matrix, for real?
	answer += vec1.x * vec2.x;
	answer += vec1.y * vec2.y;
	answer += vec1.z * vec2.z;
	
	//Pass answer back by refrenece.
	*scalar = answer;
	
	return 0;
}

// 8. Cross product.
// Computes the cross product of v1 and v2 and returns the vector value.
int cross(vec& result, vec vec1, vec vec2)
{
	result.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
	result.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
	result.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);

	return 0;
}

// 9. 
// Computes the magnitude squared of vector v and returns the scalar value.
int mag2(float* magnitude, vec& vec1)
{
	float mag[3]; //How large is a vector?
	int sum = 0;
	
	mag[0] = vec1.x * vec1.x;
	mag[1] = vec1.y * vec1.y;
	mag[2] = vec1.z * vec1.z;
	
	// 3. Sum the contents of mag[].
	for (int b = 0; b < 3; b++ )
	{
		sum += mag[b];
	}
	
	*magnitude = sum;
	
	return 0;
}

//  10.
//  Fills the vector with the a normorlized homogeneous point.
int normalize(vec& vec1)
{
	float mag = 0;
	mag2(&mag, vec1);
	mag = sqrt(mag);
	
	vec1.x /= mag;  
	vec1.y /= mag;
	vec1.z /= mag;
	
	return 0;
}

//  10b.
//  normalizes a homogenus point (pointh) and return it represented
//  as a vector.
int normalize_homog_to_vec(vec& vec1, pointh p)
{
	vec1.x = p.x / p.w;  
	vec1.y = p.y / p.w;
	vec1.z = p.z / p.w;
	
// Conventions say to represent vectors as [1x4]'s with a 4th
// position set equal to 1. In the book vol 1, page 7 shows 
// a vector struct defined with three values, so I'm copying
// the books strategy for now.  For all intensive purposes,
// the w value is 1.
	
	return 0;
}

// 11. 
// Adds two vectors together and returns the vector result.
int add(vec& result, vec vec1, vec vec2)
{
	result.x = vec1.x + vec2.x;
	result.y = vec1.y + vec2.y;
	result.z = vec1.z + vec2.z;
	
	return 0;
}

// 12. 
// Subtracts vector vec2 from vec1 and returns the vector result.
int subtract(vec& result, vec vec1, vec vec2)
{
	result.x = vec1.x - vec2.x;
	result.y = vec1.y - vec2.y;
	result.z = vec1.z - vec2.z;

	return 0;
}

// 12a.
// Subtract's 2 homogenius points.
int subtract(pointh& result, pointh p1, pointh p2)
{
	result.x = p1.x - p2.x;
	result.y = p1.y - p2.y;
	result.z = p1.z - p2.z;
	result.w = p1.w - p2.w;
	
	return 0;
}

// 13.
// Subtracts point p1 from point p2 to create a vector that is the returned result.
// (Question) Only cartesian points can be subtracted to become vectors?
//  Yes: we're good,
//  No: Homog points have 4 values and a vec has 3.  What should be,
//  	be done with the homog point w?
int subtract(vec& result, cartePoint p1, cartePoint p2)
{
	result.x = p1.x - p2.x;
	result.y = p1.y - p2.y;
	result.z = p1.z - p2.z;

	return 0;
}

// 14. 
// Converts cartesian point into a homogeneous point by just adding a size of 1 to w.
int convert(pointh& hpoint, cartePoint cpoint)
{
	hpoint.x = cpoint.x;
	hpoint.y = cpoint.y;
	hpoint.z = cpoint.z;
	hpoint.w = 1;

	return 0;
}

// 15.
// Converts a vector into a homogeneous point.
int convert(pointh& hpoint, vec vec1)
{
	hpoint.x = vec1.x;
	hpoint.y = vec1.y;
	hpoint.z = vec1.z;
	hpoint.x = 1;

	return 0;
}

// Transformations

// 1.
// Returns the identity matrix.
int identity(xform1& m)
{	
	m.n00 = 1; m.n01 = 0; m.n02 = 0; m.n03 = 0;
	m.n10 = 0; m.n11 = 1; m.n12 = 0; m.n13 = 0;
	m.n20 = 0; m.n21 = 0; m.n22 = 1; m.n23 = 0;
	m.n30 = 0; m.n31 = 0; m.n32 = 0; m.n33 = 1;

	return 0;
}

// 2.
// Multiplies m1 by m2 and returns the resulting transformation.
int multiply(xform1& result, xform1 m1, xform1 m2)
{
	// First row
	result.n00 = (m1.n00 * m2.n00) + (m1.n01 * m2.n10) + (m1.n02 * m2.n20) + (m1.n03 * m2.n30);
	result.n01 = (m1.n00 * m2.n01) + (m1.n01 * m2.n11) + (m1.n02 * m2.n21) + (m1.n03 * m2.n31);
	result.n02 = (m1.n00 * m2.n02) + (m1.n01 * m2.n12) + (m1.n02 * m2.n22) + (m1.n03 * m2.n32);
	result.n03 = (m1.n00 * m2.n03) + (m1.n01 * m2.n13) + (m1.n02 * m2.n23) + (m1.n03 * m2.n33);
	
	// Second row
	result.n10 = (m1.n10 * m2.n00) + (m1.n11 * m2.n10) + (m1.n12 * m2.n20) + (m1.n13 * m2.n30);
	result.n11 = (m1.n10 * m2.n01) + (m1.n11 * m2.n11) + (m1.n12 * m2.n21) + (m1.n13 * m2.n31);
	result.n12 = (m1.n10 * m2.n02) + (m1.n11 * m2.n12) + (m1.n12 * m2.n22) + (m1.n13 * m2.n32);
	result.n13 = (m1.n10 * m2.n03) + (m1.n11 * m2.n13) + (m1.n12 * m2.n23) + (m1.n13 * m2.n33);
	
	// Third row
	result.n20 = (m1.n20 * m2.n00) + (m1.n21 * m2.n10) + (m1.n22 * m2.n20) + (m1.n23 * m2.n30);
	result.n21 = (m1.n20 * m2.n01) + (m1.n21 * m2.n11) + (m1.n22 * m2.n21) + (m1.n23 * m2.n31);
	result.n22 = (m1.n20 * m2.n02) + (m1.n21 * m2.n12) + (m1.n22 * m2.n22) + (m1.n23 * m2.n32);
	result.n23 = (m1.n20 * m2.n03) + (m1.n21 * m2.n13) + (m1.n22 * m2.n23) + (m1.n23 * m2.n33);
	
	// Fourth row
	result.n30 = (m1.n30 * m2.n00) + (m1.n31 * m2.n10) + (m1.n32 * m2.n20) + (m1.n33 * m2.n30);
	result.n31 = (m1.n30 * m2.n01) + (m1.n31 * m2.n11) + (m1.n32 * m2.n21) + (m1.n33 * m2.n31);
	result.n32 = (m1.n30 * m2.n02) + (m1.n31 * m2.n12) + (m1.n32 * m2.n22) + (m1.n33 * m2.n32);
	result.n33 = (m1.n30 * m2.n03) + (m1.n31 * m2.n13) + (m1.n32 * m2.n23) + (m1.n33 * m2.n33);

	return 0;
}

//  3.
//	Multiplies m1 by p1 on the right and returns the resulting homogeneous point. 
int multiply(pointh &result, xform1 m1, pointh p1)
{
	result.x = (p1.x * m1.n00) + (p1.y * m1.n10) + (p1.z * m1.n20) + (p1.w * m1.n30);
	result.y = (p1.x * m1.n01) + (p1.y * m1.n11) + (p1.z * m1.n21) + (p1.w * m1.n31);
	result.z = (p1.x * m1.n02) + (p1.y * m1.n12) + (p1.z * m1.n22) + (p1.w * m1.n32);
	result.w = (p1.x * m1.n03) + (p1.y * m1.n13) + (p1.z * m1.n23) + (p1.w * m1.n33);

	return 0;
}

// // Why do I think I'm doing 4 and 5 wrong?

// 4.
// Pushes the given transform onto a stack of transforms.
int push(xform1 m1)
{
	transform_stack.push(m1);
	
	return 0;
}

// 5.
// Returns a transformation from the top of the transformation stack.
int pop(xform1& m1)
{
	m1 = transform_stack.top();
	transform_stack.pop();
	
	return 0;
}

// 6.
// Changes m1 by multiplying on the right by a translation matrix formed by 
// tx, ty, and tz.
int translate(xform1& m1, double tx, double ty, double tz)
{// This i is done by taking an identity matrix and putting
//	 x, y, z, and 1 in the right column then multiplying it by the
//   current transformation matrix.
//	 Declare working vars.
	struct xform1 transformMatrix, translationMatrix;
	
	// Load the transform matrix.
	copy_matrix(transformMatrix, m1);
	
	// Load the vertex || translation matrix?
	identity(translationMatrix);	
	translationMatrix.n03 = tx;
	translationMatrix.n13 = ty;
	translationMatrix.n23 = tz;
		
	// Multiplies 4 x 4 matrices, returns by pass by refrence to m1.
	multiply(m1, transformMatrix, translationMatrix);
	
	return 0;
} 

	// Questions:
	//Scale matrices have a [4][4] of 0?
	// also: The main translation matrix always has a 1 in poistion [4][4], yeah?


// 7.
// Changes transMatrix by multiplying on the right by a scale matrix formed 
//  by sx, sy, and sz.
int scale(xform1& mainM, double sx, double sy, double sz)
{
	// Declare working variables.
	struct xform1 transformMatrix, scaleM;
	
	// Transformation matrix is a temp variable.
	copy_matrix(transformMatrix, mainM);
	
	// Prepare the scale matrix
	identity(scaleM);
	scaleM.n00 = sx;
	scaleM.n11 = sy; 
	scaleM.n22 = sz; 
	
	// *	
	// Tranformation matrix = transformation matrix * scale matrix
	multiply(mainM, transformMatrix, scaleM);
	
	// Propare the scale matrix.
	return 0;
}

// 8.
// Changes m1 by multiplying on the right by a rotation matrix using rotation of x towards y. Theta is given in degrees.
int rotate_xy(xform1& mainM, double theta)
{
	struct xform1 transformM, rotateM;
	
	//Prepare working transformation matrix
	copy_matrix(transformM, mainM);
	
	//Pepare XY rotation matrix
	identity(rotateM);
	rotateM.n00 = cos(theta);
	rotateM.n01 = (sin(theta)) * -1;
	rotateM.n10 = sin(theta);
	rotateM.n11 = cos(theta);
	
	multiply(mainM, transformM, rotateM);

	return 0;
}
#endif

// 9.
// Changes m1 by multiplying on the right by a rotation matrix using 
//  rotation of y towards z. Theta is given in degrees.
int rotate_yz(xform1& mainM, double theta)
{
	struct xform1 transformM, rotateM;
	
	copy_matrix(transformM, mainM);
	
	// Prepare YZ rotation
	identity(rotateM);
	rotateM.n11 = cos(theta);
	rotateM.n21 = sin(theta);
	rotateM.n12 = (sin(theta)) * -1;
	rotateM.n22 = cos(theta);

	multiply(mainM, transformM, rotateM);
	
	return 0;
}

// 10.
// Changes m1 by multiplying on the right by a rotation matrix using 
//	rotation of z towards x. Theta is given in degrees.
int rotate_zx(xform1& mainM, double theta)
{
	struct xform1 transformM, rotateM;
	
	copy_matrix(transformM, mainM);
	
	// Prepare ZX rotation
	identity(rotateM);
	rotateM.n00 = cos(theta);
	rotateM.n20 = (sin(theta)) * -1;
	rotateM.n02 = sin(theta);
	rotateM.n22 = cos(theta);
	
	multiply(mainM, transformM, rotateM);

	return 0;
}

// 11.
//  World to Camera
int world_to_camera(xform1& mainM, pointh eye, pointh atIn, vec up)
{
	
	// 1. At point = Atpoint - Eyepoint	    ( Camera z )
	// 2. Normalize At
	// 3. V = At x Up						( Camera x )
	// 4. Normalize V
	// 5. U = V x At 						( Camera y )
	// 6. Construct two special 4x4 from the answers 
	//	  gathered in parts (1-5).
	// 	6a. Construct 4x4 for U/V/At matrix.
	// 	6b. Construct 4x4 for Eye point.
	// 7. Multiply U/V/At by the eye point matrix.
	// 8. Multiply that result by the current transformation matrix.
	// 9. Return result by loading mainM.

	struct pointh AtHomog;
	struct vec At;
	struct vec V;
	struct vec U;

	// 1. 
	// atIn  	-> Input At point 1x4.
	// AtHomog 	-> Temp at 1x4.
	// At 		-> Resulting At point 1x4.
	// At point = Atpoint - Eyepoint	
	subtract(AtHomog, atIn, eye);

	// 2.
	normalize_homog_to_vec(At, AtHomog);
	
	// 3. V = At x Up						
	cross(V, At, up);
	
	// 4. Normalize V
	normalize(V);
	
	// 5. U = V x At
	cross(U, V, At);
	
	// 6a
	struct xform1 VUAt;
	identity(VUAt);
	VUAt.n00 = V.x; VUAt.n01 = V.y; VUAt.n02 = V.z;
	VUAt.n10 = U.x; VUAt.n11 = U.y; VUAt.n12 = U.z;
	VUAt.n20 = At.x; VUAt.n21 = At.y; VUAt.n22 = At.z;
	
	// 6b
	struct xform1 Eye4x4;
	identity(Eye4x4);
	Eye4x4.n03 = eye.x * -1;
	Eye4x4.n13 = eye.y * -1;
	Eye4x4.n23 = eye.z * -1;
	
	struct xform1 temp1;
	struct xform1 temp2;
	
	// 7.
	multiply(temp1, VUAt, Eye4x4);
	
	// 8.
	multiply(temp2, temp1, mainM);
	
	// 9.
	copy_matrix(mainM, temp2);
	
	return 0;
}

/* Returns a transformation to go from camera coordinates to normalized clipping coordinates (x, y, z all 
 between 0 and 1). The vertical field of view of the camera is given by fov in degrees. Near and far are
 z distances of the near and far clipping planes respectively. Aspect is the ratio of horizontal to
 vertical size of the display device. This routine assumes that the viewing window is centered about the
 optical axis of the camera. */
 
int camera_to_clip(xform1& mainM, double fov, double near, double far, double aspect)
{ // This function returns just the transformation matrix for camera_to_clip	

	// Declare working 4x4
	struct xform1 trans_camera_clip, result;
		
	/* Intrestingly trig function accecpt radians and
		return degrees. This function:
		1. Converts degrees to radians
		2. Does tangest function.
		3. Loads 4x4 matrix. */
	
	double FOV = fov / 2; // part of formula for g
	
	// 1. Converts degrees to radians
	FOV =  FOV * (M_PI / 180);
	
	// 2. Does tangest function.
	double temp = tan(FOV);
	
	// 3. Loads 4x4 matrix. 
	double g = 1 / FOV;  // Warning, this product is unexpainably
						 // different than what happens on my 
						 // TI-83.  Off by like .04 when fov = 45.
	double s = aspect;
	double n = near;
	double f = far;

	// Load 4x4 with values reflecting the diagram.
	trans_camera_clip.n00 = g / s;
	trans_camera_clip.n11 = g;
	trans_camera_clip.n22 = f / (f - n);
	trans_camera_clip.n23 = -1 * ( (n*f)/(f*n) ); // (n*f)/(f*n) always evaluates to 1. Is that correct?
	trans_camera_clip.n32 = 1;
	
	multiply(result, mainM, trans_camera_clip);
	
	copy_matrix(mainM, result);
	
	return 0;
}

// Returns a transformation to go from normalized clipping coordinates to 
// device coordinates. Width and height are the size (in pixels) of the 
// final viewing device.
int clip_to_device(xform1& mainM, int width, int height)
{	
	struct xform1 result;
	
	identity(result);
	
	result.n00 = width;
	result.n11 = height * -1;
	result.n13 = height	;

	multiply(result, mainM, result);

	copy_matrix(mainM, result);
	
	return 0;
}

// You should implement two graphics pipelines, one for points and one for lines.
