/**CppFile**************************************************************
 
  FileName    [bmatchQbf.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [QBF Solving]

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

Abc_Ntk_t * Bmatch_PrepareQbfNtk( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );

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

Abc_Ntk_t * Bmatch_PrepareQbfNtk( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 )
{
    Abc_Ntk_t * pTest;
    pTest = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );

    char * pName = "Test"; // the name comes from the user’s application
    pTest->pName = Extra_UtilStrsav( pName );

    int nPrimaryInputs = 3; // this number comes from the user’s application
    Abc_Obj_t * pObj, * pObjA, * pObjB, * pObjC, * pPO;
    int i; 

    for ( i = 0; i < nPrimaryInputs; i++ )
        pObj = Abc_NtkCreatePi( pTest ); 

    pObjA = Abc_NtkPi( pTest, 0 );
    pObjB = Abc_NtkPi( pTest, 1 );
    pObjC = Abc_NtkPi( pTest, 2 );
    pPO   = Abc_NtkCreatePo( pTest );

    pObj = Abc_AigMux( (Abc_Aig_t * )pTest->pManFunc, pObjA, pObjB, pObjC );
    Abc_ObjAddFanin( pPO, pObj );

    Abc_AigCleanup( (Abc_Aig_t *)pTest->pManFunc );
    Abc_NtkAddDummyPiNames( pTest );
    Abc_NtkAddDummyPoNames( pTest ); 
    Abc_NtkAddDummyBoxNames( pTest );

    if ( !Abc_NtkCheck( pTest ) ) {
        printf( "The AIG construction has failed.\n" );     
        Abc_NtkDelete( pTest );     
        return NULL; 
    }

    return pTest;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
