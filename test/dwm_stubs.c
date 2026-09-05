#include "../dwm.h"

Monitor *selmon;

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
	(void)c;
	(void)x;
	(void)y;
	(void)w;
	(void)h;
	(void)bw;
	(void)interact;
}

void
tile(Monitor *m)
{
	(void)m;
}
