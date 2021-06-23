# Software Requirements Specification

All API specifications are provided in a syntax compliant to C++14.

All API specifications are grouped in the namespace ``genie``.
The prefix ``genie::`` is *not* repeated throughout the document for brevity.

## Core Library

Core library API specifications are grouped in the namespace ``core``.
The prefix ``core::`` is *not* repeated throughout this section for brevity.

## Transcoding Library

Transcoding library API specifications are grouped in the namespace ``transcoding``.
The prefix ``transcoding::`` is *not* repeated throughout this section for brevity.

**Transcoding.1)** The implementation shall provide the following error codes.

```
enum ErrorCode {
    success = 0,
    failure = 1
};
```

**Transcoding.2)** The implementation shall provide a function for the conversion of error codes to error strings with the following prototype.

```
std::string error_code_to_string(ErrorCode ec);
```

### FASTQ

FASTQ transcoding API specifications are grouped in the namespace ``fastq``.
The prefix ``fastq::`` is *not* repeated throughout this section for brevity.

#### Transcoding from the FASTQ Format to MPEG-G Records

API specifications in this section are grouped in the namespace ``fastq_to_mgrec``.
The prefix ``fastq_to_mgrec::`` is *not* repeated throughout this section for brevity.

**FASTQ.1)** The input file is a file in the FASTQ format, or a file in the GZIP file format, which contains a file in the FASTQ format in encoded form.

**FASTQ.2)** TODO paired file, interleaved file, missing mate

**FASTQ.3)** The output is a file that contains a sequence of concatenated MPEG-G records.

**FASTQ.4)** TODO The implementation shall provide a data structure ``Config`` containing the transcoding configuration.

```
struct Config {
    std::string fastq1_file_path;
    std::string fastq2_file_path;
    std::string mgrec_file_path;
    std::string tmp_dir_path;
    int verbosity_level;
};
```

**FASTQ.5)** TODO The transcoding configuration shall comply to the values specified in the following table.

| Variable Name   | Default Value | Accepted Values |
|-----------------|---------------|-----------------|
| sam_file_path   | ""            | String containing the path to a file in the SAM/BAM/CRAM format |
| fasta_file_path | ""            | String containing the path to a file in the FASTA format |
| mgrec_file_path | ""            | String containing the path at which a file will be created that will contain the generated output sequence of concatenated MPEG-G records |
| tmp_dir_path    | ""            | String containing the path of a directory that will be used for the storage of temporary files |
| verbosity_level | 0             | 0 (silent), 1 (errors), 2 (errors and informational messages) |

**FASTQ.6)** The implementation shall provide a function for the transcoding from the FASTQ format to MPEG-G records with the following prototype.

```
ErrorCode transcode(Config cfg);
```

#### Transcoding from MPEG-G Records to the FASTQ Format

### SAM

SAM transcoding API specifications are grouped in the namespace ``sam``.
The prefix ``sam::`` is *not* repeated throughout this section for brevity.

#### Transcoding from the SAM Format to MPEG-G Records

API specifications in this section are grouped in the namespace ``sam_to_mgrec``.
The prefix ``sam_to_mgrec::`` is *not* repeated throughout this section for brevity.

**SAM.1)** The input file is a file in the SAM, BAM, or CRAM format which is compliant to [HTSlib](https://www.htslib.org), release 1.11.

**SAM.2)** If the input SAM/BAM/CRAM file contains aligned data the corresponding reference file, i.e., a file in the FASTA format that contains the reference sequence(s) used during alignment, must be provided.

**SAM.3)** The input SAM/BAM/CRAM file must be sorted by read name (i.e., the QNAME field). (This can for example be achieved with the following command: ``samtools sort -n in.sam > out.sam``)

**SAM.4)** The output is a file that contains a sequence of concatenated MPEG-G records which are sorted by sequence ID, and each set of MPEG-G records belonging to the same sequence ID is sorted by position.

**SAM.5)** The implementation shall provide a data structure ``Config`` containing the transcoding configuration.

```
struct Config {
    std::string sam_file_path;
    std::string fasta_file_path;
    std::string mgrec_file_path;
    std::string tmp_dir_path;
    int verbosity_level;
};
```

**SAM.6)** The transcoding configuration shall comply to the values specified in the following table.

| Variable Name   | Default Value | Accepted Values |
|-----------------|---------------|-----------------|
| sam_file_path   | ""            | String containing the path to a file in the SAM/BAM/CRAM format |
| fasta_file_path | ""            | String containing the path to a file in the FASTA format |
| mgrec_file_path | ""            | String containing the path at which a file will be created that will contain the generated output sequence of concatenated MPEG-G records |
| tmp_dir_path    | ""            | String containing the path of a directory that will be used for the storage of temporary files |
| verbosity_level | 0             | 0 (silent), 1 (errors), 2 (errors and informational messages) |

**SAM.7)** The implementation shall provide a function for the transcoding from the SAM format to MPEG-G records with the following prototype.

```
ErrorCode transcode(Config cfg);
```

#### Transcoding from MPEG-G Records to the SAM Format

API specifications in this section are grouped in the namespace ``mgrec_to_sam``.
The prefix ``mgrec_to_sam::`` is *not* repeated throughout this section for brevity.

## Tools Library

Transcoding library API specifications are grouped in the namespace ``tools``.
The prefix ``tools::`` is *not* repeated throughout this section for brevity.
