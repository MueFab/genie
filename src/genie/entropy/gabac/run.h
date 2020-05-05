/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_RUN_H_
#define GABAC_RUN_H_
namespace genie {
namespace entropy {
namespace gabac {

struct EncodingConfiguration;
struct IOConfiguration;

/**
 * @brief Start a run of gabac core
 * @param conf i/o configuration
 * @param enConf gabac configuration
 * @param decode If you want to decode (true) or encode (false).
 */
unsigned long run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_RUN_H_
