#include <stdlib.h>
#include "AccessUnitTreeNode.h"

void freeAccessUnitTreeNode(
        ldns_rbnode_t *node
);

int initializeAccessUnitTreeNode(
        ldns_rbnode_t **pAccessUnitTreeNode,
        SequenceID sequenceId,
        unsigned long position,
        unsigned char classType,
        unsigned long auId,
        DataUnitAccessUnit *dataUnitAccessUnit
){

    DataUnitAccessUnitTreeNodeKey* pAccessUnitTreeNodeKey =
            (DataUnitAccessUnitTreeNodeKey*)malloc(sizeof(DataUnitAccessUnitTreeNodeKey));
    if(pAccessUnitTreeNodeKey == NULL){
        return -1;
    }
    DataUnitAccessUnitTreeNodeData* pAccessUnitTreeNodeData =
            (DataUnitAccessUnitTreeNodeData*)malloc(sizeof(DataUnitAccessUnitTreeNodeData));
    if(pAccessUnitTreeNodeData == NULL){
        free(pAccessUnitTreeNodeKey);
        return -1;
    }

    *pAccessUnitTreeNode = (ldns_rbnode_t*)malloc(sizeof(ldns_rbnode_t));
    if(*pAccessUnitTreeNode == NULL){
        return -1;
    }

    (*pAccessUnitTreeNodeKey).sequenceId = sequenceId.sequenceID;
    (*pAccessUnitTreeNodeKey).position = position;
    (*pAccessUnitTreeNodeKey).classType = classType;
    (*pAccessUnitTreeNodeKey).auId = auId;
    (*pAccessUnitTreeNodeData).dataUnitAccessUnit = dataUnitAccessUnit;

    (*pAccessUnitTreeNode)->key = pAccessUnitTreeNodeKey;
    (*pAccessUnitTreeNode)->data = pAccessUnitTreeNodeData;

    return 0;
}
void freeAccessUnitTreeNode(
        ldns_rbnode_t *node
){
    free(node);
}



int accessUnitTreeNodeCompareFunction_Seq_Au_Class(const void *thisNode, const void *thatNode){
    return accessUnitTreeNodeCompareFunctionCasted_Seq_Au_Class(
            (const DataUnitAccessUnitTreeNodeKey *) thisNode,
            (const DataUnitAccessUnitTreeNodeKey *) thatNode
    );
}
int accessUnitTreeNodeCompareFunctionCasted_Seq_Au_Class(
        const DataUnitAccessUnitTreeNodeKey *thisNode,
        const DataUnitAccessUnitTreeNodeKey *thatNode
){
    if(thisNode->sequenceId != thatNode->sequenceId){
        if(thisNode->sequenceId < thatNode->sequenceId){
            return -1;
        }else{
            return 1;
        }
    } else {
        if(thisNode->auId != thatNode->auId){
            if(thisNode->auId < thatNode->auId){
                return -1;
            }else{
                return 1;
            }
        } else {
            if(thisNode->classType == thatNode->classType) {
                return 0;
            }else {
                if (thisNode->classType < thatNode->classType) {
                    return -1;
                } else {
                    return 1;
                }
            }
        }
    }
}

