#include <criterion/criterion.h>

#include "../scroll.h"

void scroll_right(void);

/*
 * These tests describe the DESIRED scroll-layout behavior.
 *
 * They are intentionally not written around the current implementation.
 * Several should fail until scroll layout is fixed.
 */

static void
link_clients(Monitor *monitor, Client *clients, int count)
{
    monitor->clients = &clients[0];

    for (int i = 0; i < count; i++) {
        clients[i].mon = monitor;
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

    cr_assert_eq(clients[0].x, 0);
    cr_assert_eq(clients[0].w, 500);

    cr_assert_eq(clients[1].x, 500);
    cr_assert_eq(clients[1].w, 500);

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

    cr_assert_eq(clients[0].x, 100);
    cr_assert_eq(clients[0].y, 50);

    cr_assert_eq(clients[1].x, 600);
    cr_assert_eq(clients[1].y, 50);

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

    scroll_right();

    cr_assert_eq(monitor.sel, &clients[2]);
    cr_assert_eq(monitor.anchor, 50);

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
    cr_assert_eq(monitor.anchor, 0);

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
 * Initial:                         ┌───────────────────┐
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
    cr_assert_eq(monitor.anchor, 80);

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

    cr_assert_eq(clients[1].scrollw, 55);
    cr_assert_eq(monitor.anchor, 5);

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

    cr_assert_eq(clients[1].scrollw, 85);
    cr_assert_eq(monitor.anchor, 55);

    selmon = NULL;
}
