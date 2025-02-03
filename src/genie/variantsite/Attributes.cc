/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/util/runtime_exception.h"
#include "genie/variantsite/Attributes.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

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
        writers.back().flush();

        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(static_cast<uint32_t>(rowInTile+1));
        for (uint8_t i = 1; i < info.getArrayLength(); ++i) arrayDims.push_back(static_cast<uint32_t>(2));
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
    for (uint8_t i = 0; i < info.getArrayLength(); ++i) value.emplace_back(def.toArray(defaultType, defaultValue));
    write(value);
}

std::vector<std::stringstream> AttributeTile::convertTilesToTypedData() {
    std::vector<std::stringstream> TypedTiles;

    core::DataType TypeId = info.getAttributeType();
    uint8_t numArrayDims = info.getArrayLength();
    std::vector<uint32_t> arrayDims;
    arrayDims.push_back(static_cast<uint32_t>(rowInTile));
    for (uint8_t i = 1; i < numArrayDims; ++i) arrayDims.push_back(static_cast<uint32_t>(2));

    for (auto& tile : tiles) {
        genie::core::record::annotation_access_unit::TypedData typedData(TypeId, numArrayDims, arrayDims);
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
        for (uint8_t i = 1; i < info.getArrayLength(); ++i) arrayDims.push_back(static_cast<uint32_t>(2));
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

void Attributes::add(std::vector<genie::core::record::variant_site::InfoFields::Field> tags)//, std::vector<std::vector<std::vector<uint8_t>>> infoValues) {
{
    size_t index = 0;
    for (const auto& tag : tags) {
        attributeTiles[tag.tag].write(tag.values);// infoValues.at(index));
        attrWritten[tag.tag] = true;
        index++;
    }
    for (const auto& isWritten : attrWritten) {
        if (!isWritten.second) {
            attributeTiles[isWritten.first].writeMissing();
        }
        attrWritten[isWritten.first] = false;
    }
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
