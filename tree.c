#include <assert.h>

#include "dwm.h"
#include "tree.h"
#include "config.h"

static void
tree_recurse(TreeNode *node, int x, int y, int w, int h)
{
    assert(node->client || (node->a && node->b));

    if (node->client) {
        resize(node->client, x, y, w, h, borderpx, 0);
        return;
    }

    if (node->stacked) {
        // Stacked vertically: a on top, b on bottom
        //  ┌─────────┐
        //  │    a    │
        //  ├─────────┤
        //  │    b    │
        //  └─────────┘
        tree_recurse(node->a, x, y, w, h / 2); // Top half
        tree_recurse(node->b, x, y + h / 2, w, h / 2); // Bottom half
    } else {
        // Side by side: a on left, b on right
        //  ┌─────┬─────┐
        //  │  a  │  b  │
        //  └─────┴─────┘
        tree_recurse(node->a, x, y, w / 2, h); // Left half
        tree_recurse(node->b, x + w / 2, y, w / 2, h); // Right half
    }
}

void
tree(Monitor *m) 
{
    // If the root tree node of the current monitor is null for whatever reason
    // then we just provide the tiling layout
    if (!m->root) {
        tile(m);
        return;
    }

    TreeNode *node = m->root;

    // pass along the window boundaries
    tree_recurse(node, m->mx, m->my, m->ww, m->wh);
}

static void
treemove_recurse(TreeNode *node, const Arg *arg, int op, int child_is_A)
{
    if (!node)
        return;

    int left = arg->i == 0;
    int down = arg->i == 1;
    int up = arg->i == 2;
    int right = arg->i == 3;

    int stacked = node->stacked;

    int nop = (!stacked && ((left && child_is_A) || (right && !child_is_A)))
        || (stacked && ((up && child_is_A) || (down && !child_is_A)));
    int flip = (stacked && (left || right)) || (!stacked && (up || down));
    int swap = (right && child_is_A) || (down && child_is_A) || (up && !child_is_A) || (left && !child_is_A);

    if (nop) {
        treemove_recurse(node->parent, arg, 0, node->is_A);
    }

    if (flip) {
        node->stacked = !node->stacked;
    }

    if (swap) {
        TreeNode *tmp = node->a;
        node->a = node->b;
        node->b = tmp;
        node->a->is_A = 1;
        node->b->is_A = 0;
        return;
    }
}

void
treenode_move(const Arg *arg)
{
    /// Currenly focused client
    Client *sel = selmon->sel;

    if (!sel || !sel->node)
        return;

    TreeNode *node = sel->node;

    treemove_recurse(node->parent, arg, 0, node->is_A);

    arrange(selmon);
}


void
treenode_remove(Client* c) 
{   

    if (!c)
        return;

    // When we remove a node, the sibling becomes the parent.
    
    TreeNode *node = c->node;

    TreeNode *sibling, *parent, *grandparent;

    Monitor *m = c->mon;

    if (!node) 
        return;

    // make sure node doesn't have any children
    assert(!node->a && !node->b);

    // make sure it points to the same client
    assert(node->client == c);

    if (m->root == node) {
        // node is the root node within the monitor
        
        m->root = NULL;
        c->node = NULL;

        free(node);

        return;
    }

    parent = node->parent;

    // if parent did not exist here then node would be a root
    // node and thus there's an issue here
    assert(parent);
   
    // the parent is not a leaf so it should not have a client
    assert(!parent->client);

    if (node->is_A)
        sibling = parent->b;
    else
        sibling = parent->a;

    grandparent = parent->parent;
     
    // sibling becomes the parent node
    
    if (grandparent) {
        // grandparent exists then parent is not the root node

        if (parent->is_A) {
            grandparent->a = sibling;
            sibling->is_A = 1;
        } else {
            grandparent->b = sibling;
            sibling->is_A = 0;
        }

        sibling->parent = grandparent;
    } else {
        // parent is the root node, thus the new root node becomes sibling
        m->root = sibling;

        // TODO: This may not be necessary
        sibling->is_A = 1;

        // Sibling doesn't have a parent
        sibling->parent = NULL;
    }

    free(parent);
    free(node);

    // set the pointer to the node to null
    // probably not necessary since the client will be free soon enough
    c->node = NULL;
}

