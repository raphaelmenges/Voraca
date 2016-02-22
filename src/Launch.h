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
 * Launch
 *--------------
 * Singleton for launch settings.
 *
 */

#ifndef LAUNCH_H_
#define LAUNCH_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_print.hpp"

#include <string>
#include <fstream>
#include <ostream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Logger.h"
#include "CreatorHelper.h"
#include "Utilities.h"

class Launch
{
public:
    /** Get stuff. Returns 0 when successfull and 1 if there was an error **/
    static GLint getWindowWidth(GLuint& value);
    static GLint getWindowHeight(GLuint& value);
    static GLint isFullscreen(GLboolean& value);
    static GLint getVolume(std::string& value);
    static GLint getTransferfunction(std::string& value);
    static GLint getRaycaster(std::string& value);

private:

    Launch();
    ~Launch();

    /** Creates instance if necessary */
    static void createInstance();

    /** Create new launch file */
    static void createNewLaunchFile();

    /** Private copy constuctor */
    Launch(Launch const&) {};

    /** Private assignment operator */
    Launch& operator=(Launch const&) {return *this;};

    /** The pointer to the instance */
    static Launch* pInstance;

    /** Members **/
    GLuint windowWidth;
    GLuint windowHeight;
    GLboolean fullscreen;
    std::string volume;
    std::string transferfunction;
    std::string raycaster;

    GLboolean successfullRead;
};


#endif
