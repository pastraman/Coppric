module Dmux (in, sel, a, b);
  output wire a;
  output wire b;
  input wire in;
  input wire sel;
  Mux Mux_0 (.a(in), .b(1'b0), .sel(sel), .out(a));
  Mux Mux_1 (.a(1'b0), .b(in), .sel(sel), .out(b));
endmodule
