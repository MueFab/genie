#include <vector>
#include <string>
//#include "DataUnits.h"

// TODO: check if standard C++ libraries are used, or if I should switch to boost or something (do not use boost)

// returns MPEG-G record (Section 13 page 165)
// access unit structure as parameter
// 

// access unit implementation in https://github.com/mitogen/genie/blob/conformance/source/format/DataUnits/DataUnits.h

// FOR WIDTH CHECK PAGE 165 - or use almost always 32 bits



// dumb-proof it at some point
void zeroOrderComplement(uint64_t idx) 
{
	if(idx == 4) return 4;
	return 3-idx;
}

// Based on Table 36, page 49
void firstOrderComplement(uint64_t idx) // todo: make it more fancy/compact by putting stuff into an array
										// also dumb proof	
{
	if(idx <= 3) return 3-idx;
	if(idx <= 5) return 5-idx  + 4; 
	if(idx <= 7) return idx;
	if(idx <= 9) return 9-idx + 8;
	if(idx <= 13) return 13-idx + 10;
	return idx;
}

/*
// Don't need that for now

// So that we don't have to initialize vectors
template<class T>
void change(std::vector<T> &vec, uint64_t idx, const &T newvalue) {
	while(vec.size() <= idx) vec.push_back(T());
	vec[idx] = newvalue;
}
*/

// Supported symbols
void initializeSupportedSymbolsVector(std::vector<std::string> &supportedSymbols) 
{
	supportedSymbols = std::vector<std::string>(2, "");
	supportedSymbols[0] = "ACGTN";
	supportedSymbols[1] = "ACGTRYSWKMBDHVN-";
}

