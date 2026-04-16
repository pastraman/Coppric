#Coppric


Coppric is an HDL transpiler (.cpr → Verilog) I created because I hate Verilog — how it looks, and i hate its syntax.
The syntax is inspired by Nand2Tetris: clean, readable, and (almost) logic gates only.


Philosophy:

1: Anything can be made from transistors and wires, so that's all you get.
2: You will understand what you build and build what you understand.
3: Want a feature? Make it yourself. Want a multiply gate? Build it.


example Nand2Tetris ALU in Coppric:

chip ALU
  in x[16], y[16], zx, nx, zy, ny, f, no;
  out out[16], zr, ng;

  parts:
    Mux(WIDTH=16, a=x, b=false, sel=zx, out=zdx);

    Not(WIDTH=16, in=zdx, out=notzdx);
    Mux(WIDTH=16, a=zdx, b=notzdx, sel=nx, out=ndx);

    Mux(WIDTH=16, a=y, b=false, sel=zy, out=zdy);

    Not(WIDTH=16, in=zdy, out=notzdy);
    Mux(WIDTH=16, a=zdy, b=notzdy, sel=ny, out=ndy);

    Add(WIDTH=16, a=ndx, b=ndy, out=added);
    And(WIDTH=16, a=ndx, b=ndy, out=anded);
    Mux(WIDTH=16, a=anded, b=added, sel=f, out=fout);

    Not(WIDTH=16, in=fout, out=notfout);
    Mux(WIDTH=16, a=fout, b=notfout, sel=no, out=o);

    wire out = o;

    wire ng = o[15];

    Or8Way(in=o[0:7], out=or1);
    Or8Way(in=o[8:15], out=or2);
    Or(a=or1, b=or2, out=notzr);
    Not(in=notzr, out=zr);
  end
end


Status:

Lexer: done. Parser: done. codegen: done.
testing: needed

Build:

gcc src/*.c -o Coppric


License:
AGPL v3
