module top (
  pi0, pi1,
  po0
);
  input   pi0, pi1;
  output  po0;

  wire n0, n1, n2;

  and(n0, pi1, pi0);
  not(n1, pi0);
  and(n2, pi1, n1);
  or (po0, n0, n2);
endmodule
