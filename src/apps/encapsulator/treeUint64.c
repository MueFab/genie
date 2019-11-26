#include "treeUint64.h"
#include <stdio.h>
#include <stdlib.h>

void fixUp(Tree* tree, TreeNode* insertedNode);
void setAsParentOf(TreeNode* child, TreeNode* parent);
void setAsLeft(TreeNode* treeNode, TreeNode* parent);
void setAsRight(TreeNode* treeNode, TreeNode* parent);
void setBalance(TreeNode* treeNode, int balance);
TreeNode* getParent(TreeNode* treeNode);
TreeNode* getLeftChild(TreeNode* treeNode);
TreeNode* getRightChild(TreeNode* treeNode);
int8_t getBalance(TreeNode* treeNode);
void setRoot(Tree* tree, TreeNode* newNode);
void increaseBalance(TreeNode* parent);
void decreaseBalance(TreeNode* parent);
bool insertIntoNode(TreeNode* parent, TreeNode* treeNode);
void setAsNewChildOf(Tree* tree, TreeNode* parent, TreeNode* newChild, TreeNode* oldChild);
TreeNode* rotateLeft(TreeNode* parent, TreeNode* currentlyBeingFixedNode);
TreeNode* rotateRight(TreeNode* parent, TreeNode* currentlyBeingFixedNode);
TreeNode* rotateRightLeft(TreeNode* X, TreeNode* Z);
TreeNode* rotateLeftRight(TreeNode* X, TreeNode* Z);
uint64_t numberElementsInNode(TreeNode* treeNode);
void freeTreeNode(TreeNode* treeNode);

TreeNode* initTreeNode(uint64_t value) {
    TreeNode* treeNode = (TreeNode*)calloc(1, sizeof(TreeNode));
    treeNode->value = value;
    return treeNode;
}

Tree* initTree() { return (Tree*)calloc(1, sizeof(Tree)); }

void setAsParentOf(TreeNode* child, TreeNode* parent) {
    if (child != NULL) {
        child->parent = parent;
    }
}

void setAsLeft(TreeNode* treeNode, TreeNode* parent) {
    if (parent != NULL) {
        setAsParentOf(treeNode, parent);
        parent->left = treeNode;
    }
}

void setAsRight(TreeNode* treeNode, TreeNode* parent) {
    setAsParentOf(treeNode, parent);
    if (parent != NULL) {
        parent->right = treeNode;
    }
}

void setBalance(TreeNode* treeNode, int balance) { treeNode->balance = balance; }

TreeNode* getParent(TreeNode* treeNode) { return treeNode->parent; }

TreeNode* getLeftChild(TreeNode* treeNode) { return treeNode->left; }

TreeNode* getRightChild(TreeNode* treeNode) { return treeNode->right; }

int8_t getBalance(TreeNode* treeNode) { return treeNode->balance; }

void setRoot(Tree* tree, TreeNode* newNode) { tree->root = newNode; }

void increaseBalance(TreeNode* parent) { parent->balance++; }

void decreaseBalance(TreeNode* parent) { parent->balance--; }

bool insertIntoNode(TreeNode* parent, TreeNode* treeNode) {
    if (parent->value == treeNode->value) {
        return false;
    }
    bool inserted = true;
    if (treeNode->value < parent->value) {
        if (parent->left == NULL) {
            setAsLeft(treeNode, parent);
        } else {
            if (!insertIntoNode(parent->left, treeNode)) {
                inserted = false;
            }
        }
    } else {
        if (parent->right == NULL) {
            setAsRight(treeNode, parent);
        } else {
            if (!insertIntoNode(parent->right, treeNode)) {
                inserted = false;
            }
        }
    }
    return inserted;
}

bool insert(Tree* tree, TreeNode* treeNode) {
    if (tree == NULL) {
        return false;
    }
    if (tree->root == NULL) {
        tree->root = treeNode;
        return true;
    } else {
        if (insertIntoNode(tree->root, treeNode)) {
            fixUp(tree, treeNode);
            return true;
        }
        return false;
    }
}

void setAsNewChildOf(Tree* tree, TreeNode* parent, TreeNode* newChild, TreeNode* oldChild) {
    setAsParentOf(newChild, parent);
    if (parent != NULL) {
        if (oldChild == getLeftChild(parent)) {
            setAsLeft(newChild, parent);
        } else {
            setAsRight(newChild, parent);
        }
    } else {
        setRoot(tree, newChild);
    }
}

