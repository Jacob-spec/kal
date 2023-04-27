#include "parser.h"
#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>

AST *create_ast(AST ast) {
    AST *ptr = (AST *) malloc(sizeof(AST));
    *ptr = ast;
    return ptr;
}

void destroy_ast(AST *ptr) {
    AST ast = *ptr;
    switch (ast.node_type) {
        case AST_PLUS:
            destroy_ast(ast.data.AST_PLUS.left);
            destroy_ast(ast.data.AST_PLUS.right);
            break;
        case AST_MINUS:
            destroy_ast(ast.data.AST_MINUS.left);
            destroy_ast(ast.data.AST_MINUS.right);
            break;
        case AST_MULTIPLY:
            destroy_ast(ast.data.AST_MULTIPLY.left);
            destroy_ast(ast.data.AST_MULTIPLY.right);
            break;
        case AST_DIVIDE:
            destroy_ast(ast.data.AST_DIVIDE.left);
            destroy_ast(ast.data.AST_DIVIDE.right);
            break;
        default:
            break;
    }
    free(ptr);
}

void print_ast(AST *ptr) {
    AST ast = *ptr;
    switch (ast.node_type) {
        case AST_NUMBER:
            printf("%d", ast.data.AST_NUMBER.number);
            break;
        case AST_PLUS:
            printf("(");
            print_ast(ast.data.AST_PLUS.left);
            printf(" + ");
            print_ast(ast.data.AST_PLUS.right);
            printf(")");
            break;
        case AST_MINUS:
            printf("(");
            print_ast(ast.data.AST_MINUS.left);
            printf(" - ");
            print_ast(ast.data.AST_MINUS.right);
            printf(")");
            break;
        case AST_MULTIPLY:
            printf("(");
            print_ast(ast.data.AST_MULTIPLY.left);
            printf(" * ");
            print_ast(ast.data.AST_MULTIPLY.right);
            printf(")");
            break;
        case AST_DIVIDE:
            printf("(");
            print_ast(ast.data.AST_DIVIDE.left);
            printf(" / ");
            print_ast(ast.data.AST_DIVIDE.right);
            printf(")");
            break;
    }
}

Parser * create_parser(Lexer *lexer) {
    Parser *parser = (Parser *) malloc(sizeof(Parser));
    parser->tokens = lexer->tokens;
    parser->root = NULL;
    parser->number_of_tokens = lexer->number_of_tokens;
    parser->current_token_index = 0;
    parser->next_token_index = 1;
    return parser;
}

void destroy_parser(Parser *parser) {
    destroy_ast(parser->root);
    free(parser);
}

void advance_parser(Parser *parser) {
    parser->current_token_index++;
    parser->next_token_index++;
}

Token current_token(Parser *parser) {
    if ((sizeof(Token) * parser->current_token_index) < parser->number_of_tokens) {
        return parser->tokens[sizeof(Token) * parser->current_token_index];
    } else {
        return (Token) {.type = EOF_TOKEN, .value = NULL, .line_number = 0, .column_number = 0};
    }
}

Token next_token(Parser *parser) {
    if ((sizeof(Token) * parser->next_token_index) < parser->number_of_tokens) {
        return parser->tokens[sizeof(Token) * parser->next_token_index];
    } else {
        return (Token) {.type = EOF_TOKEN, .value = NULL, .line_number = 0, .column_number = 0};
    }
}

AST * parse_factor(Parser *parser) {
    Token token = current_token(parser);

    AST *node;
    if (token.type == INT) {
        // sets every type of number to the value ten until i figure out atoi
        node = create_ast( (AST) {.node_type = AST_NUMBER, .data.AST_NUMBER = (struct AST_NUMBER) {.number = 10}} );
    } else if (token.type == L_PAREN) {
        advance_parser(parser);
        node = parse_expression(parser);
    }
    return node;
}

// parse a factor, then if the next token is a * or /, parse that and return the root node of the operation
AST * parse_term(Parser *parser) {
    AST *node = parse_factor(parser);

    if (next_token(parser).type == MULTIPLY) {
        advance_parser(parser);
        node = create_ast( (AST) {.node_type = AST_MULTIPLY, .data.AST_MULTIPLY = (struct AST_MULTIPLY) {.left = node, .right = parse_factor(parser)}} );
    } else if (next_token(parser).type == DIVIDE) {
        advance_parser(parser);
        node = create_ast( (AST) {.node_type = AST_DIVIDE, .data.AST_DIVIDE = (struct AST_DIVIDE) {.left = node, .right = parse_factor(parser)}} );
    }

    return node;
}

AST * parse_expression(Parser *parser) {
    AST *node;
    Token token = current_token(parser);
    // if the current token is a plus or minus, create a node for the operator and call parse_term() to get the rhs
    while (token.type != EOF_TOKEN) {
        if (token.type == INT) {
            node = parse_term(parser);
        } else if (token.type == PLUS) {
            // consume the plus token
            advance_parser(parser);
            node = create_ast( (AST) {.node_type = AST_PLUS, .data.AST_PLUS = (struct AST_PLUS) {.left = node, .right = parse_term(parser)}} );
        } else if (token.type == MINUS) {
            advance_parser(parser);
            node = create_ast( (AST) {.node_type = AST_MINUS, .data.AST_MINUS = (struct AST_MINUS) {.left = node, .right = parse_term(parser)}});
        }
        advance_parser(parser);
        token = current_token(parser);
    }
    return node;
}

// destroys the lexer when it no longer needs it
Parser *parse() {
    Lexer lexer = Lex();
    print_tokens(&lexer);

    Parser *parser = create_parser(&lexer);
    parser->root = parse_expression(parser);
    print_ast(parse_expression(parser));
    
    destroy_lexer(&lexer);
    return parser;
}