void decode(/* TODO: properly add parameters here */ /* access unit */) 
{
	// Step 1: set a classId variable equal to the value of AU_type
	uint64_t classId = AU_type;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////
	// reserved for calling gabac
	// decoded_symbol[8][0][j80] <---- produced by gabac code
	//
	///////////////

	// Step 2: decode the following variables:
		// numberOfRecordSegments
		// numberOfAlignedRecordSegments
		// unpairedRead
	
	// To calculate these values, we first need to calculate numberOfTemplateSegments
	uint64_t numberOfTemplateSegments = number_of_template_segments_minus1+1;

	// Sub-Step 2.1: decoding numberOfRecordSegments
	uint64_t numberOfRecordSegments;
	if(numberOfTemplateSegments == 1) numberOfRecordSegments = 1;
	else if(classId == class_HM) numberOfRecordSegments = 2; // For now not going to be the case, since we are only concerned about Class P
	else if(subsequence0[j80] == 0) numberOfRecordSegments = 2;
	else numberOfRecordSegments = 1;

	// Sub-Step 2.1 TODOs: 
	// Figure out how to properly perform the 3rd check, i.e. else if(subsequence0[j80] == 0)

	// Sub-Step 2.2: decoding numberOfAlignedRecordSegments
	uint64_t numberOfAlignedRecordSegments;
	if(classId == Class_HM) numberOfAlignedRecordSegments = 1; // Here for clarity; we are focusing on the Class P case for now
	else if(classId == Class_U) numberOfAlignedRecordSegments = 0; // Same as above
	else numberOfAlignedRecordSegments = numberOfRecordSegments; // Since we are in Class P for now (perfectly aligned reads) this makes sense and will always be the case

	

	// Sub-Step 2.3: decoding unpairedRead
	uint64_t unpairedRead;
	if(classId == Class_HM) unpairedRead = 0; // Here for clarity
	else if(numberOfTemplateSegments == 1 || subsequence0[j80] == 5 || subsequence0[j80] == 6) unpairedRead = 1;
	else unpairedRead = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 3: compute the following 2D arrays:
		// softClips (dims: numberOfAlignedRecordSegments x 2)
		// softClipSizes (dims: numberOfAlignedRecordSegments x 2)
		// hardClips (dims: numberOfAlignedRecordSegments x 2)

	// Lets create the arrays first of all
	// TODO: check if we should return them, or if they will be passed as parameters; this step might not be necessary if the latter is the case
	std::vector<std::vector<std::string> > softClips(numberOfAlignedRecordSegments);
	std::vector<std::vector<uint64_t> > softClipSizes(numberOfAlignedRecordSegments), hardClips(numberOfAlignedRecordSegments); // Might be able to pull off using an integer type with less bits (to save space and stuff)

	for(uint64_t i = 0; i < numberOfAlignedRecordSegments; ++i) {
		softClips[i] = std::vector<std::string>(2, "");
		softClipSizes[i] = hardClips[i] = std::vector<uint64_t>(2, 0);
		
	}

	// Sub-Step 3.1, 3.2, 3.3: computing softClips, softClipSizes, hardClips
	std::vector<std::string> S;
	uint64_t currentRecordCount = 0; // possibly can pull off using shorter integer width

	initializeSupportedSymbolsVector(S);

	if(classId == Class_I || classId == Class_HM) {
		if(j50 < subsequence0.size() && currentRecordCount == subsequence0[j50]) 
		// one can only hope that subsequence0 (or, more accurately decoded_symbol[][][]) is a vector
	    // also what the frick is currentRecordCount
		{
			bool end = false;
			uint64_t segmentIdx;
			uint64_t leftRightIdx;
			do {
				if(subsequence1[j51] <= 3) {
					segmentIdx = (subsequence1[j51] >> 1); // divide by 2 (why tho)
					leftRightIdx = (subsequence1[j51] & 1); // take modulo 2 (why thoo)
					do {
						softClips[segmentIdx][leftRightIdx].push_back(S[alphabet_ID][subsequence2[j52]]); // NOTE: alphabet_ID is a parameter
						j52++;
					} while(subsequence2[j52] != S[alphabet_ID].size())
					j52++;
					softClipSizes[segmentIdx][leftRightIdx] = softClipSizes[segmentIdx][leftRightIdx].size();
				} else if(subsequence1[j51] <= 7) {
					segmentIdx = ((subsequence1[j51] - 4) >> 1);
					leftRightIdx = ((subsequence1[j51] - 4) & 1);

				}
				
			} while(!end);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 4: compute the following arrays:
		// readLength
		// numberOfSplicedSeg
		// splicedSegLength

	std::vector<uint64_t> readLength; // TODO: check width 
	std::vector<uint64_t> numberOfSplicedSeg; // TODO: check width
	std::vector<std::vector<uint64_t> > splicedSegLength; // TODO: check width
	uint64_t remainingLen;

	if(read_length == 0) {
		for(uint64_t i=0; i<numberOfRecordSegments; ++i)
			readLength.push_back(subsequence0[j70++]+1);
	} else {
		for(uint64_t i=0; i<numberOfRecordSegments; ++i) {
			if(classId == class_I)
				readLength.push_back(read_length - hardClips[i][0] - hardClips[i][1]);
			else if(classId == Class_HM && i == 0) 
				readLength.push_back(read_length - hardClips[0][0] - hardClips[0][1]);
			else
				readLength.push_back(read_length);
		}
	}

	for(uint64_t i=0; i<numberOfRecordSegments; ++i) {
		numberOfSplicedSeg.push_back(1);
		splicedSegLength.push_back(std::vector<uint64_t>(1, readLength[i]));
	}

	if(spliced_reads_flag && (classId == Class_I || classId == Class_HM)) {
		for(uint64_t i=0; i<numberOfAlignedRecordSegments; ++i) {
			if(read_length == 0) // notice: numberOfAlignedRecordSegments <= numberOfRecordSegments
				readLength[i] = subsequence0[j70++]+1;
			remainingLen = readLength[i];
			uint64_t j = 0;

			do {
				uint64_t spliceLen = subsequence0[j70++]; // ASK WIDTH
				remainingLen -= spliceLen;

				// while(splicedSegLength[i].size() <= j) // ASK if this is intended; these arrays are too sparce if I understand correctly.
				// 	splicedSegLength.push_back(0);

				splicedSegLength[i][j++] = spliceLen;
				splicedSegLength[i].push_back(0);
			} while(remainingLen > 0);

			numberOfSplicedSeg[i] = j;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 5: Decode output variables containing alignment information

	std::vector<uint64_t> numberOfSegmentAlignments, numberOfAlignmentPairs; // TODO: check width
	std::vector<std::vector<uint64_t> > alignPtr;
	uint64_t numberOfAlignments; // = 0 ?

	if(classId != Class_U) {
		if(multiple_alignment_flag == 0)
			numberOfSegmentAlignments.push_back(1);
		else
			numberOfSegmentAlignments.push_back(sequence0[j100++]);

	} else {
		numberOfSegmentAlignments.push_back(0);
	}

	uint64_t moreAlignments = 0, 
			 moreAlignmentsNextSeqId = 0, 
			 moreAlignmentsNextPos = 0;

	if(unpairedRead || classId == Class_HM) {
		numberOfAlignments = numberOfSegmentAlignments[0];
		for(uint64_t i=0; i<numberOfAlignments; ++i)
			alignPtr.push_back(std::vector<uint64_t>(1, i));
	} else if(classId == class_U) {
		if(numberOfRecordSegments > 1)
			numberOfSegmentAlignments.push_back(0);
		numberOfAlignments = 0;
	} else {
		numberOfSegmentAlignments[1] = 0;
		uint64_t k = 0, i = 0;
		while(i < numberOfSegmentAlignments[0]) {
			if(multiple_alignment_flag == 0)
				numberOfAlignmentPairs.push_back(1);
			else
				numberOfAlignmentPairs.push_back(sequence0[j100++]);
		
			uint64_t j = 0, ptr;

			while(j < numberOfAlignmentPairs[i]) {
				// skip this for first alignment ??? what does that mean
				if(i != 0)
					ptr = sequence1[j100++];
				else
					ptr = 0;

				alignPtr[k].push_back(numberOfSegmentAlignments[1] - ptr);
				alignPtr[k][0] = i;

				if(ptr == 0)
					numberOfSegmentAlignments[1]++;

				j++, k++;

		
			}

			i++;
		}

		numberOfAlignments = k;

	}

	if(multiple_alignment_flag == 1 && 
		classId != Class_U && 
		subsequence2[j102++]) 

	{
		moreAlignments = 1;
		moreAlignmentsNextSeqId = subsequence3[j103++];
		moreAlignmentsNextPos = subsequence4[j104++];
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 6: decode the pos descriptor
	// previousMappingPos0 is going to be a parameter, but also we can change it
	// ask

	std::vector<std::vector<uint64_t> > mappingPos;

	if(j00 > 0)
		mappingPos.push_back(std::vector<uint64_t>(1, 0));
	else
		mappingPos.push_back(std::vector<uint64_t>(AU_start_position+subsequence0[j00]));

	previousMappingPos0 = mappingPos[0][0];

	for(uint64_t i=1; i<numberOfSegmentAlignments[0]; ++i) {
		mappingPos[i][0] = mappingPos[i-1][0]+subsequence1[j01];
		j01++;
	}

	j00++;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 7: Decode the output variables containing pairing and/or splicing information

	// This means that we have to decode:
		// read1First
		// splitMate[][i] for i in [1, numberOfTemplateSegments]
		// splicedSegMappingPos[i][0] for i in [0, numberOfRecordSegments]

		// when classId \in {Class_P, Class_N, Class_M, Class_I}

		// mappingPos[][i] for i in [1, numberOfTemplateSegments]
		// mateSeqId[][i] for i in [1, numberOfTemplateSegments]

		// when classId = Class_U

		// mateRecordIndex[i] for i in [1, numberOfRecordSegments]
		// mateAuId[i] for i in [1, numberOfRecordSegments]
		// NOT GONNA HAVE THAT HERE (remember, we are just doing class P stuff), however ask if I should

	uint64_t read1First, delta;
	std::vector<std::vector<uint64_t> > splitMate, mateSeqId;

	splitMate.push_back(std::vector<uint64_t>(1, 0));
	mateSeqId.push_back(std::vector<uint64_t>(1, 0));

	if(classId != Class_HM) {
		for(uint64_t i=1; i<numberOfTemplateSegments; ++i) {
			if(subsequence0[j80] == 0) {
				splitMate[0].push_back(0);

				if(classId != class_U) {
					read1First = (subsequence1[j81] & 0x0001) ? 0 : 1;
					delta = subsequence1[j81] >> 1;
					mappingPos[0].push_back(mappingPos[0][0]+delta);
					mateSeqId[0].push_backl(seqId); // remember to have uniform naming
					j81++;
				} else {

				}
			}
		}
	}




}