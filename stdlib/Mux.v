module Mux (a, b, sel, out);
  output wire out;
  input wire a;
  input wire b;
  input wire sel;
  wire notsel;
  wire outb;
  wire outa;
  Not Not_0 (.in(sel), .out(notsel));
  And And_1 (.a(sel), .b(b), .out(outb));
  And And_2 (.a(a), .b(notsel), .out(outa));
  Or Or_3 (.a(outa), .b(outb), .out(out));
endmodule
