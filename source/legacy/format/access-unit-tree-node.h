

#ifndef TESTINGREDBLACKTREES_ACCESSUNITTREENODE_H
#define TESTINGREDBLACKTREES_ACCESSUNITTREENODE_H

#include "rbtree.h"
#include "DataUnits/data-units.h"

typedef struct {
    unsigned short sequenceId;
    unsigned long position;
    unsigned char classType;
    uint64_t auId;
} DataUnitAccessUnitTreeNodeKey;

typedef struct{
    DataUnitAccessUnit* dataUnitAccessUnit;
} DataUnitAccessUnitTreeNodeData;

int initializeAccessUnitTreeNode(
        ldns_rbnode_t **pAccessUnitTreeNode,
        SequenceID sequenceId,
        unsigned long position,
        unsigned char classType,
        unsigned long auId,
        DataUnitAccessUnit *dataUnitAccessUnit
);

int accessUnitTreeNodeCompareFunction_Seq_Au_Class(const void *thisNode, const void *thatNode);
int accessUnitTreeNodeCompareFunctionCasted_Seq_Au_Class(
        const DataUnitAccessUnitTreeNodeKey *thisNode,
        const DataUnitAccessUnitTreeNodeKey *thatNode
);

#endif //TESTINGREDBLACKTREES_ACCESSUNITTREENODE_H
