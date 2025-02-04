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

        record/variant_genotype/record.cc
        record/variant_genotype/format_field.cc
        record/variant_genotype/format_field.h

        record/linked_record/linked_record.h
        record/linked_record/linked_record.cc

        record/contact/record.cc

        record/data_unit/record.cc

        record/annotation_access_unit/record.cc
        record/annotation_access_unit/block.cc
        record/annotation_access_unit/BlockHeader.cc
        record/annotation_access_unit/BlockPayload.cc
        record/annotation_access_unit/AnnotationAccessUnitHeader.cc
        record/annotation_access_unit/TypedData.cc

        record/annotation_parameter_set/record.cc
        record/annotation_parameter_set/AttributeParameterSet.cc
        record/annotation_parameter_set/AlgorithmParameters.cc
        record/annotation_parameter_set/ContactMatrixParameters.cc
        record/annotation_parameter_set/TileConfiguation.cc
        record/annotation_parameter_set/TileStructure.cc
        record/annotation_parameter_set/DescriptorConfiguration.cc
        record/annotation_parameter_set/CompressorParameterSet.cc
        record/annotation_parameter_set/AnnotationEncodingParameters.cc
        record/annotation_parameter_set/AttributeData.cc

        arrayType.cc
        writer.cc

        stats/perf-stats.cc

        access_unit.cc
        classifier_bypass.cc
        classifier_regroup.cc
        constants.cc
        flowgraph.cc
        flowgraph_convert.cc
        flowgraph_decode.cc
        flowgraph_encode.cc
        format_importer.cc
        format_importer_null.cc
        format_exporter.cc
        format_importer_compressed.cc
        format_exporter_compressed.cc
        global_cfg.cc
        locus.cc
        name_encoder_none.cc
        read_encoder.cc
        read_decoder.cc
        reference.cc
        reference_collection.cc
        reference_manager.cc
        reference_source.cc
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
        payload.cc
        ndarray.cc
)

add_library(genie-core ${source_files})


target_link_libraries(genie-core PUBLIC genie-util)
target_link_libraries(genie-core PUBLIC genie-likelihood)
target_link_libraries(genie-core PUBLIC genie-contact)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
message("topdir='${TOP_DIR}'")
target_include_directories(genie-core PUBLIC "${TOP_DIR}")
