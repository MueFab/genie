# Software Requirements Specification

## Core Library

### Encoder

### Decoder

## FASTQ Transcoding

The FASTQ sequence identifier line can contain comments (which are separated from the actual sequence identifier by a space).
Genie simply disregards those comments.

## SAM Transcoding

### Transcoding from the SAM Format to MPEG-G Records

#### Requirements

SAM.1) The input file is a file in the SAM, BAM, or CRAM format which is compliant to HTSlib, release 1.11.

SAM.2) If the input SAM/BAM/CRAM file contains aligned data the corresponding reference file, which is a file in the FASTA format that contains the reference sequence(s) used during alignment, must be provided.

SAM.3) The input SAM/BAM/CRAM file must be sorted by QNAME.

SAM.4) The output is a file that contains a sequence of concatenated MPEG-G records which are sorted by sequence ID, and each set of MPEG-G records belonging to the same sequence ID is sorted by position.

#### API

```
struct genie::sam2mgrec::Config {
    std::string sam_file_path;
    std::string fasta_file_path;
    std::string mgrec_file_path;
    std::string tmp_dir_path;
    int verbosity_level;
};
```

SAM.5) The API is expecting C++14 and a Linux OS.

SAM.6) The implementation shall provide a data structure containing the configuration.

SAM.7) The configuration shall comply to the values specified in Table...

| Variable Name   | Default Value | Accepted Values |
|-----------------|---------------|-----------------|
| sam_file_path   | ""            | String containing the path of a file in the SAM/BAM/CRAM format |
| fasta_file_path | ""            | String containing the path of a file in the FASTA format |
| mgrec_file_path | ""            | String containing a path where a file will be created that will contain the generated sequence of concatenated MPEG-G records |
| tmp_dir_path    | "/tmp/"       | String containing the path of a directory |
| verbosity_level | 0             | 0 (silent), 1 (informational messages) |

SAM.8) The implementation shall provide a function with the following prototype.

```
genie::sam2mgrec::ErrorCode genie::sam2mgrec::transcode(struct genie::sam2mgrec::Config cfg);
```

The implementation shall provide the following return values:

```
enum genie::sam2mgrec::ErrorCode {
    success = 0,
    failure = 1
};
```

### Transcoding from MPEG-G Records to the SAM Format
