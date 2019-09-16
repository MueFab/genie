#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define P_TYPE_AU 	1
#define N_TYPE_AU 	2
#define M_TYPE_AU 	3
#define I_TYPE_AU 	4
#define HM_TYPE_AU 	5
#define U_TYPE_AU 	6

#define CLASS_P 	1
#define CLASS_N 	2
#define CLASS_M 	3
#define CLASS_I 	4
#define CLASS_HM	5
#define CLASS_U 	6

#define POS_DESCRIPTOR_ID		0
#define RCOMP_DESCRIPTOR_ID		1
#define FLAGS_DESCRIPTOR_ID		2
#define MMPOS_DESCRIPTOR_ID		3
#define MMTYPE_DESCRIPTOR_ID	4
#define CLIPS_DESCRIPTOR_ID		5
#define UREADS_DESCRIPTOR_ID	6
#define RLEN_DESCRIPTOR_ID		7
#define PAIR_DESCRIPTOR_ID		8
#define MSCORE_DESCRIPTOR_ID	9
#define MMAP_DESCRIPTOR_ID		10
#define MSAR_DESCRIPTOR_ID		11
#define RTYPE_DESCRIPTOR_ID		12
#define RGROUP_DESCRIPTOR_ID	13
#define QV_DESCRIPTOR_ID		14
#define RNAME_DESCRIPTOR_ID		15
#define RFTP_DESCRIPTOR_ID		16
#define RFTT_DESCRIPTOR_ID		17

#define NUM_DESCRIPTORS 18

#define subsequence0(ID, POS)   decoded_symbols[ID][0][AU.j[ID][POS]]
#define subsequence1(ID, POS)   decoded_symbols[ID][1][AU.j[ID][POS]]
#define subsequence2(ID, POS)   decoded_symbols[ID][2][AU.j[ID][POS]]
#define subsequence3(ID, POS)   decoded_symbols[ID][3][AU.j[ID][POS]]
#define subsequence4(ID, POS)   decoded_symbols[ID][4][AU.j[ID][POS]]
#define subsequence5(ID, POS)   decoded_symbols[ID][5][AU.j[ID][POS]]
#define subsequence6(ID, POS)   decoded_symbols[ID][6][AU.j[ID][POS]]
#define subsequence7(ID, POS)   decoded_symbols[ID][7][AU.j[ID][POS]]
#define subsequence8(ID, POS)   decoded_symbols[ID][8][AU.j[ID][POS]]
#define subsequence9(ID, POS)   decoded_symbols[ID][9][AU.j[ID][POS]]

#define _subsequence0(ID)    decoded_symbols[ID][0]
#define _subsequence1(ID)    decoded_symbols[ID][1]
#define _subsequence2(ID)    decoded_symbols[ID][2]
#define _subsequence3(ID)    decoded_symbols[ID][3]
#define _subsequence4(ID)    decoded_symbols[ID][4]
#define _subsequence5(ID)    decoded_symbols[ID][5]
#define _subsequence6(ID)    decoded_symbols[ID][6]
#define _subsequence7(ID)    decoded_symbols[ID][7]
#define _subsequence8(ID)    decoded_symbols[ID][8]
#define _subsequence9(ID)    decoded_symbols[ID][9]

#define DEBUG(var) #var << ": " << var

const uint64_t minus1 = -1;

#endif