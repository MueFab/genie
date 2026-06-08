project("genie-core")

set(source_files
        parameter/descriptor_present/decoder.cc
        parameter/descriptor_present/decoder_regular.cc
        parameter/descriptor_present/decoder_token_type.cc
        parameter/descriptor_present/descriptor_present.cc

        parameter/computed_ref.cc
        parameter/computed_ref_extended.cc
        parameter/data_unit.cc
        parameter/descriptor.cc
        parameter/descriptor_sub_sequence_cfg.cc
        parameter/parameter_set.cc
        parameter/quality_values.cc

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

        record/variant/record.cc
        record/variant/format_field.cc
        linked_record/linked_record.cc
        record/site/record.cc
        record/data_unit/record.cc
        record/contact/record.cc
        feature_record/record.cc
        functional_annotation_record/record.cc
        gene_expression_record/record.cc
        sample_record/record.cc
        track_property_record/record.cc
        track_record/record.cc

        access_unit/annotation/record.cc
        access_unit/annotation/block.cc
        access_unit/annotation/block_header.cc
        access_unit/annotation/block_payload.cc
        access_unit/annotation/block_payload_stream.cc
        access_unit/annotation/annotation_access_unit_header.cc
        access_unit/annotation/typed_data.cc

        parameter/annotation/record.cc
        parameter/annotation/attribute_parameter_set.cc
        parameter/annotation/algorithm_parameters.cc
        parameter/annotation/tile_configuration.cc
        parameter/annotation/tile_structure.cc
        parameter/annotation/descriptor_configuration.cc
        parameter/annotation/compressor_parameter_set.cc
        parameter/annotation/annotation_encoding_parameters.cc
        parameter/annotation/attribute_data.cc

        array_type.cc
        writer.cc

        stats/perf_stats.cc

        access_unit.cc
        classifier_bypass.cc
        classifier_regroup.cc
        constants.cc
        flow_graph.cc
        flow_graph_convert.cc
        flow_graph_decode.cc
        flow_graph_encode.cc
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
        payload.cc)

add_library(genie-core ${source_files})


target_link_libraries(genie-core PUBLIC genie-util genie-backend)

get_filename_component(THIS_DIR ./ ABSOLUTE)
get_filename_component(TOP_DIR ../../ ABSOLUTE)
target_include_directories(genie-core PUBLIC "${TOP_DIR}")
