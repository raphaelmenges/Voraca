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

#include "TfEditor.h"

TfEditor::TfEditor() : Viewport()
{
	type = VIEWPORT_TFEDITOR;
	tfHandle = -1;
	leftControlHandleSelected = GL_FALSE;
	rightControlHandleSelected = GL_FALSE;
	selectionOffset = glm::vec2(0,0);
	tfPointDrawScale = TFEDITOR_TFPOINT_DRAW_SCALE;
	cameraPosition = TFEDITOR_CAMERA_POSITION;
	cameraZoom = TFEDITOR_CAMERA_ZOOM;
	prevCameraMovementCoord = glm::vec2(0,0);
	activeTfPointHandle = -1;
	tfPointsLocked = GL_FALSE;
	tfVisualization = TF_VISUALIZATION_COLOR;
	preintegrationShouldBeUpdated = GL_FALSE;
	timeUntilUpdateOfPreintegration = TFEDITOR_PREINTEGRATION_UPDATE_INTERVALL;
	bar_tfFunctionOpacity = TFEDITOR_TF_OPACITY;
	bar_overwriteExisting = GL_FALSE;
	bar_showPivot = GL_TRUE;
}

TfEditor::~TfEditor()
{

}

void TfEditor::init(TfManager* pTfManager, VolumeManager* pVolumeManager, GLint handle, glm::vec3 color, std::string title, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLint windowWidth, GLint windowHeight, GLboolean barActive)
{
	// Call super method
	Viewport::init(handle, title, color, posX, posY, width, height, windowWidth, windowHeight, barActive);

	// Save pointer to tfManager
	this->pTfManager = pTfManager;

	// Save pointer to volumeManager
	this->pVolumeManager = pVolumeManager;

	// Add variables to bar
	TwAddVarRW(pBar, "Active Tf", TW_TYPE_INT32, &(bar_activeTf.value), " min=0 ");
	TwAddVarRW(pBar, "Tf Name", TW_TYPE_STDSTRING, &(bar_tfName.value), "");
	
	TwAddSeparator(pBar, NULL, "");

	TwAddVarRW(pBar, "Active Volume", TW_TYPE_INT32, &(bar_activeVolume.value), " min=0 ");
	TwAddVarRW(pBar, "Volume Name", TW_TYPE_STDSTRING, &(bar_volumeName.value), "");
	
	TwAddSeparator(pBar, NULL, "");
	
	TwAddVarRW(pBar, "Color", TW_TYPE_COLOR3F, &(bar_tfPointColor.value), " colormode=rgb group='TfPoint' ");
	TwAddVarRW(pBar, "Ambient Multiplier", TW_TYPE_FLOAT, &(bar_tfPointAmbientMultiplier.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Specular Multiplier", TW_TYPE_FLOAT, &(bar_tfPointSpecularMultiplier.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Specular Saturation", TW_TYPE_FLOAT, &(bar_tfPointSpecularSaturation.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Specular Power", TW_TYPE_FLOAT, &(bar_tfPointSpecularPower.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Gradient Alpha Multiplier", TW_TYPE_FLOAT, &(bar_tfPointGradientAlphaMultiplier.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Fresnel Alpha Multiplier", TW_TYPE_FLOAT, &(bar_tfPointFresnelAlphaMultiplier.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Reflection Color Multiplier", TW_TYPE_FLOAT, &(bar_tfPointReflectionColorMultiplier.value), " group='TfPoint' ");
	TwAddVarRW(pBar, "Emission Color Multiplier", TW_TYPE_FLOAT, &(bar_tfPointEmissionColorMultiplier.value), " group='TfPoint' ");

	TwAddButton(pBar, "(Un)Link Control Points", toggleLinkControlPointsButtonCallback, this, " group='TfPoint' ");
	TwAddButton(pBar, "Assign Active's Value", assignActiveValueToSelectedTfPointsButtonCallback, this, " group='TfPoint' ");
	TwAddButton(pBar, "Reset Active's Value", resetTfPointValueButtonCallback, this, " group=TfPoint ");
	TwAddButton(pBar, "(Un)Lock TfPoints", toggleLockTfPointsButtonCallback, this, "");
	
	TwAddSeparator(pBar, NULL, "");

	TwAddButton(pBar, "Show Color", setColorVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Ambient Multiplier", setAmbientMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Specular Multiplier", setSpecularMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Specular Saturation", setSpecularSaturationVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Specular Power", setSpecularPowerVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Gradient Alpha Multiplier", setGradientAlphaMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Fresnel Alpha Multiplier", setFresnelAlphaMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Reflection Color Multiplier", setReflectionColorMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");
	TwAddButton(pBar, "Show Emission Color Multiplier", setEmissiveColorMultiplierVisualizationButtonCallback, this, " group='Tf Visualization' ");

	TwAddSeparator(pBar, NULL, "");

	TwAddButton(pBar, "New", newTfButtonCallback, this, " group='Tf Management' ");
	TwAddButton(pBar, "Reload", reloadTfButtonCallback, this, " group='Tf Management' ");
	TwAddButton(pBar, "Save", saveTfButtonCallback, this, " group='Tf Management' ");
	TwAddVarRW(pBar, "Tf To Load", TW_TYPE_STDSTRING, &bar_pathToExternTf, " group='Tf Management' ");
	TwAddButton(pBar, "Load", loadTfButtonCallback, this, " group='Tf Management' ");
	TwAddVarRW(pBar, "Overwrite Existing", TW_TYPE_BOOLCPP, &bar_overwriteExisting, " group='Tf Management' ");
	
	TwAddSeparator(pBar, NULL, "");

	TwAddVarRW(pBar, "Show pivot", TW_TYPE_BOOLCPP, &bar_showPivot, "");
	TwAddVarRW(pBar, "Tf Function Opacity", TW_TYPE_FLOAT, &bar_tfFunctionOpacity, " min=0 max=1 step=0.1 ");
	TwAddButton(pBar, "Reset Camera", resetCameraButtonCallback, this, "");

	// Bar step width
	TwSetParam(pBar, "Ambient Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Specular Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Specular Saturation", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Specular Power", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_B);
	TwSetParam(pBar, "Gradient Alpha Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Fresnel Alpha Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Reflection Color Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);
	TwSetParam(pBar, "Emission Color Multiplier", "step", TW_PARAM_FLOAT, 1, &TFEDITOR_BAR_FLOAT_STEP_A);

	// Set min/max
	TwSetParam(pBar, "Ambient Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_AMBIENT_MULTIPLIER_MIN);
	TwSetParam(pBar, "Ambient Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_AMBIENT_MULTIPLIER_MAX);
	TwSetParam(pBar, "Specular Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_MULTIPLIER_MIN);
	TwSetParam(pBar, "Specular Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_MULTIPLIER_MAX);
	TwSetParam(pBar, "Specular Saturation", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_SATURATION_MIN);
	TwSetParam(pBar, "Specular Saturation", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_SATURATION_MAX);
	TwSetParam(pBar, "Specular Power", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_POWER_MIN);
	TwSetParam(pBar, "Specular Power", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_SPECULAR_POWER_MAX);
	TwSetParam(pBar, "Gradient Alpha Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MIN);
	TwSetParam(pBar, "Gradient Alpha Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER_MAX);
	TwSetParam(pBar, "Fresnel Alpha Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MIN);
	TwSetParam(pBar, "Fresnel Alpha Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER_MAX);
	TwSetParam(pBar, "Reflection Color Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MIN);
	TwSetParam(pBar, "Reflection Color Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER_MAX);
	TwSetParam(pBar, "Emission Color Multiplier", "min", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MIN);
	TwSetParam(pBar, "Emission Color Multiplier", "max", TW_PARAM_FLOAT, 1, &TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER_MAX);

	// Fold groups
	GLint opened = 0;
	TwSetParam(pBar, "Tf Visualization", "opened", TW_PARAM_INT32, 1, &opened);
	TwSetParam(pBar, "Tf Management", "opened", TW_PARAM_INT32, 1, &opened);

	// Get latest transferfunction (must be after bar initialization!)
	setTfHandle(pTfManager->getLatestTfHandle());

	// Get latest volume
	setVolumeHandle(pVolumeManager->getLatestVolumeHandle());

	// Fill bar variables
	fillBarVariables();

	// *** BACKGROUND SHADER ***
	backgroundShader.loadShaders("TfEditorBackground.vert", "TfEditorBackground.frag");
	backgroundShader.setVertexBuffer(primitives::quad, sizeof(primitives::quad), "positionAttribute");
	backgroundShaderModelHandle = backgroundShader.getUniformHandle("uniformModel");
	backgroundShaderViewHandle = backgroundShader.getUniformHandle("uniformView");

	// *** HISTOGRAM SHADER *** 
	histogramShader.loadShaders("Histogram.vert", "Histogram.frag");

	// Set vertex data
	std::vector<glm::vec3> histogramShaderVertices;
	GLfloat histogramShaderVerticesStepSize = 1.0f / (2*VOLUME_HISTOGRAMM_BUCKET_COUNT);
	for(GLuint i = 1; i <= 2*VOLUME_HISTOGRAMM_BUCKET_COUNT; i++)
	{
		GLfloat lastX = (i-1) * histogramShaderVerticesStepSize;
		GLfloat currentX = lastX + histogramShaderVerticesStepSize;

		glm::vec3 a = glm::vec3(currentX, 1, 0);
		glm::vec3 b = glm::vec3(lastX, 1, 0);
		glm::vec3 c = glm::vec3(lastX, 0, 0);
		glm::vec3 d = glm::vec3(currentX, 0, 0);

		histogramShaderVertices.push_back(a);
		histogramShaderVertices.push_back(b);
		histogramShaderVertices.push_back(c);
		histogramShaderVertices.push_back(c);
		histogramShaderVertices.push_back(d);
		histogramShaderVertices.push_back(a);
	}
	histogramShader.setVertexBuffer(reinterpret_cast<GLfloat*> (&(histogramShaderVertices[0])), 3*(int)histogramShaderVertices.size()*sizeof(GLfloat), "positionAttribute");

	histogramShaderModelHandle = histogramShader.getUniformHandle("uniformModel");
	histogramShaderViewHandle = histogramShader.getUniformHandle("uniformView");
	histogramShaderTextureHandle = histogramShader.getUniformHandle("uniformHistogram");

	// *** PIVOT SHADER *** 
	pivotShader.loadShaders("SlicerPivot.vert", "SlicerPivot.frag");
	pivotShader.setVertexBuffer(primitives::quad, sizeof(primitives::quad), "positionAttribute");
	pivotShaderModelHandle = pivotShader.getUniformHandle("uniformModel");
	pivotShaderViewHandle = pivotShader.getUniformHandle("uniformView"); 
}

ViewportType TfEditor::update(GLfloat tpf, InputData inputData, GLboolean selected)
{	
	// Call super method
	Viewport::update(tpf, inputData, selected);

	// *** PREINTEGRATION OF TRANSFERFUNCTION ***

	// Update preintegration if necessary
	if(preintegrationShouldBeUpdated)
	{
		timeUntilUpdateOfPreintegration -= tpf;
		if(timeUntilUpdateOfPreintegration < 0)
		{
			// Update preintegration
			pTfManager->getTf(tfHandle)->preintegrationShouldBeUpdate();

			// Reset values
			preintegrationShouldBeUpdated = GL_FALSE;
			timeUntilUpdateOfPreintegration = TFEDITOR_PREINTEGRATION_UPDATE_INTERVALL;
		}
	}

	// *** CALCULATE MATRICES ***

	// Scale tfPoints
	tfPointDrawScale = glm::min(TFEDITOR_TFPOINT_DRAW_SCALE *(TFEDITOR_TFPOINT_SCALE_ZOOM_INFLUENCE_MULTIPLIER/cameraZoom), TFEDITOR_TFPOINT_DRAW_SCALE_MAX);

	// Calculate model matrix for background shader
	backgroundShaderModelMatrix = glm::mat4(1.0f);

	if(volumeHandle >= 0)
	{
		// Calculate model matrix for histogram shader
		histogramShaderModel = glm::mat4(1.0f);
		histogramShaderModel = glm::translate(histogramShaderModel, glm::vec3(pVolumeManager->getVolume(volumeHandle)->getProperties().valueOffset, 0, TFEDITOR_HISTOGRAM_POSZ));
		histogramShaderModel = glm::scale(histogramShaderModel, glm::vec3(pVolumeManager->getVolume(volumeHandle)->getProperties().valueScale, 1, 1));

		// Calculate model matrix for pivot shader
		GLfloat pivotScale = TFEDITOR_PIVOT_SCALE/this->getAspectRatio();
		pivotShaderModel = glm::mat4(1.0f);
		pivotShaderModel = glm::translate(pivotShaderModel, 
			glm::vec3(pVolumeManager->getVolume(volumeHandle)->getProperties().valueOffset 
			+ (pVolumeManager->getVolume(volumeHandle)->getPivot() * (1.0f-pivotScale)) 
			* pVolumeManager->getVolume(volumeHandle)->getProperties().valueScale, 0, TFEDITOR_PIVOT_POSZ));
		pivotShaderModel = glm::scale(pivotShaderModel, glm::vec3(pivotScale, 1, 1));
	}

	// Calculate view matrix
	viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::scale(viewMatrix, glm::vec3(cameraZoom, cameraZoom, 1));
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-cameraPosition, 0));

	return bar_type;
}

void TfEditor::draw()
{
	// Call super method to set viewport
	Viewport::draw();

	// *** BACKGROUND SHADER ***
	backgroundShader.use();
	backgroundShader.setUniformValue(backgroundShaderModelHandle, backgroundShaderModelMatrix);
	backgroundShader.setUniformValue(backgroundShaderViewHandle, viewMatrix);
	backgroundShader.draw(GL_TRIANGLES);

	if(volumeHandle >= 0)
	{
		// *** HISTOGRAM SHADER ***
		histogramShader.use();
		histogramShader.setUniformValue(histogramShaderModelHandle, histogramShaderModel);
		histogramShader.setUniformValue(histogramShaderViewHandle, viewMatrix);
		histogramShader.setUniformTexture(histogramShaderTextureHandle, pVolumeManager->getVolume(volumeHandle)->getHistogramTextureHandle(), GL_TEXTURE_1D);
		histogramShader.draw(GL_TRIANGLES);

		// *** PIVOT SHADER ***
		if(bar_showPivot)
		{
			pivotShader.use();
			pivotShader.setUniformValue(pivotShaderModelHandle, pivotShaderModel);
			pivotShader.setUniformValue(pivotShaderViewHandle, viewMatrix);
			pivotShader.draw(GL_TRIANGLES);
		}
	}

	// Draw transferfunction into viewport
	pTfManager->getTf(tfHandle)->draw(this->tfVisualization, bar_tfFunctionOpacity, activeTfPointHandle, selectedTfPoints, tfPointsLocked, tfPointDrawScale, this->getAspectRatio(), viewMatrix);
}

void TfEditor::setTfHandle(GLint handle)
{
	if(handle >= 0)
	{
		if(pTfManager->getTf(handle) != NULL)
		{
			tfHandle = handle;
			unselectTfPoints();
		}
		bar_activeTf.setValue(tfHandle);
	}
}

void TfEditor::setVolumeHandle(GLint handle)
{
	if(handle >= 0)
	{
		if(pVolumeManager->getVolume(handle) != NULL)
		{
			volumeHandle = handle;
		}
		bar_activeVolume.setValue(volumeHandle);
	}
}

void TfEditor::newTf()
{
	setTfHandle(pTfManager->newTf());
}

void TfEditor::reloadTf()
{
	pTfManager->reloadTf(tfHandle);
	setTfHandle(tfHandle);
}

void TfEditor::saveTf()
{
	pTfManager->saveTf(tfHandle, bar_overwriteExisting);
}

void TfEditor::loadTf()
{
	setTfHandle(pTfManager->loadTf(bar_pathToExternTf));
}

void TfEditor::resetCamera()
{
	cameraPosition = TFEDITOR_CAMERA_POSITION;
	cameraZoom = TFEDITOR_CAMERA_ZOOM;
}

void TfEditor::toggleLockTfPoints()
{
	tfPointsLocked = !tfPointsLocked;
	preintegrationShouldBeUpdated = GL_TRUE;
}

void TfEditor::assignActiveValueToSelectedTfPoints()
{
	if(tfHandle >= 0 && activeTfPointHandle >= 0)
	{
		TfPointValue value = pTfManager->getTf(tfHandle)->getValueOfTfPoint(activeTfPointHandle);

		// Assing active's tfPoint values to all selected
		for(std::set<GLint>::iterator it = selectedTfPoints.begin(); it != selectedTfPoints.end(); ++it)
		{
			if((*it) != activeTfPointHandle)
			{
				pTfManager->getTf(tfHandle)->setValueOfTfPoint(*it, value);
			}
		}

		preintegrationShouldBeUpdated = GL_TRUE;
	}
}

void TfEditor::toggleLinkControlPoints()
{
	if(tfHandle >= 0 && activeTfPointHandle >= 0)
	{
		pTfManager->getTf(tfHandle)->toggleTfPointControlPointsLinked(activeTfPointHandle);
		preintegrationShouldBeUpdated = GL_TRUE;
	}
}

void TfEditor::setTfVisualization(TfVisualization tfVisualization)
{
	this->tfVisualization = tfVisualization;
}

void TfEditor::resetActiveTfPointValue()
{
	if(tfHandle >= 0 && activeTfPointHandle >= 0)
	{
		pTfManager->getTf(tfHandle)->setValueOfTfPoint(activeTfPointHandle, TfPointValue());
		preintegrationShouldBeUpdated = GL_TRUE;
	}
}

void TfEditor::handleInput(InputData inputData)
{
	// Call super method to set viewport
	Viewport::handleInput(inputData);

	// Scale input to original viewport canvas
	glm::vec2 coord = (relativeCursorPos-0.5f) * 2.0f;

	// Coord for camera movement
	glm::vec2 cameraMovementCoord = (coord / cameraZoom);

	// Adjust input to camera
	coord = (coord / cameraZoom + cameraPosition) ;

	// Are coordinates inside borders of transferfunction?
	GLboolean insideBorders = pTfManager->getTf(tfHandle)->insideBorders(coord);

	// *** TFPOINT MANIPULATION ***

	// Select tfPoint in transferfunction
	if(inputData.mouse_button == GLFW_MOUSE_BUTTON_1 && inputData.mouse_action == GLFW_PRESS)
	{
		selectTfPoint(coord, inputData.mouse_mod == GLFW_MOD_CONTROL, inputData.mouse_mod == GLFW_MOD_SHIFT);
	}

	if(!tfPointsLocked)
	{
		GLboolean changedTf = GL_FALSE;

		// Add new tfPoins only inside borders
		if(insideBorders)
		{	
			// Add new tfPoints to transferfunction
			if(!inputData.mouse_drag_left && inputData.mouse_button == GLFW_MOUSE_BUTTON_2 && inputData.mouse_action == GLFW_PRESS)
			{
				addTfPoint(coord, inputData.mouse_mod == GLFW_MOD_CONTROL);
				changedTf = GL_TRUE;
			}
		}

		// Move selected control point or whole tfPoint in transferfunction if moving after pressing
		if(activeTfPointHandle >= 0 && inputData.mouse_drag_left)
		{
			moveTfPoint(coord);
			changedTf = GL_TRUE;
		}

		// Delete selected tfPoint
		if(inputData.key_int == GLFW_KEY_DELETE && inputData.key_action == GLFW_PRESS)
		{
			deleteTfPoint();
			changedTf = GL_TRUE;
		}

		// Duplicate selection
		if(inputData.key_int == GLFW_KEY_D && inputData.key_action == GLFW_PRESS)
		{
			duplicateSelectedTfPoints();
			changedTf = GL_TRUE;
		}

		// If transferfunction was changed, to something
		if(changedTf)
		{
			preintegrationShouldBeUpdated = GL_TRUE;
		}
	}

	// Unselect by pressing escape
	if(inputData.key_int == GLFW_KEY_ESCAPE && inputData.key_action == GLFW_PRESS)
	{
		unselectTfPoints();
	}

	// *** CAMERA MANIPULATION ***

	// Create local variable for delta camera movement
	glm::vec2 deltaCameraMovementCoord(0,0);

	// Only calc delta coord if necessary
	if(inputData.mouse_drag_middle && !(inputData.mouse_button == GLFW_MOUSE_BUTTON_3 && inputData.mouse_action == GLFW_PRESS))
	{
		deltaCameraMovementCoord = (cameraMovementCoord - prevCameraMovementCoord);
	}

	// Move camera
	if(inputData.mouse_drag_middle)
	{
		cameraPosition -= deltaCameraMovementCoord;
	}

	// Update camera movement coord
	prevCameraMovementCoord = cameraMovementCoord;

	// Zoom camera
	if(inputData.scroll_changed)
	{
		cameraZoom += inputData.scroll.y * TFEDITOR_CAMERA_ZOOM_SPEED;
		cameraZoom = glm::clamp(cameraZoom, TFEDITOR_CAMERA_ZOOM_MIN, TFEDITOR_CAMERA_ZOOM_MAX);
	}
}

void TfEditor::updateBarVariables()
{
	// Call super method
	Viewport::updateBarVariables();

	// Call update methods
	bar_tfPointColor.update();
	bar_tfPointAmbientMultiplier.update();
	bar_tfPointSpecularMultiplier.update();
	bar_tfPointSpecularSaturation.update();
	bar_tfPointSpecularPower.update();
	bar_tfPointGradientAlphaMultiplier.update();
	bar_tfPointFresnelAlphaMultiplier.update();
	bar_tfPointReflectionColorMultiplier.update();
	bar_tfPointEmissionColorMultiplier.update();
	bar_tfName.update();
	bar_activeTf.update();
	bar_activeVolume.update();
	bar_volumeName.update();
}

void TfEditor::useBarVariables()
{
	// Call super method
	Viewport::useBarVariables();

	// Update active transferfunction if necessary
	if(bar_activeTf.hasChanged())
	{
		if(bar_activeTf.getValue() < 0)
		{
			bar_activeTf.setValue(0);
		}
		else
		{
			setTfHandle(bar_activeTf.getValue());
		}
	}

	// Update transferfunction's name
	if(bar_tfName.hasChanged())
	{
		pTfManager->getTf(tfHandle)->rename(bar_tfName.getValue());
	}

		
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

	// Update selected point
	if(activeTfPointHandle >= 0)
	{
		// Only if bar has changed the value, change it. Otherwise there would be conflicts when using multiple tfEditors
		TfPointValue value = pTfManager->getTf(tfHandle)->getValueOfTfPoint(activeTfPointHandle);
		GLboolean changed = GL_FALSE;

		// Color
		if(bar_tfPointColor.hasChanged())
		{
			value.color = bar_tfPointColor.getValue();
			changed = GL_TRUE;
		}

		// Ambient multiplier
		if(bar_tfPointAmbientMultiplier.hasChanged())
		{
			value.ambientMultiplier = bar_tfPointAmbientMultiplier.getValue();
			changed = GL_TRUE;
		}

		// Specular multiplier
		if(bar_tfPointSpecularMultiplier.hasChanged())
		{
			value.specularMultiplier = bar_tfPointSpecularMultiplier.getValue();
			changed = GL_TRUE;
		}

		// Specular saturation
		if(bar_tfPointSpecularSaturation.hasChanged())
		{
			value.specularSaturation = bar_tfPointSpecularSaturation.getValue();
			changed = GL_TRUE;
		}

		// Specular power
		if(bar_tfPointSpecularPower.hasChanged())
		{
			value.specularPower = bar_tfPointSpecularPower.getValue();
			changed = GL_TRUE;
		}

		// Gradient alpha multiplier
		if(bar_tfPointGradientAlphaMultiplier.hasChanged())
		{
			value.gradientAlphaMultiplier = bar_tfPointGradientAlphaMultiplier.getValue();
			changed = GL_TRUE;
		}

		// Fresnel alpha multiplier
		if(bar_tfPointFresnelAlphaMultiplier.hasChanged())
		{
			value.fresnelAlphaMultiplier = bar_tfPointFresnelAlphaMultiplier.getValue();
			changed = GL_TRUE;
		}

		// Reflection color multiplier
		if(bar_tfPointReflectionColorMultiplier.hasChanged())
		{
			value.reflectionColorMultiplier = bar_tfPointReflectionColorMultiplier.getValue();
			changed = GL_TRUE;
		}

		// Emission color multiplier
		if(bar_tfPointEmissionColorMultiplier.hasChanged())
		{
			value.emissionColorMultiplier = bar_tfPointEmissionColorMultiplier.getValue();
			changed = GL_TRUE;
		}


		// Only if something changed, update tfPoint
		if(changed)
		{
			preintegrationShouldBeUpdated = GL_TRUE;
			pTfManager->getTf(tfHandle)->setValueOfTfPoint(activeTfPointHandle, value);
		}
	}
}

void TfEditor::fillBarVariables()
{
	// Call super method
	Viewport::fillBarVariables();

	// TfName
	if(tfHandle >= 0)
	{
		bar_tfName.setValue(pTfManager->getTf(tfHandle)->getName());
	}

	// VolumeName
	if(volumeHandle >= 0)
	{
		bar_volumeName.setValue(pVolumeManager->getVolume(volumeHandle)->getName());
	}

	// TfPoint values
	if(activeTfPointHandle >= 0)
	{
		TfPointValue value = pTfManager->getTf(tfHandle)->getValueOfTfPoint(activeTfPointHandle);
		bar_tfPointColor.setValue(value.color);
		bar_tfPointAmbientMultiplier.setValue(value.ambientMultiplier);
		bar_tfPointSpecularMultiplier.setValue(value.specularMultiplier);
		bar_tfPointSpecularSaturation.setValue(value.specularSaturation);
		bar_tfPointSpecularPower.setValue(value.specularPower);
		bar_tfPointGradientAlphaMultiplier.setValue(value.gradientAlphaMultiplier);
		bar_tfPointFresnelAlphaMultiplier.setValue(value.fresnelAlphaMultiplier);
		bar_tfPointReflectionColorMultiplier.setValue(value.reflectionColorMultiplier);
		bar_tfPointEmissionColorMultiplier.setValue(value.emissionColorMultiplier);
	}
}

void TfEditor::selectTfPoint(glm::vec2 coord, GLboolean multiSelection, GLboolean rowSelection)
{
	// Create local vec2 to give it as reference to functions
	glm::vec2 tmpOffset(0,0);

	// Try to select control points
	if(activeTfPointHandle >= 0)
	{
		leftControlHandleSelected = pTfManager->getTf(tfHandle)->intersectLeftControlPoint(activeTfPointHandle, coord, tfPointDrawScale, this->getAspectRatio(), tmpOffset);
		if(leftControlHandleSelected)
		{
			// If successfull, save offset
			selectionOffset = tmpOffset;
		}
		else
		{
			rightControlHandleSelected = pTfManager->getTf(tfHandle)->intersectRightControlPoint(activeTfPointHandle, coord, tfPointDrawScale, this->getAspectRatio(), tmpOffset);
			if(rightControlHandleSelected)
			{
				// If successfull, save offset
				selectionOffset = tmpOffset;
			}
		}
	}

	// If no control point selected, try to select a tfPoint
	if(!(leftControlHandleSelected || rightControlHandleSelected))
	{
		// Save previous active tfPoint
		GLint previousActiveTfPointHandle = activeTfPointHandle;

		// Get active tfPoint
		activeTfPointHandle = pTfManager->getTf(tfHandle)->selectTfPoint(coord, tfPointDrawScale, this->getAspectRatio(), tmpOffset);

		// If there was selected a tfPoint
		if(activeTfPointHandle >= 0)
		{
			leftControlHandleSelected = GL_FALSE;
			rightControlHandleSelected = GL_FALSE;

			// Save offset
			selectionOffset = tmpOffset;

			// Check wether tf point is already marked as selected
			GLboolean tfPointHandleIsSelected = std::find(selectedTfPoints.begin(), selectedTfPoints.end(), activeTfPointHandle)!=selectedTfPoints.end();

			// If one sets new point to active without pressing control
			if(!tfPointHandleIsSelected && !(selectedTfPoints.size() == 0) && !multiSelection)
			{
				selectedTfPoints.clear();
			}

			// Push it into selection set
			selectedTfPoints.insert(activeTfPointHandle);

			// Try selection of row
			if(rowSelection)
			{
				// Check previous active tfPoint handle and compare with current one
				if(previousActiveTfPointHandle >= 0 && previousActiveTfPointHandle != activeTfPointHandle)
				{
					std::set<GLint> rowSelection = pTfManager->getTf(tfHandle)->getRowOfHandles(previousActiveTfPointHandle, activeTfPointHandle);
					selectedTfPoints.insert(rowSelection.begin(), rowSelection.end());
				}
			}

			// Show bar variables
			showBarVariable("TfPoint", GL_TRUE);

			// Fill bar variables with new info (only necessary in release mode somehow)
			fillBarVariables();
		}
		else
		{
			// No tfPoint hit
			unselectTfPoints();
		}
	}
}

void TfEditor::moveTfPoint(glm::vec2 coord)
{
	// Decide what to move
	if(leftControlHandleSelected)
	{
		pTfManager->getTf(tfHandle)->setLeftControlPoint(activeTfPointHandle, coord - selectionOffset);
	}
	else if(rightControlHandleSelected)
	{
		pTfManager->getTf(tfHandle)->setRightControlPoint(activeTfPointHandle, coord - selectionOffset);
	}
	else
	{
		// Get old position of active tfPpoint
		glm::vec2 activeTfPointOldPosition = pTfManager->getTf(tfHandle)->getPositionOfTfPoint(activeTfPointHandle);

		// Move active point
		GLboolean clamped = pTfManager->getTf(tfHandle)->setTfPoint(activeTfPointHandle, coord - selectionOffset);
		
		// Calc delta movement (get real position because could have been clamped)
		glm::vec2 movementDelta = pTfManager->getTf(tfHandle)->getPositionOfTfPoint(activeTfPointHandle) - activeTfPointOldPosition;

		// Move all selected tfPoints
		for(std::set<GLint>::iterator it = selectedTfPoints.begin(); it != selectedTfPoints.end(); ++it)
		{
			if((*it) != activeTfPointHandle)
			{
				pTfManager->getTf(tfHandle)->moveTfPoint((*it), movementDelta);
			}
		}
		
	}
}

void TfEditor::addTfPoint(glm::vec2 coord, GLboolean multiSelection)
{
	// If no multiselection is claimed
	if(!multiSelection)
	{
		unselectTfPoints();
	}
	
	// Add tfPoint
	activeTfPointHandle = pTfManager->getTf(tfHandle)->addTfPoint(coord);
	
	// Push it into vector of selected tfPoints
	selectedTfPoints.insert(activeTfPointHandle);

	// Show bar variables
	showBarVariable("TfPoint", GL_TRUE);

	// Fill bar variables with new info (only necessary in release mode somehow)
	fillBarVariables();
}

void TfEditor::deleteTfPoint()
{
	if(activeTfPointHandle >= 0)
	{
		if(pTfManager->getTf(tfHandle)->deleteTfPoint(activeTfPointHandle))
		{
			// If deletion was successful
			unselectTfPoints();
		}
	}
}

void TfEditor::duplicateSelectedTfPoints()
{
	// Duplicate all selected tfPoints
	for(std::set<GLint>::iterator it = selectedTfPoints.begin(); it != selectedTfPoints.end(); ++it)
	{
		pTfManager->getTf(tfHandle)->duplicateTfPoint((*it));
	}
}

void TfEditor::unselectTfPoints()
{
	activeTfPointHandle = -1;
	selectedTfPoints.clear();
	leftControlHandleSelected = GL_FALSE;
	rightControlHandleSelected = GL_FALSE;
	selectionOffset = glm::vec2(0,0);
	showBarVariable("TfPoint", GL_FALSE);
}

static void TW_CALL newTfButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->newTf();
}

static void TW_CALL reloadTfButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->reloadTf();
}

static void TW_CALL saveTfButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->saveTf();
}

static void TW_CALL loadTfButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->loadTf();
}

static void TW_CALL resetCameraButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->resetCamera();
}

static void TW_CALL toggleLockTfPointsButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->toggleLockTfPoints();
}

static void TW_CALL assignActiveValueToSelectedTfPointsButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->assignActiveValueToSelectedTfPoints();
}

static void TW_CALL toggleLinkControlPointsButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->toggleLinkControlPoints();
}

static void TW_CALL setColorVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_COLOR);
}

static void TW_CALL setAmbientMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_AMBIENT_MULTIPLIER);
}

static void TW_CALL setSpecularMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_SPECULAR_MULTIPLIER);
}

static void TW_CALL setSpecularSaturationVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_SPECULAR_SATURATION);
}

static void TW_CALL setSpecularPowerVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_SPECULAR_POWER);
}

static void TW_CALL setGradientAlphaMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_GRADIENT_ALPHA_MULTIPLIER);
}

static void TW_CALL setFresnelAlphaMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_FRESNEL_ALPHA_MULTIPLIER);
}

static void TW_CALL setReflectionColorMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_RELECTION_COLOR_MULTIPLIER);
}

static void TW_CALL setEmissiveColorMultiplierVisualizationButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->setTfVisualization(TF_VISUALIZATION_EMISSION_COLOR_MULTIPLIER);
}

static void TW_CALL resetTfPointValueButtonCallback(void* clientData)
{
	reinterpret_cast<TfEditor*>(clientData)->resetActiveTfPointValue();
}