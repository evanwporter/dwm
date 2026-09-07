#include <criterion/criterion.h>

#include "../tree.h"

Test(basic_tree, guard_clauses_ignore_invalid_or_duplicate_operations)
{
    Monitor monitor = {0};
    Client client = {.mon = &monitor, .workspace = 1};

    treenode_add(NULL);
    treenode_remove(NULL);
    cr_assert_null(workspace_roots[0]);

    treenode_add(&client);
    TreeNode *root = workspace_roots[0];
    treenode_add(&client);
    cr_assert_eq(workspace_roots[0], root);
    cr_assert_eq(client.node, root);
    cr_assert_eq(root->client, &client);

    treenode_remove(&client);
    treenode_remove(&client);
    cr_assert_null(workspace_roots[0]);
    cr_assert_null(client.node);
}

Test(basic_tree, adding_first_client_creates_leaf_root)
{
    Monitor monitor = {0};
    Client client = {.mon = &monitor, .workspace = 1};

    treenode_add(&client);

    cr_assert_eq(workspace_roots[0], client.node);
    cr_assert_eq(client.node->client, &client);
    cr_assert_null(client.node->a);
    cr_assert_null(client.node->b);
    cr_assert_null(client.node->parent);

    treenode_remove(&client);
    cr_assert_null(workspace_roots[0]);
    cr_assert_null(client.node);
}

Test(basic_tree, adding_second_client_splits_focused_leaf)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client second = {.mon = &monitor, .workspace = 1};

    treenode_add(&first);
    monitor.sel = &first;
    treenode_add(&second);

    TreeNode *root = workspace_roots[0];
    cr_assert_null(root->client);
    cr_assert_eq(root->a, first.node);
    cr_assert_eq(root->b, second.node);
    cr_assert_eq(first.node->client, &first);
    cr_assert_eq(second.node->client, &second);
    cr_assert_eq(first.node->parent, root);
    cr_assert_eq(second.node->parent, root);
    cr_assert_eq(first.node->is_A, 1);
    cr_assert_eq(second.node->is_A, 0);

    treenode_remove(&second);
    treenode_remove(&first);
    cr_assert_null(workspace_roots[0]);
}

Test(basic_tree, restart_add_uses_workspace_tree_when_monitor_focus_is_elsewhere)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client restored = {.mon = &monitor, .workspace = 1};
    Client focused_elsewhere = {.mon = &monitor, .workspace = 2};

    treenode_add(&first);
    treenode_add(&focused_elsewhere);
    monitor.sel = &focused_elsewhere;

    /* Startup restores windows from several workspaces while monitor->sel
     * can still point at a client from another workspace. */
    treenode_add(&restored);

    cr_assert_null(workspace_roots[0]->client);
    cr_assert_eq(workspace_roots[0]->a->client, &first);
    cr_assert_eq(workspace_roots[0]->b->client, &restored);
    cr_assert_eq(workspace_roots[1]->client, &focused_elsewhere);

    treenode_remove(&restored);
    treenode_remove(&first);
    treenode_remove(&focused_elsewhere);
    cr_assert_null(workspace_roots[0]);
    cr_assert_null(workspace_roots[1]);
}

Test(basic_tree, removing_leaf_promotes_sibling_and_preserves_tree)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client second = {.mon = &monitor, .workspace = 1};
    Client third = {.mon = &monitor, .workspace = 1};

    treenode_add(&first);
    monitor.sel = &first;
    treenode_add(&second);
    monitor.sel = &second;
    treenode_add(&third);

    treenode_remove(&third);

    TreeNode *root = workspace_roots[0];
    cr_assert_eq(root->a->client, &first);
    cr_assert_eq(root->b->client, &second);
    cr_assert_eq(second.node, root->b);
    cr_assert_eq(second.node->parent, root);
    cr_assert_null(third.node);

    treenode_remove(&first);
    cr_assert_eq(workspace_roots[0], second.node);
    cr_assert_null(second.node->parent);

    treenode_remove(&second);
    cr_assert_null(workspace_roots[0]);
}
