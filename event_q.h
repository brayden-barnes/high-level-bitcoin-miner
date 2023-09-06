//
// Created by Brayden Barnes on 2022-07-22.
//

#ifndef MINER_EVENT_Q_H
#define MINER_EVENT_Q_H


#include "transactions.h"
#include <pthread.h>

typedef struct event_variables {
    struct event_variables *next;
    trx_t *trx_set;

    //scanf variables for TRX event
    unsigned int e_trx_id;
    char e_trx_payer[32];
    char e_trx_payee[32];
    unsigned int e_trx_amount;
    unsigned int e_trx_fee;

    //scanf variables for BLK event
    unsigned int block_id;
    unsigned int num_trx;
    unsigned int prev_sig;
    unsigned int prev_id;
    unsigned int nonce;

    //scanf variables for MINE event
    unsigned int num_threads;

    //other variables
    char event_type[10];
} event_t;

typedef struct event_list {
    event_t *head;
    event_t *tail;
    pthread_mutex_t lock;
} e_list;

void event_q_add(event_t *e, e_list *list);
extern event_t * event_q_remove(e_list *list);
extern e_list * event_q_init();
extern event_t * new_event();
extern void delete_cond_var();

#endif //MINER_EVENT_Q_H