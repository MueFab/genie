/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONTACT_MATRIX_PARAMETERS_H
#define GENIE_CONTACT_CONTACT_MATRIX_PARAMETERS_H

#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

struct SampleInformation {
    uint8_t ID;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------

struct ChromosomeInformation {
    uint8_t ID;
    std::string name;
    uint64_t length;
};

// ---------------------------------------------------------------------------------------------------------------------

struct NormalizationMethodInformation {
    uint8_t ID;
    std::string name;
    bool mult_flag;
};

// ---------------------------------------------------------------------------------------------------------------------

struct NormalizedMatrixInformations {
    uint8_t ID;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------

class ContactMatrixParameters {
 private:
    std::unordered_map<uint8_t, SampleInformation> sample_infos;
    std::unordered_map<uint8_t, ChromosomeInformation> chr_infos;

    uint32_t bin_size;
    uint32_t tile_size;
    std::vector<uint32_t> interval_multipliers;

    std::unordered_map<uint8_t, NormalizationMethodInformation> norm_method_infos;
    std::unordered_map<uint8_t, NormalizedMatrixInformations> norm_mat_infos;

 public:

    /**
     * @brief Default constructor for ContactMatrixParameters.
     *
     * This is the default constructor for the ContactMatrixParameters class. It initializes an empty object.
     */
    ContactMatrixParameters();

    /**
     * @brief Parameterized constructor for ContactMatrixParameters.
     *
     * This is the parameterized constructor for the ContactMatrixParameters class. It initializes an object with the given parameters.
     *
     * @param sample_infos The sample information.
     * @param chr_infos The chromosome information.
     * @param interval The interval.
     * @param tile_size The tile size.
     * @param interval_multipliers The interval multipliers.
     * @param norm_method_infos The normalization method information.
     * @param norm_mat_infos The normalized matrix information.
     */
    ContactMatrixParameters(
        std::unordered_map<uint8_t, SampleInformation>&& sample_infos,
        std::unordered_map<uint8_t, ChromosomeInformation>&& chr_infos,
        uint32_t interval,
        uint32_t tile_size,
        std::vector<uint32_t>&& interval_multipliers,
        std::unordered_map<uint8_t, NormalizationMethodInformation>&& norm_method_infos,
        std::unordered_map<uint8_t, NormalizedMatrixInformations>&& norm_mat_infos
    );

    /**
     * @brief Constructor for ContactMatrixParameters from a bit reader.
     *
     * This constructor initializes an object from a bit reader.
     *
     * @param reader The bit reader to read from.
     */
    ContactMatrixParameters(util::BitReader& reader);

    /**
     * @brief Gets the number of samples.
     *
     * This function returns the number of samples.
     *
     * @return The number of samples.
     */
    uint8_t getNumSamples() const;

    /**
     * @brief Adds a sample.
     *
     * This function adds a sample with the given information.
     *
     * @param sample_info The information of the sample to add.
     */
    void addSample(SampleInformation&& sample_info);

    /**
     * @brief Adds a sample.
     *
     * This function adds a sample with the given ID, name, and optionally allows the addition if the sample already exists.
     *
     * @param ID The ID of the sample.
     * @param name The name of the sample.
     * @param exist_ok If true, allows the addition if the sample already exists.
     */
    void addSample(uint8_t ID, std::string&& name, bool exist_ok=true);

    /**
     * @brief Gets the samples.
     *
     * This function returns a constant reference to the samples.
     *
     * @return A constant reference to the samples.
     */
    const std::unordered_map<uint8_t, SampleInformation>& getSamples() const;

    /**
     * @brief Gets the number of chromosomes.
     *
     * This function returns the number of chromosomes.
     *
     * @return The number of chromosomes.
     */
    uint8_t getNumChromosomes() const;

    /**
     * @brief Adds a chromosome.
     *
     * This function adds a chromosome with the given information.
     *
     * @param chr_info The information of the chromosome to add.
     */
    void addChromosome(ChromosomeInformation&& chr_info);

    /**
     * @brief Upserts a chromosome.
     *
     * This function upserts a chromosome with the given ID, name, length, and optionally allows the upsert if the chromosome already exists.
     *
     * @param ID The ID of the chromosome.
     * @param name The name of the chromosome.
     * @param length The length
     *  @param exist_ok If true, allows the upsert if the chromosome already exists.
     */
    void upsertChromosome(
        uint8_t ID,
        const std::string& name,
        uint64_t length,
        bool exist_ok=true
    );

    void upsertChromosome(
        uint8_t ID,
        std::string&& name,
        uint64_t length,
        bool exist_ok=true
    );

