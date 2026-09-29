#include "config.h"
#include "dwm.h"

void
scroll(Monitor *m)
{
    int i = -1 * m->anchor * m->ww;

    Client *c = nexttiled(m->clients);

    for (; c; i += c->w, c = nexttiled(c->next))
        resize(c, i, 0, c->w, m->mh, borderpx, 0);


    for (c = nexttiled(m->clients); c; c = nexttiled(c->next))
        // Sets it to the fully window width and height
		resize(c, m->wx, m->wy, m->ww, m->wh, 0, 0);
}
