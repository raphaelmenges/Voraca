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

#include "Slicer.h"

Slicer::Slicer() : Viewport()
{
	type = VIEWPORT_SLICER;
	volumeHandle = -1;
	direction = SLICER_DIRECTION_X;
	bar_direction = direction;
	pivot = glm::vec2(0.5f, 0.5f); 
	pivotHasChanged = GL_FALSE;
}

Slicer::~Slicer()
{

}

void Slicer::init(VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive)
{
	// Call super method
	Viewport::init(handle, title, color, posX, posY, width, height, windowWidth, windowHeight, barActive);

	// Save pointer to volumeManager
	this->pVolumeManager = pVolumeManager;

	// Configurate directions in bar
	TwEnumVal directionsEV[] = { {SLICER_DIRECTION_X, "Direction X"}, {SLICER_DIRECTION_Y, "Direction Y"}, {SLICER_DIRECTION_Z, "Direction Z"} };
	TwType directionType = TwDefineEnum("Directions", directionsEV, 3);

	// Set size of bar
	GLint barGivenSizeArray[2];
	TwGetParam(pBar, NULL, "size", TW_PARAM_INT32, 2, barGivenSizeArray);
	GLint barNewSizeArray[] = {barGivenSizeArray[0], 200};
	TwSetParam(pBar, NULL, "size", TW_PARAM_INT32, 2, barNewSizeArray);

	// Add variables to bar
	TwAddVarRW(pBar, "Active Volume", TW_TYPE_INT32, &(bar_activeVolume.value), " min=0 ");
	TwAddVarRW(pBar, "Volume Name", TW_TYPE_STDSTRING, &(bar_volumeName.value), "");

	TwAddSeparator(pBar, NULL, "");

	TwAddVarRW(pBar, "Direction", directionType, &bar_direction, NULL);
	TwAddVarRW(pBar, "Current Slice", TW_TYPE_INT32, &(bar_currentSlice.value), " min=0 ");

	// Get latest volume
	setVolumeHandle(pVolumeManager->getLatestVolumeHandle());

	// Fill bar variables
	fillBarVariables();

	// *** SLICE SHADER ***
	sliceShader.loadShaders("Slicer.vert", "Slicer.frag");
	sliceShader.setVertexBuffer(primitives::screenFillQuad, sizeof(primitives::screenFillQuad), "positionAttribute");
	sliceShaderModelHandle = sliceShader.getUniformHandle("uniformModel");
	sliceShaderViewHandle = sliceShader.getUniformHandle("uniformView");
	sliceShaderVolumeTextureHandle = sliceShader.getUniformHandle("uniformVolume");
	sliceShaderDirectionHandle = sliceShader.getUniformHandle("uniformDirection");
	slicerShaderPositionHandle = sliceShader.getUniformHandle("uniformPosition");
	slicerShaderVolumeValueInformationHandle = sliceShader.getUniformHandle("uniformVolumeValueInformation"); 

	// *** SLICE PIVOT SHADER ***
	pivotShader.loadShaders("SlicerPivot.vert", "SlicerPivot.frag");
	pivotShader.setVertexBuffer(primitives::crosshair, sizeof(primitives::crosshair), "positionAttribute");
	pivotShaderModelHandle = pivotShader.getUniformHandle("uniformModel");
	pivotShaderViewHandle = pivotShader.getUniformHandle("uniformView"); 

}

ViewportType Slicer::update(GLfloat tpf, InputData inputData, GLboolean selected)
{	
	// Call super method
	Viewport::update(tpf, inputData, selected);

	// *** CALCULATE MATRICES ***

	// Slice shader
	sliceShaderModel = glm::mat4(1.0f);

	// Pivot shader
	pivotShaderModel = glm::mat4(1.0f);
	pivotShaderModel = glm::translate(pivotShaderModel, glm::vec3((2.0f*pivot)-1.0f, SLICER_PIVOT_POS_Z));
	pivotShaderModel = glm::scale(pivotShaderModel, SLICER_PIVOT_SCALE * glm::vec3(1.0f/this->getAspectRatio(), 1, 1));

	// View matrix
	viewMatrix = glm::mat4(1.0f);

	// Direction
	if(bar_direction != direction)
	{
		direction = bar_direction;
		setCurrentSliceBarVariable();
	}

	// *** SEND PIVOT TO VOLUME ***

	// Set pivot of volume if viewport is selected
	if(bar_currentSlice.hasChanged() || pivotHasChanged)
	{
		if(volumeHandle >= 0)
		{
			glm::vec3 volumeResolution = pVolumeManager->getVolume(volumeHandle)->getVolumeResolution();
			if(direction == SLICER_DIRECTION_X)
			{
				pVolumeManager->getVolume(volumeHandle)->setPivot(glm::vec3((bar_currentSlice.getValue()/volumeResolution.x), pivot.y, pivot.x));
			}
			else if(direction == SLICER_DIRECTION_Y)
			{
				pVolumeManager->getVolume(volumeHandle)->setPivot(glm::vec3(pivot.x, (bar_currentSlice.getValue()/volumeResolution.y), pivot.y));
			}
			else
			{
				pVolumeManager->getVolume(volumeHandle)->setPivot(glm::vec3(pivot, (bar_currentSlice.getValue()/volumeResolution.z)));
			}
		}

		pivotHasChanged = GL_FALSE;
	}

	return bar_type;
}

