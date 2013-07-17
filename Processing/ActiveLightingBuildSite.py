#!/usr/bin/python

import glob
import os
import sys
import argparse

parser = argparse.ArgumentParser(description="Build websites to display images processed by ActiveLighting4")
parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. Expects the normal ActiveLighting4 directory structure.")
parser.add_argument("outputDirectory", nargs=1, help="The folder in which to build the website.")
parser.add_argument("-t", "--toc", dest="toc", action="store_const", const=True, default=False, help="""
switch to Table of Contents mode. In this mode, the "targetScene" directory is assumed to contain a number of already generated websites. A TOC for these sites is placed in outputDir
""")


args = parser.parse_args(sys.argv[1:])


def convert(path1, path2, size = None):
    if(not os.path.exists(path2)):
        if(size != None):
            cmd = "convert -resize "+size+" "+path1+" "+path2
        else:
            cmd = "convert "+path1+" "+path2
        
        print cmd
        os.system(cmd)

def removeTrailingSlash(path):
    if(path[len(path)-1] == '/'):
        return path[:-1]
    return path

src = args.targetScene[0]
dst = args.outputDirectory[0]
src = removeTrailingSlash(src)
dst = removeTrailingSlash(dst)

if args.toc:
    
    dirs = glob.glob(src+"/*")
    mainfile = open(dst+"/index.html", "w")
    mainfile.write("""
    
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>

<title>
Table of Contents
</title>
</head>

<h1>Table of Contents</h1>
<br/><br/>

""")
    dirs.sort()
    for dir in dirs:
        if(os.path.isdir(dir)):
            index = dir+"/index.html"
            index = os.path.relpath(index, dst)
            leftImage = dir+"/imgs/b0s0r1.jpg"
            leftImage = os.path.relpath(leftImage,dst)
            rightImage = dir+"/imgs/b0s1r1.jpg"
            rightImage = os.path.relpath(rightImage,dst)
            name = os.path.split(dir)[1]
            mainfile.write("""
            <h3><a href="%s">%s</a><br/><br/></h3>
            <a href="%s"><img width = "500" src="%s" name="%s" onmouseover="document.%s.src='%s';" alt="" onmouseout="document.%s.src='%s';"></a><br/>
            
            
            """%(index,name,index,leftImage, name, name, rightImage, name, leftImage))
        
    mainfile.write("<br/><br/><br/></html>")
    exit(0)










if(not os.path.exists(dst+"/imgs")):
    os.makedirs(dst+"/imgs")

scenename = os.path.split(src)[1]



decodes = glob.glob(src+"/computed/decode/*")
numprojectors = len(decodes)
for projector in decodes:
    num = os.path.split(projector)[1][1:]
    convert(projector+"/left/cresult0.ppm ",dst+"/imgs/p"+num+"m0s0r0g0x0.jpg")

    convert(projector+"/right/cresult0.ppm ",dst+"/imgs/p"+num+"m0s1r0g0x0.jpg")
   
    convert( projector+"/left/cresult0.ppm ",dst+"/imgs/p"+num+"m0s0r1g0x0.jpg", size="926x617")

    convert( projector+"/right/cresult0.ppm ",dst+"/imgs/p"+num+"m0s1r1g0x0.jpg", size="926x617")
    
    convert(projector+"/left/result0.pgm ",dst+"/imgs/p"+num+"m0s0r0g1x0.jpg")

    convert(projector+"/right/result0.pgm ",dst+"/imgs/p"+num+"m0s1r0g1x0.jpg")
   
    convert( projector+"/left/result0.pgm ",dst+"/imgs/p"+num+"m0s0r1g1x0.jpg", size="926x617")

    convert( projector+"/right/result0.pgm ",dst+"/imgs/p"+num+"m0s1r1g1x0.jpg", size="926x617")
    
    convert(projector+"/left/cresult1.ppm ",dst+"/imgs/p"+num+"m0s0r0g0x1.jpg")

    convert(projector+"/right/cresult1.ppm ",dst+"/imgs/p"+num+"m0s1r0g0x1.jpg")
   
    convert( projector+"/left/cresult1.ppm ",dst+"/imgs/p"+num+"m0s0r1g0x1.jpg", size="926x617")

    convert( projector+"/right/cresult1.ppm ",dst+"/imgs/p"+num+"m0s1r1g0x1.jpg", size="926x617")
    
    convert(projector+"/left/result1.pgm ",dst+"/imgs/p"+num+"m0s0r0g1x1.jpg")

    convert(projector+"/right/result1.pgm ",dst+"/imgs/p"+num+"m0s1r0g1x1.jpg")
   
    convert( projector+"/left/result1.pgm ",dst+"/imgs/p"+num+"m0s0r1g1x1.jpg", size="926x617")

    convert( projector+"/right/result1.pgm ",dst+"/imgs/p"+num+"m0s1r1g1x1.jpg", size="926x617")
  
    
