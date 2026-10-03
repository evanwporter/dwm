#include <criterion/criterion.h>

#include "../scroll.h"

// DESIRED BEHAVIOR
// layout uses each client’s scrollw
// layout respects monitor origin
// moving right focuses the next client
// viewport only moves when needed
// viewport movement uses actual cumulative widths
// increasing selected client width keeps it visible
// anchor moves only the minimum necessary amount
//
// TODO
// increasing decreasing scrollw of focused client
//   - on increase the anchor should move right to keep entire client in focus
//     but not past the left edge of the client
//   - on decrease the anchor should stay fixed
//     unless a gap between the right edge were to be shown
//   - also it should extend up to the right edge
// client wider than viewport
// scroll left
// spawn new window
//   - should spawn it to the right of the currently focused window (rather than top of the stack)
//   - we can do so setting selmon->sel->next to our newly spawned window
// THINK ABOUT A BIT MORE: on entering the scroll layout for the first time 
// it should automatically set all scrollw to ensure the screen is completely 
// filled up

#define ASSERT_INT_EQ(actual, expected) \
    cr_assert_eq( \
        (actual), \
        (expected), \
        "%s = %d, expected %d", \
        #actual, \
        (actual), \
        (expected) \
    )

void scroll_right(void);

void scroll_left(void);

static const Layout test_scroll_layout = {
    .symbol = "[C]",
    .arrange = scroll,
};

static void
link_clients(Monitor *monitor, Client *clients, int count)
{
    monitor->clients = &clients[0];
    monitor->sel_ws = 0;
    monitor->selected_workspaces[0] = WORKSPACEBIT(1);
    workspaces[0]->sellt = 0;
    workspaces[0]->lt[0] = &test_scroll_layout;

    for (int i = 0; i < count; i++) {
        clients[i].mon = monitor;
        clients[i].workspace = 1;
        clients[i].next = (i + 1 < count) ? &clients[i + 1] : NULL;
        clients[i].isfloating = 0;
    }
}

/*
 * ┌──────────────── viewport = 100% ────────────────┐
 * │                                                 │
 * │        A = 50%           B = 50%                │
 * │ ┌──────────────────┬──────────────────┐         │
 * │ │        A         │        B         │         │
 * │ └──────────────────┴──────────────────┘         │
 * └─────────────────────────────────────────────────┘
 *
 * With anchor = 0, two 50%-wide clients should exactly fill
 * a 1000px-wide monitor.
 */
Test(scroll_layout, two_half_width_clients_fill_monitor)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[0];
    selmon = &monitor;

    scroll(&monitor);

    ASSERT_INT_EQ(clients[0].x, 0);
    ASSERT_INT_EQ(clients[0].w, 500);

    ASSERT_INT_EQ(clients[1].x, 500);
    ASSERT_INT_EQ(clients[1].w, 500);

    selmon = NULL;
}

/*
 * Monitor begins at (100, 50):
 *
 * screen:
 *
 *        x = 100
 *        ↓
 *        ┌────────────────────────────┐
 * y=50 → │      A      │      B       │
 *        │             │              │
 *        │             │              │
 *        └────────────────────────────┘
 *
 * Scroll layout should respect monitor.wx and monitor.wy rather
 * than assuming that the monitor begins at (0, 0).
 *
 * This is particularly important with multiple monitors.
 */
Test(scroll_layout, respects_monitor_position)
{
    Monitor monitor = {
        .wx = 100,
        .wy = 50,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[0];
    selmon = &monitor;

    scroll(&monitor);

    ASSERT_INT_EQ(clients[0].x, 100);
    ASSERT_INT_EQ(clients[0].y, 50);

    ASSERT_INT_EQ(clients[1].x, 600);
    ASSERT_INT_EQ(clients[1].y, 50);

    selmon = NULL;
}


/*
 * Full layout:
 *
 * ┌───────────────┬───────────────┬───────────────┐
 * │       A       │       B       │       C       │
 * │      50%      │      50%      │      50%      │
 * └───────────────┴───────────────┴───────────────┘
 *
 * Initial viewport:
 *
 * ┌───────────────────────────────┐
 * │       A       │       B       │
 * └───────────────────────────────┘
 *                 *
 *
 * Right:
 *
 *                 ┌───────────────────────────────┐
 *                 │       B       │      *C*      │
 *                 └───────────────────────────────┘
 *
 * Moving from B -> C should scroll only enough to reveal C.
 *
 * C ends at 150%, therefore:
 *
 *     anchor = 150 - 100 = 50
 */
Test(scroll_navigation, moving_right_scrolls_minimum_amount_to_reveal_client)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[3] = {
        {.scrollw = 50},
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 3);

    monitor.sel = &clients[1];
    selmon = &monitor;

    // TODO: call scroll_move and get rid of call to scroll
    scroll_right();
    scroll(&monitor);

    // Check that focus has changed
    cr_assert_eq(monitor.sel, &clients[2]);

    ASSERT_INT_EQ(monitor.anchor, 50);

    ASSERT_INT_EQ(clients[0].x, -500);
    ASSERT_INT_EQ(clients[0].y, 0);
    ASSERT_INT_EQ(clients[0].w, 500);
    ASSERT_INT_EQ(clients[0].h, 600);

    ASSERT_INT_EQ(clients[1].x, 0);
    ASSERT_INT_EQ(clients[1].y, 0);
    ASSERT_INT_EQ(clients[1].w, 500);
    ASSERT_INT_EQ(clients[1].h, 600);

    ASSERT_INT_EQ(clients[2].x, 500);
    ASSERT_INT_EQ(clients[2].y, 0);
    ASSERT_INT_EQ(clients[2].w, 500);
    ASSERT_INT_EQ(clients[2].h, 600);

    selmon = NULL;
}

