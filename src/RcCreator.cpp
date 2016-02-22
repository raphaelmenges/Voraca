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

#include "RcCreator.h"

RcCreator::RcCreator()
{
}

RcCreator::~RcCreator()
{
}

Raycaster* RcCreator::create(GLint handle, std::string name)
{
	Raycaster* pRaycaster;
	pRaycaster = new Raycaster();
	pRaycaster->init(handle, name);
	return pRaycaster;
}

GLboolean RcCreator::writeToFile(Raycaster* pRaycaster, GLboolean overwriteExisting)
{
	// Check whether there exists already a file with that name
	if(!overwriteExisting)
	{
		// Read XML from file
		std::ifstream in(std::string(RCCREATOR_PATH + pRaycaster->getName() + ".xml").c_str());

		// Check whether file exisits
		if(in.is_open())
		{
			LogError("'" + RCCREATOR_PATH + pRaycaster->getName() + ".xml' already exists!");
			return GL_FALSE;
		}
	}

	// Document object
	rapidxml::xml_document<> doc;

	// XML declaration
	rapidxml::xml_node<>* pDeclNode = doc.allocate_node(rapidxml::node_declaration);
	pDeclNode->append_attribute(doc.allocate_attribute("version", "1.0"));
	pDeclNode->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(pDeclNode);

	// Root node
	rapidxml::xml_node<>* pRootNode = doc.allocate_node(rapidxml::node_element, "raycaster");
	pRootNode->append_attribute(doc.allocate_attribute("version", "1.0"));
	doc.append_node(pRootNode);

	// Defines
	rapidxml::xml_node<>* pDefinesNode = doc.allocate_node(rapidxml::node_element, "defines");
	appendBool(pRaycaster->getUseERT(), "useERT", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseLocalIllumination(), "useLocalIllumination", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseJittering(), "useJittering", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseExtentPreservingJittering(), "useExtentPreservingJittering", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseSimpleESS(), "useSimpleESS", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseNormalsOfClassifiedData(), "useNormalsOfClassifiedData", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseExtentAwareNormals(), "useExtentAwareNormals", &doc, pDefinesNode);
	appendBool(pRaycaster->getUsePreintegration(), "usePreintegration", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseAdaptiveSampling(), "useAdaptiveSampling", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseVoxelSpacedSampling(), "useVoxelSpacedSampling", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseExtentAwareNormals(), "useExtentAwareNormals", &doc, pDefinesNode);
	appendBool(pRaycaster->getUsePreintegration(), "usePreintegration", &doc, pDefinesNode);

	appendBool(pRaycaster->getUseGradientAlphaMultiplier(), "useGradientAlphaMultiplier", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseFresnelAlphaMultiplier(), "useFresnelAlphaMultiplier", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseReflectionColorMultiplier(), "useReflectionColorMultiplier", &doc, pDefinesNode);
	appendBool(pRaycaster->getUseEmissionColorMultiplier(), "useEmissionColorMultiplier", &doc, pDefinesNode);

	pRootNode->append_node(pDefinesNode);

	// Properties
	rapidxml::xml_node<>* pPropertiesNode = doc.allocate_node(rapidxml::node_element, "properties");
	appendFloat(pRaycaster->getProperties().stepSize, "stepSize", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().stepSizeMultiplier, "stepSizeMultiplier", &doc, pPropertiesNode);
	appendFloat(static_cast<GLfloat>(pRaycaster->getProperties().outerIterations), "outerIterations", &doc, pPropertiesNode);
	appendFloat(static_cast<GLfloat>(pRaycaster->getProperties().innerIterations), "innerIterations", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().alphaThreshold, "alphaThreshold", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().jitteringRangeMultiplier, "jitteringRangeMultiplier", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().normalRangeMultiplier, "normalRangeMultiplier", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().fresnelPower, "fresnelPower", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().minAdaptiveStepSize, "minAdaptiveStepSize", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().maxAdaptiveStepSize, "maxAdaptiveStepSize", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().minAdaptiveStepSizeMultiplier, "minAdaptiveStepSizeMultiplier", &doc, pPropertiesNode);
	appendFloat(pRaycaster->getProperties().maxAdaptiveStepSizeMultiplier, "maxAdaptiveStepSizeMultiplier", &doc, pPropertiesNode);
	pRootNode->append_node(pPropertiesNode);

	// Printing
	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	// Saving
	std::ofstream out(std::string(RCCREATOR_PATH + pRaycaster->getName() + ".xml").c_str());
	out << xml_as_string;
	out.close();

	return GL_FALSE;
}

