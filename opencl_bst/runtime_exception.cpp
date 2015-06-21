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

#include "runtime_exception.hpp"

void runtime_exception::register_signal_handlers() {
	struct sigaction signal_action;
	memset(&signal_action, 0, sizeof(signal_action));
	
	signal_action.sa_sigaction = &runtime_exception::signal_handler;
	signal_action.sa_flags     = SA_SIGINFO;
	
	if (sigaction(SIGSEGV, &signal_action, nullptr) < 0) {
		DEBUG_THROW(ERR_SYSCALL_FAIL, "Fail to register signal action.\n");
	}
    if (sigaction(SIGABRT, &signal_action, nullptr) < 0) {
		DEBUG_THROW(ERR_SYSCALL_FAIL, "Fail to register signal action.\n");
	}
}

void runtime_exception::signal_handler(int32_t signal_no,
                                       siginfo_t* signal_info, 
                                       void* signal_context
) {
	string message = "";
	message += COLOR_BF_RED;
	message += "[SIGNAL] ";
	message += COLOR_DEFAULT;
	switch (signal_no) {
		case SIGABRT:
			message += "SIGABRT";
			break;
		case SIGSEGV:
			message += "SIGSEGV";
			break;
		default:
			message += "Unknown signal";
			break;
	}
	fprintf(stderr, "%s (%d) signal received.\n", message.c_str(), signal_no);
	/**
	 * Build machine context at the point where the signal delivered.
     */
	message = "";
	message += COLOR_BF_YELLOW;
	message += "[Exception Machine Context]\n";
	message += COLOR_DEFAULT;
	fprintf(stderr, "%s", message.c_str());
	switch (signal_no) {
        case SIGABRT:
		case SIGSEGV: {
			const char* registers[] = {"REG_R8",  "REG_R9",  "REG_R10", "REG_R11", "REG_R12", 
			                           "REG_R13", "REG_R14", "REG_R15", "REG_RDI", "REG_RSI",
			                           "REG_RBP", "REG_RBX", "REG_RDX", "REG_RAX", "REG_RCX",
			                           "REG_RSP", "REG_RIP"
			};
			ucontext_t* fault = (ucontext_t*) signal_context;
			for (uint32_t i = REG_R8; i < REG_RIP; i += 2) {
				fprintf(stderr, "%-7s: 0x%016llx  %-7s: 0x%016llx\n",
				        registers[i - REG_R8],     fault->uc_mcontext.gregs[i],
						registers[i + 1 - REG_R8], fault->uc_mcontext.gregs[i + 1]
				);
			}
			fprintf(stderr, "%-7s: 0x%016llx\n",
			        registers[REG_RIP], fault->uc_mcontext.gregs[REG_RIP]
			);
		} break;
		default:
			message += "Unknown signal";
			break;
	}
	message = "";
	message += COLOR_BF_YELLOW;
	message += "[Back-Stack Trace]\n";
	message += COLOR_DEFAULT;
	fprintf(stderr, "%s", message.c_str());
	/**
	 * Build stack trace information.
     */
	string stack_trace = get_stack_trace();
	fprintf(stderr, "%s", stack_trace.c_str());
	
	exit(EXIT_FAILURE);
}

runtime_exception::runtime_exception(const ExceptionType type,
                                     const char* function,
                                     const char* msg_format, ...
) {
    message_ = "";
    message_ += COLOR_BF_RED;
    switch (type) {
        case ERR_ARGUMENT_INVALID:
            message_ += "[ERR_ARGUMENT_INVALID]";
            break;
        case ERR_DATA_CURRUPT:
            message_ += "[ERR_DATA_CURRUPT]";
            break;
        case ERR_DEVICE:
            message_ += "[ERR_DEVICE]";
            break;
        case ERR_IO_FAIL:
            message_ += "[ERR_IO_FAIL]";
            break;
        default:
            message_ += "[UNKNOWN_ERROR]";
            break;
    }
    message_ += COLOR_DEFAULT;
    message_ += " ";
    /**
     * Build time stamp and function location.
     */
    time_t msg_time = time(NULL);
    char msg_time_formatted[runtime_exception::MSG_LENGTH_MAX_];
    strftime(msg_time_formatted, sizeof (msg_time_formatted), "%F %T", localtime(&msg_time));
    message_ += msg_time_formatted;
    message_ += " ";
    message_ += function;
    message_ += "(): ";
    /**
     * Build user message.
     */
    va_list msg_arg;
    char* msg_arg_formatted = new char[runtime_exception::MSG_LENGTH_MAX_];
    // Print formatted strings into allocated space.
    va_start(msg_arg, msg_format);
    vsprintf(msg_arg_formatted, msg_format, msg_arg);
    va_end(msg_arg);
    // Append message content.
    message_ += msg_arg_formatted;
    delete [] msg_arg_formatted;
    /**
     * Build stack trace information.
     */
    stack_trace_ = get_stack_trace();
}

const string runtime_exception::get_stack_trace() {

    string stack_trace = "";
    void* stack_address_list[128];
    uint32_t stack_address_list_length = backtrace(stack_address_list,
                                         sizeof (stack_address_list) / sizeof (void*)
    );
	
	if (stack_address_list_length == 0) return "";
	char** stack_symbol_list = backtrace_symbols(stack_address_list,
                                                 stack_address_list_length
    );

	for (uint32_t i = 1; i < stack_address_list_length; i++) {
        // Stack frame address.
        void* stack_address = stack_address_list[i];
        // Stack frame symbol object file name.
        size_t stack_file_ptr = 0;
        while (stack_symbol_list[i][stack_file_ptr] != '(') stack_file_ptr++;
        // Issue stack frame address to source symbol information conversion.
        char addr2line_command[1024] = {0};
        char addr2line_result[1024] = {0};
        sprintf(addr2line_command, "addr2line %p -s -e %.*s",
            stack_address, (int32_t) stack_file_ptr, stack_symbol_list[i]
        );
		FILE* addr2line_fp = popen(addr2line_command, "r");
        while (fgets(addr2line_result, sizeof (addr2line_result), addr2line_fp)) {}
        addr2line_result[strlen(addr2line_result) - 1] = '\0';
        stack_trace += "[" + to_string(i) + "] ";
        stack_trace += stack_symbol_list[i];
        stack_trace += " ";
        stack_trace += addr2line_result;
        stack_trace += "\n";
	}
	// Cleanup.
	delete [] stack_symbol_list;
	
	return stack_trace;
}

const char* runtime_exception::what() const noexcept {
    string message = "";
    message += message_;
    message += COLOR_BF_YELLOW;
    message += "[Back-Stack Trace]\n";
    message += COLOR_DEFAULT;
    message += stack_trace_;
    fprintf(stderr, "%s\n", message.c_str());
    return message.c_str();
}