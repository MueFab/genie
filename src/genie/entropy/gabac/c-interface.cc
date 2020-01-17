/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "c-interface.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "gabac.h"

const uint8_t gabac_sequence_transform_params[] = {0, 0, 1, 1, 1, 0, 4};
/*const uint8_t gabac_sequence_transform_streams[] = {
        1,
        2,
        3,
        2,
        3,
        1,
        1
};

const uint8_t gabac_sequence_transform_word_sizes[][3] = {
        {0, 255, 255},
        {0, 1,   255},
        {0, 4,   4},
        {0, 4,   255},
        {0, 0,   0},
        {0, 255, 255},
        {0, 255, 255}
};*/

int gabac_data_block_swap(gabac_data_block *stream1, gabac_data_block *stream2) {
    gabac_data_block tmp;
    memcpy(&tmp, stream1, sizeof(gabac_data_block));
    memcpy(stream1, stream2, sizeof(gabac_data_block));
    memcpy(stream2, &tmp, sizeof(gabac_data_block));
    return gabac_return_SUCCESS;
}

int gabac_data_block_copy(gabac_data_block *drain, gabac_data_block *source) {
    drain->word_size = source->word_size;
    if (gabac_data_block_resize(drain, source->values_size)) {
        return gabac_return_FAILURE;
    }
    memcpy(drain->values, source->values, source->word_size * source->values_size);
    return gabac_return_SUCCESS;
}

int gabac_data_block_init(gabac_data_block *block, const void *data, size_t size, uint8_t wordsize) {
    if (wordsize != 1 && wordsize != 2 && wordsize != 4 && wordsize != 8) {
        goto ERROR;
    }

    block->values = static_cast<uint8_t *>(calloc(wordsize * size * sizeof(uint8_t), 1));
    if (!block->values) {
        goto ERROR;
    }
    block->values_size = size;
    block->word_size = wordsize;
    if (data) {
        memcpy(block->values, data, size * wordsize);
    }

    return gabac_return_SUCCESS;

ERROR:
    block->values = nullptr;
    block->values_size = 0;
    block->word_size = 0;
    return gabac_return_FAILURE;
}

int gabac_data_block_release(gabac_data_block *block) {
    free(block->values);
    block->values = nullptr;
    block->values_size = 0;
    block->word_size = 0;
    return gabac_return_SUCCESS;
}

int gabac_data_block_resize(gabac_data_block *block, size_t size) {
    block->values_size = size * block->word_size;
    block->values = static_cast<uint8_t *>(realloc(block->values, block->values_size));
    if (!block->values && block->values_size) {
        block->values_size = 0;
        block->word_size = 1;
        return gabac_return_FAILURE;
    }
    return gabac_return_SUCCESS;
}

int gabac_data_block_equals(gabac_data_block *block1, gabac_data_block *block2) {
    return genie::util::DataBlock(block1->values, block1->values_size, block1->word_size) ==
           genie::util::DataBlock(block2->values, block2->values_size, block2->word_size);
}

uint64_t gabac_data_block_max(gabac_data_block *block) {
    return genie::util::DataBlock(block->values, block->values_size, block->word_size).getMaximum();
}

uint8_t gabac_data_block_max_wordsize(gabac_data_block *block) {
    return genie::util::DataBlock(block->values, block->values_size, block->word_size).getMaxWordSize();
}

uint64_t gabac_data_block_get(const gabac_data_block *block, size_t index) {
    switch (block->word_size) {
        case 1:
            return *(block->values + index);
        case 2:
            return *reinterpret_cast<uint16_t *>(block->values + (index << 1u));
        case 4:
            return *reinterpret_cast<uint32_t *>(block->values + (index << 2u));
        case 8:
            return *reinterpret_cast<uint64_t *>(block->values + (index << 3u));
        default:
            return 0;
    }
}

void gabac_data_block_set(const gabac_data_block *block, size_t index, uint64_t val) {
    switch (block->word_size) {
        case 1:
            *(block->values + index) = static_cast<uint8_t>(val);
            return;
        case 2:
            *reinterpret_cast<uint16_t *>(block->values + (index << 1u)) = static_cast<uint16_t>(val);
            return;
        case 4:
            *reinterpret_cast<uint32_t *>(block->values + (index << 2u)) = static_cast<uint32_t>(val);
            return;
        case 8:
            *reinterpret_cast<uint64_t *>(block->values + (index << 3u)) = static_cast<uint64_t>(val);
            return;
        default:
            return;
    }
}

const char *gabac_stream_create_file_STDOUT = "@STDOUT@";
const char *gabac_stream_create_file_STDERR = "@STDERR@";
const char *gabac_stream_create_file_STDIN = "@STDIN@";
const char *gabac_stream_create_file_TMP = "@TMP@";

