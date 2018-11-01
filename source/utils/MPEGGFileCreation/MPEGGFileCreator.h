//
// Created by daniel on 1/11/18.
//

#ifndef GENIE_MASTERINDEXTABLECREATOR_H
#define GENIE_MASTERINDEXTABLECREATOR_H
extern "C"{
#include "format/Boxes.h"
}

class MasterIndexTableCreator {
public:
    DatasetMasterIndexTable* createDatasetMasterIndexTable();
};


#endif //GENIE_MASTERINDEXTABLECREATOR_H
