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
 * TfCreator
 *--------------
 * Manages creation, loading and saving of transferfunctions.
 *
 */

#ifndef TFCREATOR_H_
#define TFCREATOR_H_

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
#include "Transferfunction.h"
#include "CreatorHelper.h"

const std::string TFCREATOR_PATH = std::string(DATA_PATH) + "/Transferfunctions/";

class TfCreator
{
public:
    TfCreator();
    ~TfCreator();

    /** Creates new transferfunction */
    Transferfunction* create(GLint handle, std::string name);

    /** Writes transferfunction to XML-File */
    GLboolean writeToFile(Transferfunction* pTransferfunction, GLboolean overwriteExisting);

    /** Reads transferfunction from XML-File, assigns handle to it */
    Transferfunction* readFromFile(std::string name, GLint handle);

protected:
    /** Write methods */
    void appendTfPoint(TfPoint* pTfPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
    void appendNormalTfPoints(std::vector<TfPoint>* pTfPoints, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
    void appendPoint(glm::vec2 point, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
    void appendLeftControlPoint(glm::vec2 leftControlPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
    void appendRightControlPoint(glm::vec2 rightControlPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);

    /** Read methods */
    void extractTfPoint(rapidxml::xml_node<>* pNode, TfPointLocation location, GLint tfPointHandleCounter, Transferfunction* pTransferfunction);
    glm::vec2 extractPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction);
    glm::vec2 extractLeftControlPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction);
    glm::vec2 extractRightControlPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction);
};

#endif
