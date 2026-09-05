#include <criterion/criterion.h>

#include "../tree.h"

static void
setup_stacked_pair(Monitor *monitor, Client *a_client, Client *b_client,
    TreeNode *root, TreeNode *a, TreeNode *b)
{
    root->a = a;
    root->b = b;
    root->stacked = 1;

    a->client = a_client;
    a->parent = root;
    a->is_A = 1;
    a_client->mon = monitor;
    a_client->node = a;

    b->client = b_client;
    b->parent = root;
    b->is_A = 0;
    b_client->mon = monitor;
    b_client->node = b;

    monitor->root = root;
    monitor->sel = a_client;
    selmon = monitor;
}

static void
setup_nested_pair(Monitor *monitor, Client *a_client, Client *ba_client,
    Client *bb_client, TreeNode *root, TreeNode *a, TreeNode *b,
    TreeNode *ba, TreeNode *bb)
{
    root->a = a;
    root->b = b;
    root->stacked = 1;

    a->client = a_client;
    a->parent = root;
    a->is_A = 1;
    a_client->mon = monitor;
    a_client->node = a;

    b->a = ba;
    b->b = bb;
    b->parent = root;
    b->is_A = 0;

    ba->client = ba_client;
    ba->parent = b;
    ba->is_A = 1;
    ba_client->mon = monitor;
    ba_client->node = ba;

    bb->client = bb_client;
    bb->parent = b;
    bb->is_A = 0;
    bb_client->mon = monitor;
    bb_client->node = bb;

    monitor->root = root;
    monitor->sel = ba_client;
    selmon = monitor;
}

typedef struct {
    Monitor monitor;
    Client aaa_client, aab_client, ab_client, baa_client, bab_client, bb_client;
    TreeNode root, a, aa, aaa, aab, ab, b, ba, baa, bab, bb;
} ComplexTree;

static void
setup_complex_tree(ComplexTree *tree)
{
    tree->root.a = &tree->a; tree->root.b = &tree->b; tree->root.stacked = 1;
    tree->a.a = &tree->aa; tree->a.b = &tree->ab; tree->a.parent = &tree->root; tree->a.is_A = 1;
    tree->aa.a = &tree->aaa; tree->aa.b = &tree->aab; tree->aa.parent = &tree->a; tree->aa.is_A = 1; tree->aa.stacked = 1;
    tree->b.a = &tree->ba; tree->b.b = &tree->bb; tree->b.parent = &tree->root; tree->b.is_A = 0; tree->b.stacked = 1;
    tree->ba.a = &tree->baa; tree->ba.b = &tree->bab; tree->ba.parent = &tree->b; tree->ba.is_A = 1;

    tree->aaa.client = &tree->aaa_client; tree->aaa.parent = &tree->aa; tree->aaa.is_A = 1;
    tree->aab.client = &tree->aab_client; tree->aab.parent = &tree->aa; tree->aab.is_A = 0;
    tree->ab.client = &tree->ab_client; tree->ab.parent = &tree->a; tree->ab.is_A = 0;
    tree->baa.client = &tree->baa_client; tree->baa.parent = &tree->ba; tree->baa.is_A = 1;
    tree->bab.client = &tree->bab_client; tree->bab.parent = &tree->ba; tree->bab.is_A = 0;
    tree->bb.client = &tree->bb_client; tree->bb.parent = &tree->b; tree->bb.is_A = 0;

    tree->aaa_client.mon = tree->aab_client.mon = tree->ab_client.mon = &tree->monitor;
    tree->baa_client.mon = tree->bab_client.mon = tree->bb_client.mon = &tree->monitor;
    tree->aaa_client.node = &tree->aaa; tree->aab_client.node = &tree->aab; tree->ab_client.node = &tree->ab;
    tree->baa_client.node = &tree->baa; tree->bab_client.node = &tree->bab; tree->bb_client.node = &tree->bb;

    tree->monitor.root = &tree->root;
    tree->monitor.sel = &tree->baa_client;
    selmon = &tree->monitor;
}

