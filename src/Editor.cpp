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

#include "Editor.h"

Editor::Editor()
{
	barsActive = GL_TRUE;
	benchmarkActive = GL_FALSE;
	doQuit = GL_FALSE;
	doReset = GL_FALSE;
	doToggleFullscreen = GL_FALSE;
	bar_viewportPreset = EDITOR_VIEWPORT_PRESET_A;
	previousViewportPreset = -1;
	bar_overwriteExisting = GL_FALSE;
	bar_setVolumeInAllViewports = GL_TRUE;
}

Editor::~Editor()
{
}

void Editor::init(GLint windowWidth, GLint windowHeight)
{
	LogInfo("");
	LogInfo("***SPECIAL KEYS***");
	LogInfo("F1 = (Un)Hide Bars");
	LogInfo("F2 = Start/Stop Benchmark");
	LogInfo("");

	// Intitalize size
	windowResize(windowWidth, windowHeight);

	// TfManager
	tfManager.init();

	// RcManager
	rcManager.init();

	// Volume manager
	volumeManager.init();

	// Try to use data from launch file
	GLint errorsInLaunchFile = 0;
	std::string launchVolume;
	std::string launchTransferfunction;
	std::string launchRaycaster;
	errorsInLaunchFile += Launch::getVolume(launchVolume);
	errorsInLaunchFile += Launch::getTransferfunction(launchTransferfunction);
	errorsInLaunchFile += Launch::getRaycaster(launchRaycaster);

	// Create or load stuff
	GLint volumeHandle = -1;

	if(errorsInLaunchFile > 0)
	{
		// New volume, raycaster and transferfunction
		volumeHandle = volumeManager.createDefaultVolume();
		tfManager.newTf();
		rcManager.newRc();
	}
	else
	{
		// Try to load volume
		volumeHandle = volumeManager.loadVolume(launchVolume);
		if(volumeHandle < 0)
		{
			volumeHandle = volumeManager.createDefaultVolume();
		}

		// Try to load raycaster
		GLint tfHandle = tfManager.loadTf(launchTransferfunction);
		if(tfHandle < 0)
		{
			tfManager.newTf();
		}

		// Try to load transferfunction
		GLint rcHandle = rcManager.loadRc(launchRaycaster);
		if(rcHandle < 0)
		{
			rcManager.newRc();
		}
	}

	// Editor is master of volumes and holds management functions
	setVolumeHandle(volumeHandle);

	// Bar initialization
	TwInit(TW_OPENGL_CORE, NULL); 
	pBar = TwNewBar(EDITOR_BAR_TITLE.c_str());
	TwDefine(" GLOBAL contained=true ");
	TwDefine(" TW_HELP visible=false "); 

	// Special functions to enable the bar to edit std::string
	TwCopyStdStringToClientFunc(copyStdStringToClient);

	// Set bar color
	GLint barColor[] = {0,0,0};
	TwSetParam(pBar, NULL, "color", TW_PARAM_INT32, 3, barColor);

	// Set position of bar
	GLint barGivenSizeArray[2];
	TwGetParam(pBar, NULL, "size", TW_PARAM_INT32, 2, barGivenSizeArray);
	GLint barPositionArray[] = {static_cast<GLint>(windowWidth) - barGivenSizeArray[0], static_cast<GLint>(0)};
	TwSetParam(pBar, NULL, "position", TW_PARAM_INT32, 2, barPositionArray);

	// Set size of bar
	GLint barNewSizeArray[] = {barGivenSizeArray[0], 450};
	TwSetParam(pBar, NULL, "size", TW_PARAM_INT32, 2, barNewSizeArray);

	// Configurate viewport preset enumerations
	TwEnumVal viewportPresetEV[] = { {EDITOR_VIEWPORT_PRESET_A, "A"}, {EDITOR_VIEWPORT_PRESET_B, "B"}, {EDITOR_VIEWPORT_PRESET_C, "C"}, {EDITOR_VIEWPORT_PRESET_D, "D"} };
	TwType viewportPresetType = TwDefineEnum("Viewport Presets", viewportPresetEV, 4);
	
	// Add variables to bar
	TwAddVarRO(pBar, "TPF", TW_TYPE_FLOAT, &bar_tpf, " precision=5 help='Time per frame (ms).' ");
	TwAddVarRO(pBar, "FPS", TW_TYPE_FLOAT, &bar_fps, " precision=0 ");
	//TwAddButton(pBar, "Toggle Fullscreen", toggleFullscreenButtonCallback, this, "");
	TwAddVarRW(pBar, "Viewport Preset", viewportPresetType, &bar_viewportPreset, NULL);

	TwAddSeparator(pBar, NULL, "");

	TwAddVarRW(pBar, "Active Volume", TW_TYPE_INT32, &(bar_activeVolume.value), " min=0 ");
	TwAddVarRW(pBar, "Volume Name", TW_TYPE_STDSTRING, &(bar_volumeName.value), "");
	
	TwAddSeparator(pBar, NULL, "");

	TwAddVarRW(pBar, "Voxel Scale Multiplier X", TW_TYPE_FLOAT, &(bar_volumeVoxelScaleMultiplier.value.x), "");
	TwAddVarRW(pBar, "Voxel Scale Multiplier Y", TW_TYPE_FLOAT, &(bar_volumeVoxelScaleMultiplier.value.y), "");
	TwAddVarRW(pBar, "Voxel Scale Multiplier Z", TW_TYPE_FLOAT, &(bar_volumeVoxelScaleMultiplier.value.z), "");
	TwAddVarRW(pBar, "Value Offset", TW_TYPE_FLOAT, &(bar_volumeValueOffset.value), "");
	TwAddVarRW(pBar, "Value Scale", TW_TYPE_FLOAT, &(bar_volumeValueScale.value), "");
	TwAddVarRW(pBar, "Alpha Rotation", TW_TYPE_FLOAT, &(bar_volumeEulerZXZRotation.value.x), "");
	TwAddVarRW(pBar, "Beta Rotation", TW_TYPE_FLOAT, &(bar_volumeEulerZXZRotation.value.y), "");
	TwAddVarRW(pBar, "Gamma Rotation", TW_TYPE_FLOAT, &(bar_volumeEulerZXZRotation.value.z), "");
	TwAddVarRW(pBar, "Mirror X", TW_TYPE_BOOLCPP, &(bar_volumeMirrorX.value), "");
	TwAddVarRW(pBar, "Mirror Y", TW_TYPE_BOOLCPP, &(bar_volumeMirrorY.value), "");
	TwAddVarRW(pBar, "Mirror Z", TW_TYPE_BOOLCPP, &(bar_volumeMirrorZ.value), "");
	TwAddVarRW(pBar, "Use Linear Filtering", TW_TYPE_BOOLCPP, &(bar_volumeLinearFiltering.value), "");

	TwAddSeparator(pBar, NULL, "");
	
	TwAddButton(pBar, "Reload", reloadVolumeButtonCallback, this, " group='Volume Management' ");
	TwAddButton(pBar, "Save", saveVolumeButtonCallback, this, " group='Volume Management' ");
	TwAddVarRW(pBar, "Volume To Load/Import", TW_TYPE_STDSTRING, &bar_pathToExternVolume, " group='Volume Management' ");
	TwAddButton(pBar, "Load", loadVolumeButtonCallback, this, " group='Volume Management' ");
	TwAddButton(pBar, "Import PVM", importPVMButtonCallback, this, " group='Volume Management' ");
	TwAddButton(pBar, "Import DAT", importDATButtonCallback, this, " group='Volume Management' ");
	TwAddVarRW(pBar, "Overwrite Existing", TW_TYPE_BOOLCPP, &bar_overwriteExisting, " group='Volume Management' ");
	TwAddVarRW(pBar, "Set Loaded/Imported Volume In All Viewports", TW_TYPE_BOOLCPP, &bar_setVolumeInAllViewports, " group='Volume Management' ");

	TwAddSeparator(pBar, NULL, "");

	TwAddButton(pBar, "Quit", quitButtonCallback, this, "");
	TwAddButton(pBar, "Reset", resetButtonCallback, this, "");

	// Bar step width
	TwSetParam(pBar, "Value Offset", "step", TW_PARAM_FLOAT, 1, &EDITOR_BAR_VOLUME_VALUE_OFFSET_STEP);
	TwSetParam(pBar, "Value Scale", "step", TW_PARAM_FLOAT, 1, &EDITOR_BAR_VOLUME_VALUE_SCALE_STEP);

	TwSetParam(pBar, "Voxel Scale Multiplier X", "step", TW_PARAM_FLOAT, 1, &EDITOR_BAR_VOLUME_VOXEL_SCALE_MULTIPLIER_STEP);
	TwSetParam(pBar, "Voxel Scale Multiplier Y", "step", TW_PARAM_FLOAT, 1, &EDITOR_BAR_VOLUME_VOXEL_SCALE_MULTIPLIER_STEP);
	TwSetParam(pBar, "Voxel Scale Multiplier Z", "step", TW_PARAM_FLOAT, 1, &EDITOR_BAR_VOLUME_VOXEL_SCALE_MULTIPLIER_STEP);

	// Some bar variable settings
	TwSetParam(pBar, "Value Offset", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VALUE_OFFSET_MIN);
	TwSetParam(pBar, "Value Offset", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VALUE_OFFSET_MAX);
	TwSetParam(pBar, "Value Scale", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VALUE_SCALE_MIN);
	TwSetParam(pBar, "Value Scale", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VALUE_SCALE_MAX);
	TwSetParam(pBar, "Alpha Rotation", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MIN.x);
	TwSetParam(pBar, "Alpha Rotation", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MAX.x);
	TwSetParam(pBar, "Beta Rotation", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MIN.y);
	TwSetParam(pBar, "Beta Rotation", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MAX.y);
	TwSetParam(pBar, "Gamma Rotation", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MIN.z);
	TwSetParam(pBar, "Gamma Rotation", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_EULER_ZXZ_ROTATION_MAX.z);
	TwSetParam(pBar, "Voxel Scale Multiplier X", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MIN.x);
	TwSetParam(pBar, "Voxel Scale Multiplier X", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MAX.x);
	TwSetParam(pBar, "Voxel Scale Multiplier Y", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MIN.y);
	TwSetParam(pBar, "Voxel Scale Multiplier Y", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MAX.y);
	TwSetParam(pBar, "Voxel Scale Multiplier Z", "min", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MIN.z);
	TwSetParam(pBar, "Voxel Scale Multiplier Z", "max", TW_PARAM_FLOAT, 1, &VOLUMEPROPERTIES_VOXEL_SCALE_MULTIPLIER_MAX.z);

	// Fold groups
	GLint opened = 0;
	TwSetParam(pBar, "Volume Management", "opened", TW_PARAM_INT32, 1, &opened);

	// Fill bar variables
	fillBarVariables();

	// ViewportMananger
	viewportManager.init(windowWidth, windowHeight, &tfManager, &rcManager, &volumeManager, barsActive);
}

