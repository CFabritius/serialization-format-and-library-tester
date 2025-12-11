#include "library_interface.h"



mxml_node_t* mxml_serialize_node(Node* node) {
    if(!node) return NULL;

    mxml_node_t* xml_node = mxmlNewElement(NULL, "node");

    mxmlNewInteger(mxmlNewElement(xml_node, "id"   ), node->id   );
    mxmlNewInteger(mxmlNewElement(xml_node, "value"), node->value);

    mxml_node_t* xml_children = mxmlNewElement(xml_node, "children");

    if(node->children) {
        for(int i = 0; i < BREADTH; i++) {
            mxml_node_t* xml_child = mxml_serialize_node(node->children[i]);

            if(xml_child) mxmlAdd(xml_children, MXML_ADD_AFTER, NULL, xml_child);
        }
    }

    return xml_node;
}

Node* mxml_deserialize_node(mxml_node_t* xml) {
    if(!xml) return NULL;

    Node* node = calloc(1, sizeof(Node));

    mxml_node_t* id_tag = mxmlFindElement(xml, xml, "id", NULL, NULL, MXML_DESCEND_FIRST);
    node->id = atoi(mxmlGetText(id_tag, 0));

    mxml_node_t* value_tag = mxmlFindElement(xml, xml, "value", NULL, NULL, MXML_DESCEND_FIRST);
    node->value = atoi(mxmlGetText(value_tag, 0));

    mxml_node_t* children_tag = mxmlFindElement(xml, xml, "children", NULL, NULL, MXML_DESCEND_FIRST);
    mxml_node_t* child_tag = mxmlGetFirstChild(children_tag);

    if(child_tag) {
        node->children = calloc(BREADTH, sizeof(Node*));
        for(int i = 0; i < BREADTH; i++) {
            node->children[i] = mxml_deserialize_node(child_tag);
            child_tag = mxmlGetNextSibling(child_tag);
        }
    }

    return node;
}




/*void libfyaml_serialize_node(struct fy_document *doc, struct fy_node *parent, Node* node) {
    if (!node) return;

    struct fy_node *map = fy_document_create_map(doc);
    fy_node_map_add_scalar(map, "id", node->id);
    fy_node_map_add_scalar(map, "value", node->value);

    struct fy_node *children_array = fy_document_create_seq(doc);
    if (node->children) {
        for (int i = 0; i < BREADTH; i++) {
            libfyaml_serialize_node(doc, children_array, node->children[i]);
        }
    }
    fy_node_map_add_node(map, "children", children_array);

    if (parent == NULL) {
        fy_document_set_root(doc, map);
    } else {
        fy_node_seq_add_node(parent, map);
    }
}*/





json_t* jansson_serialize_node(Node* node) {
    if(!node) return NULL;

    json_t* json_children;
    if(!node->children) {
        json_children = json_null();
    } else {
        json_children = json_array();

        for(int i = 0; i < BREADTH; i++) {
            json_t* json_child = jansson_serialize_node(node->children[i]);

            json_array_append_new(json_children, json_child);
        }
    }

    json_t* json_node = json_pack(
        "{s:i, s:i, s:o}",
        "id",    node->id,
        "value", node->value,
        "children", json_children
    );

    return json_node;
}

Node* jansson_deserialize_node(json_t* json) {
    if(!json || !json_is_object(json)) return NULL;

    Node* node = calloc(1, sizeof(Node));
    json_t* json_children = NULL;

    json_unpack(json,
        "{s:i, s:i, s:o?}",
        "id",    &node->id,
        "value", &node->value,
        "children", &json_children
    );

    if(json_children && json_is_array(json_children)) {
        node->children = calloc(BREADTH, sizeof(Node*));

        for(size_t i = 0; i < BREADTH; i++) {
            json_t* child_json = json_array_get(json_children, i);
            node->children[i] = jansson_deserialize_node(child_json);
        }
    }

    return node;
}





PARSON_Value* parson_serialize_node(Node* node) {
    if(!node) return NULL;

    PARSON_Value* node_val = parson_value_init_object();
    PARSON_Object* node_obj = parson_value_get_object(node_val);

    parson_object_set_number(node_obj,    "id", node->id   );
    parson_object_set_number(node_obj, "value", node->value);

    PARSON_Value* children_val = parson_value_init_null();

    if(node->children) {
        children_val = parson_value_init_array();
        PARSON_Array* children_arr = parson_value_get_array(children_val);

        for(int i = 0; i < BREADTH; i++) {
            PARSON_Value* child_val = parson_serialize_node(node->children[i]);
            parson_array_append_value(children_arr, child_val);
        }
    }

    parson_object_set_value(node_obj, "children", children_val);

    return node_val;
}

Node* parson_deserialize_node(PARSON_Value* json) {
    if(!json) return NULL;

    PARSON_Object* obj = parson_value_get_object(json);

    Node* node = calloc(1, sizeof(Node));

    node->id    = (int)parson_object_get_number(obj,    "id");
    node->value = (int)parson_object_get_number(obj, "value");

    PARSON_Value* children_val = parson_object_get_value(obj, "children");

    if(parson_value_get_type(children_val) == PARSONNull) {
        node->children = NULL;
    } else if(parson_value_get_type(children_val) == PARSONArray) {
        PARSON_Array* arr = parson_array(children_val);

        node->children = calloc(BREADTH, sizeof(Node*));

        for(int i = 0; i < BREADTH; i++) {
            PARSON_Value* child_val = parson_array_get_value(arr, i);
            node->children[i] = parson_deserialize_node(child_val);
        }
    }

    return node;
}





