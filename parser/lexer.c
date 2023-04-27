#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "./lexer.h"



void print_token_type(TokenType type) {
    switch (type) {
		case QUOTES:
			printf("QUOTES");
			break;
        case PERIOD:
            printf("PERIOD");
			break;
        case COMMA:
            printf("COMMA");
			break;
        case L_PAREN:
            printf("L_PAREN");
			break;
        case R_PAREN:
            printf("R_PAREN");
			break;
		case L_SQUARE_BRACKET:
			printf("L_SQUARE_BRACKET");
			break;
		case R_SQUARE_BRACKET:
			printf("R_SQUARE_BRACKET");
			break;
        case L_CURLY_BRACKET:
            printf("L_CURLY_BRACKET");
			break;
        case R_CURLY_BRACKET:
            printf("R_CURLY_BRACKET");
			break;
        case ASSIGN:
            printf("ASSIGN");
			break;
		case SEMICOLON:
			printf("SEMICOLON");
			break;
        case PLUS:
            printf("PLUS");
			break;
        case MINUS:
            printf("MINUS");
			break;
        case MULTIPLY:
            printf("MULTIPLY");
			break;
        case DIVIDE:
            printf("DIVIDE");
			break;
        case EXPONENT:
            printf("EXPONENT");
			break;
		case MODULO:
			printf("MODULO");
			break;
		case NOT:
			printf("NOT");
			break;
		case EQ:
			printf("EQ");
			break;
		case NOT_EQ:
			printf("NOT_EQ");
			break;
		case INCREMENT:
			printf("INCREMENT");
			break;
		case LT:
			printf("LT");
			break;
		case LT_EQ:
			printf("LT_EQ");
			break;
		case GT:
			printf("GT");
			break;
		case GT_EQ:
			printf("GT_EQ");
			break;
		case IDENT:
			printf("IDENT");
			break;
		case KW_FUNC:
			printf("KW_FUNC");
			break;
		case KW_VAR:
			printf("KW_VAR");
			break;
		case KW_RETURN:
			printf("KW_RETURN");
			break;
		case KW_IF:
			printf("KW_IF");
			break;
		case KW_ELSE:
			printf("KW_ELSE");
			break;
		case KW_FOR:
			printf("KW_FOR");
			break;
		case INT:
			printf("INT");
			break;
		case FLOAT:
			printf("FLOAT");
			break;
		case STRING:
			printf("STRING");
			break;
		case EOF_TOKEN:
			printf("EOF");
			break;
		default:
			printf("ERR");

    }
}

void print_token(Token *token) {
	print_token_type(token->type);

	if (token->value) {
		printf(" | %s", token->value);
	}

	printf(" | %d | %d\n", token->line_number, token->column_number);
}

void print_tokens(Lexer *lexer) {
	for (int i = 0; i < lexer->number_of_tokens; i++) {
		print_token(&lexer->tokens[i * sizeof(Token)]);
	}
}

// allocates and writes the substring of the content specified by start and end
// indices into a new buffer for the token.value
char * extract_token_value_substring(Lexer *lexer, int start, int end) {
	char *substr = (char *) malloc(sizeof(char) * (end - start));
	int j = 0;
	for (int i = start; i < end; i++) {
		substr[j] = lexer->contents[i];
		j++;
	}
	return substr;
}

// reads the contents of the file into a buffer, returns buffer and length
// why is the size of this getting up so damn high
char * get_file_contents(char *filename) { 
    FILE *file;
    file = fopen("/Users/jacobstoner/Code/C/hud/prog.hud", "r");
    if (file == NULL) {
        printf("File cannot be opened\n");
        return 0;
    }

    int buffer_size = 1024;
    int index_in_buffer = 0;
    char *buffer = (char *) malloc(buffer_size * sizeof(char));
	char current;
    while ((current = fgetc(file)) != EOF) {
        if (index_in_buffer == buffer_size) {
            buffer = (char *) realloc(buffer, (buffer_size + 1024));
        }
        buffer[index_in_buffer] = current;
        index_in_buffer++;
    }
	// null terminate the input string
	buffer[index_in_buffer + 1] = '\0';
    fclose(file);
    return buffer;
}

int find_input_length(char *file_contents) {
	int i = 0;
	while (1) {
		if (file_contents[i] == '\0') {
			break;
		}
		i++;
	}
	return i;
}

// frees the allocated memory for the file contents,
// the content of each token, and the token list
void destroy_lexer(Lexer *lexer) {
	for (int i = 0; i < lexer->number_of_tokens; i++) {
		if (lexer->tokens[sizeof(Token) * i].value) {
			free(lexer->tokens[sizeof(Token) * i].value);
		}
	}
	free(lexer->tokens);
	free(lexer->contents);
}

// returns the next character in the input stream or a null character and a 1
char peek(Lexer *lexer) {
	if (lexer->next_index < lexer->length_of_contents) {
		return lexer->contents[lexer->next_index];
	}

	return '\0';
}

