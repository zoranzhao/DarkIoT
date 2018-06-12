#ifndef THREAD_UTIL_H
#define THREAD_UTIL_H

#include <stdint.h>

struct sys_sem;
int32_t sys_sem_new(struct sys_sem **sem, uint8_t count);
void sys_sem_signal(struct sys_sem **s);
uint32_t sys_arch_sem_wait(struct sys_sem **s, uint32_t timeout);
void sys_sem_free(struct sys_sem **sem);

#endif
