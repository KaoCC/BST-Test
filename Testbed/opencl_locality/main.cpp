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

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <vector>

#include <CL/cl.h>

#include "profile_timer.hpp"
#include "runtime_exception.hpp"
#include "opencl_factory.hpp"


#define ELEMENT_COUNT      (64 * 4096 * 128 * 32)



using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

#ifndef _WIN32
    runtime_exception::register_signal_handlers();
#endif
    
    int* a_list = new int[ELEMENT_COUNT];
    
    initialize_cl_environment();
    load_cl_program_from_file("locality_kernel.cl", "horizontal_kernel");

    cl_mem cl_a_list = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof (int) * ELEMENT_COUNT,
                                      nullptr, nullptr);
    
    // Test 1: horizontal.
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        a_list[i] = i;
    }
    
    error = clEnqueueWriteBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                 a_list, 0, nullptr, nullptr);

    size_t global_size, local_size;
    int element_count = ELEMENT_COUNT;
    global_size       = 64 * 4096 * 128;
    local_size        = 64;

    for (int i = 1; i <= 32; i++) {
        error = clSetKernelArg(kernel, 0, sizeof (cl_mem), &cl_a_list);
        error = clSetKernelArg(kernel, 1, sizeof (int), &i);
        error = clSetKernelArg(kernel, 2, sizeof (int), &element_count);
        error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
            0, nullptr, nullptr);

        error = clEnqueueReadBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                    a_list, 0, nullptr, nullptr);
    }

    // Test 2: vertical.
    load_cl_kernel_from_program("vertical_kernel");
    
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        a_list[i] = i;
    }
    
    error = clEnqueueWriteBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                 a_list, 0, nullptr, nullptr);

    element_count = 64 * 4096;
    global_size   = 1;
    local_size    = 1;

    error = clSetKernelArg(kernel, 0, sizeof (cl_mem), &cl_a_list);
    error = clSetKernelArg(kernel, 1, sizeof (int), &element_count);
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
            0, nullptr, nullptr);

    error = clEnqueueReadBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                a_list, 0, nullptr, nullptr);
    
    // Clean up.
    delete [] a_list;

    clReleaseMemObject(cl_a_list);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);
    
    return EXIT_SUCCESS;
}

