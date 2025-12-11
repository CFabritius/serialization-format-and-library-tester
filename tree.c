#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "tree.h"

static int global_id = 0;
Node* generate_tree(int depth) {
    if(depth <= 0) return NULL;

    Node* node = malloc(sizeof(Node));
    node->id = global_id++;
    node->value = rand();
    
    if(depth == 1) {
        node->children = NULL;
    } else {
        node->children = malloc(sizeof(Node*) * BREADTH);

        for(int i = 0; i < BREADTH; i++) {
            node->children[i] = generate_tree(depth-1);
        }
    }

    return node;
}

int count_nodes(Node *tree) {
    if(!tree) {
        return 0;
    } else if(tree->children == NULL) {
        return 1;
    } else {
        int count = 1;
        for(int i = 0; i < BREADTH; i++) {
            count += count_nodes(tree->children[i]);
        }
        return count;
    }
}

char* bool_word(bool b) {
    return b ? "" : " not";
}

bool compare_tree(Node *a, Node *b) {
    if(!a && !b) return true;
    if((!a && b) || (a && !b)) {
        printf("CMP: a is%s null, while b is%s!\n", bool_word(a == NULL), bool_word(b == NULL));
        return false;
    }

    if(a->id != b->id) {
        printf("CMP: id fields are different (%d & %d)!\n", a->id, b->id);
        return false;
    }
    if(a->value != b-> value) {
        printf("CMP: value fields are different (%d & %d)!\n", a->value, b->value);
        return false;
    }

    if(!a->children && !b->children) return true;
    if((!a->children && b->children) || (a->children && !b->children)) {
        printf(
            "CMP: subtree a is%s null, while subtree b is%s!\n",
            bool_word(a->children == NULL),
            bool_word(b->children == NULL)
        );
        return false;
    }


    for(int i = 0; i < BREADTH; i++) {
        if(!compare_tree(a->children[i], b->children[i])) return false;
    }

    return true;
}
