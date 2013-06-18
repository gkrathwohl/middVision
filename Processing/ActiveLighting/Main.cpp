///////////////////////////////////////////////////////////////////////////
//
// NAME
//  Main.cpp -- dispatches different components of active lighting code
//
// USAGE
//  activeLighting <mode> <params>
//
//
// 
//
// Copyright � Daniel Scharstein, 2011.
//
///////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "imageLib/imageLib.h"
#include "Utils.h"
#include "imageLib/flowIO.h"
#include "assert.h"
#include "string.h"
#define NOMATCH -9999

// prototypes:


using namespace std;

// threshold images (defined in GreyThresh.cpp)
void greyThresh(char* outdir, char **imList, int numIm);

// decode images (defined in GreyDecode.cpp)
CFloatImage greyDecode(char* outdir, char* codefile, int direction, char **imList, int numIm);

// combine multiple confidence maps into one (defined in GreyThresh.cpp)				
void combineConfidence(char *outfile, char **imList, int numIm, int certain);

// compute pair of disparity maps from labeled (pmf) images (defined in Disparity.cpp)
void computeDisparities(char *outdir,char *in0, char *in1, int dXmin, int dXmax, int dYmin, int dYmax);

CFloatImage simpleCrossCheck(CFloatImage im0, CFloatImage im1);

void reproject(char* out, char *dispFile, char *codeFile, float rxmin, float rxmax, float rymin, float rymax);

void mergeDisparityMaps(char* output, char** filenames, int count, int mingroup);

void calibrate(int pairsnum, int circles, char** left, char** right, int boardw, int boardh, int visualize, char* dirname);

void rectify(int images, char* destdir, char** matrices, char** photos);




// prototypes:




