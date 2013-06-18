/* badPixels.cpp
 *
 * Takes decoded color image (cresult.ppm) and creates new image where out-of-place pixels are
 *   highlighted by comparing the Saturation and Value with previous pixels in the line.  
 * Name: Greg Krathwohl
 */

static const char *usage = "usage: %s [-8] in.png [out.png]\n\
  option -8 uses 8 neighbors (4 by default)\n";

#include "imageLib.h"
#define VERBOSE 1


static void RGB2HSV(float r, float g, float b,
                    float &h, float &s, float &v)
{
    float rgb_max = std::max(r, std::max(g, b));
    float rgb_min = std::min(r, std::min(g, b));
    float delta = rgb_max - rgb_min;
    s = delta / (rgb_max + 1e-20f);
    v = rgb_max;

    float hue;
    if (r == rgb_max)
        hue = (g - b) / (delta + 1e-20f);
    else if (g == rgb_max)
        hue = 2 + (b - r) / (delta + 1e-20f);
    else
        hue = 4 + (r - g) / (delta + 1e-20f);
    if (hue < 0)
        hue += 6.f;
    h = hue * (1.f / 6.f);
}


// finds BAD Pixels
void bad(CByteImage &in, CByteImage &out)
{
    CShape sh = in.Shape();
    int w = sh.width, h = sh.height;
    CShape sh2(w, h, 4);
    out.ReAllocate(sh2);

	//previous, next, current intensity
    int pi, ni, ci, asurr, up, down, left, right, level, plevel, pr, pg, pb, psum;
    
    level = 0;
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
			out.Pixel(x,y,3) = 255;

			//ci = in.Pixel(x,y,0) + in.Pixel(x,y,1) + in.Pixel(x,y,2);
			//pi = in.Pixel(x-1,y,0) + in.Pixel(x-1,y,1) + in.Pixel(x-1,y,2);
			//ni = in.Pixel(x+1,y,0) + in.Pixel(x+1,y,1) + in.Pixel(x+1,y,2);

			//left = in.Pixel(x-1, y, 0) + in.Pixel(x-1, y, 1) + in.Pixel(x-1, y, 2);
			//right = in.Pixel(x+1, y, 0) + in.Pixel(x+1, y, 1) + in.Pixel(x+1, y, 2);
			//down = in.Pixel(x, y+1, 0) + in.Pixel(x, y+1, 1) + in.Pixel(x, y+1, 2);
			//up = in.Pixel(x, y-1, 0) + in.Pixel(x, y-1, 1) + in.Pixel(x, y-1, 2);
			
			
			
			//if (y == 1000 && x % 30 == 0){
				float h, s, v;
				int r, g, b;
				r = in.Pixel(x,y,0);
				g = in.Pixel(x,y,1);
				b = in.Pixel(x,y,2);
				//printf("%d, %d, %d\n", r, g, b);

				RGB2HSV(r, g, b, h, s, v);
				//printf("%f, %f, %f\n", h*360, s, v/255);
				
			//	printf("x %d, y %d ", x, y);
		
				pr = in.Pixel(x-1,y,0);
				pg = in.Pixel(x-1,y,1);
				pb = in.Pixel(x-1,y,2);
				psum = pr + pg + pb;
				if (level != 0 && psum != 255*3)
					plevel = level;

				//level equals saturation + value
				level = s + v;
				
				//if the level is different from the previous level 
				if (abs(plevel - level) > 60 && level != 0){
					out.Pixel(x,y,0) = 255;
					out.Pixel(x,y,1) = 255; 
					out.Pixel(x,y,2) = 255;  
				//	printf("%d\n", level);
				}
				else {
					out.Pixel(x,y,0) = 0;
					out.Pixel(x,y,1) = 0; 
					out.Pixel(x,y,2) = 0;   
				}
				

				//printf("level: %d\n", level);
		//	}
			
			
			/*

			asurr = up + down + left + right;

			//if (abs((ci - pi) + (ci - ni)) > 900 && ci != 255 * 3) {
			if (in.Pixel(x,y,0) == 255 || in.Pixel(x,y,1) == 255 || in.Pixel(x,y,2) == 255) {		
				out.Pixel(x,y,0) = 255;
				out.Pixel(x,y,1) = 255; 
				out.Pixel(x,y,2) = 255;  
			} 
			else {
				out.Pixel(x,y,0) = 0;
				out.Pixel(x,y,1) = 0; 
				out.Pixel(x,y,2) = 0;   
			}
			*/	

			//if (in.Pixel(x,y,1) == 0){
				//out.Pixel(x,y,1) = 200;
			//} 
        }
    }
}


int main(int argc, char *argv[])
{
    try {
	// parse command line
	if (argc < 2 || argc > 4)
	    throw CError(usage, argv[0]);
	int argn = 1;
	int numNeighbors = 4; // default
	// check whether '-8' flag was given
	if (argv[argn][0] == '-' && argv[argn][1] == '8') {
	    numNeighbors = 8;
	    argn++;
	}
	char *inFileName = argv[argn++];
	char *outFileName = NULL;
	if (argn < argc)
	    outFileName = argv[argn++];
	if (argn != argc)
	    throw CError(usage, argv[0]);

	CByteImage img, out, outimg;
	
	
	ReadImageVerb(img, inFileName, VERBOSE);
	
	if (outFileName != NULL) {
	    bad(img, outimg);
	    WriteImageVerb(outimg, outFileName, VERBOSE);
	}
    }
    catch (CError &err) {
	fprintf(stderr, err.message);
	fprintf(stderr, "\n");
	return -1;
    }

    return 0;
}
