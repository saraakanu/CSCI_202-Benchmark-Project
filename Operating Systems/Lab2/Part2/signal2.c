/* hello_signal.c */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t got_alarm = 0;

void handler(int signum) {
    (void)signum;
    got_alarm = 1;     // just set a flag
    alarm(5);          // re-arm for the next 5s tick
}

int main(void) {
    signal(SIGALRM, handler);
    alarm(5);                     // first tick in 5s

    for (;;) {
        while (!got_alarm) pause();  // sleep until signal
        got_alarm = 0;               // clear flag

        puts("Hello World!");
        puts("Turing was right");
        // loop continues; next SIGALRM will set got_alarm again
    }
}
