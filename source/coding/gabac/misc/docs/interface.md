# Library Interface Overview #

## Configurations ##

### IOConfiguration ###
gabac::IOConfiguration holds information about data input / output sources and drains. You can provide std::istream or std::ostream to decide where the data and logging information is going. Additionally you can set a block size which specifies how many bytes will be processed by GABAC in one run. Higher values can improve the compression ratio, lower values will require less memory.
```
struct IOConfiguration
{
    std::istream *inputStream;
    std::ostream *outputStream;
    size_t blocksize;

    std::ostream *logStream;
    enum class LogLevel
    {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2, 
        WARNING = 3,
        ERROR = 4,
        FATAL = 5  
    };
    LogLevel level;

    std::ostream& log(const LogLevel& l) const;
    void validate() const;
};
```
### EncodingConfiguration ###
gabac::EncodingConfiguration holds information about the gabac encoding process, which transformations to apply and which word sizes to assume. It's needed by gabac::run to compress or decompress. You can export and import EncodingCOnfigurations from JSON files.
```
struct TransformedSequenceConfiguration
{
    bool lutTransformationEnabled;
    unsigned int lutBits;
    unsigned int lutOrder;
    bool diffCodingEnabled;
    gabac::BinarizationId binarizationId;
    std::vector<unsigned int> binarizationParameters;
    gabac::ContextSelectionId contextSelectionId;

    std::string toPrintableString() const;
};

class EncodingConfiguration
{
 public:
    EncodingConfiguration();
    explicit EncodingConfiguration(
            const std::string& jsonstring
    );

    ~EncodingConfiguration();

    std::string toJsonString() const;

    std::string toPrintableString() const;

    unsigned int wordSize;
    gabac::SequenceTransformationId sequenceTransformationId;
    unsigned int sequenceTransformationParameter;
    std::vector<TransformedSequenceConfiguration> transformedSequenceConfigurations;
};
```
### AnalysisConfiguration ###
gabac::AnalysisConfiguration holds information about the gabac analyze process, namely which parameter values to try when searching the parameter space. It's needed by gabac::analyze to find a configuration for gabac::run.
```
struct AnalysisConfiguration
{
    std::vector<unsigned> candidateWordsizes;
    std::vector<gabac::SequenceTransformationId> candidateSequenceTransformationIds;
    std::vector<uint32_t> candidateMatchCodingParameters;
    std::vector<uint32_t> candidateRLECodingParameters;
    std::vector<bool> candidateLUTCodingParameters;
    std::vector<bool> candidateDiffParameters;
    std::vector<gabac::BinarizationId> candidateUnsignedBinarizationIds;
    std::vector<gabac::BinarizationId> candidateSignedBinarizationIds;
    std::vector<unsigned> candidateBinarizationParameters;
    std::vector<gabac::ContextSelectionId> candidateContextSelectionIds;
    std::vector<unsigned> candidateLutOrder;
};
```
## Running gabac ##

### Analyze ###

gabac::analyze will brute force a predefined part of the parameter space in order to find the best encoding configuration possible for the input sequence and output that sequence. The configuration found will get converted into a json string and printed to the stream specified in ioconf.
```
void analyze(const IOConfiguration& ioconf,
             const AnalysisConfiguration& aconf
);
```

### Run ###

This will perform the actual encoding/decoding using a configuration created by analyze().

```
void run(
        const IOConfiguration& conf,
        const EncodingConfiguration& enConf,
        bool decode
);
```

### Single Transformation ###

It is possible to execute single transformations from the GABAC codec by calling:
```
const TransformationProperties& getTransformation(const gabac::SequenceTransformationId& id);
```
This function will return a reflection-like TransformationProperties object containing some information about the selected transformation: 
```
struct TransformationProperties
{
    std::string name;
    std::vector<std::string> paramNames;
    std::vector<std::string> streamNames;
    std::vector<uint8_t> wordsizes;
    SequenceTransform transform;
    SequenceTransform inverseTransform;
};
```

