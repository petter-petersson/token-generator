#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "token_generator.h"

pthread_mutex_t test_mutex;
bst_t * test_btree;
int seed_count = 0;

#define NUM_THREADS 10
#define NUM_TOKEN_GEN_PER_THREAD 20

void check_generated_items(bst_node_t * node, void * arg){
  printf("seed: %d token: %s\n", node->key, (char *) node->value);
  free(node->value);
  seed_count++;
}

void * generate_random_tokens(void * args){
  unsigned int * seed;
  char res[25];
  token_generator_t * generator = (token_generator_t *) args;

  printf("generate_random_tokens: %o\n", pthread_self());

  for(int i=0; i < NUM_TOKEN_GEN_PER_THREAD; i++){
    seed = token_generator_threadlocal_seed(generator);
    //we must call rand_r to modify the seed continously
    //*if* we don't call token_generate
    //rand_r(seed);

    token_generate(generator, res, 25);

    pthread_mutex_lock(&test_mutex);
    bst_add(test_btree, (int) *seed, strdup(res));
    pthread_mutex_unlock(&test_mutex);
  }
}

int test_generate_token(test_context_t * tctx){
  int i;
  pthread_t * threads;

  //for verification we save generated seed values in a btree
  pthread_mutex_init(&(test_mutex), NULL);
  test_btree = bst_init();

  token_generator_t generator;
  token_generator_init(&generator);

  threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t));
  if(threads == NULL){
    fprintf(stderr, "no memory, aborting\n");
    abort();
  }

  for(i=0;i < NUM_THREADS; i++){
    if (pthread_create((threads + i), NULL, generate_random_tokens, &generator) != 0) {
      fprintf(stderr, "Failed to create thread\n");
      abort();
    }
  }

  for(i=0;i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }

  token_generator_destroy(&generator);

  bst_traverse(test_btree->root, check_generated_items, NULL);
  printf("unique seed count: %d\n", seed_count);
  check(seed_count == (NUM_THREADS * NUM_TOKEN_GEN_PER_THREAD), tctx);

  pthread_mutex_destroy(&test_mutex);
  bst_free(test_btree);
  return 0;
}

int main() {
  test_context_t context;
  test_context_init(&context);

  test_ctx(test_generate_token, "test_generate_token", &context);

  test_context_show_result(&context);
}
