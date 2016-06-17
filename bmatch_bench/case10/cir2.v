//
// Conformal-LEC Version 16.10-d005 ( 21-Apr-2016 ) ( 64 bit executable )
//
module cir1 ( n0 , n1 , n2 , n3 , n4 , n5 );
input n0 , n1 , n2 , n3 ;
output n4 , n5 ;

wire n12 , n13 , n14 , n15 , n16 , n17 , n18 , n19 , n20 , 
     n21 , n22 , n23 , n24 , n25 , n26 , n27 ;
buf ( n4 , n20 );
buf ( n5 , n27 );
buf ( n14 , n0 );
buf ( n15 , n1 );
buf ( n16 , n2 );
buf ( n17 , n3 );
and ( n18 , n14 , n15 );
buf ( n19 , n18 );
buf ( n20 , n19 );
and ( n21 , n16 , n14 );
not ( n22 , n14 );
or ( n23 , n22 , n17 );
and ( n24 , n15 , n23 );
or ( n25 , n21 , n24 );
buf ( n26 , n25 );
buf ( n27 , n26 );
endmodule

