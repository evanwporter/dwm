#ifndef DWM_H
#define DWM_H

#include <signal.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
#include <X11/Xft/Xft.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/Xlib-xcb.h>
#include <xcb/res.h>
#ifdef __OpenBSD__
#include <sys/sysctl.h>
#include <kvm.h>
#endif /* __OpenBSD */

#include "drw.h"
#include "util.h"

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                               * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->workspace == C->mon->selected_workspaces[C->mon->sel_ws]))
#define HIDDEN(C)               (getstate((C)->win) == IconicState)
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define WORKSPACEMASK           ((1 << LENGTH(workspaces)) - 1)
#define WORKSPACEBIT(W)         (1U << ((W) - 1))
#define PERWORKSPACE(M)         (perworkspaces[(M)->selected_workspaces[(M)->sel_ws] - 1])
#define PERWS(M)                PERWORKSPACE(M)

/// Check workspace bounds
///    1 <= W <= LENGTH(workspaces)
#define CHECK_WS_BOUNDS(W)      (1 <= (W) && (W) <= LENGTH(workspaces))
#define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)

#define MWM_HINTS_FLAGS_FIELD       0
#define MWM_HINTS_DECORATIONS_FIELD 2
#define MWM_HINTS_DECORATIONS       (1 << 1)
#define MWM_DECOR_ALL               (1 << 0)
#define MWM_DECOR_BORDER            (1 << 1)
#define MWM_DECOR_TITLE             (1 << 3)

#define SYSTEM_TRAY_REQUEST_DOCK    0
/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_FOCUS_IN             4
#define XEMBED_MODALITY_ON         10
#define XEMBED_MAPPED              (1 << 0)
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2
#define VERSION_MAJOR               0
#define VERSION_MINOR               0
#define XEMBED_EMBEDDED_VERSION (VERSION_MAJOR << 16) | VERSION_MINOR

/* enums */
enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel, SchemeStatus, SchemeHid }; /* color schemes */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetSystemTray, NetSystemTrayOP, NetSystemTrayOrientation, NetSystemTrayOrientationHorz,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetClientInfo, NetLast }; /* EWMH atoms */
enum { Manager, Xembed, XembedInfo, XLast }; /* Xembed atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
       ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

typedef struct Monitor Monitor;
typedef struct TreeNode TreeNode;

/// In the context of dwm the Client represents a window that is managed by the
/// window manager. A set of clients is represented in form of a linked list.
typedef struct Client Client;
struct Client {
    /// The name holds the window title.
	char name[256];

    /// The mina and maxa represents the minimum and maximum aspect ratios as per size hints.
	float mina, maxa;

	/// The client x, y coordinates and size (width, height).
	int x, y, w, h;

	int oldx, oldy, oldw, oldh;

    /* These variables are all in relation to size hints.
	 *    basew - base width
	 *    baseh - base height
	 *    incw - width increment
	 *    inch - height increment
	 *    minw - minimum width
	 *    minh - minimum height
	 *    maxw - maximum width
	 *    maxh - maximum height
	 *    hintsvalid - flag indicating whether size hints need to be refreshed
	 */
	int basew, baseh, incw, inch, maxw, maxh, minw, minh, hintsvalid;

	int bw, oldbw;

	/* This represents the tags the client is shown on. This is a bitmask where each bit
	 * represents whether the client is shown on that tag.
	 *
	 * As an example consider the hexadecimal value of 0x51 (decimal 81) which has a binary
	 * value of:
	 *    001010001  - bitmask
	 *    987654321  - tags
	 *
	 * This would mean that the client is shown on tags 1, 5 and 7.
	 */
	unsigned int tags;

    /// The workspace the client is attached too
    unsigned int workspace;

	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen, isterminal, noswallow;
	pid_t pid;

	/// The next client in the client list, which is a linked list. The client list controls the
	/// order in which clients are tiled.
	Client *next;

	/* The leaf node representing this client in its monitor's tree layout. */
	TreeNode *node;

	/* The next client in the stacking order list, which is also a linked list. The stacking
	 * order indicates which window is on top of others as well as the order in which clients
	 * had focus. */
	Client *snext;

	Client *swallowing;

    /// The monitor this client belongs to.
	Monitor *mon;

    /// The managed window that this client represents.
	Window win;
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	const char *symbol;
	void (*arrange)(Monitor *);
} Layout;

