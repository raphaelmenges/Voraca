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

#include "Launch.h"

// Global static pointer to the single instance
Launch* Launch::pInstance = NULL;

 GLint Launch::getWindowWidth(GLuint& value)
{
	createInstance();
	value = pInstance->windowWidth;
	return static_cast<GLint>(!pInstance->successfullRead);
}

GLint Launch::getWindowHeight(GLuint& value)
{
	createInstance();
	value = pInstance->windowHeight;
	return static_cast<GLint>(!pInstance->successfullRead);
}

GLint Launch::isFullscreen(GLboolean& value)
{
	createInstance();
	value = pInstance->fullscreen;
	return static_cast<GLint>(!pInstance->successfullRead);
}

GLint Launch::getVolume(std::string& value)
{
	createInstance();
	value = pInstance->volume;
	return static_cast<GLint>(!pInstance->successfullRead);
}

GLint Launch::getTransferfunction(std::string& value)
{
	createInstance();
	value = pInstance->transferfunction;
	return static_cast<GLint>(!pInstance->successfullRead);
}

GLint Launch::getRaycaster(std::string& value)
{
	createInstance();
	value = pInstance->raycaster;
	return static_cast<GLint>(!pInstance->successfullRead);
}


Launch::Launch()
{
	successfullRead = GL_FALSE;

	std::string nameOfFile = "launch";

	// Read XML from file
	std::ifstream in(std::string(nameOfFile + ".xml").c_str());

	// Check whether file exisits
	if(!in.is_open())
	{
		LogWarning("'launch.xml' was not found!");
		createNewLaunchFile();
		LogInfo("New 'launch.xml' was created! 'None' warnings will follow ;)");
		in.open(std::string(nameOfFile + ".xml").c_str());
	}

	// File exisits, lets hope the best
	std::stringstream buffer;
	buffer << in.rdbuf();
	in.close();
	std::string content(buffer.str());
	std::vector<GLchar> xml(content.begin(), content.end());
    xml.push_back('\0');

	// Parse it to a doc
	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_declaration_node | rapidxml::parse_no_data_nodes>(&xml[0]);

	// Launch (jump over declaration)
	rapidxml::xml_node<>* pRootNode = doc.first_node()->next_sibling();
	checkValue(std::string(pRootNode->name()), "launch", nameOfFile);

	// Get Version
	rapidxml::xml_attribute<>* pRootNodeCurrentAttribute = pRootNode->first_attribute();
	checkValue(std::string(pRootNodeCurrentAttribute->name()), "version", nameOfFile);
	GLfloat version = convertCharToFloat(pRootNodeCurrentAttribute->value());

	// Preparation of reading
	rapidxml::xml_node<>* pChildNode;
	rapidxml::xml_attribute<>* pNodeCurrentAttribute;
	GLuint errorCounter = 0;

	// Window width
	pChildNode = pRootNode->first_node();
	errorCounter += checkValue(std::string(pChildNode->name()), "windowWidth", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	windowWidth = static_cast<GLuint>(convertCharToFloat(pNodeCurrentAttribute->value()));

	// Window height
	pChildNode = pChildNode->next_sibling();
	errorCounter += checkValue(std::string(pChildNode->name()), "windowHeight", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	windowHeight = static_cast<GLuint>(convertCharToFloat(pNodeCurrentAttribute->value()));

	// Fullscreen
	pChildNode = pChildNode->next_sibling();
	errorCounter += checkValue(std::string(pChildNode->name()), "fullscreen", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	fullscreen = convertCharToBool(pNodeCurrentAttribute->value());

	// Volume
	pChildNode = pChildNode->next_sibling();
	errorCounter += checkValue(std::string(pChildNode->name()), "volume", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	volume = convertCharToString(pNodeCurrentAttribute->value());

	// Transferfunction
	pChildNode = pChildNode->next_sibling();
	errorCounter += checkValue(std::string(pChildNode->name()), "transferfunction", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	transferfunction = convertCharToString(pNodeCurrentAttribute->value());

	// Raycaster
	pChildNode = pChildNode->next_sibling();
	errorCounter += checkValue(std::string(pChildNode->name()), "raycaster", nameOfFile);
	pNodeCurrentAttribute = pChildNode->first_attribute();
	errorCounter += checkValue(std::string(pNodeCurrentAttribute->name()), "value", nameOfFile);
	raycaster = convertCharToString(pNodeCurrentAttribute->value());

	// Check whether everything was read successfully
	successfullRead = (errorCounter == 0);
}

Launch::~Launch()
{
}

void Launch::createInstance()
{
	// Exists an instance?
	if(pInstance == NULL)
	{
		// Create instance of it on stack
		static Launch launch;
		pInstance = &launch;
	}
}

void Launch::createNewLaunchFile()
{
	std::ofstream out("launch.xml");

	out << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << "\n";
	out << "<launch version=\"1.0\">" << "\n";
	out << "<windowWidth value=\"1280\"/>" << "\n";
	out << "<windowHeight value=\"720\"/>" << "\n";
	out << "<fullscreen value=\"FALSE\"/>" << "\n";
	out << "<volume value=\"none\"/>" << "\n";
	out << "<transferfunction value=\"none\"/>" << "\n";
	out << "<raycaster value=\"none\"/>" << "\n";
	out << "</launch>";

	out.close();
}