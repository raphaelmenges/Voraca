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

in vec3 position;
in vec3 direction;
in vec3 sunDirection;
in vec3 volumeExtentMin;
in vec3 volumeExtentMax;
out vec4 fragmentColor;

// Volumes
uniform sampler3D uniformVolume;
uniform sampler3D uniformImportanceVolume;

// Transferfunctions (preintegrated)
uniform sampler2D uniformColorAlphaPreintegration;
uniform sampler2D uniformAmbientSpecularPreintegration;
uniform sampler2D uniformAdvancedPreintegration;

// Transferfunctions (for posterinterpolation)
uniform sampler1D uniformColorAlpha;
uniform sampler1D uniformAmbientSpecular;
uniform sampler1D uniformAdvanced;

// Some textures for effects like jittering or reflection
uniform sampler2D uniformNoise;
uniform sampler2D uniformReflection;

// Model and view matrices
uniform mat4 uniformModelScale;
uniform mat4 uniformView;

// Lighting stuff
uniform vec4 uniformSunColor;
uniform vec3 uniformAmbientColor;

// Many different things like iteration count, fresnel power etc.
uniform vec4 uniformBasicInput;
uniform vec4 uniformAdvancedInput;

// Information from the volume object, like value scale and offset
uniform vec2 uniformVolumeValueInformation;
uniform vec3 uniformVolumeResolution;

// Some nasty constants
const float shadowBias = 5;
const float standardStepSize = 0.008;
const float shadowAlphaMultiplier = 5;
const float jitteringFade = 0.1;
const float emptySpaceSkippingTreshold = 0.001;

/** Calculate voxel spaced step size */
float voxelSpacedStepSize(vec3 dir)
{
	vec3 voxelSize = 1.0/uniformVolumeResolution;
	vec3 result = voxelSize * dir;
	return length(result);
}

