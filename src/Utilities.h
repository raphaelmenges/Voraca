/**************************************************************************
 * Voraca 0.97 (VOlume RAy-CAster)
 **************************************************************************
 * Copyright (c) 2016, Raphael Philipp Menges
 *
 * Permission to use, copy, modify, and/or distribute this software for any 
 * purpose with or without fee is hereby granted, provided that the above 
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************/

/*
 * Utilities
 *--------------
 * Functions needed all over the project.
 *
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <sstream>
#include <string>

namespace UT
{
	inline std::string to_string(int x)
	{
		std::stringstream ss;
		ss << x;
		return ss.str();
    }

	inline std::string to_string(unsigned int x)
    {
		std::stringstream ss;
        ss << x;
		return ss.str();
    }

    inline std::string to_string(float x)
    {
        std::stringstream ss;
        ss << x;
		return ss.str();
    }

	inline std::string to_string(double x)
	{
		std::stringstream ss;
        ss << x;
        return ss.str();
    }

	inline FILE* openFile(std::string path)
	{
#ifdef _WIN32
		FILE* pRawDataFile;
		fopen_s(&pRawDataFile, path.c_str(), "wb");
		return pRawDataFile;
#else
		return fopen(path.c_str(), "wb");
#endif
	}
}
#endif
