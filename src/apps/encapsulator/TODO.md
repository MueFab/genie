# TODO

---

##Bit writer 

- current bit writer fills bytes from right to left and then writes out full bytes
  - add stream like bit writing
    - in src/genie/format/mpegg_p1/dataset_header.cc the writing is done by concatenating all bits in a string and giving the bitwriter full and correctly ordered bytes -> needs probably to be redone with a proper bit writer
    
##write outs

- start from src/genie/format/mpegg_p1/dataset.cc::writeToFile
  - calculation for length of Datasets needs to be done
    - calculation for length of Dataset Headers needs to be done
    - calculation for length of Dataset Parameter Sets needs to be done
        - part 1 PSs have currently only the Dataset ID and the DatasetGroup ID in addition to part 2 PSs -> check if it is possible to use part 2 functions
    - calculation for length of Access Units needs to be done
        - part 1 AUs are currently similar to part 2 AUs if MIT_flag is set to 0 -> check if it is possible to use part 2 functions
    
  - as mentioned before probably redo /format/mpegg_p1/dataset_header.cc
    - write out code is there but needs to call the new bit writer function
    
  - write out for Dataset Parameter Sets and Access Units
    - check if it is possible to use part 2 functions

## FIXMEs and TODOs

- check for FIXMEs and TODOs