#include "config.h"
#include "dwm.h"

void
deck(Monitor* m)
{
    Client *c;
    unsigned int n;

    // Sets n to number of tiled counts
	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

    // Early exit if there are no visible clients
	if (n == 0)
		return;

    unsigned int bw = n == 1 ? 0 : borderpx;

    c = nexttiled(m->clients);

    if (n == 1)
        // No border if only one visible client
        resize(c, m->wx, m->wy, m->ww, m->wh, bw, 0);
    else {
        resize(c, m->wx, m->wy, m->ww / 2, m->wh, bw, 0);

        c = nexttiled(c->next);

        for (; c; c = nexttiled(c->next)) {
            resize(c, m->wx + (m->ww / 2), m->wy, m->ww / 2 - (borderpx / 2), m->wh - (borderpx / 2), bw, 0);
            // c = nexttiled(c->next);
        }
    }
}
