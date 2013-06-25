'''
One scene Web page build fit for Active Lighting 2012 file system.
For more information, please see Active Lighting 2012 Instruction.

Author: Xi Wang
Date: 2012.8.3

'''

import glob
import os
import sys
import argparse
import re

parser = argparse.ArgumentParser(description="Build web sites to display images for analyzing. This script works on Ambient and Merge.")
parser.add_argument("targetScene", nargs=1, help="The scene folder to be processed. Expects the normal ActiveLighting4 directory structure.")
parser.add_argument("outputDirectory", nargs=1, help="The folder in which to build the web site.")

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

src = args.targetScene[0]
dst = args.outputDirectory[0]
src = removeTrailingSlash(src)
dst = removeTrailingSlash(dst)

safemkdirs(dst+"/imgs")
scenename = os.path.split(src)[1]


#change disparity
disparitydir = src+"/computed/disparity/"
safemkdirs(dst+"/imgs/disparity")

convert(disparitydir+"leftx.ppm", dst+"/imgs/disparity/leftx.jpg")
convert(disparitydir+"rightx.ppm", dst+"/imgs/disparity/rightx.jpg")
convert(disparitydir+"leftx.pgm", dst+"/imgs/disparity/leftxg.jpg")
convert(disparitydir+"rightx.pgm", dst+"/imgs/disparity/rightxg.jpg")
convert(disparitydir+"leftx.ppm", dst+"/imgs/disparity/leftx.s.jpg", size="600x400")
convert(disparitydir+"rightx.ppm", dst+"/imgs/disparity/rightx.s.jpg", size="600x400")
convert(disparitydir+"leftx.pgm", dst+"/imgs/disparity/leftxg.s.jpg", size="600x400")
convert(disparitydir+"rightx.pgm", dst+"/imgs/disparity/rightxg.s.jpg", size="600x400")
convert(disparitydir+"lefty.ppm", dst+"/imgs/disparity/lefty.jpg")
convert(disparitydir+"righty.ppm", dst+"/imgs/disparity/righty.jpg")
convert(disparitydir+"lefty.pgm", dst+"/imgs/disparity/leftyg.jpg")
convert(disparitydir+"righty.pgm", dst+"/imgs/disparity/rightyg.jpg")
convert(disparitydir+"lefty.ppm", dst+"/imgs/disparity/lefty.s.jpg", size="600x400")
convert(disparitydir+"righty.ppm", dst+"/imgs/disparity/righty.s.jpg", size="600x400")
convert(disparitydir+"lefty.pgm", dst+"/imgs/disparity/leftyg.s.jpg", size="600x400")
convert(disparitydir+"righty.pgm", dst+"/imgs/disparity/rightyg.s.jpg", size="600x400")

print "Done with disparity"

#this part is to get how many exposure there
for t in range (0, 12):
    tempdir = src + "/computed/rectifiedAmbient/L"+str(t)+"/left"
    if(os.path.exists(tempdir)):
        globdir=glob.glob(tempdir+"/*.ppm")
        index = 0
        for photo in globdir:
            index += 1
        exposure = index

#change rectifiedAmbient
safemkdirs(dst+"/imgs/rectifiedAmbient")
safemkdirs(dst+"/imgs/rectifiedAmbient/left")
safemkdirs(dst+"/imgs/rectifiedAmbient/right")

rect = glob.glob(src+"/computed/rectifiedAmbient/L*/left/*.ppm")
#rect.sort()
naturalsort(rect)
#print rect
index = 0    
for photo in rect:
    convert(photo, dst+"/imgs/rectifiedAmbient/left/image-"+`index`+".jpg")
    convert(photo, dst+"/imgs/rectifiedAmbient/left/image-"+`index`+".s.jpg", size = "600x400")
    index += 1

nrect=index
    
