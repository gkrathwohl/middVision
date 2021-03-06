///////////////////////////////////////////////////////////////////////////
//
// NAME
//  SolveProjection.cpp -- compute projection matrix for new view and compute 
//						   reprojected disparities
//
// DESCRIPTION
//  Takes a disparity map and the u, v code maps for an illumination direction, and
//  computes the projection matrix:
//
//  Input: 
//   - set of k scene points Si = [xi yi di 1]'
//   - coordinates of those points in new view Pi = [ui vi 1]'
//
//   Output: projection matrix M
//
//  Relationship: Pi = M Si  where M is unknown 4x3 projection matrix [m0 m1 m2]'
//  This gives two linear equations per point:
//    ui m2 Si = m0 Si
//    vi m2 Si = m1 Si
//  Which gives a large overconstrained system Ax ~ b where x is the vector of 
//  the 11 unknowns (we fix m23=1), and A is 2k x 11, and b is 2k x 1:
//
//  A=															b=
//  x1  y1  d1  1   0   0   0   0   -x1*u1 -y1*u1 -d1*u1		u1
//  0   0   0   0   x1  y1  d1  1   -x1*v1 -y1*v1 -d1*v1		v1
//  ...															...
//
//  This is solved using least squares by solving A'Ax = A'b using Gauss elimination
//
//  The goodness of fit is then evaluated, and the outliers identified.  The fit is repeated
//  with smaller and smaller outlier threholds until a good fit is found.  The robustly 
//  estimated projection matrix is then used to translate the u,v values into disparities.
//
// Copyright � Daniel Scharstein, 2002.
//
///////////////////////////////////////////////////////////////////////////

// TODO: these need to be global variables or constants:
#define UNK -9999	// label for uncertain code pixel in both Int and Float image
#define oldDMIN 330
#define oldDMAX 580
#define DMIN 210
#define DMAX 450
#define NOMATCH -9999


#include "imageLib/imageLib.h"
#include "Utils.h"
#include "imageLib/flowIO.h"
#include "cv.h"



void SolveProjectionCV(CFloatImage disp, CFloatImage codeu, CFloatImage codev,
					 CByteImage badmap, double *M, int step, int xmin, int xmax, int ymin, int ymax)
{


	CShape sh = disp.Shape();
	int x, y;//, w = sh.width, h = sh.height;
	int verbose = 1;



    if (verbose) printf("building matrix (step=%d)\n", step);

	// count unknowns:
	int cntd = 0;
	int cntc = 0;
	int cnt = 0;
	cv::Mat A;
	vector<float> bvec;
	// estimate the matrix
	for (y = ymin; y < ymax; y += step) {
		float *dis = &disp.Pixel(0, y, 0);
		float *cu = &codeu.Pixel(0, y, 0);
		float *cv = &codev.Pixel(0, y, 0);
		uchar *bad = &badmap.Pixel(0, y, 0);

		for (x=xmin; x < xmax; x += step) {
			if (bad[x])		// used for robust fit
				continue;

			float d = dis[x];
			float u = cu[x];
			float v = cv[x];

			cnt++;

			if (d == NOMATCH )
				cntd++;

			if (u == UNK || v == UNK)
				cntc++;

			if (d == NOMATCH || u == UNK || v == UNK )
				continue;

			cv::Mat Arow(2,11,CV_32FC1);



			// even row (first equation)
			Arow.at<float>(0,0) =  x; Arow.at<float>(0,1) =  y; Arow.at<float>(0,2) =  d; Arow.at<float>(0,3) =  1;
			Arow.at<float>(0,4) =  0; Arow.at<float>(0,5) =  0; Arow.at<float>(0,6) =  0; Arow.at<float>(0,7) =  0;
			Arow.at<float>(0,8) =  -x*u; Arow.at<float>(0,9) =  -y*u; Arow.at<float>(0,10) =  -d*u;
			bvec.push_back(u);



			// odd row (second equation)
			Arow.at<float>(1,0) =  0; Arow.at<float>(1,1) =  0; Arow.at<float>(1,2) =  0; Arow.at<float>(1,3) =  0;
			Arow.at<float>(1,4) =  x; Arow.at<float>(1,5) =  y; Arow.at<float>(1,6) =  d; Arow.at<float>(1,7) =  1;
			Arow.at<float>(1,8) =  -x*v; Arow.at<float>(1,9) =  -y*v; Arow.at<float>(1,10) =  -d*v;


			bvec.push_back(v);

			if(A.cols == 0){
				A = Arow;
			}else{
				A.push_back(Arow);
			}



		}
	}

    if (verbose)
		printf("unknown d: %.2f%%, unknown code: %.2f%%\n", 100.0*cntd/cnt, 100.0*cntc/cnt);
    if (0 && verbose) printf("solving matrix\n");

    cv::Mat b(bvec);

    cv::Mat sol;
    cv::solve(A,b,sol,cv::DECOMP_LU | cv::DECOMP_NORMAL);
    for(int i = 0 ;i < sol.rows; i++){
        	for(int j = 0; j < sol.cols; j++){
        		M[i*sol.cols+j] = sol.at<float>(i,j);
        	}
        }

    M[11] = 1;




	if (verbose) {
		printf("projection matrix M:\n");
		for (int i=0; i<3; i++) {
			for (int j=0; j<4; j++)
				printf("%12.6f  ", M[i*4+j]);
			printf("\n");
		}
	}
}

