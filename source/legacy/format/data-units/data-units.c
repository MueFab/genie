#include <utils.h>
#include "data-units.h"
#define INITIAL_PARAMETER_IDS_INFO_ALLOCATION 20

DataUnits* initDataUnits(){
    DataUnits* dataUnits = (DataUnits*)malloc(sizeof(DataUnits));
    if(dataUnits == NULL){
        return NULL;
    }
    dataUnits->parameters = initVector();
    if(dataUnits->parameters == NULL){
        goto freeStructure;
    }
    dataUnits->dataUnitsAccessUnits = initVector();
    if(dataUnits->dataUnitsAccessUnits == NULL){
        goto freeDataUnitParameters;
    }


    dataUnits->allocatedDataUnitParametersIdsInfo.allocatedElements = INITIAL_PARAMETER_IDS_INFO_ALLOCATION;
    dataUnits->allocatedDataUnitParametersIdsInfo.datasetgroupid = malloc(
            sizeof(uint8_t)*INITIAL_PARAMETER_IDS_INFO_ALLOCATION
    );
    dataUnits->allocatedDataUnitParametersIdsInfo.datasetid = malloc(
            sizeof(uint16_t)*INITIAL_PARAMETER_IDS_INFO_ALLOCATION
    );
    dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DatasetScope = malloc(
            sizeof(uint16_t)*INITIAL_PARAMETER_IDS_INFO_ALLOCATION
    );
    dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DataUnitsScope = malloc(
            sizeof(uint16_t)*INITIAL_PARAMETER_IDS_INFO_ALLOCATION
    );
    dataUnits->allocatedDataUnitParametersIdsInfo.createdElements = 0;

    return dataUnits;

    freeDataUnitParameters: freeVector(dataUnits->parameters);
    freeStructure: free(dataUnits);
    return NULL;
}

bool getParametersIdDataUnitsScope(
        DataUnits *dataUnits,
        uint8_t datasetgroupId,
        uint16_t datasetId,
        uint8_t parameterIdDatasetScope,
        uint8_t *parameterIdDataUnitsScope
){
    for(
        size_t parameterIds_i=0;
        parameterIds_i < dataUnits->allocatedDataUnitParametersIdsInfo.createdElements;
        parameterIds_i++
    ){
        if(
            datasetgroupId == dataUnits->allocatedDataUnitParametersIdsInfo.datasetgroupid[parameterIds_i]
            && datasetId == dataUnits->allocatedDataUnitParametersIdsInfo.datasetid[parameterIds_i]
            && parameterIdDatasetScope ==
                dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DatasetScope[parameterIds_i]
        ){
            *parameterIdDataUnitsScope = dataUnits->allocatedDataUnitParametersIdsInfo
                    .parameterSetId_DataUnitsScope[parameterIds_i];
            return true;
        }
    }
    return false;
}

uint8_t createParameterSetIdDataUnitsScope(
        DataUnits* dataUnits,
        uint8_t datasetgroupId,
        uint16_t datasetId,
        uint8_t parameterIdDatasetScope
){
    uint8_t index = dataUnits->allocatedDataUnitParametersIdsInfo.createdElements;
    dataUnits->allocatedDataUnitParametersIdsInfo.datasetgroupid[index] = datasetgroupId;
    dataUnits->allocatedDataUnitParametersIdsInfo.datasetid[index] = datasetId;
    dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DatasetScope[index] = parameterIdDatasetScope;
    dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DataUnitsScope[index] = (uint8_t)index;

    if(index == dataUnits->allocatedDataUnitParametersIdsInfo.allocatedElements){
        dataUnits->allocatedDataUnitParametersIdsInfo.datasetgroupid = realloc(
                dataUnits->allocatedDataUnitParametersIdsInfo.datasetgroupid,
                sizeof(uint8_t)*(index + INITIAL_PARAMETER_IDS_INFO_ALLOCATION)
        );
        dataUnits->allocatedDataUnitParametersIdsInfo.datasetid = realloc(
                dataUnits->allocatedDataUnitParametersIdsInfo.datasetid,
                sizeof(uint16_t)*(index + INITIAL_PARAMETER_IDS_INFO_ALLOCATION)
        );
        dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DatasetScope = realloc(
                dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DatasetScope,
                sizeof(uint16_t)*(index + INITIAL_PARAMETER_IDS_INFO_ALLOCATION)
        );
        dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DataUnitsScope = realloc(
                dataUnits->allocatedDataUnitParametersIdsInfo.parameterSetId_DataUnitsScope,
                sizeof(uint16_t)*(index + INITIAL_PARAMETER_IDS_INFO_ALLOCATION)
        );
    }
    dataUnits->allocatedDataUnitParametersIdsInfo.createdElements++;
    return index;
}

