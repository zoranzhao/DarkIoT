#include "thread_util.h"

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
 
#ifndef SYS_ARCH_TIMEOUT
#define SYS_ARCH_TIMEOUT 1000000
#endif

#ifndef ERR_OK
#define ERR_OK 0
#endif

#ifndef ERR_MEM
#define ERR_MEM -1
#endif

struct sys_sem {
  unsigned int c;
  pthread_condattr_t condattr;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
};


static struct sys_sem * sys_sem_new_internal(uint8_t count)
{
  struct sys_sem *sem;
  sem = (struct sys_sem *)malloc(sizeof(struct sys_sem));
  if (sem != NULL) {
    sem->c = count;
    pthread_condattr_init(&(sem->condattr));
    pthread_cond_init(&(sem->cond), &(sem->condattr));
    pthread_mutex_init(&(sem->mutex), NULL);
  }
  return sem;
}

int32_t sys_sem_new(struct sys_sem **sem, uint8_t count)
{
  *sem = sys_sem_new_internal(count);
  if (*sem == NULL) {
    return ERR_MEM;
  }
  return ERR_OK;
}

static uint32_t cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t timeout);

void
sys_sem_signal(struct sys_sem **s)
{
  struct sys_sem *sem;
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  sem->c++;

  if (sem->c > 1) {
    sem->c = 1;
  }

  pthread_cond_broadcast(&(sem->cond));
  pthread_mutex_unlock(&(sem->mutex));
}


uint32_t
sys_arch_sem_wait(struct sys_sem **s, uint32_t timeout)
{
  uint32_t time_needed = 0;
  struct sys_sem *sem;
  sem = *s;

  pthread_mutex_lock(&(sem->mutex));
  while (sem->c <= 0) {
    if (timeout > 0) {
      time_needed = cond_wait(&(sem->cond), &(sem->mutex), timeout);
      if (time_needed == SYS_ARCH_TIMEOUT) {
        pthread_mutex_unlock(&(sem->mutex));
        return SYS_ARCH_TIMEOUT;
      }
    } else {
      cond_wait(&(sem->cond), &(sem->mutex), 0);
    }
  }
  sem->c--;
  pthread_mutex_unlock(&(sem->mutex));
  return (uint32_t)time_needed;
}

static void
get_monotonic_time(struct timespec *ts)
{
  clock_gettime(CLOCK_MONOTONIC, ts);
}

static uint32_t cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex, uint32_t timeout)
{
  struct timespec rtime1, rtime2, ts;
  int ret;
  if (timeout == 0) {
    pthread_cond_wait(cond, mutex);
    return 0;
  }
  get_monotonic_time(&rtime1);
  ts.tv_sec = rtime1.tv_sec + timeout / 1000L;
  ts.tv_nsec = rtime1.tv_nsec + (timeout % 1000L) * 1000000L;
  if (ts.tv_nsec >= 1000000000L) {
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000L;
  }
  ret = pthread_cond_timedwait(cond, mutex, &ts);
  if (ret == ETIMEDOUT) {
    return SYS_ARCH_TIMEOUT;
  }
  get_monotonic_time(&rtime2);
  ts.tv_sec = rtime2.tv_sec - rtime1.tv_sec;
  ts.tv_nsec = rtime2.tv_nsec - rtime1.tv_nsec;
  if (ts.tv_nsec < 0) {
    ts.tv_sec--;
    ts.tv_nsec += 1000000000L;
  }
  return (uint32_t)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}
