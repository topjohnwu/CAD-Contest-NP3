//
// Conformal-LEC Version 16.10-d005 ( 21-Apr-2016 ) ( 64 bit executable )
//
module cir1 ( n0 , n1 , n2 , n3 , n4 , n5 );
input n0 , n1 , n2 , n3 ;
output n4 , n5 ;

wire n12 , n13 , n14 , n15 , n16 , n17 , n18 , n19 , n20 , 
     n21 , n22 , n23 , n24 , n25 ;
buf ( n4 , n22 );
buf ( n5 , n25 );
buf ( n14 , n3 );
buf ( n15 , n0 );
buf ( n16 , n1 );
not ( n17 , n14 );
and ( n18 , n16 , n17 );
or ( n19 , n15 , n18 );
and ( n20 , n14 , n19 );
buf ( n21 , n20 );
buf ( n22 , n21 );
or ( n23 , n14 , n15 );
buf ( n24 , n23 );
buf ( n25 , n24 );
endmodule

