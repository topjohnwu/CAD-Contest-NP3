module top( a, b, c, g, f, h );
input a, b, c;
output g, f, h;
and ( f, a, b );
or ( h, a, c );
xor ( g, f, h );
endmodule
