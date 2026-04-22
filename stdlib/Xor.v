module Xor (a, b, out);
  output wire out;
  input wire a;
  input wire b;
  wire Na;
  wire Nb;
  wire outa;
  wire outb;
  Not Not_0 (.in(a), .out(Na));
  Not Not_1 (.in(b), .out(Nb));
  And And_2 (.a(Na), .b(b), .out(outa));
  And And_3 (.a(a), .b(Nb), .out(outb));
  Or Or_4 (.a(outa), .b(outb), .out(out));
endmodule
