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
 * CreatorHelper
 *--------------
 * Helper functions needed by all creator classes.
 *
 */

#ifndef TFCREATORHELPER_H_
#define TFCREATORHELPER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "RapidXML/rapidxml.hpp"

#include <string>

#include "Logger.h"

/** Write methods */
static void appendVec3(glm::vec3 value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
static void appendFloat(GLfloat value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);
static void appendBool(GLboolean value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent);

/** Write methods' helpers */
static GLchar* convertFloatToChar(rapidxml::xml_document<>* pDoc, GLfloat value);
static GLchar* convertIntToChar(rapidxml::xml_document<>* pDoc, GLint value);
static GLchar* convertBoolToChar(rapidxml::xml_document<>* pDoc, GLboolean value);

/** Read methods */
static glm::vec3 extractVec3(std::string expectedName, std::string nameOfFile, rapidxml::xml_node<>* pNode);
static GLfloat extractFloat(std::string expectedName, std::string nameOfFile, rapidxml::xml_node<>* pNode);
static GLboolean extractBool(std::string expectedName, std::string nameOfFile, rapidxml::xml_node<>* pNode);

/** Read methods' helpers */
static GLfloat convertCharToFloat(GLchar* value);
static GLint convertCharToInt(GLchar* value);
static GLboolean convertCharToBool(GLchar* value);
static std::string convertCharToString(GLchar* value);

/** Some checking methods */
static GLuint checkValue(std::string value, std::string expectedValue, std::string nameOfFile);

/** Here comes the definiton */
static void appendVec3(glm::vec3 value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
    // Create vec3 node
    rapidxml::xml_node<>* pVec3Node = pDoc->allocate_node(rapidxml::node_element, name);

    // Append channels
    pVec3Node->append_attribute(pDoc->allocate_attribute("r", convertFloatToChar(pDoc, value.x)));
    pVec3Node->append_attribute(pDoc->allocate_attribute("g", convertFloatToChar(pDoc, value.y)));
    pVec3Node->append_attribute(pDoc->allocate_attribute("b", convertFloatToChar(pDoc, value.z)));

    // Append vec3 node
    pParent->append_node(pVec3Node);
}

static void appendFloat(GLfloat value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
    // Create float node
    rapidxml::xml_node<>* pFloatNode = pDoc->allocate_node(rapidxml::node_element, name);

    // Append value
    pFloatNode->append_attribute(pDoc->allocate_attribute("value", convertFloatToChar(pDoc, value)));

    // Append float node
    pParent->append_node(pFloatNode);
}

static void appendBool(GLboolean value, GLchar* name, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
    // Create float node
    rapidxml::xml_node<>* pBoolNode = pDoc->allocate_node(rapidxml::node_element, name);

    // Append value
    pBoolNode->append_attribute(pDoc->allocate_attribute("value", convertBoolToChar(pDoc, value)));

    // Append float node
    pParent->append_node(pBoolNode);
}

static glm::vec3 extractVec3(std::string expectedName, std::string nameOfFile, rapidxml::xml_node<>* pNode)
{
    // Check node
    checkValue(std::string(pNode->name()), expectedName, nameOfFile);

    // Result
    glm::vec3 value;

    // Extract red
    rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
    checkValue(std::string(pNodeCurrentAttribute->name()), "r", nameOfFile);
    value.r = convertCharToFloat(pNodeCurrentAttribute->value());

    // Extract green
    pNodeCurrentAttribute = pNodeCurrentAttribute->next_attribute();
    checkValue(std::string(pNodeCurrentAttribute->name()), "g", nameOfFile);
    value.g = convertCharToFloat(pNodeCurrentAttribute->value());

    // Extract blue
    pNodeCurrentAttribute = pNodeCurrentAttribute->next_attribute();
    checkValue(std::string(pNodeCurrentAttribute->name()), "b", nameOfFile);
    value.b = convertCharToFloat(pNodeCurrentAttribute->value());

    return value;
}

static GLfloat extractFloat(std::string expectedName,std::string nameOfFile, rapidxml::xml_node<>* pNode)
{
    // Check node
    checkValue(std::string(pNode->name()), expectedName, nameOfFile);

    // Result
    GLfloat value;

    // Extract value
    rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
    checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
    value = convertCharToFloat(pNodeCurrentAttribute->value());

    return value;
}

static GLboolean extractBool(std::string expectedName, std::string nameOfFile, rapidxml::xml_node<>* pNode)
{
    // Check node
    checkValue(std::string(pNode->name()), expectedName, nameOfFile);

    // Result
    GLboolean value;

    // Extract value
    rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
    checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
    value = convertCharToBool(pNodeCurrentAttribute->value());

    return value;
}

static GLchar* convertFloatToChar(rapidxml::xml_document<>* pDoc, GLfloat value)
{
    GLchar buffer[64];
#ifdef _WIN32
    sprintf_s(buffer, "%f", value);
#else
    sprintf(buffer, "%f", value);
#endif
    return pDoc->allocate_string(buffer);
}

static GLchar* convertIntToChar(rapidxml::xml_document<>* pDoc, GLint value)
{
    GLchar buffer[64];
#ifdef _WIN32
    sprintf_s(buffer, "%i", value);
#else
    sprintf(buffer, "%i", value);
#endif
    return pDoc->allocate_string(buffer);
}

static GLchar* convertBoolToChar(rapidxml::xml_document<>* pDoc, GLboolean value)
{
    if(value)
    {
        return pDoc->allocate_string("TRUE");
    }
    else
    {
        return pDoc->allocate_string("FALSE");
    }
}

static GLfloat convertCharToFloat(GLchar* value)
{
    return static_cast<GLfloat>(std::atof(value));
}

static GLint convertCharToInt(GLchar* value)
{
    return std::atoi(value);
}

static GLboolean convertCharToBool(GLchar* value)
{
    std::string string(value);
    if(string == "TRUE")
    {
        return GL_TRUE;
    }
    else
    {
        return GL_FALSE;
    }
}

static std::string convertCharToString(GLchar* value)
{
    return std::string(value);
}

static GLuint checkValue(std::string value, std::string expectedValue, std::string nameOfFile)
{
    if(value != expectedValue)
    {
        LogError("In file '" + nameOfFile + ".xml' was read: '" + value + "', but expected: '" + expectedValue + "'");
        return 1;
    }

    return 0;
}

#endif
