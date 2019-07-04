//
//  aux_compression.c
//  samFastqIO
//
//  Created by Carlos Navarro Astiasarán on 29/08/15.
//  Copyright (c) 2015 Carlos Navarro Astiasarán. All rights reserved.
//

#include "sam_block.h"


//Converts a-z to 0-25, A-Z to 26-51, 0-9 to 52-61. (so that 6bits is enough)
uint8_t charMap(char c) {
    if (c >=97 && c<=122) return c-97;
    else if(c>=65 && c<=90) return c-39;
    else if(c>=48 && c<=57) return c+4;
    else return 63;
}

//Inverse of charMap
char inverseCharMap(uint8_t c) {
    if(c>=0 && c<=25) return c+97;
    else if(c>=26 && c<=51) return c+39;
    else if(c>=52 && c<=61) return c-4;
    else return 0;
}

uint8_t typeMap(char t) {
    int index;
    for(index=0;index<TYPELUTLENGTH;index++)
        if(typeLUT[index]==t) return index;
    return TYPELUTLENGTH;
}


/*   ptr: Points to the first char of the tag.
     The uint16_t output has the mapped data.
     rawType just returns the type.
 */
uint16_t preprocessTagType(char *ptr, uint8_t* rawType) {
    char tagChar1,tagChar2,type;

    tagChar1 = *ptr;
    tagChar2 = *(ptr+1);
    type     = *(ptr+3);
    *rawType = type;

    //We first check whether this TAG:TYPE appears on the LUT
    char tagType[4];
    tagType[0]=tagChar1; tagType[1]=tagChar2; tagType[2]=type; tagType[3]=0;
    int index;
    for(index=0;index<TAGTYPELUTLENGTH;index++)
        if(strcmp(tagTypeLUT[index],tagType)==0) break;

    //index=TAGTYPELUTLENGTH; //force to use NOT FOUND (just 2 check)
    if(index==TAGTYPELUTLENGTH) {
        //not found:
        //1st bit flag = 1,
        //2nd to 7th bit = first char of tag (mapped w/charMap),
        //8th to 13th bit = second char of tag (mapped w/charMap),
        //14th to 16th bit = type (mapped w/LUT).
        return NOTFOUNDLUTFLAG | charMap(tagChar1)<<9 | charMap(tagChar2)<<3 | typeMap(type); //what if typeMap(type) is not mapped to anything? (unknown type) then typeMap(type) = TYPELUTLENGTH

    } else {
        //found:
        //1st bit flag = 0,
        //2nd to 10th bit = 0,
        //11th to 16th bit = index.
        return index;
    }
}



uint16_t preprocessTagType_update(char *ptr, uint8_t* rawType, char (*tagTypeLUT_update)[4], uint8_t max_index) {
    char tagChar1,tagChar2,type;

    tagChar1 = *ptr;
    tagChar2 = *(ptr+1);
    type     = *(ptr+3);
    *rawType = type;
//printf("%c\n",type);
    //We first check whether this TAG:TYPE appears on the LUT
    char tagType[4];
    tagType[0]=tagChar1; tagType[1]=tagChar2; tagType[2]=type; tagType[3]=0;
    int index;
    for(index=0;index<max_index;index++){
        if(strcmp(tagTypeLUT_update[index],tagType)==0) break;
    }

    //index=TAGTYPELUTLENGTH; //force to use NOT FOUND (just 2 check)
    if(index==max_index) {
        //not found:
        //1st bit flag = 1,
        //2nd to 7th bit = first char of tag (mapped w/charMap),
        //8th to 13th bit = second char of tag (mapped w/charMap),
        //14th to 16th bit = type (mapped w/LUT).
        //printf("%c\n",type);printf("%d\n",typeMap(type));
        return NOTFOUNDLUTFLAG | charMap(tagChar1)<<9 | charMap(tagChar2)<<3 | typeMap(type); //what if typeMap(type) is not mapped to anything? (unknown type) then typeMap(type) = TYPELUTLENGTH

    } else {
        //found:
        //1st bit flag = 0,
        //2nd to 10th bit = 0,
        //11th to 16th bit = index.
        return index;
    }
}




