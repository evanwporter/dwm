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

