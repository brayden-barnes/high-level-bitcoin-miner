//
// This C project was written by Brayden Barnes in fulfillment of the assignment descriptions of 
// Dalhousie University's CSCI3120 Operating Systems course.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transactions.h"
#include "mempool.h"
#include "siggen.h"
#include "nonce.h"
#include "reader.h"
#include "event_q.h"

#define END_EVENT "END"
#define MINE_EVENT "MINE"
#define TRX_EVENT "TRX"
#define BLK_EVENT "BLK"
#define EPOCH_EVENT "EPOCH"

static void trx_print(char *prefix, trx_t *trx) {
    printf("%s%d %s %s %d %d\n", prefix, trx->id, trx->payer,
           trx->payee, trx->amount, trx->fee);
}

static void trx_age(trx_t *trx, int prio) {
    printf("Aging transaction (%d):", prio);
    trx_print("", trx);
}

int main() {
    char buffer[10]; // Assume input will be correct.
    trx_list_t *selected = transaction_list();
    e_list  *e_list = event_q_init(); //initialize event queue
    unsigned int prev_id = 0;
    unsigned int prev_sig = 0;

    mempool_init();
    start_reader(); //begin reader thread

    //Consumer
    for (;;) {

        //event_q_remove() fetches event from queue to be consumed
        event_t *e = event_q_remove(e_list);

        if (!strcmp(e->event_type, END_EVENT)) {
            break;
        } else if (!strcmp(e->event_type, EPOCH_EVENT)) {
            mempool_age(trx_age);
        } else if (!strcmp(e->event_type, TRX_EVENT)) {
            trx_t * trx = transaction_new();

            //trx_read() replacement
            trx->id = e->e_trx_id;
            strcpy(trx->payer, e->e_trx_payer);
            strcpy(trx->payee, e->e_trx_payee);
            trx->fee = e->e_trx_fee;
            trx->amount = e->e_trx_amount;

            trx_print("Adding transaction: ", trx);
            mempool_add(trx);
        } else if (!strcmp(e->event_type, BLK_EVENT)) {
            unsigned int block_id = e->block_id;
            unsigned int num_trx = e->num_trx;
            trx_t *trx = NULL;
            prev_id = e->prev_id;
            prev_sig = e->prev_sig;

            for (int i = 0; i < num_trx; i++) {
                trx->id = e->trx_set->id;
                strcpy(trx->payer, e->trx_set->payer);
                strcpy(trx->payee, e->trx_set->payee);
                trx->fee = e->trx_set->fee;
                trx->amount = e->trx_set->amount;

                trx_t *t = mempool_remove(trx->id);
                if (t != NULL) {
                    trx_print("Removing transaction: ", t);
                    transaction_delete(t);
                }
            }

            unsigned int nonce = e->nonce;
            prev_id = block_id;
            prev_sig = e->prev_sig;

        } else if (!strcmp(e->event_type, MINE_EVENT)) {
            unsigned int id = prev_id + 1;
            unsigned int num_trx = 0;
            unsigned int available = 256 - 24; // 24 bytes in block without transactions
            unsigned int num_threads;

            for (trx_t *t = mempool_select(available); t != NULL; t = mempool_select(available)) {
                available -= transaction_size(t);
                num_trx++;
                transaction_append(selected, t);
            }

            unsigned int sig = siggen_new();
            sig = siggen_int(sig, id);
            sig = siggen_int(sig, prev_id);
            sig = siggen_int(sig, prev_sig);
            sig = siggen_int(sig, num_trx);
            printf("Block mined: %d %d 0x%8.8x %d\n", id, prev_id, prev_sig, num_trx);

            for (int i = 0; i < num_trx; i++) {
                trx_t *t = transaction_pop(selected);
                sig = siggen_int(sig, t->id);
                sig = siggen_string(sig, t->payer);
                sig = siggen_string(sig, t->payee);
                sig = siggen_int(sig, t->amount);
                sig = siggen_int(sig, t->fee);
                trx_print("", t);
                transaction_delete(t);
            }

            num_threads = e->num_threads;
            unsigned int nonce = get_nonce(sig, num_threads);
            sig = siggen_int(sig, nonce);
            printf("0x%8.8x 0x%8.8x\n", nonce, sig);
            prev_id = id;
            prev_sig = sig;
        }
        free(e);
    }
    stop_reader();
    delete_cond_var();
    return 0;
}

