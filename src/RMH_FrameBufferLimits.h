// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Cyclotronic

/*
 *  RMH_FrameBufferLimits.h
 *
 *  Size of the global frame buffers, and the largest frame the processing chain can hold.
 *  Every frame that comes from a file or a camera is checked against these limits before it is
 *  copied into, or indexed within, the global frame buffers (GlobalObjectsAndVariables.cpp).
 *
 */

#pragma once

// RMH_FrameBufferLimits.h
#ifndef RMH_FrameBufferLimits_H
#define RMH_FrameBufferLimits_H

// Number of elements in each global frame buffer: width * height * bands
#define MaximumFrameDataArraySize                         (1600 * 1200 * 3)

// Largest frame (width * height) the processing chain can hold. The ultra resolution buffers hold the
// frame upscaled 2 x 2 with 3 bands, so 12 elements per source pixel must fit in one frame buffer.
#define _MaximumSupportedFramePixels                      (MaximumFrameDataArraySize / 12)

// Largest single frame dimension accepted (a sanity limit; the pixel limit above is the real constraint)
#define _MaximumSupportedFrameDimension                   4096

// This routine returns true if a frame of the given width and height fits the frame buffers
inline bool RMH_FrameBuffer_IsFrameSizeSupported(unsigned long long FrameWidth, unsigned long long FrameHeight) {

	// Reject empty frames and frames with an unreasonable single dimension
	if (FrameWidth == 0 || FrameHeight == 0 || FrameWidth > _MaximumSupportedFrameDimension || FrameHeight > _MaximumSupportedFrameDimension) {

		return false;

	}

	// Reject frames with more pixels than the processing chain can hold
	return (FrameWidth * FrameHeight) <= (unsigned long long)_MaximumSupportedFramePixels;

}

#endif /* RMH_FrameBufferLimits_H */
