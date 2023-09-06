//
// Created by Brayden Barnes on 2022-06-28.
// pthread setup derived from https://www.youtube.com/watch?v=xoXzp4B8aQk&ab_channel=CodeVault
//

#include "nonce.h"
#include "siggen.h"
#include <stdio.h>
#include <pthread.h>

static void *runner(void *arg) {

    th_var *runner_struct = (th_var *)arg; //convert attribute from (void*) type to (th_var*) type
    pthread_mutex_init(&runner_struct->lock, NULL); //initialize lock
    unsigned int id = runner_struct->thread_id; //first thread to run gets id 0
    runner_struct->thread_id = runner_struct->thread_id + 1; //next thread to run gets id of (prev_id + 1)

    //thread t should check nonce t, t + num_threads, t + 2num_threads, ...
    for (int nonce = id; (nonce + 1) != 0; nonce = nonce + runner_struct->num_threads) {

        printf("Thread %d checking nonce 0x%8.8x\n", id, nonce);

        if ((siggen_int(runner_struct->sig, nonce) & 0xff000000) == 0) {
            pthread_mutex_lock(&runner_struct->lock); //shared nonce variable becomes critical section
            if(nonce < runner_struct->nonce) { //thread updates shared nonce var to the smallest nonce found by thread
                runner_struct->nonce = nonce; //if another thread finds smaller nonce than what is in shared nonce var,
            }                                 //shared nonce var is updated to the new smallest nonce
            pthread_mutex_unlock(&runner_struct->lock); //end critical section
            break;
        }
    }
    return NULL; //runner returns type NULL. struct will have updated nonce.
}

extern unsigned get_nonce(unsigned partial_sig, int num_threads) {

    //initialize num_threads amount of threads, struct to hold vars required for runner
    pthread_t t_id[num_threads];
    th_var thread_attributes;
    thread_attributes.num_threads = num_threads;
    thread_attributes.sig = partial_sig;
    thread_attributes.nonce = 0xffffffff;
    thread_attributes.thread_id = 0;

    //assign threads ids from 0 to num_threads - 1
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&t_id[i], NULL, runner, &thread_attributes);
    }

    //join threads and destroy lock
    for(int j = 0; j < num_threads; j++) {
        pthread_join(t_id[j], NULL);
    }
    pthread_mutex_destroy(&thread_attributes.lock);

    unsigned int nonce = thread_attributes.nonce;
    return nonce;
}