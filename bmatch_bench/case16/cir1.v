//
// Conformal-LEC Version 16.10-d160 ( 04-Jul-2016 ) ( 64 bit executable )
//
module top ( n0 , n1 , n2 , n3 , n4 , n5 , n6 , n7 , n8 , n9 , n10 , n11 , n12 , n13 , n14 , n15 );
input n0 , n1 , n2 , n3 , n4 , n5 , n6 , n7 ;
output n8 , n9 , n10 , n11 , n12 , n13 , n14 , n15 ;

wire n32 , n33 , n34 , n35 , n36 , n37 , n38 , n39 , n40 , 
     n41 , n42 , n43 , n44 , n45 , n46 , n47 , n48 , n49 , n50 , 
     n51 , n52 , n53 , n54 , n55 , n56 , n57 , n58 , n59 ;
buf ( n15 , n43 );
buf ( n10 , n46 );
buf ( n12 , n49 );
buf ( n11 , n52 );
buf ( n8 , n55 );
buf ( n14 , n58 );
buf ( n9 , n59 );
buf ( n13 , 1'b1 );
buf ( n37 , n6 );
buf ( n38 , n7 );
buf ( n39 , n3 );
buf ( n40 , n0 );
buf ( n41 , n5 );
buf ( n42 , n4 );
buf ( n43 , n37 );
xor ( n44 , n38 , n37 );
buf ( n45 , n44 );
buf ( n46 , n45 );
xor ( n47 , n39 , n38 );
buf ( n48 , n47 );
buf ( n49 , n48 );
xor ( n50 , n40 , n39 );
buf ( n51 , n50 );
buf ( n52 , n51 );
xor ( n53 , n41 , n40 );
buf ( n54 , n53 );
buf ( n55 , n54 );
xor ( n56 , n42 , n41 );
buf ( n57 , n56 );
buf ( n58 , n57 );
buf ( n59 , n42 );
endmodule

