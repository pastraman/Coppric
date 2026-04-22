// Nand.v — primitive gate for Coppric
// This is the ONLY hand-written Verilog file.

module Nand (
    a,
    b,
    out
);
  parameter WIDTH = 1;
  input wire [WIDTH-1:0] a;
  input wire [WIDTH-1:0] b;
  output wire [WIDTH-1:0] out;
  assign out = ~(a & b);
endmodule
