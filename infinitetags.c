#include "dwm.h"
#include "infinitetags.h"
#include "config.h"

static CanvasOffset *
canvasoffset(Monitor *m)
{
	return &m->canvas[m->selected_workspaces[m->sel_ws] - 1];
}

void
homecanvas(const Arg *arg)
{
	CanvasOffset *canvas = canvasoffset(selmon);
	for (Client *c = selmon->clients; c; c = c->next)
		if (ISVISIBLE(c)) {
			c->x -= canvas->cx;
			c->y -= canvas->cy;
			XMoveWindow(dpy, c->win, c->x, c->y);
		}
	canvas->cx = canvas->cy = 0;
	drawbar(selmon);
	XFlush(dpy);
}

void
movecanvas(const Arg *arg)
{
	int dx = 0, dy = 0;
	if (PERWS(selmon)->lt[PERWS(selmon)->sellt]->arrange != NULL ||
	    (selmon->sel && selmon->sel->isfullscreen))
		return;
	switch (arg->i) {
	case 0: dx = -MOVE_CANVAS_STEP; break;
	case 1: dx = MOVE_CANVAS_STEP; break;
	case 2: dy = -MOVE_CANVAS_STEP; break;
	case 3: dy = MOVE_CANVAS_STEP; break;
	}
	CanvasOffset *canvas = canvasoffset(selmon);
	canvas->cx -= dx;
	canvas->cy -= dy;
	for (Client *c = selmon->clients; c; c = c->next)
		if (ISVISIBLE(c)) {
			c->x -= dx;
			c->y -= dy;
			XMoveWindow(dpy, c->win, c->x, c->y);
		}
	drawbar(selmon);
}

void
manuallymovecanvas(const Arg *arg)
{
	int start_x, start_y, di;
	unsigned int dui;
	Window dummy;
	XEvent ev;
	if (PERWS(selmon)->lt[PERWS(selmon)->sellt]->arrange != NULL ||
	    (selmon->sel && selmon->sel->isfullscreen) ||
	    !XQueryPointer(dpy, root, &dummy, &dummy, &start_x, &start_y, &di, &di, &dui) ||
	    XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
	                 None, cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
		return;
	CanvasOffset *canvas = canvasoffset(selmon);
	do {
		XMaskEvent(dpy, MOUSEMASK | ExposureMask | SubstructureRedirectMask, &ev);
		if (ev.type == MotionNotify) {
			int dx = ev.xmotion.x - start_x, dy = ev.xmotion.y - start_y;
			for (Client *c = selmon->clients; c; c = c->next)
				if (ISVISIBLE(c)) {
					c->x += dx; c->y += dy;
					XMoveWindow(dpy, c->win, c->x, c->y);
				}
			canvas->cx += dx; canvas->cy += dy;
			drawbar(selmon);
			start_x = ev.xmotion.x; start_y = ev.xmotion.y;
		}
	} while (ev.type != ButtonRelease);
	XUngrabPointer(dpy, CurrentTime);
}

void
save_canvas_positions(Monitor *m)
{
	CanvasOffset *canvas = canvasoffset(m);
	canvas->saved_cx = canvas->cx;
	canvas->saved_cy = canvas->cy;
	for (Client *c = m->clients; c; c = c->next)
		if (ISVISIBLE(c)) {
			c->saved_cx = c->x + canvas->cx;
			c->saved_cy = c->y + canvas->cy;
			c->saved_cw = c->w; c->saved_ch = c->h;
			c->was_on_canvas = 1;
		}
}

void
restore_canvas_positions(Monitor *m)
{
	CanvasOffset *canvas = canvasoffset(m);
	canvas->cx = canvas->saved_cx;
	canvas->cy = canvas->saved_cy;
	for (Client *c = m->clients; c; c = c->next)
		if (ISVISIBLE(c) && c->was_on_canvas) {
			c->isfloating = 1;
			c->x = c->saved_cx - canvas->cx;
			c->y = c->saved_cy - canvas->cy;
			c->w = c->saved_cw; c->h = c->saved_ch;
			XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
			configure(c);
		}
	XSync(dpy, False);
}

void
centerwindow(const Arg *arg)
{
	Client *c = arg && arg->v ? (Client *)arg->v : selmon->sel;
	if (!c || PERWS(c->mon)->lt[PERWS(c->mon)->sellt]->arrange != NULL)
		return;
	Monitor *m = c->mon;
	int dx = m->wx + m->ww / 2 - (c->x + WIDTH(c) / 2);
	int dy = m->wy + m->wh / 2 - (c->y + HEIGHT(c) / 2);
	for (Client *other = m->clients; other; other = other->next)
		if (ISVISIBLE(other)) {
			other->x += dx; other->y += dy;
			XMoveWindow(dpy, other->win, other->x, other->y);
		}
	canvasoffset(m)->cx += dx;
	canvasoffset(m)->cy += dy;
	drawbar(m);
}
