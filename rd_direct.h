#ifndef RD_ENGINE_DIRECT_H
#define RD_ENGINE_DIRECT_H

/* Filename: rd_direct.h */

#include "rd_enginebase.h"
#include "rd_display.h"
#include "pnm_display.h"

#include <string>
using std::string;


// This is a rendering engine that renders directly to the image buffer as
// primitives come in.  A depth buffer is obviously needed.  Transparency is
// not properly handled.

class REDirect: public RenderEngine
{
 public:
	/* General functions */
 	virtual int rd_display(const string & name, const string & type,
       		const string & mode);
	virtual int rd_format(int xresolution, int yresolution);
	virtual int rd_frame_begin(int frame_no);
	virtual int rd_world_begin(void);
	virtual int rd_world_end(void);
	virtual int rd_frame_end(void);

	
	/* Lighting and Shading*/
	virtual int rd_color(const float color[]);
	virtual int rd_background(const float color[]);
	virtual int rd_fill(const float seed_point[3]);
	
	/* Geomatric objects*/
	virtual int rd_point(const float p[3]);
	virtual int rd_line(const float start[3], const float end[3]);
	virtual int rd_circle(const float center[3], float radius);
	virtual int rd_cube();
	virtual int rd_sphere(float radius, float zmin, float zmax, float theta);
	virtual int rd_polyset(const string & vertex_type,
                          int nvertex, const float * vertex_arr,
                          int nface, const int * face);
	virtual int rd_cone(float height, float radius, float thetamax);
	virtual int rd_cylinder(float radius, float zmin, float zmax, float thetamax);
	virtual int rd_pointset(const string & vertex_type, int nvertex, 
		const float * vertex);
	
	/* Assignment 2 */
	virtual int rd_translate(const float p[3]);
	virtual int rd_scale(const float p[3]);
	virtual int rd_rotate_xy(const float theta);
	virtual int rd_rotate_yz(const float theta);
	virtual int rd_rotate_zx(const float theta);
	virtual int rd_camera_eye(const float p[4]);
	virtual int rd_camera_at(const float p[4]);
	virtual int rd_camera_up(const float p[4]);
	virtual int rd_camera_fov(const float fov);
	virtual int rd_clipping(const float p[2]);
	virtual int rd_xform_push();
	virtual int rd_xform_pop();
	virtual int rd_clipping(const float near, const float far);
	virtual int rd_attribute_push(void);
	virtual int rd_attribute_pop(void);
	
};

#endif /* RD_ENGINE_DIRECT_H */
