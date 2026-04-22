module Or (
    a,
    b,
    out
);
  output wire out;
  input wire a;
  input wire b;
  wire Nota;
  wire Notb;
  Not Not_0 (
      .in (a),
      .out(Nota)
  );
  Not Not_1 (
      .in (a),
      .out(Notb)
  );
  Nand Nand_2 (
      .a  (Nota),
      .b  (Notb),
      .out(out)
  );
endmodule