EditorCallToApp Editor::update(GLfloat tpf, InputData inputData)
{
	// If benchmark is active, update it
	if(benchmarkActive)
	{
		benchmarkTime += static_cast<GLdouble>(tpf);
		benchmarkCounter++;
	}

	// Process some input
	if(inputData.key_action == GLFW_PRESS)
	{
		// Bars
		if(inputData.key_int == GLFW_KEY_F1)
		{
			barsActive = !barsActive;
			viewportManager.setBarsActive(barsActive);
		}

		// Benchmark mode
		if(inputData.key_int == GLFW_KEY_F2)
		{
			if(benchmarkActive)
			{
				// Stop benchmark
				LogInfo("");
				LogInfo("***BENCHMARK RESULTS***");
				LogInfo("Average TPF: " + UT::to_string(benchmarkTime/benchmarkCounter));
				LogInfo("Average FPS: " + UT::to_string(benchmarkCounter/benchmarkTime));
				LogInfo("");
			}
			else
			{
				// Start benchmark
				LogInfo("Benchmark started...");
				benchmarkTime = 0;
				benchmarkCounter = 0;
			}
			benchmarkActive = !benchmarkActive;
		}
	}

	// Viewport presets
	if(previousViewportPreset != bar_viewportPreset)
	{
		previousViewportPreset = bar_viewportPreset;

		viewportManager.deleteAllViewports();

		switch(bar_viewportPreset)
		{
			case EDITOR_VIEWPORT_PRESET_A:
				{
					viewportManager.addViewport(VIEWPORT_TFEDITOR, 0, 0, 1, 0.4f);
					viewportManager.addViewport(VIEWPORT_RENDERER, 0, 0.4f, 0.6f, 0.6f);
					viewportManager.addViewport(VIEWPORT_SLICER, 0.6f, 0.4f, 0.4f, 0.6f);
					break;
				}
			case EDITOR_VIEWPORT_PRESET_B:
				{
					viewportManager.addViewport(VIEWPORT_RENDERER, 0, 0, 1.0f, 1.0f);
					break;
				}
			case EDITOR_VIEWPORT_PRESET_C:
				{
					viewportManager.addViewport(VIEWPORT_TFEDITOR, 0, 0, 1, 0.3f);
					viewportManager.addViewport(VIEWPORT_RENDERER, 0, 0.3f, 1.0f, 0.7f);
					break;
				}
			case EDITOR_VIEWPORT_PRESET_D:
				{
					viewportManager.addViewport(VIEWPORT_TFEDITOR, 0, 0, 1, 0.25f);
					viewportManager.addViewport(VIEWPORT_TFEDITOR, 0, 0.25f, 1, 0.25f);
					viewportManager.addViewport(VIEWPORT_RENDERER, 0, 0.5f, 0.4f, 0.5f);
					viewportManager.addViewport(VIEWPORT_RENDERER, 0.4f, 0.5f, 0.3f, 0.5f);
					viewportManager.addViewport(VIEWPORT_SLICER, 0.7f, 0.5f, 0.3f, 0.5f);
					break;
				}
		}		
	}

	// First update bars
	if(barsActive)
	{
		// Update
		bar_tpf = tpf;
		bar_fps = 1.0f / tpf;

		// Test if mouse is in use by another component, otherwise handle input to bars
		if((!inputData.mouse_drag_left && !inputData.mouse_drag_right && !inputData.mouse_drag_middle) || inputHandledByBars)
		{
			inputHandledByBars = 0;
			mouseButtonsHandledByBars = 0;
			forwardInputToBars(inputData);

			// If bars used input, don't forward it
			if(inputHandledByBars+mouseButtonsHandledByBars > 0)
			{
				inputData.reset();
				inputData.resetMouseDrags();
				if(mouseButtonsHandledByBars > 0)
				{
					viewportManager.unselectSelectedViewport();
				}
			}
		}
	}

	// Update bar variables
	if(barsActive)
	{
		updateBarVariables();
		useBarVariables();
	}

	// Update viewports
	viewportManager.update(tpf, inputData);

	if(doQuit)
	{
		return EDITOR_CALL_QUIT;
	}
	else if(doReset)
	{
		doReset = GL_FALSE;
		return EDITOR_CALL_RESET;
	}
	else if(doToggleFullscreen)
	{
		doToggleFullscreen = GL_FALSE;
		return EDITOR_CALL_TOGGLE_FULLSCREEN;
	}
	else
	{
		return EDITOR_CALL_NONE;
	}
}

