//
// Created by bscuser on 14/02/18.
//

#ifndef MPEGG_CAPSULATOR_TREEUINT64_H
#define MPEGG_CAPSULATOR_TREEUINT64_H

#ifdef __APPLE_CC__
#include <stdint.h>
#else
#include <stdint-gcc.h>
#endif
#include <stdbool.h>

typedef struct TreeNode_ TreeNode;
struct TreeNode_ {
    TreeNode* parent;
    TreeNode* left;
    TreeNode* right;
    uint64_t value;
    int8_t balance;
};

typedef struct {
    TreeNode* root;
} Tree;

TreeNode* initTreeNode(uint64_t value);
Tree* initTree();
bool insert(Tree* tree, TreeNode* treeNode);
int checkTree(Tree* tree);
uint64_t numberElements(Tree* tree);
void freeTree(Tree* tree);
TreeNode* findNode(Tree* tree, uint64_t value);
TreeNode* getNext(TreeNode* treeNode);
TreeNode* findNodeInNode(TreeNode* treeNode, uint64_t value);
TreeNode* getMinInTree(TreeNode* treeNode);
TreeNode* getNextHigherInternalNode(TreeNode* treeNode);

#endif  // MPEGG_CAPSULATOR_TREEUINT64_H
