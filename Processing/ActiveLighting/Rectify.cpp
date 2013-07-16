/*
 * Rectify.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: wwestlin
 */

#include <cv.h>
#include <highgui.h>


void rectify(int images, char* destdir, char** matrices, char** photos){



	//Load matrices from file

	CvMat* intrinsic = (CvMat*)cvLoad(matrices[0]);
	CvMat* distort = (CvMat*)cvLoad(matrices[1]);
	CvMat* rotate  = (CvMat*)cvLoad(matrices[2]);
	CvMat* projection  = (CvMat*)cvLoad(matrices[3]);



	//first image, for finding image size
	IplImage* image = cvLoadImage(photos[0]);

	//rectification map images
	IplImage* mapx = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
	IplImage* mapy = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);

	cvInitUndistortRectifyMap(intrinsic, distort, rotate, projection, mapx, mapy);



	//Load image, remap, save to file
	for(int i = 0; i < images; i++){

		image = cvLoadImage(photos[i]);

		IplImage* clone = cvCloneImage(image);
		cvRemap(clone, image, mapx, mapy);
		cvReleaseImage(&clone);

		char buffer[1024];
		sprintf(buffer,"%s/image%.3d.ppm",destdir,i);
		if(i%10 == 0){
			printf(".");
			fflush(stdout);
		}
		cvSaveImage(buffer, image);

	}
	printf("\n");
 }