struct Monitor {
    /* This represents the monitor number, or the monitor index if you wish. */
	int num;

	/* The by variable defines the bar windows position on the y axis and this is set in the
	 * updatebarpos function. */
	int by;               /* bar geometry */
	int btw;              /* width of tasks portion of bar */
	int bt;               /* number of tasks */

	/* These variables represents the position and dimensions of the monitor.
	 *    mx - monitor position on the x-axis
	 *    my - monitor position on the y-axis
	 *    mw - the monitor's width
	 *    mh - the monitor's height
	 */
	int mx, my, mw, mh;

	/* These variables represents the position and dimensions of the window area, as in the part
	 * of the monitor where windows are tiled. This is the space of the monitor excluding the
	 * bar window. These are set in the updatebarpos function.
	 *    wx - window area position on the x-axis
	 *    wy - window area position on the y-axis
	 *    ww - the window area's width
	 *    wh - the window area's height
	 */
	int wx, wy, ww, wh;

	/* The seltags variable is either 0 or 1 and represents the currently selected tagset.
	 *
	 * This allows for a clever mechanism where one can easily flip between the current and
	 * previous tagset by simply flipping the value of seltags:
	 *
	 *    selmon->seltags ^= 1;
	 *
	 * For this reason when referring to the selected tags for a monitor you will often find
	 * these kind of patterns:
	 *
	 *    m->tagset[m->seltags]
	 *    selmon->tagset[selmon->seltags]
	 *    c->mon->tagset[c->mon->seltags]
	 *
	 * In principle this could just have been defined as two variables for the monitor.
	 *
	 *    m->tags
	 *    m->prevtags
	 *
	 * which would make the above patterns slightly easier to read, i.e.
	 *
	 *    m->tags
	 *    selmon->tags
	 *    c->mon->tags
	 *
	 * The benefit of using this mechanism, however, is that we save on a single line of code
	 * in the view function when the argument is 0 and we toggle back to the previous view.
	 */
	unsigned int seltags;

	/* This array holds the previously and currently viewed tags for the monitor, the index of
	 * which is indicated by the seltags variable. */
	unsigned int tagset[2];

	/* This represents the workspaces the monitor owns.
	 *
	 * As an example consider the hexadecimal value of 0x51 (decimal 81) which has a binary
	 * value of:
	 *    001010001  - bitmask
	 *    987654321  - workspaces
	 *
	 * This would mean that the monitor owns workspaces 1, 5 and 7.
	 */
    unsigned int workspaces;

    /// The currently selected workspace and the one being displayed on the monitor.
    /// Also it has the previously displayed workspace.
    unsigned int selected_workspaces[2];

    int sel_ws;

	int hidsel;

	/* The client list. This represents the start of a linked list of clients which determines
	 * the order in which clients are tiled. */
	Client *clients;

	/* This represents the monitor's selected client. */
	Client *sel;

	/* The stacking order list. This represents the order in which client windows are stacked on
	 * top of each other, as well as the order in which clients had last focus. */
	Client *stack;

	/* Monitors are also managed as a linked list with the mons variable referring to the first
	 * monitor. The next variable on the monitor refers to the next monitor in the list. */
	Monitor *next;

    // /// The root of the tree tile display
    // TreeNode *root;

	/* This is the bar window which is used to draw the bar. Each monitor has their own bar. */
	Window barwin;
};

