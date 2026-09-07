#include <criterion/criterion.h>
#include <stdlib.h>

#include "../tree.h"


Test(auto_add, inserts_next_to_the_shallowest_leaf_not_the_focused_client)
{
    /* The shallowest leaf is a, while ba is the currently focused client.
     *
     * Initial:                 Final:
     * ┌───────┐                ┌───┬───┐
     * │   a   │                │ a │new│
     * ├───┬───┤  auto-add      ├───┴───┤
     * │*ba│ bb│  -------->     │*ba│ bb│
     * └───┴───┘                └───┴───┘
     *
     * * = focused client
     */
    Monitor monitor = {0};
    Client a_client = {0}, ba_client = {0}, bb_client = {0}, new_client = {0};
    TreeNode root = {0}, a = {0}, b = {0}, ba = {0}, bb = {0};

    root.a = &a;
    root.b = &b;
    a.client = &a_client;
    a.parent = &root;
    a.is_A = 1;
    b.a = &ba;
    b.b = &bb;
    b.parent = &root;
    b.is_A = 0;
    ba.client = &ba_client;
    ba.parent = &b;
    ba.is_A = 1;
    bb.client = &bb_client;
    bb.parent = &b;

    a_client.mon = ba_client.mon = bb_client.mon = new_client.mon = &monitor;
    a_client.workspace = ba_client.workspace = bb_client.workspace = new_client.workspace = 1;
    a_client.node = &a;
    ba_client.node = &ba;
    bb_client.node = &bb;
    monitor.sel = &ba_client;
    perworkspaces[0]->root = &root;

    treenode_auto_add(&new_client);

    cr_assert_eq(root.a, &a);
    cr_assert_null(a.client);
    cr_assert_eq(a.a->client, &a_client);
    cr_assert_eq(a.b->client, &new_client);
    cr_assert_eq(new_client.node, a.b);
    cr_assert_eq(monitor.sel, &ba_client);

    free(a.a);
    free(a.b);
    perworkspaces[0]->root = NULL;
}
