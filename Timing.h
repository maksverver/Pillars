#ifndef TIMING_H_INCLUDED
#define TIMING_H_INCLUDED

void time_initialize(double time_limit);     /* initialize the timer */

void time_pause();          /* pause the timer */
double time_resume();       /* resume the timer; reports time waited */

double time_used();         /* number of seconds used */
double time_left();         /* number of seconds left */

#endif /* TIMING_H_INCLUDED */
