//
// Created by Brayden Barnes on 2022-07-22.
//
// Defines functions to add or remove event requests
// to/from the event queue
//

#include "event_q.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_cond_t q_not_empty;

extern e_list * event_q_init() {
    return calloc(1, sizeof(e_list));
}

extern event_t * new_event() {
    return calloc(1, sizeof(event_t));
}

void event_q_add(event_t *e, e_list *list) {
    pthread_mutex_init(&list->lock, NULL); //initialize lock
    pthread_cond_init(&q_not_empty, NULL); //initialize cond var

    //critical section
    pthread_mutex_lock(&list->lock);
    if (list->head) {
        list->tail->next = e;
    } else {
        list->head = e;
    }
    list->tail = e;
    e->next = NULL;
    pthread_mutex_unlock(&list->lock);

    printf("Received event %s", e->event_type);
    if(!strcmp(e->event_type, "BLK")) {
        printf(" with ID %u\n", e->block_id);
    }else if(!strcmp(e->event_type, "TRX")) {
        printf(" with ID %u\n", e->e_trx_id);
    }else {
        printf("\n");
    }

    pthread_cond_signal(&q_not_empty);
}

extern event_t * event_q_remove(e_list *list) {
    pthread_cond_wait(&q_not_empty, &list->lock);
    pthread_mutex_lock(&list->lock);
    event_t * e = list->head;
    if (e) {
        list->head = e->next;
    }
    pthread_mutex_unlock(&list->lock);
    return e;
}

extern void delete_cond_var(){
    pthread_cond_destroy(&q_not_empty);
}