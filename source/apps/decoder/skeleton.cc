#include <vector>
#include <string>
//#include "data-units.h"

// TODO: check if standard C++ libraries are used, or if I should switch to boost or something (do not use boost)

// returns MPEG-G record (Section 13 page 165)
// access unit structure as parameter
// 

// access unit implementation in https://github.com/mitogen/genie/blob/conformance/source/format/DataUnits/DataUnits.h

// FOR WIDTH CHECK PAGE 165 - or use almost always 32 bits


// TODO: move the following data structure on a seperate source file
// Also TODO: change the code so that it uses expandable arrays

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class expandable_array
{
private:
	std::vector<T> vec;

public:
	expandable_array();
	T& operator[](size_t);
	const T& operator[](size_t) const;
	size_t size();
};

template<class T>
expandable_array::expandable_array()
{
	vec = std::vector<T>();
}

template<class T>
T& expandable_array::operator[](size_t idx)
{
	while(vec.size() <= idx)
		vec.push_back(T());
	return vec[idx];
}

template<class T>
const T& expandable_array::operator[](size_t idx) const
{
	while(vec.size() <= idx)
		vec.push_back(T());
	return vec[idx];
}

template<class T>
size_t expandable_array::size()
{
	return vec.size();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// dumb-proof it at some point
void zeroOrderComplement(int64_t idx) 
{
	if(idx == 4) return 4;
	return 3-idx;
}

// Based on Table 36, page 49
void firstOrderComplement(int64_t idx) // todo: make it more fancy/compact by putting stuff into an array
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
void change(std::vector<T> &vec, int64_t idx, const &T newvalue) {
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

// NOTE: right now I am using signed 64-bit integers for ALL variables; check section 13 at some point to use appropriate integer width for each variable

void decode(/* TODO: properly add parameters here */ /* access unit */ /* parameter set */ ) 
{
	// Step 1: set a classId variable equal to the value of AU_type
	int64_t classId = AU_type;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// 
	// reserved for calling gabac
	// decoded_symbol[8][0][j80] <---- produced by gabac code
	//
	///////////////////////////////////////////////////////////////////////////

	// Step 2: decode the following variables:
		// numberOfRecordSegments
		// numberOfAlignedRecordSegments
		// unpairedRead
	
	// To calculate these values, we first need to calculate numberOfTemplateSegments
	int64_t numberOfTemplateSegments = number_of_template_segments_minus1+1;

	// Sub-Step 2.1: decoding numberOfRecordSegments
	int64_t numberOfRecordSegments;
	if(numberOfTemplateSegments == 1) numberOfRecordSegments = 1;
	else if(classId == class_HM) numberOfRecordSegments = 2; // For now not going to be the case, since we are only concerned about Class P
	else if(subsequence0[j80] == 0) numberOfRecordSegments = 2;
	else numberOfRecordSegments = 1;

	// Sub-Step 2.1 TODOs: 
	// Figure out how to properly perform the 3rd check, i.e. else if(subsequence0[j80] == 0)

	// Sub-Step 2.2: decoding numberOfAlignedRecordSegments
	int64_t numberOfAlignedRecordSegments;
	if(classId == Class_HM) numberOfAlignedRecordSegments = 1; // Here for clarity; we are focusing on the Class P case for now
	else if(classId == Class_U) numberOfAlignedRecordSegments = 0; // Same as above
	else numberOfAlignedRecordSegments = numberOfRecordSegments; // Since we are in Class P for now (perfectly aligned reads) this makes sense and will always be the case

	

	// Sub-Step 2.3: decoding unpairedRead
	int64_t unpairedRead;
	if(classId == Class_HM) unpairedRead = 0; // Here for clarity
	else if(numberOfTemplateSegments == 1 || subsequence0[j80] == 5 || subsequence0[j80] == 6) unpairedRead = 1;
	else unpairedRead = 0;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 3: compute the following 2D arrays:
		// softClips (dims: numberOfAlignedRecordSegments x 2)
		// softClipSizes (dims: numberOfAlignedRecordSegments x 2)
		// hardClips (dims: numberOfAlignedRecordSegments x 2)

	// Lets create the arrays first of all
	// TODO: check if we should return them, or if they will be passed as parameters; this step might not be necessary if the latter is the case
	std::vector<std::vector<std::string> > softClips(numberOfAlignedRecordSegments);
	std::vector<std::vector<int64_t> > softClipSizes(numberOfAlignedRecordSegments), hardClips(numberOfAlignedRecordSegments); // Might be able to pull off using an integer type with less bits (to save space and stuff)

	for(int64_t i = 0; i < numberOfAlignedRecordSegments; ++i) {
		softClips[i] = std::vector<std::string>(2, "");
		softClipSizes[i] = hardClips[i] = std::vector<int64_t>(2, 0);
		
	}

	// Sub-Step 3.1, 3.2, 3.3: computing softClips, softClipSizes, hardClips
	std::vector<std::string> S;
	int64_t currentRecordCount = 0; // possibly can pull off using shorter integer width

	initializeSupportedSymbolsVector(S);

	if(classId == Class_I || classId == Class_HM) {
		if(j50 < subsequence0.size() && currentRecordCount == subsequence0[j50]) 
		// one can only hope that subsequence0 (or, more accurately decoded_symbol[][][]) is a vector
	    // also what the frick is currentRecordCount
		{
			bool end = false;
			int64_t segmentIdx;
			int64_t leftRightIdx;
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
				} 
				else if(subsequence1[j51] <= 7) {
					segmentIdx = ((subsequence1[j51] - 4) >> 1);
					leftRightIdx = ((subsequence1[j51] - 4) & 1);

				}
				
			} while(!end);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 4: compute the following arrays:
		// readLength
		// numberOfSplicedSeg
		// splicedSegLength

	std::vector<int64_t> readLength; // TODO: check width 
	std::vector<int64_t> numberOfSplicedSeg; // TODO: check width
	std::vector<std::vector<int64_t> > splicedSegLength; // TODO: check width
	int64_t remainingLen;

	if(read_length == 0) {
		for(int64_t i=0; i<numberOfRecordSegments; ++i)
			readLength.push_back(subsequence0[j70++]+1);
	}
	else {
		for(int64_t i=0; i<numberOfRecordSegments; ++i) {
			if(classId == class_I)
				readLength.push_back(read_length - hardClips[i][0] - hardClips[i][1]);
			else if(classId == Class_HM && i == 0) 
				readLength.push_back(read_length - hardClips[0][0] - hardClips[0][1]);
			else
				readLength.push_back(read_length);
		}
	}

	for(int64_t i=0; i<numberOfRecordSegments; ++i) {
		numberOfSplicedSeg.push_back(1);
		splicedSegLength.push_back(std::vector<int64_t>(1, readLength[i]));
	}

	if(spliced_reads_flag && (classId == Class_I || classId == Class_HM)) {
		for(int64_t i=0; i<numberOfAlignedRecordSegments; ++i) {
			if(read_length == 0) // notice: numberOfAlignedRecordSegments <= numberOfRecordSegments
				readLength[i] = subsequence0[j70++]+1;
			remainingLen = readLength[i];
			int64_t j = 0;

			do {
				int64_t spliceLen = subsequence0[j70++]; // ASK WIDTH
				remainingLen -= spliceLen;

				// while(splicedSegLength[i].size() <= j) // ASK if this is intended; these arrays are too sparce if I understand correctly.
				// 	splicedSegLength.push_back(0);

				splicedSegLength[i][j++] = spliceLen;
				splicedSegLength[i].push_back(0);
			} while(remainingLen > 0);

			numberOfSplicedSeg[i] = j;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 5: Decode output variables containing alignment information

	std::vector<int64_t> numberOfSegmentAlignments, numberOfAlignmentPairs; // TODO: check width
	std::vector<std::vector<int64_t> > alignPtr;
	int64_t numberOfAlignments; // = 0 ?

	if(classId != Class_U) {
		if(multiple_alignment_flag == 0)
			numberOfSegmentAlignments.push_back(1);
		else
			numberOfSegmentAlignments.push_back(sequence0[j100++]);

	} 
	else {
		numberOfSegmentAlignments.push_back(0);
	}

	int64_t moreAlignments = 0, 
			 moreAlignmentsNextSeqId = 0, 
			 moreAlignmentsNextPos = 0;

	if(unpairedRead || classId == Class_HM) {
		numberOfAlignments = numberOfSegmentAlignments[0];
		for(int64_t i=0; i<numberOfAlignments; ++i)
			alignPtr.push_back(std::vector<int64_t>(1, i));
	} 
	else if(classId == class_U) {
		if(numberOfRecordSegments > 1)
			numberOfSegmentAlignments.push_back(0);
		numberOfAlignments = 0;
	} 
	else {
		numberOfSegmentAlignments[1] = 0;
		int64_t k = 0, i = 0;
		while(i < numberOfSegmentAlignments[0]) {
			if(multiple_alignment_flag == 0)
				numberOfAlignmentPairs.push_back(1);
			else
				numberOfAlignmentPairs.push_back(sequence0[j100++]);
		
			int64_t j = 0, ptr;

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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	>>>>>>>>>>>>>>>>>>>>> // Step 6: decode the pos descriptor
	>>>>>>>>>>>>>>>>>>>>> // previousMappingPos0 is going to be a parameter, but also we can change it
	>>>>>>>>>>>>>>>>>>>>> // ask
	*/

	std::vector<std::vector<int64_t> > mappingPos;

	if(j00 > 0)
		mappingPos.push_back(std::vector<int64_t>(1, 0));
	else
		mappingPos.push_back(std::vector<int64_t>(AU_start_position+subsequence0[j00]));

	previousMappingPos0 = mappingPos[0][0];

	for(int64_t i=1; i<numberOfSegmentAlignments[0]; ++i) {
		mappingPos[i][0] = mappingPos[i-1][0]+subsequence1[j01];
		j01++;
	}

	j00++;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	int64_t read1First, delta;
	std::vector<std::vector<int64_t> > splitMate, mateSeqId;
	std::vector<int64_t> mateAuId, mateRecordIndex;

	splitMate.push_back(std::vector<int64_t>(1, 0));
	mateSeqId.push_back(std::vector<int64_t>(1, 0));
	mateAuId.push_back(0);
	mateAuId.push_back(0);
	mateRecordIndex.push_back(0);

	// NOTE: for debugging refer to section 10.4.9, pages 76-...
	// Decoding process of the pair descriptor for the first alignment, or class U

	if(classId != Class_HM) {
		for(int64_t i=1; i<numberOfTemplateSegments; ++i) {
			if(subsequence0[j80] == 0) {
				splitMate[0].push_back(0);

				if(classId != class_U) {
					read1First = (subsequence1[j81] & 0x0001) ? 0 : 1;
					delta = subsequence1[j81] >> 1;
					mappingPos[0].push_back(mappingPos[0][0]+delta);
					mateSeqId[0].push_back(seqId); // remember to have uniform naming
					j81++;
				} 
				else {
					read1First = 1;
					mateAuId[1] = -1; // why 1?
					mateRecordIndex.push_back(-1);
				}
			} 
			else if(subsequence0[j80] == 1) {
				splitMate[0].push_back(1);
				read1First = 0;

				if(classId != class_U) {
					mappingPos[0].push_back(subsequence2[j82]);
					mateSeqId[0].push_back(seqId);
				} 
				else {
					mateAuId[1] = -1;
					mateRecordIndex.push_back(subsequence2[j82]);
				}

				j82++;
			} 
			else if(subsequence0[j80] == 2) {
				splitMate[0].push_back(1);
				read1First = 1;

				if(classId != Class_U) {
					mappingPos[0].push_back(subsequence3[j83]);
					mateSeqId[0].push_back(seqId);
				}
				else {
					mateAuId[1] = -1;
					mateRecordIndex[i] = subsequence2[j83];
				}

				j83++;
			}
			else if(subsequence0[j80] == 3) {
				splitMate[0].push_back(1);
				read1First = 0;

				if(classId != Class_U) { // Note: in specification this is referred to as CLASS_U for some reason. Typo?
					mappingPos[0].push_back(subsequence6[j86]);
					mateSeqId[0].push_back(subsequence4[j84]);

				}
				else {
					mateAuId[1] = subsequence4[j84];
					mateRecordIndex[1] = subsequence2[j86];
				}

				j84++;
				j86++;
			}
			else if(subsequence0[j80] == 4) {
				splitMate[0].push_back(1);
				read1First = 1;

				if(classId != Class_U) {
					mappingPos[0].push_back(subsequence7[j87]);
					mateSeqId[0].push_back(subsequence5[j85]);
				}
				else {
					mateAuId[1] = subsequence5[j85];
					mateRecordIndex[1] = subsequence2[j87];
				}

				j85++;
				j87++;
			}
			else if(subsequence0[j80] == 5)	{
				splitMate[0].push_back(2);
				read1First = 1;
			}
			else if(subsequence0[j80] == 6) {
				splitMate[0].push_back(2);
				read1First = 0;
			}

			j80++;
		}
	}

	// Decoding process of the pair descriptor subsequences for the alignments after the first one

	for(int64_t i=1; i<numberOfSegmentAlignments[0]; ++i)
		splitMate.push_back(std::vector<int64_t>(1, 0));

	if((classId == class_P || classId == class_N ||
		classId == Class_M || classId == class_I) && !unpairedRead) {
		for(int64_t j=1; j<numberOfTemplateSegments; ++j) {
			int64_t currAlignIdx = 0;

			for(int64_t i=1; i<numberOfAlignments; ++i) {
				int64_t alignIdx = alignPtr[i][j];
				if(alignIdx > currAlignIdx) {
					currAlignIdx = alignIdx;

					if(subsequence0[j80] == 0) {
						splitMate[alignIdx][j] = 0; // TODO: THIS MIGHT CAUSE A RUNTIME ERROR; to fix re-write code using expandable_array
						delta = subsequence1[j81]>>1;

						if(subsequence1[j81] & 0x0001)
							delta = -delta;

						mappingPos[alignIdx][j] = mappingPos[alignPtr[i][0]]][0] + delta;
						mateSeqId[alignIdx][j] = seqId;
						j81++;
					}
					else if(subsequence0[j80] == 2) {
						splitMate[alignIdx][j] = 0;
						mappingPos[alignIdx][j] = subsequence3[j83];
						mateSeqId[alignIdx][j] = seqId;
						j83++;
					}
					else if(subsequence0[j80] == 4) {
						splitMate[alignIdx][j] = 1;
						mappingPos[alignIdx][j] = subsequence7[j87];
						mateSeqId[alignIdx][j] = subsequence5[j85];
						j87++;
						j85++;
					}
					else {
						// other subsequence0[j80] values
						// what does that mean??
						// "reserved"
						// what does THAT mean??
						// ASK
					}

					j80++;
				}
			}
		}
	}

	// Decoding process of the pair descriptor subsequences for spliced reads

	for(int64_t i=0; i<numberOfAlignedRecordSegments; ++i)
		splicedSegMappingPos[i][0] = mappingPos[0][i];

	if(classId == Class_I || classId == Class_HM) {
		for(int64_t i=0; i<numberOfAlignedRecordSegments; ++i) {
			for(int64_t j=1; j<numberOfSplicedSeg[i]; ++j) {
				int64_t prevSpliceMappingEnd = splicedSegMappingPos[i][j-1] + splicedSegLength[i][j-1];

				if(subsequence0[j80] == 0) {
					delta = subsequence1[j81]>>1;

					if(subsequence1[j81] & 0x0001)
						delta = -delta;

					splicedSegMappingPos[i][j] = prevSpliceMappingEnd + delta;
					j81++;
				}
				else if(subsequence0[j81] == 2) {
					splicedSegMappingPos[i][j] = subsequence3[j83];
					j83++;
				}
				else {
					// other subsequence0[j80] values
					// what does that mean??
					// "reserved"
					// what does THAT mean??
					// ASK
				}

				j80++;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	>>>>>>>>>>>>>>>>>>>>> // Step 8: decode the rcomp descriptor
	*/

	std::vector<std::vector<std::vector<int64_t> > > reverseComp; // NOTE: uninitialized; change to expandable_array and everything will work nicely (:

	for(int64_t i=0; i<numberOfAlignedRecordSegments; ++i) {
		for(int64_t j=0; j<numberOfSegmentAlignments[i]; ++j) {
			if(splitMate[j][i] == 0) {
				for(int64_t k=0; k<numberOfSplicedSeg[i]; ++k)
					reverseComp[k][j][i] = subsequence0[j10++];
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 9: if num_groups is greater than 0, decode the rgourp descriptor

	int64_t readGroupId = -1;
// j[13][0]
	if(num_groups > 0) {
		readGroupId = subsequence0[j130++];
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 10: decode readName variable
		// Note: check section 10.4.19, tokentype descriptor decoding process to check how to decode rname
		// Step 10.1: decode rname
		// Step 10.2: create array decodedStrings[] (as specified in clause 10.4.19.4)
		// Step 10.3: readName = decodedStrings[i], for the i^th record (?? what is i here)

	// TODO: get back to this; weirdly complicated; maybe ask for help

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 11: if as_depth > 0, decode mscore descriptor (as specified in clause 10.4.10)

	std::vector<std::vector<std::vector<int64_t> > > mappingScores; // again, uinitialized; change to expandable_array

	for(int64_t i=0; i<as_depth; ++i) {
		for(int64_t j=0; j<numberOfAlignedRecordSegments; ++j) {
			for(int64_t k=0; k<numberOfSegmentAlignments[j]; ++k) {
				if(splitMate[k][j] == 0)
					mappingScores[k][j][i] = subsequence0[j90]++;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 12: if multiple_alignments_flag, decode msar descriptor (as specified in subclause 10.4.12)

	// NOTE: ASK multiple_alignments_flag vs multiple_alignment_flag??????

	std::vector<std::vector<std::string> > decodedMsar; // again, make sure to change to expandable_array

	if(multiple_alignments_flag) {
		int64_t k = 0;
		for(int64_t i=0; i<numberOfAlignedRecordSegments; ++i) {
			for(int64_t j=0; j<numberOfSegmentAlignments[i]-1; ++j) {
				if(splitMate[j][i] == 0)
					decodedMsar[j][i] = decodedStrings[k++];
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Step 13:
	// Decode flags (as specified in 10.4.3)
	// Decode qv (as specified in 10.4.15)
	// "If present"?? what does that mean and how do we perform such a check?

	// Step 13.1 flags

	int64_t decodedFlags = 0;
	decodedFlags |= (subsequence0[j20] << 0);
	decodedFlags |= (subsequence1[j21] << 1);
	decodedFlags |= (subsequence2[j22] << 2);
	j20++;
	j21++;
	j22++;


	// Step 13.2 qv
	// Maybe create a seperate function for that?
	// decode_quality_values()
	// TODO: too messy; redo pls
	
	int64_t qvNumCodebooksAligned = -1;
	int64_t qvPresentFlag = -1;
	std::string qvString;
	int64_t len = 0;
	int64_t revComp = -1;
	std::string::const_iterator leftIter, rightIter;
	std::string qvSplice;
	std::vector<std::vector<std::string> > qualityValues;
	int64_t qvCodebookId = -1;

	if(genomicRecordIndex == 0) {
		// decode_qv_codebook_indexes()

		if(classId == Class_I || classId == Class_HM) qvNumCodebooksAligned = qv_num_codebooks_total-1;
		else if(classId != Class_U) qvNumCodebooksAligned = qv_num_codebooks_total;
		else qvNumCodebooksAligned = 0;
	}

	for(int64_t tSeg=0; tSeg<numberOfTemplateSegments; ++tSeg) {
		for(int64_t qs=0; qs<qv_depth; ++qs) {
			if(j140 < subsequence0.size()) { // ask if this is a vector; if not, convert to vector (or even better use expandable_array)
				qvPresentFlag = subsequence0[j140];
				j140++;
			}
			else {
				qvPresentFlag = 1;
			}

			if(qvPresentFlag == 1) {
				// decode_qvs()

				for(int64_t baseIdx=0; baseIdx<numBases; ++baseIdx) {
					if((classId == Class_I || classId == Class_HM) && !isAligned(baseIdx)) { // is this already implemented for me ?? if not how do i do that??
						qvCodebookId = qv_num_codebooks_total-1;

					}
					else if(classId == Class_U) {
						qvCodebookId = 0;
					} else if(qvNumCodebooksAligned > 1) {
						qvCodebookId = qvCodeBookIds[basePos[baseIdx]];
					}
				}

				for(int64_t i=0; i<numberOfSplicedSeg[tSeg]; ++i) {
					revComp = reverseComp[i][0][tSeg];
					leftIter = qualityValues[tSeg][qs].begin()+len;
					rightIter = qualityValues[tSeg][qs].begin()+len+splicedSegLength[tSeg][i];
					qvSplice = std::string(leftIter, rightIter);

					if(qv_reverse_flag && revComp) {
						std::string qvSpliceReversed = qvSplice;
						std::reverse(qvSpliceReversed.begin(), qvSpliceReversed.end());
						qvString += qvSpliceReversed;
					}
					else {
						qvString += qvSplice;
					}
				}

				qualityValues[tSeq][sq] = qvString;
			}
			else {
				qualityValues[tSeg][sq] = "";
			}
		}
	}




}