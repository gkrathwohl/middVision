/*
 * Calibrate.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: wwestlin
 */

#include <cv.h>
#include "opencv2/opencv.hpp"
#include <highgui.h>



using namespace std;
using namespace cv;


int DEBUG = 0;
int VISUALIZE = 0;
int board_w= 0;
int board_h = 0;
CvSize board_sz;
int board_n = 0;
CvSize image_sz;

void getRefinedCorners(char* pic, CvPoint2D32f* corners, int* corner_count){



	printf("%s\n",pic);
	IplImage* image = cvLoadImage(pic);
	image_sz = cvGetSize(image);
	
	int result = cvFindChessboardCorners(image, board_sz, corners,
			corner_count,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK);


	IplImage* gray_image = cvCreateImage(cvGetSize(image),8,1);

	//Get subpixel measures
	cvCvtColor(image, gray_image, CV_BGR2GRAY);
	cvFindCornerSubPix(gray_image, corners, *corner_count, cvSize(11,11),
			cvSize(-1,-1), cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));

	if(VISUALIZE){
		cvDrawChessboardCorners( image, board_sz, corners,
				*corner_count, result );
		IplImage* clone = cvCreateImage(cvSize(800, 600), image->depth, image->nChannels);
		cvResize(image, clone);
		cvNamedWindow("Corners");
		cvShowImage("Corners",clone);
		cvWaitKey(0);
		cvReleaseImage(&clone);
	}
	cvReleaseImage(&image);
	cvReleaseImage(&gray_image);

}

void getRefinedDots(char* pic, CvPoint2D32f* corners, int* corner_count){



	printf("%s\n",pic);

	IplImage* image = cvLoadImage(pic);

	image_sz = cvGetSize(image);

	IplImage* gray_image = cvCreateImage(cvGetSize(image),8,1);

		//Get subpixel measures
		cvCvtColor(image, gray_image, CV_BGR2GRAY);


	vector<Point2f> centers;
	int result = cv::findCirclesGrid(cv::Mat(gray_image), board_sz, centers, CALIB_CB_ASYMMETRIC_GRID);

	if(!result){
		printf("failure to find circle grid\n");
	}

	for(int i =0; i < (int)centers.size(); i++){

		corners[i] = centers[i];

	}

	*corner_count = centers.size();

	//IplImage* gray_image = cvCreateImage(cvGetSize(image),8,1);

	//Get subpixel measures
	if(VISUALIZE){
		cvDrawChessboardCorners( image, board_sz, corners,
				*corner_count, result );
		IplImage* clone = cvCreateImage(cvSize(800, 600), image->depth, image->nChannels);
		cvResize(image, clone);
		cvNamedWindow("Corners");
		cvShowImage("Corners",clone);
		cvWaitKey(0);
		cvReleaseImage(&clone);
	}
	cvReleaseImage(&image);
	cvReleaseImage(&gray_image);

}




CvMat* shrinkMatByRows(CvMat* mat, int start_row, int end_row){
	cvGetRows(mat, mat, start_row, end_row);
	return mat;
}



