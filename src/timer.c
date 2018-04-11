#include "../include/timer.h"


/* シグナルハンドラ(必要らしいので用意する) */
void _TimerHandler( int signo ){}

void SetTimer( Timer *t, int id, double sec ){
  if( SIGRTMIN+id < SIGRTMIN ){
    printf("Please id >= 0\n");
    return;
  }

  if( SIGRTMIN+id > SIGRTMAX ){
    printf("Please id <= %d\n", SIGRTMAX-SIGRTMIN);
    return;
  }

  /* シグナルハンドラを設定する */
  t->sigact.sa_handler = _TimerHandler;

  /* シグナルハンドラの動作を変更する為のフラグ(特に指定しない) */
  t->sigact.sa_flags = 0;

  /* シグナル集合を空にする */
  sigemptyset( &t->sigact.sa_mask );

  /* SIGRTMIN+id に対してシグナルハンドラを設定 */
  sigaction( SIGRTMIN + id, &t->sigact, NULL );
  t->sigeve.sigev_notify = SIGEV_SIGNAL;
  t->sigeve.sigev_signo = SIGRTMIN + id;

  /* タイマーを作成 */
  timer_create( CLOCK_REALTIME, &t->sigeve, &t->timer_id );

  /* タイマーインターバル設定 */
  t->itval.it_value.tv_sec = (int)sec;
  t->itval.it_value.tv_nsec = (sec-(int)sec) * 1E9;
  t->itval.it_interval.tv_sec = (int)sec;
  t->itval.it_interval.tv_nsec = (sec-(int)sec) * 1E9;

  /* タイマーを設定 */
  timer_settime( t->timer_id, 0, &t->itval, NULL );

  /* マスクを設定 */
  sigemptyset( &t->ss );
  sigaddset( &t->ss, SIGRTMIN + id );
  sigprocmask( SIG_BLOCK, &t->ss, 0 );
}

double max = 0;

/* シグナルが送られてくるまで待機 */
int WaitTimerWatch( Timer *t ) {
  int sig;
  timespec_get(&t->end, TIME_UTC);
  if( (double)(t->end.tv_nsec - t->start.tv_nsec)/1E6 > max ) max = (double)(t->end.tv_nsec - t->start.tv_nsec)/1E6;
  if( max > 100 ) max = 0;
  printf("%.1f ms %.1f ms\n", (double)(t->end.tv_nsec - t->start.tv_nsec)/1E6, max );
  sigwait( &t->ss, &sig );
  timespec_get(&t->start, TIME_UTC);
  return sig;
}

#define NODEBUG

int WaitTimer( Timer *t ) {
  int sig;
#ifdef DEBUG
  timespec_get(&t->end, TIME_UTC);
  if( t->itval.it_interval.tv_sec == 0 && t->end.tv_nsec - t->start.tv_nsec > t->itval.it_interval.tv_nsec )
    printf("Timer %d is over %8.2f ms\n", t->sigeve.sigev_signo - SIGRTMIN, (double)( t->end.tv_nsec - t->start.tv_nsec - t->itval.it_interval.tv_nsec ) / 1E6 );
#endif
  sigwait( &t->ss, &sig );
#ifdef DEBUG
  timespec_get(&t->start, TIME_UTC);
#endif
  return sig;
}


void KillTimer( Timer *t ){
  timer_delete( t->timer_id );
}





void CreateThread( Timer *t, void *func(void *), void *until ){
  pthread_create( &t->thread, NULL, func, until );
}

void WaitThread( Timer *t ){
  pthread_join( t->thread, NULL );
  KillTimer( t );
}


struct timespec start, end;
void TimerWatchStart( void ) {
  timespec_get(&start, TIME_UTC);
}

double TimerWatchEnd( void ) {
  timespec_get(&end, TIME_UTC);
  printf("%f ms\n",(double)(end.tv_nsec - start.tv_nsec)/1E6);
  return (double)(end.tv_nsec - start.tv_nsec)/1E6;
}
