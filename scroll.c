#ifndef SCROLL_H
#define SCROLL_H

#include "config.h"
#include "dwm.h"

int
in_scroll(void)
{
    const Layout *layout;

    if (!selmon || !(layout = PERWS(selmon)->lt[PERWS(selmon)->sellt]))
        return 0;

    return layout->arrange == scroll || !strcmp(layout->symbol, "[C]");
}

void
scroll(Monitor *m)
{
    Client *c;
    if (!in_scroll()) {
        // TODO: we should not have to set the anchor here
        int n = 0;
        int focused_num = 0;
        Client *c = nexttiled(m->clients);
        for (; c; c = nexttiled(c->next), n++) {
            if (c == m->sel)
                focused_num = n;
        }
        m->anchor = (focused_num - 1) * 33;
    }

    int x = -1 * (m->anchor / 100) * m->ww;

    c = nexttiled(m->clients);

    /// TODO: 33 should be the client scroll width
    const int width = (int)(0.333 * m->ww);

    for (; c; x += width, c = nexttiled(c->next))
        resize(c, x, 0, width, m->wh, borderpx, 0);
}

void
scroll_right(void)
{
    /// The window we want to move the focus too
    Client *focus = nexttiled(selmon->sel);

    /// If its null we don't do anything
    if (focus) return;
   
    /// Relatively where on the screen the left edge of window
    /// we want to focus too
    int new_start = 0;

    /// Collect the total width
    Client *c = nexttiled(selmon->clients);
    for (; c; c = nexttiled(c->next)) {
        if (c == focus) {
            break;
        }

        new_start += c->scrollw;
    }

    const int new_end = new_start + focus->scrollw;

    if (new_end - 100 <= selmon->anchor) {
        // I'm not sure why end - 100 == anchor?
        
        // This means that we able to display the full window
        // without needed to adjust the anchor at all
    } else {
        // This moves the anchor the minimum possible. If the window
        // is wider than the monitor then we just place the anchor at
        // start position of the new window
        selmon->anchor = MIN(new_end - 100, new_start);
    }

    focusstackvis(&(Arg){ .i = +1 });
}

void
scroll_move(const Arg *arg)
{
    switch (arg->i) {
    case 0: setmfact(&(Arg){ .f = -0.05f }); break; /* h / left */
    case 1: focusstackvis(&(Arg){ .i = +1 }); break; /* j / down */
    case 2: focusstackvis(&(Arg){ .i = -1 }); break; /* k / up */
    case 3: scroll_right(); break; /* l / right */
    }
}

#endif // SCROLL_H
