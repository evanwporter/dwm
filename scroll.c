#include "config.h"
#include "dwm.h"

void
scroll(Monitor *m)
{

    int n = 0;
    
    int focused_num;

    Client *c = nexttiled(m->clients);

    for (; c; c = nexttiled(c->next), n++) {
        if (c == m->sel)
            focused_num = n;
    }


    m->anchor = (focused_num - 1) * 33;
    int x = -1 * m->anchor * m->ww;

    c = nexttiled(m->clients);

    const int width = 33 * m->ww;

    for (; c; x += c->w, c = nexttiled(c->next))
        resize(c, x, 0, width, m->mh, borderpx, 0);
}
