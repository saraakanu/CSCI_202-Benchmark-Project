/* hello_signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t get_alarm = 0;

void handler(int signum)
{ //signal handler
  (void)signum;
  get_alarm = 1;
}

int main(void)
{
  signal(SIGALRM,handler); //register handler to handle SIGALRM
  alarm(5); //Schedule a SIGALRM for 5 seconds
  while (!get_alarm) pause();
  //busy wait for signal to be delivered
  printf("Hello World!\n");
  printf("Turing was right");
  return 0; //never reached
}