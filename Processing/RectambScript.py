'''
Active Lighting Control 2012 - Point & Shoot Rectified Ambient

Call rectify for images took by both DSLR and point-shoot cameras with corresponding calibration. Based on script from Active Lighting 2011, by Porter Westlin.

Author: Xi Wang
Date: 2012.7.27

'''
import os
import sys
import glob
import argparse

def execute(cmd):
	print cmd
	sys.stdout.flush()
	os.system(cmd);

def safemkdirs(dir):
	if(not os.path.exists(dir)):
		os.makedirs(dir);

def rectify(scenedir, calib, step):
	#print heyheyhey
	origdir = os.path.join(scenedir, "orig")
	parts = os.path.split(calib)[1].split("2")
	leftImgs = ""
	leftImgs = os.path.join(origdir, step, "L*", parts[0],"*.JPG") 
	
	rightImgs = ""
	rightImgs = os.path.join(origdir, step, "L*", parts[1],"*.JPG") 
	
	#safemkdirs(os.path.join(scenedir, "computed", step))
	#safemkdirs(os.path.join(scenedir, "computed", step, parts[0]+"2"+parts[1]))
	#safemkdirs(os.path.join(dstdir, step, parts[0]))
	#safemkdirs(os.path.join(dstdir, step, parts[1]))

	dstdir = os.path.join(scenedir, "computed", step, parts[0]+"2"+parts[1])
	leftdir = os.path.join(dstdir, parts[0])
	rightdir = os.path.join(dstdir, parts[1])    
	safemkdirs(dstdir)
	safemkdirs(leftdir)
	safemkdirs(rightdir)


	execute("ActiveLighting4 rectify "+ leftdir +" "+ os.path.join(calib,"Intrinsicsleft.xml")+" "+ os.path.join(calib,"Distortionleft.xml")+" "+ os.path.join(calib,"Rotationleft.xml") +" "+os.path.join(calib,"Projectionleft.xml")+" "+leftImgs)
	execute("ActiveLighting4 rectify "+ rightdir +" "+ os.path.join(calib,"Intrinsicsright.xml")+" "+ os.path.join(calib,"Distortionright.xml")+" "+ os.path.join(calib,"Rotationright.xml") +" "+os.path.join(calib,"Projectionright.xml")+" "+rightImgs)

if __name__ == '__main__':
	parser = argparse.ArgumentParser(description="Control the processing pipeline for rectified Ambient images with point and shoot data")
	parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")
#    parser.add_argument("-a", "--auto", dest="auto", action="store_const", const=True, default=False, help="Do not ask for confirmation before going to the next rectified step")
#    parser.add_argument("-s", "--step", dest="step", default="ambient", help="The step of the pipeline to start from. See README for steps. Errors may occur if a step is chosen which expects files or folder which have not yet been computed")

	args = parser.parse_args(sys.argv[1:])

	sceneDir = args.targetScene[0]
	origDir = os.path.join(sceneDir, "orig")
	if(not os.path.exists(sceneDir)):
		print "Target directory does not exist"

	calibDir = os.path.join(sceneDir,"computed/calibration")
	#dstDir = os.path.join(sceneDir, "computed")
	
	if(not os.path.exists(calibDir)):
		print "Can not find calibration directory"
		#calib = os.path.join(calibDir,"left2right")
	calibs = glob.glob(os.path.join(calibDir,"*"))
	for calib in calibs:
		rectify(sceneDir, calib, "ambient")
		rectify(sceneDir, calib, "ambientBall")
		if(os.path.exists(os.path.join(origDir, "ambientOccluders"))):
			rectify(sceneDir, calib, "ambientOccluders")
	print "Done with rectified all"
	