typedef struct Perworkspace {
	/* This represents the number of clients that are to be tiled in the master area. This has
	 * no upper limit but cannot be less than 0. The default value is configured in the
	 * configuration file and the value is adjusted via the incnmaster function. */
    //  Default nmaster = 1:
    // ┌───────────┬────┐
    // │           │ C2 │
    // │    C1     ├────┤
    // │  (master) │ C3 │
    // │           ├────┤
    // │           │ C4 │
    // └───────────┴────┘
    //
    // With nmaster = 2:
    // ┌───────────┬────┐
    // │    C1     │ C3 │
    // │  (master) ├────┤
    // ├───────────┤ C4 │
    // │    C2     ├────┤
    // │ (also     │ C5 │
    // │  master)  │    │
    // └───────────┴────┘
    /// Number of windows in master area
	int nmaster;

    /// What percentage of the screen master gets
	float mfact;

	/* The sellt variable is either 0 or 1 and represents the currently selected layout. This
	 * follows the same mechanism as seltags above giving patterns such a:
	 *
	 *    m->lt[m->sellt]
	 *    selmon->lt[selmon->sellt]
	 *    c->mon->lt[c->mon->sellt]
	 */
	unsigned int sellt;

	/* This array holds the previous and current layout for the monitor, the index of which is
	 * indicated by the sellt variable. */
	const Layout *lt[2];

	/* This holds the layout symbol text, typically as defined in the layouts array. This is
	 * used when drawing the layout symbol on the bar. The reason why this is defined for the
	 * monitor rather than simply using the layout symbol as defined in the layouts array is
	 * that some layouts, like the monocle layout for example, may alter the layout symbol
	 * depending on how many clients are present. */
	char ltsymbol[16];

	/* Internal flag indicating whether the bar is shown or not. */
	int showbar;

	/* Internal flag indicating whether the bar is shown at the top or at the bottom. */
	int topbar;

    /// The tag root tree node 
    TreeNode* root;
} Perworkspace;

typedef struct {
	const char *class;
	const char *instance;
	const char *title;
	unsigned int workspace;
	int isfloating;
	int isterminal;
	int noswallow;
	int monitor;
} Rule;

typedef struct Systray   Systray;
struct Systray {
	Window win;
	Client *icons;
};

