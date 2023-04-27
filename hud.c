#include <stdio.h>
#include "parser/parser.h"

int main() {
    Parser *parser = parse();
    destroy_parser(parser);
    return 0;
}