void Slicer::draw()
{
	// Call super method to set viewport
	Viewport::draw();

	// *** SLICE SHADER ***
	if(volumeHandle >= 0)
	{
		glm::vec3 volumeResolution = pVolumeManager->getVolume(volumeHandle)->getVolumeResolution(); 
		GLfloat currentPosition = static_cast<GLfloat>(bar_currentSlice.getValue());

		if(volumeResolution.x > 1 && volumeResolution.y > 1 && volumeResolution.z > 1)
		{
			if(direction == SLICER_DIRECTION_X)
			{
				currentPosition = currentPosition/(volumeResolution.x-1);
			}
			else if(direction == SLICER_DIRECTION_Y)
			{
				currentPosition = currentPosition/(volumeResolution.y-1);
			}
			else
			{
				currentPosition = currentPosition/(volumeResolution.z-1);
			}
		}

		VolumeProperties properties = pVolumeManager->getVolume(volumeHandle)->getProperties();

		sliceShader.use();
		sliceShader.setUniformValue(sliceShaderModelHandle, sliceShaderModel);
		sliceShader.setUniformValue(sliceShaderViewHandle, viewMatrix);
		sliceShader.setUniformTexture(sliceShaderVolumeTextureHandle, pVolumeManager->getVolume(volumeHandle)->getTextureHandle(), GL_TEXTURE_3D);
		sliceShader.setUniformValue(sliceShaderDirectionHandle, direction);
		sliceShader.setUniformValue(slicerShaderPositionHandle, currentPosition);
		sliceShader.setUniformValue(slicerShaderVolumeValueInformationHandle, glm::vec2(properties.valueOffset, properties.valueScale));
		sliceShader.draw(GL_TRIANGLES);
	}

	// *** PIVOT SHADER ***
	pivotShader.use();
	pivotShader.setUniformValue(pivotShaderModelHandle, pivotShaderModel);
	pivotShader.setUniformValue(pivotShaderViewHandle, viewMatrix);
	pivotShader.draw(GL_TRIANGLES);
}

void Slicer::setVolumeHandle(GLint handle)
{
	if(handle >= 0)
	{
		if(pVolumeManager->getVolume(handle) != NULL)
		{
			volumeHandle = handle;
		}
		bar_activeVolume.setValue(volumeHandle);
		setCurrentSliceBarVariable();
	}
}

void Slicer::handleInput(InputData inputData)
{
	// Call super method to set viewport
	Viewport::handleInput(inputData);

	// Position of pivot
	if(inputData.mouse_drag_left)
	{
		pivot = relativeCursorPos;
		pivotHasChanged = GL_TRUE;
	}

	// Current slice
	if(inputData.scroll_changed)
	{
		bar_currentSlice.setValue(static_cast<GLuint>(static_cast<GLfloat>(bar_currentSlice.getValue()) + inputData.scroll.y));
		clampCurrentSliceBarVariable();
		pivotHasChanged = GL_TRUE;
	}
}

void Slicer::updateBarVariables()
{
	// Call super method
	Viewport::updateBarVariables();

	// Call update methods
	bar_activeVolume.update();
	bar_volumeName.update();
	bar_currentSlice.update();
}

void Slicer::useBarVariables()
{
	// Call super method
	Viewport::useBarVariables();

	// Update active volume if necessary
	if(bar_activeVolume.hasChanged())
	{
		if(bar_activeVolume.getValue() < 0)
		{
			bar_activeVolume.setValue(0);
		}
		else
		{
			setVolumeHandle(bar_activeVolume.getValue());
			bar_activeVolume.setValue(volumeHandle);
		}
	}

	// Update volumes's name
	if(bar_volumeName.hasChanged())
	{
		pVolumeManager->getVolume(volumeHandle)->rename(bar_volumeName.getValue());
	}
}

void Slicer::fillBarVariables()
{
	// Call super method
	Viewport::fillBarVariables();

	// VolumeName
	if(volumeHandle >= 0)
	{
		bar_volumeName.setValue(pVolumeManager->getVolume(volumeHandle)->getName());
	}
}

void Slicer::setCurrentSliceBarVariable()
{
	if(volumeHandle >= 0)
	{
		glm::vec3 volumeResolution = pVolumeManager->getVolume(volumeHandle)->getVolumeResolution();
		GLfloat maxSlice = 0;

		if(direction == SLICER_DIRECTION_X)
		{
			maxSlice = volumeResolution.x-1;
			bar_currentSlice.setValue(static_cast<GLuint>(volumeResolution.x)/2);
		}
		else if(direction == SLICER_DIRECTION_Y)
		{
			maxSlice = volumeResolution.y-1;
			bar_currentSlice.setValue(static_cast<GLuint>(volumeResolution.y)/2);
		}
		else
		{
			maxSlice = volumeResolution.z-1;
			bar_currentSlice.setValue(static_cast<GLuint>(volumeResolution.z)/2);
		}

		maxSlice = maxSlice > 0 ? maxSlice : 0;
		TwSetParam(pBar, "Current Slice", "max", TW_PARAM_FLOAT, 1, &maxSlice);
	}
}

void Slicer::clampCurrentSliceBarVariable()
{
	if(volumeHandle >= 0)
	{
		glm::vec3 volumeResolution = pVolumeManager->getVolume(volumeHandle)->getVolumeResolution();
		GLint maxSlice = 0;

		if(direction == SLICER_DIRECTION_X)
		{
			maxSlice = static_cast<GLint>(volumeResolution.x-1);
		}
		else if(direction == SLICER_DIRECTION_Y)
		{
			maxSlice = static_cast<GLint>(volumeResolution.y-1);
		}
		else
		{
			maxSlice = static_cast<GLint>(volumeResolution.z-1);
		}

		maxSlice = maxSlice > 0 ? maxSlice : 0;
		if(bar_currentSlice.getValue() >= 0)
		{
			bar_currentSlice.setValue(bar_currentSlice.getValue() % maxSlice);
		}
		else
		{
			bar_currentSlice.setValue(maxSlice);
		}
	}
}