void Editor::draw()
{
	if(barsActive)
	{
		fillBarVariables();
	}

	// Draw viewports
	viewportManager.draw();

	// Draw bars at last
	if(barsActive)
	{
		TwDraw();
	}
}

void Editor::terminate()
{
	viewportManager.terminate();
	TwTerminate();
}

void Editor::windowResize(GLint windowWidth, GLint windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	// Tell Bars size of window
	TwWindowSize(windowWidth, windowHeight);

	// Tell viewportMananger size of window
	viewportManager.windowResize(windowWidth, windowHeight);
}

void Editor::quit()
{
	doQuit = GL_TRUE;
}

void Editor::reset()
{
	doReset = GL_TRUE;
}

void Editor::toggleFullscreen()
{
	doToggleFullscreen = GL_TRUE;
}

void Editor::reloadVolume()
{
	volumeManager.reloadVolume(volumeHandle);
	setVolumeHandle(volumeHandle);
}

void Editor::saveVolume()
{
	volumeManager.saveVolume(volumeHandle, bar_overwriteExisting);
}

void Editor::loadVolume()
{
	if(setVolumeHandle(volumeManager.loadVolume(bar_pathToExternVolume)))
	{
		setVolumeInAllViewports(volumeHandle);
	}
}

void Editor::importPVM()
{
	if(setVolumeHandle(volumeManager.importPVM(bar_pathToExternVolume)))
	{
		setVolumeInAllViewports(volumeHandle);
	}
}

