#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
typedef struct {
  int width;
  char name[256];
} port;

typedef struct {
  char key[256];
  char value[256];
} param;

typedef struct {
  int paramcount;
  param *parameters;
  char name[256];
} gate;

typedef struct {
  char source[256];
  char name[256];
} wire;

typedef struct {
  char name[256];
  port *input;
  int iportcount;
  port *output;
  int oportcount;
  gate *gates;
  int gatecount;
  wire *wires;
  int wirecount;
} chip;

chip parse(Token *tokens, int token_count);
#endif
