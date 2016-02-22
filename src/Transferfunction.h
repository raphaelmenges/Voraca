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
 * Transferfunction
 *--------------
 * Manages TfPoints and creates
 * transferfunction textures from them.
 * Preintegration update is called by TfEditor
 * because this class has no update method and objects
 * can be manipulated by multiple TfEditors.
 *
 */

#ifndef TRANSFERFUNCTION_H_
#define TRANSFERFUNCTION_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <vector>
#include <algorithm>
#include <set>

#include "Logger.h"
#include "Shader.h"
#include "TfPoint.h"
#include "Primitives.h"

const GLuint TRANSFERFUNCTION_TFPOINT_BORDER_VERTEX_COUNT = 32;
const GLuint TRANSFERFUNCTION_VISUALIZATION_STEPS = 1000;
const GLuint TRANSFERFUNCTION_TEXTURES_RES = 256;
const GLuint TRANSFERFUNCTION_BEZIER_STEPS = 1024;
const GLuint TRANSFERFUNCTION_BEZIER_MIN_STEPS_BETWEEN_TFPOINTS = 64;
const GLfloat TRANSFERFUNCTION_VISUALIZATION_POS_Z = -0.2f;
const GLboolean TRANSFERFUNCTION_CLAMP_CONTROL_POINTS = GL_FALSE;
const GLfloat TRANSFERFUNCTION_X_MIN = 0;
const GLfloat TRANSFERFUNCTION_X_MAX = 1;
const GLfloat TRANSFERFUNCTION_Y_MIN = 0;
const GLfloat TRANSFERFUNCTION_Y_MAX = 1;
const GLboolean TRANSFERFUNCTION_TEXTURES_LINEAR_FILTERING = GL_TRUE;
const GLfloat TRANSFERFUNCTION_VISUALIZATION_X_OVERLAPPING = 0.0001f;
const GLfloat TRANSFERFUNCTION_TFPOINT_DUPLICATE_OFFSET = 0.01f;

enum TfVisualization
{
    TF_VISUALIZATION_COLOR, TF_VISUALIZATION_AMBIENT_MULTIPLIER,
    TF_VISUALIZATION_SPECULAR_MULTIPLIER,TF_VISUALIZATION_SPECULAR_SATURATION,
    TF_VISUALIZATION_SPECULAR_POWER, TF_VISUALIZATION_GRADIENT_ALPHA_MULTIPLIER,
    TF_VISUALIZATION_FRESNEL_ALPHA_MULTIPLIER, TF_VISUALIZATION_RELECTION_COLOR_MULTIPLIER,
    TF_VISUALIZATION_EMISSION_COLOR_MULTIPLIER
};

class Transferfunction
{
public:
    Transferfunction();
    ~Transferfunction();

    /** Friends of this class */
    friend class TfCreator;

    /** Initialization */
    void init(GLint handle, std::string name);

    /** Drawing */
    void draw(TfVisualization visualization, GLfloat functionOpacity, GLint activeTfPoint, std::set<GLint> selectedTfPoints, GLboolean locked, GLfloat scale, GLfloat aspectRatio, glm::mat4 viewMatrix);

    /** Selection and intersection in local coordinates*/
    GLint selectTfPoint(glm::vec2 coord, GLfloat scale, GLfloat, glm::vec2 &offset);
    GLboolean intersectLeftControlPoint(GLint handle, glm::vec2 coord, GLfloat scale, GLfloat aspectRatio, glm::vec2 &offset);
    GLboolean intersectRightControlPoint(GLint handle, glm::vec2 coord, GLfloat scale, GLfloat aspectRatio, glm::vec2 &offset);

    /** Test if coordinates are inside  borders */
    GLboolean insideBorders(glm::vec2 coord);

    /** Move selected point or its control points */
    GLboolean setTfPoint(GLint handle, glm::vec2 coord);
    GLboolean moveTfPoint(GLint handle, glm::vec2 delta);
    void setLeftControlPoint(GLint handle, glm::vec2 coord);
    void setRightControlPoint(GLint handle, glm::vec2 coord);

    /** Simple add tfPoint method, may not be called by this class */
    GLint addTfPoint(glm::vec2 coord);

    /** Delete tfPoint */
    GLboolean deleteTfPoint(GLint handle);

    /** Duplicate tfPoint */
    GLint duplicateTfPoint(GLint handle);

