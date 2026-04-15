#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// tokent = tokentype
Token expect(TokenType expected, Token *tokens, int *pos) {
  if (expected != tokens[*pos].type) {
    fprintf(stderr, "error on line %d: expected %d, got %d\n",
            tokens[*pos].line, expected, tokens[*pos].type);
    exit(1);
  }
  (*pos)++;
  return tokens[(*pos) - 1];
}

chip parse(Token *tokens, int token_count) {
  chip result;
  port currentPort;
  int pos = 0;
  expect(TOK_CHIP, tokens, &pos);
  Token nameToken = expect(TOK_NAME, tokens, &pos);
  strcpy(result.name, nameToken.value);

  result.iportcount = 0;
  result.oportcount = 0;
  result.gatecount = 0;
  result.wirecount = 0;

  // input loop
  result.input = malloc(64 * sizeof(port));
  expect(TOK_IN, tokens, &pos);
  while (tokens[pos].type != TOK_SEMICOLON) {
    Token current = expect(TOK_NAME, tokens, &pos);
    strcpy(currentPort.name, current.value);
    if (tokens[pos].type == TOK_LBRACKET) {
      expect(TOK_LBRACKET, tokens, &pos);
      Token widthToken = expect(TOK_NUMBER, tokens, &pos);
      currentPort.width = atoi(widthToken.value);
      expect(TOK_RBRACKET, tokens, &pos);
    } else {
      currentPort.width = 1;
    }
    result.input[result.iportcount] = currentPort;
    result.iportcount++;
    if (tokens[pos].type == TOK_COMMA) {
      pos++;
    }
  }
  pos++;

  // outut loop
  result.output = malloc(64 * sizeof(port));
  expect(TOK_OUT, tokens, &pos);
  while (tokens[pos].type != TOK_SEMICOLON) {
    Token current = expect(TOK_NAME, tokens, &pos);
    strcpy(currentPort.name, current.value);
    if (tokens[pos].type == TOK_LBRACKET) {
      expect(TOK_LBRACKET, tokens, &pos);
      Token widthToken = expect(TOK_NUMBER, tokens, &pos);
      currentPort.width = atoi(widthToken.value);
      expect(TOK_RBRACKET, tokens, &pos);
    } else {
      currentPort.width = 1;
    }
    result.output[result.oportcount] = currentPort;
    result.oportcount++;
    if (tokens[pos].type == TOK_COMMA) {
      pos++;
    }
  }
  pos++;

  // gate loop
  expect(TOK_PARTS, tokens, &pos);
  expect(TOK_COLON, tokens, &pos);
  gate currentGate;
  result.gates = malloc(64 * sizeof(gate));
  result.wires = malloc(64 * sizeof(wire));
  while (tokens[pos].type != TOK_END) {
    if (tokens[pos].type == TOK_WIRE) {
      expect(TOK_WIRE, tokens, &pos);
      wire wirename;
      Token currentToken = expect(TOK_NAME, tokens, &pos);
      strcpy(wirename.name, currentToken.value);
      expect(TOK_EQUALS, tokens, &pos);
      currentToken = expect(TOK_NAME, tokens, &pos);
      strcpy(wirename.source, currentToken.value);
      expect(TOK_SEMICOLON, tokens, &pos);
      result.wires[result.wirecount] = wirename;
      result.wirecount++;
      continue;
    }
    Token gateNameToken = expect(TOK_NAME, tokens, &pos);
    currentGate.paramcount = 0;
    currentGate.parameters = malloc(64 * sizeof(param));
    param currentParam;
    strcpy(currentGate.name, gateNameToken.value);

    expect(TOK_LPAREN, tokens, &pos);
    while (tokens[pos].type != TOK_RPAREN) {

      if (tokens[pos].type == TOK_NAME) {
        Token keyToken = expect(TOK_NAME, tokens, &pos);
        strcpy(currentParam.key, keyToken.value);
      }

      else if (tokens[pos].type == TOK_IN) {
        Token keyToken = expect(TOK_IN, tokens, &pos);
        strcpy(currentParam.key, keyToken.value);
      }

      else if (tokens[pos].type == TOK_OUT) {
        Token keyToken = expect(TOK_OUT, tokens, &pos);
        strcpy(currentParam.key, keyToken.value);
      }
      expect(TOK_EQUALS, tokens, &pos);
      if (tokens[pos].type == TOK_NAME) {
        Token valToken = expect(TOK_NAME, tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }

      else if (tokens[pos].type == TOK_NUMBER) {
        Token valToken = expect(TOK_NUMBER, tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }

      else if (tokens[pos].type == TOK_TRUE) {
        Token valToken = expect(TOK_TRUE, tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }

      else if (tokens[pos].type == TOK_FALSE) {
        Token valToken = expect(TOK_FALSE, tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }

      else if (tokens[pos].type == TOK_UNDERSCORE) {
        Token valToken = expect(TOK_UNDERSCORE, tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }
      currentGate.parameters[currentGate.paramcount] = currentParam;
      currentGate.paramcount++;
      if (tokens[pos].type == TOK_COMMA) {
        expect(TOK_COMMA, tokens, &pos);
      }
    }
    expect(TOK_RPAREN, tokens, &pos);
    expect(TOK_SEMICOLON, tokens, &pos);
    result.gates[result.gatecount] = currentGate;
    result.gatecount++;
  }
  pos++;
  expect(TOK_END, tokens, &pos);
  return result;
}
