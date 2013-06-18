 #!/usr/bin/python

'''
Active Lighting control script to control all steps for Active Lighting process.
Original version works for file system 2011. In Summer 2012, we use new file system this year,
thus this script is adjusted to fit for the new file system.
For more information, see Active Lighting 2012 Instruction.

Created on Jun 28, 2011
Edited on 2012.6 by Nera Nesic for intermediate result directory
Edited on 2012.8.3

Original Author: wwestlin
Edited Author: Xi Wang

<<<<<<< HEAD
=======

Edited 2013 Authors: Greg & York


>>>>>>> c390d8ede73b2afd487d82ccfaf4661c89b0e917
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
    
    execute("FloVis/FloVis -s "+speed+" -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)

def VisGrey(dir, input,outputx, outputy, reverse = False):
    if(reverse):
        execute("FloVis/FloVis -g -r -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)
    else:
        execute("FloVis/FloVis -g -i "+vmin+" -x "+vmax+" "+dir+"/"+input+" "+dir+"/"+outputx+" "+dir+"/"+outputy)
    
def cleanExit(config, configpath):
    json.dump(config, open(configpath, 'w'), sort_keys=True, indent = 4 )
    exit(0)
    
if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description="Control the processing pipeline for ActiveLighting4")
    #parser.add_argument("-pp" "--path to processing programs",  
    parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. See README for directory structure expected.")
    parser.add_argument("-v", "--visualize", dest="visualize", action="store_const", const=True, default=False, help="Visualize processing at every step")
    parser.add_argument("-m", "--intermediateRes", dest="intermediateRes", action="store_const", const=True, default=False, help="Store the intermediary results in the scene folder rather than on a temp location. Changed by Nera Nesic")
    
    parser.add_argument("-a", "--auto", dest="auto", action="store_const", const=True, default=False, help="Do not ask for confirmation before going to the next step")
    parser.add_argument("-q", "--query", dest="query", action="store_const", const=True, default=False, help="Query for processing parameters at each step, rather than using defaults or options")
    parser.add_argument("-s", "--step", dest="step", default="calibrate", help="The step of the pipeline to start from. See README for steps. Errors may occur if a step is chosen which expects files or folder which have not yet been computed")
    parser.add_argument("-ip","--intermediatePlace", dest="intermediatePlace", default="/home2/tmp/intermediates", help="The place for intermediate result. Default to '/home2/tmp/intermediates'")
    
    parser.add_argument("-i", "--width", dest="width", default="17", help="The width of the calibration pattern used. Defaults to 17")
    parser.add_argument("-j", "--height", dest="height", default="12", help="The height of the calibration pattern used. Defaults to 12")
    
    #parser.add_argument("-c", "--code", dest="code", default="../codes/minSW.dat", help="The code file to use for decoding. Defaults to '../codes/minSW.dat' ")
    #parser.add_argument("-c", "--code", dest="code", default="/home/nnesic/public_html/VisionResearch/research2012/codes/minSW.dat", help="The code file to use for decoding. Changed by Nera Nesic.")
    parser.add_argument("-c", "--code", dest="code", default="minSW.dat", help="The code file to use for decoding. Should be in same directory as script.")

    parser.add_argument("-t", "--certainty", dest="cert", default="115", help="The certainty threshold to use during thresholding. Defaults to 115.")
    
    parser.add_argument("-xi", "--xmin", dest="xmin", default="-1000", help="X minimum for disparity search. Defaults to -1000")
    parser.add_argument("-xx", "--xmax", dest="xmax", default="1000", help="X maximum for disparity search. Defaults to 1000")
    parser.add_argument("-yi", "--ymin", dest="ymin", default="-5", help="Y minimum for disparity search. Defaults to -5")
    parser.add_argument("-yx", "--ymax", dest="ymax", default="5", help="Y maximum for disparity search. Defaults to 5")
    
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
	exit(0)
    if(not scenedir[len(scenedir)-1] == '/'):
        scenedir += '/'
        
    #temporary directory for intermediary results  
    if (args.intermediateRes):
        tempdir = scenedir
    else:
        #here we set home2 as place for intermediary results.
        #For more information, see Active Lighting 2012 Instruction 
        tempdir = args.intermediatePlace
        p = re.compile("/\w*/$")
        t = p.findall(scenedir)

        # if can't use home2, or otherwise fails, use -m
        try:
            tempdir = tempdir +"" + t[0]
            safemkdirs(tempdir)      
        except:
            print("* * * * * * * * ")
            print("Error: Default temp directory didnt work. Storing temp files in scene directory.")
            tempdir = scenedir
            safemkdirs(tempdir) 
        
    
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
        
    #this calibration step only works for 2 DSLR cameras
    #for point-shoot cameras, please see Control/calibScript.py
    
    while(step == "calibrate"):
        calibDir = os.path.join(scenedir,"orig/calibration")
        #confidenceDir = os.path.join(scenedir,"computed/calibration")
        confidenceDir = os.path.join(scenedir,"computed/calibration/left2right")
        safemkdirs(confidenceDir)
        leftDir = os.path.join(calibDir,"left/*.JPG")
        rightDir = os.path.join(calibDir,"right/*.JPG")

        #check for images in leftDir and rightDir (this could be done for other steps too)
        leftIms = glob.glob(leftDir)
        rightIms = glob.glob(rightDir)
        if len(leftIms) != len(rightIms):
            print("Error: Left and right calibration folders do not contain same number of images.\n")
            exit(0)
        while (leftIms == 0 or rightIms == 0):
            print("No JPG images found in calibration folders...")
            subdirs = os.listdir(leftIms)
            for thing in subdirs:
		        if (os.path.isdir(os.path.join(leftIms, thing))):
		            print("Subdirectory " + thing + " found. Searching for images within this subdirectory.")
		            os.path.join(leftIms, thing)
		            os.path.join(rightIms, thing)
									
	    contin = raw_input()
        
        w = args.width
        h = args.height
        
        if(w == None):
            w = raw_input("Pattern width?")
        if(h == None):
            h = raw_input("Pattern height?")
        config['PatternWidth'] = w
        config['PatternHeight'] = h
        execute("ActiveLighting/ActiveLighting4 calibrate 0 "+w+" "+h+" "+confidenceDir+" "+leftDir+" "+rightDir)
        
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
        rectifiedDir = os.path.join(tempdir,"computed/rectified")
        safemkdirs(rectifiedDir)
        
        intrinsics = [os.path.join(scenedir,"computed/calibration/left2right/Intrinsicsleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Intrinsicsright.xml")]
        distortion = [os.path.join(scenedir,"computed/calibration/left2right/Distortionleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Distortionright.xml")]
        rotation = [os.path.join(scenedir,"computed/calibration/left2right/Rotationleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Rotationright.xml")]
        projection = [os.path.join(scenedir,"computed/calibration/left2right/Projectionleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Projectionright.xml")]
        
        for dir in photoDirs:
            match = re.match(".*take([0-9]+)exp([0-9]+)P([0-9]+)",dir)
            if match:
                confidenceDir = os.path.join(rectifiedDir,"P"+`(int(match.group(3))+2*(int(match.group(1))-1))`+"/exp"+match.group(2))
                outLeft = confidenceDir+"/left"
                safemkdirs(outLeft)
                execute("ActiveLighting/ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+dir+"/left/*")
                
                outRight = confidenceDir+"/right"
                safemkdirs(outRight)
                execute("ActiveLighting/ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+dir+"/right/*")
                    
        
        
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
        #read .xml files
        intrinsics = [os.path.join(scenedir,"computed/calibration/left2right/Intrinsicsleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Intrinsicsright.xml")]
        distortion = [os.path.join(scenedir,"computed/calibration/left2right/Distortionleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Distortionright.xml")]
        rotation = [os.path.join(scenedir,"computed/calibration/left2right/Rotationleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Rotationright.xml")]
        projection = [os.path.join(scenedir,"computed/calibration/left2right/Projectionleft.xml"), os.path.join(scenedir,"computed/calibration/left2right/Projectionright.xml")]
        
        #rectify ambient images Edited on June 12, 2013 by York Kitajima
        rectambdir = os.path.join(scenedir,"computed/rectifiedAmbient")
        photoDirs = glob.glob(os.path.join(scenedir, "orig/ambient/*"))
        safemkdirs(rectambdir)
        for dir in photoDirs:
            print dir
            match = re.match(".*L([0-9]+)",dir)
            if match:
                lightingdir = os.path.join(rectambdir, "L"+match.group(1))
                outLeft = lightingdir+"/left"
                safemkdirs(outLeft)
                execute("ActiveLighting/ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+dir+"/left/*.JPG")

                outRight = lightingdir+"/right"
                safemkdirs(outRight)
                execute("ActiveLighting/ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+dir+"/right/*.JPG")
        

        #rectify ambient with ball images if exist
        if(os.path.exists(os.path.join(scenedir,"orig/ambientBall"))):
            rectambwbdir = os.path.join(scenedir,"computed/rectifiedAmbientBall") 
            photoDirs = glob.glob(os.path.join(scenedir, "orig/ambientBall/*"))
            safemkdirs(rectambwbdir)
            for dir in photoDirs:
                print dir
                match = re.match(".*L([0-9]+)",dir)
                if match:
                    lightingdir = os.path.join(rectambwbdir, "L"+match.group(1))
                    outLeft = lightingdir+"/left"
                    safemkdirs(outLeft)
                    execute("ActiveLighting/ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+dir+"/left/*.JPG")

                    outRight = lightingdir+"/right"
                    safemkdirs(outRight)
                    execute("ActiveLighting/ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+dir+"/right/*.JPG")
        

        else:
	        print("No ambient ball images found, continuing")

        
        #rectify perturbed ambient images if exist
        #for some scenes, we do not have this. In summer 2012, Classroom1 and Garage secen have this folder.
        if(os.path.exists(os.path.join(scenedir,"orig/ambientOccluders"))):
            rectambodir = os.path.join(scenedir,"computed/rectifiedAmbientOccluders") 
            safemkdirs(rectambodir)
            outLeft = rectambodir+"/left"
            outRight = rectambodir+"/right"
            safemkdirs(outLeft)
            safemkdirs(outRight)
            execute("ActiveLighting/ActiveLighting4 rectify "+outLeft+" "+intrinsics[0]+" "+distortion[0]+" "+rotation[0]+" "+projection[0]+" "+os.path.join(scenedir,"orig/ambientOccluders/L*/left/*.JPG"))
            execute("ActiveLighting/ActiveLighting4 rectify "+outRight+" "+intrinsics[1]+" "+distortion[1]+" "+rotation[1]+" "+projection[1]+" "+os.path.join(scenedir,"orig/ambientOccluders/L*/right/*.JPG"))
        
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
        confidenceDir = os.path.join(tempdir,"computed/confidence")
        safemkdirs(confidenceDir)
        
        photoFolders = glob.glob(os.path.join(tempdir,"computed/rectified")+"/*/*/*")
        
        for folder in photoFolders:
            dirs = folder.split("/")
            outputDir = confidenceDir+"/"+dirs[len(dirs)-3]+"/"+dirs[len(dirs)-2]+"/"+dirs[len(dirs)-1]
            safemkdirs(outputDir)
            execute("ActiveLighting/ActiveLighting4 confidence "+outputDir+" "+folder+"/*")
        
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
        thresholdDir = os.path.join(tempdir,"computed/threshold")
        safemkdirs(thresholdDir)
        
        projectorFolders = glob.glob(os.path.join(tempdir,"computed/confidence")+"/*")
        
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
                
                cmd = "ActiveLighting/ActiveLighting4 threshold "+outLeft+"/"+os.path.split(photos[0])[1]+" "+cert+" "
                for p in photos:
                    cmd += p+" "
                execute(cmd)
                
                
                photos = []
                for e in exposures:
                    photos.append(glob.glob(e+"/right/*")[i])
                
                cmd = "ActiveLighting/ActiveLighting4 threshold "+outRight+"/"+os.path.split(photos[0])[1]+" "+cert+" "
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
        decodeDir = os.path.join(tempdir,"computed/decode")
        safemkdirs(decodeDir)
     
     
        photoDirs = glob.glob(tempdir+"computed/threshold/*")
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
            cmd = "ActiveLighting/ActiveLighting4 decode "+outLeft+" "+codefile
            for p in phs:
                cmd += " "+p
            execute(cmd)
            
            phs = glob.glob(dir+"/right/*")
            naturalsort(phs)
            cmd = "ActiveLighting/ActiveLighting4 decode "+outRight+" "+codefile
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
    
    #generate disparity of each projectors. In summer 2012 file system, it is as disparityintermediate.
    #but keep the step name as disparity             
    while(step == "disparity"):
        dispDir = os.path.join(scenedir,"computed/disparityintermediate")
        safemkdirs(dispDir)
        dirs = glob.glob(os.path.join(tempdir,"computed/decode")+"/*")
        
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
                execute("ActiveLighting/ActiveLighting4 disparity "+outputdir+" "+dir+"/left/result.flo "+dir+"/right/result.flo "+xmin+" "+xmax+" "+ymin+" "+ymax)
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
        reDir = os.path.join(tempdir,"computed/reproject")
        codeDir = os.path.join(tempdir,"computed/decode")
        safemkdirs(reDir)
        dirs = glob.glob(os.path.join(scenedir,"computed/disparityintermediate")+"/*")
        
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
                execute("ActiveLighting/ActiveLighting4 reproject "+outLeft+" "+dir+"/Xout0.flo "+codeDir+"/"+projector+"/left/result.flo"+edges)
            execute("FloVis/FloVis "+outLeft+" "+outputdir+"/"+projector+"cam0.ppm")
            execute("FloVis/FloVis -g "+outLeft+" "+outputdir+"/"+projector+"cam0.pgm")
            VisColor(outputdir, projector+"cam0.flo", projector+"cam0x.ppm", projector+"cam0y.ppm")
            VisGrey(outputdir, projector+"cam0.flo", projector+"cam0x.pgm", projector+"cam0y.pgm", reverse = True)
            
            if not args.visonly:
                execute("ActiveLighting/ActiveLighting4 reproject "+outRight+" "+dir+"/Xout1.flo "+codeDir+"/"+projector+"/right/result.flo"+edges)
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
            
    #this is disparity for file system 2012.
           
    while(step == "merge"):   
        mergeDir = os.path.join(scenedir,"computed/disparity")
        reproDir = os.path.join(tempdir,"computed/reproject")
        dispDir = os.path.join(scenedir,"computed/disparityintermediate")
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
            
        cmd = "ActiveLighting/ActiveLighting4 merge "+gs+" "
        if not args.visonly:
            execute(cmd+mL)
        VisColor(mergeDir, "left.flo", "leftx.ppm", "lefty.ppm")
        VisGrey(mergeDir, "left.flo", "leftx.pgm", "lefty.pgm", reverse = True)
        
        cmd = "ActiveLighting/ActiveLighting4 merge "+gs+" "
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
    print "Done with the scene and clean exit"      
    cleanExit(config, configpath) 
                
                
                
 
 
 

               
                
                
    
