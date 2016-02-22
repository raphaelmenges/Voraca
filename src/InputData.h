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
 * InputData
 *--------------
 * Just the input data.
 *
 */

#ifndef INPUTDATA_H_
#define INPUTDATA_H_

#include "OpenGLLoader/gl_core_3_3.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

class InputData
{
public:
    InputData();
    ~InputData();

    GLint key_int;
    GLint key_int_old;
    GLint key_action;
    GLint key_mods;
    GLint key_changed;
    glm::vec2 cursor_pos;
    GLboolean cursor_pos_changed;
    glm::vec2 cursor_pos_delta;
    GLboolean mouse_drag_left;
    GLboolean mouse_drag_right;
    GLboolean mouse_drag_middle;
    GLint mouse_button;
    GLint mouse_action;
    GLint mouse_mod;
    GLboolean mouse_changed;
    glm::vec2 scroll;
    GLboolean scroll_changed;

    /** Does reset everything but mouse_drags */
    void reset();

    /** Reset mouse_drag_left, mouse_drag_right and mouse_drag_middle */
    void resetMouseDrags();

    /** Tells wether any mouse drag is ongoing */
    GLboolean isMouseDragOngoing() const;

protected:
    glm::vec2 lastCursorPos;
};

#endif