void freeDataUnits(DataUnits* dataUnits){
    if(dataUnits == NULL){
        return;
    }
    if(dataUnits->parameters != NULL){
        size_t numberParameters = getSize(dataUnits->parameters);
        for(size_t param_i=0; param_i < numberParameters; param_i++){
            DataUnitParametersSet* parametersSet = getValue(dataUnits->parameters, param_i);
            if(parametersSet != NULL) {
                freeDataUnitsParametersSet(parametersSet);
            }
        }
        freeVector(dataUnits->parameters);
    }
    if(dataUnits->dataUnitsAccessUnits != NULL){
        size_t numberDataUnits = getSize(dataUnits->dataUnitsAccessUnits);
        for(size_t access_unit_i=0; access_unit_i < numberDataUnits; access_unit_i++){
            DataUnitAccessUnit* dataUnitAccessUnit = getValue(dataUnits->dataUnitsAccessUnits, access_unit_i);
            if(dataUnitAccessUnit != NULL){
                freeDataUnitAccessUnit(dataUnitAccessUnit);
            }
        }
        freeVector(dataUnits->dataUnitsAccessUnits);
    }
    free(dataUnits);
}

int parseDataUnits(FILE* inputFile, DataUnits** dataUnits, char* filename){
    *dataUnits = initDataUnits();
    if(*dataUnits == NULL){
        fprintf(stderr, "Data units could not be allocated.\n");
        return -1;
    }

    uint8_t dataUnitType;
    utils_read(&dataUnitType, inputFile);
    while(!feof(inputFile)){
        if(dataUnitType == 0){
            fprintf(stderr, "Unsupport data unit\n.");
            freeDataUnits(*dataUnits);
            return -1;
        }else if( dataUnitType == 1){
            uint32_t size;
            if(!readBigEndian32FromFile(&size, inputFile)){
                fprintf(stderr, "Error reading input\n.");
                freeDataUnits(*dataUnits);
                return -1;
            }

            DataUnitParametersSet* parametersSet;
            parseDataUnitsParametersSet(&parametersSet, inputFile, size - 5);
            if(parametersSet == NULL){
                return -1;
            }
            addDataUnitParameters(*dataUnits, parametersSet);


        } else if(dataUnitType == 2){
            uint32_t size;
            if(!readBigEndian32FromFile(&size, inputFile)){
                fprintf(stderr, "Error reading input\n.");
                freeDataUnits(*dataUnits);
                return -1;
            }
            DataUnitAccessUnit* dataUnitAccessUnit;
            parseDataUnitAccessUnit(&dataUnitAccessUnit, *dataUnits, false, inputFile, filename, size);
            addDataUnitAccessUnit(*dataUnits, dataUnitAccessUnit);
        } else {
            fprintf(stderr, "Unknown data unit.\n");
            freeDataUnits(*dataUnits);
            return -1;
        }
        utils_read(&dataUnitType, inputFile);
    }
    return 0;
}

