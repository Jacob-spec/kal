#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.h"

typedef enum AST_Node_Type {
    AST_PLUS,
    AST_MINUS,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_NUMBER,
}AST_Node_Type;

typedef struct AST AST;

struct AST {
    AST_Node_Type node_type;
    union {
        struct AST_NUMBER { int number; } AST_NUMBER;
        struct AST_PLUS { AST *left; AST *right; } AST_PLUS;
        struct AST_MINUS { AST *left; AST *right; } AST_MINUS;
        struct AST_MULTIPLY { AST *left; AST *right; } AST_MULTIPLY;
        struct AST_DIVIDE { AST *left; AST *right; } AST_DIVIDE;
    }data;
};

typedef struct Parser {
    Token *tokens;
    AST *root;
    int number_of_tokens;
    int current_token_index;
    int next_token_index;
}Parser;

Parser *create_parser(Lexer *lexer);
void destroy_parser(Parser *parser);

void advance_parser(Parser *parser);

Token current_token(Parser *parser);
Token next_token(Parser *parser);

AST *create_ast(AST ast);
void print_ast(AST *ptr);
void destroy_ast(AST *ptr);
AST * parse_expression(Parser *parser);
AST * parse_factor(Parser *parser);
Parser* parse();

#endif