TreeNode* rotateLeft(TreeNode* parent, TreeNode* currentlyBeingFixedNode) {
    TreeNode* child = getLeftChild(currentlyBeingFixedNode);
    parent->right = child;
    if (child != NULL) {
        setAsParentOf(child, parent);
    }
    currentlyBeingFixedNode->left = parent;
    parent->parent = currentlyBeingFixedNode;

    if (getBalance(currentlyBeingFixedNode) == 0) {
        increaseBalance(parent);  // t23 now higher
        decreaseBalance(currentlyBeingFixedNode);
    } else {  // 2nd case happens with insertion or deletion:
        setBalance(parent, 0);
        setBalance(currentlyBeingFixedNode, 0);
    }
    return currentlyBeingFixedNode;
}

TreeNode* rotateRight(TreeNode* parent, TreeNode* currentlyBeingFixedNode) {
    TreeNode* child = getRightChild(currentlyBeingFixedNode);
    parent->left = child;
    if (child != NULL) {
        setAsParentOf(child, parent);
    }
    setAsRight(parent, currentlyBeingFixedNode);
    setAsParentOf(parent, currentlyBeingFixedNode);

    if (getBalance(currentlyBeingFixedNode) == 0) {
        decreaseBalance(parent);
        increaseBalance(currentlyBeingFixedNode);
    } else {
        setBalance(parent, 0);
        setBalance(currentlyBeingFixedNode, 0);
    }
    return currentlyBeingFixedNode;
}

TreeNode* rotateRightLeft(TreeNode* X, TreeNode* Z) {
    TreeNode* Y = getLeftChild(Z);
    TreeNode* t3 = getRightChild(Y);

    int8_t balanceY = getBalance(Y);

    Z->left = t3;
    if (t3 != NULL) {
        setAsParentOf(t3, Z);
    }
    Y->right = Z;
    Z->parent = Y;
    TreeNode* t2 = getLeftChild(Y);
    X->right = t2;
    if (t2 != NULL) {
        t2->parent = X;
    }
    Y->left = X;
    X->parent = Y;

    // 1st case, BalanceFactor(Y) > 0, happens with insertion or deletion:
    if (getBalance(Y) > 0) {  // t3 was higher
        setBalance(Z, 0);
        setBalance(X, -1);
    } else {  // 2nd case, BalanceFactor(Y) == 0, only happens with deletion, not insertion:
        if (getBalance(Y) == 0) {
            setBalance(X, 0);
            setBalance(Z, 0);
        } else {  // 3rd case happens with insertion or deletion:
            // t2 was higher
            setBalance(X, 0);
            setBalance(Z, 1);
        }
    }
    setBalance(Y, 0);
    return Y;  // return new root of rotated subtree
}

TreeNode* rotateLeftRight(TreeNode* X, TreeNode* Z) {
    TreeNode* Y = getRightChild(Z);
    TreeNode* t2 = getLeftChild(Y);
    TreeNode* t3 = getRightChild(Y);

    Z->right = t2;
    if (t2 != NULL) {
        setAsParentOf(t2, Z);
    }
    Y->left = Z;
    Z->parent = Y;

    X->left = t3;
    if (t3 != NULL) {
        t3->parent = X;
    }
    Y->right = X;
    X->parent = Y;

    // 1st case, BalanceFactor(Y) > 0, happens with insertion or deletion:
    if (getBalance(Y) > 0) {  // t3 was higher
        setBalance(X, 0);
        setBalance(Z, -1);
    } else {  // 2nd case, BalanceFactor(Y) == 0, only happens with deletion, not insertion:
        if (getBalance(Y) == 0) {
            setBalance(X, 0);
            setBalance(Z, 0);
        } else {  // 3rd case happens with insertion or deletion:
            // t2 was higher
            setBalance(X, 1);
            setBalance(Z, 0);
        }
    }
    setBalance(Y, 0);
    return Y;  // return new root of rotated subtree
}

void fixUp(Tree* tree, TreeNode* insertedNode) {
    TreeNode* Z = insertedNode;
    for (TreeNode* X = getParent(Z); X != NULL; X = getParent(Z)) {
        TreeNode* G;
        TreeNode* N;
        if (Z == getRightChild(X)) {
            if (getBalance(X) > 0) {  // X is right-heavy
                G = getParent(X);     // Save parent of X around rotations
                if (getBalance(Z) < 0) {
                    N = rotateRightLeft(X, Z);  // Double rotation: Right(Z) then Left(X)
                } else {
                    N = rotateLeft(X, Z);  // Single rotation Left(X)
                }
            } else {
                if (getBalance(X) < 0) {
                    setBalance(X, 0);  // Z’s height increase is absorbed at X.
                    break;             // Leave the loop
                }
                increaseBalance(X);
                Z = X;  // Height(Z) increases by 1
                continue;
            }
        } else {
            if (getBalance(X) < 0) {
                G = getParent(X);
                if (getBalance(Z) > 0) {
                    N = rotateLeftRight(X, Z);
                } else {
                    N = rotateRight(X, Z);
                }
            } else {
                if (getBalance(X) > 0) {
                    setBalance(X, 0);  // Z’s height increase is absorbed at X.
                    break;             // Leave the loop
                }
                decreaseBalance(X);
                Z = X;  // Height(Z) increases by 1
                continue;
            }
        }
        setAsNewChildOf(tree, G, N, X);
        break;
    }
}

