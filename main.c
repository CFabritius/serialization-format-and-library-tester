#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "library_interface.h"

#define BOLD   "\033[1m"
#define GREEN  "\033[32m"
#define RESULT BOLD GREEN
#define RESET  "\033[0m"


#ifndef LIBRARY
#define LIBRARY LIB_JANSSON
#endif



typedef enum {
    LIB_MXML        = 0,
    LIB_FYAML       = 1,
    LIB_JANSSON     = 2,
    LIB_PARSON      = 3,
    LIB_CJSON       = 4,
    LIB_MSGPACK     = 5,
    LIB_FLATBUFFERS = 6
} Library;

char* library_string() {
    switch(LIBRARY) {
        case LIB_MXML:        return "Mini-XML";
        case LIB_FYAML:       return "libfyaml";
        case LIB_JANSSON:     return "Jansson";
        case LIB_PARSON:      return "Parson";
        case LIB_CJSON:       return "cJSON";
        case LIB_MSGPACK:     return "msgpack-c";
        case LIB_FLATBUFFERS: return "flatcc";
        default:              return "Unknown library";
    }
}

char* library_format_string() {
    switch(LIBRARY) {
        case LIB_MXML:        return "XML";
        case LIB_FYAML:       return "YAML";
        case LIB_JANSSON:     return "JSON";
        case LIB_PARSON:      return "JSON";
        case LIB_CJSON:       return "JSON";
        case LIB_MSGPACK:     return "MessagePack";
        case LIB_FLATBUFFERS: return "FlatBuffers";
        default:              return "Unknown format";
    }
}

char* library_output_filename() {
    switch(LIBRARY) {
        case LIB_MXML:        return "./output/mxml.xml";
        case LIB_FYAML:       return "./output/libfyaml.yaml";
        case LIB_JANSSON:     return "./output/jansson.json";
        case LIB_PARSON:      return "./output/parson.json";
        case LIB_CJSON:       return "./output/cjson.json";
        case LIB_MSGPACK:     return "./output/msgpack-c.bin";
        case LIB_FLATBUFFERS: return "./output/flatcc.bin";
        default:              return "unknown";
    }
}



static double diff_ms(struct timespec a, struct timespec b) {
    long diff_sec  = b.tv_sec  - a.tv_sec;
    long diff_nsec = b.tv_nsec - a.tv_nsec;
    return (double)diff_sec * 1000.0 + (double)diff_nsec / 1e6;
}



typedef struct {
    unsigned char* content;
    long size;
} SerializedData;

SerializedData read_serialized_data(const char* path, bool binary) {
    FILE* fp = fopen(path, binary ? "rb" : "r");
    SerializedData data;

    fseek(fp, 0, SEEK_END);
    data.size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    data.content = malloc(data.size);

    fread(data.content, 1, data.size, fp);

    fclose(fp);
    return data;
}

long get_file_size(const char* path) {
    FILE* fp = fopen(path, "rb");
    fseek(fp, 0, SEEK_END);
    long byte_count = ftell(fp);
    fclose(fp);
    return byte_count;
}