void calibrate(int pairsnum, int circles, char** left, char** right, int boardw, int boardh, int visualize, char* dirname){


	VISUALIZE = visualize;

	//size of chessboard pattern in interior corners
	board_w = boardw;
	board_h = boardh;

	board_n = board_w*board_h;
	board_sz = cvSize(board_w, board_h);


	//structures for corner values
	int corner_count1;
	CvPoint2D32f* corners1 = new CvPoint2D32f[ board_n ];
	int corner_count2;
	CvPoint2D32f* corners2 = new CvPoint2D32f[ board_n ];

	//number of image pairs
	int pairs = pairsnum;

	//lists of images
	char** cam1 = left;
	char** cam2 = right;

	//Counter for found chessboards
	//set to arg for matrix init
	//so that we know we have enough room
	int n_boards = pairs;
	printf("Checking %d pairs",n_boards);


	//Allocate storage
	CvMat* image_points1      = cvCreateMat(n_boards*board_n,2,CV_32FC1);
	CvMat* object_points1     = cvCreateMat(n_boards*board_n,3,CV_32FC1);
	CvMat* point_counts1      = cvCreateMat(n_boards,1,CV_32SC1);
	CvMat* intrinsic_matrix1  = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs1 = cvCreateMat(4,1,CV_32FC1);

	CvMat* image_points2      = cvCreateMat(n_boards*board_n,2,CV_32FC1);
	CvMat* object_points2     = cvCreateMat(n_boards*board_n,3,CV_32FC1);
	CvMat* point_counts2      = cvCreateMat(n_boards,1,CV_32SC1);
	CvMat* intrinsic_matrix2  = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs2 = cvCreateMat(4,1,CV_32FC1);

	int successes = 0;
	int step = 0;
	n_boards = 0;
	for(int i = 0; i < pairs ; i++){
		printf("Checking image pair %i\n",i);

		//Load image, find chessboard with sub-pixel corners
		if(circles){
			getRefinedDots(cam1[i],corners1, &corner_count1);
			getRefinedDots(cam2[i],corners2, &corner_count2);

		}else{
			getRefinedCorners(cam1[i],corners1, &corner_count1);
			getRefinedCorners(cam2[i],corners2, &corner_count2);
		}

		//If we found a good board, include it in the data
		if( corner_count1 == board_n && corner_count2 == board_n) {
			int x1 = corners1[board_n-1].x - corners1[0].x;
			int y1 = corners1[board_n-1].y - corners1[0].y;
			int x2 = corners2[board_n-1].x - corners2[0].x;
			int y2 = corners2[board_n-1].y - corners2[0].y;

			double mag1 = sqrt(x1*x1+y1*y1);
			double mag2 = sqrt(x2*x2+y2*y2);

			double angle = acos((x1*x2+y1*y2)/(mag1*mag2))*(180/3.14159265358979323);
			printf("Angle between boards is %f\n",angle);
			if(angle < 20){
				printf("Found both boards with same orientation!\n");
				step = successes*board_n;
				for( int i=step, j=0; j<board_n; ++i,++j ) {
					CV_MAT_ELEM(*image_points1, float,i,0) = corners1[j].x;
					CV_MAT_ELEM(*image_points1, float,i,1) = corners1[j].y;
					CV_MAT_ELEM(*object_points1,float,i,0) = j/board_w;
					CV_MAT_ELEM(*object_points1,float,i,1) = j%board_w;
					CV_MAT_ELEM(*object_points1,float,i,2) = 0.0f;
				}
				CV_MAT_ELEM(*point_counts1, int,successes,0) = board_n;


				for( int i=step, j=0; j<board_n; ++i,++j ) {
					CV_MAT_ELEM(*image_points2, float,i,0) = corners2[j].x;
					CV_MAT_ELEM(*image_points2, float,i,1) = corners2[j].y;
					CV_MAT_ELEM(*object_points2,float,i,0) = j/board_w;
					CV_MAT_ELEM(*object_points2,float,i,1) = j%board_w;
					CV_MAT_ELEM(*object_points2,float,i,2) = 0.0f;
				}
				CV_MAT_ELEM(*point_counts2, int,successes,0) = board_n;

				successes++;
				n_boards++;
				printf("Added board points to matrices\n");


			}
		}

	}
	printf("**********\nFound %i valid pairs\n**********\n",n_boards);
	printf("Picking out valid values\n");

	image_points1      = shrinkMatByRows(image_points1, 0, n_boards*board_n);
	object_points1     = shrinkMatByRows(object_points1, 0, n_boards*board_n);
	point_counts1      = shrinkMatByRows(point_counts1, 0, n_boards);

	image_points2      = shrinkMatByRows(image_points2, 0, n_boards*board_n);
	object_points2     = shrinkMatByRows(object_points2, 0, n_boards*board_n);
	point_counts2      = shrinkMatByRows(point_counts2, 0, n_boards);





	printf("Done resizeing!\n");

	// At this point we have all of the chessboard corners we need.
	// Initialize the intrinsic matrix such that the two focal
	// lengths have a ratio of 1.0
	//
	CV_MAT_ELEM( *intrinsic_matrix1, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *intrinsic_matrix1, float, 1, 1 ) = 1.0f;


	CV_MAT_ELEM( *intrinsic_matrix2, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *intrinsic_matrix2, float, 1, 1 ) = 1.0f;

	//Matrices to store StereoCalibrate stuff
	CvMat* R  = cvCreateMat(3,3,CV_64F);
	CvMat* T  = cvCreateMat(3,1,CV_64F);
	CvMat* E  = cvCreateMat(3,3,CV_64F);
	CvMat* F  = cvCreateMat(3,3,CV_64F);




	//Calculate instrinsics for both cameras separately

	printf("Starting calibration...\n");

	cvCalibrateCamera2(
			object_points1, image_points1, point_counts1,
			image_sz, intrinsic_matrix1, distortion_coeffs1, NULL, NULL, 0);


	cvCalibrateCamera2(
			object_points2, image_points2, point_counts2,
			image_sz, intrinsic_matrix2, distortion_coeffs2, NULL, NULL, 0);

	printf("Calibrating stereo...\n");

	double err = cvStereoCalibrate(
			object_points1, image_points1,image_points2,
			point_counts1,
			intrinsic_matrix1, distortion_coeffs1,
			intrinsic_matrix2, distortion_coeffs2,
			image_sz,
			R, T, E ,F,
			cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, 1e-10) ,CV_CALIB_FIX_INTRINSIC);
	printf("Total reprojection error is %f\n", err);
	printf("Average reprojection error is %f\n", (err/n_boards)/board_n);


	printf("Calibration complete!\n");


	printf("Generating rectification maps...\n");


	CvMat* Rl  = cvCreateMat(3,3,CV_64F);
	CvMat* Rr  = cvCreateMat(3,3,CV_64F);
	CvMat* Pl  = cvCreateMat(3,4,CV_64F);
	CvMat* Pr  = cvCreateMat(3,4,CV_64F);

	cvStereoRectify( intrinsic_matrix1, intrinsic_matrix2, distortion_coeffs1, distortion_coeffs2,
			image_sz, R, T, Rl, Rr, Pl, Pr,0,0);

	// SAVE THE INTRINSICS AND DISTORTIONS
	printf(" *** DONE!\n\nStoring Calibration Files\n\n");
	cvSave("Intrinsicsleft.xml",intrinsic_matrix1);
	cvSave("Distortionleft.xml",distortion_coeffs1);
	cvSave("Intrinsicsright.xml",intrinsic_matrix2);
	cvSave("Distortionright.xml",distortion_coeffs2);
	cvSave("Fundamental.xml", F);
	cvSave("Rotationleft.xml", Rl);
	cvSave("Rotationright.xml", Rr);
	cvSave("Projectionleft.xml", Pl);
	cvSave("Projectionright.xml", Pr);

	//Move generated files to destination directory
	//Probably cleaner to save them there to begin with, but this works
	char *mv = (char *)malloc(512);
	strcpy(mv,"mv *.xml ");
	char* command = strcat(mv, dirname);
	system(command);
	delete mv;




}



