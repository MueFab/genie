project("genie-core")

set(source_files
        parameter/sequencing/descriptor_present/decoder.cc
        parameter/sequencing/descriptor_present/decoder_regular.cc
        parameter/sequencing/descriptor_present/decoder_token_type.cc
        parameter/sequencing/descriptor_present/descriptor_present.cc

        parameter/sequencing/computed_ref.cc
        parameter/sequencing/computed_ref_extended.cc
        parameter/sequencing/data_unit.cc
        parameter/sequencing/descriptor.cc
        parameter/sequencing/descriptor_sub_sequence_cfg.cc
        parameter/sequencing/parameter_set.cc
        parameter/sequencing/quality_values.cc

        record/sequencing/alignment_external/none.cc
        record/sequencing/alignment_external/other_rec.cc
        record/sequencing/alignment_split/other_rec.cc
        record/sequencing/alignment_split/same_rec.cc
        record/sequencing/alignment_split/unpaired.cc
        record/sequencing/segment.cc
        record/sequencing/record.cc
        record/sequencing/chunk.cc
        record/sequencing/alignment_split.cc
        record/sequencing/alignment_shared_data.cc
        record/sequencing/alignment_external.cc
        record/sequencing/alignment_box.cc
        record/sequencing/alignment.cc

        record/genotype/record.cc
        record/genotype/format_field.cc
        record/linked/record.cc
        record/site/record.cc
        record/data_unit/record.cc
        record/contact/record.cc
        record/feature/record.cc
        record/functional_annotation/record.cc
        record/gene_expression/record.cc
        record/sample/record.cc
        record/track_property/record.cc
        record/track/record.cc

        access_unit/annotation/access_unit.cc
        access_unit/annotation/block.cc
        access_unit/annotation/block_header.cc
        access_unit/annotation/block_payload.cc
        access_unit/annotation/block_payload_stream.cc
        access_unit/annotation/annotation_access_unit_header.cc
        access_unit/annotation/typed_data.cc

        parameter/annotation/parameter_set.cc
        parameter/annotation/attribute_parameter_set.cc
        parameter/annotation/algorithm_parameters.cc
        parameter/annotation/tile_configuration.cc
        parameter/annotation/tile_structure.cc
        parameter/annotation/descriptor_configuration.cc
        parameter/annotation/compressor_parameter_set.cc
        parameter/annotation/annotation_encoding_parameters.cc
        parameter/annotation/attribute_data.cc

        array_type.cc
        # writer.cc

        stats/perf_stats.cc

        access_unit/sequencing/access_unit.cc
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