/*
 * Full layout:
 *
 * ┌──────────┬──────────┬──────────┐
 * │    A     │    B     │    C     │
 * │   33%    │   33%    │   33%    │
 * └──────────┴──────────┴──────────┘
 *
 * Viewport already contains all three:
 *
 * ┌─────────────────────────────────┐
 * │    A     │   *B*    │    C      │
 * └─────────────────────────────────┘
 *
 * Right -> C
 *
 * Since C is already completely visible, changing focus should
 * NOT move the viewport.
 */
Test(scroll_navigation, moving_right_does_not_scroll_when_client_is_visible)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[3] = {
        {.scrollw = 33},
        {.scrollw = 33},
        {.scrollw = 33},
    };

    link_clients(&monitor, clients, 3);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_right();

    cr_assert_eq(monitor.sel, &clients[2]);
    ASSERT_INT_EQ(monitor.anchor, 0);

    selmon = NULL;
}

/*
 * Full layout:
 *
 * ┌──────────────────────────┬──────────┐
 * │            A             │    B     │
 * │           130%           │   50%    │
 * └──────────────────────────┴──────────┘
 *
 *                                        viewport
 * Initial:                        ┌───────────────────┐
 *                                 │ tail A │   *B*    │
 *                                 └───────────────────┘
 *
 * B starts at 130% and ends at 180%.
 *
 * To make all of B visible:
 *
 *     anchor = 180 - 100 = 80
 *
 * This verifies that scroll movement is based on actual preceding
 * widths rather than number-of-windows * some assumed width.
 */
Test(scroll_navigation, right_uses_actual_client_widths)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 130},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[0];
    selmon = &monitor;

    scroll_right();

    cr_assert_eq(monitor.sel, &clients[1]);
    ASSERT_INT_EQ(monitor.anchor, 80);

    selmon = NULL;
}

/*
 * Before:
 *
 * viewport
 * ┌─────────────────────────────────┐
 * │       A       │      *B*        │
 * │      50%      │      50%        │
 * └─────────────────────────────────┘
 *
 * Increase B:
 *
 * layout:
 *
 * ┌───────────────┬─────────────────┐
 * │       A       │       *B*       │
 * │      50%      │       55%       │
 * └───────────────┴─────────────────┘
 *                                 ↑ 105%
 *
 * viewport must shift right by 5%:
 *
 *     anchor = 5
 */
Test(scroll_resize, increasing_right_edge_client_keeps_it_visible)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_increase_width(NULL);

    ASSERT_INT_EQ(clients[1].scrollw, 55);
    ASSERT_INT_EQ(monitor.anchor, 5);

    selmon = NULL;
}

/*
 * Before:
 *
 *              viewport
 *              ┌───────────────────────────────┐
 * ┌────────────┼───┬───────────────────────────┤
 * │     A      │   │            *B*            │
 * │    70%     │   │            80%            │
 * └────────────┴───┴───────────────────────────┘
 *              ^
 *          anchor = 50
 *
 * Increasing B to 85% should move the viewport only as much
 * as necessary to keep B's right edge visible.
 *
 * B ends at:
 *
 *     70 + 85 = 155
 *
 * therefore the minimum anchor is:
 *
 *     155 - 100 = 55
 */
Test(scroll_resize, increasing_width_moves_anchor_only_as_much_as_needed)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 50,
    };

    Client clients[2] = {
        {.scrollw = 70},
        {.scrollw = 80},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_increase_width(NULL);

    ASSERT_INT_EQ(clients[1].scrollw, 85);
    ASSERT_INT_EQ(monitor.anchor, 55);

    selmon = NULL;
}

/*
 * Full layout:
 *
 * ┌───────────────┬───────────────┬───────────────┐
 * │       A       │       B       │       C       │
 * │      50%      │      50%      │      50%      │
 * └───────────────┴───────────────┴───────────────┘
 *
 * Initial viewport:
 *
 *                 ┌───────────────────────────────┐
 *                 │      *B*      │       C       │
 *                 └───────────────────────────────┘
 *                 ^
 *             anchor = 50
 *
 * Left:
 *
 * ┌───────────────────────────────┐
 * │      *A*      │       B       │
 * └───────────────────────────────┘
 *
 * Moving from B -> A requires moving the anchor from 50 to 0.
 */
