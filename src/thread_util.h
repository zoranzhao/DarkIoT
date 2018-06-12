#ifndef THREAD_UTIL_H
#define THREAD_UTIL_H

#include <stdint.h>
struct sys_thread;
typedef struct sys_thread* sys_thread_t;
/*multithreading APIs*/
sys_thread_t sys_thread_new(const char *name, void *(*function) (void *), void *arg, int stacksize, int prio);

/*Semaphore APIs*/
struct sys_sem;
typedef struct sys_sem* sys_sem_t;
int32_t sys_sem_new(struct sys_sem **sem, uint8_t count);
void sys_sem_signal(struct sys_sem **s);
uint32_t sys_arch_sem_wait(struct sys_sem **s, uint32_t timeout);
void sys_sem_free(struct sys_sem **sem);

#endif
