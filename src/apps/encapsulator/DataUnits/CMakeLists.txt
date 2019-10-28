project("encapsulatorDataUnits")

set(source_files
        DataUnitAccessUnit.c
        DataUnitBlockHeader.c
        DataUnitRawReference.c
        DataUnits.c
        DataUnitsParametersSet.c
)

add_library(encapsulatorDataUnits ${source_files})