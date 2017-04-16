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

#include "bst_cpu.hpp"

void bst_build(BSTNode* node_list, const int node_count) {
    // Initialize child pointers.
    for (int i = 0; i < node_count; i++) {
        node_list[i].left  = -1;
        node_list[i].right = -1;
    }
    // Insert all nodes.
    for (int i = 1; i < node_count; i++) {
        int node_insert_id = i;
        int node_visit_id  = 0;
        while (node_visit_id >= 0 && node_visit_id < node_insert_id) {
            BSTNode* node_insert = &node_list[node_insert_id]; 
            BSTNode* node_visit  = &node_list[node_visit_id];
            if (node_insert->data.key < node_visit->data.key) {
                node_visit_id = node_visit->left;
                if (node_visit_id == -1) {
                    node_visit->left = node_insert_id;
                    break;
                }
            } else if (node_insert->data.key > node_visit->data.key) {
                node_visit_id = node_visit->right;
                if (node_visit_id == -1) {
                    node_visit->right = node_insert_id;
                    break;
                }
            } else {
                // Found duplicate, ignore.
                break;
            }
        }
    }
}

void bst_find(const BSTNode* node_list, const int node_count,
              int* find_result,
              const int find_key
) {
    // Initialize visit node index.
    int node_visit_id = 0;
    // Walk down the BST.
    while (node_visit_id >= 0 && node_visit_id < node_count) {
        const BSTNode* node_visit = &node_list[node_visit_id];
        if (find_key < node_visit->data.key) {
            node_visit_id = node_visit->left;
        } else if (find_key > node_visit->data.key) {
            node_visit_id = node_visit->right;
        } else {
            // Found match.
            break;
        }
    }
    
    *find_result = node_visit_id;
}