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

#include "bst_type.hpp"

__kernel void bst_find(__global void* node_list, int node_count,
                       __global int* find_result_list,
                       __global int*  find_key_list,
                       int find_thread_count
) {

    int gid             = get_global_id(0);
    int node_current_id = 0;
    
    if (gid >= find_thread_count) return;
    __global struct BSTNode* node_list_cast = node_list;

    while (node_current_id >= 0 && node_current_id < node_count) {
        __global struct BSTNode* node_current = &node_list_cast[node_current_id];

        if (find_key_list[gid] < node_current->data.key) {
            node_current_id = node_current->left;
        } else if (find_key_list[gid] > node_current->data.key) {
            node_current_id = node_current->right;
        } else {
            break;
        }
    }
    
    find_result_list[gid] = node_current_id;
    return;
}

