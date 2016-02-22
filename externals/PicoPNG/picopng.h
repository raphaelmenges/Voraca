/*
 * picopng
 *--------------
 * Header to use picoPNG. Current website of this project: http://lodev.org/lodepng/
 *
 */

#ifndef PICOPNG_H_
#define PICOPNG_H_

#include <vector>

/** Decodes a PNG file */
int decodePNG(std::vector<unsigned char>& out_image, unsigned long& image_width, unsigned long& image_height, const unsigned char* in_png, int in_size, bool convert_to_rgba32 = true);

#endif
