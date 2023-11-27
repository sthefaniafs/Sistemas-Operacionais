#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <linux/futex.h>

#define LOCK_FREE 0
#define LOCK_TAKEN 1
#define LOCK_WAITING 2

struct mutex {
    atomic_uint v;
};

void mutex_init(struct mutex *m)
{
    m->v = LOCK_FREE;
}

void mutex_lock(struct mutex *m)
{
    uint32_t v; //variável de que determina estado do futex

    for (;;) {
   	 v = LOCK_FREE; //inicio setando v como LOCK_FREE, que significa que o mutex está desbloqueado

   	 /*em seguida, comparo &m->v com &v se forem iguais m->v recebe LOCK_TAKEN (1)
   	 e como retorno tenho true, assim o mutex estava livre mas agora ficou bloqueado
   	 para a thread poder ser executada
   	 */
   	 if (atomic_compare_exchange_strong(&m->v, &v, LOCK_TAKEN)) {
   		 break; //sai do loop
   	 }
   	 else if (atomic_compare_exchange(&m->v, LOCK_TAKEN, LOCK_WAITING) || m->v==LOCK_WAITING){
   		futex(&m->v, FUTEX_WAIT, LOCK_WAITING);
    	 }

    }
}

void mutex_unlock(struct mutex *m)
{
    if(atomic_dec(&m->v) != LOCK_TAKEN){
   	 atomic_store(&m->v, LOCK_FREE);
   	 futex(&m->v, FUTEX_WAKE, 1);
        // 1 indica que acorda uma única thread
   	 //nessa alteração só é possível chamar quando há alguém pra acordar
    }

}