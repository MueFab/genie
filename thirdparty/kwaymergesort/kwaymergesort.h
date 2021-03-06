/****************************************************************************
   kwaymergesort.h (c) 2009,2010,2011 Aaron Quinlan (c) 2016 David Nogueira
   Center for Public Health Genomics
   University of Virginia
   All rights reserved.

   MIT License

****************************************************************************/

// Source:
// https://github.com/davidalbertonogueira/kway-mergesort/blob/fed33572006bcc6c24571fa6b382c1e181eaab64/kwaymergesort.h

// FURTHER CHANGES:
// 1. Use std::getline() in place of >> to allow sorting of lines containing
// whitespaces and tabs
// 2. Restrict to strings rather than generic template

#ifndef KWAYMERGESORT_H
#define KWAYMERGESORT_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
#ifndef _WIN32
#include <libgen.h>  //for basename()
#include <unistd.h>
#endif

namespace {
#ifdef _WIN32
#include <assert.h>
#include <locale.h>
#include <stdio.h>

#define strdup _strdup

#if defined _WIN32 || defined __WIN32__ || defined __EMX__ || defined __DJGPP__
/* Win32, OS/2, DOS */
#define HAS_DEVICE(P) ((((P)[0] >= 'A' && (P)[0] <= 'Z') || ((P)[0] >= 'a' && (P)[0] <= 'z')) && (P)[1] == ':')
#define FILESYSTEM_PREFIX_LEN(P) (HAS_DEVICE(P) ? 2 : 0)
#define ISSLASH(C) ((C) == '/' || (C) == '\\')
#endif

#ifndef FILESYSTEM_PREFIX_LEN
#define FILESYSTEM_PREFIX_LEN(Filename) 0
#endif

#ifndef ISSLASH
#define ISSLASH(C) ((C) == '/')
#endif

char *basename(char const *name) {
    char const *base = name += FILESYSTEM_PREFIX_LEN(name);
    int all_slashes = 1;
    char const *p;

    for (p = name; *p; p++) {
        if (ISSLASH(*p))
            base = p + 1;
        else
            all_slashes = 0;
    }

    /* If NAME is all slashes, arrange to return `/'. */
    if (*base == '\0' && ISSLASH(*name) && all_slashes) --base;

    /* Make sure the last byte is not a slash. */
    assert(all_slashes || !ISSLASH(*(p - 1)));

    return (char *)base;
}
#endif
}  // namespace

namespace genie {
namespace read {
namespace spring {
namespace kwaymergesort {

using namespace std;

bool isRegularFile(const string &filename);
// STLized version of basename()
// (because POSIX basename() modifies the input string pointer)
// Additionally: removes any extension the basename might have.
std::string stl_basename(const std::string &path) {
    string result;

    char *path_dup = strdup(path.c_str());
    char *basename_part = basename(path_dup);
    result = basename_part;
    free(path_dup);

    size_t pos = result.find_last_of('.');
    if (pos != string::npos) result = result.substr(0, pos);

    return result;
}

class MERGE_DATA {
 public:
    // data
    string data;
    istream *stream;
    bool (*compFunc)(const string &a, const string &b);

    // constructor
    MERGE_DATA(const string &_data, istream *_stream, bool (*_compFunc)(const string &a, const string &b))
        : data(_data), stream(_stream), compFunc(_compFunc) {}

    // comparison operator for maps keyed on this structure
    bool operator<(const MERGE_DATA &a) const {
        if (compFunc)
            return compFunc(data, a.data);
        else
            return data < a.data;
    }
};

//************************************************
// DECLARATION
// Class methods and elements
//************************************************

class KwayMergeSortIterator;

class KwayMergeSort {
 public:
    friend KwayMergeSortIterator;
    typedef KwayMergeSortIterator iterator;

    // constructor, using custom comparison function
    KwayMergeSort(const string &inFile, ostream *out = nullptr,
                  bool (*compareFunction)(const string &a, const string &b) = NULL, int maxBufferSize = 1000000,
                  bool compressOutput = false, string tempPath = "");

    // constructor, using T's overloaded < operator.  Must be defined.
    KwayMergeSort(const string &inFile, ostream *out = nullptr, int maxBufferSize = 1000000,
                  bool compressOutput = false, string tempPath = "");

    // destructor
    ~KwayMergeSort(void);
    // Sort the data
    void Sort();
    // change the buffer size
    void SetBufferSize(int bufferSize);
    // change the sort criteria
    void SetComparison(bool (*compareFunction)(const string &a, const string &b));

    KwayMergeSortIterator begin();
    KwayMergeSortIterator end();

 protected:
    // drives the creation of sorted sub-files stored on disk.
    void DivideAndSort();
    // drives the merging of the sorted temp files.
    // final, sorted and merged output is written to "out".
    void Merge();

