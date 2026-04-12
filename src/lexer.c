#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *tokenize(const char *source, int *token_count) {
  int pos = 0;
  int count = 0;
  int line = 1;
  int capacity = 64;
  Token *tokens = malloc(capacity * sizeof(Token));

  while (source[pos] != '\0') {
    // grow array if full
    if (count >= capacity) {
      capacity *= 2;
      tokens = realloc(tokens, capacity * sizeof(Token));
    }

    // skip whitespace
    if (source[pos] == ' ' || source[pos] == '\t' || source[pos] == '\r') {
      pos++;
      continue;
    }

    // newlines — track line number
    else if (source[pos] == '\n') {
      line++;
      pos++;
      continue;
    }

    // comments: # skip to end of line
    else if (source[pos] == '#') {
      while (source[pos] != '\0' && source[pos] != '\n') {
        pos++;
      }
      continue;
    }

    // single character tokens
    else if (source[pos] == '(') {
      tokens[count].type = TOK_LPAREN;
      tokens[count].value[0] = '(';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == ')') {
      tokens[count].type = TOK_RPAREN;
      tokens[count].value[0] = ')';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == '[') {
      tokens[count].type = TOK_LBRACKET;
      tokens[count].value[0] = '[';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == ']') {
      tokens[count].type = TOK_RBRACKET;
      tokens[count].value[0] = ']';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == '=') {
      tokens[count].type = TOK_EQUALS;
      tokens[count].value[0] = '=';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == ',') {
      tokens[count].type = TOK_COMMA;
      tokens[count].value[0] = ',';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == ';') {
      tokens[count].type = TOK_SEMICOLON;
      tokens[count].value[0] = ';';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == ':') {
      tokens[count].type = TOK_COLON;
      tokens[count].value[0] = ':';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    } else if (source[pos] == '_') {
      tokens[count].type = TOK_UNDERSCORE;
      tokens[count].value[0] = '_';
      tokens[count].value[1] = '\0';
      tokens[count].line = line;
      count++;
      pos++;
    }

    // numbers
    else if (isdigit(source[pos])) {
      int start = pos;
      while (isdigit(source[pos])) {
        pos++;
      }
      int len = pos - start;
      strncpy(tokens[count].value, &source[start], len);
      tokens[count].value[len] = '\0';
      tokens[count].type = TOK_NUMBER;
      tokens[count].line = line;
      count++;
    }

    // names and keywords
    else if (isalpha(source[pos]) || source[pos] == '_') {
      int start = pos;
      while (isalpha(source[pos]) || isdigit(source[pos]) ||
             source[pos] == '_') {
        pos++;
      }
      int len = pos - start;
      strncpy(tokens[count].value, &source[start], len);
      tokens[count].value[len] = '\0';
      tokens[count].line = line;

      // keyword check
      if (strcmp(tokens[count].value, "chip") == 0)
        tokens[count].type = TOK_CHIP;
      else if (strcmp(tokens[count].value, "in") == 0)
        tokens[count].type = TOK_IN;
      else if (strcmp(tokens[count].value, "out") == 0)
        tokens[count].type = TOK_OUT;
      else if (strcmp(tokens[count].value, "parts") == 0)
        tokens[count].type = TOK_PARTS;
      else if (strcmp(tokens[count].value, "end") == 0)
        tokens[count].type = TOK_END;
      else if (strcmp(tokens[count].value, "wire") == 0)
        tokens[count].type = TOK_WIRE;
      else if (strcmp(tokens[count].value, "true") == 0)
        tokens[count].type = TOK_TRUE;
      else if (strcmp(tokens[count].value, "false") == 0)
        tokens[count].type = TOK_FALSE;
      else
        tokens[count].type = TOK_NAME;

      count++;
    }

    // unknown character
    else {
      fprintf(stderr, "lexer error: unexpected '%c' on line %d\n", source[pos],
              line);
      pos++;
    }
  }

  // emit end of file (EFO)
  if (count >= capacity) {
    capacity++;
    tokens = realloc(tokens, capacity * sizeof(Token));
  }
  tokens[count].type = TOK_EOF;
  tokens[count].value[0] = '\0';
  tokens[count].line = line;
  count++;

  *token_count = count;
  return tokens;
}
