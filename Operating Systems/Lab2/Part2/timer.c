/* hello_signal.c */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

static volatile sig_atomic_t ticks = 0;
static volatile sig_atomic_t stop  = 0;
static time_t start_time = 0;

static void on_alarm(int signum)
{
    (void)signum;
    ++ticks;
    alarm(1);               /* re-arm for another second */
}

static void on_int(int signum)
{
    (void)signum;
    stop = 1;               /* tell main loop to exit */
}

int main(void)
{
    start_time = time(NULL);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = on_alarm;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("sigaction SIGALRM");
        return 1;
    }

    sa.sa_handler = on_int;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction SIGINT");
        return 1;
    }

    alarm(1);                 /* start 1s periodic alarms */

    while (!stop) pause();    /* wait until Ctrl-C */

    time_t end_time = time(NULL);
    long elapsed = (long)(end_time - start_time);

    printf("\nTotal elapsed (wall-clock) seconds: %ld\n", elapsed);
    printf("Total SIGALRM ticks counted: %d\n", (int)ticks);
    return 0;
}