// returns the current character in the input stream
char current(Lexer *lexer) {
	return lexer->contents[lexer->current_index];
}

// advance the current and next index of the lexer
void advance(Lexer *lexer) {
	lexer->column_number++;
	lexer->current_index++;
	lexer->next_index++;
}

// creates the token then appends it to the list in the lexer, and advances the lexer
void create_token(Lexer *lexer, TokenType type, char *value) {
	Token token;
	token.type = type;
	token.line_number = lexer->line_number;
	token.column_number = lexer->column_number;
	token.value = value;
	// allocates more memory for the tokens pointer of the lexer if needed
	if ((lexer->number_of_tokens * sizeof(Token)) >= lexer->token_buffer_length) {
		lexer->tokens = (Token *) realloc(lexer->tokens, sizeof(Token) * 1024);
	}
	lexer->tokens[lexer->number_of_tokens * sizeof(Token)] = token;
	lexer->number_of_tokens++;
}

// lexes all the one character tokens on its own, passes over spaces, 
// increments the newline count of the lexer,  and calls
// lex_long_token if it isn't a 1 character reserved token
Lexer lex_all(Lexer lexer) {
	while (lexer.current_index < lexer.length_of_contents) {
		switch (current(&lexer)) {
			case '"':
				create_token(&lexer, QUOTES, NULL);
				advance(&lexer);
				lex_string(&lexer);
				break;
			case '.':
				create_token(&lexer, PERIOD, NULL);
				advance(&lexer);
				break;
			case ',':
				create_token(&lexer, COMMA, NULL);
				advance(&lexer);
				break;
			case '(':
				create_token(&lexer, L_PAREN, NULL);
				advance(&lexer);
				break;
			case ')':
				create_token(&lexer, R_PAREN, NULL);
				advance(&lexer);
				break;
			case '[':
				create_token(&lexer, L_SQUARE_BRACKET, NULL);
				advance(&lexer);
				break;
			case ']':
				create_token(&lexer, R_SQUARE_BRACKET, NULL);
				advance(&lexer);
				break;
			case '{':
				create_token(&lexer, L_CURLY_BRACKET, NULL);
				advance(&lexer);
				break;
			case '}':
				create_token(&lexer, R_CURLY_BRACKET, NULL);
				advance(&lexer);
				break;
			case '=':
				lex_equals_sign(&lexer);
				break;
			case ';':
				create_token(&lexer, SEMICOLON, NULL);
				advance(&lexer);
				break;
			case '+':
				lex_plus(&lexer);
				break;
			case '-':
				create_token(&lexer, MINUS, NULL);
				advance(&lexer);
				break;
			case '*':
				create_token(&lexer, MULTIPLY, NULL);
				advance(&lexer);
				break;
			case '/':
				create_token(&lexer, DIVIDE, NULL);
				advance(&lexer);
				break;
			case '^':
				create_token(&lexer, EXPONENT, NULL);
				advance(&lexer);
				break;
			case '%':
				create_token(&lexer, MODULO, NULL);
				advance(&lexer);
				break;
			case '!':
				lex_exclamation_point(&lexer);
				break;
			case '<':
				lex_less_than(&lexer);
				break;
			case '>':
				lex_greater_than(&lexer);
				break;
			case ' ':
				advance(&lexer);
				break;
			case '\n':
				lexer.line_number++;
				lexer.column_number = 1;
				// can't use advance because it'll mess with
				// the line and column numbering for the tokens
				lexer.current_index++;
				lexer.next_index++;
				break;
			case '\t':
				advance(&lexer);
				break;
			default:
				lex_long_token(&lexer);
		}
	}
	// adds an eof token at the end
	lexer.tokens[lexer.number_of_tokens * sizeof(Token)] = (Token) {.type = EOF_TOKEN, .value = NULL, 0, 0};
	lexer.number_of_tokens++;
	return lexer;
}

// creates a lexer and initializes it with default values
// -- currently just reads from the default file I have in hud/
Lexer Lex() {
	// int *buffer_length;
	char *buffer = get_file_contents("/home/jacob/Code/C/hud/test.hud");
	Lexer lexer;
	lexer.contents = buffer;
	lexer.current_index = 0;
	lexer.next_index = 1;
	lexer.line_number = 1;
	lexer.length_of_contents = find_input_length(buffer);
	lexer.number_of_tokens = 0;
	lexer.token_buffer_length = sizeof(Token) * 1024;
	lexer.tokens = (Token *) malloc(sizeof(Token) * 1024);

	lexer = lex_all(lexer);
	return lexer;
}

void lex_plus(Lexer *lexer) {
	if (peek(lexer) == '=') {
		create_token(lexer, INCREMENT, NULL);
		advance(lexer);
		advance(lexer);
	} else {
		create_token(lexer, PLUS, NULL);
		advance(lexer);
	}
}

