#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Node_Kind {  // Define various kinds of AST nodes.
        NODE_EXPRESSION,
        NODE_IDENTIFIER,
        NODE_CONSTANT,
        NODE_PROCEDURE_CALL,
        NODE_ASSIGNMENT,
};

enum Binary_Operation {  // Represent different binary operations.
        BINARY_OP_ADD,
        BINARY_OP_SUBTRACT,
        BINARY_OP_MULTIPLY,
        BINARY_OP_DIVIDE,
};

struct node {  // Define AST node structure.
        enum Node_Kind kind;
        union {
                struct {
                        enum Binary_Operation operation;
                        struct node          *left;
                        struct node          *right;
                } binary_expr;

                struct {
                        char *name;
                } identifier;

                struct {
                        int value;
                } constant;

                struct {
                        char        *procedure_name;
                        struct node *arguments;  // List of argument nodes.
                } procedure_call;

                struct {
                        struct node *left;   // Variable to assign.
                        struct node *right;  // Value to assign.
                } assignment;
        };
};

void ast_node_free(struct node *node) {
        if (node == NULL) return;

        switch (node->kind) {
        case NODE_EXPRESSION:
                ast_node_free(node->binary_expr.left);
                ast_node_free(node->binary_expr.right);
                break;
        case NODE_IDENTIFIER: free(node->identifier.name); break;
        case NODE_CONSTANT: break;
        case NODE_PROCEDURE_CALL:
                free(node->procedure_call.procedure_name);
                ast_node_free(node->procedure_call.arguments);
                break;
        case NODE_ASSIGNMENT:
                ast_node_free(node->assignment.left);
                ast_node_free(node->assignment.right);
                break;
        default: break;
        }
        free(node);
}

// constant == int_lit?
// This isn't idetifier though..
struct node *node_init_constant(int value) {
        struct node *node = (struct node *)malloc(sizeof(struct node));
        if (node == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
        }
        node->kind           = NODE_CONSTANT;
        node->constant.value = value;
        return node;
}

struct node *node_init_identifier(const char *name) {
        struct node *node = (struct node *)malloc(sizeof(struct node));
        if (node == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
        }
        node->kind            = NODE_IDENTIFIER;
        node->identifier.name = strdup(name);
        if (node->identifier.name == NULL) {
                perror("String duplication failed");
                free(node);
                exit(EXIT_FAILURE);
        }
        return node;
}

struct node *node_init_assignment(struct node *left, struct node *right) {
        struct node *node = (struct node *)malloc(sizeof(struct node));
        if (node == NULL) {
                perror("Memory allocation failed");
                ast_node_free(left);
                ast_node_free(right);
                exit(EXIT_FAILURE);
        }
        node->kind             = NODE_ASSIGNMENT;
        node->assignment.left  = left;
        node->assignment.right = right;
        return node;
}

struct node *node_init_binary_expr(enum Binary_Operation operation,
                                   struct node *left, struct node *right) {
        struct node *node = (struct node *)malloc(sizeof(struct node));
        if (node == NULL) {
                perror("Memory allocation failed");
                ast_node_free(left);
                ast_node_free(right);
                exit(EXIT_FAILURE);
        }
        node->kind                  = NODE_EXPRESSION;
        node->binary_expr.operation = operation;
        node->binary_expr.left      = left;
        node->binary_expr.right     = right;
        return node;
}

struct node *node_init_procedure_call(char *proc_name, struct node *arguments) {
        struct node *node = (struct node *)malloc(sizeof(struct node));
        if (node == NULL) {
                perror("Memory allocation failed");
                ast_node_free(arguments);
                exit(EXIT_FAILURE);
        }
        node->kind                          = NODE_PROCEDURE_CALL;
        node->procedure_call.procedure_name = strdup(proc_name);
        if (node->procedure_call.procedure_name == NULL) {
                perror("String duplication failed");
                free(node);
                ast_node_free(arguments);
                exit(EXIT_FAILURE);
        }
        node->procedure_call.arguments = arguments;
        return node;
}

void ast_print_nodes(struct node *node) {
        if (node == NULL) return;

        switch (node->kind) {
        case NODE_EXPRESSION:
                printf("(");
                ast_print_nodes(node->binary_expr.left);
                switch (node->binary_expr.operation) {
                case BINARY_OP_ADD: printf(" + "); break;
                case BINARY_OP_SUBTRACT: printf(" - "); break;
                case BINARY_OP_MULTIPLY: printf(" * "); break;
                case BINARY_OP_DIVIDE: printf(" / "); break;
                }
                ast_print_nodes(node->binary_expr.right);
                printf(")");
                break;
        case NODE_IDENTIFIER: printf("%s", node->identifier.name); break;
        case NODE_CONSTANT: printf("%d", node->constant.value); break;
        case NODE_PROCEDURE_CALL:
                printf("%s(", node->procedure_call.procedure_name);
                ast_print_nodes(node->procedure_call.arguments);
                printf(")");
                break;
        case NODE_ASSIGNMENT:
                ast_print_nodes(node->assignment.left);
                printf(" = ");
                ast_print_nodes(node->assignment.right);
                break;
        }
}
int main(int argc, char *argv[]) {
        printf("$ syntaxflowc\n");

        if (1) {  // Handle creating simple AST primitives
                struct node *expression =
                    node_init_binary_expr(BINARY_OP_ADD, node_init_constant(4),
                                          node_init_constant(3));
                struct node *variable = node_init_identifier("my_var_x");
                struct node *assignment =
                    node_init_assignment(variable, expression);

                printf("Assignment AST: ");
                ast_print_nodes(assignment);
                printf("\n");

                ast_node_free(assignment);
        }

        if (1) {  // Handle creating procedure call AST
                struct node *proc_args =
                    node_init_binary_expr(BINARY_OP_ADD, node_init_constant(1),
                                          node_init_constant(5));
                struct node *proc_call =
                    node_init_procedure_call("my_procedure", proc_args);

                printf("Procedure Call AST: ");
                ast_print_nodes(proc_call);
                printf("\n");

                ast_node_free(proc_call);
        }

        return 0;
};
