# rd_view: Computer graphics engine in c++.

**Sample output**

Look in the folder 'Output gallery'

**Usage**

Mac users: works, no need to install extra packages

Windows users: follow these cygwin installation [instructions](http://graphics.cs.niu.edu/csci631/cyginstall.html) 

**Description**

This source code for a computer graphics rendering engine written in C++. This program impliments some of fundementels of
computer graphics. Notable contents are listed below. The jist of this program is that it renders a collection of primitives
using the University of Utah style old school transformation pipeline described in the file above named 'High level overview' . This is a direct rendering computer graphics engine as opposed
to a ray tracing computer graphics engine. For more information about the language, visit this courses [homepage](http://graphics.cs.niu.edu/).

**Noteable 2D algorithms**
* 2D four-connected fill algorithm named the Four Fast Flood Fill in rd_direct.cc on line 118
* Bresenhams line algorithm in rd_direct.cc on line number 499

**Noteable 3D algorithms**
* Transformation line pipeline in rd_direct.cc on line 368
* Transformation point pipeline in rd_direct.cc on line 455
* Sphere primitive in C++ in rd_direct.cc on line 920. Speical thanks to https://www.songho.ca/opengl/gl_sphere.html
* Cone primitive in C++ in rd_direct.cc on line 1005
* Polyset in C++ (arbritary polygon) in rd_direct.cc on line 1104
* Cylinder primitive in C++ in rd_direct.cc line 1170
* Poinset in C++ (arbritary set of points) in rd_direct.cc on line 1315

**Noteable linear algebra functions**
* Declaring vectors and various matrices as structs in matrix.h at top of file
* World to Camera transformation matrix on in matrix.h on line 555
* Camera to Clip transformation matrix on in matrix.h on line 604
* Clip to Device transformation matrix on in matrix.h on line 673




