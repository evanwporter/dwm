#ifndef TREE_H
#define TREE_H

#include "dwm.h"

/// Manual Node is either a program (Client) or a split of two ManualNodes.
/// The children nodes are either vertically or horizontally split
struct TreeNode {
    // TODO: Perhaps wrap (a and b) and (stacked, client and proportion) in a union
    // to better represent their mutual exclusivity

    /// Children Nodes that will be display when its split
    /// Can either be arranged as:
    ///     a | b
    /// or
    ///      a
    ///     ---
    ///      b
    TreeNode *a, *b;

    int is_A;

    /// (0) they are side by side or 
    /// (1) they are stacked
    int stacked;

    /// Assuming this node is not split, then it will have a
    /// client node which is the actual window that's being displayed
    Client* client;

    TreeNode *parent;
};

void tree(Monitor *m);
void treenode_add(Client *c);
void treenode_auto_add(Client *c);
void treenode_move_node(const Arg *arg);
void treenode_move(const Arg *arg);
void treenode_navigate(const Arg *arg);
void treenode_remove(Client *c);

#endif /* TREE_H */