// peeks the next character to see if the equals sign is an assignment or an equality
void lex_equals_sign(Lexer *lexer) {
	if (peek(lexer) == '=') {
		create_token(lexer, EQ, NULL);
		advance(lexer);
		advance(lexer);
	} else {
		create_token(lexer, ASSIGN, NULL);
		advance(lexer);
	}
}

void lex_exclamation_point(Lexer *lexer) {
	if (peek(lexer) == '=') {
		create_token(lexer, NOT_EQ, NULL);
		advance(lexer);
		advance(lexer);
	} else {
		create_token(lexer, NOT, NULL);
		advance(lexer);
	}
}
// peeks to see if it's less than or less than/equals
void lex_less_than(Lexer *lexer) {
	if (peek(lexer) == '=') {
		create_token(lexer, LT_EQ, NULL);
		advance(lexer);
		advance(lexer);
	} else {
		create_token(lexer, LT, NULL);
		advance(lexer);
	}
}

// peeks to see if it's less than or less than/equals
void lex_greater_than(Lexer *lexer) {
	if (peek(lexer) == '=') {
		create_token(lexer, GT_EQ, NULL);
		advance(lexer);
		advance(lexer);
	} else {
		create_token(lexer, GT, NULL);
		advance(lexer);
	}
}

// lexes everything other than the one and two character tokens
void lex_long_token(Lexer *lexer) {	
	char c = current(lexer);
	if (isalpha(c) || c == '_') {
		lex_keyword(lexer);
	} else if (isdigit(c)) {
		lex_number(lexer);
	}
}

// lexes a possible keyword, and if it doesn't match one,
// makes the token type an identifier
void lex_keyword(Lexer *lexer) {
	int start = lexer->current_index;
	int end;
	// consumes opening character of the identifier
	advance(lexer);
	// find where the kw/identifier ends
	for (int i = start; i < lexer->length_of_contents; i++) {
		char c = current(lexer);
		if (c == ' ') {
			end = lexer->current_index;
			break;
		} else if (isalpha(c) == 0 && c != '_') {
			end = lexer->current_index;
			break;
		} else {
			advance(lexer);
		}
	}

	char *token_value = extract_token_value_substring(lexer, start, end);
	if (check_for_keywords(lexer, token_value) != 0) {
		create_token(lexer, IDENT, token_value);
	}

}

// checks an identifier value against the reserved keywords
// and if they match, it creates a keyword token
int check_for_keywords(Lexer *lexer, char *token_value) {
	if(strcmp(token_value, "func") == 0) {
		create_token(lexer, KW_FUNC, token_value);
		return 0;
	} else if (strcmp(token_value, "var") == 0) {
		create_token(lexer, KW_VAR, token_value);
		return 0;
	} else if (strcmp(token_value, "return") == 0) {
		create_token(lexer, KW_RETURN, token_value);
		return 0;
	} else if (strcmp(token_value, "if") == 0) {
		create_token(lexer, KW_IF, token_value);
		return 0;
	} else if (strcmp(token_value, "else") == 0) {
		create_token(lexer, KW_ELSE, token_value);
		return 0;
	} else if (strcmp(token_value, "for") == 0) {
		create_token(lexer, KW_FOR, token_value);
		return 0;
	}
	return 1;
}

// lexes a number
void lex_number(Lexer *lexer) {
	int start = lexer->current_index;
	int end;
	// finds where the number ends
	for (int i = start; i < lexer->length_of_contents; i++) {
		char c = current(lexer);
		if (c == '.') {
			// consume the period
			advance(lexer);
			lex_float(lexer, start);
			break;
		} else if (!isdigit(c)) {
			end = lexer->current_index;
			char *token_value = extract_token_value_substring(lexer, start, end);
			create_token(lexer, INT, token_value);
			break;
		} else {
			advance(lexer);
		}
	}

	
}

void lex_float(Lexer *lexer, int start) {
	int end;
	for (int i = start; i < lexer->length_of_contents; i++) {
		char c = current(lexer);
		if (!isdigit(c)) {
			end = lexer->current_index;
			break;		
		} else {
			advance(lexer);
		}
	}

	char *token_value = extract_token_value_substring(lexer, start, end);
	create_token(lexer, FLOAT, token_value);
}

void lex_string(Lexer *lexer) {
	int start = lexer->current_index;
	int end;
	for (int i = start; i < lexer->length_of_contents; i++) {
		char c = current(lexer);
		if (c == '"') {
			end = lexer->current_index;
			break;
		} else {
			advance(lexer);
		}
	}

	int j = 0;
	char *token_value = (char *) malloc(sizeof(char) * (end - start));
	for (int i = start; i < end; i++) {
		token_value[j] = lexer->contents[i];
		j++;
	}

	create_token(lexer, STRING, token_value);
	advance(lexer);
	// consumes the closing quotes so that it lex_all doesn't try to parse
	// everything after the closing quotes as another string
	create_token(lexer, QUOTES, NULL);
}