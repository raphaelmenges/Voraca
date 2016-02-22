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
 * Slicer
 *--------------
 * Subclass of viewport.
 * Let one scroll through the volume data.
 *
 */

#ifndef SLICER_H_
#define SLICER_H_

#include "Viewport.h"
#include "Shader.h"
#include "Transferfunction.h"
#include "TfManager.h"
#include "VolumeManager.h"
#include "Primitives.h"

const GLfloat SLICER_PIVOT_POS_Z = -0.1f;
const GLfloat SLICER_PIVOT_SCALE = 0.1f;

enum SlicerDirection
{
	SLICER_DIRECTION_X, SLICER_DIRECTION_Y, SLICER_DIRECTION_Z
};

/** Slicer class */
class Slicer : public Viewport
{
public:
	Slicer();
	virtual ~Slicer();

	/** Viewport methods */
	void init(VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive);
	ViewportType update(GLfloat tpf, InputData inputData, GLboolean selected);
	void draw();

	/** Set active volume */
	void setVolumeHandle(GLint handle);
	
protected:
	/** Viewport method, too */
	virtual void handleInput(InputData inputData);

	/** Updaet bar variables */
	virtual void updateBarVariables();

	/** Use bar vars */
	virtual void useBarVariables();

	/** Fill bar variables with new data (done before drawing)*/
	virtual void fillBarVariables();

	/** Reset slicer when setting new volume */
	void setCurrentSliceBarVariable();

	/** Clamping via bar is not enough */
	void clampCurrentSliceBarVariable();

	/** Direction of slicing */
	SlicerDirection direction;
	SlicerDirection bar_direction;

	/** Position of pivot */
	glm::vec2 pivot;
	GLboolean pivotHasChanged;

	/** Handle to volume */
	GLint volumeHandle;

	/** Pointer to volumeManager of editor */
	VolumeManager* pVolumeManager;

	/** View matrix for all draws inside this viewport */
	glm::mat4 viewMatrix;

	/** Shader for visualization of slice */
	Shader sliceShader;
	GLuint sliceShaderModelHandle;
	GLuint sliceShaderViewHandle;
	GLuint sliceShaderVolumeTextureHandle;
	GLuint sliceShaderDirectionHandle;
	GLuint slicerShaderPositionHandle;
	GLuint slicerShaderVolumeValueInformationHandle;
	glm::mat4 sliceShaderModel;

	/** Shader for pivot */
	Shader pivotShader;
	GLuint pivotShaderModelHandle;
	GLuint pivotShaderViewHandle;
	glm::mat4 pivotShaderModel;

	/** Bar variables */
	BarVariable<GLint> bar_activeVolume;
	BarVariable<std::string> bar_volumeName;
	BarVariable<GLint> bar_currentSlice;
};
 
#endif