#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "matrix.h"


// int identity(xform1& m);

/* Here we have more translate functions perscribed by assignment 2.*/

// 6.
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
// Changes m1 by multiplying on the right by a translation matrix formed by 
// tx, ty, and tz.
// The books tell me that a the 1x4 translat. vertex * the 4x4 translat. matrix is a 1x4
// int translate(xform1& m1, double tx, double ty, double tz)
// {
	// struct xform1 trans;
	
	
	// identity(trans);
	// Major prolbem, these function arn't recognized in here but they do in the 
	// child.
	
	
	// m1.n03 = tx;
	// m1.n13 = ty;
	// m1.n23 = tz;
	
	// multiply(m1, xform1 m1, xform1 m2)
	
	
	
	 // cout << m1.n00 << " " << m1.n01 << " " << m1.n02 << " " << m1.n03 << " " << endl;
	 // cout << m1.n10 << " " << m1.n11 << " " << m1.n12 << " " << m1.n13 << " " << endl;
	 // cout << m1.n20 << " " << m1.n21 << " " << m1.n22 << " " << m1.n23 << " " << endl;
	 // cout << m1.n30 << " " << m1.n31 << " " << m1.n32 << " " << m1.n33 << " " << endl;
	
	
	// return 0;
// } 

#endif