depths = glob.glob(src+"/computed/disparity/*")
for projector in depths:
    num = os.path.split(projector)[1][1:]
    convert(projector+"/out0Colorx.ppm ",dst+"/imgs/p"+num+"m1s0r0g0x0.jpg")

    convert(projector+"/out1Colorx.ppm ",dst+"/imgs/p"+num+"m1s1r0g0x0.jpg")
   
    convert( projector+"/out0Colorx.ppm ",dst+"/imgs/p"+num+"m1s0r1g0x0.jpg", size="926x617")

    convert( projector+"/out1Colorx.ppm ",dst+"/imgs/p"+num+"m1s1r1g0x0.jpg", size="926x617")
    
    convert(projector+"/out0Greyx.pgm ",dst+"/imgs/p"+num+"m1s0r0g1x0.jpg")

    convert(projector+"/out1Greyx.pgm ",dst+"/imgs/p"+num+"m1s1r0g1x0.jpg")
   
    convert( projector+"/out0Greyx.pgm ",dst+"/imgs/p"+num+"m1s0r1g1x0.jpg", size="926x617")

    convert( projector+"/out1Greyx.pgm ",dst+"/imgs/p"+num+"m1s1r1g1x0.jpg", size="926x617")
    
    convert(projector+"/out0Colory.ppm ",dst+"/imgs/p"+num+"m1s0r0g0x1.jpg")

    convert(projector+"/out1Colory.ppm ",dst+"/imgs/p"+num+"m1s1r0g0x1.jpg")
   
    convert( projector+"/out0Colory.ppm ",dst+"/imgs/p"+num+"m1s0r1g0x1.jpg", size="926x617")

    convert( projector+"/out1Colory.ppm ",dst+"/imgs/p"+num+"m1s1r1g0x1.jpg", size="926x617")
    
    convert(projector+"/out0Greyy.pgm ",dst+"/imgs/p"+num+"m1s0r0g1x1.jpg")

    convert(projector+"/out1Greyy.pgm ",dst+"/imgs/p"+num+"m1s1r0g1x1.jpg")
   
    convert( projector+"/out0Greyy.pgm ",dst+"/imgs/p"+num+"m1s0r1g1x1.jpg", size="926x617")

    convert( projector+"/out1Greyy.pgm ",dst+"/imgs/p"+num+"m1s1r1g1x1.jpg", size="926x617")
    
repros =  glob.glob(src+"/computed/reproject/*") 
for projector in repros:
    num = os.path.split(projector)[1][1:]
    convert( projector+"/P"+num+"cam0x.ppm ",dst+"/imgs/p"+num+"m2s0r0g0x0.jpg")

    convert(projector+"/P"+num+"cam1x.ppm ",dst+"/imgs/p"+num+"m2s1r0g0x0.jpg")
   
    convert( projector+"/P"+num+"cam0x.ppm ",dst+"/imgs/p"+num+"m2s0r1g0x0.jpg", size="926x617")

    convert( projector+"/P"+num+"cam1x.ppm ",dst+"/imgs/p"+num+"m2s1r1g0x0.jpg", size="926x617")
    
    convert( projector+"/P"+num+"cam0x.pgm ",dst+"/imgs/p"+num+"m2s0r0g1x0.jpg")

    convert(projector+"/P"+num+"cam1x.pgm ",dst+"/imgs/p"+num+"m2s1r0g1x0.jpg")
   
    convert( projector+"/P"+num+"cam0x.pgm ",dst+"/imgs/p"+num+"m2s0r1g1x0.jpg", size="926x617")

    convert( projector+"/P"+num+"cam1x.pgm ",dst+"/imgs/p"+num+"m2s1r1g1x0.jpg", size="926x617")
    
    convert( projector+"/P"+num+"cam0y.ppm ",dst+"/imgs/p"+num+"m2s0r0g0x1.jpg")

    convert(projector+"/P"+num+"cam1y.ppm ",dst+"/imgs/p"+num+"m2s1r0g0x1.jpg")
   
    convert( projector+"/P"+num+"cam0y.ppm ",dst+"/imgs/p"+num+"m2s0r1g0x1.jpg", size="926x617")

    convert( projector+"/P"+num+"cam1y.ppm ",dst+"/imgs/p"+num+"m2s1r1g0x1.jpg", size="926x617")
    
    convert( projector+"/P"+num+"cam0y.pgm ",dst+"/imgs/p"+num+"m2s0r0g1x1.jpg")

    convert(projector+"/P"+num+"cam1y.pgm ",dst+"/imgs/p"+num+"m2s1r0g1x1.jpg")
   
    convert( projector+"/P"+num+"cam0y.pgm ",dst+"/imgs/p"+num+"m2s0r1g1x1.jpg", size="926x617")

    convert( projector+"/P"+num+"cam1y.pgm ",dst+"/imgs/p"+num+"m2s1r1g1x1.jpg", size="926x617")


