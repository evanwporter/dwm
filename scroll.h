#ifndef SCROLL_H
#define SCROLL_H

#include "dwm.h"

void scroll(Monitor *m);

void scroll_exit(Monitor* m);

void scroll_move(const Arg *arg);

int in_scroll(void);

void scroll_increase_width(const Arg *arg);

void scroll_decrease_width(const Arg *arg);

#endif // SCROLL_H
