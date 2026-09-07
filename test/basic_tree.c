#include <criterion/criterion.h>

#include "../tree.h"

Test(basic_tree, guard_clauses_ignore_invalid_or_duplicate_operations)
{
    Monitor monitor = {0};
    Client client = {.mon = &monitor, .workspace = 1};

    tree_add_client(NULL);
    tree_remove_client(NULL);
    cr_assert_null(perworkspaces[0]->root);

    tree_add_client(&client);
    TreeNode *root = perworkspaces[0]->root;
    tree_add_client(&client);
    cr_assert_eq(perworkspaces[0]->root, root);
    cr_assert_eq(client.node, root);
    cr_assert_eq(root->client, &client);

    tree_remove_client(&client);
    tree_remove_client(&client);
    cr_assert_null(perworkspaces[0]->root);
    cr_assert_null(client.node);
}

Test(basic_tree, adding_first_client_creates_leaf_root)
{
    Monitor monitor = {0};
    Client client = {.mon = &monitor, .workspace = 1};

    tree_add_client(&client);

    cr_assert_eq(perworkspaces[0]->root, client.node);
    cr_assert_eq(client.node->client, &client);
    cr_assert_null(client.node->a);
    cr_assert_null(client.node->b);
    cr_assert_null(client.node->parent);

    tree_remove_client(&client);
    cr_assert_null(perworkspaces[0]->root);
    cr_assert_null(client.node);
}

Test(basic_tree, adding_second_client_splits_focused_leaf)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client second = {.mon = &monitor, .workspace = 1};

    tree_add_client(&first);
    monitor.sel = &first;
    tree_add_client(&second);

    TreeNode *root = perworkspaces[0]->root;
    cr_assert_null(root->client);
    cr_assert_eq(root->a, first.node);
    cr_assert_eq(root->b, second.node);
    cr_assert_eq(first.node->client, &first);
    cr_assert_eq(second.node->client, &second);
    cr_assert_eq(first.node->parent, root);
    cr_assert_eq(second.node->parent, root);
    cr_assert_eq(first.node->is_A, 1);
    cr_assert_eq(second.node->is_A, 0);

    tree_remove_client(&second);
    tree_remove_client(&first);
    cr_assert_null(perworkspaces[0]->root);
}

Test(basic_tree, restart_add_uses_workspace_tree_when_monitor_focus_is_elsewhere)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client restored = {.mon = &monitor, .workspace = 1};
    Client focused_elsewhere = {.mon = &monitor, .workspace = 2};

    tree_add_client(&first);
    tree_add_client(&focused_elsewhere);
    monitor.sel = &focused_elsewhere;

    /* Startup restores windows from several workspaces while monitor->sel
     * can still point at a client from another workspace. */
    tree_add_client(&restored);

    cr_assert_null(perworkspaces[0]->root->client);
    cr_assert_eq(perworkspaces[0]->root->a->client, &first);
    cr_assert_eq(perworkspaces[0]->root->b->client, &restored);
    cr_assert_eq(perworkspaces[1]->root->client, &focused_elsewhere);

    tree_remove_client(&restored);
    tree_remove_client(&first);
    tree_remove_client(&focused_elsewhere);
    cr_assert_null(perworkspaces[0]->root);
    cr_assert_null(perworkspaces[1]->root);
}

Test(basic_tree, removing_leaf_promotes_sibling_and_preserves_tree)
{
    Monitor monitor = {0};
    Client first = {.mon = &monitor, .workspace = 1};
    Client second = {.mon = &monitor, .workspace = 1};
    Client third = {.mon = &monitor, .workspace = 1};

    tree_add_client(&first);
    monitor.sel = &first;
    tree_add_client(&second);
    monitor.sel = &second;
    tree_add_client(&third);

    tree_remove_client(&third);

    TreeNode *root = perworkspaces[0]->root;
    cr_assert_eq(root->a->client, &first);
    cr_assert_eq(root->b->client, &second);
    cr_assert_eq(second.node, root->b);
    cr_assert_eq(second.node->parent, root);
    cr_assert_null(third.node);

    tree_remove_client(&first);
    cr_assert_eq(perworkspaces[0]->root, second.node);
    cr_assert_null(second.node->parent);

    tree_remove_client(&second);
    cr_assert_null(perworkspaces[0]->root);
}
