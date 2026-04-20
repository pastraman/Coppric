#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function: accepts any token type that can legally be used as a name.
// TOK_NAME is the obvious case, but keywords like `in`, `out`, `parts`, `end`,
// and `wire` can also appear in name positions (e.g. a port named "out", or a
// signal called "in"). This lets the parser accept those without a giant
// if/else-if chain at every name-reading site.
// If the token matches one of the allowed types, advances pos and returns the
// token. Otherwise prints an error with the line number and kills the program.
Token expectName(Token *tokens, int *pos) {
  if (tokens[*pos].type == TOK_NAME || tokens[*pos].type == TOK_OUT ||
      tokens[*pos].type == TOK_IN || tokens[*pos].type == TOK_PARTS ||
      tokens[*pos].type == TOK_END || tokens[*pos].type == TOK_WIRE) {

    // Verilog reserved words — can't use these as names even though they're
    // valid in Coppric, because the output .v file would be invalid Verilog.
    const char *verilogKeywords[] = {
        // ── Verilog 1995 ──
        "always", "and", "assign", "begin", "buf", "bufif0", "bufif1", "case",
        "casex", "casez", "cmos", "deassign", "default", "defparam", "disable",
        "edge", "else", "end", "endcase", "endfunction", "endmodule",
        "endprimitive", "endspecify", "endtable", "endtask", "event", "for",
        "force", "forever", "fork", "function", "highz0", "highz1", "if",
        "ifnone", "initial", "inout", "input", "integer", "join", "large",
        "macromodule", "medium", "module", "nand", "negedge", "nmos", "nor",
        "not", "notif0", "notif1", "or", "output", "parameter", "pmos",
        "posedge", "primitive", "pull0", "pull1", "pulldown", "pullup", "rcmos",
        "real", "realtime", "reg", "release", "repeat", "rnmos", "rpmos",
        "rtran", "rtranif0", "rtranif1", "scalared", "small", "specify",
        "specparam", "strong0", "strong1", "supply0", "supply1", "table",
        "task", "time", "tran", "tranif0", "tranif1", "tri", "tri0", "tri1",
        "triand", "trior", "trireg", "vectored", "wait", "wand", "weak0",
        "weak1", "while", "wire", "wor", "xnor", "xor",
        // ── Verilog 2001 additions ──
        "automatic", "cell", "config", "design", "endconfig", "endgenerate",
        "generate", "genvar", "incdir", "include", "instance", "liblist",
        "library", "localparam", "noshowcancelled", "pulsestyle_ondetect",
        "pulsestyle_onevent", "showcancelled", "signed", "unsigned", "use",
        // ── Verilog 2005 additions ──
        "uwire",
        // ── SystemVerilog additions (common ones) ──
        "alias", "always_comb", "always_ff", "always_latch", "assert", "assume",
        "before", "bind", "bins", "binsof", "bit", "break", "byte", "chandle",
        "class", "clocking", "const", "constraint", "context", "continue",
        "cover", "covergroup", "coverpoint", "cross", "dist", "do", "endclass",
        "endclocking", "endgroup", "endinterface", "endpackage", "endprogram",
        "endproperty", "endsequence", "enum", "expect", "export", "extends",
        "extern", "final", "first_match", "foreach", "forkjoin", "iff",
        "ignore_bins", "illegal_bins", "import", "inside", "int", "interface",
        "intersect", "join_any", "join_none", "local", "logic", "longint",
        "matches", "modport", "new", "null", "package", "packed", "priority",
        "program", "property", "protected", "pure", "rand", "randc", "randcase",
        "randsequence", "ref", "return", "sequence", "shortint", "shortreal",
        "solve", "static", "string", "struct", "super", "tagged", "this",
        "throughout", "timeprecision", "timeunit", "type", "typedef", "union",
        "unique", "var", "virtual", "void", "wait_order", "wildcard", "with",
        "within"};
    int keywordCount = sizeof(verilogKeywords) / sizeof(verilogKeywords[0]);

    for (int i = 0; i < keywordCount; i++) {
      if (strcmp(tokens[*pos].value, verilogKeywords[i]) == 0) {
        fprintf(stderr,
                "error on line %d: '%s' is a reserved word in Verilog and "
                "cannot be used as a name\n",
                tokens[*pos].line, tokens[*pos].value);
        exit(1);
      }
    }

    (*pos)++;
    return tokens[(*pos) - 1];
  }

  fprintf(stderr, "error on line %d, keyword used incorrectly",
          tokens[*pos].line);
  exit(1);
}
// Helper function: checks that the current token matches the expected type.
// If it does, advances pos and returns the token (so the caller can read its
// value). If it doesn't, prints an error with the line number and kills the
// program. This is called everywhere the parser wants to consume a specific
// token.
Token expect(TokenType expected, Token *tokens, int *pos) {
  // Check if the current token is the one we wanted.
  if (expected != tokens[*pos].type) {
    // Mismatch — print what we expected, what we got, and which line.
    fprintf(stderr, "error on line %d: expected %d, got %d\n",
            tokens[*pos].line, expected, tokens[*pos].type);
    // Parser doesn't try to recover — just kill the program.
    exit(1);
  }

  // Match! Advance the position counter past this token.
  (*pos)++;

  // Return the token we just consumed so the caller can read its value.
  return tokens[(*pos) - 1];
}

