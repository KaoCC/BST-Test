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
#include <vector>
#include <algorithm>

#include <CL/cl.h>

#include "profile_timer.hpp"
#include "runtime_exception.hpp"
#include "opencl_factory.hpp"

#define ELEMENT_COUNT      (1024 * 4096 * 128)
#define WORKITEM_COUNT     (1024 * 4096)

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

#ifndef _WIN32
    runtime_exception::register_signal_handlers();
#endif
    
    int* a_list = new int[ELEMENT_COUNT];
    int* b_list = new int[ELEMENT_COUNT];
    for (int i = 0; i < ELEMENT_COUNT; i++) {
        a_list[i] = i;
    }
    
    initialize_cl_environment();
    load_cl_program_from_file("array_copy_kernel.cl", "array_copy_kernel_stride_add");

    cl_mem cl_a_list = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof (int) * ELEMENT_COUNT,
                                      nullptr, nullptr);
    cl_mem cl_b_list = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof (int) * ELEMENT_COUNT,
                                      nullptr, nullptr);
    
    error = clEnqueueWriteBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                 a_list, 0, nullptr, nullptr);

    size_t global_size, local_size;
    int element_count = ELEMENT_COUNT;
    global_size       = WORKITEM_COUNT;
    error = clGetKernelWorkGroupInfo(kernel, device_id,
                                     CL_KERNEL_WORK_GROUP_SIZE,
                                     sizeof (local_size), &local_size, nullptr);
    local_size = std::min(local_size, global_size);

    for (int i = 0; i < 128; ++i) {
        error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_a_list);
        error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_b_list);
        error = clSetKernelArg(kernel, 2, sizeof(int),    &element_count);
        error = clSetKernelArg(kernel, 3, sizeof(int),    &i);
        error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
                                       0, nullptr, nullptr);

    }    
    error = clEnqueueReadBuffer(command_queue, cl_b_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                b_list, 0, nullptr, nullptr);

    load_cl_kernel_from_program("array_copy_kernel_stride_mul");
    error = clEnqueueWriteBuffer(command_queue, cl_a_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                 a_list, 0, nullptr, nullptr);

    element_count = ELEMENT_COUNT;
    global_size   = WORKITEM_COUNT;
    error = clGetKernelWorkGroupInfo(kernel, device_id,
                                     CL_KERNEL_WORK_GROUP_SIZE,
                                     sizeof (local_size), &local_size, nullptr);
    local_size = min(local_size, global_size);

    for (int i = 0; i < 128; ++i) {
        error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_a_list);
        error = clSetKernelArg(kernel, 1, sizeof(cl_mem), &cl_b_list);
        error = clSetKernelArg(kernel, 2, sizeof(int),    &element_count);
        error = clSetKernelArg(kernel, 3, sizeof(int),    &i);
        error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr, &global_size, &local_size,
                                       0, nullptr, nullptr);

    }    
    error = clEnqueueReadBuffer(command_queue, cl_b_list, CL_TRUE, 0, sizeof (int) * ELEMENT_COUNT,
                                b_list, 0, nullptr, nullptr);
    
    // Clean up.
    delete [] a_list;
    delete [] b_list;
    clReleaseMemObject(cl_a_list);
    clReleaseMemObject(cl_b_list);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);
    
    return EXIT_SUCCESS;
}

