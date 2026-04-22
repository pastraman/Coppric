module Not (
    in,
    out
);
  output wire out;
  input wire in;
  Nand Nand_0 (
      .a  (in),
      .b  (in),
      .out(out)
  );
endmodule
