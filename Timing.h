#ifndef TIMING_H_INCLUDED
#define TIMING_H_INCLUDED

extern const double time_limit;     /* time limit in seconds */

void time_initialize();     /* initialize the timer */

void time_pause();          /* pause the timer */
double time_resume();       /* resume the timer; reports time waited */

double time_used();         /* number of seconds used */
double time_left();         /* number of seconds left */

#endif /* TIMING_H_INCLUDED */
