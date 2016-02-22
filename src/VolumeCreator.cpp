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

#include "VolumeCreator.h"

VolumeCreator::VolumeCreator()
{
}

VolumeCreator::~VolumeCreator()
{
}

Volume* VolumeCreator::importPVM(std::string name, GLint handle)
{
    std::string path = VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_PVM + name + ".pvm";

    std::ifstream in(path.c_str(), std::ios::in|std::ios::binary);

    // Check whether file exisits
    if(!in.is_open())
    {
        // Try it without ".pvm"
        std::string path = VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_PVM + name;
        in.open(path.c_str(), std::ios::in|std::ios::binary);

        // Ok, there is really no file
        if(!in.is_open())
        {
            LogWarning("'" +  VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_PVM + name + ".pvm' was not found!");
            return NULL;
        }
    }

    // *** READ HEADER ***
    in.seekg(0, std::ios::beg);
    GLchar buffer[5];
    GLchar nextLineDummy;

    // Format
    in.read(buffer, 4*sizeof(GLchar));
    buffer[4] = 0;
    std::string format(buffer);
    if(format != "PVM3")
    {
        LogWarning("Cannot import anything else than PVM3");
        return NULL;
    }
    in.read(&nextLineDummy, sizeof(GLchar));

    // *** READ INFORMATION ABOUT VOLUME ***
    glm::vec3 res;
    glm::vec3 scale;
    GLint bitDepth;

    // Read resolution
    res.x = static_cast<GLfloat>(extractIntFromCharArray(&in));
    res.y = static_cast<GLfloat>(extractIntFromCharArray(&in));
    res.z = static_cast<GLfloat>(extractIntFromCharArray(&in));

    if(res.x < 4 || res.y < 4 || res.z < 4)
    {
        LogError("'" +  VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_PVM + name + ".pvm' resolution too low (under 4x4x4)!");
        return NULL;
    }

    // Read scales
    scale.x = extractFloatFromCharArray(&in);
    scale.y = extractFloatFromCharArray(&in);
    scale.z = extractFloatFromCharArray(&in);

    // Read value resolution (bitDepth is 1 (8Bit) or 2 (16 bit))
    bitDepth = extractIntFromCharArray(&in);

    // *** READ RAW DATA ***

    GLuint voxelCount = static_cast<GLuint>(res.x * res.y * res.z);
    GLubyte* volumeData = (GLubyte*)calloc(voxelCount, bitDepth * sizeof(GLubyte));

    // Read data
    in.read((char*)volumeData, voxelCount * bitDepth * sizeof(GLubyte));

    // Close file
    in.close();

    // Value resolution
    VolumeValueResolution valueResolution;
    if(bitDepth == 1)
    {
        valueResolution = VOLUME_8BIT;
    }
    else
    {
        valueResolution = VOLUME_16BIT;
    }

    GLuint textureHandle = createTexture(volumeData, res, valueResolution, VOLUMEPROPERTIES_USE_LINEAR_FILTERING);

    // *** CREATE VOLUME ***
    Volume* pVolume = new Volume();

    // Initialize volume
    pVolume->init(handle, name, textureHandle, res, scale, valueResolution, volumeData);

    // Volume raw data is freed by volume object at destruction

    return pVolume;
}

