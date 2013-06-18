/* badPixels.cpp
 *
 * Takes decoded color image (cresult.ppm) and creates new image where out-of-place pixels are
 *   highlighted by comparing the Saturation and Value with previous pixels in the line.  
 * Name: Greg Krathwohl
 */

static const char *usage = "usage: %s in.png out.png\n";

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


// finds bad Pixels
void bad(CByteImage &in, CByteImage &out)
{
    CShape sh = in.Shape();
    int w = sh.width, h = sh.height;
    CShape sh2(w, h, 4);
    out.ReAllocate(sh2);

	//calculate the sum of Saturation and Value for each pixel, store in 'level' image.
    CByteImage levels;
	levels.ReAllocate(sh2);
    int level = 0;	
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
			float h, s, v;
			int r, g, b;
			r = in.Pixel(x,y,0);
			g = in.Pixel(x,y,1);
			b = in.Pixel(x,y,2);

			//get HSV
			RGB2HSV(r, g, b, h, s, v);

			//level equals saturation + value
			level = s + v;

			levels.Pixel(x,y, 0) = level;
        }
    }

	//create image highlighting 'bad' pixels, where the level stands out
	int level1, level2, level3, prevGood, goodCount, avgNeighbors;
	prevGood = levels.Pixel(0,0,0);
	for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
			//set opacity to 255
			out.Pixel(x,y,3) = 255;

			level1 = levels.Pixel(x-1,y,0);
			level2 = levels.Pixel(x,y,0);
			level3 = levels.Pixel(x+1,y,0);

			goodCount = 0;
			if (levels.Pixel(x-1,y,0) != 0)
				goodCount ++;
			if (levels.Pixel(x+1,y,0) != 0)
				goodCount ++;
			if (levels.Pixel(x,y-1,0) != 0)
				goodCount ++;
			if (levels.Pixel(x,y+1,0) != 0)
				goodCount ++;
			if (levels.Pixel(x-1,y-1,0) != 0)
				goodCount ++;
			if (levels.Pixel(x+1,y+1,0) != 0)
				goodCount ++;
			if (levels.Pixel(x-1,y+1,0) != 0)
				goodCount ++;
			if (levels.Pixel(x+1,y-1,0) != 0)
				goodCount ++;

			if (goodCount != 0)
				avgNeighbors = (levels.Pixel(x-1,y,0) + levels.Pixel(x+1,y,0) + levels.Pixel(x,y+1,0) + levels.Pixel(x,y-1,0) + levels.Pixel(x-1,y-1,0) + levels.Pixel(x+1,y+1,0) + levels.Pixel(x-1,y+1,0) + levels.Pixel(x+1,y-1,0)) / goodCount;
			else
				avgNeighbors = prevGood;
			
			//if the level is different from the previous level set black
			if ((abs(level2 - avgNeighbors) > 70) && level2 != 0){
				out.Pixel(x,y,0) = 255;
				out.Pixel(x,y,1) = 255; 
				out.Pixel(x,y,2) = 255;
				prevGood = level2;
			}

			//else set white
			else {
				out.Pixel(x,y,0) = 0;
				out.Pixel(x,y,1) = 0; 
				out.Pixel(x,y,2) = 0;   
			}
	
			//keep track of most recently seen non-zero level
			//if (level != 0)
				//plevel = level;
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
