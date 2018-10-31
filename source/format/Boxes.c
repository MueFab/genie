//
// Created by gencom on 17/11/17.
//

#include <stdint.h>

const char fileHeaderBoxName[]="flhd";
const char datasetsGroupBoxName[]="dgcn";
const char datasetsGroupHeaderName[]="dghd";
const char datasetsGroupMetadataName[]="dgmd";
const char datasetsGroupProtectionName[]="dgpr";
const char datasetsGroupReferencesName[]="rfgn";
const char datasetsGroupLabelName[]="lbll";
const char datasetsGroupLabelsListName[]="labl";

const char datasetContainerName[]="dtcn";
const char datasetMasterIndexTableName[]="mitb";
const char datasetHeaderName[]="dthd";
const char datasetMetadataName[]="dtmd";
const char datasetParametersName[]="pars";
const char datasetProtectionName[]="dtpr";

const char streamContainerName[]="dscn";
const char streamHeaderName[]="dshd";
const char streamMetadataName[]="dsmd";
const char streamProtectionName[]="dspt";

const char accessUnitContainerName[]="aucn";
const char accessUnitHeaderName[]="auhd";
const char accessUnitInformationName[]="auin";
const char accessUnitProtectionName[]="aupr";

const uint8_t P_TYPE_AU = 1;
const uint8_t N_TYPE_AU = 2;
const uint8_t M_TYPE_AU = 3;
const uint8_t I_TYPE_AU = 4;
const uint8_t HM_TYPE_AU = 5;
const uint8_t U_TYPE_AU = 6;