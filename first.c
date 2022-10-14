#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct node{
	int tag;
	int rec;
	struct node* next;
}node;

node** cache;

int notPow2(int check){
	return ceil(log2(check)) != floor(log2(check));
}

int mask(long mem, int bits, int before_bits){
	mem = mem>>before_bits;
	long mask = (pow(2, bits) - 1);
	return mem & mask;
}

void insert(int tag, int index, int assoc){
	node* new = (node*)malloc(sizeof(node));
	new->tag = tag;
	new->rec = 0;
	new->next = cache[index];
	cache[index] = new;
	node* ptr = new->next;
	node* prev = new;
	while(ptr != NULL){
		ptr->rec++;
		if(ptr->rec == assoc){
			prev->next = NULL;
			free(ptr);
			break;
		}
		prev = ptr;
		ptr = ptr->next;
	}
}

int main(int argc, char* argv[argc + 1]){
	int cache_size = atoi(argv[1]);
	int block_size = atoi(argv[3]);
	int assoc = 0;
	int sets = 0;
	char associtivity[15];
	if(strcmp(argv[2], "direct") == 0){
		assoc = 1;
		sets = cache_size/block_size;
	}
	else if(strcmp(argv[2], "assoc") == 0){
		sets = 1;
		assoc = cache_size/block_size;
	}
	else{
		strncpy(associtivity, &argv[2][6], strlen(argv[2]) - 6);
		assoc = atoi(associtivity);
		sets = cache_size/(assoc*block_size);
	}
	FILE* in_file = fopen(argv[4], "r");
	if(notPow2(cache_size) || notPow2(assoc) || notPow2(block_size) || in_file == NULL){
		printf("error\n");
		return EXIT_SUCCESS;
	}
	int offset_bits = log2(block_size);
	int index_bits = log2(sets);
	int tag_bits = 48 - offset_bits - index_bits;
	
	char comm;
	long mem;
	int reads = 0;
	int writes = 0;
	int hits = 0;
	int misses = 0;
	cache = (node**)malloc(sizeof(node) * sets);
	for(int i = 0; i < sets; i++){
		cache[i] = NULL;
	}
	while(fscanf(in_file, "%c %lx\n", &comm, &mem) != EOF){
		int new_tag = mask(mem, tag_bits, (48 - tag_bits));
		int index = mask(mem, index_bits, offset_bits);
		if(comm == 'R'){
			node* ptr = cache[index];
			while(ptr != NULL){
				if(ptr->tag == new_tag){
					hits++;
					break;
				}
				ptr = ptr->next;
			}
			if(ptr == NULL){
				insert(new_tag, index, assoc);
				reads++;
				misses++;
			}
		}
		else{
			node* ptr = cache[index];
			while(ptr != NULL){
				if(ptr->tag == new_tag){
					hits++;
					writes++;
					break;
				}
				ptr = ptr->next;
			}
			if(ptr == NULL){
				insert(new_tag, index, assoc);
				reads++;
				misses++;
				writes++;
			}
		}
	}
	node* ptr;
	node* prev;
	for(int i = 0; i < sets; i++){
		ptr = cache[i];
		while(ptr != NULL){
		prev = ptr;
		ptr = ptr->next;
		free(prev);
		}
	}
	printf("memread:%d\n", reads);
	printf("memwrite:%d\n", writes);
	printf("cachehit:%d\n", hits);
	printf("cachemiss:%d\n", misses);
	return EXIT_SUCCESS;
}
