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

#include <cstdlib>
#include <cstdio>
#include <random>
#include <vector>

#include <CL/cl.h>

#include "profile_timer.hpp"
#include "runtime_exception.hpp"
#include "opencl_factory.hpp"

using namespace std;

void benchmark_convert_rgb2yuv();
void benchmark_black_scholes();

/*
 * 
 */
int main(int argc, char** argv) {

    runtime_exception::register_signal_handlers();

    initialize_cl_environment();
    load_cl_program_from_file("benchmark_kernel.cl");
    
    /*========== convert_rgb2yuv ==========
     * convert a 1024x1024 RGBA image into YUVA color space.
     * each thread fetch 4 char = 4 bytes for coalescing.
     */
    benchmark_convert_rgb2yuv();
    benchmark_black_scholes();
    
    // Clean up.
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);
    
    return EXIT_SUCCESS;
}

void benchmark_convert_rgb2yuv() {
    
    load_cl_kernel_from_program("convert_rgb2yuv");
    
    int element_count  = 1024 * 1024 * 4;
    int workitem_count = 1024 * 1024;
    
    /* Prepare STL random engine. */
    random_device rand_dev;
    mt19937 rand_gen(rand_dev());
    uniform_int_distribution<int> rand_distribute(0, 255);
    
    char* pixel = new char[element_count];
    for (int i = 0; i < element_count; i++) {
        pixel[i] = (char) rand_distribute(rand_gen);
    }

    cl_mem cl_pixel = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char) * element_count,
                                     nullptr, nullptr);
    
    error = clEnqueueWriteBuffer(command_queue, cl_pixel, CL_TRUE, 0, sizeof(char) * element_count,
                                 pixel, 0, nullptr, nullptr);

    size_t global_size, local_size;
    int pixel_size = element_count;
    global_size    = workitem_count;
    local_size     = 64;

    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_pixel);
    error = clSetKernelArg(kernel, 1, sizeof(int),    &pixel_size);
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
                                   0, nullptr, nullptr);   
    error = clEnqueueReadBuffer(command_queue, cl_pixel, CL_TRUE, 0, sizeof(char) * element_count,
                                pixel, 0, nullptr, nullptr);
    
    // Clean up.
    delete [] pixel;
    clReleaseMemObject(cl_pixel);
    
    free_cl_kernel_from_program("convert_rgb2yuv");
}

void benchmark_black_scholes() {
    
    load_cl_kernel_from_program("black_scholes");
    
    int element_count  = 1024 * 1024;
    int workitem_count = 1024 * 1024;
    
    /* Prepare STL random engine. */
    random_device rand_dev;
    mt19937 rand_gen(rand_dev());
    uniform_real_distribution<double> rand_distribute(0.0, 1.0);
    
    float* s = new float[element_count];
    float* x = new float[element_count];
    float* t = new float[element_count];
    float* r = new float[element_count];
    float* v = new float[element_count];
    float* c = new float[element_count];
    float* p = new float[element_count];
    for (int i = 0; i < element_count; i++) {
        s[i] = (float) rand_distribute(rand_gen) * 1000;
        x[i] = (float) rand_distribute(rand_gen) * 1000;
        t[i] = (float) rand_distribute(rand_gen);
        r[i] = (float) rand_distribute(rand_gen);
        v[i] = (float) rand_distribute(rand_gen);
    }

    cl_mem cl_s = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_x = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_t = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_r = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_v = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    cl_mem cl_p = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * element_count,
                                 nullptr, nullptr);
    
    error = clEnqueueWriteBuffer(command_queue, cl_s, CL_TRUE, 0, sizeof(float) * element_count,
                                 s, 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_x, CL_TRUE, 0, sizeof(float) * element_count,
                                 x, 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_t, CL_TRUE, 0, sizeof(float) * element_count,
                                 t, 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_r, CL_TRUE, 0, sizeof(float) * element_count,
                                 r, 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_v, CL_TRUE, 0, sizeof(float) * element_count,
                                 v, 0, nullptr, nullptr);

    size_t global_size, local_size;
    int task_size = element_count;
    global_size   = workitem_count;
    local_size    = 64;

    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_s);
    error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_x);
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_t);
    error = clSetKernelArg(kernel, 3, sizeof(cl_mem), &cl_r);
    error = clSetKernelArg(kernel, 4, sizeof(cl_mem), &cl_v);
    error = clSetKernelArg(kernel, 5, sizeof(cl_mem), &cl_c);
    error = clSetKernelArg(kernel, 6, sizeof(cl_mem), &cl_p);
    error = clSetKernelArg(kernel, 7, sizeof(int),    &task_size);
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
                                   0, nullptr, nullptr);   
    error = clEnqueueReadBuffer(command_queue, cl_c, CL_TRUE, 0, sizeof(float) * element_count,
                                c, 0, nullptr, nullptr);
    error = clEnqueueReadBuffer(command_queue, cl_p, CL_TRUE, 0, sizeof(float) * element_count,
                                p, 0, nullptr, nullptr);
    
    // Clean up.
    delete [] s;
    delete [] x;
    delete [] t;
    delete [] r;
    delete [] v;
    delete [] c;
    delete [] p;
    clReleaseMemObject(cl_s);
    clReleaseMemObject(cl_x);
    clReleaseMemObject(cl_t);
    clReleaseMemObject(cl_r);
    clReleaseMemObject(cl_v);
    clReleaseMemObject(cl_c);
    clReleaseMemObject(cl_p);
    
    free_cl_kernel_from_program("black_scholes");
}