/* function declarations */
void applyrules(Client *c);
int applysizehints(Client *c, int *x, int *y, int *w, int *h, int *bw, int interact);
void arrange(Monitor *m);
void arrangemon(Monitor *m);
void attach(Client *c);
void attachstack(Client *c);
void buttonpress(XEvent *e);
void checkotherwm(void);
void cleanup(void);
void cleanupmon(Monitor *mon);
void clientmessage(XEvent *e);
void configure(Client *c);
void configurenotify(XEvent *e);
void configurerequest(XEvent *e);
Monitor *createmon(void);
void destroynotify(XEvent *e);
void detach(Client *c);
void detachstack(Client *c);
Monitor *dirtomon(int dir);
void drawbar(Monitor *m);
void drawbars(void);
int drawstatusbar(Monitor *m, int bh, char* text);
void enternotify(XEvent *e);
void expose(XEvent *e);
void focus(Client *c);
void focusin(XEvent *e);
void focusmon(const Arg *arg);
void focusstackvis(const Arg *arg);
void focusstackhid(const Arg *arg);
void focusstack(int inc, int hid);
Atom getatomprop(Client *c, Atom prop);
int getrootptr(int *x, int *y);
long getstate(Window w);
unsigned int getsystraywidth();
int gettextprop(Window w, Atom atom, char *text, unsigned int size);
void grabbuttons(Client *c, int focused);
void grabkeys(void);
void hide(const Arg *arg);
void hidewin(Client *c);
void incnmaster(const Arg *arg);
void key_move(const Arg *arg);
void key_shift_move(const Arg *arg);
void keypress(XEvent *e);
void killclient(const Arg *arg);
void manage(Window w, XWindowAttributes *wa);
void mappingnotify(XEvent *e);
void maprequest(XEvent *e);
void monocle(Monitor *m);
void motionnotify(XEvent *e);
void movemouse(const Arg *arg);
void movetoworkspace(const Arg *arg);
Client *nexttiled(Client *c);
void pop(Client *c);
void propertynotify(XEvent *e);
void quit(const Arg *arg);
Monitor *recttomon(int x, int y, int w, int h);
void removesystrayicon(Client *i);
void resize(Client *c, int x, int y, int w, int h, int bw, int interact);
void resizebarwin(Monitor *m);
void resizeclient(Client *c, int x, int y, int w, int h, int bw);
void resizemouse(const Arg *arg);
void resizerequest(XEvent *e);
void restack(Monitor *m);
void run(void);
void scan(void);
int sendevent(Window w, Atom proto, int m, long d0, long d1, long d2, long d3, long d4);
void sendmon(Client *c, Monitor *m);
void sendtoworkspace(const Arg *arg);
void setclientstate(Client *c, long state);
void setclientworkspaceprop(Client *c);
void setfocus(Client *c);
void setfullscreen(Client *c, int fullscreen);
void setlayout(const Arg *arg);
void setmfact(const Arg *arg);
void setup(void);
void seturgent(Client *c, int urg);
void show(const Arg *arg);
void showall(const Arg *arg);
void showwin(Client *c);
void showhide(Client *c);
void sighup(int unused);
void sigterm(int unused);
void spawn(const Arg *arg);
Monitor *systraytomon(Monitor *m);
void tagmon(const Arg *arg);
void tile(Monitor *m);
void tree(Monitor *m);
void togglebar(const Arg *arg);
void togglefloating(const Arg *arg);
void togglewin(const Arg *arg);
void treenode_add(Client *c);
void treenode_move(const Arg *arg);
void treenode_navigate(const Arg *arg);
void treenode_remove(Client *c);
void unfocus(Client *c, int setfocus);
void unmanage(Client *c, int destroyed);
void unmapnotify(XEvent *e);
void updatebarpos(Monitor *m);
void updatebars(void);
void updateclientlist(void);
int updategeom(void);
void updatemotifhints(Client *c);
void updatenumlockmask(void);
void updatesizehints(Client *c);
void updatestatus(void);
void updatesystray(void);
void updatesystrayicongeom(Client *i, int w, int h);
void updatesystrayiconstate(Client *i, XPropertyEvent *ev);
void updatetitle(Client *c);
void updatewindowtype(Client *c);
void updatewmhints(Client *c);
void view(const Arg *arg);
void viewworkspace(const Arg *arg);
Client *wintoclient(Window w);
Monitor *wintomon(Window w);
Client *wintosystrayicon(Window w);
int xerror(Display *dpy, XErrorEvent *ee);
int xerrordummy(Display *dpy, XErrorEvent *ee);
int xerrorstart(Display *dpy, XErrorEvent *ee);
void zoom(const Arg *arg);
pid_t getparentprocess(pid_t p);
int isdescprocess(pid_t p, pid_t c);
Client *swallowingclient(Window w);
Client *termforwin(const Client *c);
pid_t winpid(Window w);

/* variables */
extern Systray *systray;
extern const char broken[];
extern char stext[1024];
extern int screen;
extern int sw, sh;           /* X display screen geometry width, height */
extern int bh;               /* bar height */
extern int lrpad;            /* sum of left and right padding for text */
extern int (*xerrorxlib)(Display *, XErrorEvent *);
extern unsigned int numlockmask;
extern void (*handler[LASTEvent]) (XEvent *);
extern Atom wmatom[WMLast], netatom[NetLast], xatom[XLast], motifatom;
extern int restart;
extern int running;
extern Cur *cursor[CurLast];
extern Clr **scheme;
extern Display *dpy;
extern Drw *drw;

/// First monitor and selected monitor
extern Monitor *mons, *selmon;

/// Root window and supporting window
extern Window root, wmcheckwin;

extern xcb_connection_t *xcon;

extern Perworkspace *perworkspaces[];

#endif /* DWM_H */
