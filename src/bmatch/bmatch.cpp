/**CppFile**************************************************************
 
  FileName    [bmatch.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Command file.]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/


#include "bmatch.h"
#include "base/main/mainInt.h"

#ifndef _WIN32
#include <unistd.h>
#endif

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int BmatchCommandBmatch          ( Abc_Frame_t * pAbc, int argc, char **argv );


////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Start / Stop the bmatch package]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/


void Bmatch_Init( Abc_Frame_t * pAbc )
{
    Cmd_CommandAdd( pAbc , "z Bmatch" , "bmatch" , BmatchCommandBmatch , 0 );
}

void Bmatch_End( Abc_Frame_t * pAbc )
{
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int BmatchCommandBmatch( Abc_Frame_t * pAbc, int argc, char **argv )
{
    return 0;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END