index = 0    
rect = glob.glob(src+"/computed/rectifiedAmbient/L*/right/*.ppm")
#rect.sort()
naturalsort(rect)
for photo in rect:
    convert(photo, dst+"/imgs/rectifiedAmbient/right/image-"+`index`+".jpg")
    convert(photo, dst+"/imgs/rectifiedAmbient/right/image-"+`index`+".s.jpg", size = "600x400")
    index += 1
    
#change rectified Ambient with ball
rectball = glob.glob(src+"/computed/rectifiedAmbientBall/L*/left/*.ppm")
naturalsort(rectball)
index = nrect
for photo in rectball:
    convert(photo, dst+"/imgs/rectifiedAmbient/left/image-"+`index`+".jpg")
    convert(photo, dst+"/imgs/rectifiedAmbient/left/image-"+`index`+".s.jpg", size = "600x400")
    index += 1
    
index = nrect
rectball = glob.glob(src+"/computed/rectifiedAmbientBall/L*/right/*.ppm")
naturalsort(rectball)
for photo in rectball:
    convert(photo, dst+"/imgs/rectifiedAmbient/right/image-"+`index`+".jpg")
    convert(photo, dst+"/imgs/rectifiedAmbient/right/image-"+`index`+".s.jpg", size = "600x400")
    index += 1




