#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

typedef enum TokenType {
    // one or two character semantic tokens
    QUOTES,
    PERIOD,
    COMMA,
    L_PAREN,
    R_PAREN,
    L_SQUARE_BRACKET,
    R_SQUARE_BRACKET,
    L_CURLY_BRACKET,
    R_CURLY_BRACKET,
    ASSIGN,
    SEMICOLON,
    // mathematical operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    EXPONENT,
    MODULO,
    INCREMENT,
    NOT,
    // comparison operators
    EQ,
    NOT_EQ,
    LT,
    LT_EQ,
    GT,
    GT_EQ,
    // keywords and identifiers
    IDENT,
    KW_FUNC,
    KW_VAR,
    KW_RETURN,
    KW_IF,
    KW_ELSE,
    KW_FOR,
	// types
	INT,
    FLOAT,
    STRING,
    EOF_TOKEN,
}TokenType;

typedef struct Token {
    TokenType type;
    char *value;
    int line_number;
    int column_number;
}Token;


typedef struct Lexer {
	char *contents;
	int current_index;
	int next_index;
	int line_number;
    int column_number;
	int length_of_contents;
	int number_of_tokens;
	int token_buffer_length;
	Token * tokens;
}Lexer;

char * get_file_contents(char * filename);
char * extract_token_value_substring(Lexer *lexer, int start, int end);

int find_input_length(char *file_contents);

void destroy_lexer(Lexer *lexer);

Lexer lex_all(Lexer lexer);

Lexer Lex();
// djb2 hash function
// unsigned long hash(unsigned char *str, int length) {
//     unsigned long hash = 5381;
//     int c;
//     for (int i = 0; i < length; i++) {
// 		c = *str[i];
//         hash = ((hash << 5) + hash) + c;
//     }
//     return hash;
// }

// implements djb2 hash for keyword lookup
// enum Keyword {
//     Func = hash("func", 4),
//     Var = hash("var", 3),
//     Return = hash("return", 6),
//     If = hash("if", 2),
//     Else = hash("else", 4),
//     For = hash("for", 3),
// }Keyword;


void print_token_type(TokenType type);

void print_token(Token *token);

void print_tokens(Lexer *lexer);

void lex_plus(Lexer *lexer);

void lex_exclamation_point(Lexer *lexer);

void lex_equals_sign(Lexer *lexer);

void lex_less_than(Lexer *lexer);

void lex_greater_than(Lexer *lexer);

void lex_long_token(Lexer *lexer);

void lex_keyword(Lexer *lexer);

int check_for_keywords(Lexer *lexer, char *token_value);

void lex_number(Lexer *lexer);
void lex_float(Lexer *lexer, int start);

void lex_string(Lexer *lexer);
#endif