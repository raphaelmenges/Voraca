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

#include "TfCreator.h"

TfCreator::TfCreator()
{
}

TfCreator::~TfCreator()
{
}

Transferfunction* TfCreator::create(GLint handle, std::string name)
{
	// Create new transferfunctions
	Transferfunction* pTransferfunction = new Transferfunction();
	pTransferfunction->init(handle, name);

	// Add leftend and rightend tfPoints
	pTransferfunction->internalAddTfPoint(TFPOINT_LOCATION_LEFTEND, glm::vec2(0,0.5f));
	pTransferfunction->internalAddTfPoint(TFPOINT_LOCATION_RIGHTEND, glm::vec2(1,0.5f));

	// Move and and set color
	pTransferfunction->setTfPoint(0, glm::vec2(0,0));
	TfPointValue value0;
	value0.color = glm::vec3(0,0,0);
	pTransferfunction->setValueOfTfPoint(0, value0);

	pTransferfunction->setTfPoint(1, glm::vec2(1,1));
	TfPointValue value1;
	value1.color = glm::vec3(1,1,1);
	pTransferfunction->setValueOfTfPoint(1, value1);

	return pTransferfunction;
}

GLboolean TfCreator::writeToFile(Transferfunction* pTransferfunction, GLboolean overwriteExisting)
{
	// Check whether there exists already a file with that name
	if(!overwriteExisting)
	{
		// Read XML from file
		std::ifstream in(std::string(TFCREATOR_PATH + pTransferfunction->getName() + ".xml").c_str());

		// Check whether file exisits
		if(in.is_open())
		{
			LogError("'" + TFCREATOR_PATH + pTransferfunction->getName() + ".xml' already exists!");
			return GL_FALSE;
		}
	}

	// Get tfPoints out of pointer
	std::vector<TfPoint>* pTfPoints = &(pTransferfunction->tfPoints);

	// Document object
	rapidxml::xml_document<> doc;

	// XML declaration
	rapidxml::xml_node<>* pDeclNode = doc.allocate_node(rapidxml::node_declaration);
	pDeclNode->append_attribute(doc.allocate_attribute("version", "1.0"));
	pDeclNode->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(pDeclNode);

	// Root node
	rapidxml::xml_node<>* pRootNode = doc.allocate_node(rapidxml::node_element, "transferfunction");
	pRootNode->append_attribute(doc.allocate_attribute("version", "1.0"));
	doc.append_node(pRootNode);

	// Leftend tfPoint
	rapidxml::xml_node<>* pLeftendNode = doc.allocate_node(rapidxml::node_element, "leftend");
	appendTfPoint(&(*pTfPoints)[0], &doc, pLeftendNode);
	pRootNode->append_node(pLeftendNode);

	// Normal tfPoints
	rapidxml::xml_node<>* pNormalNode = doc.allocate_node(rapidxml::node_element, "normal");
	appendNormalTfPoints(pTfPoints, &doc, pNormalNode);
	pRootNode->append_node(pNormalNode);

	// Rightend tfPoint
	rapidxml::xml_node<>* pRightendNode = doc.allocate_node(rapidxml::node_element, "rightend");
	appendTfPoint(&(*pTfPoints)[pTfPoints->size()-1], &doc, pRightendNode);
	pRootNode->append_node(pRightendNode);

	// Printing
	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	// Saving
	std::ofstream out(std::string(TFCREATOR_PATH + pTransferfunction->getName() + ".xml").c_str());
	out << xml_as_string;
	out.close();

	return GL_TRUE;
}