uint8_t inversePreprocessTagType(char byte1, char byte2, char *ptr)
{
    uint8_t mappedTagChar1,mappedTagChar2,mappedType;
    char tagChar1,tagChar2,type;

    uint8_t tagTypeIndex;

    uint16_t d = (byte1<<8) | byte2;

    //appears on LUT?
    if (d & NOTFOUNDLUTFLAG) {
        //No
        mappedTagChar1 = (d & MASKTAGCHAR1)>>9;
        mappedTagChar2 = (d & MASKTAGCHAR2)>>3;
        mappedType = (d & MASKTYPE);
        tagChar1 = inverseCharMap(mappedTagChar1);
        tagChar2 = inverseCharMap(mappedTagChar2);
        type = typeLUT[mappedType];
        *ptr=tagChar1;
        *(ptr+1)=tagChar2;
        *(ptr+2)=':';
        *(ptr+3)=type;
        *(ptr+4)=':';
        return 1;
    }

    //Yes
    tagTypeIndex = d & 0xFF;
    *ptr=(char)tagTypeLUT[tagTypeIndex][0];
    *(ptr+1)=(char)tagTypeLUT[tagTypeIndex][1];
    *(ptr+2)=':';
    *(ptr+3)=(char)tagTypeLUT[tagTypeIndex][2];
    *(ptr+4)=':';
    return 0;
}


uint8_t create_most_common_list(sam_block sb) {
    //MOST_COMMON_LIST_ANALIZED_FIELDS
    //MOST_COMMON_LIST_SIZE
    static char list[MOST_COMMON_SEARCHLIST_SIZE+1][MAX_AUX_LENGTH] = {0};
    int duplicates[MOST_COMMON_SEARCHLIST_SIZE+1][2] = {0};

    long currentPosition;

    float entropy = 0.0;
    float prob;

    char buffer[1024];
    char *ptr;

    int different_aux_fields_cnt = 0;
    int total_analyzed_aux_fields_cnt = 0;
    int i,j,n,tmpA,tmpB,ptrLen;
    int appearFlag = 0;
    int maxReachedFlag = 0;
    int analyzedLines = 0;

    //save file pointer to rewind it after analysis is done.
    currentPosition = ftell(sb->fs);

    while (fgets(buffer, 1024, sb->fs) && total_analyzed_aux_fields_cnt<MOST_COMMON_LIST_ANALIZED_FIELDS) {
        ptr = strtok(buffer, "\t");

        for(j=0;j<10;j++)
            ptr = strtok(NULL, "\t");

        while( NULL != (ptr = strtok(NULL, "\t")) ){

            //check if end of line and delete it.
            ptrLen = strlen(ptr);
            if(ptr[ptrLen-1]=='\n') ptr[ptrLen-1] = 0;

                //Check if exists in list, otherwise add.
                appearFlag = 0;
                for (j=0;j<different_aux_fields_cnt;j++) {
                    if( !strcmp(ptr, list[j]) ) {
                        //Appears
                        duplicates[j][0] = j;
                        duplicates[j][1]++;
                        appearFlag = 1;
                        break;
                    }
                }

                if(!appearFlag && different_aux_fields_cnt<MOST_COMMON_SEARCHLIST_SIZE) {
                    //First appearance, add.
                    strcpy(list[different_aux_fields_cnt],ptr);
                    duplicates[different_aux_fields_cnt][0] = different_aux_fields_cnt;
                    duplicates[different_aux_fields_cnt][1] = 1;

                    different_aux_fields_cnt++;

                }

                total_analyzed_aux_fields_cnt++;
            //}

        }
        analyzedLines++;

        if(maxReachedFlag) break;
    }



    // Sort by no. of appearances
    n = different_aux_fields_cnt;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n-i;j++)
        {
            if(duplicates[j][1]<duplicates[j+1][1])
            {
                tmpA=duplicates[j+1][1];
                tmpB=duplicates[j+1][0];
                duplicates[j+1][1]=duplicates[j][1];
                duplicates[j+1][0]=duplicates[j][0];
                duplicates[j][1]=tmpA;
                duplicates[j][0]=tmpB;
            }
        }

    }


    //If there is a point in the ordered list from where all the fields have just 1 appearance we stop the list right here, as it makes no sense to save in the most common list something that appears just once (it would be bad for the compression).
    for(i=0;i<n;i++)
        if(duplicates[i][1]<=100) {
            n = i; break;
        }


    //Copy the first MOST_COMMON_LIST_SIZE values into the list
    if(n>MOST_COMMON_LIST_SIZE)
        n=MOST_COMMON_LIST_SIZE;

    sb->aux->most_common_size = n;

    for (i=0;i<n;i++) {
        strcpy(sb->aux->most_common[i],list[duplicates[i][0]]);
    }

    //Return to original position (1st read)
    fseek(sb->fs, currentPosition, SEEK_SET);


    return 0;
}

