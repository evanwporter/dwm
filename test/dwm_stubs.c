#include "../dwm.h"
#include "../config.h"

Monitor *selmon;
static Perworkspace perworkspace_storage[LENGTH(workspaces)];
Perworkspace *perworkspaces[LENGTH(workspaces)] = {
    &perworkspace_storage[0],
    &perworkspace_storage[1],
    &perworkspace_storage[2],
    &perworkspace_storage[3],
    &perworkspace_storage[4],
    &perworkspace_storage[5],
    &perworkspace_storage[6],
    &perworkspace_storage[7],
    &perworkspace_storage[8],
};

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

Client *
nexttiled(Client *c)
{
	for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
	return c;
}