Volume* VolumeCreator::importDAT(std::string name, GLint handle)
{
    std::string path = VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_DAT + name + ".dat";

    std::ifstream in(path.c_str(), std::ios::in|std::ios::binary);

    // Check whether file exisits
    if(!in.is_open())
    {
        // Try it without ".dat"
        std::string path = VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_DAT + name;
        in.open(path.c_str(), std::ios::in|std::ios::binary);

        // Ok, there is really no file
        if(!in.is_open())
        {
            LogWarning("'" +  VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_DAT + name + ".dat' was not found!");
            return NULL;
        }
    }

    // *** READ HEADER ***
    in.seekg(0, std::ios::beg);
    GLushort xdim, ydim, zdim;
    in.read((GLchar*)(&xdim), sizeof(GLushort));
    in.read((GLchar*)(&ydim), sizeof(GLushort));
    in.read((GLchar*)(&zdim), sizeof(GLushort));
    glm::vec3 res(xdim, ydim, zdim);

    if(res.x < 4 || res.y < 4 || res.z < 4)
    {
        LogError("'" +  VOLUMECREATOR_PATH + VOLUMECREATOR_SUBDIR_DAT + name + ".dat' resolution too low (under 4x4x4)!");
        return NULL;
    }

    // *** READ RAW DATA ***

    GLuint voxelCount = static_cast<GLuint>(res.x * res.y * res.z);
    GLubyte* volumeData = (GLubyte*)calloc(voxelCount, sizeof(GLushort));

    // Read data
    in.read((GLchar*)volumeData, voxelCount * sizeof(GLushort));

    GLuint textureHandle = createTexture(volumeData, res, VOLUME_16BIT, VOLUMEPROPERTIES_USE_LINEAR_FILTERING);

    /*
    // Some hack to load giant volumes. 16Bit depth is too much, so let reduce it
    GLushort value;
    GLfloat tmpValue;
    GLubyte* volumeData = (GLubyte*)calloc(voxelCount, sizeof(GLubyte));
    GLuint iterations =  xdim*ydim*zdim;
    for(GLuint i = 0; i < iterations; i++)
    {
        in.read((GLchar*)(&value), sizeof(GLushort));
        tmpValue = value/65535.0f * 20.0f * 255.0f;
        volumeData[i] = static_cast<GLubyte>(tmpValue);

    }
    GLuint textureHandle = createTexture(volumeData, res, VOLUME_8BIT, VOLUMEPROPERTIES_USE_LINEAR_FILTERING);*/

    // *** CREATE VOLUME ***
    Volume* pVolume = new Volume();

    // Initialize volume
    pVolume->init(handle, name, textureHandle, res, glm::vec3(1,1,1), VOLUME_16BIT, volumeData);

    // Volume raw data is freed by volume object at destruction

    return pVolume;
}

Volume* VolumeCreator::createDefaultVolume(std::string name, GLint handle)
{
    const GLuint xdim = 4;
    const GLuint ydim = 4;
    const GLuint zdim = 4;

    // Initialize
    GLubyte* volumeData = (GLubyte*)calloc(xdim * ydim * zdim, sizeof(GLubyte));
    for(GLint i = 0; i < xdim; i++)
    {
        for(GLint j = 0; j < ydim; j++)
        {
            for(GLint k = 0; k < zdim; k++)
            {
                volumeData[i + j*ydim + k*ydim*zdim] = (255 * (i + j*ydim + k*ydim*zdim))/(xdim*ydim*zdim-1);
            }
        }
    }

    GLuint textureHandle = createTexture(volumeData, glm::vec3(xdim, ydim, zdim), VOLUME_8BIT, VOLUMEPROPERTIES_USE_LINEAR_FILTERING);

    // Create volume object
    Volume* pVolume = new Volume();

    // Initialize volume
    pVolume->init(handle, name, textureHandle, glm::vec3(xdim, ydim, zdim), glm::vec3(1.0f), VOLUME_8BIT, volumeData);

    return pVolume;
}