    void InitializeMergeStep();
    string MergeStepByStep();

    void WriteToTempFile();
    void OpenTempFiles();
    void CloseTempFiles();

 protected:
    string _inFile;
    bool (*_compareFunction)(const string &a, const string &b);
    string _tempPath;
    vector<string> _vTempFileNames;
    vector<ifstream *> _vTempFiles;
    unsigned int _maxBufferSize;
    unsigned int _runCounter;
    bool _compressOutput;
    bool _tempFileUsed;
    ostream *_out;

    // priority queue for the buffer.
    std::multiset<MERGE_DATA> _outQueue;
    string _line;
    // for cases when merge is not necessary
    vector<string> _lineBuffer;
    typename vector<string>::iterator _lineBuffer_iterator;
};

class KwayMergeSortIterator
    : public std::iterator<std::forward_iterator_tag, string, ptrdiff_t, const string *, const string &> {
 public:
    friend KwayMergeSort;

    // Construction/Destruction //////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    KwayMergeSortIterator() : _owner(nullptr), _eof(false){};
    KwayMergeSortIterator(KwayMergeSort *owner, string elem, bool eof = false)
        : _owner(owner), _elem(elem), _eof(eof){};
    ~KwayMergeSortIterator(){};

    KwayMergeSortIterator(const KwayMergeSortIterator &other) : KwayMergeSortIterator() { this->operator=(other); }

    KwayMergeSortIterator &operator=(const KwayMergeSortIterator &other) {
        _owner = other._owner;
        _elem = other._elem;
        _eof = other._eof;
        return *this;
    }
    // Forward immutable standard iterator interface ///////////////////////
    //////////////////////////////////////////////////////////////////////////////
    bool operator==(const KwayMergeSortIterator &other) const {
        if (other._eof == true && _eof == true)
            return true;
        else if (_eof == true)
            return false;
        else if (other._eof == true)
            return false;
        else {  // both _eof false
            if (_elem == other._elem)
                return true;
            else
                return false;
        }
    };
    bool operator!=(const KwayMergeSortIterator &other) const {
        if (other._eof == true && _eof == true)
            return false;
        else if (_eof == true)
            return true;
        else if (other._eof == true)
            return true;
        else {  // both _eof false
            if (_elem == other._elem)
                return false;
            else
                return true;
        }
    };

    KwayMergeSortIterator &operator++() {
        if (_owner->_tempFileUsed) {
            if (_owner->_outQueue.empty()) {
                _eof = true;
                _owner->CloseTempFiles();
                std::cout << "Sort completed." << std::endl;
                return *this;
            } else {
                _elem = _owner->MergeStepByStep();
                return *this;
            }
        } else {
            (_owner->_lineBuffer_iterator)++;
            if (_owner->_lineBuffer_iterator != _owner->_lineBuffer.end()) {
                _elem = *(_owner->_lineBuffer_iterator);
                return *this;
            } else {
                _eof = true;
                return *this;
            }
        }
    };

    KwayMergeSortIterator operator++(int) {
        KwayMergeSortIterator tmp = *this;
        ++*this;
        return tmp;
    };

    const string &operator*() const { return _elem; };

    string value() { return _elem; };

 protected:
    KwayMergeSort *_owner;
    string _elem;
    bool _eof;
};

//************************************************
// IMPLEMENTATION
// Class methods and elements
//************************************************

// constructor
KwayMergeSort::KwayMergeSort(const string &inFile, ostream *out,
                             bool (*compareFunction)(const string &a, const string &b), int maxBufferSize,
                             bool compressOutput, string tempPath)
    : _inFile(inFile),
      _compareFunction(compareFunction),
      _tempPath(tempPath),
      _maxBufferSize(maxBufferSize),
      _runCounter(0),
      _compressOutput(compressOutput),
      _out(out) {}

// constructor
KwayMergeSort::KwayMergeSort(const string &inFile, ostream *out, int maxBufferSize, bool compressOutput,
                             string tempPath)
    : _inFile(inFile),
      _compareFunction(NULL),
      _tempPath(tempPath),
      _maxBufferSize(maxBufferSize),
      _runCounter(0),
      _compressOutput(compressOutput),
      _out(out) {}

// destructor
KwayMergeSort::~KwayMergeSort(void) {}

// API for sorting.
void KwayMergeSort::Sort() {
    DivideAndSort();
    Merge();
}

// change the buffer size used for sorting
void KwayMergeSort::SetBufferSize(int bufferSize) { _maxBufferSize = bufferSize; }

// change the sorting criteria
void KwayMergeSort::SetComparison(bool (*compareFunction)(const string &a, const string &b)) {
    _compareFunction = compareFunction;
}

void KwayMergeSort::DivideAndSort() {
    istream *input = new ifstream(_inFile.c_str(), ios::in);
    // gzipped
    // if ((isGzipFile(_inFile) == true) && (isRegularFile(_inFile) == true)) {
    //     delete input;
    //     input = new igzstream(_inFile.c_str(), ios::in);
    // }

    // bail unless the file is legit
    if (input->good() == false) {
        cerr << "Error: The requested input file (" << _inFile << ") could not be opened. Exiting!" << endl;
        exit(1);
    }

    _lineBuffer.reserve(100000);
    unsigned int totalBytes = 0;  // track the number of bytes consumed so far.

    // track whether or not we actually had to use a temp
    // file based on the memory that was allocated
    _tempFileUsed = false;

    // keep reading until there is no more input data
    std::string line;
    while (getline(*input, line)) {
        // add the current line to the buffer and track the memory used.
        _lineBuffer.push_back(line);
        totalBytes += sizeof(line);  // buggy?

        // sort the buffer and write to a temp file if we have filled up our quota
        if (totalBytes > _maxBufferSize) {
            if (_compareFunction != NULL)
                sort(_lineBuffer.begin(), _lineBuffer.end(), *_compareFunction);
            else
                sort(_lineBuffer.begin(), _lineBuffer.end());
            // write the sorted data to a temp file
            WriteToTempFile();
            // clear the buffer for the next run
            _lineBuffer.clear();
            _tempFileUsed = true;
            totalBytes = 0;
        }
    }

    // handle the run (if any) from the last chunk of the input file.
    if (_lineBuffer.empty() == false) {
        // write the last "chunk" to the tempfile if
        // a temp file had to be used (i.e., we exceeded the memory)
        if (_tempFileUsed == true) {
            if (_compareFunction != NULL)
                sort(_lineBuffer.begin(), _lineBuffer.end(), *_compareFunction);
            else
                sort(_lineBuffer.begin(), _lineBuffer.end());
            // write the sorted data to a temp file
            WriteToTempFile();
        }
        // otherwise, the entire file fit in the memory given,
        // so we can just dump to the output.
        else {
            if (_compareFunction != NULL)
                sort(_lineBuffer.begin(), _lineBuffer.end(), *_compareFunction);
            else
                sort(_lineBuffer.begin(), _lineBuffer.end());
            for (size_t i = 0; i < _lineBuffer.size(); ++i) {
                if (_out) {
                    *_out << _lineBuffer[i] << endl;
                }
            }
        }
    }
    static_cast<ifstream *>(input)->close();
}

void KwayMergeSort::WriteToTempFile() {
    // name the current tempfile
    stringstream tempFileSS;
    if (_tempPath.size() == 0)
        tempFileSS << _inFile << "." << _runCounter;
    else
        tempFileSS << _tempPath << stl_basename(_inFile) << "." << _runCounter;
    string tempFileName = tempFileSS.str();

    // do we want a regular or a gzipped tempfile?
    ofstream *output;
    // if (_compressOutput == true)
    // output = new ogzstream(tempFileName.c_str(), ios::out);
    // else
    output = new ofstream(tempFileName.c_str(), ios::out);

    // write the contents of the current buffer to the temp file
    for (size_t i = 0; i < _lineBuffer.size(); ++i) {
        *output << _lineBuffer[i] << endl;
    }

    // update the tempFile number and add the tempFile to the list of tempFiles
    ++_runCounter;
    output->close();
    delete output;
    _vTempFileNames.push_back(tempFileName);
}

//---------------------------------------------------------
// MergeDriver()
//
// Merge the sorted temp files.
// uses a priority queue, with the values being a pair of
// the record from the file, and the stream from which the record came.
// SEE: http://stackoverflow.com/questions/2290518/c-n-way-merge-for-external-sort,
// post from Eric Lippert.
//----------------------------------------------------------
void KwayMergeSort::Merge() {
    if (_tempFileUsed)
        std::cout << "Temporary files were used... merging now." << std::endl;
    else
        std::cout << "No temporary files were created... sort completed." << std::endl;

    // we can skip this step if there are no temp files to
    // merge.  That is, the entire inout file fit in memory
    // and thus we just dumped to stdout.
    if (_tempFileUsed == false) return;

    // open the sorted temp files up for merging.
    // loads ifstream pointers into _vTempFiles
    OpenTempFiles();

    // extract the first line from each temp file
    InitializeMergeStep();

    // keep working until the queue is empty
    while (_outQueue.empty() == false) {
        MergeStepByStep();
    }
    // clean up the temp files.
    CloseTempFiles();
    std::cout << "Sort completed." << std::endl;
}

void KwayMergeSort::InitializeMergeStep() {
    // extract the first line from each temp file
    for (size_t i = 0; i < _vTempFiles.size(); ++i) {
        getline(*_vTempFiles[i], _line);
        _outQueue.insert(MERGE_DATA(_line, _vTempFiles[i], _compareFunction));
    }
}

string KwayMergeSort::MergeStepByStep() {
    // grab the lowest element, print it, then ditch it.
    MERGE_DATA lowest = *(_outQueue.begin());
    // write the entry from the top of the queue
    if (_out) {
        *_out << lowest.data << endl;
    }
    // remove this record from the queue
    _outQueue.erase(_outQueue.begin());
    // add the next line from the lowest stream (above) to the queue
    // as long as it's not EOF.
    getline(*(lowest.stream), _line);
    if (*(lowest.stream)) _outQueue.insert(MERGE_DATA(_line, lowest.stream, _compareFunction));

    return lowest.data;
}

KwayMergeSortIterator KwayMergeSort::begin() {
    DivideAndSort();

    if (_tempFileUsed)
        std::cout << "Temporary files were used... merging now." << std::endl;
    else
        std::cout << "No temporary files were created... sort completed." << std::endl;

    // we can skip this step if there are no temp files to
    // merge.  That is, the entire inout file fit in memory
    // and thus we just dumped to stdout.
    if (_tempFileUsed == false) {
        _lineBuffer_iterator = _lineBuffer.begin();
        return KwayMergeSortIterator(this, *_lineBuffer_iterator);
    }

    // open the sorted temp files up for merging.
    // loads ifstream pointers into _vTempFiles
    OpenTempFiles();

    // extract the first line from each temp file
    InitializeMergeStep();

    return KwayMergeSortIterator(this, MergeStepByStep());
}

KwayMergeSortIterator KwayMergeSort::end() { return KwayMergeSortIterator(this, {}, true); }

void KwayMergeSort::OpenTempFiles() {
    for (size_t i = 0; i < _vTempFileNames.size(); ++i) {
        ifstream *file = nullptr;

        // not gzipped
        // if ((isGzipFile(_vTempFileNames[i]) == false) && (isRegularFile(_vTempFileNames[i]) == true)) {
        if (isRegularFile(_vTempFileNames[i]) == true) {
            file = new ifstream(_vTempFileNames[i].c_str(), ios::in);
        }
        // gzipped
        // else if ((isGzipFile(_vTempFileNames[i]) == true) && (isRegularFile(_vTempFileNames[i]) == true)) {
        //    file = new igzstream(_vTempFileNames[i].c_str(), ios::in);
        //}

        if (file->good() == true) {
            // add a pointer to the opened temp file to the list
            _vTempFiles.push_back(file);
        } else {
            cerr << "Unable to open temp file (" << _vTempFileNames[i]
                 << ").  I suspect a limit on number of open file handles.  Exiting." << endl;
            exit(1);
        }
    }
}

void KwayMergeSort::CloseTempFiles() {
    // delete the pointers to the temp files.
    for (size_t i = 0; i < _vTempFiles.size(); ++i) {
        _vTempFiles[i]->close();
        delete _vTempFiles[i];
    }
    // delete the temp files from the file system.
    for (size_t i = 0; i < _vTempFileNames.size(); ++i) {
        remove(_vTempFileNames[i].c_str());  // remove = UNIX "rm"
    }
}

/*
   returns TRUE if the file is a regular file:
     not a pipe/device.

   This implies that the file can be opened/closed/seek'd multiple times without losing information
 */
bool isRegularFile(const string &filename) {
    struct stat buf;
    int i;

    i = stat(filename.c_str(), &buf);
    if (i != 0) {
        cerr << "Error: can't determine file type of '" << filename << endl; //"': " << strerror(errno) << endl;
        exit(1);
    }
    //  if (S_ISREG(buf.st_mode))
    return true;
}

/*
   returns TRUE if the file has a GZIP header.
   Should only be run on regular files.
 */
bool isGzipFile(const string &filename) {
    // see http://www.gzip.org/zlib/rfc-gzip.html#file-format
    struct {
        unsigned char id1;
        unsigned char id2;
        unsigned char cm;
    } gzip_header;
    ifstream f(filename.c_str(), ios::in | ios::binary);
    if (!f) return false;

    if (!f.read((char *)&gzip_header, sizeof(gzip_header))) return false;

    if (gzip_header.id1 == 0x1f && gzip_header.id2 == 0x8b && gzip_header.cm == 8) return true;

    return false;
}

}  // namespace kwaymergesort
}  // namespace spring
}  // namespace read
}  // namespace genie

#endif /* KWAYMERGESORT_H */
