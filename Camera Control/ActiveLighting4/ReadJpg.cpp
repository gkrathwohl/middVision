///////////////////////////////////////////////////////////////////////////
//
// NAME
//  ReadJpg.cpp -- reads jpeg image
//
// DESCRIPTION
//  Uses Small JPEG Decoder Library by Rich Geldreich 
//  http://www.users.voicenet.com/~richgel/
//
//  This file is derived from jpgd093b/jpg2tga/Jpg2tga.cpp
//
//  Note: this may not be a high-quality jpeg decoder.  The
//  readme file says it's built for simplicity and speed...
//  We may want to change this to JPEG group software
//  (http://www.ijg.org) or Intel's JPEG library.
//
//
// SEE ALSO
//  ImageIO.cpp
//
// Copyright © Richard Szeliski and Daniel Scharstein, 2002.
// See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#include "Image.h"
#include "Error.h"
#include "ImageIO.h"
#include <vector>

#include "jpgd093b/inc/jpegdecoder.h"
const bool use_mmx = true;


void ReadFileJPG(CByteImage& img, const char* filename)
{
	
	Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();	
	if (Pinput_stream->open(filename)) {
		delete Pinput_stream;
        throw CError("ReadFileJPG: Unable to open file \"%s\" for reading!", filename);
	}
	
	Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, use_mmx);
	if (Pd->get_error_code() != 0) {
		delete Pd;
		delete Pinput_stream;
        throw CError("ReadFileJPG: Decoder failed! Error status: %i\n", Pd->get_error_code());
	}

    // Determine the image shape
	int width = Pd->get_width();
	int height = Pd->get_height();
	int nBands = Pd->get_num_components();
	if (nBands == 3)
		nBands = 4;		// read color image into 4-band image (RGBA)
    CShape sh(width, height, nBands);
    
    // Allocate the image if necessary
	img.ReAllocate(sh, false);
	
	if (Pd->begin()) {
		delete Pd;
		delete Pinput_stream;
		throw CError("ReadFileJPG: Decoder failed! Error status: %i", Pd->get_error_code());
	}
	
	int lines_decoded = 0;
	
	for (int y = 0; ; y++) {

		void *Pscan_line_ofs;
		uint scan_line_len;
		
		if (Pd->decode(&Pscan_line_ofs, &scan_line_len))
			break;
		
		lines_decoded++;
		
		uchar *Psb = (uchar *)Pscan_line_ofs;
		int src_bpp = Pd->get_bytes_per_pixel();
		uchar *Pdb = (uchar *)img.PixelAddress(0, y, 0);
		
		if (nBands == 1) {	// gray image

			for (int x = 0; x < width; x++, Psb += src_bpp, Pdb++) {
				Pdb[0] = Psb[0];
			}

		} else {			// color image

			for (int x = 0; x < width; x++, Psb += src_bpp, Pdb += 4) {
				Pdb[0] = Psb[2];
				Pdb[1] = Psb[1];
				Pdb[2] = Psb[0];
				Pdb[3] = 255; // full alpha
			}
		}
	}		
	//printf("height: %d\n", height);
	//printf("Lines decoded: %i\n", lines_decoded);
	//printf("Input file size:  %i\n", Pinput_stream->get_size());
	//printf("Input bytes actually read: %i\n", Pd->get_total_bytes_read());
	
	if (lines_decoded != height)
		throw CError("ReadFileJPG(%s): file is too short", filename);

	delete Pd;
	delete Pinput_stream;
}
