//
// Conformal-LEC Version 16.10-d160 ( 04-Jul-2016 ) ( 64 bit executable )
//
module top ( n0 , n1 , n2 , n3 , n4 , n5 , n6 , n7 , n8 , n9 , n10 , n11 , n12 , n13 , n14 , n15 );
input n0 , n1 , n2 , n3 , n4 , n5 , n6 , n7 ;
output n8 , n9 , n10 , n11 , n12 , n13 , n14 , n15 ;

wire n32 , n33 , n34 , n35 , n36 , n37 , n38 , n39 , n40 , 
     n41 , n42 , n43 , n44 , n45 , n46 , n47 , n48 , n49 , n50 , 
     n51 , n52 , n53 , n54 , n55 , n56 , n57 , n58 , n59 , n60 , 
     n61 , n62 , n63 ;
buf ( n13 , n42 );
buf ( n12 , n45 );
buf ( n10 , n48 );
buf ( n8 , n51 );
buf ( n9 , n54 );
buf ( n14 , n57 );
buf ( n15 , n60 );
buf ( n11 , n63 );
buf ( n34 , n6 );
buf ( n35 , n1 );
buf ( n36 , n2 );
buf ( n37 , n0 );
buf ( n38 , n7 );
buf ( n39 , n4 );
buf ( n40 , n3 );
buf ( n41 , n5 );
buf ( n42 , n34 );
xor ( n43 , n35 , n34 );
buf ( n44 , n43 );
buf ( n45 , n44 );
xor ( n46 , n36 , n35 );
buf ( n47 , n46 );
buf ( n48 , n47 );
xor ( n49 , n37 , n36 );
buf ( n50 , n49 );
buf ( n51 , n50 );
xor ( n52 , n38 , n37 );
buf ( n53 , n52 );
buf ( n54 , n53 );
xor ( n55 , n39 , n38 );
buf ( n56 , n55 );
buf ( n57 , n56 );
xor ( n58 , n40 , n39 );
buf ( n59 , n58 );
buf ( n60 , n59 );
xor ( n61 , n41 , n40 );
buf ( n62 , n61 );
buf ( n63 , n62 );
endmodule

