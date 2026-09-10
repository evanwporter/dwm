#include "dwm.h"
#include "tree.h"

#include <string.h>

static int
intree(void)
{
    const Layout *layout;

    if (!selmon || !(layout = selmon->lt[selmon->sellt]))
        return 0;

    return layout->arrange == tree || !strcmp(layout->symbol, "[T]");
}

void
key_move(const Arg *arg)
{
    if (intree()) {
        tree_focus_neighbor(arg);
        return;
    }

    switch (arg->i) {
    case 0: setmfact(&(Arg){ .f = -0.05f }); break; /* h / left */
    case 1: focusstackvis(&(Arg){ .i = +1 }); break; /* j / down */
    case 2: focusstackvis(&(Arg){ .i = -1 }); break; /* k / up */
    case 3: setmfact(&(Arg){ .f = +0.05f }); break; /* l / right */
    }
}

void
key_shift_move(const Arg *arg)
{
    if (intree()) {
        treenode_move(arg);
        return;
    }

    switch (arg->i) {
    case 1: focusstackhid(&(Arg){ .i = +1 }); break; /* Shift+j */
    case 2: focusstackhid(&(Arg){ .i = -1 }); break; /* Shift+k */
    }
}
