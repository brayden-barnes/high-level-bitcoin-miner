//
// Created by Brayden Barnes on 2022-07-22.
// 
// This file runs a thread that constantly reads input until an END event occurs.
//

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#include "reader.h"
#include "event_q.h"
#include "transactions.h"

#define END_EVENT "END"
#define MINE_EVENT "MINE"
#define TRX_EVENT "TRX"
#define BLK_EVENT "BLK"
#define EPOCH_EVENT "EPOCH"

static pthread_t e_thread;

//Producer
static void * runner(void * arg) {

    e_list *list = (e_list *)arg;
    for(;;){ //while no end event
        event_t *event = new_event();
        scanf ("%s", event->event_type);
        if (!strcmp(event->event_type, END_EVENT)) {
            event_q_add(event, list);
            break;
        } else if (!strcmp(event->event_type, TRX_EVENT)) {
            scanf("%u %s %s %u %u", &event->e_trx_id, event->e_trx_payer, event->e_trx_payee, &event->e_trx_amount, &event->e_trx_fee);
        } else if (!strcmp(event->event_type, BLK_EVENT)) {
            scanf("%u %u %i %u", &event->block_id, &event->prev_id, &event->prev_sig, &event->num_trx);
            event->trx_set = malloc((event->num_trx) * sizeof(trx_t));
            for(int i = 0; i < event->num_trx; i++) {
                scanf("%u %s %s %u %u", &event->trx_set[i].id, event->trx_set[i].payer, event->trx_set[i].payee, &event->trx_set[i].amount, &event->trx_set[i].fee);
            }
            scanf("%i %i", &event->nonce, &event->prev_sig);
        } else if (!strcmp(event->event_type, EPOCH_EVENT)) {
            //no scan required
        } else if (!strcmp(event->event_type, MINE_EVENT)) {
            scanf("%d", &event->num_threads);
        }
        event_q_add(event, list);
    }
    return NULL;
}

void start_reader() {
    e_list * event_list = event_q_init();
    pthread_create(&e_thread, NULL, runner, &event_list);
}

void stop_reader() {
    pthread_join(e_thread, NULL);
}