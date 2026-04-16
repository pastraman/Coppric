#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  FILE *file = fopen(argv[1], "r");
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *source = malloc(size + 1);
  fread(source, 1, size, file);
  source[size] = '\0';
  fclose(file);

  int count;
  Token *tokens = tokenize(source, &count);
  chip chips = parse(tokens, count);

  char outName[256];
  strcpy(outName, argv[1]);
  char *dot = strrchr(outName, '.');
  strcpy(dot, ".v");

  generate(outName, chips);

  free(source);
  return 0;
}