Test(scroll_navigation, moving_left_scrolls_minimum_amount_to_reveal_client)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 50,
    };

    Client clients[3] = {
        {.scrollw = 50},
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 3);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_left();
    scroll(&monitor);

    cr_assert_eq(monitor.sel, &clients[0]);

    ASSERT_INT_EQ(monitor.anchor, 0);

    ASSERT_INT_EQ(clients[0].x, 0);
    ASSERT_INT_EQ(clients[0].w, 500);

    ASSERT_INT_EQ(clients[1].x, 500);
    ASSERT_INT_EQ(clients[1].w, 500);

    ASSERT_INT_EQ(clients[2].x, 1000);
    ASSERT_INT_EQ(clients[2].w, 500);

    selmon = NULL;
}

/*
 * Full layout:
 *
 * ┌──────────┬──────────┬──────────┐
 * │    A     │    B     │    C     │
 * │   33%    │   33%    │   33%    │
 * └──────────┴──────────┴──────────┘
 *
 * All clients are already visible:
 *
 * ┌─────────────────────────────────┐
 * │    A     │    B     │   *C*     │
 * └─────────────────────────────────┘
 *
 * Left -> B
 *
 * B is already completely visible, so the viewport should not move.
 */
Test(scroll_navigation, moving_left_does_not_scroll_when_client_is_visible)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[3] = {
        {.scrollw = 33},
        {.scrollw = 33},
        {.scrollw = 33},
    };

    link_clients(&monitor, clients, 3);

    monitor.sel = &clients[2];
    selmon = &monitor;

    scroll_left();

    cr_assert_eq(monitor.sel, &clients[1]);
    ASSERT_INT_EQ(monitor.anchor, 0);

    selmon = NULL;
}

/*
 * Full layout:
 *
 * ┌──────────┬──────────────────────────┬──────────┐
 * │    A     │            B             │    C     │
 * │   30%    │           130%           │   50%    │
 * └──────────┴──────────────────────────┴──────────┘
 *
 * B begins at 30%.
 *
 * Initial viewport:
 *
 *                          ┌────────────────────────┐
 *                          │ tail B │      *C*       │
 *                          └────────────────────────┘
 *                          ^
 *                      anchor = 120
 *
 * Moving C -> B should place B's left edge at the left side
 * of the viewport:
 *
 *     anchor = 30
 *
 * This verifies that scroll-left movement uses the actual
 * cumulative widths of clients preceding B.
 */
Test(scroll_navigation, left_uses_actual_client_widths)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 120,
    };

    Client clients[3] = {
        {.scrollw = 30},
        {.scrollw = 130},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 3);

    monitor.sel = &clients[2];
    selmon = &monitor;

    scroll_left();

    cr_assert_eq(monitor.sel, &clients[1]);
    ASSERT_INT_EQ(monitor.anchor, 30);

    selmon = NULL;
}

/*
 * Moving left from the first tiled client should do nothing.
 */
Test(scroll_navigation, moving_left_from_first_client_does_nothing)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[0];
    selmon = &monitor;

    scroll_left();

    cr_assert_eq(monitor.sel, &clients[0]);
    ASSERT_INT_EQ(monitor.anchor, 0);

    selmon = NULL;
}

/*
 * Before:
 *
 * viewport
 * ┌─────────────────────────────────┐
 * │       A       │      *B*        │
 * │      50%      │      50%        │
 * └─────────────────────────────────┘
 *
 * Increasing B to 55% pushes its right edge to 105%.
 *
 * The viewport should move right by exactly 5%.
 */
Test(scroll_resize, increase_moves_anchor_right_to_keep_client_visible)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 0,
    };

    Client clients[2] = {
        {.scrollw = 50},
        {.scrollw = 50},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_increase_width(NULL);

    ASSERT_INT_EQ(clients[1].scrollw, 55);
    ASSERT_INT_EQ(monitor.anchor, 5);

    selmon = NULL;
}

/*
 * Before:
 *
 * Full layout ends at 150%.
 *
 *              viewport
 *              ┌───────────────────────────────┐
 * ┌────────────┼───────────────────────────────┤
 * │     A      │             *B*               │
 * │    70%     │             80%               │
 * └────────────┴───────────────────────────────┘
 *              ^
 *          anchor = 50
 *
 * viewport = [50, 150]
 *
 * Decreasing B from 80% -> 75% makes the layout end at 145%.
 *
 * Keeping anchor = 50 would display [50, 150], leaving a 5%
 * empty gap on the right.
 *
 * Therefore anchor should move left to 45 so that:
 *
 *     viewport = [45, 145]
 *
 * and the right edge remains filled.
 */
Test(scroll_resize, decrease_moves_anchor_left_to_avoid_right_gap)
{
    Monitor monitor = {
        .wx = 0,
        .wy = 0,
        .ww = 1000,
        .wh = 600,
        .anchor = 50,
    };

    Client clients[2] = {
        {.scrollw = 70},
        {.scrollw = 80},
    };

    link_clients(&monitor, clients, 2);

    monitor.sel = &clients[1];
    selmon = &monitor;

    scroll_decrease_width(NULL);

    ASSERT_INT_EQ(clients[1].scrollw, 75);
    ASSERT_INT_EQ(monitor.anchor, 45);

    selmon = NULL;
}
