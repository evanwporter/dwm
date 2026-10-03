#include "config.h"
#include "dwm.h"

Client *
prevtiled(const Monitor *m, const Client *c)
{
    Client *i = nexttiled(m->clients);
    Client *prev = NULL;

	for (; i && i != c; i = nexttiled(i->next))
        prev = i;

	return prev;
}

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

    /// TODO on switching layouts or exiting this one we set anchor to 
    /// show the focused client on the far left

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

    // Calculate x relative to the monitor x position
    int x = -((m->anchor * m->ww) / 100) + m->wx;

    c = nexttiled(m->clients);

    for (; c; c = nexttiled(c->next)) {
        const int width = (c->scrollw * m->ww) / 100;

        setwindowattr(c, "_PICOM_ANIMATE", 1);

        resize(c, x, m->wy, width - (2 * borderpx), m->wh - (2 * borderpx), borderpx, 0);

        x += width;
    }
}

void
scroll_exit(Monitor *m)
{
    (void)(m);
}

void
scroll_right(void)
{
    /// The window we want to move the focus too
    Client *focus = nexttiled(selmon->sel->next);

    /// If its null we don't do anything
    if (!focus) return;
   
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

    /// The right edge of the window we want want to focus
    const int new_end = new_start + focus->scrollw;

    if (new_end - 100 <= selmon->anchor) {
        // This means that we able to display the full window
        // without needed to adjust the anchor at all
    } else {
        // This moves the anchor the minimum possible. If the window
        // is wider than the monitor then we just place the anchor at
        // start position of the new window
        selmon->anchor = MIN(new_end - 100, new_start);
    }

    focusstackvis(&(Arg){ .i = +1 });

    arrange(selmon);
}

void
scroll_left(void)
{
    /// The window we want to move the focus too
    Client *focus = prevtiled(selmon, selmon->sel);

    /// If its null we don't do anything
    if (!focus) return;
   
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

    if (new_start <= selmon->anchor) {
        // If the new start is before the anchor then we need to adjust the window
        selmon->anchor = new_start;
    } else {
        // Otherwise its fully visible and in view
    }

    focusstackvis(&(Arg){ .i = -1 });

    arrange(selmon);
}

void
scroll_increase_width(const Arg *arg)
{
    selmon->sel->scrollw += 5;

    /// Relatively where on the screen the left edge of window
    /// we want to focus too
    int new_start = 0;

    /// Collect the total width
    Client *c = nexttiled(selmon->clients);
    for (; c; c = nexttiled(c->next)) {
        if (c == selmon->sel) {
            break;
        }

        new_start += c->scrollw;
    }

    if (selmon->sel->scrollw + new_start - selmon->anchor > 100)
        selmon->anchor += 5;

    scroll(selmon);
}

void
scroll_decrease_width(const Arg *arg)
{
    (void)arg;

    selmon->sel->scrollw -= 5;

    int total_width = 0;

    Client *c = nexttiled(selmon->clients);
    for (; c; c = nexttiled(c->next))
        total_width += c->scrollw;

    /*
     * Normally shrinking should not move the viewport.
     *
     * But if shrinking creates empty space on the right side,
     * move the viewport left just enough to keep the content
     * flush with the right edge.
     */
    if (total_width < selmon->anchor + 100)
        selmon->anchor = MAX(0, total_width - 100);

    scroll(selmon);
}

void
scroll_move(const Arg *arg)
{
    switch (arg->i) {
    case 0: scroll_left(); break; /* h / left */
    case 1: focusstackvis(&(Arg){ .i = +1 }); break; /* j / down */
    case 2: focusstackvis(&(Arg){ .i = -1 }); break; /* k / up */
    case 3: scroll_right(); break; /* l / right */
    }
}
