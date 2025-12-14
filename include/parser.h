#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

void init_parser(const char *source);
ASTNode* parse();

#endif
