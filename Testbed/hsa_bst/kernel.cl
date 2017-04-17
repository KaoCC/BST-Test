

struct Data {
	int key;
	int value;
};


struct Node {

	struct Data data;


	struct Node* left;
	struct Node* right;


};


__kernel void Find (__global void* node, __global void* out, __global int* key_list) {

	int gid = get_global_id(0);

	if (!node) {
		// error
		return;
	}

	struct Node** address_base = out;
	struct Node* tmp = (struct Node*)(node);


	while (tmp != NULL) {

		if (key_list[gid] > tmp->data.key) {

			tmp = tmp->right;

		} else if (key_list[gid] < tmp->data.key){

			tmp = tmp->left;
		} else {
			break;
		}

	}



	address_base[gid] = tmp;


}



// Should guarantee consistency & data race free !
__kernel void Insert(__global void* rootNode, __global void* dataNode) {

	int gid = get_global_id(0);
	
	
	struct Node* tmp = (struct Node*)(rootNode);
	struct Node** dataNode_list = (struct Node**)(dataNode);

	while (tmp != NULL) {
		
		if (dataNode_list[gid]->data.key > tmp->data.key) {

			tmp = tmp->right;

		} else if (dataNode_list[gid]->data.key < tmp->data.key) {

			tmp = tmp->left;

		} else {
			// Error !!  Duplicate
		}

	}

	// append
	// must ensure sync on nodes.
	// See HSAIL spec for more details (memory order & atomic operations)

	barrier(CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE);

	// ensure insertion order

	tmp = dataNode_list[gid];
	

}










