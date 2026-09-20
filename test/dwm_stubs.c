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
