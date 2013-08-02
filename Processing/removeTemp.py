
import sys
import os
import argparse

def execute(cmd):
    print cmd
    os.system(cmd)

parser = argparse.ArgumentParser(description="Script to remove calibration, confidence, decode, disparity intermediate, rectified, reproject, and threshold folders")

parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")
args = parser.parse_args(sys.argv[1:])
scenedir = args.targetScene[0]

execute("rm -r " + scenedir + "/computed/calibration")
execute("rm -r " + scenedir + "/computed/confidence")
execute("rm -r " + scenedir + "/computed/decode")
execute("rm -r " + scenedir + "/computed/disparityintermediate")
execute("rm -r " + scenedir + "/computed/rectified")
execute("rm -r " + scenedir + "/computed/reproject")
execute("rm -r " + scenedir + "/computed/threshold")

print("removed temp files :) :)")
