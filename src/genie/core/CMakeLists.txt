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

        record/alignment_external/none.cc
        record/alignment_external/other_rec.cc
        record/alignment_split/other_rec.cc
        record/alignment_split/same_rec.cc
        record/alignment_split/unpaired.cc
        record/segment.cc
        record/record.cc
        record/chunk.cc
        record/alignment_split.cc
        record/alignment_shared_data.cc
        record/alignment_external.cc
        record/alignment_box.cc
        record/alignment.cc

        variant_site_record/record.cc

        variant_genotype_record/record.cc
        variant_genotype_record/format_field.cc
        variant_genotype_record/format_field.h

        record/linked_record/linked_record.h
        record/linked_record/linked_record.cc

        record/contact_record/record.cc

        record/data_unit_record/record.cc

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

        stats/perf_stats.cc

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
        meta/access_unit.cc
        meta/block_header.cc
        meta/dataset.cc
        meta/dataset_group.cc
        meta/descriptor_stream.cc
        meta/label.cc
        meta/external_ref.cc
        meta/internal_ref.cc
        meta/external_ref/fasta.cc
        meta/external_ref/raw.cc
        meta/external_ref/mpeg.cc
        meta/block_header/enabled.cc
        meta/block_header/disabled.cc
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
