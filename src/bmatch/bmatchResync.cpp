/**CppFile**************************************************************
 
  FileName    [resync.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Resync function]

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

void Bmatch_Resync( Abc_Ntk_t * &pNtk );
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Resync function]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_Resync( Abc_Ntk_t * &pNtk )
{
    Abc_Ntk_t * pNtkTemp;

    // balance
    pNtkTemp = Abc_NtkBalance( pNtk, 0, 0, 1 );
    Abc_NtkDelete( pNtk );
    pNtk = pNtkTemp;

    // rewrite
    Abc_NtkRewrite( pNtk, 1, 0, 0, 0, 0 );

    // rewrite -z
    Abc_NtkRewrite( pNtk, 1, 1, 0, 0, 0 );

    // balance
    pNtkTemp = Abc_NtkBalance( pNtk, 0, 0, 1 );
    Abc_NtkDelete( pNtk );
    pNtk = pNtkTemp;

    // rewrite -z
    Abc_NtkRewrite( pNtk, 1, 1, 0, 0, 0 );

    // balance
    pNtkTemp = Abc_NtkBalance( pNtk, 0, 0, 1 );
    Abc_NtkDelete( pNtk );
    pNtk = pNtkTemp;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
