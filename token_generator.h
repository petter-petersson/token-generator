#ifndef __TOKEN_GENERATOR_H
#define __TOKEN_GENERATOR_H

#include <pthread.h>
#include "binary_search_tree.h"

typedef struct token_generator_s {
  int num_threads;
  pthread_mutex_t mutex;
  bst_t * btree;
} token_generator_t;

#ifdef DEBUG
#define deref_token_generator_t(_n) (assert((_n)!=0), (_n))
#else
#define deref_token_generator_t(_n) (_n)
#endif
#define x_btree_token_generator_t(_n) (deref_token_generator_t(_n)->btree)
#define btree_token_generator_t(_n) ((void)0, x_btree_token_generator_t(_n))

void token_generator_init(token_generator_t * generator);
void token_generator_destroy(token_generator_t * generator);

void token_generator_free_seeds(bst_node_t * node, void * arg);
unsigned int * token_generator_threadlocal_seed(token_generator_t *generator);
void token_generate(token_generator_t * generator, char * str, const size_t len);

#endif