GLboolean VolumeCreator::writeToFile(Volume* pVolume, GLboolean overwriteExisting)
{
    // Check whether there exists already a file with that name
    if(!overwriteExisting)
    {
        // Read XML from file
        std::ifstream in(std::string(VOLUMECREATOR_PATH + pVolume->getName() + ".xml").c_str());

        // Check whether file exisits
        if(in.is_open())
        {
            LogError("'" + VOLUMECREATOR_PATH + pVolume->getName() + ".xml' already exists!");
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
    rapidxml::xml_node<>* pRootNode = doc.allocate_node(rapidxml::node_element, "volume");
    pRootNode->append_attribute(doc.allocate_attribute("version", "1.0"));
    doc.append_node(pRootNode);

    // Get properties
    VolumeProperties properties = pVolume->getProperties();

    // Mirror
    appendBool(properties.mirrorX, "mirrorX", &doc, pRootNode);
    appendBool(properties.mirrorY, "mirrorY", &doc, pRootNode);
    appendBool(properties.mirrorZ, "mirrorZ", &doc, pRootNode);

    // Filtering
    appendBool(properties.useLinearFiltering, "useLinearFiltering", &doc, pRootNode);

    // Volume resolution
    appendVec3(pVolume->getVolumeResolution(), "volumeResolution", &doc, pRootNode);

    // Voxel scale
    appendVec3(pVolume->getVoxelScale(), "voxelScale", &doc, pRootNode);

    // Value resolution
    GLfloat bitDepth = 8;
    if (pVolume->getValueResolution() == VOLUME_16BIT)
    {
        bitDepth = 16;
    }
    appendFloat(bitDepth, "valueResolution", &doc, pRootNode);

    // Voxel scale multiplier
    appendVec3(properties.voxelScaleMultiplier, "voxelScaleMultiplier", &doc, pRootNode);

    // Value offset
    appendFloat(properties.valueOffset, "valueOffset", &doc, pRootNode);

    // Value scale
    appendFloat(properties.valueScale, "valueScale", &doc, pRootNode);

    // Rotation
    appendVec3(properties.eulerZXZRotation, "eulerZXZRotation", &doc, pRootNode);

    // Printing
    std::string xml_as_string;
    rapidxml::print(std::back_inserter(xml_as_string), doc);

    // Saving of xml
    std::ofstream out(std::string(VOLUMECREATOR_PATH + pVolume->getName() + ".xml").c_str());
    out << xml_as_string;
    out.close();

    // Saving of raw data
    glm::vec3 volumeResolution = pVolume->getVolumeResolution();
    GLuint size = static_cast<GLuint>(volumeResolution.x * volumeResolution.y * volumeResolution.z);
    FILE* pRawDataFile = UT::openFile(std::string(VOLUMECREATOR_PATH + pVolume->getName() + ".raw"));
    fwrite(pVolume->pRawData, static_cast<size_t>(bitDepth/8)*sizeof(GLubyte), size, pRawDataFile);
    fclose(pRawDataFile);

    return GL_TRUE;
}

Volume* VolumeCreator::readFromFile(std::string name, GLint handle)
{
    // First try to open raw data
    std::string path = VOLUMECREATOR_PATH + name + ".raw";
    std::ifstream rawDataFile(path.c_str(), std::ios::in|std::ios::binary);

    // Check whether file exisits
    if(!rawDataFile.is_open())
    {
        LogWarning("'" + VOLUMECREATOR_PATH + name + ".raw' was not found!");
        return NULL;
    }

    // *** READ XML ***

    // Read XML from file
    std::ifstream in(std::string(VOLUMECREATOR_PATH + name + ".xml").c_str());

    // Check whether file exisits
    if(!in.is_open())
    {
        LogWarning("'" + VOLUMECREATOR_PATH + name + ".xml' was not found!");
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

    // Volume (jump over declaration)
    rapidxml::xml_node<>* pRootNode = doc.first_node()->next_sibling();
    checkValue(std::string(pRootNode->name()), "volume", name);

    // Get Version
    rapidxml::xml_attribute<>* pRootNodeCurrentAttribute = pRootNode->first_attribute();
    checkValue(std::string(pRootNodeCurrentAttribute->name()), "version", name);
    GLfloat version = convertCharToFloat(pRootNodeCurrentAttribute->value());

    // Pointer to nodes
    rapidxml::xml_node<>* pChildNode = pRootNode->first_node();

    // Create properties
    VolumeProperties properties;

    // Mirror
    properties.useLinearFiltering = extractBool("mirrorX", name, pChildNode);

    pChildNode = pChildNode->next_sibling();
    properties.useLinearFiltering = extractBool("mirrorY", name, pChildNode);

    pChildNode = pChildNode->next_sibling();
    properties.useLinearFiltering = extractBool("mirrorZ", name, pChildNode);

    // Filtering
    pChildNode = pChildNode->next_sibling();
    properties.useLinearFiltering = extractBool("useLinearFiltering", name, pChildNode);

    // Volume resolution
    pChildNode = pChildNode->next_sibling();
    glm::vec3 volumeResolution = extractVec3("volumeResolution", name, pChildNode);

    if(volumeResolution.x < 4 || volumeResolution.y < 4 || volumeResolution.z < 4)
    {
        LogError("'" +  VOLUMECREATOR_PATH + name + ".xml' resolution too low (under 4x4x4)!");
        return NULL;
    }

    // Voxel scale
    pChildNode = pChildNode->next_sibling();
    glm::vec3 voxelScale = extractVec3("voxelScale", name, pChildNode);

    // Value resolution
    pChildNode = pChildNode->next_sibling();
    GLuint bitDepth = static_cast<GLuint>(extractFloat("valueResolution", name, pChildNode))/8;
    VolumeValueResolution valueResolution = VOLUME_8BIT;
    if(bitDepth == 2)
    {
        valueResolution = VOLUME_16BIT;
    }

    // Voxel scale
    pChildNode = pChildNode->next_sibling();
    properties.voxelScaleMultiplier = extractVec3("voxelScaleMultiplier", name, pChildNode);

    // Value offset
    pChildNode = pChildNode->next_sibling();
    properties.valueOffset = extractFloat("valueOffset", name, pChildNode);

    // Value scale
    pChildNode = pChildNode->next_sibling();
    properties.valueScale = extractFloat("valueScale", name, pChildNode);

    // Rotation
    pChildNode = pChildNode->next_sibling();
    properties.eulerZXZRotation = extractVec3("eulerZXZRotation", name, pChildNode);

    // *** READ RAW DATA ***

    GLuint voxelCount = static_cast<GLuint>(volumeResolution.x * volumeResolution.y * volumeResolution.z);
    GLubyte* volumeData = (GLubyte*)calloc(voxelCount,  static_cast<GLuint>(bitDepth) * sizeof(GLubyte));

    // Read data
    rawDataFile.read((char*)volumeData, static_cast<GLuint>(voxelCount * bitDepth) * sizeof(GLubyte));

    GLuint textureHandle = createTexture(volumeData, volumeResolution, valueResolution, properties.useLinearFiltering);

    // Volume raw data is freed by volume object at destruction

    // *** CREATE VOLUME ***
    Volume* pVolume = new Volume();

    // Initialize volume
    pVolume->init(handle, name, textureHandle, volumeResolution, voxelScale, valueResolution, volumeData);
    pVolume->setProperties(properties);

    return pVolume;
}

GLint VolumeCreator::createTexture(GLubyte* volumeData, glm::vec3 volumeResolution, VolumeValueResolution valueResolution, GLboolean useLinearFiltering)
{
    // Assign to handle and set parameters
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    if(useLinearFiltering)
    {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    if(valueResolution == VOLUME_8BIT)
    {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, static_cast<GLuint>(volumeResolution.x), static_cast<GLuint>(volumeResolution.y), static_cast<GLuint>(volumeResolution.z), 0, GL_RED, GL_UNSIGNED_BYTE, volumeData);
    }
    else
    {
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R16, static_cast<GLuint>(volumeResolution.x), static_cast<GLuint>(volumeResolution.y), static_cast<GLuint>(volumeResolution.z), 0, GL_RED, GL_UNSIGNED_SHORT, volumeData);
    }

    // Unbind texture
    glBindTexture(GL_TEXTURE_3D, 0);

    return textureHandle;
}

GLint VolumeCreator::extractIntFromCharArray(std::ifstream* pIn)
{
    GLchar value[20];
    GLchar helper = 'A';
    pIn->read(&helper, sizeof(GLchar));
    GLint i = 0;
    while(helper != ' ' && helper != '\n')
    {
        value[i] =helper;
        pIn->read(&helper, sizeof(GLchar));
        i++;
    }
    value[i] = 0;

    return atoi(value);
}

GLfloat VolumeCreator::extractFloatFromCharArray(std::ifstream* pIn)
{
    GLchar value[20];
    GLchar helper = 'A';
    pIn->read(&helper, sizeof(GLchar));
    GLint i = 0;
    while(helper != ' ' && helper != '\n')
    {
        value[i] =helper;
        pIn->read(&helper, sizeof(GLchar));
        i++;
    }
    value[i] = 0;

    return static_cast<GLfloat>(atof(value));
}
