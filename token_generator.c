#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "token_generator.h"
#include "binary_search_tree.h"

void token_generator_free_seeds(bst_node_t * node, void * arg){
  //printf("node key: %d value: %o\n", key_bst_node_t(node), (unsigned) value_bst_node_t(node));
  free(node->value);
}

void token_generator_destroy(token_generator_t * generator){
  pthread_mutex_destroy(&(generator->mutex));
  bst_traverse(btree_token_generator_t(generator)->root, token_generator_free_seeds, NULL);
  bst_free(btree_token_generator_t(generator));
}

void token_generator_init(token_generator_t * generator){
  x_btree_token_generator_t(generator) = bst_init();
  pthread_mutex_init(&(generator->mutex), NULL);
}

unsigned int * token_generator_threadlocal_seed(token_generator_t *generator){
  unsigned int * seed = NULL;
  bst_node_t * current;

  pthread_t id = pthread_self();

  pthread_mutex_lock(&(generator->mutex));
  current = bst_find(btree_token_generator_t(generator)->root, (int) id);
  pthread_mutex_unlock(&(generator->mutex));

  if(current == NULL){
    seed = malloc(sizeof(unsigned int));
    if(seed == NULL){
      printf("%s:%d malloc\n", __FILE__, __LINE__);
      abort();
    }
    (*seed) = time(NULL) ^ (unsigned int) id;

    pthread_mutex_lock(&(generator->mutex));
    bst_add(btree_token_generator_t(generator), (int) id, (void *)seed);
    pthread_mutex_unlock(&(generator->mutex));
  } else {
    seed = (unsigned int *)value_bst_node_t(current);
  }
  return seed;
}

void token_generate(token_generator_t *generator, char * str, const size_t len) {
  unsigned int * rand_state;
  char * buf = str;
  if(len < 1){
    return;
  }
  rand_state = token_generator_threadlocal_seed(generator);

  for(int i = 0; i < len - 1; i++){
    long r1 = rand_r(rand_state) % 3;
    switch(r1){
      case 0:
        *(buf + i) =  'a' + rand_r(rand_state) % 26; 
        break;
      case 1:
        *(buf + i) =  'A' + rand_r(rand_state) % 26; 
        break;
      case 2:
        *(buf + i) =  '0' + rand_r(rand_state) % 10; 
        break;
      default:
        printf("unexpected default\n");
    }
  }
  *(buf + len - 1) = 0;
}
