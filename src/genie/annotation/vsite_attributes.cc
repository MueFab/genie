/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/annotation/attributes.h"
#include "genie/core/array_type.h"
#include "genie/core/functional_annotation_record/record.h"
#include "genie/core/track_property_record/record.h"
#include "genie/core/track_record/record.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

void Attributes::add(std::vector<genie::core::record::variant_site::InfoFields::Field> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.tag].write(field.values);
        attrWritten[field.tag] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void Attributes::add(std::vector<genie::core::record::feature::FeatureFields::Field> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.attr].write(field.attr_values);
        attrWritten[field.attr] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void Attributes::add(std::vector<genie::core::record::sample::SampleFields::Field> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.attr].write(field.attr_values);
        attrWritten[field.attr] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void Attributes::add(std::vector<genie::core::record::track::Attribute> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.attr_tag].write({field.attr_value});
        attrWritten[field.attr_tag] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void Attributes::add(std::vector<genie::core::record::track_property::TrackProperty> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.track_property].write(field.track_property_values);
        attrWritten[field.track_property] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void Attributes::add(std::vector<genie::core::record::functional_annotation::Attribute> fields) {
    for (const auto& field : fields) {
        attributeTiles[field.attr_tag].write(field.attr_values);
        attrWritten[field.attr_tag] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

void AttributeTile::write(std::vector<std::vector<uint8_t>> value) {
    AddFirst();
    genie::core::ArrayType arraytype;
    for (const auto& onearray : value) {
        arraytype.toFile(info.getAttributeType(), onearray, writers.back());
    }

    typedTiles.back().writeElement(value);

    if (rowsPerTile == 0) {
    } else if (rowInTile < (rowsPerTile-1)) {
        rowInTile++;
        typedTiles.back().setArrayDim0(static_cast<uint32_t>(rowInTile));
    } else {
        typedTiles.back().setArrayDim0(static_cast<uint32_t>(rowInTile + 1));
        writers.back().Flush();

        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(static_cast<uint32_t>(rowInTile+1));
        for (uint8_t idx_i = 1; idx_i < info.getArrayLength(); ++idx_i) arrayDims.push_back(static_cast<uint32_t>(2));
        typedTiles.emplace_back(info.getAttributeType(), info.getArrayLength(), arrayDims);
        tiles.emplace_back("");
        writers.emplace_back(&tiles.back());
        rowInTile = 0;
    }
}

void AttributeTile::writeMissing() {
    AddFirst();
    genie::core::ArrayType def{};
    auto defaultType = info.getAttributeType();
    auto defaultValue = def.getDefaultValue(defaultType);
    std::vector<std::vector<uint8_t>> value;
    for (uint8_t idx_i = 0; idx_i < info.getArrayLength(); ++idx_i) value.emplace_back(def.toArray(defaultType, defaultValue));
    write(value);
}

std::vector<std::stringstream> AttributeTile::convertTilesToTypedData() {
    std::vector<std::stringstream> TypedTiles;

    core::DataType TypeId = info.getAttributeType();
    uint8_t numArrayDims = info.getArrayLength();
    std::vector<uint32_t> arrayDims;
    arrayDims.push_back(static_cast<uint32_t>(rowInTile));
    for (uint8_t idx_i = 1; idx_i < numArrayDims; ++idx_i) arrayDims.push_back(static_cast<uint32_t>(2));

    for (auto& tile : tiles) {
        genie::core::access_unit::annotation::TypedData typedData(TypeId, numArrayDims, arrayDims);
        util::BitReader reader(tile);
        typedData.convertToTypedData(reader);
        TypedTiles.emplace_back("");
        core::Writer writer(&TypedTiles.back());
        typedData.write(writer);
    }

    return TypedTiles;
}

void AttributeTile::AddFirst() {
    if (tiles.empty()) {
        tiles.emplace_back("");
        writers.emplace_back(&tiles.back());
    }
    if (typedTiles.empty()) {
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(static_cast<uint32_t>(rowsPerTile));
        for (uint8_t idx_i = 1; idx_i < info.getArrayLength(); ++idx_i) arrayDims.push_back(static_cast<uint32_t>(2));
        typedTiles.emplace_back(info.getAttributeType(), info.getArrayLength(), arrayDims);
    }
}

AttributeTile& AttributeTile::operator=(const AttributeTile& other) {
    info = other.info;
    rowsPerTile = other.rowsPerTile;
    rowInTile = other.rowInTile;
    tiles.clear();
    writers.clear();
    return *this;
}

AttributeTile::AttributeTile(const AttributeTile& other) {
    info = other.info;
    rowsPerTile = other.rowsPerTile;
    rowInTile = other.rowInTile;
    tiles.clear();
    writers.clear();
}

void AttributeTile::setCompressedData(uint64_t tilenr, std::stringstream& compressedData) {
    (void)tilenr;
    (void)compressedData;
}

void Attributes::add(std::map<std::string, genie::core::record::variant_site::Info_tag> tags, std::map<std::string, std::vector<std::vector<uint8_t>>> infoValues) {
    for (const auto& tag : tags) {
        attributeTiles[tag.first].write(infoValues[tag.first]);
        attrWritten[tag.first] = true;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
}

Attributes::Attributes(Attributes& other) {
    info = other.info;
    rowsPerTile = other.rowsPerTile;
    attributeTiles = other.attributeTiles;
    attrWritten = other.attrWritten;
}

Attributes& Attributes::operator=(const Attributes& other) {
    info = other.info;
    rowsPerTile = other.rowsPerTile;
    attributeTiles = other.attributeTiles;
    attrWritten = other.attrWritten;
    return *this;
}

void Attributes::initAttributeTiles() {
    for (auto attrInfo : info) {
        attributeTiles[attrInfo.first] = AttributeTile(rowsPerTile, attrInfo.second);
        attrWritten[attrInfo.first] = false;
    }
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