// Helper function: reads a signal name with optional bus slicing.
// Accepts plain names like "x", single-bit slices like "o[15]", and
// ranges like "o[0:7]". Returns a Token where .value contains the full
// string including brackets, so codegen can pass it straight to Verilog.
Token expectSignal(Token *tokens, int *pos) {
  // Start with the base name (port name, wire name, or internal signal).
  Token nameToken = expectName(tokens, pos);

  // If there's no opening bracket, it's just a plain name — return as-is.
  if (tokens[*pos].type != TOK_LBRACKET) {
    return nameToken;
  }

  // There IS a bracket — build the sliced version.
  // Append the opening bracket to the name.
  strcat(nameToken.value, "[");
  expect(TOK_LBRACKET, tokens, pos);

  // Read the first number (either the single bit index or the range start).
  // Capture but DON'T strcat yet — we might need to reorder for Verilog syntax.
  Token numberToken = expect(TOK_NUMBER, tokens, pos);

  // Optional colon + second number for range slicing.
  // Nand2Tetris uses [LSB:MSB] like [0:7], Verilog uses [MSB:LSB] like [7:0].
  // So when a colon exists, we swap the order.
  if (tokens[*pos].type == TOK_COLON) {
    expect(TOK_COLON, tokens, pos);

    // Second number — the range end in Nand2Tetris, but the MSB for Verilog.
    Token numberToken2 = expect(TOK_NUMBER, tokens, pos);

    // Write second number first, then colon, then first number.
    // [0:7] in Coppric becomes [7:0] in Verilog.
    strcat(nameToken.value, numberToken2.value);
    strcat(nameToken.value, ":");
    strcat(nameToken.value, numberToken.value);
  } else {
    // No colon — single bit index, no swap needed.
    strcat(nameToken.value, numberToken.value);
  }

  // Close the bracket in both the string and the token stream.
  strcat(nameToken.value, "]");
  expect(TOK_RBRACKET, tokens, pos);

  return nameToken;
}

