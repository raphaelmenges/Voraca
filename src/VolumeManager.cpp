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

#include "VolumeManager.h"

VolumeManager::VolumeManager()
{
	volumeHandleCounter = 0;
	newVolumeCounter = 0;
}

VolumeManager::~VolumeManager()
{
	// Delete all volumes
	for(std::map<GLint, Volume*>::iterator it = volumes.begin(); it != volumes.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	
	}

	// Clear map
	volumes.clear();
}

void VolumeManager::init()
{
}

GLint VolumeManager::importPVM(std::string name)
{
	// Logging
	LogInfo("Import volume: " + name);

	// Let the creator do the work
	Volume* pVolume = volumeCreator.importPVM(name, volumeHandleCounter);

	// Check wether importing was successful
	if(pVolume != NULL)
	{
		// Add to map
		volumes[volumeHandleCounter] = pVolume;

		// Set latest handle
		latestVolumeHandle = volumeHandleCounter;

		// Increment handle counter
		volumeHandleCounter++;
	}

	return latestVolumeHandle;
}

GLint VolumeManager::importDAT(std::string name)
{
	// Logging
	LogInfo("Import volume: " + name);

	// Let the creator do the work
	Volume* pVolume = volumeCreator.importDAT(name, volumeHandleCounter);

	// Check wether importing was successful
	if(pVolume != NULL)
	{
		// Add to map
		volumes[volumeHandleCounter] = pVolume;

		// Set latest handle
		latestVolumeHandle = volumeHandleCounter;

		// Increment handle counter
		volumeHandleCounter++;
	}

	return latestVolumeHandle;
}

GLint VolumeManager::createDefaultVolume()
{
	// Logging
	LogInfo("Create default volume");

	// Create default volume
	Volume* pVolume = volumeCreator.createDefaultVolume("Cube", volumeHandleCounter);

	// Add to map
	volumes[volumeHandleCounter] = pVolume;

	// Set latest volumeHandle
	latestVolumeHandle = volumeHandleCounter;

	// Increment volumeHandle counter
	volumeHandleCounter++;

	// Return handle
	return latestVolumeHandle;
}

void VolumeManager::reloadVolume(GLint handle)
{
	// Logging
	LogInfo("Reload volume: " + getVolume(handle)->getName());

	Volume* pOldVolume = getVolume(handle);

	Volume* pReloadedVolume = volumeCreator.readFromFile(pOldVolume->getName(), handle);

	// If there exists a XML-File with same name as volume, proceed
	if(pReloadedVolume != NULL)
	{
		delete pOldVolume;
		volumes[handle] = pReloadedVolume;
	}
}

GLboolean VolumeManager::saveVolume(GLint handle, GLboolean overwriteExisting)
{
	// Logging
	LogInfo("Save volume: " + getVolume(handle)->getName());

	// Forward to creator
	return volumeCreator.writeToFile(getVolume(handle), overwriteExisting);
}

GLint VolumeManager::loadVolume(std::string name)
{
	// Logging
	LogInfo("Load volume: " + name);

	// Read volume from file
	Volume* pVolume = volumeCreator.readFromFile(name, volumeHandleCounter);

	// Check wether loading was successful
	if(pVolume != NULL)
	{
		// Add to map
		volumes[volumeHandleCounter] = pVolume;

		// Set latest volumeHandle
		latestVolumeHandle = volumeHandleCounter;

		// Increment volumeHandle counter
		volumeHandleCounter++;

		// Return handle
		return latestVolumeHandle;
	}
	else
	{
		return -1;
	}
}

Volume* VolumeManager::getVolume(GLint handle)
{
	// Only possible because volumes may not be deleted
	if(volumes.size() > static_cast<GLuint>(handle))
	{
		return volumes.at(handle);
	}
	else
	{
		return NULL;
	}
}


GLint VolumeManager::getLatestVolumeHandle()
{
	return latestVolumeHandle;
}
