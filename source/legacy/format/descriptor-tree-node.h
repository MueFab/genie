#include "DataUnits/data-units.h"
#include "rbtree.h"

#ifndef MPEGG_REFERENCE_SW_DESCRIPTORTREENODE_H
#define MPEGG_REFERENCE_SW_DESCRIPTORTREENODE_H

typedef struct {
    uint8_t descriptorId;
} DescriptorTreeNodeKey;

int initializeDescriptorTreeNode(ldns_rbnode_t **descriptorTreeNode, uint8_t descriptorId);

int descriptorTreeNodeCompareFunction(const void *thisNode, const void *thatNode);
int descriptorTreeNodeCompareFunctionCasted(const DescriptorTreeNodeKey *thisNode,
                                            const DescriptorTreeNodeKey *thatNode);
void freeDescriptorTreeNode(ldns_rbnode_t *node);

#endif  // MPEGG_REFERENCE_SW_DESCRIPTORTREENODE_H
