#ifndef INFINITETAGS_H
#define INFINITETAGS_H

#include "dwm.h"

void movecanvas(const Arg *arg);
void manuallymovecanvas(const Arg *arg);
void homecanvas(const Arg *arg);
void save_canvas_positions(Monitor *m);
void restore_canvas_positions(Monitor *m);
void centerwindow(const Arg *arg);

#endif // INFINITETAGS_H
