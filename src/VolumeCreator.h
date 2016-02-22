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
 * VolumeCreator
 *--------------
 * Manages creation, loading and saving of volumes.
 *
 */

#ifndef VOLUMECREATOR_H_
#define VOLUMECREATOR_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_print.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "Logger.h"
#include "Volume.h"
#include "CreatorHelper.h"

const std::string VOLUMECREATOR_PATH = std::string(DATA_PATH) + "/Volumes/";
const std::string VOLUMECREATOR_SUBDIR_PVM = "PVM/";
const std::string VOLUMECREATOR_SUBDIR_DAT = "DAT/";

class VolumeCreator
{
public:
    VolumeCreator();
    ~VolumeCreator();

    /** Imports PVM */
    Volume* importPVM(std::string name, GLint handle);

    /** Imports DAT*/
    Volume* importDAT(std::string name, GLint handle);

    /** Creates default volume */
    Volume* createDefaultVolume(std::string name, GLint handle);

    /** Writes volume to XML-File and Raw-File */
    GLboolean writeToFile(Volume* pVolume, GLboolean overwriteExisting);

    /** Reads volume from XML-File and Raw-File, assigns handle to it */
    Volume* readFromFile(std::string name, GLint handle);

protected:
    GLint createTexture(GLubyte* volumeData, glm::vec3 volumeResolution, VolumeValueResolution valueResolution, GLboolean useLinearFiltering);
    GLint extractIntFromCharArray(std::ifstream* pIn);
    GLfloat extractFloatFromCharArray(std::ifstream* pIn);
};

#endif
