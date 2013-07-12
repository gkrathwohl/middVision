 #!/usr/bin/python

'''
Merge 2 identical scenes where an object has been removed to get a clear view of the background

Summer 2013
Slimmed down ActiveLighting Control
Greg Krathwohl

Usage: python merge.py foregroundScene backgroundScene outputScene
'''

import sys
import os
import glob
import argparse
import re
import json

vmin = 0
vmax = 0

def safemkdirs(dir):
    if(not os.path.exists(dir)):
        os.makedirs(dir);

def execute(cmd):
    print cmd
    os.system(cmd)
    
def ColorMerge(dir, dir1, input, outputx, outputy, speed="10"):
    txtDir = os.path.join(scenedir,"computed/");
    execute("FloVis/FloVis -s "+speed+" -i "+vmin+" -x "+vmax+" "+ " -m " + dirClose+"/"+input+" "+dirFar+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy + " > " + txtDir + "resultColor.txt")

def GreyMerge(dir, dir1, input, outputx, outputy, reverse = False):
    txtDir = os.path.join(scenedir,"computed/");
    if(reverse):
        execute("FloVis/FloVis -g -r -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy + " > " + txtDir + "resultGrey.txt")
    else:
        execute("FloVis/FloVis -g -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy + " > " + txtDir + "resultGrey.txt")
    
def cleanExit(config, configpath):
    json.dump(config, open(configpath, 'w'), sort_keys=True, indent = 4 )
    exit(0)
    
if __name__ == '__main__':
    
	parser = argparse.ArgumentParser(description="Control the processing pipeline for ActiveLighting4")


    parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")


	scenedir = args.targetScene[0]
    if(not os.path.exists(scenedir)):
        print "Target directory does not exist"

    mergeDir = os.path.join(scenedir,"computed/disparity")
    safemkdirs(mergeDir)

    VisColor(mergeDir, "left.flo", "leftx.ppm", "lefty.ppm")
    VisGrey(mergeDir, "left.flo", "leftx.pgm", "lefty.pgm", reverse = True)
    
    VisColor(mergeDir, "right.flo", "rightx.ppm", "righty.ppm")
    VisGrey(mergeDir, "right.flo", "rightx.pgm", "righty.pgm")

    print "Done with the merging and clean exit"      
    cleanExit(config, configpath) 
                
                
                
 
 
 

               
                
                
    
