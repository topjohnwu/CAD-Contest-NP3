/**CppFile**************************************************************
 
  FileName    [bmatchPrintNtkStats.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Print Ntk Stats]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#include "bmatch.h"

#ifndef _WIN32
#include <unistd.h>
#endif

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

void Bmatch_PrintNtkStats       ( Abc_Ntk_t * pNtk );
void Bmatch_PrintIO             ( Abc_Ntk_t * pNtk );
void Bmatch_PrintObj            ( Abc_Ntk_t * pNtk );
void Bmatch_Print3Circuit       ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk3 );
void Bmatch_PrintMatchPairs 	( vector< pair < suppWrap *, vector< suppWrap * > > > & a );

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Bmatch_Resync function]

  Description [resync the input ntk]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_PrintNtkStats( Abc_Ntk_t * pNtk )
{
    printf("Network name :    %s \n", Abc_NtkName(pNtk));
    printf("number of PIs   = %d \n", Abc_NtkPiNum( pNtk ));
    printf("number of POs   = %d \n", Abc_NtkPoNum( pNtk ));
    printf("number of Nodes = %d \n", Abc_NtkNodeNum( pNtk ));
}

void Bmatch_PrintObj( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;
    int i;

    Abc_NtkForEachObj( pNtk, pObj, i){
        printf("Name %15s : ", Abc_ObjName( pObj ));
        Abc_ObjPrint( stdout, pObj );
    }
}

void Bmatch_PrintIO( Abc_Ntk_t * pNtk )
{
    int i;
    Abc_Obj_t * pObj;

    Abc_NtkForEachPi( pNtk, pObj, i){
        printf("PI %15s : ", Abc_ObjName( pObj) );
        Abc_ObjPrint( stdout, pObj );
    }

    Abc_NtkForEachPo( pNtk, pObj, i){
        printf("PO %15s : ", Abc_ObjName( pObj) );
        Abc_ObjPrint( stdout, pObj );
    }
    printf("Total           PI : %7d   PO : %7d \n", Abc_NtkPiNum( pNtk ), Abc_NtkPoNum( pNtk ) );
}

void Bmatch_Print3Circuit       ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk3 )
{
    printf("Total   Cir1    PI : %7d   PO : %7d \n", Abc_NtkPiNum( pNtk1 ), Abc_NtkPoNum( pNtk1 ) );
    printf("Total   Cir2    PI : %7d   PO : %7d \n", Abc_NtkPiNum( pNtk2 ), Abc_NtkPoNum( pNtk2 ) );
    printf("Total   CirQbf  PI : %7d   PO : %7d \n", Abc_NtkPiNum( pNtk3 ), Abc_NtkPoNum( pNtk3 ) );  
}


void Bmatch_PrintMatchPairs ( vector< pair < suppWrap *, vector< suppWrap * > > > & a )
{
    for( int i = 0, n = a.size(); i < n; ++i ) {
        printf("%7s is equivalent to : \n", Abc_ObjName( a[i].first->thisObj ) );
        for( int j = 0, m = a[i].second.size(); j < m; ++j ) {
        	if( a[i].second[j]->status == NEGATIVE )
            	printf( "-%8s \n", Abc_ObjName( a[i].second[j]->thisObj ));
            else
            	printf( "%9s \n", Abc_ObjName( a[i].second[j]->thisObj ));
        }
    }
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
