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
 * VolumeMananger
 *--------------
 * Manages volumes. It is not allowed to delete
 * volumes. Otherwise one has to change the implementation
 * of getVolume().
 *
 */

#ifndef VOLUMEMANAGER_H_
#define VOLUMEMANAGER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"

#include <map>
#include <string>

#include "Logger.h"
#include "Volume.h"
#include "VolumeCreator.h"

const std::string VOLUMEMANAGER_NEW_VOLUME_NAME = "newVolume";

class VolumeManager
{
public:
    VolumeManager();
    ~VolumeManager();

    /** Initialization */
    void init();

    /** Import PVM */
    GLint importPVM(std::string name);

    /** Import DAT */
    GLint importDAT(std::string name);

    /** Create simple default volume */
    GLint createDefaultVolume();

    /** Reload already loaded volume */
    void reloadVolume(GLint handle);

    /** Save volume */
    GLboolean saveVolume(GLint handle, GLboolean overwriteExisting);

    /** Load volume. Returns -1 if it fails */
    GLint loadVolume(std::string name);

    /** Returns pointer to volume, for one-time-use only! */
    Volume* getVolume(GLint handle);

    /** Returns latest volumeHandle */
    GLint getLatestVolumeHandle();

protected:
    /** Latest used handle */
    GLint latestVolumeHandle;

    /** Map with volumes */
    std::map<GLint, Volume*> volumes;

    /** Counter for next handle that is free */
    GLint volumeHandleCounter;

    /** Creator of the volumes */
    VolumeCreator volumeCreator;

    /** Counter for new volumes */
    GLint newVolumeCounter;
};

#endif
