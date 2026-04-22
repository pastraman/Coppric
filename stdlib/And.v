module And (
    a,
    b,
    out
);
  output wire out;
  input wire a;
  input wire b;
  wire Notout;
  Nand Nand_0 (
      .a  (a),
      .b  (b),
      .out(Notout)
  );
  Not Not_1 (
      .in (Notout),
      .out(out)
  );
endmodule
