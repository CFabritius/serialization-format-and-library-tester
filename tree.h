#ifndef DEPTH
#define DEPTH 2
#endif

#ifndef BREADTH
#define BREADTH 2
#endif

typedef struct Node {
    int id;
    int value;
    struct Node** children;
} Node;

Node* generate_tree(int depth);

int count_nodes(Node *tree);

char* bool_word(bool b);

bool compare_tree(Node *a, Node *b);