Raycaster* RcCreator::readFromFile(std::string name, GLint handle)
{
	// Read XML from file
	std::ifstream in(std::string(RCCREATOR_PATH + name + ".xml").c_str());

	// Check whether file exisits
	if(!in.is_open())
	{
		LogWarning("'" + RCCREATOR_PATH + name + ".xml' was not found!");
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

	// Create new raycaster object
	Raycaster* pRaycaster = new Raycaster();
	pRaycaster->init(handle, name);

	// Raycaster (jump over declaration)
	rapidxml::xml_node<>* pRootNode = doc.first_node()->next_sibling();
	checkValue(std::string(pRootNode->name()), "raycaster", name);

	// Get Version
	rapidxml::xml_attribute<>* pRootNodeCurrentAttribute = pRootNode->first_attribute();
	checkValue(std::string(pRootNodeCurrentAttribute->name()), "version", name);
	GLfloat version = convertCharToFloat(pRootNodeCurrentAttribute->value());

	// Pointer to nodes
	rapidxml::xml_node<>* pChildNode;
	rapidxml::xml_node<>* pGrandChildNode;
	rapidxml::xml_attribute<>* pCurrentAttribute;

	// Defines
	pChildNode = pRootNode->first_node();
	checkValue(std::string(pChildNode->name()), "defines", name);

	pGrandChildNode = pChildNode->first_node();
	checkValue(std::string(pGrandChildNode->name()), "useERT", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useERT = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useLocalIllumination", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useLocalIllumination = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useJittering", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useJittering = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useExtentPreservingJittering", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useExtentPreservingJittering = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useSimpleESS", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useSimpleESS = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useNormalsOfClassifiedData", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useNormalsOfClassifiedData = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useExtentAwareNormals", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useExtentAwareNormals = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "usePreintegration", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->usePreintegration = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useAdaptiveSampling", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useAdaptiveSampling = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useVoxelSpacedSampling", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useVoxelSpacedSampling = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useExtentAwareNormals", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useExtentAwareNormals = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "usePreintegration", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->usePreintegration = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useGradientAlphaMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useGradientAlphaMultiplier = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useFresnelAlphaMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useFresnelAlphaMultiplier = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useReflectionColorMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useReflectionColorMultiplier = convertCharToBool(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "useEmissionColorMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	pRaycaster->useEmissionColorMultiplier  = convertCharToBool(pCurrentAttribute->value());

	// Properties
	RaycasterProperties properties;

	pChildNode = pChildNode->next_sibling();
	checkValue(std::string(pChildNode->name()), "properties", name);

	pGrandChildNode = pChildNode->first_node();
	checkValue(std::string(pGrandChildNode->name()), "stepSize", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.stepSize = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "stepSizeMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.stepSizeMultiplier = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "outerIterations", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.outerIterations = static_cast<GLint>(convertCharToFloat(pCurrentAttribute->value()));

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "innerIterations", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.innerIterations = static_cast<GLint>(convertCharToFloat(pCurrentAttribute->value()));

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "alphaThreshold", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.alphaThreshold = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "jitteringRangeMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.jitteringRangeMultiplier = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "normalRangeMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.normalRangeMultiplier = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "fresnelPower", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.fresnelPower = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "minAdaptiveStepSize", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.minAdaptiveStepSize = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "maxAdaptiveStepSize", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.maxAdaptiveStepSize = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "minAdaptiveStepSizeMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.minAdaptiveStepSizeMultiplier = convertCharToFloat(pCurrentAttribute->value());

	pGrandChildNode = pGrandChildNode->next_sibling();
	checkValue(std::string(pGrandChildNode->name()), "maxAdaptiveStepSizeMultiplier", name);
	pCurrentAttribute = pGrandChildNode->first_attribute();
	properties.maxAdaptiveStepSizeMultiplier = convertCharToFloat(pCurrentAttribute->value());

	pRaycaster->setProperties(properties);
	pRaycaster->reloadShader();

	return pRaycaster;
}
