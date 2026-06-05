/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/functionalannotation/descriptors.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/array_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace functional_annotation {

void Descriptors::write(genie::core::record::functional_annotation::Record functionalAnnotation) {
    // Write basic descriptors
    tiles[genie::core::AnnotDesc::SEQUENCEID].write(functionalAnnotation.GetSeqId(), 16);
    tiles[genie::core::AnnotDesc::STARTPOS].write(functionalAnnotation.GetStartPos(), 64);
    tiles[genie::core::AnnotDesc::ENDPOS].write(functionalAnnotation.GetEndPos(), 64);
    tiles[genie::core::AnnotDesc::STRAND].write(functionalAnnotation.GetStrand(), 2);

    // Write feature name as index (uint32) into feature names list in annotation parameter set
    // FEATURENAME descriptor (ID 18) stores the index, not the string itself
    uint32_t featureNameIndex = 0;
    auto it = featureNameToIndex.find(functionalAnnotation.GetFeatureName());
    if (it != featureNameToIndex.end()) {
        featureNameIndex = it->second;
    }
    tiles[genie::core::AnnotDesc::FEATURENAME].write(featureNameIndex, 32);

    // Write feature ID as uint32 (ID from an Ontology)
    // FEATUREID descriptor (ID 19) is a 32-bit ontology ID
    tiles[genie::core::AnnotDesc::FEATUREID].write(functionalAnnotation.GetFeatureID(), 32);

    // Write ontology names as indices (uint32) into ontology names list in annotation parameter set
    // ONTOLOGYNAME descriptor (ID 20) stores indices, with a fixed number per record
    // If fewer ontologies than maxOntologiesPerRecord, pad with 0xFFFFFFFF (missing value)
    const auto& ontologies = functionalAnnotation.GetOntologies();
    for (size_t i = 0; i < maxOntologiesPerRecord; ++i) {
        if (i < ontologies.size()) {
            uint32_t ontologyNameIndex = 0xFFFFFFFF;  // Default to missing
            auto it = ontologyNameToIndex.find(ontologies[i].ontology_name);
            if (it != ontologyNameToIndex.end()) {
                ontologyNameIndex = it->second;
            }
            tiles[genie::core::AnnotDesc::ONTOLOGYNAME].write(ontologyNameIndex, 8);
        } else {
            // Pad with missing value (0xFFFFFFFF for uint32)
            tiles[genie::core::AnnotDesc::ONTOLOGYNAME].write(static_cast<uint32_t>(0xFFFFFFFF), 8);
        }
    }

    // Write ontology IDs as uint32
    // ONTOLOGYID descriptor (ID 21) is a 32-bit ontology ID, fixed number per record
    // Pad with 0xFFFFFFFF (missing value) when record has fewer ontologies
    for (size_t i = 0; i < maxOntologiesPerRecord; ++i) {
        if (i < ontologies.size()) {
            tiles[genie::core::AnnotDesc::ONTOLOGYID].write(ontologies[i].ontology_ID, 32);
        } else {
            // Pad with missing value
            tiles[genie::core::AnnotDesc::ONTOLOGYID].write(static_cast<uint32_t>(0xFFFFFFFF), 32);
        }
    }

    // Write link information if this is a linked record
    if (functionalAnnotation.IsLinkedRecord()) {
        tiles[genie::core::AnnotDesc::LINKNAME].write(functionalAnnotation.GetLinkName());
        tiles[genie::core::AnnotDesc::LINKID].write(functionalAnnotation.GetReferenceBoxID(), 8);
    } else {
        tiles[genie::core::AnnotDesc::LINKNAME].emptyForRow();
        tiles[genie::core::AnnotDesc::LINKID].write((uint8_t)255, 8);
    }
}

void Descriptors::writeDanglingBits() {
    tiles[genie::core::AnnotDesc::SEQUENCEID].wrapUp();      // 1
    tiles[genie::core::AnnotDesc::STARTPOS].wrapUp();        // 2
    tiles[genie::core::AnnotDesc::ENDPOS].wrapUp();          // 3
    tiles[genie::core::AnnotDesc::STRAND].wrapUp();          // 4
    tiles[genie::core::AnnotDesc::LINKNAME].wrapUp();        // 7
    tiles[genie::core::AnnotDesc::LINKID].wrapUp();          // 8
    tiles[genie::core::AnnotDesc::FEATURENAME].wrapUp();     // 18
    tiles[genie::core::AnnotDesc::FEATUREID].wrapUp();       // 19
    tiles[genie::core::AnnotDesc::ONTOLOGYNAME].wrapUp();    // 20
    tiles[genie::core::AnnotDesc::ONTOLOGYID].wrapUp();      // 21
}

void Descriptors::init() {
    tiles[genie::core::AnnotDesc::SEQUENCEID].setRowsPerTile(rowsPerTile);      // 1
    tiles[genie::core::AnnotDesc::STARTPOS].setRowsPerTile(rowsPerTile);        // 2
    tiles[genie::core::AnnotDesc::ENDPOS].setRowsPerTile(rowsPerTile);          // 3
    tiles[genie::core::AnnotDesc::STRAND].setRowsPerTile(rowsPerTile);          // 4
    tiles[genie::core::AnnotDesc::LINKNAME].setRowsPerTile(rowsPerTile);        // 7
    tiles[genie::core::AnnotDesc::LINKID].setRowsPerTile(rowsPerTile);          // 8
    tiles[genie::core::AnnotDesc::FEATURENAME].setRowsPerTile(rowsPerTile);     // 18
    tiles[genie::core::AnnotDesc::FEATUREID].setRowsPerTile(rowsPerTile);       // 19
    tiles[genie::core::AnnotDesc::ONTOLOGYNAME].setRowsPerTile(rowsPerTile);    // 20
    tiles[genie::core::AnnotDesc::ONTOLOGYID].setRowsPerTile(rowsPerTile);      // 21
}

}  // namespace functional_annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
