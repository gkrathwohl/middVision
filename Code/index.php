<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Strict//EN"
 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">

<head><title> Stereo Vision Data Sets </title>

<script type="text/javascript">
var nrect = 18
var exposure = 6
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
    n = ((n+1)%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n+1)%nrect;
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
    n = ((n-1)%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n-1)%nrect;
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
    n = ((n+exposure)%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n+exposure)%nrect;
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
    n = ((n-exposure)%nrect)+nrect;
    nc=n;
    Img1 = "imgs/rectifiedAmbient/left/image-"+(n)+".s.jpg";
    Img2 = "imgs/rectifiedAmbient/right/image-"+(n)+".s.jpg";
    showImg(Img1,Img2);
    
}else{
    n = (n-exposure)%nrect;
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
n = (n+nrect)%(nrect*2);
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


function showImg(x, y) {

var expo=1;
var li=1;
var images = new Array(x,y);
document.getElementById('im1').src = images[0];
document.getElementById('im2').src = images[1];
var p1 = images[0].split("/");
var p2 = images[1].split("/");

if (isNaN(n)==true || a==false){
    document.getElementById('label1').innerHTML = p1[p1.length-1];
    document.getElementById('label2').innerHTML = p2[p2.length-1];
}else{
    if(n<nrect){
        expo=n%exposure+1;
        li=Math.floor(n/exposure)+1;
    }else{
        expo=(n-nrect)%exposure+1;
        li=Math.floor((n-nrect)/exposure)+1;
    }
    document.getElementById('label1').innerHTML = p1[p1.length-1] +"; "+"exposure:  "+expo+", "+"light:  "+li;
    document.getElementById('label2').innerHTML = p2[p2.length-1] +"; "+"exposure:  "+expo+", "+"light:  "+li;
    
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
</div><h2>Disparity<br/></h2>
<p>
Left   color   
<a href="imgs/disparity/leftx.jpg"><img width = "50" src="imgs/disparity/leftx.s.jpg" onMouseOver="showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg')" alt=""></img></a>
<a href="imgs/disparity/lefty.jpg"><img width = "50" src="imgs/disparity/lefty.s.jpg" onMouseOver="showImg('imgs/disparity/lefty.s.jpg','imgs/disparity/righty.s.jpg')" alt=""></img></a>
 grey 
<a href="imgs/disparity/leftxg.jpg"><img width = "50" src="imgs/disparity/leftxg.s.jpg" onMouseOver="showImg('imgs/disparity/leftxg.s.jpg','imgs/disparity/rightxg.s.jpg')" alt=""></img></a>

<br/>
Right  color  
<a href="imgs/disparity/rightx.jpg"><img width = "50" src="imgs/disparity/rightx.s.jpg" onMouseOver="showImg('imgs/disparity/leftx.s.jpg','imgs/disparity/rightx.s.jpg')" alt=""></img></a>
<a href="imgs/disparity/righty.jpg"><img width = "50" src="imgs/disparity/righty.s.jpg" onMouseOver="showImg('imgs/disparity/lefty.s.jpg','imgs/disparity/righty.s.jpg')" alt=""></img></a>
 grey 
<a href="imgs/disparity/rightxg.jpg"><img width = "50" src="imgs/disparity/rightxg.s.jpg" onMouseOver="showImg('imgs/disparity/leftxg.s.jpg','imgs/disparity/rightxg.s.jpg')" alt=""></img></a>
</br>
<TABLE  HEIGHT = "30" ALIGN = "left" cellspacing="0">
  y disparity<TBODY>
    <TR><td></td><TD bgcolor=#E272FF width=25>-4
    </TD>
<TD bgcolor=#72AAFF width=25>-3</TD>
<TD bgcolor=#72FFAA width=25>-2</TD>
<TD bgcolor=#E2FF72 width=25>-1</TD>
<TD bgcolor=#FF7272 width=25>0</TD>
<TD bgcolor=#E272FF width=25>+1</TD>
<TD bgcolor=#72AAFF width=25>+2</TD>
<TD bgcolor=#72FFAA width=25>+3</TD>
<TD bgcolor=#E2FF72 width=25>+4</TD></TR>
  </TBODY>
</TABLE>
</br>
</br>
</p>

<h2>Rectified Ambient</h2>
<table> 
 <tr> 
 <td align = "center"> <h3> Left </h3> </td> 
 <td align = "center"> <h3> Right </h3> </td> 
 </tr> 
<td> 
<table> 
<tr> 
<td><br/><br/><br/></td> 
<td align = "center"> Exposure 1 </td>
<td align = "center"> Exposure 2 </td>
<td align = "center"> Exposure 3 </td>
<td align = "center"> Exposure 4 </td>
<td align = "center"> Exposure 5 </td>
<td align = "center"> Exposure 6 </td>
</tr> 
    
<tr> 
<td align = "center">Lighting 1<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-0.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-0.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-0.s.jpg','imgs/rectifiedAmbient/right/image-0.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-1.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-1.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-1.s.jpg','imgs/rectifiedAmbient/right/image-1.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-2.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-2.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-2.s.jpg','imgs/rectifiedAmbient/right/image-2.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-3.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-3.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-3.s.jpg','imgs/rectifiedAmbient/right/image-3.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-4.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-4.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-4.s.jpg','imgs/rectifiedAmbient/right/image-4.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-5.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-5.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-5.s.jpg','imgs/rectifiedAmbient/right/image-5.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 2<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-6.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-6.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-6.s.jpg','imgs/rectifiedAmbient/right/image-6.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-7.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-7.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-7.s.jpg','imgs/rectifiedAmbient/right/image-7.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-8.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-8.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-8.s.jpg','imgs/rectifiedAmbient/right/image-8.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-9.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-9.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-9.s.jpg','imgs/rectifiedAmbient/right/image-9.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-10.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-10.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-10.s.jpg','imgs/rectifiedAmbient/right/image-10.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-11.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-11.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-11.s.jpg','imgs/rectifiedAmbient/right/image-11.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 3<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-12.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-12.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-12.s.jpg','imgs/rectifiedAmbient/right/image-12.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-13.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-13.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-13.s.jpg','imgs/rectifiedAmbient/right/image-13.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-14.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-14.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-14.s.jpg','imgs/rectifiedAmbient/right/image-14.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-15.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-15.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-15.s.jpg','imgs/rectifiedAmbient/right/image-15.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-16.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-16.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-16.s.jpg','imgs/rectifiedAmbient/right/image-16.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-17.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-17.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-17.s.jpg','imgs/rectifiedAmbient/right/image-17.s.jpg')" alt=""></img></a></td></tr>
</table> 
</td>
<td> 
<table> 
<tr> 
<td><br/><br/><br/></td> 
<td align = "center"> Exposure 1 </td>
<td align = "center"> Exposure 2 </td>
<td align = "center"> Exposure 3 </td>
<td align = "center"> Exposure 4 </td>
<td align = "center"> Exposure 5 </td>
<td align = "center"> Exposure 6 </td>
</tr> 
    
<tr> 
<td align = "center">Lighting 1<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-0.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-0.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-0.s.jpg','imgs/rectifiedAmbient/right/image-0.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-1.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-1.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-1.s.jpg','imgs/rectifiedAmbient/right/image-1.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-2.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-2.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-2.s.jpg','imgs/rectifiedAmbient/right/image-2.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-3.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-3.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-3.s.jpg','imgs/rectifiedAmbient/right/image-3.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-4.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-4.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-4.s.jpg','imgs/rectifiedAmbient/right/image-4.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-5.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-5.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-5.s.jpg','imgs/rectifiedAmbient/right/image-5.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 2<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-6.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-6.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-6.s.jpg','imgs/rectifiedAmbient/right/image-6.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-7.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-7.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-7.s.jpg','imgs/rectifiedAmbient/right/image-7.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-8.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-8.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-8.s.jpg','imgs/rectifiedAmbient/right/image-8.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-9.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-9.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-9.s.jpg','imgs/rectifiedAmbient/right/image-9.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-10.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-10.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-10.s.jpg','imgs/rectifiedAmbient/right/image-10.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-11.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-11.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-11.s.jpg','imgs/rectifiedAmbient/right/image-11.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 3<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-12.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-12.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-12.s.jpg','imgs/rectifiedAmbient/right/image-12.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-13.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-13.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-13.s.jpg','imgs/rectifiedAmbient/right/image-13.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-14.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-14.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-14.s.jpg','imgs/rectifiedAmbient/right/image-14.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-15.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-15.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-15.s.jpg','imgs/rectifiedAmbient/right/image-15.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-16.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-16.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-16.s.jpg','imgs/rectifiedAmbient/right/image-16.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-17.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-17.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-17.s.jpg','imgs/rectifiedAmbient/right/image-17.s.jpg')" alt=""></img></a></td></tr>
</table> 
</td>    

</tr> 

<tr> 
 <td align = "center"> <h3> Left with Ball </h3> </td> 
 <td align = "center"> <h3> Right with Ball</h3> </td> 
</tr><td> 
<table> 
<tr> 
<td><br/><br/><br/></td> 
<td align = "center"> Exposure 1 </td>
<td align = "center"> Exposure 2 </td>
<td align = "center"> Exposure 3 </td>
<td align = "center"> Exposure 4 </td>
<td align = "center"> Exposure 5 </td>
<td align = "center"> Exposure 6 </td>
</tr> 
    
<tr> 
<td align = "center">Lighting 1<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-18.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-18.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-18.s.jpg','imgs/rectifiedAmbient/right/image-18.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-19.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-19.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-19.s.jpg','imgs/rectifiedAmbient/right/image-19.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-20.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-20.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-20.s.jpg','imgs/rectifiedAmbient/right/image-20.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-21.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-21.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-21.s.jpg','imgs/rectifiedAmbient/right/image-21.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-22.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-22.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-22.s.jpg','imgs/rectifiedAmbient/right/image-22.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-23.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-23.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-23.s.jpg','imgs/rectifiedAmbient/right/image-23.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 2<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-24.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-24.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-24.s.jpg','imgs/rectifiedAmbient/right/image-24.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-25.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-25.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-25.s.jpg','imgs/rectifiedAmbient/right/image-25.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-26.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-26.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-26.s.jpg','imgs/rectifiedAmbient/right/image-26.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-27.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-27.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-27.s.jpg','imgs/rectifiedAmbient/right/image-27.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-28.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-28.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-28.s.jpg','imgs/rectifiedAmbient/right/image-28.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-29.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-29.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-29.s.jpg','imgs/rectifiedAmbient/right/image-29.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 3<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-30.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-30.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-30.s.jpg','imgs/rectifiedAmbient/right/image-30.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-31.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-31.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-31.s.jpg','imgs/rectifiedAmbient/right/image-31.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-32.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-32.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-32.s.jpg','imgs/rectifiedAmbient/right/image-32.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-33.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-33.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-33.s.jpg','imgs/rectifiedAmbient/right/image-33.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-34.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-34.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-34.s.jpg','imgs/rectifiedAmbient/right/image-34.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/left/image-35.jpg"><img width = "50" src="imgs/rectifiedAmbient/left/image-35.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-35.s.jpg','imgs/rectifiedAmbient/right/image-35.s.jpg')" alt=""></img></a></td></tr>
</table> 
</td>
<td> 
<table> 
<tr> 
<td><br/><br/><br/></td> 
<td align = "center"> Exposure 1 </td>
<td align = "center"> Exposure 2 </td>
<td align = "center"> Exposure 3 </td>
<td align = "center"> Exposure 4 </td>
<td align = "center"> Exposure 5 </td>
<td align = "center"> Exposure 6 </td>
</tr> 
    
<tr> 
<td align = "center">Lighting 1<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-18.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-18.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-18.s.jpg','imgs/rectifiedAmbient/right/image-18.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-19.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-19.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-19.s.jpg','imgs/rectifiedAmbient/right/image-19.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-20.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-20.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-20.s.jpg','imgs/rectifiedAmbient/right/image-20.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-21.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-21.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-21.s.jpg','imgs/rectifiedAmbient/right/image-21.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-22.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-22.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-22.s.jpg','imgs/rectifiedAmbient/right/image-22.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-23.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-23.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-23.s.jpg','imgs/rectifiedAmbient/right/image-23.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 2<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-24.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-24.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-24.s.jpg','imgs/rectifiedAmbient/right/image-24.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-25.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-25.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-25.s.jpg','imgs/rectifiedAmbient/right/image-25.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-26.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-26.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-26.s.jpg','imgs/rectifiedAmbient/right/image-26.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-27.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-27.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-27.s.jpg','imgs/rectifiedAmbient/right/image-27.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-28.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-28.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-28.s.jpg','imgs/rectifiedAmbient/right/image-28.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-29.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-29.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-29.s.jpg','imgs/rectifiedAmbient/right/image-29.s.jpg')" alt=""></img></a></td></tr>

<td align = "center">Lighting 3<br/><br/></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-30.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-30.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-30.s.jpg','imgs/rectifiedAmbient/right/image-30.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-31.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-31.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-31.s.jpg','imgs/rectifiedAmbient/right/image-31.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-32.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-32.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-32.s.jpg','imgs/rectifiedAmbient/right/image-32.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-33.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-33.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-33.s.jpg','imgs/rectifiedAmbient/right/image-33.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-34.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-34.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-34.s.jpg','imgs/rectifiedAmbient/right/image-34.s.jpg')" alt=""></img></a></td>
<td align = "center"><a href="imgs/rectifiedAmbient/right/image-35.jpg"><img width = "50" src="imgs/rectifiedAmbient/right/image-35.s.jpg" onMouseOver="showImg('imgs/rectifiedAmbient/left/image-35.s.jpg','imgs/rectifiedAmbient/right/image-35.s.jpg')" alt=""></img></a></td></tr>
</table> 
</td>    
</tr> 
</table>
</div>