Using the function pointers transform or inverseTransform and providing the right vectors of streams and parameters for this transformation,
you can execute the transformations.

```
using SequenceTransform = std::function<void(const std::vector<uint64_t>& param, std::vector<gabac::DataBlock> *const)>;
```

Note that all transformation work in place, so your input data streams will get discarded and replaced by the transformed streams. 
Create a copy before if you want to keep them.

Sidenote: there is a version of TransformationProperties for binarizations:

```
using SignedBinarizationBorder = std::function<uint64_t(uint64_t parameter)>;
struct BinarizationProperties
{
    std::string name;
    int64_t paramMin;
    int64_t paramMax;
    bool isSigned;
    SignedBinarizationBorder min;
    SignedBinarizationBorder max;
    bool sbCheck(uint64_t minv, uint64_t maxv, uint64_t parameter) const;
};

const BinarizationProperties& getBinarization(const gabac::BinarizationId& id);
```

min() / max() will return the minimum and maximum symbol value valid for a specific binarization parameter. sbCheck() will check if a stream with a set of minimum / maximum symbol values can be used with this binarization and a specified parameter.

## Constants ##

Gabac contains many enum and other constants to make the usage of it easier. The most important constants regarding transformations and binarizations can be aquired using gabac::getTransformation(id) or getBinarization(const gabac::BinarizationId& id). However there are some more enums available.

```
enum class SequenceTransformationId;
enum class BinarizationId;
enum class ContextSelectionId;
```

## Data block ##

gabac::DataBlock is the central data structure of the gabac interface. A DataBlock manages a chunk of memory (currently utilizing a std::vector internally) while being aware of the word size of the data inside. The advantage over a traditional std::vector is, that you can adapt the word size dynamically in runtime without having to waste memory. A std::vector<uint64\_t> can only contain 64 bit values, a std::vector<uint32\_t> only 32 bit values. Notice that these are also different types, so a function having std::vector<uint64\_t> as parameters can not be called with std::vector<uint32\_t>. A DataBlock avoids all these problems by encapsulating the memory and retrieving a symbol of the right word size on the fly when accessing it. The DataBlock main constructor takes a size in elements and the requested word size, the remaining interface is very similar to std::vector. There are also constructors available to create a data block directly from a vector or string.

```
DataBlock b(100, 4);  // 100 Values with 4 bytes per value
b.set(0, 5);  // Set value at index 0 to 5
auto size = b.size(); // Get number of elements
b.push_back(42);  // Append new value
// ...

```

## Block stepper ##

gabac::BlockStepper is basically a simple structure that allows you to traverse a data block as fast as possible. While you could also just use a range based loop or iterators, the block stepper is the fastest possibility. 

```
BlockStepper s = dataBlock.getReader();
while (s.isValid()){
    uint64_t val = s.get();
    // Do something....
    s.set(val);
    s.inc();
}
```

## Exceptions ##

In case of error, gabac public interfaces throw a gabac::RuntimeException which is similar to std::exception. In some cases of some low level errors, std::exceptions can also be thrown directly by the standard library. Besides these there should not be any exceptions thrown.
```
try {
// Do something with gabac
} catch (const gabac::RuntimeException& e) {
    std::cerr << "Gabac exception: " << e.what() << std::endl;
} catch (const std::exception& e) {
    std::cerr << "STL exception: " << e.what() << std::endl;
}
```

## Streams ##

gabac::IFileStream, gabac::OFileStream, gabac::IBufferStream, gabac::OBufferStream are custom streams implementing the std::istream and std::ostream interfaces to have GABAC interacting directly with gabac::DataBlock and c-style FILE* pointers. Hence you can read from / spill data to various different types of memory.
```
gabac::OBufferStream o(&datablock):
gabac::IFileStream i(fileptr);
gabac::IOConfiguration ioconf = {&istream,
                                 &ostream, ......
```
