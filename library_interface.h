#include <mxml.h>
#include <libfyaml.h>
#include <jansson.h>
#include "libraries/include/parson.h"
#include <cJSON.h>
#include <msgpack.h>
/*#include <flatbuffers_common_builder.h>
#include <flatbuffers_common_reader.h>
#include <flatbuffers_tree_verifier.h>
#include <flatbuffers_tree_builder.h>
#include <flatbuffers_tree_reader.h>*/

#include "tree.h"

mxml_node_t* mxml_serialize_node(Node* node);
Node* mxml_deserialize_node(mxml_node_t* xml);

//void libfyaml_serialize_node(struct fy_document *doc, struct fy_node *parent, Node* node);

json_t* jansson_serialize_node(Node* node);
Node* jansson_deserialize_node(json_t* json);

PARSON_Value* parson_serialize_node(Node* node);
Node* parson_deserialize_node(PARSON_Value* json);

cJSON* cjson_serialize_node(Node* node);
Node* cjson_deserialize_node(cJSON* json);

void msgpack_serialize_node(msgpack_packer* pk, Node* node); 
Node* msgpack_deserialize_node(msgpack_object* msgpack);

//Tree_Node_ref_t flatcc_serialize_node(flatbuffers_builder_t *B, Node *node);
//Node *flatcc_deserialize_node(Tree_Node_table_t node_tbl);
