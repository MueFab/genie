#include "descriptor-tree-node.h"
#include <stdlib.h>

void freeDescriptorTreeNodeKeyCasted(DescriptorTreeNodeKey *pClassTreeNodeKey);

int initializeDescriptorTreeNode(ldns_rbnode_t **descriptorTreeNode, uint8_t descriptorId) {
    DescriptorTreeNodeKey *pClassTreeNodeKey = (DescriptorTreeNodeKey *)malloc(sizeof(DescriptorTreeNodeKey));
    if (pClassTreeNodeKey == NULL) {
        return -1;
    }

    *descriptorTreeNode = (ldns_rbnode_t *)malloc(sizeof(ldns_rbnode_t));
    if (*descriptorTreeNode == NULL) {
        return -1;
    }

    (*pClassTreeNodeKey).descriptorId = descriptorId;

    (*descriptorTreeNode)->key = pClassTreeNodeKey;
    (*descriptorTreeNode)->data = NULL;

    return 0;
}

int descriptorTreeNodeCompareFunction(const void *thisNode, const void *thatNode) {
    return descriptorTreeNodeCompareFunctionCasted((const DescriptorTreeNodeKey *)thisNode,
                                                   (const DescriptorTreeNodeKey *)thatNode);
}
int descriptorTreeNodeCompareFunctionCasted(const DescriptorTreeNodeKey *thisNode,
                                            const DescriptorTreeNodeKey *thatNode) {
    if (thisNode->descriptorId != thatNode->descriptorId) {
        if (thisNode->descriptorId < thatNode->descriptorId) {
            return -1;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}