ambients = glob.glob(src+"/orig/ambient/*/left/*.JPG")
ambients.sort()
numambs = len(ambients)
index = 0
for photo in ambients:
    convert(photo, dst+"/imgs/a"+`index`+"s0r0.jpg")
    convert(photo, dst+"/imgs/a"+`index`+"s0r1.jpg", size="926x617")
    index += 1
    
ambients = glob.glob(src+"/orig/ambient/*/right/*.JPG")
ambients.sort()
index = 0
for photo in ambients:
    convert(photo, dst+"/imgs/a"+`index`+"s1r0.jpg")
    convert(photo, dst+"/imgs/a"+`index`+"s1r1.jpg", size="926x617")
    index += 1


mergedir = src+"/computed/merge/"

convert(mergedir+"leftx.ppm", dst+"/imgs/ms0r0g0x0.jpg")
convert(mergedir+"rightx.ppm", dst+"/imgs/ms1r0g0x0.jpg")
convert(mergedir+"leftx.pgm", dst+"/imgs/ms0r0g1x0.jpg")
convert(mergedir+"rightx.pgm", dst+"/imgs/ms1r0g1x0.jpg")
convert(mergedir+"leftx.ppm", dst+"/imgs/ms0r1g0x0.jpg", size="926x617")
convert(mergedir+"rightx.ppm", dst+"/imgs/ms1r1g0x0.jpg", size="926x617")
convert(mergedir+"leftx.pgm", dst+"/imgs/ms0r1g1x0.jpg", size="926x617")
convert(mergedir+"rightx.pgm", dst+"/imgs/ms1r1g1x0.jpg", size="926x617")
convert(mergedir+"lefty.ppm", dst+"/imgs/ms0r0g0x1.jpg")
convert(mergedir+"righty.ppm", dst+"/imgs/ms1r0g0x1.jpg")
convert(mergedir+"lefty.pgm", dst+"/imgs/ms0r0g1x1.jpg")
convert(mergedir+"righty.pgm", dst+"/imgs/ms1r0g1x1.jpg")
convert(mergedir+"lefty.ppm", dst+"/imgs/ms0r1g0x1.jpg", size="926x617")
convert(mergedir+"righty.ppm", dst+"/imgs/ms1r1g0x1.jpg", size="926x617")
convert(mergedir+"lefty.pgm", dst+"/imgs/ms0r1g1x1.jpg", size="926x617")
convert(mergedir+"righty.pgm", dst+"/imgs/ms1r1g1x1.jpg", size="926x617")


index = 0    
rect = glob.glob(src+"/computed/rectifiedAmbient/left/*.ppm")
rect.sort()
for photo in rect:
    convert(photo, dst+"/imgs/b"+`index`+"s0r0.jpg")
    convert(photo, dst+"/imgs/b"+`index`+"s0r1.jpg", size = "926x617")
    index += 1
    
index = 0    
rect = glob.glob(src+"/computed/rectifiedAmbient/right/*.ppm")
rect.sort()
for photo in rect:
    convert(photo, dst+"/imgs/b"+`index`+"s1r0.jpg")
    convert(photo, dst+"/imgs/b"+`index`+"s1r1.jpg", size = "926x617")
    index += 1


os.system("cp "+src+"/computed/config.txt "+dst)

print "Writing index.html..."
mainfile = open(dst+"/index.html", "w")

