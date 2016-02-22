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
 * RcCreator
 *--------------
 * Manages creation, loading and saving of raycasters.
 *
 */

#ifndef RCCREATOR_H_
#define RCCREATOR_H_

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
#include "Raycaster.h"
#include "CreatorHelper.h"

const std::string RCCREATOR_PATH = std::string(DATA_PATH) + "/Raycaster/";

class RcCreator
{
public:
    RcCreator();
    ~RcCreator();

    /** Creates new raycaster */
    Raycaster* create(GLint handle, std::string name);

    /** Writes raycaster to XML-File */
    GLboolean writeToFile(Raycaster* pRaycaster, GLboolean overwriteExisting);

    /** Read raycaster from XML-File, assigns handle to it */
    Raycaster* readFromFile(std::string name, GLint handle);
};

#endif
