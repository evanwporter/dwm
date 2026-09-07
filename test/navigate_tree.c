#include <criterion/criterion.h>

#include "../tree.h"
#include "util.h"

Test(navigate_node, navigating_ba_up_focuses_a)
{
    /* Initial:       Final:
     * ┌───────┐      ┌───────┐
     * │   a   │      │*  a  *│
     * ├───┬───┤  Up  ├───┬───┤
     * │*ba│ bb│  ->  │ ba│ bb│
     * └───┴───┘      └───┴───┘
     *  * = focused
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0};
    TreeNode root = {0}, a = {0}, b = {0}, ba = {0}, bb = {0};
    Arg up = {.i = 2};

    setup_nested_pair(&monitor, &a_client, &ba_client, &bb_client,
        &root, &a, &b, &ba, &bb);
    tree_focus_neighbor(&up);

    cr_assert_eq(monitor.sel, &a_client);
    cr_assert_eq(monitor.sel->node, &a);
    selmon = NULL;
}

Test(navigate_node, navigating_bb_up_focuses_baa_in_a_nested_tree)
{
    /* Initial and final layout; *bb is focused initially and *baa finally:
     * ┌───┬───┐
     * │aaa│ ab│
     * ├───┤   │
     * │aab│   │
     * ├───┴───┤
     * │baa│bab│
     * ├───┴───┤
     * │*  bb  │  Up  ->  *baa
     * └───────┘
     */
    Monitor monitor = {0};
    Client aaa_client = {0}, aab_client = {0}, ab_client = {0};
    Client baa_client = {0}, bab_client = {0}, bb_client = {0};
    TreeNode root = {0}, a = {0}, aa = {0}, aaa = {0}, aab = {0}, ab = {0};
    TreeNode b = {0}, ba = {0}, baa = {0}, bab = {0}, bb = {0};
    Arg up = {.i = 2};

    root.a = &a; root.b = &b; root.stacked = 1;
    a.a = &aa; a.b = &ab; a.parent = &root; a.is_A = 1;
    aa.a = &aaa; aa.b = &aab; aa.parent = &a; aa.is_A = 1; aa.stacked = 1;
    b.a = &ba; b.b = &bb; b.parent = &root; b.is_A = 0; b.stacked = 1;
    ba.a = &baa; ba.b = &bab; ba.parent = &b; ba.is_A = 1;

    aaa.client = &aaa_client; aaa.parent = &aa; aaa.is_A = 1;
    aab.client = &aab_client; aab.parent = &aa; aab.is_A = 0;
    ab.client = &ab_client; ab.parent = &a; ab.is_A = 0;
    baa.client = &baa_client; baa.parent = &ba; baa.is_A = 1;
    bab.client = &bab_client; bab.parent = &ba; bab.is_A = 0;
    bb.client = &bb_client; bb.parent = &b; bb.is_A = 0;

    aaa_client.mon = aab_client.mon = ab_client.mon = &monitor;
    baa_client.mon = bab_client.mon = bb_client.mon = &monitor;
    aaa_client.node = &aaa; aab_client.node = &aab; ab_client.node = &ab;
    baa_client.node = &baa; bab_client.node = &bab; bb_client.node = &bb;

    perworkspaces[0]->root = &root;
    monitor.sel = &bb_client;
    selmon = &monitor;

    tree_focus_neighbor(&up);

    cr_assert_eq(monitor.sel, &baa_client);
    cr_assert_eq(monitor.sel->node, &baa);
    selmon = NULL;
}

Test(navigate_node, navigating_baa_left_keeps_focus_at_the_left_edge)
{
    /* ┌───┬───┐
     * │aaa│ ab│
     * ├───┤   │
     * │aab│   │
     * ├───┴───┤
     * │*ba│bab│  Left -> *baa (left edge)
     * ├───┴───┤
     * │  bb   │
     * └───────┘
     */
    ComplexTree tree = {0};
    Arg left = {.i = 0};

    setup_complex_tree(&tree);
    tree_focus_neighbor(&left);

    cr_assert_eq(tree.monitor.sel, &tree.baa_client);
    selmon = NULL;
}

Test(navigate_node, navigating_baa_down_focuses_bb)
{
    /* ┌───┬───┐
     * │aaa│ ab│
     * ├───┤   │
     * │aab│   │
     * ├───┴───┤
     * │*ba│bab│  Down -> *bb
     * ├───┴───┤
     * │  bb   │
     * └───────┘
     */
    ComplexTree tree = {0};
    Arg down = {.i = 1};

    setup_complex_tree(&tree);
    tree_focus_neighbor(&down);

    cr_assert_eq(tree.monitor.sel, &tree.bb_client);
    selmon = NULL;
}

Test(navigate_node, navigating_baa_up_focuses_aab)
{
    /* ┌───┬───┐
     * │aaa│ ab│
     * ├───┤   │
     * │*aab   │  Up <- *baa
     * ├───┴───┤
     * │*ba│bab│
     * ├───┴───┤
     * │  bb   │
     * └───────┘
     */
    ComplexTree tree = {0};
    Arg up = {.i = 2};

    setup_complex_tree(&tree);
    tree_focus_neighbor(&up);

    cr_assert_eq(tree.monitor.sel, &tree.aab_client);
    selmon = NULL;
}

Test(navigate_node, navigating_baa_right_focuses_bab)
{
    /* ┌───┬───┐
     * │aaa│ ab│
     * ├───┤   │
     * │aab│   │
     * ├───┴───┤
     * │*ba│*ba│  Right -> *bab
     * ├───┴───┤
     * │  bb   │
     * └───────┘
     */
    ComplexTree tree = {0};
    Arg right = {.i = 3};

    setup_complex_tree(&tree);
    tree_focus_neighbor(&right);

    cr_assert_eq(tree.monitor.sel, &tree.bab_client);
    selmon = NULL;
}