mainfile.write("""

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>

<script type="text/javascript">

var projector = 1;
var numprojectors = %d
var mode = 0;
var modes = ["Code", "Depth", "Reprojection" ];
var side = 0;
var sides = ["Left", "Right"];
var resolution = 1;

var grey = 0;
var greys = ["Color", "Grey"];

var direction = 0;
var directions = ["X", "Y"];

var ambient = 0;
var ambtotal = %d;
var ambindex = 0;


var merge = 0;

var rect = 0;
var rectindex = 0;

function keyInput(e) {
    if (!e) var e = window.event;
    var code;
    if (e.keyCode)
    code = e.keyCode;
    else if (e.which)
    code = e.which;
    
    if(code == 48){
       projector = ((projector)%%(numprojectors))+1
       ambient = 0;
       merge = 0;
    }
    if (code >= 49 && code <= 57 && code-48<=numprojectors) { // projector number
        projector = code-48;
        ambient = 0;
        merge = 0;
        rect = 0;
    } else if (code ==  76) { // L
        side = 0;
    }else if (code ==  82) { // R
        side = 1;
    }else if (code ==  67) { // C
        mode = 0;
    }else if (code ==  68) { // D
        mode = 1;
    }else if (code ==  80) { // P
        mode = 2;
     }else if (code ==  83) { // s
        resolution = (resolution+1)%%2;
    }else if (code ==  71) { // G
        grey = (grey+1)%%2;
    }else if (code ==  88) { // X
        direction = (direction+1)%%2;
    }else if (code ==  37 && ambient == 1) { // left arrow
        ambindex = (ambindex-1);
        if(ambindex < 0){
            ambindex = ambtotal-1;
        }
    }else if (code == 39 && ambient == 1){
        ambindex = (ambindex+1)%%ambtotal; 

    }else if (code ==  37 && rect == 1) { // left arrow
        rectindex = (rectindex-1);
        if(rectindex < 0){
            rectindex = ambtotal-1;
        }
    
    }else if (code == 39 && rect == 1){
        rectindex = (rectindex+1)%%ambtotal; 

    }else if (code ==  65) { // A
        ambient = (ambient+1)%%2;
        merge = 0;
        rect = 0;
    }else if (code ==  77) { // M
        merge = (merge+1)%%2;
        ambient = 0;
        rect = 0;
    }else if (code ==  66) { // B
        rect = (rect+1)%%2;
        ambient = 0;
        merge = 0;
    }
    
    showImg();
}


function showImg(){

    
    if(ambient == 1){
        document.getElementById("title").innerHTML = "Ambient "+ambindex+" "+sides[side];
        document.getElementById("show").src = "imgs/"+"a"+ambindex+"s"+side+"r"+resolution+".jpg";
        document.getElementById("show").alt = "imgs/"+"a"+ambindex+"s"+side+"r"+resolution+".jpg is not available";
    }else if(merge){
        document.getElementById("title").innerHTML = "Merged "+sides[side]+" "+greys[grey]+" "+directions[direction] ;
        document.getElementById("show").src = "imgs/"+"ms"+side+"r"+resolution+"g"+grey+"x"+direction+".jpg";
        document.getElementById("show").alt = "imgs/"+"ms"+side+"r"+resolution+"g"+grey+"x"+direction+".jpg is not available";
    }else if(rect){
        document.getElementById("title").innerHTML = "Rectified Ambient "+rectindex+" "+sides[side];
        document.getElementById("show").src = "imgs/"+"b"+rectindex+"s"+side+"r"+resolution+".jpg";
        document.getElementById("show").alt = "imgs/"+"b"+rectindex+"s"+side+"r"+resolution+".jpg is not available";
    }else{
        document.getElementById("title").innerHTML = "Projector "+projector+" "+modes[mode]+" "+sides[side]+" "+greys[grey]+" "+directions[direction];
        document.getElementById("show").src = "imgs/"+"p"+projector+"m"+mode+"s"+side+"r"+resolution+"g"+grey+"x"+direction+".jpg";
        document.getElementById("show").alt = "imgs/"+"p"+projector+"m"+mode+"s"+side+"r"+resolution+"g"+grey+"x"+direction+".jpg is not available";
    }
}






</script>

<title>
%s
</title>
</head>


<body onkeydown='keyInput(event);' onLoad='showImg();'>
<h3 id="title"> empty </h3>
<img id="show" src=""  alt="None" />
<br/>
<br/>

<a href="config.txt">Parameters Used</a><br/>

Instructions:<br/>
1-9 = Select projector<br/>
0 = Step through projectors (allows access to projectors greater than 9)<br/>
r,l = Choose left or right camera<br/>
c = code visualizations<br/>
d = depth visualization<br/>
p = projection visualization<br/>
x = switch between x and y visualization<br/>
g = switch between grey and color visualizations<br/>
a = ambient images<br/>
m = merged images<br/>
b = rectified ambient images<br/>
left arrow, right arrow = move through ambient or merged images<br/>
s = switch between full resolution and web-friendly resolution<br/>
</body>
</html>


"""%(numprojectors,numambs, scenename))

print "Done"
    

    