    /** Control TfPoints */
    TfPointValue getValueOfTfPoint(GLint handle);
    void setValueOfTfPoint(GLint handle, TfPointValue value);
    glm::vec2 getPositionOfTfPoint(GLint handle);
    GLboolean toggleTfPointControlPointsLinked(GLint handle);

    /** Preintegration should be updated as fast as possible */
    void preintegrationShouldBeUpdate();

    /** Getter for textures */
    GLuint getColorAlphaFunctionHandle() const;
    GLuint getAmbientSpecularFunctionHandle() const;
    GLuint getAdvancedFunctionHandle() const;

    /** Getter for texture handles of preintegrated transferfunctions */
    GLuint getColorAlphaPreintegrationHandle() const;
    GLuint getAmbientSpecularPreintegrationHandle() const;
    GLuint getAdvancedPreintegrationHandle() const;

    /** Get texture resolution (let's assume all textures have the same) */
    GLuint getTextureResolution() const;

    /** Returns handle */
    GLint getHandle() const;

    /** Returns row of handles between two handles */
    std::set<GLint> getRowOfHandles(GLint startHandle, GLint endHandle);

    /** Returns name */
    std::string getName() const;

    /** Renaming */
    void rename(std::string name);

protected:
    /** Internal add method. Dangerous! */
    TfPoint* internalAddTfPoint(TfPointLocation location, glm::vec2 coord);

    /** Method for creation of the function */
    void updateFunction();

    /** Update preintegration table */
    void updatePreintegation();

    /** Helps the preintegration update function */
    void updatePreintegrationHelper(std::vector<glm::vec4>& function, GLuint& textureHandle);

    /** Searches for a pointer to a object with that handle */
    TfPoint* getTfPointByHandle(GLint handle);

    /** Reorder vector */
    void reorderTfPoints();

    /** Some nasty OpenGL stuff */
    void textureInitialization(GLuint& textureHandle, GLenum mode);

    /** Delete texture */
    void deleteTexture(GLuint textureHandle);

    /** Point clamping */
    GLboolean clampTfPointPosition(TfPoint* pTfPoint);
    GLboolean clampLeftControlPointPosition(TfPoint* pTfPoint);
    GLboolean clampRightControlPointPosition(TfPoint* pTfPoint);

    /** Some basics */
    GLint handle;
    std::string name;
    GLint tfPointHandleCounter;
    GLboolean functionShouldBeUpdated;
    GLboolean preintegrationShouldBeUpdated;
    std::vector<TfPoint> tfPoints;
    GLboolean overwriteProtected;

    /** Generated textures from functions */
    GLuint colorAlphaFunctionHandle;
    GLuint ambientSpecularFunctionHandle;
    GLuint advancedFunctionHandle;

    /** Same as textures put in a std::vector */
    std::vector<glm::vec4> colorAlphaFunction;
    std::vector<glm::vec4> ambientSpecularFunction;
    std::vector<glm::vec4> advancedFunction;

    /** Preintegration stuff */
    GLuint colorAlphaPreintegrationHandle;
    GLuint ambientSpecularPreintegrationHandle;
    GLuint advancedPreintegrationHandle;

    /* Own shader to visualize colorAlphaFunction behind tfPoints */
    Shader functionShader;

    /* Its uniform handles */
    GLuint functionShaderModelHandle;
    GLuint functionShaderViewHandle;
    GLuint functionShaderColorAlphaFunctionHandle;
    GLuint functionShaderAmbientSpecularFunctionHandle;
    GLuint functionShaderAdvancedFunctionHandle;
    GLuint functionShaderVisualizationStateHandle;
    GLuint functionShaderOpacityHandle;
    GLuint functionShaderValueRangeHandle;

    /* Shaders which are given to the tfPoints as handles */
    Shader pointShader;
    Shader controlPointShader;
    Shader controlLineShader;

    /* Uniform handles of the shaders */
    GLuint pointShaderModelHandle;
    GLuint pointShaderViewHandle;
    GLuint pointShaderInnerColorHandle;
    GLuint pointShaderBorderColorHandle;
    GLuint controlPointShaderModelHandle;
    GLuint controlPointShaderViewHandle;
    GLuint controlLineShaderModelHandle;
    GLuint controlLineShaderViewHandle;
    GLuint controlLineShaderColorHandle;
};
#endif
