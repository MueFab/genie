#include "AccessUnitTree.h"

void freeAccessTreeNode(ldns_rbnode_t *node, void *ignored);

AccessUnitTree* initAccessUnitTree(){
    AccessUnitTree* accessUnitTree = (AccessUnitTree*) calloc(1,sizeof(AccessUnitTree));
    if(accessUnitTree == NULL){
        goto error;
    }
    accessUnitTree->AccessUnitTree = ldns_rbtree_create(&accessUnitTreeNodeCompareFunction_Seq_Au_Class);
    if(accessUnitTree->AccessUnitTree == NULL){
        goto error;
    }

    accessUnitTree->currentNumberElements = 0;
    accessUnitTree->allocated_elements = 16;

    accessUnitTree->data = (DataUnitAccessUnitTreeNodeData*)
            malloc(sizeof(DataUnitAccessUnitTreeNodeData) * accessUnitTree->allocated_elements);
    if(accessUnitTree->data == NULL){
        goto error;
    }
    accessUnitTree->keys = (DataUnitAccessUnitTreeNodeKey*)
            malloc(sizeof(DataUnitAccessUnitTreeNodeKey) * accessUnitTree->allocated_elements);
    if(accessUnitTree->keys == NULL){
        goto error;
    }
    return accessUnitTree;

    error:
    free(accessUnitTree->data);
    free(accessUnitTree->keys);
    if(accessUnitTree->AccessUnitTree != NULL){
        ldns_rbtree_free(accessUnitTree->AccessUnitTree);
    }
    free(accessUnitTree);
    return NULL;
}

void freeAccessUnitTree(AccessUnitTree* accessUnitTree){
    if(accessUnitTree == NULL){
        return;
    }
    free(accessUnitTree->data);
    free(accessUnitTree->keys);

    ldns_traverse_postorder(accessUnitTree->AccessUnitTree, &freeAccessTreeNode, NULL);

    if(accessUnitTree->AccessUnitTree != NULL){
        ldns_rbtree_free(accessUnitTree->AccessUnitTree);
    }
    free(accessUnitTree);
}

void freeAccessTreeNode(ldns_rbnode_t *node, void *ignored) {
    free(node);
    (void)(ignored);
}

bool addElement(
        AccessUnitTree* accessUnitTree,
        DataUnitAccessUnitTreeNodeKey dataUnitAccessUnitTreeNodeKey,
        DataUnitAccessUnitTreeNodeData dataUnitAccessUnitTreeNodeData
){
    if(accessUnitTree->allocated_elements == accessUnitTree->currentNumberElements){
        DataUnitAccessUnitTreeNodeData* newDatas = realloc(
                accessUnitTree->data, accessUnitTree->allocated_elements*2
        );
        DataUnitAccessUnitTreeNodeKey* newKeys = realloc(
                accessUnitTree->data, accessUnitTree->allocated_elements*2
        );
        if(newDatas != NULL){
            accessUnitTree->data = newDatas;
        }
        if(newKeys != NULL){
            accessUnitTree->keys = newKeys;
        }
        if(newDatas == NULL || newKeys == NULL){
            return false;
        }
        accessUnitTree->allocated_elements *= 2;
    }
    accessUnitTree->data[accessUnitTree->currentNumberElements] = dataUnitAccessUnitTreeNodeData;
    accessUnitTree->keys[accessUnitTree->currentNumberElements] = dataUnitAccessUnitTreeNodeKey;


    ldns_rbnode_t* new_node = (ldns_rbnode_t*)calloc(1,sizeof(ldns_rbnode_t));
    new_node->data = &(accessUnitTree->data[accessUnitTree->currentNumberElements]);
    new_node->key = &(accessUnitTree->keys[accessUnitTree->currentNumberElements]);

    accessUnitTree->currentNumberElements++;
    ldns_rbtree_insert(
            accessUnitTree->AccessUnitTree,
            new_node
    );
    return true;
}

ldns_rbnode_t* getFirstAccessUnitNode(AccessUnitTree* accessUnitTree){
    return ldns_rbtree_first(accessUnitTree->AccessUnitTree);
}

ldns_rbnode_t* getNextAccessUnitNode(ldns_rbnode_t* currentNode){
    return ldns_rbtree_next(currentNode);
}