Transferfunction* TfCreator::readFromFile(std::string name, GLint handle)
{
	// Read XML from file
	std::ifstream in(std::string(TFCREATOR_PATH + name + ".xml").c_str());

	// Check whether file exisits
	if(!in.is_open())
	{
		LogWarning("'" + TFCREATOR_PATH + name + ".xml' was not found!");
		return NULL;
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

	// Create new transferfunction object
	Transferfunction* pTransferfunction = new Transferfunction();
	pTransferfunction->init(handle, name);

	// Transferfunction (jump over declaration)
	rapidxml::xml_node<>* pRootNode = doc.first_node()->next_sibling();
	checkValue(std::string(pRootNode->name()), "transferfunction", name);

	// Get Version
	rapidxml::xml_attribute<>* pRootNodeCurrentAttribute = pRootNode->first_attribute();
	checkValue(std::string(pRootNodeCurrentAttribute->name()), "version", name);
	GLfloat version = convertCharToFloat(pRootNodeCurrentAttribute->value());

	// Create tfPointHandleCounter
	GLint tfPointHandleCounter = 0;

	// Pointer to nodes
	rapidxml::xml_node<>* pChildNode;
	rapidxml::xml_node<>* pGrandChildNode;

	// *** LEFTEND ***
	pChildNode = pRootNode->first_node();
	checkValue(std::string(pChildNode->name()), "leftend", name);
	pGrandChildNode = pChildNode->first_node();
	extractTfPoint(pGrandChildNode, TFPOINT_LOCATION_LEFTEND, tfPointHandleCounter, pTransferfunction);
	tfPointHandleCounter++;

	// *** NORMAL TFPOINTS ***
	pChildNode = pChildNode->next_sibling();
	checkValue(std::string(pChildNode->name()), "normal", name);

	// Get first grand child
	pGrandChildNode = pChildNode->first_node();

	// When there are nodes, proceed
	while(pGrandChildNode !=0)
	{
		extractTfPoint(pGrandChildNode, TFPOINT_LOCATION_NORMAL, tfPointHandleCounter, pTransferfunction);
		tfPointHandleCounter++;
		pGrandChildNode = pGrandChildNode->next_sibling();
	}

	// *** RIGHTEND ***
	pChildNode = pChildNode->next_sibling();
	checkValue(std::string(pChildNode->name()), "rightend", name);
	pGrandChildNode = pChildNode->first_node();
	extractTfPoint(pGrandChildNode, TFPOINT_LOCATION_RIGHTEND, tfPointHandleCounter, pTransferfunction);
	tfPointHandleCounter++;

	// Set handle counter
	pTransferfunction->tfPointHandleCounter = tfPointHandleCounter;

	return pTransferfunction;
}

void TfCreator::appendTfPoint(TfPoint* pTfPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
	// Create tfPointNode
	rapidxml::xml_node<>* pTfPointNode = pDoc->allocate_node(rapidxml::node_element, "tfPoint");

	// Append information about linking of control points
	pTfPointNode->append_attribute(pDoc->allocate_attribute("controlPointsLinked", convertBoolToChar(pDoc, pTfPoint->getControlPointsLinked())));

	// Point
	appendPoint(pTfPoint->getPoint(), pDoc, pTfPointNode);

	// LeftControlPoint
	if(pTfPoint->getLocation() != TFPOINT_LOCATION_LEFTEND)
	{
		appendLeftControlPoint(pTfPoint->getLeftControlPoint(), pDoc, pTfPointNode);
	}

	// RightControlPoint
	if(pTfPoint->getLocation() != TFPOINT_LOCATION_RIGHTEND)
	{
		appendRightControlPoint(pTfPoint->getRightControlPoint(), pDoc, pTfPointNode);
	}

	// Values
	TfPointValue value = pTfPoint->getValue();
	appendVec3(value.color, "color", pDoc, pTfPointNode);
	appendFloat(value.ambientMultiplier, "ambientMultiplier", pDoc, pTfPointNode);
	appendFloat(value.specularMultiplier, "specularMultiplier", pDoc, pTfPointNode);
	appendFloat(value.specularSaturation, "specularSaturation", pDoc, pTfPointNode);
	appendFloat(value.specularPower, "specularPower", pDoc, pTfPointNode);
	appendFloat(value.gradientAlphaMultiplier, "gradientAlphaMultiplier", pDoc, pTfPointNode);
	appendFloat(value.fresnelAlphaMultiplier, "fresnelAlphaMultiplier", pDoc, pTfPointNode);
	appendFloat(value.reflectionColorMultiplier, "reflectionColorMultiplier", pDoc, pTfPointNode);
	appendFloat(value.emissionColorMultiplier, "emissionColorMultiplier", pDoc, pTfPointNode);

	// Append tfPointNode
	pParent->append_node(pTfPointNode);
}

void TfCreator::appendNormalTfPoints(std::vector<TfPoint>* pTfPoints, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
	// Iterate over normal tfPoints
	for(std::vector<TfPoint>::iterator it = pTfPoints->begin()+1; it != pTfPoints->end()-1; ++it)
	{
		appendTfPoint(&(*it), pDoc, pParent);
	}
}

void TfCreator::appendPoint(glm::vec2 point, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
	// Create pointNode
	rapidxml::xml_node<>* pPointNode = pDoc->allocate_node(rapidxml::node_element, "point");

	// Append coordinates
	pPointNode->append_attribute(pDoc->allocate_attribute("x", convertFloatToChar(pDoc, point.x)));
	pPointNode->append_attribute(pDoc->allocate_attribute("y", convertFloatToChar(pDoc, point.y)));

	// Append pointNode
	pParent->append_node(pPointNode);
}

void TfCreator::appendLeftControlPoint(glm::vec2 leftControlPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
	// Create leftControlPointNode
	rapidxml::xml_node<>* pLeftControlPointNode = pDoc->allocate_node(rapidxml::node_element, "leftControlPoint");

	// Append coordinates
	pLeftControlPointNode->append_attribute(pDoc->allocate_attribute("x", convertFloatToChar(pDoc, leftControlPoint.x)));
	pLeftControlPointNode->append_attribute(pDoc->allocate_attribute("y", convertFloatToChar(pDoc, leftControlPoint.y)));

	// Append leftControlPointNode
	pParent->append_node(pLeftControlPointNode);
}

void TfCreator::appendRightControlPoint(glm::vec2 rightControlPoint, rapidxml::xml_document<>* pDoc, rapidxml::xml_node<>* pParent)
{
	// Create rightControlPointNode
	rapidxml::xml_node<>* pRightControlPointNode = pDoc->allocate_node(rapidxml::node_element, "rightControlPoint");

	// Append coordinates
	pRightControlPointNode->append_attribute(pDoc->allocate_attribute("x", convertFloatToChar(pDoc, rightControlPoint.x)));
	pRightControlPointNode->append_attribute(pDoc->allocate_attribute("y", convertFloatToChar(pDoc, rightControlPoint.y)));

	// Append rightControlPointNode
	pParent->append_node(pRightControlPointNode);
}

void TfCreator::extractTfPoint(rapidxml::xml_node<>* pNode, TfPointLocation location, GLint tfPointHandleCounter, Transferfunction* pTransferfunction)
{
	// Check if node is a tfPoint
	checkValue(std::string(pNode->name()), "tfPoint", pTransferfunction->getName());

	// Create tfPoint
	TfPoint tfPoint;

	// Extract whether control points are linked
	rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "controlPointsLinked", pTransferfunction->getName());
	GLboolean controlPointsLinked = convertCharToBool(pNodeCurrentAttribute->value());

	// Initialize tfPoint
	tfPoint.init(
		tfPointHandleCounter,
		location,
		glm::vec2(0,0),
		&(pTransferfunction->pointShader),
		&(pTransferfunction->controlPointShader),
		&(pTransferfunction->controlLineShader),
		pTransferfunction->pointShaderModelHandle,
		pTransferfunction->pointShaderViewHandle,
		pTransferfunction->pointShaderInnerColorHandle,
		pTransferfunction->pointShaderBorderColorHandle,
		pTransferfunction->controlPointShaderModelHandle,
		pTransferfunction->controlPointShaderViewHandle,
		pTransferfunction->controlLineShaderModelHandle,
		pTransferfunction->controlLineShaderViewHandle,
		pTransferfunction->controlLineShaderColorHandle);

	// Extract point
	rapidxml::xml_node<>* pChild = pNode->first_node();
	glm::vec2 point = extractPoint(pChild, pTransferfunction);
	tfPoint.setPoint(point);

	// Extract left control point
	if(location != TFPOINT_LOCATION_LEFTEND)
	{
		pChild = pChild->next_sibling();
		tfPoint.setLeftControlPoint(extractLeftControlPoint(pChild, pTransferfunction));
	}
	else
	{
		point.x = 0;
	}

	// Extract right control point
	if(location != TFPOINT_LOCATION_RIGHTEND)
	{
		pChild = pChild->next_sibling();
		tfPoint.setRightControlPoint(extractRightControlPoint(pChild, pTransferfunction));
	}
	else
	{
		point.x = 1;
	}

	// Extract values
	TfPointValue value = tfPoint.getValue();
	pChild = pChild->next_sibling();
	value.color = extractVec3("color", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.ambientMultiplier = extractFloat("ambientMultiplier", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.specularMultiplier = extractFloat("specularMultiplier", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.specularSaturation = extractFloat("specularSaturation", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.specularPower = extractFloat("specularPower", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.gradientAlphaMultiplier = extractFloat("gradientAlphaMultiplier", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.fresnelAlphaMultiplier = extractFloat("fresnelAlphaMultiplier", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.reflectionColorMultiplier = extractFloat("reflectionColorMultiplier", pTransferfunction->getName(), pChild);
	pChild = pChild->next_sibling();
	value.emissionColorMultiplier = extractFloat("emissionColorMultiplier", pTransferfunction->getName(), pChild);

	tfPoint.setValue(value);

	// Set control points link state
	tfPoint.setControlPointsLinked(controlPointsLinked);

	// Add tfPoint to transferfunction
	pTransferfunction->tfPoints.push_back(tfPoint);

}

glm::vec2 TfCreator::extractPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction)
{
	// Check if node is a point
	checkValue(std::string(pNode->name()), "point", pTransferfunction->getName());

	// Result
	glm::vec2 point;

	// Extract x
	rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "x", pTransferfunction->getName());
	point.x = convertCharToFloat(pNodeCurrentAttribute->value());

	// Extract y
	pNodeCurrentAttribute = pNodeCurrentAttribute->next_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "y", pTransferfunction->getName());
	point.y = convertCharToFloat(pNodeCurrentAttribute->value());

	return point;
}

glm::vec2 TfCreator::extractLeftControlPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction)
{
	// Check if node is a leftControlPoint
	checkValue(std::string(pNode->name()), "leftControlPoint", pTransferfunction->getName());

	// Result
	glm::vec2 leftControlPoint;

	// Extract x
	rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "x", pTransferfunction->getName());
	leftControlPoint.x = convertCharToFloat(pNodeCurrentAttribute->value());

	// Extract y
	pNodeCurrentAttribute = pNodeCurrentAttribute->next_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "y", pTransferfunction->getName());
	leftControlPoint.y = convertCharToFloat(pNodeCurrentAttribute->value());

	return leftControlPoint;
}

glm::vec2 TfCreator::extractRightControlPoint(rapidxml::xml_node<>* pNode, Transferfunction* pTransferfunction)
{
	// Check if node is a rightControlPoint
	checkValue(std::string(pNode->name()), "rightControlPoint", pTransferfunction->getName());

	// Result
	glm::vec2 rightControlPoint;

	// Extract x
	rapidxml::xml_attribute<>* pNodeCurrentAttribute = pNode->first_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "x", pTransferfunction->getName());
	rightControlPoint.x = convertCharToFloat(pNodeCurrentAttribute->value());

	// Extract y
	pNodeCurrentAttribute = pNodeCurrentAttribute->next_attribute();
	checkValue(std::string(pNodeCurrentAttribute->name()), "y", pTransferfunction->getName());
	rightControlPoint.y = convertCharToFloat(pNodeCurrentAttribute->value());

	return rightControlPoint;
}