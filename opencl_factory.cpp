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

#include "opencl_factory.hpp"

cl_int           error;
cl_platform_id   platform_id;
cl_device_id     device_id;
cl_context       context;
cl_command_queue command_queue;
cl_program       program;
cl_kernel        kernel;

void initialize_cl_environment() {
    
    error = clGetPlatformIDs(1, &platform_id, nullptr);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clGetPlatformIDs error: %d\n", error);
    
    error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, nullptr);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clGetDeviceIDs error: %d\n", error);
    
    char device_name[256];
    error = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), device_name, nullptr);
    DEBUG_PRINT(ERR_INFO, "Use device name: %s\n", device_name);
    
    context = clCreateContext(0, 1, &device_id, nullptr, nullptr, &error);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clCreateContext error: %d\n", error);
    
    command_queue = clCreateCommandQueue(context, device_id, 0, &error);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clCreateCommandQueue error: %d\n", error);
    
}

void load_cl_program_from_file(const char* file_name) {

/*    
    ifstream file_stream(file_name);  
    file_stream.seekg(0, ios::end);  
    size_t file_length = file_stream.tellg();  
    char* file_buffer = new char[file_length];  
    file_stream.seekg(0, ios::beg);   
    file_stream.read(file_buffer, file_length);  
    file_stream.close(); */ 
  

	std::fstream file;
	file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
	file.open(file_name, std::fstream::in | std::fstream::binary);

	std::string program_source(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));

	const char* source = program_source.c_str();

  
    program = clCreateProgramWithSource(context, 1, &source, nullptr, &error);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clCreateProgramWithSource error");
    
    error = clBuildProgram(program, 0, nullptr, "-I.", nullptr, nullptr);
    if (error != CL_SUCCESS) {
        size_t error_len;
        char error_buffer[2048];
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 
                              sizeof(error_buffer), error_buffer, &error_len
        );
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clCreateProgramWithSource %s", error_buffer);
    }
    
    kernel = clCreateKernel(program, "bst_find", &error);
    if (error != CL_SUCCESS)
        DEBUG_THROW(ERR_SYSCALL_FAIL, "clCreateKernel error");
}