// Main parser entry point.
// Takes the token array from the lexer and the total token count.
// Walks through the tokens left-to-right, builds a filled-in chip struct,
// and returns it. Codegen then reads that struct to emit Verilog.
chip parse(Token *tokens, int token_count) {
  // The chip struct we're building up and returning at the end.
  chip result;

  // Reused for each port we read — gets overwritten every loop iteration.
  port currentPort;

  // Current position in the token array, starts at the first token.
  int pos = 0;

  // ===== Step 1: chip header =====
  // Read the `chip` keyword.
  expect(TOK_CHIP, tokens, &pos);

  // Read the chip's name (like "ALU") — captured so we can grab its value.
  Token nameToken = expectName(tokens, &pos);

  // Copy the name string into the result struct.
  strcpy(result.name, nameToken.value);

  // Initialize all the array counts to 0 — C doesn't zero-init local structs,
  // so without this they'd hold garbage values and our arrays would be broken.
  result.iportcount = 0;
  result.oportcount = 0;
  result.gatecount = 0;
  result.wirecount = 0;

  // ===== Step 2: input ports =====
  // Allocate space for up to 64 input ports (should be plenty for any chip).
  result.input = malloc(64 * sizeof(port));

  // Read the `in` keyword that starts the input section.
  expect(TOK_IN, tokens, &pos);

  // Loop reading ports one at a time until we hit the terminating semicolon.
  while (tokens[pos].type != TOK_SEMICOLON) {
    // Read the port name — expectName lets keywords like "out" be valid names.
    // Note: port DECLARATIONS use width brackets like x[16], which is
    // different from bus slicing. That's why we still use expectName here
    // and handle the bracket manually below for width parsing.
    Token current = expectName(tokens, &pos);
    strcpy(currentPort.name, current.value);

    // Optional width: `[16]` means 16-bit wide. No brackets means 1 bit.
    if (tokens[pos].type == TOK_LBRACKET) {
      // Opening bracket, then the number, then closing bracket.
      expect(TOK_LBRACKET, tokens, &pos);
      Token widthToken = expect(TOK_NUMBER, tokens, &pos);
      // atoi converts the string "16" to the int 16.
      currentPort.width = atoi(widthToken.value);
      expect(TOK_RBRACKET, tokens, &pos);
    }

    else {
      // No bracket — default width is 1 bit (a single wire).
      currentPort.width = 1;
    }

    // Store the filled-in port into the array and bump the count.
    result.input[result.iportcount] = currentPort;
    result.iportcount++;

    // If there's a comma, skip past it — more ports are coming.
    // If no comma, the while loop will exit on the semicolon.
    if (tokens[pos].type == TOK_COMMA) {
      pos++;
    }
  }

  // Skip past the semicolon that ended the input list.
  pos++;

  // ===== Step 3: output ports =====
  // Exact same logic as inputs, just using output/oportcount instead.
  result.output = malloc(64 * sizeof(port));
  expect(TOK_OUT, tokens, &pos);

  while (tokens[pos].type != TOK_SEMICOLON) {
    // Port name — expectName so keywords-as-names work here too.
    Token current = expectName(tokens, &pos);
    strcpy(currentPort.name, current.value);

    if (tokens[pos].type == TOK_LBRACKET) {
      expect(TOK_LBRACKET, tokens, &pos);
      Token widthToken = expect(TOK_NUMBER, tokens, &pos);
      currentPort.width = atoi(widthToken.value);
      expect(TOK_RBRACKET, tokens, &pos);
    }

    else {
      currentPort.width = 1;
    }

    result.output[result.oportcount] = currentPort;
    result.oportcount++;

    if (tokens[pos].type == TOK_COMMA) {
      pos++;
    }
  }

  // Skip past the semicolon that ended the output list.
  pos++;

  // ===== Step 4: parts section (gates and wires) =====
  // Read `parts:` — that's two tokens: the keyword and the colon.
  expect(TOK_PARTS, tokens, &pos);
  expect(TOK_COLON, tokens, &pos);

  // Reused for each gate we read.
  gate currentGate;

  // Allocate space for gates and wires.
  result.gates = malloc(64 * sizeof(gate));
  result.wires = malloc(64 * sizeof(wire));

  // Loop reading gate/wire statements until we hit `end`.
  while (tokens[pos].type != TOK_END) {

    // ----- Wire branch -----
    // If the line starts with `wire`, handle the whole wire statement here
    // then `continue` back to the top so the gate code below doesn't run.
    if (tokens[pos].type == TOK_WIRE) {
      expect(TOK_WIRE, tokens, &pos);

      // A wire has a name and a source signal.
      wire wirename;

      // Wire name — expectSignal so "wire ng[15] = ..." style slicing works.
      Token currentToken = expectSignal(tokens, &pos);
      strcpy(wirename.name, currentToken.value);

      // The `=` between name and source.
      expect(TOK_EQUALS, tokens, &pos);

      // Source signal — expectSignal so "= o[15]" or "= o[0:7]" works.
      currentToken = expectSignal(tokens, &pos);
      strcpy(wirename.source, currentToken.value);

      // Closing semicolon.
      expect(TOK_SEMICOLON, tokens, &pos);

      // Store and increment.
      result.wires[result.wirecount] = wirename;
      result.wirecount++;

      // Skip back to top — don't fall into gate parsing.
      continue;
    }

    // ----- Gate branch -----
    // Read the gate name (like "Mux", "Not", "Add"). Gate names can't be
    // keywords, but expectName is fine — it only rejects non-name tokens.
    Token gateNameToken = expectName(tokens, &pos);

    // Reset params for this gate — each gate starts fresh with 0 params.
    currentGate.paramcount = 0;
    currentGate.parameters = malloc(64 * sizeof(param));

    // Reused for each param we read inside this gate.
    param currentParam;

    // Copy the gate name into the struct.
    strcpy(currentGate.name, gateNameToken.value);

    // Opening paren of the gate's param list.
    expect(TOK_LPAREN, tokens, &pos);

    // Inner loop reads params one at a time until we hit `)`.
    while (tokens[pos].type != TOK_RPAREN) {

      // --- Param key ---
      // expectSignal so you can slice on the key side too: in[8:10]=something
      Token keyToken = expectSignal(tokens, &pos);
      strcpy(currentParam.key, keyToken.value);

      // Equals sign between key and value.
      expect(TOK_EQUALS, tokens, &pos);

      // --- Param value ---
      // Can be a name (most common), number, true, false, or underscore.
      // Underscore is the "discard" symbol for unused outputs.
      // Still need the if/else chain here because numbers and constants
      // aren't name-like.
      if (tokens[pos].type == TOK_NUMBER) {
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

      else {
        // Default case — a signal name with optional slicing.
        // expectSignal handles both "=x" and "=o[0:7]".
        Token valToken = expectSignal(tokens, &pos);
        strcpy(currentParam.value, valToken.value);
      }

      // Store the completed param in this gate's param array.
      currentGate.parameters[currentGate.paramcount] = currentParam;
      currentGate.paramcount++;

      // If there's a comma, more params are coming.
      // If not, the inner while will exit on the closing paren.
      if (tokens[pos].type == TOK_COMMA) {
        expect(TOK_COMMA, tokens, &pos);
      }
    }

    // End of this gate — close paren, then the terminating semicolon.
    expect(TOK_RPAREN, tokens, &pos);
    expect(TOK_SEMICOLON, tokens, &pos);

    // Store the completed gate in the result's gates array.
    result.gates[result.gatecount] = currentGate;
    result.gatecount++;
  }

  // ===== Step 5: closing =====
  // Skip past the first `end` (which closes the parts section).
  pos++;

  // Expect a second `end` (which closes the whole chip).
  expect(TOK_END, tokens, &pos);

  // Return the fully populated chip to main, which hands it to codegen.
  return result;
}
