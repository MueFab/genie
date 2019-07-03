//
//  aux_data.h
//  samFastqIO
//
//  Created by Carlos Navarro Astiasarán on 14/10/15.
//  Copyright (c) 2015 Carlos Navarro Astiasarán. All rights reserved.
//

#ifndef samFastqIO_aux_data_h
#define samFastqIO_aux_data_h


//idoia
#define MAXLUT 100
#define TAGTYPELUTLENGTH 44
//#define TAGTYPELUTLENGTH 43
//idoia
#define TYPELUTLENGTH 6
#define NOTFOUNDLUTFLAG 0x8000
#define MASKTAGCHAR1 0x7E00
#define MASKTAGCHAR2 0x01F8
#define MASKTYPE 0x0007
#define MAX_AUX_FIELDS 20 //max. aux fields in a read. If there are more, they will be ignored.
#define MAX_AUX_LENGTH UINT16_MAX //max. length of an aux field
#define MOST_COMMON_LIST_SIZE 200 //MAX 255. No. of elements in the most common list (elements in this list will be mapped to a number from 0 to MOST_COMMON_LIST_SIZE-1).
#define MOST_COMMON_LIST_ANALIZED_FIELDS 500000 //No. of aux fields that will be analyzed in order to generate the list.
#define MOST_COMMON_SEARCHLIST_SIZE 1000 //No. of different aux fields that will be analyzed. If there are more than this, they wont be analyzed. Ideally, this value could be the same as MOST_COMMON_LIST_ANALIZED_FIELDS (just in case all the aux fields are repeated just once) but this is both not likely and a waste of memmory.

#define MAX_CIGAR_LENGTH 2048 //Twice as long as MAX_READ_LENGTH


/*
 // Aux. format: TAG:TYPE:VALUE
 
 //     tag: [a-zA-Z][a-zA-Z0-9],
 //    type: [A,i,f,Z,H,B],
 */

#include <stdint.h>

//lookup table corresponding to the most used tag-type pairs given in the specification (SAMv1.pdf)
//static char tagTypeLUT[TAGTYPELUTLENGTH][4] = {"AMi","ASi","BCZ","BQZ","CCZ","CMi","COZ","CPi","CQZ","CSZ","CTZ","E2Z","FIi","FSZ","FZB","LBZ","H0i","H1i","H2i","HIi","IHi","MCZ","MDZ","MQi","NHi","NMi","OQZ","OPi","OCZ","PGZ","PQi","PTZ","PUZ","QTZ","Q2Z","R2Z","RGZ","RTZ","SAZ","SMi","TCi","U2Z","UQi"};
static char tagTypeLUT[TAGTYPELUTLENGTH][4] = {"AMi","ASi","BCZ","BQZ","CCZ","CMi","COZ","CPi","CQZ","CSZ","CTZ","E2Z","FIi","FSZ","FZB","FZS","LBZ","H0i","H1i","H2i","HIi","IHi","MCZ","MDZ","MQi","NHi","NMi","OQZ","OPi","OCZ","PGZ","PQi","PTZ","PUZ","QTZ","Q2Z","R2Z","RGZ","RTZ","SAZ","SMi","TCi","U2Z","UQi"};

//available types
static char typeLUT[]="AifZHB";

uint16_t preprocessTagType(char *ptr, uint8_t* rawType);
//idoia
uint16_t preprocessTagType_update(char *ptr, uint8_t* rawType, char (*tagTypeLUT_update)[4], uint8_t max_index);
//idoia
uint8_t charMap(char c);
char inverseCharMap(uint8_t c);
uint8_t typeMap(char t);

static int globalCnt = 0;

#endif
