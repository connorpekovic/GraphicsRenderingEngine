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
};

#endif /* RD_ENGINE_DIRECT_H */