void
treenode_add(Client *c)
{
    Client *focused;
    TreeNode *focused_node, *node_a, *node_b;

    if (!c || !c->mon || c->node)
        return;

    focused = c->mon->sel;
    if (!c->mon->root || !focused || !focused->node) {
        // Early Exit if there is no root node
        c->node = ecalloc(1, sizeof(*c->node));
        c->node->client = c;
        c->mon->root = c->node;
        return;
    }

    focused_node = focused->node;
    assert(focused_node->client == focused);
    assert(!focused_node->a && !focused_node->b);

    node_a = ecalloc(1, sizeof(*node_a));
    node_b = ecalloc(1, sizeof(*node_b));

    node_a->client = focused;
    node_a->is_A = 1;
    node_a->parent = focused_node;

    node_b->client = c;
    node_b->parent = focused_node;

    focused->node = node_a;
    c->node = node_b;

    if (focused_node->parent)
        focused_node->stacked = !focused_node->parent->stacked;
    else focused_node->stacked = 0;

    focused_node->a = node_a;
    focused_node->b = node_b;
    focused_node->client = NULL;
}

static TreeNode *
navigate_tree(TreeNode *node, int dir)
{
    if (!node)
        return NULL;

    // Direction Mapping: 0 = LEFT, 1 = DOWN, 2 = UP, 3 = RIGHT
    int is_vertical   = (dir == 1 || dir == 2); // Axis: 1 for V-splits, 0 for H-splits
    int coming_from_a = (dir == 1 || dir == 3); // DOWN and RIGHT require coming from 'a'

    int perp_choices[64];
    int perp_count = 0;

    TreeNode *curr = node;
    TreeNode *parent = node->parent;

    // ASCENT: Climb until we find a split along our movement axis that we can cross
    while (parent) {
        if (parent->stacked == is_vertical) {
            // Split is along our movement axis:
            // Check if we are currently on the starting side of the split
            if (curr->is_A == coming_from_a) {
                // Cross boundary: if we came from 'a', cross to 'b', and vice-versa
                curr = coming_from_a ? parent->b : parent->a;
                break;
            }
        } else {
            // Split is perpendicular: record position choice to preserve alignment
            if (perp_count < 64)
                perp_choices[perp_count++] = curr->is_A ? 1 : 0;
        }

        curr = parent;
        parent = parent->parent;
    }

    // Edge of screen: no window exists in this direction
    if (!parent)
        return NULL;

    // DESCENT: Walk down target subtree to the boundary-adjacent leaf window
    while (curr && (curr->a || curr->b)) {
        if (curr->stacked == is_vertical) {
            // Split is along our movement axis:
            // Pick child closest to boundary (a = top/left, b = bottom/right)
            if (coming_from_a)
                curr = curr->a ? curr->a : curr->b;
            else
                curr = curr->b ? curr->b : curr->a;
            
        } else {
            // Split is perpendicular: replay recorded LIFO choices
            if (perp_count > 0) {
                int went_a = perp_choices[--perp_count];
                if (went_a)
                    curr = curr->a ? curr->a : curr->b;
                else
                    curr = curr->b ? curr->b : curr->a;
            } else {
                // Fallback for deeper subtrees
                curr = curr->a ? curr->a : curr->b;
            }
        }
    }

    return curr;
}

void
treenode_navigate(const Arg *arg)
{
    Client *sel = selmon->sel;
    if (!sel || !sel->node)
        return;

    TreeNode *target = navigate_tree(sel->node, arg->i);

    if (target && target->client) {
        focus(target->client);
        restack(selmon);
    }
}