int main(int argc, char* argv[])
{

    try {
	    if (argc < 2)
			throw CError("\n  usage: activeLighting <mode> <params>\n"
"\n"
  "mode = calibrate  :  calibrate a pair of cameras using images of a chess board\n"
  "mode = rectify    :  rectify images based on a provided calibration\n"
  "mode = confidence :  read images taken by the camera and produces confidence maps \n"
  "mode = threshold  :  combine different confidence maps into a single thresholded image\n"
  "mode = decode     :  read thresholded images and decode them \n"
  "mode = disparity  :  compute pair of disparity maps from labeled flo images \n"
  "mode = crosscheck :  crosscheck disparity maps for accuracy \n"
  "mode = reproject  :  compute illumination disparities using reprojection \n"
  "mode = merge      :  merge an arbitrary number of registered disparity maps \n"
"");
	

		string modestr = string(argv[1]);



		if(modestr.compare("confidence") == 0){
			if (argc < 4)
				throw CError("usage: ActiveLighting4 confidence outputdir im0 im0i [im1 im1i ...]");
			{
				char* outdir = argv[2];
				char **imList = &argv[3];
				int numIm = argc-3;
				greyThresh(outdir, imList, numIm);
			}
			return 0;
		}



		if(modestr.compare("calibrate") == 0){
				            if (argc < 4)
				                throw CError("usage: ActiveLighting4 calibrate visualize patternWidth patternHeight outdir leftimgs... rightimgs...");
				            {

								char *outdir = argv[5];
								int visualize = atoiSafe(argv[2]);
								if(!((argc-6)%2 == 0)){
									throw CError("Must have equal number of left and right images");
								}
								int numIm = (argc-6)/2;
								assert(numIm+numIm+6 == argc);
				                char **left = &argv[6];
				                char **right = &argv[6+numIm];

				                int w = atoiSafe(argv[3]);
				                int h = atoiSafe(argv[4]);

				                int circles = 0;
								calibrate(numIm, circles, left, right, w, h, visualize, outdir);
				            }
				            return 0;
				}

		if(modestr.compare("rectify") == 0){
						            if (argc < 6)
						                throw CError("usage: ActiveLighting4 rectify outdir intrinsic.xml distort.xml rot.xml rect.xml imgs...");
						            {

										char *outdir = argv[2];
										int numIm = argc-7;

						                char **mats = &argv[3];
						                char **images = &argv[7];

										rectify(numIm,outdir, mats, images);
						            }
						            return 0;
						}

			
		if(modestr.compare("threshold") == 0){
			if (argc < 5)
				throw CError("usage: ActiveLighting4 threshold out.pgm certainthresh in1.pgm in2.pgm ...");
			{
				// combine different confidence maps into one

				char *outfile = argv[2];
				int certain = atoiSafe(argv[3]);
				char **imList = &argv[4];
				int numIm = argc-4;
				combineConfidence(outfile, imList, numIm, certain);
			}
			return 0;
		}


		if(modestr.compare("decode") == 0){
			if (argc < 5)
				throw CError("usage: ActiveLighting4 decode outdir codefile im0 [im1 ...]    <vertical patterns first, then horizontal>");
			{

				// decode images
				char* outdir = argv[2];
				char* codefile = argv[3];
				char **imList = &argv[4];
				assert((argc-4)/2 % 2 == 0);
				int numIm = (argc-4) / 2;
				CFloatImage x = greyDecode(outdir, codefile, 0, imList, numIm);
				imList = &argv[4+numIm];
				CFloatImage y = greyDecode(outdir, codefile, 1, imList, numIm);

				CFloatImage merged = mergeToFloImage(x,y);
				char filename[1024];
				sprintf(filename, "%s/result.flo",outdir);
				WriteFlowFile(merged, filename);

			}
			return 0;
		}
			

			

			

		if(modestr.compare("disparity") == 0){
            if (argc < 8)
                throw CError("usage: ActiveLighting4 disparity outdir in0.flo in1.flo dXmin dXmax dYmin dYmax ");
            {
                // compute disparity map

            	char* outdir = argv[2];
				char *in0 = argv[3];
				char *in1 = argv[4];
				int dmin = atoi(argv[5]);
				int dmax = atoi(argv[6]);
				int dYmin = atoi(argv[7]);
				int dYmax = atoi(argv[8]);



				computeDisparities(outdir, in0, in1,dmin,dmax, dYmin, dYmax);
            }
            return 0;
		}

		if(modestr.compare("crosscheck") == 0){
                    if (argc < 4)
                        throw CError("usage: ActiveLighting4 crosscheck disp0.pgm disp1.pgm");
                    {
                        // cross-check a pair of disparity maps
        				char *disp0 = argv[2];
        				char *disp1 = argv[3];

        				CFloatImage im0, im1;
        				ReadFlowFile(im0,disp0);
        				ReadFlowFile(im1,disp1);

        				CFloatImage X = simpleCrossCheck(im0,im1);
        				WriteFlowFile(X, "Xout.flo");


                    }
                    return 0;
		}


		if(modestr.compare("reproject") == 0){
			if (argc < 8)
				throw CError("usage: ActiveLighting4 reproject out.flo disp.flo code.flo xmin xmax ymin ymax");


			char* outdir = argv[2];
			char *disp = argv[3];
			char *code = argv[4];
			float xmin = atof(argv[5]);
			float xmax = atof(argv[6]);
			float ymin = atof(argv[7]);
			float ymax = atof(argv[8]);
			reproject(outdir,disp,code, xmin, xmax, ymin, ymax);
			return 0;
		}


		if(modestr.compare("merge") == 0){
			if (argc < 4)
				throw CError("usage: ActiveLighting4 merge mingroup output.flo disp1.flo disp2.flo ... dispN.flo");

			int mingroup = atoiSafe(argv[2]);
			char* output = argv[3];
			char** disps = &argv[4];
			int count = argc - 4;

			mergeDisparityMaps(output, disps, count, mingroup);
			return 0;
		}

			throw CError("unknown mode");

	}
    catch (CError &err) {
        fprintf(stderr, err.message);
        fprintf(stderr, "\n");
        return -1;
    }

    return 0;
}