// evaluate goodness of fit of projection matrix M for disparity map and u and v code 
// value maps, and mark all pixels whose error is larger than maxerr in badmap
void EvaluateFit(CFloatImage disp, CFloatImage codeu, CFloatImage codev,
				 CByteImage badmap, double *M, double maxerr, int xmin, int xmax, int ymin, int ymax)
{
	CShape sh = disp.Shape();
	int x, y;//, w = sh.width, h = sh.height;
	int i, j;
	int verbose = 1;
	double suu = 0, svv = 0;
	int badu = 0, badv = 0;
	CByteImage imu0, imu1, imv0, imv1;
	int cnt = 0;


	
	badmap.ClearPixels();

	for (y = ymin; y < ymax; y++) {
		float *dis = &disp.Pixel(0, y, 0);
		float *cu = &codeu.Pixel(0, y, 0);
		float *cv = &codev.Pixel(0, y, 0);
		uchar *bad = &badmap.Pixel(0, y, 0);
	

	
		for (x=xmin; x < xmax; x++) {
			float d = dis[x];
			float u = cu[x];
			float v = cv[x];
			
			if (d == NOMATCH || u==UNK || v == UNK || d == 0)
				continue;
	
			cnt++;
			
			double S[4] = {x, y, d, 1};
			double P[3] = {0, 0, 0};
			
			// project S by computing X = M S
			for (i=0; i<3; i++) {
				for (j=0; j<4; j++)
					P[i] += M[4*i + j] * S[j];
			}
			
			double nu = P[0] / P[2];
			double nv = P[1] / P[2];
			double du = nu - u;
			double dv = nv - v;
			suu += du * du;
			svv += dv * dv;
			if (ABS(du) > maxerr) {
				badu++;
				bad[x] = 1;		// mark as bad pixel for subsequent passes
			}
			if (ABS(dv) > maxerr) {
				badv++;
				bad[x] = 1;		// mark as bad pixel for subsequent passes
			}
			

		}
	}
    if (verbose) 
		printf("rmsu= %.2f, rmsv= %.2f, badu= %.2f%%, badv= %.2f%%  (bad thresh= %g)\n",
			sqrt(suu/cnt), sqrt(svv/cnt), 100.0*badu/cnt, 100.0*badv/cnt, maxerr);

}