#write website
mainfile = open(dst+"/index.php", "w")
mainfile.write("""<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Strict//EN"
 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">

<head><title> Stereo Vision Data Sets </title>


<script type="text/javascript">
var nrect = %d
var exposure = %d
var fn;
var n;
var nc=0;
var a = false;
var tab = true;




function keyInput(e) {
if (!e) var e = window.event;
var code;
if (e.keyCode)
    code = e.keyCode;
else if (e.which)
    code = e.which;
if (code == 39 || code == 77) { // right arrow or 'm'
a=true;
fn = (document.getElementById('im1').src.match(/([0-9]+)\.s\.jpg$/));
n = parseInt(fn[1]);
if(n>=nrect){
    n = ((n+1)%%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n+1)%%nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}
} else if (code == 37 || code == 78) { // left arrow or 'n'
a=true;
fn = (document.getElementById('im1').src.match(/([0-9]+)\.s\.jpg$/));
n = parseInt(fn[1]);
if(n>0){
if(n>=nrect){
    n = ((n-1)%%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n-1)%%nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}
}
} else if (code == 90) { // 'z'
a=true;
fn = (document.getElementById('im1').src.match(/([0-9]+)\.s\.jpg$/));
n = parseInt(fn[1]);
if(n>=nrect){
    n = ((n+exposure)%%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n+exposure)%%nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}
} else if (code == 88) { // 'x'
a=true;
fn = (document.getElementById('im1').src.match(/([0-9]+)\.s\.jpg$/));
n = parseInt(fn[1]);
if(n>exposure){
if(n>=nrect){
    n = ((n-exposure)%%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n-exposure)%%nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}
}
} else if (code == 66) { // 'b'
a=true;
fn = (document.getElementById('im1').src.match(/([0-9]+)\.s\.jpg$/));
n = parseInt(fn[1]);
n = (n+nrect)%%(nrect*2);
Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
showImg(Img1,Img2);
nc=n;
} else if (code == 65) { // 'a'
a=true;
Img1 = "imgs/rectifiedAmbient/left/image-"+(nc)+".s.jpg";
Img2 = "imgs/rectifiedAmbient/right/image-"+(nc)+".s.jpg";
showImg(Img1,Img2);
} else if (code == 82) { // 'r'
a = false;
showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg');

}else if (code == 89) { // 'y'
a = false;
showImg('imgs/disparity/lefty.s.jpg','imgs/disparity/righty.s.jpg');

}else if (code == 71) { // 'g'
a = false;
showImg('imgs/disparity/leftxg.s.jpg','imgs/disparity/rightxg.s.jpg');

}else if (code == 84) { // 't'
Img1 = document.getElementById('im1').src;
Img2 = document.getElementById('im2').src;
showImg(Img2,Img1);
tab=!tab;
}else if (code == 27) { // 'Esc'
a = false;
Img1 = document.getElementById('im1').src;
Img2 = document.getElementById('im2').src;
showImg(Img1,Img2);
}else {}
}


function showImg(x, y, note, showLegend) {
 if(typeof(note)==='undefined') note = '';
if(typeof(showLegend)==='undefined') showLegend = false;

var expo=1;
var li=1;
var images = new Array(x,y);
document.getElementById('im1').src = images[0];
document.getElementById('im2').src = images[1];
var p1 = images[0].split("/");
var p2 = images[1].split("/");

if (isNaN(n)==true || a==false){
    document.getElementById('label1').innerHTML = p1[p1.length-1] + "               " + note;
    document.getElementById('label2').innerHTML = p2[p2.length-1];
}else{
    if(n<nrect){
        expo=n%%exposure+1;
        li=Math.floor(n/exposure)+1;
    }else{
        expo=(n-nrect)%%exposure+1;
        li=Math.floor((n-nrect)/exposure)+1;
    }
    document.getElementById('label1').innerHTML = p1[p1.length-1] +"; "+"exposure:  "+expo+", "+"light:  "+li + "               " + note;
    document.getElementById('label2').innerHTML = p2[p2.length-1] +"; "+"exposure:  "+expo+", "+"light:  "+li;
    
    
}
if (showLegend){
        document.getElementById('legend').style.visibility="visible";
    }
    else {
         document.getElementById('legend').style.visibility="hidden"; 
    }
}


function convert(path){
path = path.replace("\.s\.jpg","\.jpg");
return path;
}

</script>
</head>


<body onload="showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg')" onkeydown = "keyInput(event)">

<div class="text">
Move the mouse on to picture to show the picture above.
Click the exact picture will show the original pictures.<br/>
You need to move your mouse to any ambient image first before you use the keyboard.
Hit the N/M or left/right arrow key to advance through all exposures.<br/>
Hit the Z/X key to change to advance through all lighting conditions.
Hit the B key to see the same image with ball. 
Hit the T key to toggle left and right.

</div>

<div>
<table border="1"><tr><td>
<table border="0"><tr>
<td><img width="600" height = "400" id = 'im1' src = '' alt = 'image' onclick = "JavaScript:window.open(convert(document.getElementById('im1').src))"/><br/>
<span id = 'label1'></span></td>
<td><img width="600" height = "400" id = 'im2' src = '' alt = 'image2' onclick = "JavaScript:window.open(convert(document.getElementById('im2').src))"/><br/>
<span id = 'label2'></span></td>
</tr></table>
</td></tr></table>
</div>
<table cellspacing="3">
<tr><td style="vertical-align:top" >
<div id=legend>
Y disparity legend: <br></br>
<TABLE HEIGHT = "30" ALIGN = "right" cellspacing="0"><TBODY><TR>
<td></td>
"""%(nrect,exposure))

#Open the RGB codes
f = open(src + '/computed/resultColor.txt', 'r')
text = str(f.read())

#looks for codes with regular expressions
p = re.compile('valy: (-*\d) rgb: (\d+) (\d+) (\d+)')
codes = p.findall(text) #[(valy, r, g, b,), (valy, r, g, b)...]

#http://stackoverflow.com/questions/214359/converting-hex-color-to-rgb-and-vice-versa
def rgb_to_hex(rgb):
    return '#%02x%02x%02x' % rgb

#the number of codes is determined by the txt output in FloVis
for code in codes:
    valy = int(code[0])
    hexCol = rgb_to_hex((int(code[1]), int(code[2]), int(code[3])))
    mainfile.write("""
<TD bgcolor=%s width=25>%d</TD>"""%(hexCol, valy))

mainfile.write("""
</TR>
  </TBODY>
</TABLE><br></br></div>""")


