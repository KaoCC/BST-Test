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

#include <CL/cl.h>

#include "bst_cpu.hpp"
#include "profile_timer.hpp"
#include "runtime_exception.hpp"
#include "opencl_factory.hpp"

#define BST_NODE_COUNT      (1024 * 4096)

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    runtime_exception::register_signal_handlers();
    
    BSTNode* node_list = new BSTNode[BST_NODE_COUNT];
    for (int i = 0; i < BST_NODE_COUNT; i++) {
        node_list[i].data.key   = rand() % BST_NODE_COUNT;
        node_list[i].data.value = i;
    }
    
    start_timer();
    bst_build(node_list, BST_NODE_COUNT);
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST build time: %lld\n", dump_timer_delta());
    
    int* find_key_list    = new int[BST_NODE_COUNT];
    int* find_result_list = new int[BST_NODE_COUNT];
    
    /**
     * GPU OpenCL random traverse.
     */
    for (int i = 0; i < BST_NODE_COUNT; i++) {
        find_key_list[i] = rand() % BST_NODE_COUNT;
    }
    
    start_timer();
    for (int i = 0; i < BST_NODE_COUNT; i++) {
        bst_find(node_list, BST_NODE_COUNT, &find_result_list[i], find_key_list[i]);
    }
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST CPU search time: %lld\n", dump_timer_delta());
    
    initialize_cl_environment();
    load_cl_program_from_file("bst_kernel.cl");

    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clCreateBuffer">
    cl_mem cl_node_list = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                         sizeof (BSTNode) * BST_NODE_COUNT,
                                         nullptr, nullptr);
    cl_mem cl_find_key_list = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                             sizeof (int) * BST_NODE_COUNT,
                                             nullptr, nullptr);
    cl_mem cl_find_result_list = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                                sizeof (int) * BST_NODE_COUNT,
                                                nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clCreateBuffer time: %lld\n", dump_timer_delta());
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueWriteBuffer">
    error = clEnqueueWriteBuffer(command_queue, cl_node_list, CL_TRUE, 0,
                                 sizeof (BSTNode) * BST_NODE_COUNT, node_list,
                                 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_find_key_list, CL_TRUE, 0,
                                 sizeof (int) * BST_NODE_COUNT, find_key_list,
                                 0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueWriteBuffer time: %lld\n", dump_timer_delta());
    
    int node_count = BST_NODE_COUNT;
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_node_list);
    error = clSetKernelArg(kernel, 1, sizeof(int),    &node_count);
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_find_result_list);
    error = clSetKernelArg(kernel, 3, sizeof(cl_mem), &cl_find_key_list);
    error = clSetKernelArg(kernel, 4, sizeof(int),    &node_count);
    
    size_t global_size, local_size;
    global_size = node_count;
    error = clGetKernelWorkGroupInfo(kernel, device_id, 
                                     CL_KERNEL_WORK_GROUP_SIZE, 
                                     sizeof(local_size), &local_size, nullptr);
    local_size = min(local_size, global_size);
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueNDRangeKernel">
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr,
                                   &global_size, &local_size, 0, nullptr,
                                   nullptr);
    clFinish(command_queue);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueNDRangeKernel time: %lld\n", dump_timer_delta());
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueReadBuffer">
    error = clEnqueueReadBuffer(command_queue, cl_find_result_list, CL_TRUE, 0,
                                sizeof (int) * BST_NODE_COUNT, find_result_list,
                                0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueReadBuffer time: %lld\n", dump_timer_delta());
    
    return 0;

    /**
     * GPU OpenCL no branch traverse.
     */
    for (int i = 0; i < BST_NODE_COUNT; i++) {
        find_key_list[i] = BST_NODE_COUNT / 2;
    }
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueWriteBuffer">
    error = clEnqueueWriteBuffer(command_queue, cl_node_list, CL_TRUE, 0,
                                 sizeof (BSTNode) * BST_NODE_COUNT, node_list,
                                 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_find_key_list, CL_TRUE, 0,
                                 sizeof (int) * BST_NODE_COUNT, find_key_list,
                                 0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueWriteBuffer time: %lld\n", dump_timer_delta());
    
    node_count = BST_NODE_COUNT;
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_node_list);
    error = clSetKernelArg(kernel, 1, sizeof(int),    &node_count);
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_find_result_list);
    error = clSetKernelArg(kernel, 3, sizeof(cl_mem), &cl_find_key_list);
    error = clSetKernelArg(kernel, 4, sizeof(int),    &node_count);
    
    global_size = node_count;
    error = clGetKernelWorkGroupInfo(kernel, device_id, 
                                     CL_KERNEL_WORK_GROUP_SIZE, 
                                     sizeof(local_size), &local_size, nullptr);
    local_size = min(local_size, global_size);
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueNDRangeKernel">
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr,
                                   &global_size, &local_size, 0, nullptr,
                                   nullptr);
    clFinish(command_queue);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueNDRangeKernel time: %lld\n", dump_timer_delta());
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueReadBuffer">
    error = clEnqueueReadBuffer(command_queue, cl_find_result_list, CL_TRUE, 0,
                                sizeof (int) * BST_NODE_COUNT, find_result_list,
                                0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueReadBuffer time: %lld\n", dump_timer_delta());
    
    /**
     * GPU OpenCL warp no branch grouping traverse.
     */
    for (int i = 0; i < BST_NODE_COUNT; i++) {
        find_key_list[i] = (int) ((float) i / local_size * BST_NODE_COUNT);
    }
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueWriteBuffer">
    error = clEnqueueWriteBuffer(command_queue, cl_node_list, CL_TRUE, 0,
                                 sizeof (BSTNode) * BST_NODE_COUNT, node_list,
                                 0, nullptr, nullptr);
    error = clEnqueueWriteBuffer(command_queue, cl_find_key_list, CL_TRUE, 0,
                                 sizeof (int) * BST_NODE_COUNT, find_key_list,
                                 0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueWriteBuffer time: %lld\n", dump_timer_delta());
    
    node_count = BST_NODE_COUNT;
    error = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_node_list);
    error = clSetKernelArg(kernel, 1, sizeof(int),    &node_count);
    error = clSetKernelArg(kernel, 2, sizeof(cl_mem), &cl_find_result_list);
    error = clSetKernelArg(kernel, 3, sizeof(cl_mem), &cl_find_key_list);
    error = clSetKernelArg(kernel, 4, sizeof(int),    &node_count);
    
    global_size = node_count;
    error = clGetKernelWorkGroupInfo(kernel, device_id, 
                                     CL_KERNEL_WORK_GROUP_SIZE, 
                                     sizeof(local_size), &local_size, nullptr);
    local_size = min(local_size, global_size);
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueNDRangeKernel">
    error = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr,
                                   &global_size, &local_size, 0, nullptr,
                                   nullptr);
    clFinish(command_queue);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueNDRangeKernel time: %lld\n", dump_timer_delta());
    
    start_timer();
    // <editor-fold defaultstate="collapsed" desc="clEnqueueReadBuffer">
    error = clEnqueueReadBuffer(command_queue, cl_find_result_list, CL_TRUE, 0,
                                sizeof (int) * BST_NODE_COUNT, find_result_list,
                                0, nullptr, nullptr);
    // </editor-fold>
    stop_timer();
    DEBUG_PRINT(ERR_INFO, "BST GPU clEnqueueReadBuffer time: %lld\n", dump_timer_delta());
    
    // Clean up.
    delete [] node_list;
    clReleaseMemObject(cl_node_list);
    clReleaseMemObject(cl_find_key_list);
    clReleaseMemObject(cl_find_result_list);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);
    clReleaseDevice(device_id);
    
    return EXIT_SUCCESS;
}