int addDataUnitDatasetParameters(DataUnits *dataUnits, DatasetParameters *datasetParameters,
                                 uint8_t *parametersIdDataUnitsScope) {
    uint8_t parentParameterIdScopeDataUnits;
    if (!getParametersIdDataUnitsScope(
            dataUnits,
            datasetParameters->dataset_group_ID,
            datasetParameters->dataset_ID,
            datasetParameters->parent_parameter_set_ID,
            &parentParameterIdScopeDataUnits
    )){
        return -1;
    }

    uint8_t parameterIdScopeDataUnits = createParameterSetIdDataUnitsScope(
            dataUnits,
            datasetParameters->dataset_group_ID,
            datasetParameters->dataset_ID,
            datasetParameters->parameter_set_ID
    );
    *parametersIdDataUnitsScope = parameterIdScopeDataUnits;

    DataUnitParametersSet* dataUnitParameterSet = initParametersSet(
            parentParameterIdScopeDataUnits,
            parameterIdScopeDataUnits,
            datasetParameters->encoding_parameters
    );

    return addDataUnitParameters(dataUnits, dataUnitParameterSet);
}

int addDataUnitParameters(DataUnits* dataUnits, DataUnitParametersSet* parametersSet){
    return pushBack(dataUnits->parameters, parametersSet);
}

int addDataUnitAccessUnit(DataUnits* dataUnits, DataUnitAccessUnit* dataUnitAccessUnit){
    return pushBack(dataUnits->dataUnitsAccessUnits, dataUnitAccessUnit);
}
int getDataUnitsParameters(DataUnits* dataUnits, Vector** dataUnitsParameters){
    *dataUnitsParameters = dataUnits->parameters;
    return 0;
}
int getDataUnitsAccessUnits(DataUnits* dataUnits, Vector** dataUnitsAccessUnits){
    *dataUnitsAccessUnits = dataUnits->dataUnitsAccessUnits;
    return 0;
}
int getDataUnitsParametersById(DataUnits* dataUnits, uint16_t parameterSetId, DataUnitParametersSet** parametersSet){
    *parametersSet = NULL;
    if(dataUnits == NULL){
        return -1;
    }
    size_t parametersLength = getSize(dataUnits->parameters);
    for(uint16_t parameter_i = 0; parameter_i<parametersLength; parameter_i++){
        DataUnitParametersSet* fetchedParameters = getValue(dataUnits->parameters, parameter_i);
        if(fetchedParameters->parameter_setId == parameterSetId){
            *parametersSet = fetchedParameters;
            return 0;
        }
    }
    return -1;
}

bool writeDataUnits(DataUnits* dataUnits, FILE* outputStream){
    size_t numberDataUnitParameters = getSize(dataUnits->parameters);
    for(size_t dataUnitParameter_i = 0; dataUnitParameter_i < numberDataUnitParameters; dataUnitParameter_i++){
        DataUnitParametersSet *dataUnitParameterSet = getValue(dataUnits->parameters, dataUnitParameter_i);
        bool parametersWriteSuccessful = writeParametersSet(dataUnitParameterSet, outputStream);
        if(!parametersWriteSuccessful){
            fprintf(stderr, "Could not write parameters\n");
            return false;
        }
    }
    size_t numberAccessUnits = getSize(dataUnits->dataUnitsAccessUnits);
    for(size_t dataUnitAccessUnit_i = 0; dataUnitAccessUnit_i < numberAccessUnits; dataUnitAccessUnit_i++){
        DataUnitAccessUnit* dataUnitAccessUnit = getValue(dataUnits->dataUnitsAccessUnits, dataUnitAccessUnit_i);
        DataUnitParametersSet* parametersSet;
        if(getDataUnitsParametersById(dataUnits, dataUnitAccessUnit->parameterSetId, &parametersSet)){
            fprintf(stderr, "Could not obtain parameters\n");
        }
        bool accessUnitSuccessfulWrite = writeDataUnitAccessUnit(
                dataUnitAccessUnit,
                parametersSet->encodingParameters->multiple_alignments_flag,
                parametersSet->encodingParameters->dataset_type,
                parametersSet->encodingParameters->pos_40_bits,
                outputStream
        );
        if(!accessUnitSuccessfulWrite){
            fprintf(stderr, "Could not write Access Unit\n");
            return false;
        }
    }
    return true;
}