int main() {
    setlocale(LC_NUMERIC, "");

    struct timespec start, end;
    double wall_time;

    FILE *fp;
    SerializedData data;
    Node *deserialized;

    Node* root = generate_tree(DEPTH);
    printf("Depth: " BOLD "%d" RESET " layers total\n", DEPTH);
    printf("Breadth: " BOLD "%d" RESET " children per node\n", BREADTH);
    printf("Total: " BOLD "%d" RESET " nodes\n", count_nodes(root));
    printf("Library: " BOLD "%s" RESET " (%s)\n", library_string(), library_format_string());
    printf(BOLD "-------------------\n" RESET);

    printf("Started Serialization\n");

    
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    switch(LIBRARY) {
        case LIB_MXML:
            mxml_node_t *mxml_tree = mxml_serialize_node(root);
            mxmlSaveFilename(mxml_tree, NULL, library_output_filename());
            break;
        
        case LIB_FYAML:
            printf("NOT IMPLEMENTED!\n");
            /*struct fy_document *fyd = fy_document_create(NULL);
            libfyaml_serialize_node(fyd, NULL, root);
            fy_emit_document_to_file(fyd, FYECF_SORT_KEYS, library_output_filename());*/
            break;

        case LIB_JANSSON:
            json_t *jansson_tree = jansson_serialize_node(root);
            json_dump_file(jansson_tree, library_output_filename(), JSON_COMPACT/*JSON_INDENT(4)*/);
            break;

        case LIB_PARSON:
            PARSON_Value* parson_tree = parson_serialize_node(root);
            parson_serialize_to_file(parson_tree, library_output_filename());
            break;

        case LIB_CJSON:
            cJSON* cjson_tree = cjson_serialize_node(root);
            fp = fopen(library_output_filename(), "w");
            fputs(cJSON_PrintUnformatted(cjson_tree), fp);
            fclose(fp);
            break;

        case LIB_MSGPACK:
            msgpack_sbuffer* buffer = msgpack_sbuffer_new();
            msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
            msgpack_serialize_node(pk, root);

            fp = fopen(library_output_filename(), "wb");
            fwrite(buffer->data, buffer->size, 1, fp);
            fclose(fp);
            break;

        case LIB_FLATBUFFERS:
            printf("NOT IMPLEMENTED!\n");
            /*flatcc_builder_t builder;
            flatcc_builder_init(&builder);
            Tree_Node_ref_t root_ref = flatcc_serialize_node(&builder, root);

            size_t size;
            void* flat_buffer = flatcc_builder_finalize_buffer(&builder, &size);
            printf("IS EVERYTHING FUCKED?! -> %d\n", Tree_Node_verify_as_root(flat_buffer, size));

            fp = fopen(library_output_filename(), "wb");
            fwrite(flat_buffer, 1, size, fp);
            fclose(fp);*/
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    wall_time = diff_ms(start, end);
    printf("Output size: " RESULT "%d bytes\n" RESET, get_file_size(library_output_filename()));
    printf("Wall time: " RESULT "%.3f ms\n" RESET, wall_time);
    printf(BOLD "-------------------\n" RESET);
    printf("Started Deserialization\n");

    clock_gettime(CLOCK_MONOTONIC, &start);

    switch(LIBRARY) {
        case LIB_MXML:
            FILE* fp = fopen(library_output_filename(), "r");
            mxml_node_t* xml_root = mxmlLoadFile(NULL, NULL, fp);
            deserialized = mxml_deserialize_node(xml_root);
            break;

        case LIB_FYAML:
            printf("NOT IMPLEMENTED!\n");
            break;

        case LIB_JANSSON:
            json_error_t error;
            json_t* json_root = json_load_file(library_output_filename(), 0, &error);
            deserialized = jansson_deserialize_node(json_root);
            break;

        case LIB_PARSON:
            PARSON_Value* parson_root = parson_parse_file(library_output_filename());
            deserialized = parson_deserialize_node(parson_root);
            break;

        case LIB_CJSON:
            data = read_serialized_data(library_output_filename(), false);
            
            cJSON* cjson_root = cJSON_ParseWithLength(data.content, data.size);
            deserialized = cjson_deserialize_node(cjson_root);
            break;

        case LIB_MSGPACK:
            data = read_serialized_data(library_output_filename(), true);

            msgpack_unpacker* unpacker = msgpack_unpacker_new(data.size);
            memcpy(msgpack_unpacker_buffer(unpacker), data.content, data.size);
            msgpack_unpacker_buffer_consumed(unpacker, data.size);

            msgpack_unpacked msgpack_root;
            msgpack_unpacked_init(&msgpack_root);
            if(!msgpack_unpacker_next(unpacker, &msgpack_root)) {
                printf("msgpack_unpacker_next failed!\n");
            }

            printf("root type: %d\n", msgpack_root.data.type);

            deserialized = msgpack_deserialize_node(&msgpack_root.data);
            break;

        case LIB_FLATBUFFERS:
            printf("NOT IMPLEMENTED!\n");
            //data = read_serialized_data(library_output_filename(), true);
            //printf("%d\n", Tree_Node_verify_as_root(data.content, data.size));
            //Tree_Node_table_t flatbuffers_root = Tree_Node_as_root(data.content);
            //deserialized = flatcc_deserialize_node(flatbuffers_root);
            break;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Trees " RESULT "are%s" RESET " the same!\n", bool_word(compare_tree(root, deserialized)));

    wall_time = diff_ms(start, end);
    printf("Wall time: " RESULT "%.3f ms\n" RESET, wall_time);
}
