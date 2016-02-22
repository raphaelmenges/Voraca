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
 * TfEditor
 *--------------
 * Subclass of viewport.
 * Let one manipulate transferfunctions.
 *
 */

#ifndef TFEDITOR_H_
#define TFEDITOR_H_

#include <set>

#include "Viewport.h"
#include "Shader.h"
#include "Transferfunction.h"
#include "TfManager.h"
#include "Primitives.h"
#include "VolumeManager.h"

const GLfloat TFEDITOR_CAMERA_MOVE_SPEED = 1.0f;
const GLfloat TFEDITOR_CAMERA_ZOOM_SPEED = 0.3f;
const GLfloat TFEDITOR_TFPOINT_SCALE_ZOOM_INFLUENCE_MULTIPLIER = 2.0f;
const GLfloat TFEDITOR_CAMERA_ZOOM_MAX = 20.0f;
const GLfloat TFEDITOR_CAMERA_ZOOM_MIN = 0.5f;
const glm::vec2 TFEDITOR_CAMERA_POSITION(0.5f, 0.5f);
const GLfloat TFEDITOR_CAMERA_ZOOM = 2.0f;
const GLfloat TFEDITOR_TFPOINT_DRAW_SCALE = 1.0f;
const GLfloat TFEDITOR_TFPOINT_DRAW_SCALE_MAX = 1.0f;
const GLfloat TFEDITOR_BAR_FLOAT_STEP_A = 0.01f;
const GLfloat TFEDITOR_BAR_FLOAT_STEP_B = 0.5f;
const GLfloat TFEDITOR_PREINTEGRATION_UPDATE_INTERVALL = 0.1f;
const GLfloat TFEDITOR_HISTOGRAM_POSZ = -0.1f;
const GLfloat TFEDITOR_PIVOT_POSZ = -0.5f;
const GLfloat TFEDITOR_PIVOT_SCALE = 0.01f;
const GLfloat TFEDITOR_TF_OPACITY = 0.8f;

/** TfEditor class */
class TfEditor : public Viewport
{
public:
	TfEditor();
	virtual ~TfEditor();

	/** Viewport methods */
	void init(TfManager* pTfManager, VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive);
	ViewportType update(GLfloat tpf, InputData inputData, GLboolean selected);
	void draw();

	/** Set active transferfunction */
	void setTfHandle(GLint handle);

	/** Set active volume */
	void setVolumeHandle(GLint handle);

	/** Methods for bar buttons */
	void newTf();
	void reloadTf();
	void saveTf();
	void loadTf();
	void resetCamera();
	void toggleLockTfPoints();
	void assignActiveValueToSelectedTfPoints();
	void toggleLinkControlPoints();
	void setTfVisualization(TfVisualization tfVisualization);
	void resetActiveTfPointValue();
	
protected:
	/** Viewport method, too */
	virtual void handleInput(InputData inputData);

	/** Updaet bar variables */
	virtual void updateBarVariables();

	/** Use bar vars */
	virtual void useBarVariables();

	/** Fill bar variables with new data (done before drawing)*/
	virtual void fillBarVariables();

	/** Controlling transferfuncton */
	void selectTfPoint(glm::vec2 coord, GLboolean multiSelection, GLboolean rowSelection);
	void moveTfPoint(glm::vec2 coord);
	void addTfPoint(glm::vec2 coord, GLboolean multiSelection);
	void deleteTfPoint();
	void duplicateSelectedTfPoints();
	void unselectTfPoints();

	/** Handle of transferfunction */
	GLint tfHandle;

	/** Handle to volume */
	GLint volumeHandle;

	/** Handle to a point of current transferfunction */
	std::set<GLint> selectedTfPoints;
	GLint activeTfPointHandle;
	GLboolean leftControlHandleSelected;
	GLboolean rightControlHandleSelected;
	GLboolean tfPointsLocked;

	/** Current visualization */
	TfVisualization tfVisualization;

	/** Current selection offset */
	glm::vec2 selectionOffset;
	
	/** Draw scale of tfPoints */
	GLfloat tfPointDrawScale;

	/** Preintegration helpers */
	GLfloat timeUntilUpdateOfPreintegration;
	GLboolean preintegrationShouldBeUpdated;

	/** Pointer to tfMananger of editor */
	TfManager* pTfManager;

	/** Pointer to volumeManager of editor */
	VolumeManager* pVolumeManager;

	/** Camera */
	glm::vec2 cameraPosition;
	GLfloat cameraZoom;
	glm::vec2 prevCameraMovementCoord;

	/** View matrix for all draws inside this viewport */
	glm::mat4 viewMatrix;

	/** Background */
	Shader backgroundShader;
	GLuint backgroundShaderModelHandle;
	GLuint backgroundShaderViewHandle;
	glm::mat4 backgroundShaderModelMatrix;

	/** Histogram */
	Shader histogramShader;
	GLuint histogramShaderModelHandle;
	GLuint histogramShaderViewHandle;
	GLuint histogramShaderTextureHandle;
	glm::mat4 histogramShaderModel;

	/** Pivot */
	Shader pivotShader;
	GLuint pivotShaderModelHandle;
	GLuint pivotShaderViewHandle;
	glm::mat4 pivotShaderModel;

	/** Non template variables for bar */
	GLfloat bar_tfFunctionOpacity;
	std::string bar_pathToExternTf;
	GLboolean bar_overwriteExisting;
	GLboolean bar_showPivot;

	/** Bar variables */
	BarVariable<glm::vec3> bar_tfPointColor;
	BarVariable<GLfloat> bar_tfPointAmbientMultiplier;
	BarVariable<GLfloat> bar_tfPointSpecularMultiplier;
	BarVariable<GLfloat> bar_tfPointSpecularSaturation;
	BarVariable<GLfloat> bar_tfPointSpecularPower;
	BarVariable<GLfloat> bar_tfPointGradientAlphaMultiplier;
	BarVariable<GLfloat> bar_tfPointFresnelAlphaMultiplier;
	BarVariable<GLfloat> bar_tfPointReflectionColorMultiplier;
	BarVariable<GLfloat> bar_tfPointEmissionColorMultiplier;
	BarVariable<std::string> bar_tfName;
	BarVariable<GLint> bar_activeTf;
	BarVariable<GLint> bar_activeVolume;
	BarVariable<std::string> bar_volumeName;
};

/** Callbacks bar buttons */
static void TW_CALL newTfButtonCallback(void* clientData);
static void TW_CALL reloadTfButtonCallback(void* clientData);
static void TW_CALL saveTfButtonCallback(void* clientData);
static void TW_CALL loadTfButtonCallback(void* clientData);
static void TW_CALL resetCameraButtonCallback(void* clientData);
static void TW_CALL toggleLockTfPointsButtonCallback(void* clientData);
static void TW_CALL assignActiveValueToSelectedTfPointsButtonCallback(void* clientData);
static void TW_CALL toggleLinkControlPointsButtonCallback(void* clientData);
static void TW_CALL setColorVisualizationButtonCallback(void* clientData);
static void TW_CALL setAmbientMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL setSpecularMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL setSpecularSaturationVisualizationButtonCallback(void* clientData);
static void TW_CALL setSpecularPowerVisualizationButtonCallback(void* clientData);
static void TW_CALL setGradientAlphaMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL setFresnelAlphaMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL setReflectionColorMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL setEmissiveColorMultiplierVisualizationButtonCallback(void* clientData);
static void TW_CALL resetTfPointValueButtonCallback(void* clientData);
 
#endif