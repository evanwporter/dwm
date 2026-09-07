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

    perworkspaces[0]->root = root;
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

    perworkspaces[0]->root = root;
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

    perworkspaces[0]->root = &tree->root;
    tree->monitor.sel = &tree->baa_client;
    selmon = &tree->monitor;
}