int checkNode(TreeNode* node, TreeNode* parent);
int checkTree(Tree* tree) {
    if (tree == NULL) {
        return -2;
    }
    return checkNode(tree->root, NULL);
}

int checkNode(TreeNode* node, TreeNode* parent) {
    if (node == NULL) {
        return -1;
    }
    if (node->parent != parent) {
        return -2;
    }

    int reportLeft = checkNode(getLeftChild(node), node);
    int reportRight = checkNode(getRightChild(node), node);

    if (reportLeft == -2 || reportRight == -2) {
        return -2;
    }

    if (getLeftChild(node) != NULL) {
        if (getLeftChild(node)->value >= node->value) {
            fprintf(stderr, "Wrong value on the left side.\n");
            return -2;
        }
    }

    if (getRightChild(node) != NULL) {
        if (getRightChild(node)->value <= node->value) {
            fprintf(stderr, "Wrong value on the right side.\n");
            return -2;
        }
    }

    int rightMinusLeft = reportRight - reportLeft;
    if (rightMinusLeft >= 2 || rightMinusLeft <= -2) {
        fprintf(stderr, "Unbalanced tree.\n");
        return -2;
    }
    if (rightMinusLeft != getBalance(node)) {
        fprintf(stderr, "Error in stored balance.\n");
    }
    int maxReport = reportLeft < reportRight ? reportRight : reportLeft;
    return maxReport + 1;
}

uint64_t numberElementsInNode(TreeNode* treeNode) {
    if (treeNode == NULL) {
        return 0;
    }
    uint64_t numberElements = 0;
    if (getLeftChild(treeNode) != NULL) {
        numberElements += numberElementsInNode(getLeftChild(treeNode));
    }
    if (getRightChild(treeNode) != NULL) {
        numberElements += numberElementsInNode(getRightChild(treeNode));
    }
    return numberElements + (uint64_t)1;
}

uint64_t numberElements(Tree* tree) {
    if (tree == NULL) {
        return 0;
    }
    if (tree->root == NULL) {
        return 0;
    }
    return numberElementsInNode(tree->root);
}

void freeTreeNode(TreeNode* treeNode) {
    if (treeNode == NULL) {
        return;
    }
    freeTreeNode(getLeftChild(treeNode));
    freeTreeNode(getRightChild(treeNode));
    free(treeNode);
}

void freeTree(Tree* tree) {
    if (tree == NULL) {
        return;
    }
    freeTreeNode(tree->root);
    free(tree);
}

TreeNode* findNodeInNode(TreeNode* treeNode, uint64_t value) {
    if (treeNode != NULL) {
        if (treeNode->value == value) {
            return treeNode;
        } else {
            if (value < treeNode->value) {
                return findNodeInNode(treeNode->left, value);
            } else {
                return findNodeInNode(treeNode->right, value);
            }
        }
    }
    return NULL;
}

TreeNode* findNode(Tree* tree, uint64_t value) {
    if (tree == NULL) return NULL;
    return findNodeInNode(tree->root, value);
}

TreeNode* getMinInTree(TreeNode* treeNode) {
    if (treeNode == NULL) return NULL;
    if (treeNode->left == NULL) return treeNode;
    return getMinInTree(treeNode->left);
}

TreeNode* getNextHigherInternalNode(TreeNode* treeNode) {
    if (getParent(treeNode) == NULL) {
        return NULL;
    } else {
        if (getLeftChild(getParent(treeNode)) == treeNode) {
            return getParent(treeNode);
        } else {
            return getNextHigherInternalNode(getParent(treeNode));
        }
    }
}

TreeNode* getNext(TreeNode* treeNode) {
    if (treeNode == NULL) {
        return NULL;
    }
    if (treeNode->right == NULL) {
        TreeNode* getHigherNode = getNextHigherInternalNode(treeNode);
        if (getHigherNode == NULL) {
            return NULL;
        } else {
            /*if(getHigherNode->right == NULL){
                return getHigherNode;
            }else {
                return getMinInTree(getHigherNode->right);
            }*/
            return getHigherNode;
        }
    } else {
        return getMinInTree(treeNode->right);
    }
}
