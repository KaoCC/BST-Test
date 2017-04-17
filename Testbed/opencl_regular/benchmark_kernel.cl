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

__kernel void convert_rgb2yuv(__global char* pixel, int size) {

    int gid = get_global_id(0);
 
    if (gid * 4 >= size) return;
    
    float r = (float) pixel[4 * gid + 0];
    float g = (float) pixel[4 * gid + 1];
    float b = (float) pixel[4 * gid + 2];
    float a = (float) pixel[4 * gid + 3];
    pixel[4 * gid + 0] = (char) ( 0.21260f * r + 0.71520f * g + 0.07220f * b);
    pixel[4 * gid + 1] = (char) (-0.09991f * r - 0.33609f * g + 0.43600f * b);
    pixel[4 * gid + 2] = (char) ( 0.61500f * r - 0.55861f * g - 0.05639f * b);
    pixel[4 * gid + 3] = (char) a;
    
    return;
}

float black_scholes_cdn(float x) {
    
    const float a[5] = {0.31938153f, -0.356563782f, 1.781477937f, -1.821255978f, 1.330274429f};
    float l = fabs(x);
    float k = 1.0f / (1.0f + 0.2316419f * l);
    float s = a[0] * k + a[1] * pow(k, 2) + a[2] * pow(k, 3) + a[3] * pow(k, 4) + a[4] * pow(k, 5);
    float w = 1.0f - 1.0f / sqrt(2 * 3.141592653f) * exp(-l * l / 2.0f) * s;
    
    return x < 0.0f ? 1.0f - w : w;
}

__kernel void black_scholes(__global float* s, __global float* x, __global float* t,
                            __global float* r, __global float* v, __global float* c,
                            __global float* p, int size) {
    
    int gid = get_global_id(0);
 
    if (gid >= size) return;
    
    float ls = s[gid];
    float lx = x[gid];
    float lt = t[gid];
    float lr = r[gid];
    float lv = v[gid];
    float d1 = (log(ls / lx) + (lr + lv * lv / 2) * lt) / (lv * sqrt(lt));
    float d2 = d1 - lv * sqrt(lt);
    c[gid] = ls * black_scholes_cdn(d1) - lx * exp(-lr * lt) * black_scholes_cdn(d2);
    p[gid] = ls * exp(-lr * lt) * black_scholes_cdn(-d2) - ls * black_scholes_cdn(-d1);
    
    return;
    
}