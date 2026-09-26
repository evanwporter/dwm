#include "dwm.h"
#include "config.h"

// TODO: this only works when there's one monitor
/// In conveyor mode this is the position on screen 
/// of the focused node. It can be a number ranging 
/// from 0 to 4.
///     0  1  2  3  4
static int focused_position = 2;

// TODO: use gap from config
static const int gap = 16;

void
conveyor(Monitor *m)
{
    Client *c = nexttiled(m->clients);

    Atom picom_animate = XInternAtom(dpy, "_PICOM_ANIMATE", False);

    /// The position of focused client in the current workspace client stack
    int focused_num = 0;

    /// Number of clients
    int n = 0;

    // TODO: make sure we have something focused (ie m->sel is not null)

    for (; c; c = nexttiled(c->next), n++) {
        if (c == m->sel)
            focused_num = n;
    }

    // Reset c to the beginning of clients
    c = nexttiled(m->clients);
    
    /// Width of a single client in the layout.
    /// The width is set so that two clients can sit side by side
    /// with a distance of `gap` between them.
    const int width = (m->ww - 3 * gap) / 2;

    for (int i = 0; c; c = nexttiled(c->next), i++) {
        /*
         * Equivalent to:
         *
         * xprop -id "$WINDOW_ID" \
         *     -f _PICOM_ANIMATE 32c \
         *     -set _PICOM_ANIMATE 1
         */
        const unsigned long animate = 1;

        XChangeProperty(
            dpy,
            c->win,
            picom_animate,
            XA_CARDINAL,
            32,
            PropModeReplace,
            (const unsigned char *)&animate,
            1
        );

        int x;

        /// Position 0 - 4 of the client in question
        const int position = focused_position + (i - focused_num) * 2;
        
        const int mw = m->ww;

        if (position < 0) {
            x = -width;
        } else if (position == 0) {
            x = (mw / 2) - gap - (3 * width / 2);
        } else if (position == 1) {
            x = gap;
        } else if (position == 2) {
            x = (mw / 2) - (width / 2);
        } else if (position == 3) {
            x = gap + width;
        } else {
            x = (mw / 2) + (width / 2) + gap;
        }

        resize(c, x, 0, width - (borderpx * 2), m->wh - (borderpx * 2), borderpx, 0);
    }
}

// TODO combine the next two funcs and just make them all around cleaner

static void
conveyor_left(int dir)
{
    if (focused_position == 0) {
        return;
    }
    else if (focused_position == 1) {
        focused_position = 2;
    }
    else if (focused_position == 2) {
        focused_position = 2;
        focusstackvis(&(Arg){ .i = dir });
    } else if (focused_position == 3) {
        focused_position = 2;
        focusstackvis(&(Arg){.i = dir});
    } else {
        focused_position = 3;
        focusstackvis(&(Arg){.i = dir});
    }
}

static void
conveyor_right(void)
{
    if (focused_position == 4) {
        return;
    }
    else if (focused_position == 3) {
        focused_position = 2;
    }
    else if (focused_position == 2) {
        focused_position = 2;
        focusstackvis(&(Arg){ .i = 1 });
    } else if (focused_position == 1) {
        focused_position = 2;
        focusstackvis(&(Arg){ .i = 1 });
    } else {
        focused_position = 1;
        focusstackvis(&(Arg){ .i = 1 });
    }
}


void
conveyor_move(const Arg *arg)
{
    switch (arg->i) {
    case 0: break; // setmfact(&(Arg){ .f = -0.05f }); break; /* h / left */
    case 2: conveyor_left(-1); break; /* j / down */
    case 1: conveyor_right(); break; /* k / up */
    case 3: break; // setmfact(&(Arg){ .f = +0.05f }); break; /* l / right */
    }

    arrange(selmon);
}
