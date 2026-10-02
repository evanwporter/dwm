#include "../dwm.h"
#include "../config.h"

Monitor *selmon;

static Workspace workspace_storage[NUMTAGS];

Workspace *workspaces[NUMTAGS] = {
    &workspace_storage[0],
    &workspace_storage[1],
    &workspace_storage[2],
    &workspace_storage[3],
    &workspace_storage[4],
    &workspace_storage[5],
    &workspace_storage[6],
    &workspace_storage[7],
    &workspace_storage[8],
    &workspace_storage[9],
};


/* -------------------------------------------------------------------------- */
/* Core                                                                       */
/* -------------------------------------------------------------------------- */

void
focus(Client *c)
{
    if (!c || !c->mon)
        return;

    selmon = c->mon;
    selmon->sel = c;
}

void
restack(Monitor *m)
{
    (void)m;
}

void
arrange(Monitor *m)
{
    (void)m;
}

void
resize(Client *c, int x, int y, int w, int h, int bw, int interact)
{
    (void)interact;

    c->oldx = c->x;
    c->oldy = c->y;
    c->oldw = c->w;
    c->oldh = c->h;
    c->oldbw = c->bw;

    c->x = x;
    c->y = y;
    c->w = w;
    c->h = h;
    c->bw = bw;
}

Client *
nexttiled(Client *c)
{
    for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next)
        ;

    return c;
}

/* -------------------------------------------------------------------------- */
/* Layouts                                                                    */
/* -------------------------------------------------------------------------- */

void
tile(Monitor *m)
{
    (void)m;
}

void
monocle(Monitor *m)
{
    (void)m;
}

// void
// tree(Monitor *m)
// {
//     (void)m;
// }

void
deck(Monitor *m)
{
    (void)m;
}


/* -------------------------------------------------------------------------- */
/* Generic key actions                                                        */
/* -------------------------------------------------------------------------- */

void
spawn(const Arg *arg)
{
    (void)arg;
}

void
togglebar(const Arg *arg)
{
    (void)arg;
}

void
incnmaster(const Arg *arg)
{
    (void)arg;
}

void
zoom(const Arg *arg)
{
    (void)arg;
}

void
view(const Arg *arg)
{
    (void)arg;
}

void
killclient(const Arg *arg)
{
    (void)arg;
}

void
setlayout(const Arg *arg)
{
    (void)arg;
}

void
togglefloating(const Arg *arg)
{
    (void)arg;
}

void
focusmon(const Arg *arg)
{
    (void)arg;
}

void
tagmon(const Arg *arg)
{
    (void)arg;
}

void
quit(const Arg *arg)
{
    (void)arg;
}


/* -------------------------------------------------------------------------- */
/* Workspace actions                                                          */
/* -------------------------------------------------------------------------- */

void
viewworkspace(const Arg *arg)
{
    (void)arg;
}

void
movetoworkspace(const Arg *arg)
{
    (void)arg;
}

void
toggleviewworkspace(const Arg *arg)
{
    (void)arg;
}


/* -------------------------------------------------------------------------- */
/* Tree movement                                                              */
/* -------------------------------------------------------------------------- */

void
key_move(const Arg *arg)
{
    (void)arg;
}

void
key_shift_move(const Arg *arg)
{
    (void)arg;
}

// void
// treenode_move_node(const Arg *arg)
// {
//     (void)arg;
// }

// void
// tree_change_proportion(const Arg *arg)
// {
//     (void)arg;
// }


/* -------------------------------------------------------------------------- */
/* Visibility                                                                 */
/* -------------------------------------------------------------------------- */

void
showall(const Arg *arg)
{
    (void)arg;
}

void
show(const Arg *arg)
{
    (void)arg;
}

void
hide(const Arg *arg)
{
    (void)arg;
}


/* -------------------------------------------------------------------------- */
/* Mouse / button bindings                                                    */
/* -------------------------------------------------------------------------- */

void
togglewin(const Arg *arg)
{
    (void)arg;
}

void
sigstatusbar(const Arg *arg)
{
    (void)arg;
}

void
movemouse(const Arg *arg)
{
    (void)arg;
}

void
resizemouse(const Arg *arg)
{
    (void)arg;
}


// TODO: put somewhere
void
focusstackvis(const Arg *arg)
{
    Client *c = NULL, *i;

    if (!selmon || !selmon->sel)
        return;

    if (arg->i > 0) { // inc move forward
        if (!(c = nexttiled(selmon->sel->next)))
            c = nexttiled(selmon->clients);
    } else {
        for (i = selmon->clients; i != selmon->sel; i = i->next)
            if (!i->isfloating && ISVISIBLE(i))
                c = i;

        if (!c) {
            for (; i; i = i->next)
                if (!i->isfloating && ISVISIBLE(i))
                    c = i;
        }
    }

    if (c)
        focus(c);
}

void
setmfact(const Arg *arg)
{
    (void)arg;
}
