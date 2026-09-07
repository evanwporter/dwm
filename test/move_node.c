#include <criterion/criterion.h>

#include "../tree.h"
#include "util.h"

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

Test(move_node, moving_client_then_removing_all_leaves_no_stale_nodes)
{
    Monitor monitor = {0};
    Client a = {.mon = &monitor, .workspace = 1};
    Client b = {.mon = &monitor, .workspace = 1};
    Arg right = {.i = 3};

    tree_add_client(&a);
    monitor.sel = &a;
    tree_add_client(&b);
    selmon = &monitor;

    treenode_move_node(&right);

    cr_assert_null(perworkspaces[0]->root->client);
    cr_assert_eq(perworkspaces[0]->root->a->client, &b);
    cr_assert_eq(perworkspaces[0]->root->b->client, &a);
    cr_assert_eq(a.node, perworkspaces[0]->root->b);

    tree_remove_client(&a);
    tree_remove_client(&b);
    cr_assert_null(perworkspaces[0]->root);
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