#Open grey disparity range
g = open(src + '/computed/resultGrey.txt', 'r')
textGrey = str(g.read())

#look for range with reg ex
q = re.compile('max: (-*\d+).*min: (-*\d+).*')
greyRange = q.findall(textGrey)
g1 = str(greyRange[0][0])
g2 = str(greyRange[0][1])

mainfile.write("""
<h3 ALIGN="center">Disparity</h3>
<div align="center">
<a href="imgs/disparity/leftx.jpg"><img width = "50" src="imgs/disparity/leftx.s.jpg" onMouseOver="showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg')" alt=""></img></a><a href="imgs/disparity/rightx.jpg"><img width = "50" src="imgs/disparity/rightx.s.jpg" onMouseOver="showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg')" alt=""></img></a><br></br>

<a href="imgs/disparity/leftxg.jpg"><img width = "50" src="imgs/disparity/leftxg.s.jpg" onMouseOver="showImg('imgs/disparity/leftxg.s.jpg','imgs/disparity/rightxg.s.jpg', 'Disparity Range  Max: %s Min: %s ')" alt=""></img></a><a href="imgs/disparity/rightxg.jpg"><img width = "50" src="imgs/disparity/rightxg.s.jpg" onMouseOver="showImg('imgs/disparity/leftxg.s.jpg','imgs/disparity/rightxg.s.jpg', 'Disparity Range  Max: %s Min: %s')" alt=""></img></a><br></br>
<a href="imgs/disparity/lefty.jpg"><img width = "50" src="imgs/disparity/lefty.s.jpg" onMouseOver="showImg('imgs/disparity/lefty.s.jpg','imgs/disparity/righty.s.jpg', '', true)" alt=""></img></a><a href="imgs/disparity/righty.jpg"><img width = "50" src="imgs/disparity/righty.s.jpg" onMouseOver="showImg('imgs/disparity/lefty.s.jpg','imgs/disparity/righty.s.jpg', '', true)" alt=""></img></a>
</div>
</td><td><h3 ALIGN="center"> Ambient </h3>
"""%(g1,g2,g1,g2))

light = nrect/exposure

mainfile.write("""
<table> 
<tr> 
<td><br/><br/><br/></td> """)

for y in range (0,exposure):
    y += 1
    mainfile.write("""
<td align = "center"> Exposure %d </td>"""%y)
mainfile.write("""
</tr> 
    
<tr> """)
     
for z in range (0,light):                
    mainfile.write("""
<td align = "center">Lighting %d<br/><br/></td>"""%(z+1))
    for i in range (exposure*z,exposure*(z+1)):
        mainfile.write("""
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-%d.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-%d.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-%d.s.jpg','imgs/rectifiedAmbient/right/image-%d.s.jpg')" alt=""></img></a></td>"""%(i,i,i,i))
    mainfile.write("""</tr>
""")
mainfile.write("""</table>  <h3 ALIGN="center"> Ambient with Ball</h3> <table>""")

mainfile.write("""<td> 
<table> 
<tr> 
<td><br/><br/><br/></td> """)
for y in range (0,exposure):
    y += 1
    mainfile.write("""
<td align = "center"> Exposure %d </td>"""%y)
mainfile.write("""
</tr> 
    
<tr> """)
       
for z in range (0,light):                
    mainfile.write("""
<td align = "center">Lighting %d<br/><br/></td>"""%(z+1))
    for i in range (exposure*z,exposure*(z+1)):
        mainfile.write("""
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-%d.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-%d.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-%d.s.jpg','imgs/rectifiedAmbient/right/image-%d.s.jpg')" alt=""></img></a></td>"""%((i+nrect),(i+nrect),(i+nrect),(i+nrect)))
    mainfile.write("""</tr>
""")
mainfile.write("""</table> 
</td>
""")

mainfile.write("""
</table> 
</td>

 """)
    

mainfile.write("""
</table>
</td></table>
</div>
""")
print "Done with website built"
