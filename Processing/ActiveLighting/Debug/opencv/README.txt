Copied this directory from /home/wwestlin/opencv
DS 7/18/2013
Should be reasonably standalone, though when you compile and then use "ldd" to 
check what libraries are linked, there still are a few in /home/wwestlin/opencv/

My guess is that these are referenced from others that were copied here.

In general, there should be no need for a local opencv copy --
this version is 2.3.0
on the lab machines we have 2.3.1, so that should work too, but didn't
have enough time to get it to work...


gkrathwohl@hogback Debug]$ ldd ActiveLighting4 | grep open
        libopencv_core.so.2.3 => opencv/lib/libopencv_core.so.2.3 (0x00007fc0d3b41000)
        libopencv_highgui.so.2.3 => opencv/lib/libopencv_highgui.so.2.3 (0x00007fc0d38b3000)
        libopencv_calib3d.so.2.3 => opencv/lib/libopencv_calib3d.so.2.3 (0x00007fc0d3600000)
        libopencv_legacy.so.2.3 => opencv/lib/libopencv_legacy.so.2.3 (0x00007fc0d334a000)
        libopencv_imgproc.so.2.3 => opencv/lib/libopencv_imgproc.so.2.3 (0x00007fc0d2eee000)
        libopencv_features2d.so.2.3 => opencv/lib/libopencv_features2d.so.2.3 (0x00007fc0d2bf5000)
        libopencv_flann.so.2.3 => /home/wwestlin/opencv/lib/libopencv_flann.so.2.3 (0x00007fc0d29e5000)
        libopencv_video.so.2.3 => /home/wwestlin/opencv/lib/libopencv_video.so.2.3 (0x00007fc0d27a9000)

