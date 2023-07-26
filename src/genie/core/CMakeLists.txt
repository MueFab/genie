project("genie-core")

set(source_files
        parameter/descriptor_present/decoder.cc
        parameter/descriptor_present/decoder-regular.cc
        parameter/descriptor_present/decoder-tokentype.cc
        parameter/descriptor_present/descriptor_present.cc

        parameter/computed_ref.cc
        parameter/computed_ref_extended.cc
        parameter/data_unit.cc
        parameter/descriptor.cc
        parameter/descriptor_subseq_cfg.cc
        parameter/parameter_set.cc
        parameter/quality-values.cc

        record/alignment/alignment_external/none.cc
        record/alignment/alignment_external/other-rec.cc
        record/alignment/alignment_split/other-rec.cc
        record/alignment/alignment_split/same-rec.cc
        record/alignment/alignment_split/unpaired.cc
        record/alignment/segment.cc
        record/alignment/record.cc
        record/alignment/chunk.cc
        record/alignment/alignment-split.cc
        record/alignment/alignment-shared-data.cc
        record/alignment/alignment-external.cc
        record/alignment/alignment-box.cc
        record/alignment/alignment.cc

        record/variant_site/record.cc
        record/variant_site/VariantSiteParser.cc
        record/variant_site/JsonInfoFieldParser.cc

        record/variant_genotype/record.cc

        record/data_unit/record.cc

        record/contact/record.cc

        record/annotation_access_unit/record.cc
        record/annotation_access_unit/block.cc
        record/annotation_access_unit/BlockHeader.cc
        record/annotation_access_unit/BlockPayload.cc
        record/annotation_access_unit/BlockPayloadStream.cc
        record/annotation_access_unit/AnnotationAccessUnitHeader.cc
        record/annotation_access_unit/VariantsPayload.cc
        record/annotation_access_unit/RowColIDsPayload.cc
        record/annotation_access_unit/AmaxPayload.cc
        record/annotation_access_unit/BinaryMatrixPayload.cc
        record/annotation_access_unit/GenotypePayload.cc
        record/annotation_access_unit/LikelihoodPayload.cc
        record/annotation_access_unit/ContactMatrixBinPayload.cc
        record/annotation_access_unit/ContactMatrixMatPayload.cc

        record/annotation_parameter_set/record.cc
        record/annotation_parameter_set/LikelihoodParameters.cc
        record/annotation_parameter_set/GenotypeParameters.cc
        record/annotation_parameter_set/AttributeParameterSet.cc
        record/annotation_parameter_set/AlgorithmParameters.cc
        record/annotation_parameter_set/ContactMatrixParameters.cc
        record/annotation_parameter_set/TileConfiguation.cc
        record/annotation_parameter_set/TileStructure.cc
        record/annotation_parameter_set/DescriptorConfiguration.cc
        record/annotation_parameter_set/CompressorParameterSet.cc
        record/annotation_parameter_set/AnnotationEncodingParameters.cc

        arrayType.cc
        writer.cc

        stats/perf-stats.cc

        access-unit.cc
        classifier-bypass.cc
        classifier-regroup.cc
        constants.cc
        flowgraph.cc
        flowgraph-convert.cc
        flowgraph-decode.cc
        flowgraph-encode.cc
        format-importer.cc
        format-importer-null.cc
        format-exporter.cc
        format-importer-compressed.cc
        format-exporter-compressed.cc
        global-cfg.cc
        locus.cc
        name-encoder-none.cc
        read-encoder.cc
        read-decoder.cc
        reference.cc
        reference-collection.cc
        reference-manager.cc
        reference-source.cc
        meta/sequence.cc
        meta/region.cc
        meta/reference.cc
        meta/access-unit.cc
        meta/blockheader.cc
        meta/dataset.cc
        meta/dataset-group.cc
        meta/descriptor-stream.cc
        meta/label.cc
        meta/external-ref.cc
        meta/internal-ref.cc
        meta/external-ref/fasta.cc
        meta/external-ref/raw.cc
        meta/external-ref/mpeg.cc
        meta/blockheader/enabled.cc
        meta/blockheader/disabled.cc
        api.cc
        c_api.cc
        payload.cc)

add_library(genie-core ${source_files})


target_link_libraries(genie-core PUBLIC genie-util)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-core PUBLIC "${TOP_DIR}")
