#!/usr/bin/python

'''
Created on Jun 28, 2011

@author: wwestlin
'''

import sys
import os
import glob
import argparse
import re
import json

vmin = 0
vmax = 0


def tryint(s):
    try:
        return int(s)
    except:
        return s

def alphanum_key(s):
    """ Turn a string into a list of string and number chunks.
        "z23a" -> ["z", 23, "a"]
    """
    return [ tryint(c) for c in re.split('([0-9]+)', s) ]

def naturalsort(l):
    """ Sort the given list in the way that humans expect.
    """
    l.sort(key=alphanum_key)

def safemkdirs(dir):
    if(not os.path.exists(dir)):
        os.makedirs(dir);

def execute(cmd):
    print cmd
    os.system(cmd)
    
def VisColor(dir, input, outputx, outputy, speed="10"):
    execute("FloVis -s "+speed+" -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)
def VisGrey(dir, input,outputx, outputy, reverse = False):
    if(reverse):
        execute("FloVis -g -r -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)
    else:
        execute("FloVis -g -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)
    
def cleanExit(config, configpath):
    json.dump(config, open(configpath, 'w'), sort_keys=True, indent = 4 )
    exit(0)
    
if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description="Control the processing pipeline for ActiveLighting4")
    parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")
    parser.add_argument("-v", "--visualize", dest="visualize", action="store_const", const=True, default=False, help="Visualize processing at every step")
    parser.add_argument("-a", "--auto", dest="auto", action="store_const", const=True, default=False, help="Do not ask for confirmation before going to the next step")
    parser.add_argument("-q", "--query", dest="query", action="store_const", const=True, default=False, help="Query for processing parameters at each step, rather than using defaults or options")
    parser.add_argument("-s", "--step", dest="step", default="calibrate", help="The step of the pipeline to start from. See README for steps. Errors may occur if a step is chosen which expects files or folder which have not yet been computed")
    
    parser.add_argument("-i", "--width", dest="width", default="17", help="The width of the calibration pattern used. Defaults to 17")
    parser.add_argument("-j", "--height", dest="height", default="11", help="The height of the calibration pattern used. Defaults to 11")
    
    parser.add_argument("-c", "--code", dest="code", default="../codes/minSW.dat", help="The code file to use for decoding. Defaults to '../codes/minSW.dat' ")
    
    parser.add_argument("-t", "--certainty", dest="cert", default="115", help="The certainty threshold to use during thresholding. Defaults to 115.")
    
    parser.add_argument("-xi", "--xmin", dest="xmin", default="-1200", help="X minimum for disparity search. Defaults to -600")
    parser.add_argument("-xx", "--xmax", dest="xmax", default="1200", help="X maximum for disparity search. Defaults to 600")
    parser.add_argument("-yi", "--ymin", dest="ymin", default="-5", help="Y minimum for disparity search. Defaults to -3")
    parser.add_argument("-yx", "--ymax", dest="ymax", default="5", help="Y maximum for disparity search. Defaults to 3")
    
    parser.add_argument("-vi", "--vmin", dest="vmin", default=None, help="Minimum disparity for grey visualization. Defaults to image minimum disparity.")
    parser.add_argument("-vx", "--vmax", dest="vmax", default=None, help="Maximum disparity for grey visualization. Defaults to image maximum disparity.")
    
    parser.add_argument("-rxi", "--rxmin", dest="rxmin", default="0", help="X minimum for reprojection fitting as a fraction. Defaults to 0.")
    parser.add_argument("-rxx", "--rxmax", dest="rxmax", default="1", help="X maximum for reprojection fitting as a fraction. Defaults to 1.")
    parser.add_argument("-ryi", "--rymin", dest="rymin", default="0", help="Y minimum for reprojection fitting as a fraction. Defaults to 0.")
    parser.add_argument("-ryx", "--rymax", dest="rymax", default="1", help="Y maximum for reprojection fitting as a fraction. Defaults to 1.")
    
    parser.add_argument("-gs", "--groupsize", dest="gs", default="2", help="Minimum group size to accept during disparity map merging. Defaults to 2")
    parser.add_argument("-o", "--visonly", dest="visonly", action="store_const", const=True, default=False, help="Only recompute the visualizations. Starts from the disparity step and reruns FloVis")
    parser.add_argument("--sitedir", dest="sitedir", default=None, help="Directory to build visualization website in")
    parser.add_argument("-pc","--previousconfig", dest="prevconfig", action="store_const", const=True, default=False, help="Load the last used configuration from config.txt and re-use it. Overrides any other options")
    parser.add_argument("-pv","--previousvis", dest="prevvis", action="store_const", const=True, default=False, help="Load the last used vmin and vmax from config.txt")
    parser.add_argument("-pd","--prevVisAsDispSearch", dest="prevdisp", action="store_const", const=True, default=False, help="Load the last used vmin and vmax from config.txt and use it as xmin and xmax for the disparity search range")
    
    
    args = parser.parse_args(sys.argv[1:])
    
    step = args.step
    
    
    
    
    if args.visonly:
        if step != "merge" and step != "reproject":
            step = "disparity"
    scenedir = args.targetScene[0]
    if(not os.path.exists(scenedir)):
        print "Target directory does not exist"
    if(not scenedir[len(scenedir)-1] == '/'):
        scenedir += '/'
    if(args.query):
        args.xmin = None
        args.xmax = None
        args.ymin = None
        args.ymax = None
        args.xmin = None
        args.rxmin = None
        args.rxmax = None
        args.rymin = None
        args.rymax = None
        args.cert = None
        args.code = None
        args.height = None
        args.width = None
        args.gs = None
        
    if args.vmin == None:
        vmin = args.xmin
    else:
        vmin = args.vmin
        
    if args.vmax == None:
        vmax = args.xmax
    else:
        vmax = args.vmax
    
    configpath = scenedir+"/computed/config.txt"
    if(os.path.exists(configpath)):
        config = json.load(open(configpath))
    else:            
        config = {}
    
    if(args.prevconfig):
        print "Option previous config not yet implemented"
        exit(0)
        
    if(args.prevvis):
        vmax =  config["VisualizationMaximumDepth"]
        vmin =  config["VisualizationMinimumDepth"]
    if(args.prevdisp):    
        xmax =  config["VisualizationMaximumDepth"]
        xmin =  config["VisualizationMinimumDepth"]
        
    
    while(step == "calibrate"):
        calibDir = os.path.join(scenedir,"orig/calibration")
        confidenceDir = os.path.join(scenedir,"computed/calibration")
        safemkdirs(confidenceDir)
        leftDir = os.path.join(calibDir,"left/*")
        rightDir = os.path.join(calibDir,"right/*")
        
        w = args.width
        h = args.height
        
        if(w == None):
            w = raw_input("Pattern width?")
        if(h == None):
            h = raw_input("Pattern height?")
        config['PatternWidth'] = w
        config['PatternHeight'] = h
        execute("ActiveLighting4 calibrate 0 "+w+" "+h+" "+confidenceDir+" "+leftDir+" "+rightDir)
        
        answer = ""
        if(args.auto):
            step = "rectify"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "rectify"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "calibrate" 
    
    while(step == "rectify"):
        sceneName = os.path.split(os.path.abspath(scenedir))[1]
        
        photoDirs = glob.glob(os.path.join(scenedir, "orig/greycode/*"))
        
        rectifiedDir = os.path.join(scenedir,"computed/rectified")
        safemkdirs(rectifiedDir)
        
        intrinsics = [os.path.join(scenedir,"computed/calibration/Intrinsicsleft.xml"), os.path.join(scenedir,"computed/calibration/Intrinsicsright.xml")]
        distortion = [os.path.join(scenedir,"computed/calibration/Distortionleft.xml"), os.path.join(scenedir,"computed/calibration/Distortionright.xml")]
        rotation = [os.path.join(scenedir,"computed/calibration/Rotationleft.xml"), os.path.join(scenedir,"computed/calibration/Rotationright.xml")]
        projection = [os.path.join(scenedir,"computed/calibration/Projectionleft.xml"), os.path.join(scenedir,"computed/calibration/Projectionright.xml")]
        
        for dir in photoDirs:
            match = re.match(".*take([0-9]+)exp([0-9]+)P([0-9]+)",dir)
            if match:
                confidenceDir = os.path.join(rectifiedDir,"P"+`(int(match.group(3))+2*(int(match.group(1))-1))`+"/exp"+match.group(2))
                outLeft = confidenceDir+"/left"
                safemkdirs(outLeft)
                execute("ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+dir+"/left/*")
                
                outRight = confidenceDir+"/right"
                safemkdirs(outRight)
                execute("ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+dir+"/right/*")
                    
        
        
        answer = ""
        if(args.auto):
            step = "rectifyambient"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "rectifyambient"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "rectify"                


    while(step == "rectifyambient"):
        rectambdir = os.path.join(scenedir,"computed/rectifiedAmbient")
        safemkdirs(rectambdir)
        intrinsics = [os.path.join(scenedir,"computed/calibration/Intrinsicsleft.xml"), os.path.join(scenedir,"computed/calibration/Intrinsicsright.xml")]
        distortion = [os.path.join(scenedir,"computed/calibration/Distortionleft.xml"), os.path.join(scenedir,"computed/calibration/Distortionright.xml")]
        rotation = [os.path.join(scenedir,"computed/calibration/Rotationleft.xml"), os.path.join(scenedir,"computed/calibration/Rotationright.xml")]
        projection = [os.path.join(scenedir,"computed/calibration/Projectionleft.xml"), os.path.join(scenedir,"computed/calibration/Projectionright.xml")]
        outLeft = rectambdir+"/left"
        outRight = rectambdir+"/right"
        safemkdirs(outLeft)
        safemkdirs(outRight)
        execute("ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+os.path.join(scenedir,"orig/ambient/L*/left/*.JPG"))
        
        execute("ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+os.path.join(scenedir,"orig/ambient/L*/right/*.JPG"))
        
        answer = ""
        if(args.auto):
            step = "confidence"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
   
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "confidence"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "rectifyambient" 
        

    while(step == "confidence"):
        confidenceDir = os.path.join(scenedir,"computed/confidence")
        safemkdirs(confidenceDir)
        
        photoFolders = glob.glob(os.path.join(scenedir,"computed/rectified")+"/*/*/*")
        
        for folder in photoFolders:
            dirs = folder.split("/")
            outputDir = confidenceDir+"/"+dirs[len(dirs)-3]+"/"+dirs[len(dirs)-2]+"/"+dirs[len(dirs)-1]
            safemkdirs(outputDir)
            execute("ActiveLighting4 confidence "+outputDir+" "+folder+"/*")
        
        
        
        answer = ""
        if(args.auto):
            step = "threshold"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "threshold"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "confidence" 
    
    while(step == "threshold"):
        thresholdDir = os.path.join(scenedir,"computed/threshold")
        safemkdirs(thresholdDir)
        
        projectorFolders = glob.glob(os.path.join(scenedir,"computed/confidence")+"/*")
        
        cert = args.cert
        if(cert == None):
            cert = raw_input("Certainty Threshold?")
        config['CertaintyThreshold'] = cert
        for proj in projectorFolders:
            projDir = thresholdDir+"/"+os.path.split(proj)[1]
            outLeft = projDir+"/left"
            outRight = projDir+"/right"
            safemkdirs(outLeft)
            safemkdirs(outRight)
            exposures = glob.glob(proj+"/*")
            numPhotos = len(glob.glob(exposures[0]+"/left/*"))
            
            
            
            for i in range(0,numPhotos):
                photos = []
                for e in exposures:
                    photos.append(glob.glob(e+"/left/*")[i])
                
                cmd = "ActiveLighting4 threshold "+outLeft+"/"+os.path.split(photos[0])[1]+" "+cert+" "
                for p in photos:
                    cmd += p+" "
                execute(cmd)
                
                
                photos = []
                for e in exposures:
                    photos.append(glob.glob(e+"/right/*")[i])
                
                cmd = "ActiveLighting4 threshold "+outRight+"/"+os.path.split(photos[0])[1]+" "+cert+" "
                for p in photos:
                    cmd += p+" "
                execute(cmd)
            
                
        
        answer = ""
        if(args.auto):
            step = "decode"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "decode"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "threshold"
    
    while(step == "decode"):
        decodeDir = os.path.join(scenedir,"computed/decode")
        safemkdirs(decodeDir)
     
     
        photoDirs = glob.glob(scenedir+"computed/threshold/*")
        print scenedir
        for dir in photoDirs:
            out = decodeDir+"/"+os.path.split(dir)[1]
            outLeft = out+"/left"
            outRight = out+"/right"
            safemkdirs(outLeft)
            safemkdirs(outRight)
            
            codefile = args.code
            if(codefile == None):
                codefile = raw_input("Code file?")
            
            phs = glob.glob(dir+"/left/*")
            naturalsort(phs)
            cmd = "ActiveLighting4 decode "+outLeft+" "+codefile
            for p in phs:
                cmd += " "+p
            execute(cmd)
            
            phs = glob.glob(dir+"/right/*")
            naturalsort(phs)
            cmd = "ActiveLighting4 decode "+outRight+" "+codefile
            for p in phs:
                cmd += " "+p
            execute(cmd)
         
        answer = ""
        if(args.auto):
            step = "disparity"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "disparity"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "decode"
                
    while(step == "disparity"):
        dispDir = os.path.join(scenedir,"computed/disparity")
        safemkdirs(dispDir)
        dirs = glob.glob(os.path.join(scenedir,"computed/decode")+"/*")
        
        xmin = args.xmin
        xmax = args.xmax
        ymin = args.ymin
        ymax = args.ymax
        
        
        
        if(args.xmin == None):
            xmin = raw_input("xmin?")
        if(args.xmax == None):   
            xmax = raw_input("xmax?")
        if(args.ymin == None):    
            ymin = raw_input("ymin?")
        if(args.ymax == None):    
            ymax = raw_input("ymax?")
            
        config['DisparityXMin'] = xmin
        config['DisparityXMax'] = xmax
        config['DisparityYMin'] = ymin
        config['DisparityYMax'] = ymax
        
        config["VisualizationMaximumDepth"] = vmax
        config["VisualizationMinimumDepth"] = vmin
        
        for dir in dirs:
            outputdir = dispDir+"/"+os.path.split(dir)[1]
            safemkdirs(outputdir)
            if not args.visonly:
                execute("ActiveLighting4 disparity "+outputdir+" "+dir+"/left/result.flo "+dir+"/right/result.flo "+xmin+" "+xmax+" "+ymin+" "+ymax)
            VisColor(outputdir, "Xout0.flo", "out0Colorx.ppm", "out0Colory.ppm")
            VisGrey(outputdir, "Xout0.flo", "out0Greyx.pgm", "out0Greyy.pgm", reverse = True)
            
            VisColor(outputdir, "Xout1.flo", "out1Colorx.ppm", "out1Colory.ppm")
            VisGrey(outputdir, "Xout1.flo", "out1Greyx.pgm", "out1Greyy.pgm")
            
           
        answer = ""
        if(args.auto):
            step = "reproject"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "reproject"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "disparity"
            
    while(step == "reproject"):
        reDir = os.path.join(scenedir,"computed/reproject")
        codeDir = os.path.join(scenedir,"computed/decode")
        safemkdirs(reDir)
        dirs = glob.glob(os.path.join(scenedir,"computed/disparity")+"/*")
        
        xmin = args.rxmin
        xmax = args.rxmax
        ymin = args.rymin
        ymax = args.rymax
        
        if(args.rxmin == None):
            xmin = raw_input("rxmin?")
        if(args.rxmax == None):   
            xmax = raw_input("rxmax?")
        if(args.rymin == None):    
            ymin = raw_input("rymin?")
        if(args.rymax == None):    
            ymax = raw_input("rymax?")
        
        config['ReprojectionMaskXMin'] = xmin
        config['ReprojectionMaskXMax'] = xmax
        config['ReprojectionMaskYMin'] = ymin
        config['ReprojectionMaskYMax'] = ymax
        
        config["VisualizationMaximumDepth"] = vmax
        config["VisualizationMinimumDepth"] = vmin
        
        edges = " "+xmin+" "+xmax+" "+ymin+" "+ymax
        
        for dir in dirs:
            projector = os.path.split(dir)[1]
            outputdir = reDir+"/"+projector
            outLeft = outputdir+"/"+projector+"cam0.flo"
            outRight = outputdir+"/"+projector+"cam1.flo"
            safemkdirs(outputdir)
            if not args.visonly:
                execute("ActiveLighting4 reproject "+outLeft+" "+dir+"/Xout0.flo "+codeDir+"/"+projector+"/left/result.flo"+edges)
            execute("FloVis "+outLeft+" "+outputdir+"/"+projector+"cam0.ppm")
            execute("FloVis -g "+outLeft+" "+outputdir+"/"+projector+"cam0.pgm")
            VisColor(outputdir, projector+"cam0.flo", projector+"cam0x.ppm", projector+"cam0y.ppm")
            VisGrey(outputdir, projector+"cam0.flo", projector+"cam0x.pgm", projector+"cam0y.pgm", reverse = True)
            
            if not args.visonly:
                execute("ActiveLighting4 reproject "+outRight+" "+dir+"/Xout1.flo "+codeDir+"/"+projector+"/right/result.flo"+edges)
            VisColor(outputdir, projector+"cam1.flo", projector+"cam1x.ppm", projector+"cam1y.ppm")
            VisGrey(outputdir, projector+"cam1.flo", projector+"cam1x.pgm", projector+"cam1y.pgm")
            
        answer = ""
        if(args.auto):
            step = "merge"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "merge"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "reproject"
            
             
    while(step == "merge"):   
        mergeDir = os.path.join(scenedir,"computed/merge")
        reproDir = os.path.join(scenedir,"computed/reproject")
        dispDir = os.path.join(scenedir,"computed/disparity")
        safemkdirs(mergeDir)
        disps = glob.glob(dispDir+"/*")
        repros = glob.glob(reproDir+"/*")
        mL = " "+mergeDir+"/left.flo"
        mR = " "+mergeDir+"/right.flo"

        gs = args.gs
        if(gs == None):
            gs = raw_input("Group Size?")
        
        config["MergeConsensusGroupMinimumSize"] = gs
        config["VisualizationMaximumDepth"] = vmax
        config["VisualizationMinimumDepth"] = vmin
        for d in disps:
            mL += " "+d+"/Xout0.flo"
            mR += " "+d+"/Xout1.flo"
            
        for r in repros:
            mL += " "+r+"/"+os.path.split(r)[1]+"cam0.flo"
            mR += " "+r+"/"+os.path.split(r)[1]+"cam1.flo"
            
        cmd = "ActiveLighting4 merge "+gs+" "
        if not args.visonly:
            execute(cmd+mL)
        VisColor(mergeDir, "left.flo", "leftx.ppm", "lefty.ppm")
        VisGrey(mergeDir, "left.flo", "leftx.pgm", "lefty.pgm", reverse = True)
        
        cmd = "ActiveLighting4 merge "+gs+" "
        if not args.visonly:
            execute(cmd+mR)
        VisColor(mergeDir, "right.flo", "rightx.ppm", "righty.ppm")
        VisGrey(mergeDir, "right.flo", "rightx.pgm", "righty.pgm")
        
        
        step = None
        
        answer = ""
        
        if(args.auto):
            step = "buildsite"
        else:
            answer = raw_input("Keep Going? (yes/no/repeat) ")
        
        if(answer == "yes" or answer == "y" or answer == ""):
            step = "buildsite"
        if(answer == "no" or answer == "n"):
            cleanExit(config, configpath)
        if(answer == "repeat" or answer == "r"):
            step = "merge"
            
#    while(step == "buildsite" and args.sitedir != None):
#        execute("ActiveLightingBuildSite.py "+scenedir+" "+args.sitedir)
#        config["WebsiteDirectory"] = args.sitedir
#        answer = ""
#        
#        if(args.auto):
#            step = "none"
#        else:
#            answer = raw_input("Keep Going? (yes/no/repeat) ")
#        
#        if(answer == "yes" or answer == "y" or answer == ""):
#            step = "none"
#        if(answer == "no" or answer == "n"):
#            cleanExit(config, configpath)
#        if(answer == "repeat" or answer == "r"):
#            step = "buildsite" 
            
    cleanExit(config, configpath) 
                
                
                
 
 
 

               
                
                
    