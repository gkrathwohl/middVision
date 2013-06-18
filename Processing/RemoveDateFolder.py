#!/usr/bin/python


import shutil
import os
import argparse
import sys
import glob
import re
import json

def execute(cmd):
    print cmd
    os.system(cmd)

parser = argparse.ArgumentParser(description="Control the processing pipeline for ActiveLighting4")
parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")

args = parser.parse_args(sys.argv[1:])
scenedir = args.targetScene[0]
if(not os.path.exists(scenedir)):
    print "Target directory does not exist"
    exit(0)
if(not scenedir[len(scenedir)-1] == '/'):
    scenedir += '/'


ambdir = scenedir+"orig/ambient"
print ambdir

calibdir = scenedir+"orig/calibration"
print calibdir

greydir = scenedir+"orig/greycode"
print greydir

'''
source = os.path.join("sample/orig/ambient/L*/left/*/*.JPG", "ambdir/L*/left")
destination = "ambdir/L0/left"
for files in source:
    #if files.endswith(".txt"):
        shutil.move(files,destination)
'''
print os.listdir(ambdir)

for dir in os.listdir(ambdir):
    execute("mv "+ambdir+"/"+dir+"/left/*/* "+ambdir+"/"+dir+"/left")
    execute("rm -rf "+ambdir+"/"+dir+"/left/*/")
    execute("mv "+ambdir+"/"+dir+"/right/*/* "+ambdir+"/"+dir+"/right")
    execute("rm -rf "+ambdir+"/"+dir+"/right/*/")

for dir in os.listdir(calibdir):
    execute("mv "+calibdir+"/"+dir+"/*/* "+calibdir+"/"+dir)
    execute("rm -rf "+calibdir+"/"+dir+"/*/")
    execute("mv "+calibdir+"/"+dir+"/*/* "+calibdir+"/"+dir)
    execute("rm -rf "+calibdir+"/"+dir+"/*/")

for dir in os.listdir(greydir):
    execute("mv "+greydir+"/"+dir+"/left/*/* "+greydir+"/"+dir+"/left")
    execute("rm -rf "+greydir+"/"+dir+"/left/*/")
    execute("mv "+greydir+"/"+dir+"/right/*/* "+greydir+"/"+dir+"/right")
    execute("rm -rf "+greydir+"/"+dir+"/right/*/")
