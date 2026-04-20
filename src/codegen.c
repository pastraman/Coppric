#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void generate(const char *filename, chip c) {
  FILE *f = fopen(filename, "w");
  int i;
  int j;
  char *outName = NULL;
  int width = 1;
  fprintf(f, "module %s (", c.name);
  int first = 1;
  for (i = 0; i < c.iportcount; i++) {
    if (!first) {
      fprintf(f, ", ");
    }
    fprintf(f, "%s", c.input[i].name);
    first = 0;
  }

  for (i = 0; i < c.oportcount; i++) {
    if (!first) {
      fprintf(f, ", ");
    }
    fprintf(f, "%s", c.output[i].name);
    first = 0;
  }

  fprintf(f, ");\n");

  for (i = 0; i < c.oportcount; i++) {
    if (c.output[i].width > 1) {
      fprintf(f, "  output wire [%d:0] %s;\n", c.output[i].width - 1,
              c.output[i].name);
    }

    else {
      fprintf(f, "  output wire %s;\n", c.output[i].name);
    }
  }

  for (i = 0; i < c.iportcount; i++) {
    if (c.input[i].width > 1) {
      fprintf(f, "  input wire [%d:0] %s;\n", c.input[i].width - 1,
              c.input[i].name);
    }

    else {
      fprintf(f, "  input wire %s;\n", c.input[i].name);
    }
  }

  for (i = 0; i < c.gatecount; i++) {
    bool isOutput = false;
    outName = NULL;
    width = 1;
    for (j = 0; j < c.gates[i].paramcount; j++) {
      if (strcmp(c.gates[i].parameters[j].key, "out") == 0) {
        outName = c.gates[i].parameters[j].value;
      }
      if (strcmp(c.gates[i].parameters[j].key, "WIDTH") == 0) {
        width = atoi(c.gates[i].parameters[j].value);
      }
    }
    if (outName == NULL || strcmp(outName, "_") == 0) {
      continue;
    }
    for (int k = 0; k < c.oportcount; k++) {
      if (strcmp(c.output[k].name, outName) == 0) {
        isOutput = true;
      }
    }

    if (!isOutput) {
      if (width > 1) {
        fprintf(f, "  wire [%d:0] %s;\n", width - 1, outName);
      } else {
        fprintf(f, "  wire %s;\n", outName);
      }
    }
  }

  for (i = 0; i < c.gatecount; i++) {
    width = 1;
    for (j = 0; j < c.gates[i].paramcount; j++) {
      if (strcmp(c.gates[i].parameters[j].key, "WIDTH") == 0) {
        width = atoi(c.gates[i].parameters[j].value);
      }
    }
    fprintf(f, "  %s", c.gates[i].name);
    if (width > 1) {
      fprintf(f, " #(.WIDTH(%d))", width);
    }
    fprintf(f, " %s_%d (", c.gates[i].name, i);
    first = 1;

    for (j = 0; j < c.gates[i].paramcount; j++) {
      if (strcmp(c.gates[i].parameters[j].key, "WIDTH") == 0) {
        continue;
      }

      if (!first) {
        fprintf(f, ", ");
      }

      if (strcmp(c.gates[i].parameters[j].value, "_") == 0) {
        fprintf(f, ".%s()", c.gates[i].parameters[j].key);
      }

      else if (strcmp(c.gates[i].parameters[j].value, "true") == 0) {
        fprintf(f, ".%s(1'b1)", c.gates[i].parameters[j].key);
      }

      else if (strcmp(c.gates[i].parameters[j].value, "false") == 0) {
        fprintf(f, ".%s(1'b0)", c.gates[i].parameters[j].key);
      }

      else {
        fprintf(f, ".%s(%s)", c.gates[i].parameters[j].key,
                c.gates[i].parameters[j].value);
      }
      first = 0;
    }
    fprintf(f, ");\n");
  }

  for (i = 0; i < c.wirecount; i++) {
    fprintf(f, "  assign %s = %s;\n", c.wires[i].name, c.wires[i].source);
  }

  fprintf(f, "endmodule\n");

  fclose(f);
}
