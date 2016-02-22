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

#include "TfPointValue.h"

TfPointValue::TfPointValue()
{
	reset();
}
	
TfPointValue::~TfPointValue()
{
}

void TfPointValue::reset()
{
	color = TFPOINTVALUE_COLOR;
	ambientMultiplier = TFPOINTVALUE_AMBIENT_MULTIPLIER;
	specularMultiplier = TFPOINTVALUE_SPECULAR_MULTIPLIER;
	specularSaturation = TFPOINTVALUE_SPECULAR_SATURATION;
	specularPower = TFPOINTVALUE_SPECULAR_POWER;
	gradientAlphaMultiplier = TFPOINTVALUE_GRADIENT_ALPHA_MULTIPLIER;
	fresnelAlphaMultiplier = TFPOINTVALUE_FRESNEL_ALPHA_MULTIPLIER;
	reflectionColorMultiplier = TFPOINTVALUE_REFLECTION_COLOR_MULTIPLIER;
	emissionColorMultiplier = TFPOINTVALUE_EMISSION_COLOR_MULTIPLIER;
}