void Editor::importDAT()
{
	if(setVolumeHandle(volumeManager.importDAT(bar_pathToExternVolume)))
	{
		setVolumeInAllViewports(volumeHandle);
	}
}

void Editor::forwardInputToBars(InputData inputData)
{
	inputHandledByBars += TwEventCharGLFW(inputData.key_int_old, inputData.key_action);
	inputHandledByBars += TwEventKeyGLFW(inputData.key_int_old, inputData.key_action);
	inputHandledByBars += TwEventMousePosGLFW((GLint)inputData.cursor_pos.x, windowHeight - 1 - (GLint)inputData.cursor_pos.y);
	//inputHandledByBars += TwEventMouseWheelGLFW((GLint)inputData.scroll.y);
	mouseButtonsHandledByBars += TwEventMouseButtonGLFW(inputData.mouse_button, inputData.mouse_action);
}

bool Editor::setVolumeHandle(GLint handle)
{
	bool check = GL_FALSE;

	// Try to assign volume handle
	if(handle >= 0)
	{
		Volume* pVolume = volumeManager.getVolume(handle);
		if(pVolume != NULL)
		{
			volumeHandle = handle;
			check = GL_TRUE;
		}
		bar_activeVolume.setValue(volumeHandle);
	}

	return check;
}

void Editor::setVolumeInAllViewports(GLint handle)
{
	if(bar_setVolumeInAllViewports)
	{
		viewportManager.setVolumeInAllViewports(handle);
	}
}

