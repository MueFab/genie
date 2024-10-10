/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#define NOMINMAX
#include "apps/genie/transcode-fasta/main.h"
#include <filesystem>  // NOLINT
#include <memory>
#include <string>
#include <utility>
#include "apps/genie/transcode-fasta/program-options.h"
#include "genie/format/fasta/manager.h"
#include "genie/format/mgb/raw_reference.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp::transcode_fasta {

// ---------------------------------------------------------------------------------------------------------------------

std::string file_extension(const std::string& path) {
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    for (auto& c : ext) {
        c = static_cast<char>(std::tolower(c));
    }
    return ext;
}

// ---------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ProgramOptions pOpts(argc, argv);
    if (pOpts.help) {
        return 0;
    }

    auto in_ext = file_extension(pOpts.inputFile);
    auto out_ext = file_extension(pOpts.outputFile);

    if (in_ext == "mgr" && (out_ext == "fasta" || out_ext == "fa")) {
        UTILS_DIE("Mgr to fasta transcoding not yet supported");
    } else if (!(out_ext == "mgr" && (in_ext == "fasta" || in_ext == "fa"))) {
        UTILS_DIE("Unknown combination of file extensions.");
    }

    std::string fai_name = pOpts.inputFile.substr(0, pOpts.inputFile.find_last_of('.')) + ".fai";
    std::string sha_name = pOpts.inputFile.substr(0, pOpts.inputFile.find_last_of('.')) + ".sha256";
    if (!std::filesystem::exists(fai_name)) {
        std::ifstream fasta_in(pOpts.inputFile);
        std::ofstream fai_out(fai_name);
        genie::format::fasta::FastaReader::index(fasta_in, fai_out);
    }
    if (!std::filesystem::exists(sha_name)) {
        std::ifstream fasta_in(pOpts.inputFile);
        std::ifstream fai_in(fai_name);
        genie::format::fasta::FaiFile faifile(fai_in);
        std::ofstream sha_out(sha_name);
        genie::format::fasta::FastaReader::hash(faifile, fai_in, sha_out);
    }

    auto fastaFile = std::make_unique<std::ifstream>(pOpts.inputFile);
    auto faiFile = std::make_unique<std::ifstream>(fai_name);
    auto shaFile = std::make_unique<std::ifstream>(sha_name);
    auto refMgr = std::make_unique<genie::core::ReferenceManager>(4);
    auto fastaMgr =
        std::make_unique<genie::format::fasta::Manager>(*fastaFile, *faiFile, *shaFile, refMgr.get(), pOpts.inputFile);
    genie::format::mgb::RawReference raw_ref(true);
    for (uint16_t i = 0; i < static_cast<uint16_t>(refMgr->getSequences().size()); ++i) {
        auto name = refMgr->ID2Ref(i);
        auto length = refMgr->getLength(name);
        std::string seq_str = refMgr->load(name, 0, length).getString(0, length);
        raw_ref.addSequence(genie::format::mgb::RawReferenceSequence(i, 0, std::move(seq_str)));
    }

    std::ofstream outfile(pOpts.outputFile);
    genie::util::BitWriter bw(outfile);
    raw_ref.write(bw);

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genieapp::transcode_fasta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
