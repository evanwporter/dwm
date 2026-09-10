#include <assert.h>

#include "dwm.h"
#include "tree.h"
#include "config.h"

static void
tree_recurse(const TreeNode *node, int x, int y, int w, int h, int bw)
{
    assert(node->client || (node->a && node->b));

    if (node->client) {
        resize(node->client, x, y, w, h, bw, 0);
        return;
    }

    /// TODO: If there's an odd number of pixels then there's a one pixel gap
    if (node->stacked) {
        // Stacked vertically: a on top, b on bottom
        //  ┌─────────┐
        //  │    a    │
        //  ├─────────┤
        //  │    b    │
        //  └─────────┘
        tree_recurse(node->a, x, y, w, h / 2, bw); // Top half
        tree_recurse(node->b, x, y + h / 2, w, h / 2, bw); // Bottom half
    } else {
        // Side by side: a on left, b on right
        //  ┌─────┬─────┐
        //  │  a  │  b  │
        //  └─────┴─────┘
        tree_recurse(node->a, x, y, w / 2, h, bw); // Left half
        tree_recurse(node->b, x + w / 2, y, w / 2, h, bw); // Right half
    }
}

/// Starting from the root node recusively generate the tree layout based upon
/// a binary space partition
void
tree(Monitor *m) 
{
    const TreeNode *root = PERWS(m)->root;

    /* Tree may be absent while current workspace still has tiled clients. */
    if (!root) {
        tile(m);
        return;
    }

    /// Set the border width to 0 if the root node has a 
    /// client, since this means there is only one node
    /// in the tree.
    const int bw = root->client ? 0 : borderpx;

    // Pass along the window boundaries
    tree_recurse(root, m->mx, m->my, m->ww, m->wh, bw);
}

static void
treemove_recurse(TreeNode *node, const Arg *arg, int child_is_A)
{
    if (!node)
        return;

    int left = arg->i == 0;
    int down = arg->i == 1;
    int up = arg->i == 2;
    int right = arg->i == 3;

    int stacked = node->stacked;

    /// Do nothing
    int nop = (!stacked && ((left && child_is_A) || (right && !child_is_A)))
        || (stacked && ((up && child_is_A) || (down && !child_is_A)));

    /// Flip the stacked bit
    int flip = (stacked && (left || right)) || (!stacked && (up || down));

    /// Swap a and b nodes
    int swap = (right && child_is_A) || (down && child_is_A) || (up && !child_is_A) || (left && !child_is_A);

    if (nop)
        treemove_recurse(node->parent, arg, node->is_A);

    if (flip)
        node->stacked = !node->stacked;

    if (swap) {
        TreeNode *tmp = node->a;
        node->a = node->b;
        node->b = tmp;
        node->a->is_A = 1;
        node->b->is_A = 0;
    }
}

void
treenode_move(const Arg *arg)
{
    /// Currenly focused client
    Client *sel = selmon->sel;

    /// No client is selected; ie no client is being displayed
    /// TODO: remove the sel->node check once I can ascertain 
    ///       that no client can be created without a node
    if (!sel || !sel->node)
        return;

    TreeNode *node = sel->node;

    treemove_recurse(node->parent, arg, node->is_A);

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

    if (!node)
        return;

    // make sure node doesn't have any children
    assert(!node->a && !node->b);

    // make sure it points to the same client
    assert(node->client == c);

    if (perworkspaces[c->workspace - 1]->root == node) {
        // node is the root node within the monitor
        perworkspaces[c->workspace - 1]->root = NULL;
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
        perworkspaces[c->workspace - 1]->root = sibling;

        // TODO: This may not be necessary
        sibling->is_A = 1;

        // Sibling doesn't have a parent
        sibling->parent = NULL;
    }

    /// Get rid of the parent since the sibling is the new parent
    free(parent);

    /// Get rid of the node obviously since we are removing the client
    free(node);

    // set the pointer to the node to null
    // probably not necessary since the client will be free soon enough
    c->node = NULL;
}

static void
treenode_internal_add(Client *c, Client *focused)
{
    TreeNode *focused_node, *node_a, *node_b;

    if (!perworkspaces[c->workspace - 1]->root || !focused || !focused->node) {
        // Early Exit if there is no root node
        c->node = ecalloc(1, sizeof(*c->node));
        c->node->client = c;
        perworkspaces[c->workspace - 1]->root = c->node;
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

void
treenode_add(Client *c)
{
    Client *focused;

    if (!c || !c->mon || c->node)
        return;

    focused = c->mon->sel;
    if (!perworkspaces[c->workspace - 1]->root)
        treenode_internal_add(c, NULL);
    else if (focused && focused->workspace == c->workspace && focused->node)
        treenode_internal_add(c, focused);
    else
        treenode_auto_add(c);
}

static TreeNode *
navigate_tree(TreeNode *node, int dir)
{
    if (!node)
        return NULL;

    // Direction Mapping: 0 = LEFT, 1 = DOWN, 2 = UP, 3 = RIGHT
    int is_vertical   = (dir == 1 || dir == 2); // Axis: 1 for V-splits, 0 for H-splits
    int coming_from_a = (dir == 1 || dir == 3); // DOWN and RIGHT require coming from 'a'

    int choices[64];
    int count = 0;

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
            if (count < 64)
                choices[count++] = curr->is_A ? 1 : 0;
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
            if (count > 0) {
                int went_a = choices[--count];
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

/// Move node across tree
void
treenode_move_node(const Arg *arg)
{
    Client *sel = selmon->sel;
    if (!sel || !sel->node)
        return;

    // TODO: Check if we are on the tree mode and return if no

    TreeNode *target = navigate_tree(sel->node, arg->i);

    if (target && target->client) {
        /* Remove first: insertion replaces sel->node.  Removing afterward
         * would remove new leaf and leave old leaf pointing at sel. */
        Client *target_client = target->client;

        treenode_remove(sel);
        treenode_internal_add(sel, target_client);
        arrange(selmon);
    }
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

static TreeNode*
closest_leaf(int workspace)
{
    if (!perworkspaces[workspace - 1]->root)
        return NULL;

    /// TODO: Currently there is no mechanism to allocate more space in
    /// the queue if the number of nodes exceeds 64. Furthermore there's
    /// no wrapping mechanism to insure our index always lands between 0
    /// and 63
    TreeNode *queue[64];
    int front = 0;
    int rear = 1;

    queue[0] = perworkspaces[workspace - 1]->root;

    // BFS
    while (front < rear) { // check if the queue is empty
        TreeNode *current = queue[front];
        front++;
        
        if (current->a == NULL && current->b == NULL)
            return current;

        if (current->a != NULL) {
            queue[rear] = current->a;
            rear++;
        }

        if (current->b != NULL) {
            queue[rear] = current->b;
            rear++;
        }
    }

    // Should never get here
    return NULL;
}

/// This automatically places a new client based on the shortest leaf.
/// This is called by send_to_workspace, when the focused client is not 
/// in the same workspace as the target workspace.
void
treenode_auto_add(Client *c)
{
    if (!c || !c->mon || c->node)
        return;

    TreeNode *node = closest_leaf(c->workspace);
    Client *focused = node ? node->client : NULL;
    treenode_internal_add(c, focused);
}