int gabac_stream_create_file(gabac_stream *stream, const char *filename, size_t filename_size, int write) {
    const char *mode = write ? "wb" : "rb";
    filename_size = std::max(strlen(gabac_stream_create_file_STDOUT), filename_size);
    if (!strncmp(filename, gabac_stream_create_file_STDOUT, filename_size)) {
        stream->data = stdout;
    } else if (!strncmp(filename, gabac_stream_create_file_STDERR, filename_size)) {
        stream->data = stderr;
    } else if (!strncmp(filename, gabac_stream_create_file_STDIN, filename_size)) {
        stream->data = stdin;
    } else if (!strncmp(filename, gabac_stream_create_file_TMP, filename_size)) {
        stream->data = tmpfile();
    } else {
        stream->data = fopen(filename, mode);
    }
    if (!stream->data) {
        return gabac_return_FAILURE;
    }
    stream->input_mode = gabac_stream_mode_FILE;
    return gabac_return_SUCCESS;
}

int gabac_stream_create_file(gabac_stream *stream, FILE *file) {
    stream->data = file;
    stream->input_mode = gabac_stream_mode_FILE;
    return gabac_return_SUCCESS;
}

int gabac_stream_create_buffer(gabac_stream *stream, gabac_data_block *block) {
    stream->data = calloc(1, sizeof(gabac_data_block));
    if (!stream->data) return gabac_return_FAILURE;
    static_cast<gabac_data_block *>(stream->data)->word_size = 1;
    if (block) {
        if (gabac_data_block_swap(static_cast<gabac_data_block *>(stream->data), block)) {
            return gabac_return_FAILURE;
        }
    }
    stream->input_mode = gabac_stream_mode_BUFFER;
    return gabac_return_SUCCESS;
}

int gabac_stream_swap_block(gabac_stream *stream, gabac_data_block *block) {
    if (stream->input_mode != gabac_stream_mode_BUFFER) {
        return gabac_return_FAILURE;
    }
    auto *cblock = static_cast<gabac_data_block *>(stream->data);
    return gabac_data_block_swap(block, cblock);
}

int gabac_stream_swap_file(gabac_stream *stream, FILE **file) {
    if (stream->input_mode != gabac_stream_mode_FILE) {
        return gabac_return_FAILURE;
    }
    FILE *tmp = *file;
    *file = static_cast<FILE *>(stream->data);
    stream->data = tmp;
    return gabac_return_SUCCESS;
}

int gabac_stream_release(gabac_stream *stream) {
    if (stream->input_mode == gabac_stream_mode_BUFFER) {
        gabac_data_block_release(static_cast<gabac_data_block *>(stream->data));
        free(stream->data);
    } else if (stream->data) {
        if (stream->data != stdin && stream->data != stdout && stream->data != stderr) {
            fclose(reinterpret_cast<FILE *>(stream->data));
        }
    }
    stream->data = nullptr;
    return gabac_return_SUCCESS;
}

int gabac_execute_transform(uint8_t transformationID, const uint64_t *param, int inverse, gabac_data_block *input) {
    try {
        auto transID = genie::entropy::gabac::SequenceTransformationId(transformationID);
        std::vector<genie::util::DataBlock> blocks(genie::entropy::gabac::getTransformation(transID).wordsizes.size());
        std::vector<uint64_t> params_vec(gabac_sequence_transform_params[transformationID]);
        for (size_t i = 0; i < blocks.size(); ++i) {
            blocks[i] = genie::util::DataBlock(input[i].values, input[i].values_size, input[i].word_size);
            if (gabac_data_block_release(&input[i])) {
                GABAC_DIE("C interface error");
            }
        }

        for (size_t i = 0; i < params_vec.size(); ++i) {
            params_vec[i] = param[i];
        }

        if (inverse) {
            genie::entropy::gabac::getTransformation(transID).inverseTransform(params_vec, &blocks);
        } else {
            genie::entropy::gabac::getTransformation(transID).transform(params_vec, &blocks);
        }

        for (size_t i = 0; i < blocks.size(); ++i) {
            if (gabac_data_block_init(&input[i], blocks[i].getData(), blocks[i].size(), blocks[i].getWordSize())) {
                GABAC_DIE("C interface error");
            }
            blocks[i].clear();
            blocks[i].shrink_to_fit();
        }
        return gabac_return_SUCCESS;
    } catch (...) {
        return gabac_return_FAILURE;
    }
}