//Finds if the aux_field is in the most common list. If so, returns the index, otherwise returns sb->aux->most_common_size.
uint8_t get_most_common_token(char** list, uint32_t list_size, char* aux_field)
{
    uint8_t n = list_size;
    uint8_t k;
    uint8_t token = n;

    for(k=0;k<n;k++)
        if( !strcmp(list[k],aux_field) )
        { token=k; break; }

    return token;
}

// RECONSTRUCTING THE CIGAR AS A FUNCTION OF:
// Dels[], Insers[], numDels, numIns, totalReadLength
// (convertir en funcion)

uint32_t reconstructCigar(uint32_t* Dels, ins* Insers, uint32_t numDels, uint32_t numIns, uint32_t totalReadLength, char* recCigar) {

    cigarIndels cid[MAX_READ_LENGTH];
    //int totalReadLength = 100; //This should be variable

    int c=0,itI=0,itD=0,ci=0,cd=0,k=0,posD=0,posI=0,i=0;
    int actM = 0;

    //array w. pos,type,num for dels and ins, then sort array by pos.
    while (itD<numDels) {
        posD += Dels[itD];
        k=1;
        while((itD+k)<numDels && Dels[itD+k]==0) {
            k++;
        }
        //printf("Pos: %d, Dels: %d\n",posD,k);
        cid[c].pos = posD;
        cid[c].num = k;
        cid[c].letter = 'D';
        itD+=k;
        c++;
    }

    while (itI<numIns) {
        posI += Insers[itI].pos;
        k=1;
        while((itI+k)<numIns && Insers[itI+k].pos==0) {
            k++;
        }
        cid[c].pos = posI;
        cid[c].num = k;
        if(itI==0 && posI == 0)
            cid[c].letter = 'S';
            else
                cid[c].letter = 'I';
                itI+=k;
                c++;
    }

    //sorting
    uint32_t tmpPos, tmpNum;
    int n,j,valM,valN;
    char tmpLetter;
    n = c-1;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n-i;j++)
        {
            if(cid[j].pos>cid[j+1].pos)
            {
                tmpPos    = cid[j+1].pos;
                tmpNum    = cid[j+1].num;
                tmpLetter = cid[j+1].letter;
                cid[j+1].pos    = cid[j].pos;
                cid[j+1].num    = cid[j].num;
                cid[j+1].letter = cid[j].letter;
                cid[j].pos    = tmpPos;
                cid[j].num    = tmpNum;
                cid[j].letter = tmpLetter;
            }
        }
    }

    for (i=0;i<MAX_CIGAR_LENGTH;i++)
        recCigar[i]=0;

    int chrCigarCnt = 0;

    int totalCnt = 0;

    uint32_t index = 0;
    for (i=0;i<c;i++) {
        valM = 0;
        valN = 0;

        //Dels should not be added to the totalCnt as they are just 'absences'
        if(cid[i].pos==0) {
            //assert(strlen(recCigar) == index);
            sprintf(recCigar + index,"%dS",cid[i].num);
            index += compute_num_digits(cid[i].num) + 1;
        } else {
            if (i==0) valM = cid[i].pos;
            else valM = cid[i].pos-cid[i-1].pos;
            assert(strlen(recCigar) == index);
            sprintf(recCigar + index,"%dM",valM);
            index += compute_num_digits(valM) + 1;
            //assert(strlen(recCigar) == index);
            sprintf(recCigar + index,"%d%c",cid[i].num,cid[i].letter);
            index += compute_num_digits(cid[i].num) + 1;
        }

        if (cid[i].letter != 'D') valN = cid[i].num;
        totalCnt += valN + valM;
    }

    return 1;
}