void Editor::updateBarVariables()
{
	// Call update methods
	bar_activeVolume.update();
	bar_volumeVoxelScaleMultiplier.update();
	bar_volumeValueOffset.update();
	bar_volumeValueScale.update();
	bar_volumeEulerZXZRotation.update();
	bar_volumeMirrorX.update();
	bar_volumeMirrorY.update();
	bar_volumeMirrorZ.update();
	bar_volumeLinearFiltering.update();
	bar_volumeName.update();
}

void Editor::useBarVariables()
{
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
		}
	}

	// Update volume's name
	if(bar_volumeName.hasChanged())
	{
		volumeManager.getVolume(volumeHandle)->rename(bar_volumeName.getValue());
	}

	// Update volumes members
	VolumeProperties properties = volumeManager.getVolume(volumeHandle)->getProperties();
	GLboolean changed = GL_FALSE;

	if(bar_volumeVoxelScaleMultiplier.hasChanged())
	{
		properties.voxelScaleMultiplier = bar_volumeVoxelScaleMultiplier.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeValueOffset.hasChanged())
	{
		properties.valueOffset = bar_volumeValueOffset.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeValueScale.hasChanged())
	{
		properties.valueScale = bar_volumeValueScale.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeEulerZXZRotation.hasChanged())
	{
		properties.eulerZXZRotation = bar_volumeEulerZXZRotation.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeMirrorX.hasChanged())
	{
		properties.mirrorX = bar_volumeMirrorX.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeMirrorY.hasChanged())
	{
		properties.mirrorY = bar_volumeMirrorY.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeMirrorZ.hasChanged())
	{
		properties.mirrorZ = bar_volumeMirrorZ.getValue();
		changed = GL_TRUE;
	}
	if(bar_volumeLinearFiltering.hasChanged())
	{
		properties.useLinearFiltering = bar_volumeLinearFiltering.getValue();
		changed = GL_TRUE;
	}

	if(changed)
	{
		volumeManager.getVolume(volumeHandle)->setProperties(properties);
	}
	
}

void Editor::fillBarVariables()
{
	// Volume name
	if(volumeHandle >= 0)
	{
		Volume* pVolume = volumeManager.getVolume(volumeHandle);
		bar_volumeName.setValue(pVolume->getName());

		VolumeProperties properties = volumeManager.getVolume(volumeHandle)->getProperties();
		bar_volumeVoxelScaleMultiplier.setValue(properties.voxelScaleMultiplier);
		bar_volumeValueOffset.setValue(properties.valueOffset);
		bar_volumeValueScale.setValue(properties.valueScale);
		bar_volumeEulerZXZRotation.setValue(properties.eulerZXZRotation);
		bar_volumeMirrorX.setValue(properties.mirrorX);
		bar_volumeMirrorY.setValue(properties.mirrorY);
		bar_volumeMirrorZ.setValue(properties.mirrorZ);
		bar_volumeLinearFiltering.setValue(properties.useLinearFiltering);
	}
}


void TW_CALL copyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
  // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
  destinationClientString = sourceLibraryString;
}

static void TW_CALL quitButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->quit();
}

static void TW_CALL resetButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->reset();
}

static void TW_CALL toggleFullscreenButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->toggleFullscreen();
}

static void TW_CALL reloadVolumeButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->reloadVolume();
}

static void TW_CALL saveVolumeButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->saveVolume();
}

static void TW_CALL loadVolumeButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->loadVolume();
}

static void TW_CALL importPVMButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->importPVM();
}

static void TW_CALL importDATButtonCallback(void* clientData)
{
	reinterpret_cast<Editor*>(clientData)->importDAT();
}