int gabac_run(gabac_operation operation, gabac_io_config *io_config, const char *config_json, size_t json_length) {
    try {
        genie::entropy::gabac::IOConfiguration ioconf_cpp = {nullptr, nullptr, 0, nullptr,
                                                             genie::entropy::gabac::IOConfiguration::LogLevel::TRACE};
        ioconf_cpp.blocksize = io_config->blocksize;
        ioconf_cpp.level = static_cast<genie::entropy::gabac::IOConfiguration::LogLevel>(io_config->log_level);
        std::unique_ptr<std::ostream> output;
        std::unique_ptr<std::ostream> log;
        std::string config(config_json, json_length);

        if (io_config->output.input_mode == gabac_stream_mode_FILE) {
            output.reset(new genie::entropy::gabac::OFileStream(static_cast<FILE *>(io_config->output.data)));
        } else {
            auto *ptr = static_cast<gabac_data_block *>(io_config->output.data);
            genie::util::DataBlock tmp(0, ptr->word_size);
            output.reset(new genie::entropy::gabac::OBufferStream(&tmp));
        }
        ioconf_cpp.outputStream = output.get();

        if (io_config->log.input_mode == gabac_stream_mode_FILE) {
            log.reset(new genie::entropy::gabac::OFileStream(static_cast<FILE *>(io_config->log.data)));
        } else {
            auto *ptr = static_cast<gabac_data_block *>(io_config->log.data);
            genie::util::DataBlock tmp(0, ptr->word_size);
            log.reset(new genie::entropy::gabac::OBufferStream(&tmp));
        }
        ioconf_cpp.logStream = log.get();

        {
            std::unique_ptr<std::istream> input;
            if (io_config->input.input_mode == gabac_stream_mode_FILE) {
                input.reset(new genie::entropy::gabac::IFileStream(static_cast<FILE *>(io_config->input.data)));
            } else {
                auto *cblock = static_cast<gabac_data_block *>(io_config->input.data);
                genie::util::DataBlock tmp(cblock->values, cblock->values_size, cblock->word_size);
                if (gabac_data_block_resize(cblock, 0)) {
                    GABAC_DIE("Resize failed - input");
                }
                input.reset(new genie::entropy::gabac::IBufferStream(&tmp));
            }
            ioconf_cpp.inputStream = input.get();

            genie::entropy::gabac::EncodingConfiguration enConf;
            genie::entropy::gabac::AnalysisConfiguration analyseConfig;
            switch (operation) {
                case gabac_operation_ANALYZE:
                    // analyseConfig = ...
                    genie::entropy::gabac::analyze(ioconf_cpp, analyseConfig);
                    break;
                case gabac_operation_ENCODE:
                    enConf = genie::entropy::gabac::EncodingConfiguration(config);
                    genie::entropy::gabac::run(ioconf_cpp, enConf, false);
                    break;
                case gabac_operation_DECODE:
                    enConf = genie::entropy::gabac::EncodingConfiguration(config);
                    genie::entropy::gabac::run(ioconf_cpp, enConf, true);
                    break;
            }
        }

        if (io_config->output.input_mode == gabac_stream_mode_BUFFER) {
            genie::util::DataBlock b(0, 1);
            auto *cblock = static_cast<gabac_data_block *>(io_config->output.data);
            dynamic_cast<genie::entropy::gabac::OBufferStream *>(output.get())->flush(&b);
            cblock->word_size = b.getWordSize();
            if (gabac_data_block_resize(cblock, b.size())) {
                GABAC_DIE("Resize failed - output");
            }
            memcpy(cblock->values, b.getData(), b.getRawSize());
        }

        if (io_config->log.input_mode == gabac_stream_mode_BUFFER) {
            genie::util::DataBlock b(0, 1);
            auto *cblock = static_cast<gabac_data_block *>(io_config->log.data);
            dynamic_cast<genie::entropy::gabac::OBufferStream *>(log.get())->flush(&b);
            cblock->word_size = b.getWordSize();
            if (gabac_data_block_resize(cblock, b.size())) {
                GABAC_DIE("Resize failed - log");
            }
            memcpy(cblock->values, b.getData(), b.getRawSize());
        }
        return gabac_return_SUCCESS;
    } catch (genie::entropy::gabac::Exception &e) {
        return gabac_return_FAILURE;
    }
}

int gabac_config_is_general(const char *inconf, size_t inconf_size, uint64_t max, uint8_t wsize) {
    genie::entropy::gabac::EncodingConfiguration conf(std::string(inconf, inconf_size));
    return conf.isGeneral(max, wsize);
}

int gabac_config_generalize_create(const char *inconf, size_t inconf_size, uint64_t max, uint8_t wsize, char **outconf,
                                   size_t *outconf_size) {
    genie::entropy::gabac::EncodingConfiguration conf(std::string(inconf, inconf_size));
    auto nconf = conf.generalize(max, wsize);
    auto str = nconf.toJsonString();
    *outconf = static_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
    *outconf_size = str.size();
    memcpy(*outconf, str.c_str(), *outconf_size + 1);
    return gabac_return_SUCCESS;
}

int gabac_config_is_optimal(const char *inconf, size_t inconf_size, uint64_t max, uint8_t wsize) {
    genie::entropy::gabac::EncodingConfiguration conf(std::string(inconf, inconf_size));
    return conf.isOptimal(max, wsize);
}

int gabac_config_optimize_create(const char *inconf, size_t inconf_size, uint64_t max, uint8_t wsize, char **outconf,
                                 size_t *outconf_size) {
    genie::entropy::gabac::EncodingConfiguration conf(std::string(inconf, inconf_size));
    auto nconf = conf.optimize(max, wsize);
    auto str = nconf.toJsonString();
    *outconf = static_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
    *outconf_size = str.size();
    memcpy(*outconf, str.c_str(), *outconf_size + 1);
    return gabac_return_SUCCESS;
}

int gabac_config_free(char **outconf) {
    free(*outconf);
    *outconf = nullptr;
    return gabac_return_SUCCESS;
}