cJSON* cjson_serialize_node(Node* node) {
    if(!node) return NULL;

    cJSON* node_obj = cJSON_CreateObject();

    cJSON_AddNumberToObject(node_obj,    "id", node->id   );
    cJSON_AddNumberToObject(node_obj, "value", node->value);

    if(!node->children) {
        cJSON_AddNullToObject(node_obj, "children");
    } else {
        cJSON* children_arr = cJSON_CreateArray();

        for(int i = 0; i < BREADTH; i++) {
            cJSON* child_obj = cjson_serialize_node(node->children[i]);
            if(child_obj) cJSON_AddItemToArray(children_arr, child_obj);
        }

        cJSON_AddItemToObject(node_obj, "children", children_arr);
    }

    return node_obj;
}

Node* cjson_deserialize_node(cJSON* json) {
    if(!json) return NULL;

    Node* node = calloc(1, sizeof(Node));

    node->id    = cJSON_GetObjectItem(json,    "id")->valueint;
    node->value = cJSON_GetObjectItem(json, "value")->valueint;

    cJSON* children = cJSON_GetObjectItem(json, "children");

    if(cJSON_IsArray(children)) {
        node->children = calloc(BREADTH, sizeof(Node*));
        for (int i = 0; i < BREADTH; i++) {
            cJSON* child_obj = cJSON_GetArrayItem(children, i);
            node->children[i] = cjson_deserialize_node(child_obj);
        }
    } else {
        node->children = NULL;
    }

    return node;
}





void msgpack_serialize_node(msgpack_packer* pk, Node* node) {
    if(!node) return;

    msgpack_pack_map(pk, 3);

    msgpack_pack_str(pk, 2);
    msgpack_pack_str_body(pk, "id", 2);
    msgpack_pack_int(pk, node->id);

    msgpack_pack_str(pk, 5);
    msgpack_pack_str_body(pk, "value", 5);
    msgpack_pack_int(pk, node->value);

    msgpack_pack_str(pk, 8);
    msgpack_pack_str_body(pk, "children", 8);

    if(!node->children) {
        msgpack_pack_nil(pk);
    } else {
        msgpack_pack_array(pk, BREADTH);
        for(int i = 0; i < BREADTH; i++) {
            msgpack_serialize_node(pk, node->children[i]);
        }
    }
}

Node* msgpack_deserialize_node(msgpack_object* msgpack) {
    if(!msgpack || msgpack->type == MSGPACK_OBJECT_NIL) return NULL;

    Node* node = malloc(sizeof(Node));

    const msgpack_object_map* map = &msgpack->via.map;
    const msgpack_object* id_obj = NULL, *val_obj = NULL, *children_obj = NULL;

    for(int i = 0; i < map->size; i++) {
        const msgpack_object_kv* kv = &map->ptr[i];

        const msgpack_object_str* ks = &kv->key.via.str;

        if(strncmp(ks->ptr, "id", ks->size) == 0) {
            id_obj = &kv->val;
        } else if(strncmp(ks->ptr, "value", ks->size) == 0) {
            val_obj = &kv->val;
        } else if(strncmp(ks->ptr, "children", ks->size) == 0) {
            children_obj = &kv->val;
        }
    }

    node->id    = (int) id_obj->via.u64;
    node->value = (int)val_obj->via.u64;

    if(!children_obj || children_obj->type == MSGPACK_OBJECT_NIL) {
        node->children = NULL;
    } else {
        const msgpack_object_array* children_arr = &children_obj->via.array;

        node->children = malloc(sizeof(Node*) * BREADTH);

        for(int i = 0; i < BREADTH; i++) {
            node->children[i] = msgpack_deserialize_node(&children_arr->ptr[i]);
        }
    }

    return node;
}

/*Tree_Node_ref_t flatcc_serialize_node(flatbuffers_builder_t *B, Node *node) {
    if(!node) return 0;

    Tree_Node_start(B);

    Tree_Node_id_add(B, node->id);
    Tree_Node_value_add(B, node->value);

    Tree_Node_children_start(B);

    if(node->children) {
        for(int i = 0; i < BREADTH; i++) {
            flatcc_serialize_node(B, node->children[i]);
        }
    }

    Tree_Node_children_end(B);

    return Tree_Node_end(B);
}*/

/*Node *flatcc_deserialize_node(Tree_Node_table_t node_tbl) {
    if(!node_tbl) return NULL;

    Node *n = calloc(1, sizeof(Node));
    n->id = Tree_Node_id(node_tbl);
    n->value = Tree_Node_value(node_tbl);
    if(Tree_Node_children_is_present(node_tbl)) {
        n->children = calloc(BREADTH, sizeof(Node*));

        for(int i = 0; i < BREADTH; i++) {
            Tree_Node_table_t child_tbl = (Tree_Node_table_t)Tree_Node_children(node_tbl);

            n->children[i] = flatcc_deserialize_node(child_tbl);
        }
    } else {
        n->children = NULL;
    }

    return n;
}*/
