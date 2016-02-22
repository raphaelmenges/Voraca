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
 * RcMananger
 *--------------
 * Manages raycaster. It is not allowed to delete
 * raycaster. Otherwise one has to change the implementation
 * of getRc().
 *
 */

#ifndef RCMANAGER_H_
#define RCMANAGER_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"

#include <map>
#include <string>

#include "Logger.h"
#include "Raycaster.h"
#include "RcCreator.h"

const std::string RCMANAGER_NEW_RC_NAME = "newRc";

class RcManager
{
public:
    RcManager();
    ~RcManager();

    /** Initialization */
    void init();

    /** Create new raycaster */
    GLint newRc();

    /** Reload raycaster */
    void reloadRc(GLint handle);

    /** Save raycaster */
    GLboolean saveRc(GLint handle, GLboolean overwriteExisting);

    /** Load raycaster. Returns -1 if it fails */
    GLint loadRc(std::string name);

    /** Returns pointer to raycaster, for one-time-use only! */
    Raycaster* getRc(GLint handle) const;

    /** Returns latest rcHandle */
    GLint getLatestRcHandle() const;

protected:
    /** Latest used handle */
    GLint latestRcHandle;

    /** Map with raycaster */
    std::map<GLint, Raycaster*> raycaster;

    /** Counter for next handle that is free */
    GLint rcHandleCounter;

    /** Creator of the raycaster */
    RcCreator rcCreator;

    /** Counter for new raycaster*/
    GLint newRcCounter;
};

#endif