    /**
     * @brief Gets the chromosomes.
     *
     * This function returns a constant reference to the chromosomes.
     *
     *  @return A constant reference to the chromosomes.
     */
    const std::unordered_map<uint8_t, ChromosomeInformation>& getChromosomes() const;

    /**
     * @brief Gets the bin size.
     *
     * This function returns the bin size.
     *
     *  @return The bin size.
     */
    uint32_t getBinSize() const;

    /**
     * @brief Sets the bin size.
     *
     * This function sets the bin size.
     *
     *  @param _bin_size The bin size to set.
     */
    void setBinSize(uint32_t _bin_size);

    /**
     * @brief Gets the tile size.
     *
     * This function returns the tile size.
     *
     *  @return The tile size.
     */
    uint32_t getTileSize() const;

    /**
     * @brief Sets the tile size.
     *
     * This function sets the tile size.
     *
     *  @param tile_size The tile size to set.
     */
    void setTileSize(uint32_t tile_size);

    /**
     * @brief Gets the number of interval multipliers.
     *
     * This function returns the number of interval multipliers.
     *
     *  @return The number of interval multipliers.
     */
    uint8_t getNumIntervalMultipliers() const;

    /**
     * @brief Gets the number of normalization methods.
     *
     * This function returns the number of normalization methods.
     *
     *  @return The number of normalization methods.
     */
    uint8_t getNumNormMethods() const;

    /**
     * @brief Adds a normalization method.
     *
     * This function adds a normalization method with the given ID and information.
     *
     *  @param ID The ID of the normalization method.
     *  @param norm_method_info The information of the normalization method to add.
     */
    void addNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info);

    /**
     * @brief Gets the normalization methods.
     *
     * This function returns a constant reference to the normalization methods.
     *
     * @return A constant reference to the normalization methods.
     */
    const std::unordered_map<uint8_t, NormalizationMethodInformation>& getNormMethods() const;

    /**
     * @brief Gets the number of normalized matrices.
     *
     * This function returns the number of normalized matrices.
     *
     *   @return The number of normalized matrices.
     */
    uint8_t getNumNormMats() const;

    /**
     * @brief Adds a normalized matrix.
     *
     * This function adds a normalized matrix with the given ID and information.
     *
     *  @param ID The ID of the normalized matrix.
     *  @param norm_mat_info The information of the normalized matrix to add.
     */
    void addNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info);

    /**
     * @brief Gets the normalized matrices.
     *
     * This function returns a constant reference to the normalized matrices.
     *
     *   @return A constant reference to the normalized matrices.
     */
    const std::unordered_map<uint8_t, NormalizedMatrixInformations>& getNormMats() const;

    /**
     * @brief Gets the number of bin entries.
     *
     * This function returns the number of bin entries for a given chromosome ID and an optional interval multiplier.
     *
     * @param chr_ID The ID of the chromosome.
     * @param interv_mult The interval multiplier. Defaults to 1.
     *
     * @return The number of bin entries.
     */
    uint64_t getNumBinEntries(
        uint8_t chr_ID,
        uint32_t interv_mult=1
    );

    /**
     * @brief Gets the number of tiles.
     *
     * This function returns the number of tiles for a given chromosome ID and an optional interval multiplier.
     *
     * @param chr_ID The ID of the chromosome.
     * @param interv_mult The interval multiplier. Defaults to 1.
     *
     * @return The number of tiles.
     */
    uint32_t getNumTiles(
        uint8_t chr_ID,
        uint32_t interv_mult=1
    );

    /**
     * @brief Writes the object to a writer.
     *
     * This function writes the object to a writer.
     *
     * @param writer The writer to write to.
     */
    void write(core::Writer& writer) const;

    /**
     * @brief Gets the size of the object when written to a writer.
     *
     * This function returns the size of the object when written to a writer.
     *
     * @param writesize The writer to calculate the size for.
     *
     * @return The size of the object when written to the writer.
     */
    size_t getSize(core::Writer& writesize) const;
};

// ---------------------------------------------------------------------------------------------------------------------

///**
// * @brief Checks if a pair of chromosomes is symmetrical.
// *
// * This function checks if a pair of chromosomes is symmetrical. It takes the IDs of the two chromosomes as parameters.
// *
// * @param chr1_ID The ID of the first chromosome.
// * @param chr2_ID The ID of the second chromosome.
// *
// * @return True if the pair of chromosomes is symmetrical, false otherwise.
// */
//bool isIntraSCM(uint8_t chr1_ID, uint8_t chr2_ID);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
