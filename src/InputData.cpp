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

#include "InputData.h"

InputData::InputData()
{
	mouse_drag_left = false;
	mouse_drag_right = false;
	mouse_drag_middle = false;
}

InputData::~InputData()
{
}

void InputData::reset()
{
	key_int = -1;
	key_int_old = -1;
	key_action = -1;
	key_mods = -1;
	key_changed = false;
	cursor_pos_delta = cursor_pos - lastCursorPos;
	lastCursorPos = cursor_pos;
	cursor_pos_changed = false;
	mouse_button = -1;
	mouse_action = -1;
	mouse_mod = -1;
	mouse_changed = false;
	scroll = glm::vec2(0, 0);
	scroll_changed = false;
	// mouse_left, mouse_right and mouse_middle controlled by Input::buttons
}

void InputData::resetMouseDrags()
{
	mouse_drag_left = false;
	mouse_drag_right = false;
	mouse_drag_middle = false;
}

GLboolean InputData::isMouseDragOngoing() const
{
	return mouse_drag_left || mouse_drag_right || mouse_drag_middle;
}