/** Calculate ray length */
float rayLength(vec3 pos, vec3 dir, vec3 volumeExtMin, vec3 volumeExtMax)
{
	// Initialization
	float rayLength = 100000;
	float t;

	if(dir.z != 0)
	{
		// Front
		t = (volumeExtMin.z - pos.z) / dir.z;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Back
		t = (volumeExtMax.z - pos.z) / dir.z;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	if(dir.x != 0)
	{
		// Left
		t = (volumeExtMin.x - pos.x) / dir.x;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Right
		t = (volumeExtMax.x - pos.x) / dir.x;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	if(dir.y != 0)
	{
		// Top
		t = (volumeExtMin.y - pos.y) / dir.y;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}

		// Button
		t = (volumeExtMax.y - pos.y) / dir.y;
		if(t > 0)
		{
			rayLength = min(rayLength, t);
		}
	}

	return rayLength;
}

/** Returns vec4(normalized normal, magnitude), model space */
vec4 normalOnRawData(vec3 pos, float offset)
{
	vec3 nrm = vec3(0,0,0);

	// Get values from volume
	float x1 = texture(uniformVolume, vec3(pos.x + offset, pos.y, pos.z)).r;
	float x2 = texture(uniformVolume, vec3(pos.x - offset, pos.y, pos.z)).r;
	float y1 = texture(uniformVolume, vec3(pos.x, pos.y + offset, pos.z)).r;
	float y2 = texture(uniformVolume, vec3(pos.x, pos.y - offset, pos.z)).r;
	float z1 = texture(uniformVolume, vec3(pos.x, pos.y, pos.z + offset)).r;
	float z2 = texture(uniformVolume, vec3(pos.x, pos.y, pos.z - offset)).r;

	// Create good normals at extent
	#if defined(USE_EXTENT_AWARE_NORMALS)
		if(pos.x + offset > volumeExtentMax.x)
		{
			x1 = 0;
		}
		if(pos.y + offset > volumeExtentMax.y)
		{
			y1 = 0;
		}
		if(pos.z + offset > volumeExtentMax.z)
		{
			z1 = 0;
		}
		if(pos.x - offset < volumeExtentMin.x)
		{
			x2 = 0;
		}
		if(pos.y - offset < volumeExtentMin.y)
		{
			y2 = 0;
		}
		if(pos.z - offset < volumeExtentMin.z)
		{
			z2 = 0;
		}
	#endif

	// Gradient
	nrm.x = x1 - x2;
	nrm.y = y1 - y2;
	nrm.z = z1 - z2;

	// Use length as magnitude, divided through maximal length of sqrt(3)
	float mag = length(nrm) / 1.7320508;

	// Transformate with uniformModelScale
	nrm = (uniformModelScale * vec4(nrm,0)).rgb;

	nrm = normalize(nrm);
	return vec4(nrm, mag);
}

/** Returns vec4(normalized normal, magnitude), model space */
vec4 normalOnClassifiedData(vec3 pos, float offset, float valueOffset, float valueScale)
{
	vec3 nrm = vec3(0,0,0);

	// Get values from volume
	float x1 = texture(uniformVolume, vec3(pos.x + offset, pos.y, pos.z)).r;
	float x2 = texture(uniformVolume, vec3(pos.x - offset, pos.y, pos.z)).r;
	float y1 = texture(uniformVolume, vec3(pos.x, pos.y + offset, pos.z)).r;
	float y2 = texture(uniformVolume, vec3(pos.x, pos.y - offset, pos.z)).r;
	float z1 = texture(uniformVolume, vec3(pos.x, pos.y, pos.z + offset)).r;
	float z2 = texture(uniformVolume, vec3(pos.x, pos.y, pos.z - offset)).r;

	#if defined(USE_PREINTEGRATION)
		// Normals calculated from preintegrated alphas
		float originValue = texture(uniformVolume, pos).r * valueScale + valueOffset;
		x1 = texture(uniformColorAlphaPreintegration, vec2(originValue, x1 * valueScale + valueOffset)).a;
		x2 = texture(uniformColorAlphaPreintegration, vec2(originValue, x2 * valueScale + valueOffset)).a;
		y1 = texture(uniformColorAlphaPreintegration, vec2(originValue, y1 * valueScale + valueOffset)).a;
		y2 = texture(uniformColorAlphaPreintegration, vec2(originValue, y2 * valueScale + valueOffset)).a;
		z1 = texture(uniformColorAlphaPreintegration, vec2(originValue, z1 * valueScale + valueOffset)).a;
		z2 = texture(uniformColorAlphaPreintegration, vec2(originValue, z2 * valueScale + valueOffset)).a;
	#else
		// Normals calculated from postinterpolated alphas
		x1 = texture(uniformColorAlpha, x1 * valueScale + valueOffset).a;
		x2 = texture(uniformColorAlpha, x2 * valueScale + valueOffset).a;
		y1 = texture(uniformColorAlpha, y1 * valueScale + valueOffset).a;
		y2 = texture(uniformColorAlpha, y2 * valueScale + valueOffset).a;
		z1 = texture(uniformColorAlpha, z1 * valueScale + valueOffset).a;
		z2 = texture(uniformColorAlpha, z2 * valueScale + valueOffset).a;
	#endif

	// Create good normals at extent
	#if defined(USE_EXTENT_AWARE_NORMALS)
		if(pos.x + offset > volumeExtentMax.x)
		{
			x1 = 0;
		}
		if(pos.y + offset > volumeExtentMax.y)
		{
			y1 = 0;
		}
		if(pos.z + offset > volumeExtentMax.z)
		{
			z1 = 0;
		}
		if(pos.x - offset < volumeExtentMin.x)
		{
			x2 = 0;
		}
		if(pos.y - offset < volumeExtentMin.y)
		{
			y2 = 0;
		}
		if(pos.z - offset < volumeExtentMin.z)
		{
			z2 = 0;
		}
	#endif

	// Gradient
	nrm.x = x1 - x2;
	nrm.y = y1 - y2;
	nrm.z = z1 - z2;

	// Use length as magnitude, divided through maximal length of sqrt(3)
	float mag = length(nrm) / 1.7320508;

	// Transformate with uniformModelScale
	nrm = (uniformModelScale * vec4(nrm,0)).rgb;

	nrm = normalize(nrm);
	return vec4(nrm, mag);
}


/** Returns shaded color */
vec3 calcLighting(vec3 col, vec3 nrm, vec4 mat, vec3 dir, float shadow)
{
	// Brightness of direct light
	float brightness = (1.0 - shadow) * uniformSunColor.a;

	// Lighting (Phong)
	vec3 light = mat.x * uniformAmbientColor + uniformSunColor.rgb * max(dot(sunDirection, nrm),0) * brightness;
	vec3 spec = uniformSunColor.rgb * pow(max(0.0, dot(reflect(-sunDirection, nrm), dir)), mat.w) * brightness;

	// Combine lighting and color
	vec3 result = col * light;

	// Add specular lighting
	result += ((col * mat.z + (1-mat.z) * (col.r+col.g+col.b)/3 ) * mat.y * spec);

	// Some clamping
	result = min(result, 1);

	return result;
}

/** Cast ray into direction of sun */
float castShadowRay(
	vec3 pos,
	vec3 sunDir,
	float shadowThreshold,
	float stepSize,
	float minStepSize,
	float maxStepSize,
	float valueOffset,
	float valueScale,
	float innerIterations,
	vec3 volumeExtentMin,
	vec3 volumeExtentMax)
{
	// Initializations
	float dst = 0;
	float src;
	float currValue = 0;
	float currRayLength = 0;
	float importance;

	#if defined(USE_VOXEL_SPACED_SAMPLING)
		#if defined(USE_ADAPTIVE_SAMPLING)
			stepSize = (minStepSize + maxStepSize)/2 * voxelSpacedStepSize(sunDir);
		#else
			stepSize = stepSize * voxelSpacedStepSize(sunDir);
		#endif
	#endif

	float currStepSize = stepSize;
	vec3 currPos = pos - shadowBias * sunDir * currStepSize;
	float maxRayLength = rayLength(currPos, -sunDir, volumeExtentMin, volumeExtentMax);

	#if defined(USE_PREINTEGRATION)
		vec3 nextPos = currPos;
		float prevValue = texture(uniformVolume, nextPos).r * valueScale + valueOffset;
	#endif

	// Raycasting loop
	while(dst <= shadowThreshold)
	{
		for(int i = 0; i < innerIterations; i++)
		{
			// Update step size if adaptive
			#if defined(USE_ADAPTIVE_SAMPLING)
				importance = texture(uniformImportanceVolume, pos).r;
				#if defined(USE_VOXEL_SPACED_SAMPLING)
					currStepSize = stepSize * (minStepSize * importance + (1-importance) * maxStepSize);
				#else
					currStepSize = (minStepSize * importance + (1-importance) * maxStepSize);
				#endif
			#endif

			// Decide whether to use preintegration or postinterpolation
			#if defined(USE_PREINTEGRATION)
				// Preintegration (use 'currValue' for next position's value)
				nextPos = currPos - sunDir * currStepSize;
				currValue = texture(uniformVolume, nextPos).r;
				currValue = currValue * valueScale + valueOffset;
				src = texture(uniformColorAlphaPreintegration, vec2(prevValue, currValue)).a;
				// Prepare next run
				prevValue = currValue;
				currPos = nextPos;
			#else
				// Postinterpolation
				currValue = texture(uniformVolume, currPos).r;
				currValue = currValue * valueScale + valueOffset;
				src = texture(uniformColorAlpha, currValue).a;
				currPos -= sunDir * currStepSize;
			#endif

			// No real composition, here is something wrong
			dst += (1-pow(1-src, currStepSize/standardStepSize)) * shadowAlphaMultiplier;
			currRayLength += currStepSize;
		}

		// Still inside volume?
		if(currRayLength >= maxRayLength)
		{
			break;
		}
	}

	return min(dst,1);
}

/** Main function */
void main()
{
	// Direction of ray is needed for some initialization
	vec3 dir = normalize(direction);

	// Translate some vectors and calc step size
	float stepSize;
	float minStepSize = 0;
	float maxStepSize = 0;

	#if defined(USE_VOXEL_SPACED_SAMPLING)
		#if defined(USE_ADAPTIVE_SAMPLING)
			minStepSize = uniformBasicInput.x;
			maxStepSize = uniformAdvancedInput.w;
			stepSize = (minStepSize + maxStepSize)/2 * voxelSpacedStepSize(dir);
		#else
			stepSize = uniformBasicInput.x * voxelSpacedStepSize(dir);
		#endif
	#else
		#if defined(USE_ADAPTIVE_SAMPLING)
			minStepSize = uniformBasicInput.x;
			maxStepSize = uniformAdvancedInput.w;
			stepSize = (minStepSize + maxStepSize)/2;
		#else
			stepSize = uniformBasicInput.x;
		#endif
	#endif

	float outerIterations = uniformBasicInput.y;
	float innerIterations = uniformBasicInput.z;
	float alphaThreshold = uniformBasicInput.w;
	float jitteringRangeMultiplier = uniformAdvancedInput.x;
	float normalRangeMultiplier = uniformAdvancedInput.y;
	float fresnelPower = uniformAdvancedInput.z;
	float valueOffset = uniformVolumeValueInformation.x;
	float valueScale = uniformVolumeValueInformation.y;

	// Some variable reservations
	vec4 src;
	vec4 dst = vec4(0,0,0,0);
	float currRayLength = 0;
	vec4 nrm;
	float currValue;
	float importance;
	vec4 advanced;
	float fresnel;
	float reflection;
	vec3 col;
	vec4 mat;
	float shadow;
	vec3 emission = vec3(0,0,0);
	vec3 jitteringOffset = vec3(0,0,0);
	vec3 currJitteringOffset;
	float positionInRay;

	// Further variables...
	float currStepSize = stepSize;	// If no adaptive sampling, currStepSize always stays this value
	float prevValue;
	vec3 vsDir = vec3(0,0,1);
	vec3 vsNrm;
	vec3 currPos = position;
	vec3 nextPos;
	float nrmCalulationOffset = stepSize * normalRangeMultiplier;
	float maxRayLength = rayLength(currPos, dir, volumeExtentMin, volumeExtentMax);

	// Ray jittering
	#if defined(USE_JITTERING)
		float jitter = texture(uniformNoise, gl_FragCoord.xy/NOISE_RES).r;
		#if defined(USE_EXTENT_PRESERVING_JITTERING)
			jitteringOffset = dir * jitteringRangeMultiplier * jitter;
		#else
			// Simple one time jittering
			currPos = currPos + dir * jitteringRangeMultiplier * currStepSize * jitter;
		#endif
	#endif

	// For preintegration one need two values from volume, even on first run
	#if defined(USE_PREINTEGRATION)
		currValue = texture(uniformVolume, currPos).r * valueScale + valueOffset;
		currValue = min(currValue, 1);
	#endif

	// Sample along the ray
	for(int i = 0; i < outerIterations; i++)
	{
		for(int j = 0; j < innerIterations; j++)
		{
			// *** PREPARE SAMPLING ***

			// If extent preserving jittering is used, calculate it here
			#if defined(USE_EXTENT_PRESERVING_JITTERING)
				// Calculate strength of jittering at this point of ray
				positionInRay = currRayLength/maxRayLength;
				if(positionInRay < jitteringFade)
				{
					// Infade
					currJitteringOffset = (positionInRay/jitteringFade) * jitteringOffset * currStepSize;
				}
				else
				{
					currJitteringOffset = jitteringOffset * currStepSize;
				}
			#endif

			// Calculate current step size, if adaptive sampling is used
			#if defined(USE_ADAPTIVE_SAMPLING)
				importance = texture(uniformImportanceVolume, currPos + currJitteringOffset).r;
				#if defined(USE_VOXEL_SPACED_SAMPLING)
					// Relative values as shader input and * stepSize
					currStepSize = stepSize * (minStepSize * importance + (1-importance) * maxStepSize);
				#else
					// Absolut values as shader input
					currStepSize = (minStepSize * importance + (1-importance) * maxStepSize);
				#endif
			#endif

			// Set previous value for preintegration
			#if defined(USE_PREINTEGRATION)
				prevValue = currValue;
			#endif

			// Calculate already next position
			nextPos = currPos+dir*currStepSize;

			// *** GET VALUE FROM VOLUMEN AND THEN COLOR+ALPHA FROM TRANSFERFUNCTION ***

			// Decide whether to use preintegration or postinterpolation
			#if defined(USE_PREINTEGRATION)
				// Preintegration (use 'currValue' for value of next position)
				currValue = texture(uniformVolume, nextPos + currJitteringOffset).r * valueScale + valueOffset;
				currValue = min(currValue, 1);
				src = texture(uniformColorAlphaPreintegration, vec2(prevValue, currValue)).rgba;
			#else
				// Postinterpolation
				currValue = texture(uniformVolume, currPos + currJitteringOffset).r * valueScale + valueOffset;
				currValue = min(currValue, 1);
				src = texture(uniformColorAlpha, currValue).rgba;
			#endif

			// *** EMPTY SPACE SKIPPING ***

			// Primitive empty space skipping
			#if defined(USE_SIMPLE_ESS)
				if(src.a < emptySpaceSkippingTreshold)
				{
					// Prepare for next sample and continue
					currPos = nextPos;
					currRayLength += currStepSize;
					continue;
				}
			#endif

			// *** PREPARATION OF COLOR+ALPHA MANIPULATION ***

			// Calculate normal if necessary
			#if defined(USE_LOCAL_ILLUMINATION) || defined(USE_GRADIENT_ALPHA_MULTIPLIER) || defined(USE_FRESNEL_ALPHA_MULTIPLIER) || defined(USE_REFLECTION_COLOR_MULTIPLIER)
				#if defined(USE_NORMALS_OF_CLASSIFIED_DATA)
					nrm = normalOnClassifiedData(currPos + currJitteringOffset, nrmCalulationOffset, valueOffset, valueScale).rgba;
				#else
					nrm = normalOnRawData(currPos + currJitteringOffset, nrmCalulationOffset).rgba;
				#endif
				vsNrm = (uniformView * vec4(nrm.xyz,1)).xyz;
			#endif

			// Get vec4 out of transferfunction for advanced techniques
			#if defined(USE_GRADIENT_ALPHA_MULTIPLIER) || defined(USE_FRESNEL_ALPHA_MULTIPLIER) || defined(USE_REFLECTION_COLOR_MULTIPLIER) || defined(USE_EMISSION_COLOR_MULTIPLIER)
				#if defined(USE_PREINTEGRATION)
					advanced = texture(uniformAdvancedPreintegration, vec2(prevValue, currValue)).rgba;
				#else
					advanced = texture(uniformAdvanced, currValue).rgba;
				#endif
			#endif

			// *** ALPHA MANIPULATION ***

			// Gradient alpha multiplier
			#if defined(USE_GRADIENT_ALPHA_MULTIPLIER)
				src.a = (advanced.r * nrm.a + (1-advanced.r)) * src.a;
			#endif

			// Fresnel
			#if defined(USE_FRESNEL_ALPHA_MULTIPLIER)
				fresnel = 1-pow(dot(dir, nrm.xyz), fresnelPower);
				src.a = min(1, advanced.g * fresnel + (1-advanced.g)) * src.a;
			#endif

			// Normalize alpha value (src.a may not be = 1)
			src.a = min(src.a, 0.99999f);
			src.a = 1-pow(1-src.a, currStepSize/standardStepSize);

			// *** COLOR MANIPULATION ***

			// Color
			col = (1.0-dst.a) * src.rgb * src.a;

			// Emission
			#if defined(USE_EMISSION_COLOR_MULTIPLIER)
				emission = advanced.a * col;
			#endif

			// Reflection
			#if defined(USE_REFLECTION_COLOR_MULTIPLIER)
				vec3 ref = reflect(vsDir, vsNrm);
				float a = 2.0 * sqrt( ref.x*ref.x + ref.y*ref.y + (ref.z+1.0)*(ref.z+1.0) );
				vec2 reflectionUV = vec2(ref.xy);
				reflectionUV = reflectionUV/a + 0.5;
				reflection = texture(uniformReflection, reflectionUV).r;
				col = (advanced.b * reflection + (1-advanced.b)) * col;
			#endif

			// Modify color with lighting
			#if defined(USE_LOCAL_ILLUMINATION)
				#if defined(USE_PREINTEGRATION)
					mat = texture(uniformAmbientSpecularPreintegration, vec2(prevValue, currValue)).rgba;
				#else
					mat = texture(uniformAmbientSpecular, currValue).rgba;
				#endif
				#if defined(USE_SHADOWS)
					shadow = castShadowRay(currPos + currJitteringOffset,
											sunDirection,
											alphaThreshold,
											#if defined(USE_VOXEL_SPACED_SAMPLING)
												uniformBasicInput.x, // Function needs multiplier, not the absolute value
											#else
												stepSize,
											#endif
											minStepSize,
											maxStepSize,
											valueOffset,
											valueScale,
											innerIterations,
											volumeExtentMin,
											volumeExtentMax);
					col = calcLighting(col, nrm.rgb, mat, dir, shadow);
				#else
					col = calcLighting(col, nrm.rgb, mat, dir, 0.0);
				#endif
			#endif

			// *** FINAL COMPOSITION ***

			// Compositing
			dst.rgb += col + emission;
			dst.a += (1.0-dst.a) * src.a;

			// Prepare for next sample
			currPos = nextPos;
			currRayLength += currStepSize;
		}

		// *** EARLY TERMINATION CONDITION ***

		// Early Ray Termination
		#if defined(USE_ERT)
			if(dst.a > alphaThreshold)
			{
				break;
			}
		#endif

		// Still inside volume?
		if(currRayLength >= maxRayLength)
		{
			break;
		}
	}

	// Final output
	fragmentColor = vec4(dst.rgb, 1);
}
