#include "config.h"
#include "dwm.h"

void
scroll(Monitor *m)
{
    // TODO: we should not have to set the anchor here
    int n = 0;
    int focused_num = 0;
    Client *c = nexttiled(m->clients);
    for (; c; c = nexttiled(c->next), n++) {
        if (c == m->sel)
            focused_num = n;
    }
    m->anchor = (focused_num - 1) * 33;

    int x = -1 * (m->anchor / 100) * m->ww;

    c = nexttiled(m->clients);

    /// TODO: 33 should be the client scroll width
    const int width = (int)(0.333 * m->ww);

    for (; c; x += width, c = nexttiled(c->next))
        resize(c, x, 0, width, m->mh, borderpx, 0);
}
