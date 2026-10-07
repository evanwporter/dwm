#include "tilegap.h"
#include "config.h"

/* Arrange clients as tile(), with an outer gap and gaps between clients. */
void
tilegap(Monitor *m)
{
	unsigned int i, n, h, mw, my, ty, bw;
	const Workspace *workspace = PERWS(m);
	Client *c;
	const unsigned int gap = MIN(gappx, MIN(m->ww, m->wh) / 2);

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);

	if (n == 0)
		return;
	bw = n == 1 ? 0 : borderpx;

	if (n > workspace->nmaster)
		mw = workspace->nmaster ? m->ww * workspace->mfact : 0;
	else
		mw = m->ww - gap;

	for (i = 0, my = ty = gap, c = nexttiled(m->clients);
	     c;
	     c = nexttiled(c->next), i++) {
		if (i < workspace->nmaster) {
			h = (m->wh - my) / (MIN(n, workspace->nmaster) - i) - gap;
			resize(c, m->wx + gap, m->wy + my,
			       mw - 2 * bw - gap, h - 2 * bw, bw, 0);
			if (my + HEIGHT(c) + gap < m->wh)
				my += HEIGHT(c) + gap;
		} else {
			h = (m->wh - ty) / (n - i) - gap;
			resize(c, m->wx + mw + gap, m->wy + ty,
			       m->ww - mw - 2 * bw - 2 * gap, h - 2 * bw, bw, 0);
			if (ty + HEIGHT(c) + gap < m->wh)
				ty += HEIGHT(c) + gap;
		}
	}
}
