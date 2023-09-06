//
// Created by Brayden Barnes on 2022-06-28.
//

#ifndef MINER_NONCE_H
#define MINER_NONCE_H
#include <pthread.h>

typedef struct thread_variables {
    pthread_mutex_t lock;
    unsigned int thread_id;
    unsigned int nonce;
    unsigned int num_threads;
    unsigned int sig;
} th_var;

extern unsigned get_nonce(unsigned partial_sig, int num_threads);

#endif //MINER_NONCE_H