// reproject a disparity map based on the recovered projection matrix
// don't use pixels that were identfied as outliers
void projectDisp(CFloatImage codeu, CFloatImage codev, CByteImage badmap,
				 CFloatImage ndisp, double *M0, double *M1, double *M2)
{
	CShape sh = codeu.Shape();
	int x, y, w = sh.width, h = sh.height;


	for (y = 0; y < h; y++) {
		float *cu = &codeu.Pixel(0, y, 0);
		float *cv = &codev.Pixel(0, y, 0);
		uchar *bad = &badmap.Pixel(0, y, 0);
		float *d = &ndisp.Pixel(0, y, 0);

		for (x=0; x < w; x++) {
			float u = cu[x];
			float v = cv[x];
			if (u == UNK || v == UNK || bad[x]){
				d[x] = NOMATCH;
			} else {
				// do least squares combination of the two estimates for d
				double bu = x * (M2[0]*u - M0[0]) + y * (M2[1]*u - M0[1]) + (M2[3]*u - M0[3]);
				double bv = x * (M2[0]*v - M1[0]) + y * (M2[1]*v - M1[1]) + (M2[3]*v - M1[3]);
				double au =   - (M2[2]*u - M0[2]);
				double av =   - (M2[2]*v - M1[2]);

				double dd = (au * bu + av * bv) / (au * au + av * av);
				

					d[x] = (float) dd;		// map to standard disparity encoding

			}
		}
	}
}

//compare two disparity maps and report statistics
void compareDisp(CFloatImage disp0, CFloatImage disp1)
{
	CShape sh = disp0.Shape();
	int x, y, w = sh.width, h = sh.height;
	int verbose = 1;

	int badThresh = 2;
	int cntBad = 0;
	int cnt = 0;
	//CByteImage err(sh);

	for (y = 0; y < h; y++) {
		float *d0 = &disp0.Pixel(0, y, 0);
		float *d1 = &disp1.Pixel(0, y, 0);
		//uchar *e = &err.Pixel(0, y, 0);

		for (x=0; x < w; x++) {
			//e[x] = 180; // unknown

			if (d0[x] == NOMATCH || d1[x] == NOMATCH)
				continue;

			cnt++;
			//e[x] = 255; // good

			if (ABS(d0[x] - d1[x]) > badThresh) {
				cntBad++;
				//e[x] = 0; // bad
			}
		}
	}

	if (verbose)
		printf("compared: %.2f%%, bad: %.2f%% (badthresh = %d)\n", 
		100.0*cnt/(w*h), 100.0*cntBad/cnt, badThresh);
		
	//WriteImageVerb(err, badMapName, verbose);
}

// takes disparity map and two code maps and recovers projection matrix for projector
// then reprojects projector's disparities into camera disparities
void reproject(char* out, char *dispFile, char *codeFile, float rxmin, float rxmax, float rymin, float rymax)
{
	CFloatImage disp,dispboth,code,codex,codey;
	CShape sh;
	char filename[1000];






	double M[12]; // projection matrix

	ReadFlowFile(dispboth, dispFile);
	pair<CFloatImage, CFloatImage> d = splitFloImage(dispboth);
	disp = d.first;


	ReadFlowFile(code, codeFile);
	pair<CFloatImage, CFloatImage> p = splitFloImage(code);
	codex = p.first;
	codey = p.second;

	sh = disp.Shape();

	int xmin, xmax, ymin, ymax;

	xmin = (int)(rxmin*sh.width);
	xmax = (int)(rxmax*sh.width);
	ymin = (int)(rymin*sh.height);
	ymax = (int)(rymax*sh.height);

	printf("%f %f %f %f\n", rxmin, rxmax, rymin, rymax);

	CByteImage badmap(sh);
	badmap.ClearPixels();
	int step, maxerr;



	step = 3;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
	maxerr = 40; EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);
	step = 2;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
	maxerr = 5;  EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);
	step = 2;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
	maxerr = 3;  EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);
	step = 1;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
	maxerr = 3;  EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);

	step = 1;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
		maxerr = 2;  EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);

		step = 1;    SolveProjectionCV(disp, codex, codey, badmap, M, step, xmin, xmax, ymin, ymax);
			maxerr = 1;  EvaluateFit(disp, codex, codey, badmap, M, maxerr, xmin, xmax, ymin, ymax);




	printf("=======Matrix========\n");
	for(int i =0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			printf("%f ",M[4*i+j]);
		}
		printf("\n");
	}

	CFloatImage ndisp(sh);
	CFloatImage blank;
	blank.ReAllocate(sh);
	blank.ClearPixels();

	projectDisp(codex, codey, badmap, ndisp, &M[0], &M[4], &M[8]);
	compareDisp(disp, ndisp);
	ndisp = mergeToFloImage(ndisp,blank);

	sprintf(filename, "%s", out);
	WriteFlowFile(ndisp, filename);
}
