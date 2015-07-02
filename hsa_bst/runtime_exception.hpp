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

#ifndef RUNTIMEEXCEPTION_HPP
#define	RUNTIMEEXCEPTION_HPP

#include <exception>
#include <cassert>
#include <csignal>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <string>

#include <execinfo.h>

#define COLOR_BF_RED    "\033[1;31m"
#define COLOR_BF_YELLOW "\033[1;33m"
#define COLOR_BF_CYAN   "\033[1;36m"
#define COLOR_DEFAULT   "\033[0m"

#define DEBUG_PRINT(type, format, ...) \
	do { \
		time_t t = time(NULL); \
		char mbstr[100]; strftime(mbstr, sizeof(mbstr), "%F %T", localtime(&t)); \
		if (ERR_GLOBAL & type) fprintf(stderr, "%s %s:[%04d]:%s(): " format, \
				type == ERR_INFO ? COLOR_BF_CYAN "[INFO]" COLOR_DEFAULT : "[ERROR]", mbstr, \
				__LINE__, __func__, ##__VA_ARGS__); } while (0)

#define DEBUG_THROW(type, format, ...) \
	throw runtime_exception(type, __func__, format, ##__VA_ARGS__)

#define DEBUG_ASSERT_NOT_REACH() \
	DEBUG_PRINT("Forbidden line reached\n"); \
	assert(0);

using namespace std;

enum ExceptionType {
	ERR_GLOBAL            = (0xffffffff),
	ERR_INFO              = (1 << 0),
	ERR_DATA_CURRUPT      = (1 << 1),
	ERR_ARGUMENT_INVALID  = (1 << 2),
	ERR_SYSCALL_FAIL      = (1 << 3),
	ERR_MEMORY_INVALID    = (1 << 4),
	ERR_IO_FAIL           = (1 << 5),
	ERR_DEVICE            = (1 << 6),
	ERR_LIB_DEPEND        = (1 << 7)
};

class runtime_exception : public exception {
public:
	static void register_signal_handlers(void);
	static void signal_handler(int32_t signal_no, siginfo_t* signal_info, 
	                           void* signal_context
	);
	//
	runtime_exception(const ExceptionType type, const char* function,
	                  const char* msg_format, ...);
	static const string get_stack_trace();
	virtual const char* what() const noexcept;
	
private:
	static const int32_t MSG_LENGTH_MAX_ = 1024;
	string message_;
	string stack_trace_;
};

#endif	/* RUNTIMEEXCEPTION_HPP */

