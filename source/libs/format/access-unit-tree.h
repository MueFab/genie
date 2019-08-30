#ifndef MPEGG_REFERENCE_SW_ACCESSUNITTREE_H
#define MPEGG_REFERENCE_SW_ACCESSUNITTREE_H

#include "access-unit-tree-node.h"
#include "rbtree.h"

typedef struct {
    ldns_rbtree_t* AccessUnitTree;
    DataUnitAccessUnitTreeNodeKey* keys;
    DataUnitAccessUnitTreeNodeData* data;
    size_t allocated_elements;
    size_t currentNumberElements;
} AccessUnitTree;

AccessUnitTree* initAccessUnitTree();
void freeAccessUnitTree(AccessUnitTree* accessUnitTree);
bool addElement(AccessUnitTree* accessUnitTree, DataUnitAccessUnitTreeNodeKey dataUnitAccessUnitTreeNodeKey,
                DataUnitAccessUnitTreeNodeData dataUnitAccessUnitTreeNodeData);
ldns_rbnode_t* getFirstAccessUnitNode(AccessUnitTree* accessUnitTree);
ldns_rbnode_t* getNextAccessUnitNode(ldns_rbnode_t* currentNode);

#endif  // MPEGG_REFERENCE_SW_ACCESSUNITTREE_H