Test(move_node, moving_a_down_swaps_a_and_b)
{
    /* Initial:       Final:
     * ┌───┐          ┌───┐
     * │ a │   Down   │ b │
     * ├───┤    ->    ├───┤
     * │ b │          │ a │
     * └───┘          └───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {0}, a = {0}, b = {0};
    Arg down = {.i = 1};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);
    treenode_move(&down);

    cr_assert_eq(root.a, &b);
    cr_assert_eq(root.b, &a);
    selmon = NULL;
}

Test(move_node, moving_a_left_changes_to_side_by_side)
{
    /* Initial:       Final:
     * ┌───┐          ┌───┬───┐
     * │ a │   Left   │ a │ b │
     * ├───┤    ->    └───┴───┘
     * │ b │
     * └───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {0}, a = {0}, b = {0};
    Arg left = {.i = 0};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);
    treenode_move(&left);

    cr_assert_eq(root.stacked, 0);
    selmon = NULL;
}

Test(move_node, moving_ba_right_swaps_ba_and_bb)
{
    /* Initial:       Final:
     * ┌───────┐      ┌───────┐
     * │   a   │      │   a   │
     * ├───┬───┤ Right├───┬───┤
     * │ ba│ bb│  ->  │ bb│ ba│
     * └───┴───┘      └───┴───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0};
    TreeNode root = {0}, a = {0}, b = {0}, ba = {0}, bb = {0};
    Arg right = {.i = 3};

    setup_nested_pair(&monitor, &a_client, &ba_client, &bb_client,
        &root, &a, &b, &ba, &bb);
    treenode_move(&right);

    cr_assert_eq(root.a, &a);
    cr_assert_eq(root.b, &b);
    cr_assert_eq(b.a, &bb);
    cr_assert_eq(b.b, &ba);
    selmon = NULL;
}

Test(move_node, moving_ba_up_stacks_ba_and_bb)
{
    /* Initial:       Final:
     * ┌───────┐      ┌───────┐
     * │   a   │      │   a   │
     * ├───┬───┤  Up  ├───┬───┤
     * │ ba│ bb│  ->  │ ba│ bb│
     * └───┴───┘      └───┴───┘
     *                  (ba stacked above bb)
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0};
    TreeNode root = {0}, a = {0}, b = {0}, ba = {0}, bb = {0};
    Arg up = {.i = 2};

    setup_nested_pair(&monitor, &a_client, &ba_client, &bb_client,
        &root, &a, &b, &ba, &bb);
    treenode_move(&up);

    cr_assert_eq(root.stacked, 1);
    cr_assert_eq(b.stacked, 1);
    cr_assert_eq(b.a, &ba);
    cr_assert_eq(b.b, &bb);
    selmon = NULL;
}

Test(move_node, moving_ba_left_flips_the_outer_split)
{
    /* Initial:       Final:
     * ┌───────┐      ┌───┬───┐
     * │   a   │      │ ba│   │
     * ├───┬───┤ Left ├───┤ a │
     * │ ba│ bb│  ->  │ bb│   │
     * └───┴───┘      └───┴───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0};
    TreeNode root = {0}, a = {0}, b = {0}, ba = {0}, bb = {0};
    Arg left = {.i = 0};

    setup_nested_pair(&monitor, &a_client, &ba_client, &bb_client,
        &root, &a, &b, &ba, &bb);
    treenode_move(&left);

    cr_assert_eq(root.stacked, 0);
    cr_assert_eq(root.a, &b);
    cr_assert_eq(root.b, &a);

    cr_assert_eq(b.a, &ba);
    cr_assert_eq(b.b, &bb);
    selmon = NULL;
}

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
    treenode_navigate(&up);

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

    monitor.root = &root;
    monitor.sel = &bb_client;
    selmon = &monitor;

    treenode_navigate(&up);

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
    treenode_navigate(&left);

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
    treenode_navigate(&down);

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
    treenode_navigate(&up);

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
    treenode_navigate(&right);

    cr_assert_eq(tree.monitor.sel, &tree.bab_client);
    selmon = NULL;
}
