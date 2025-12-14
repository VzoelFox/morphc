#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"

void init_evaluator();
void evaluate(ASTNode *node);
void cleanup_evaluator();

#endif
