/** 
 * Copyright (c) 2015 VCO Lab CSIE department of National Taiwan University All
 * rights reserved.
 * 
 * This file is part of OpenCL Binary Search Tree for HSA comparison
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "profile_timer.hpp"

#ifndef _WIN32

struct timeval tv1, tv2;
long long int delta;

void start_timer() {
    // Get current time.
    gettimeofday(&tv1, nullptr);
}

void stop_timer() {
    // Get current time.
    gettimeofday(&tv2, nullptr);
    // Compute delta time.
    delta = (tv2.tv_sec  - tv1.tv_sec) * 1000000 +
            (tv2.tv_usec - tv1.tv_usec);
}

long long int dump_timer_delta() {
    return delta;
}


#else

#include <chrono>

// For Windows using C++ Chronos

static std::chrono::high_resolution_clock::time_point  gTimePoint[2];
static std::chrono::duration<long long int, std::nano> gDuration;
//static std::chrono::duration<long long int> gDurationInt;

void start_timer() {
	gTimePoint[0] = std::chrono::high_resolution_clock::now();
}

void stop_timer() {
	gTimePoint[1] = std::chrono::high_resolution_clock::now();
	gDuration = gTimePoint[1] - gTimePoint[0];
}


//check this one 
long long int dump_timer_delta() {
	auto durationInt{ std::chrono::duration_cast<std::chrono::microseconds>(gDuration) };
	return durationInt.count();
}


#endif

