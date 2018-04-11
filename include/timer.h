/**
 \file
 time.h

 \brief
 擬似時間割り込み関数
*/
#ifndef _Timer
#define _Timer

/* 必要なライブラリ： gcc -lrt */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
  timer_t timer_id;
  struct sigaction sigact;
  struct itimerspec itval;
  struct sigevent sigeve;
  struct timespec start, end;
  sigset_t ss;
  pthread_t thread;
} Timer;


#define TIMERIDMAX SIGRTMAX-SIGRTMIN
#define FPS(fps) (1.0/(double)fps)

void SetTimer( Timer *t, int id, double sec );
int WaitTimerWatch( Timer *t );
int WaitTimer( Timer *t );
void KillTimer( Timer *t );

void CreateThread( Timer *t, void *func(void *), void *until );
void WaitThread( Timer *t );

void TimerWatchStart( void );
double TimerWatchEnd( void );

#endif
