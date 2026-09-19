#include <criterion/criterion.h>

#include "../tree.h"
#include "util.h"

Test(proportion, vertical_directions_adjust_the_stacked_split)
{
    /* ┌───┐
     * │*a │
     * ├───┤
     * │ b │
     * └───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {.proportion = 50}, a = {0}, b = {0};
    Arg down = {.i = 1};
    Arg up = {.i = 2};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);

    // a is selected
    tree_change_proportion(&down);
    cr_assert_eq(root.proportion, 55);

    tree_change_proportion(&up);
    cr_assert_eq(root.proportion, 50);

    // b is selected
    monitor.sel = &b_client;
    tree_change_proportion(&up);
    cr_assert_eq(root.proportion, 45);

    tree_change_proportion(&down);
    cr_assert_eq(root.proportion, 50);
    selmon = NULL;
}

Test(proportion, vertical_bottom_edge_uses_the_opposite_boundary)
{
    /* ┌───┐
     * │ a │
     * ├───┤
     * │*b │
     * └───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {.proportion = 50}, a = {0}, b = {0};
    Arg down = {.i = 1};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);
    monitor.sel = &b_client;

    tree_change_proportion(&down);
    cr_assert_eq(root.proportion, 55);

    root.proportion = 95;
    tree_change_proportion(&down);
    cr_assert_eq(root.proportion, 95);
    selmon = NULL;
}

Test(proportion, horizontal_directions_adjust_the_side_by_side_split)
{
    /* ┌───┬───┐
     * │*a │ b │
     * └───┴───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {.proportion = 50}, a = {0}, b = {0};
    Arg right = {.i = 3};
    Arg left = {.i = 0};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);
    root.stacked = 0;

    tree_change_proportion(&right);
    cr_assert_eq(root.proportion, 55);

    monitor.sel = &b_client;
    tree_change_proportion(&left);
    cr_assert_eq(root.proportion, 50);
    selmon = NULL;
}

Test(proportion, adjusts_the_nearest_crossable_ancestor)
{
    /* ┌───────┐
     * │   a   │
     * ├───┬───┤
     * │*ba│ bb│
     * └───┴───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0};
    TreeNode root = {.proportion = 50}, a = {0}, b = {.proportion = 50};
    TreeNode ba = {0}, bb = {0};
    Arg up = {.i = 2};

    setup_nested_pair(&monitor, &a_client, &ba_client, &bb_client,
        &root, &a, &b, &ba, &bb);

    tree_change_proportion(&up);

    cr_assert_eq(root.proportion, 45);
    cr_assert_eq(b.proportion, 50);
    selmon = NULL;
}

Test(proportion, clamps_at_limits)
{
    /* ┌───┬───┐
     * │*a │ b │
     * └───┴───┘
     */
    Monitor monitor = {0};
    Client a_client = {0}, b_client = {0};
    TreeNode root = {.proportion = 95}, a = {0}, b = {0};
    Arg right = {.i = 3};
    Arg left = {.i = 0};

    setup_stacked_pair(&monitor, &a_client, &b_client, &root, &a, &b);
    root.stacked = 0;

    tree_change_proportion(&right);
    cr_assert_eq(root.proportion, 95);

    root.proportion = 5;
    monitor.sel = &b_client;
    tree_change_proportion(&left);
    cr_assert_eq(root.proportion, 5);

    monitor.sel = &a_client;
    root.proportion = 50;
    tree_change_proportion(&left);
    cr_assert_eq(root.proportion, 45);
    selmon = NULL;
}
