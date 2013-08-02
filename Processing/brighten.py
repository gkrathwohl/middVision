#Brightening script

# uses convert tool to brighten images, useful for calibration images that were too dark.
# this script is unfinished, but can easily be used to batch brighten calibration images. Run from scene folder or insert directory into command below

import os


for x in range(1,44):
	num = str(x).zfill(2)
	os.system("convert orig/calibration/right/IMG_00" + num + ".JPG -modulate 170 orig/calib/right/IMG00" + num + ".JPG")
