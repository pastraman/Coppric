#ifndef LEXER_H
#define LEXER_H

typedef enum {
  TOK_TRUE,
  TOK_FALSE,
  TOK_COLON,
  TOK_CHIP,
  TOK_IN,
  TOK_OUT,
  TOK_PARTS,
  TOK_END,
  TOK_WIRE,
  TOK_NAME,
  TOK_NUMBER,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_LBRACKET,
  TOK_RBRACKET,
  TOK_EQUALS,
  TOK_COMMA,
  TOK_SEMICOLON,
  TOK_UNDERSCORE,
  TOK_EOF
} TokenType;

typedef struct {
  TokenType type;
  char value[256];
  int line;
} Token;

Token *tokenize(const char *source, int *token_count);

#endif
