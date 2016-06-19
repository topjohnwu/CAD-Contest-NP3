/**CppFile**************************************************************
 
  FileName    [bmatch.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Command file]

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

#ifdef __cplusplus
extern "C" {
#endif

void Bmatch_Init( Abc_Frame_t * pAbc );
void Bmatch_End( Abc_Frame_t * pAbc );

static int BmatchCommandBmatch( Abc_Frame_t * pAbc, int argc, char **argv );

#ifdef __cplusplus
}
#endif

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

  Synopsis    [bmatch command]

  Description [Command for boolean matching]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int BmatchCommandBmatch( Abc_Frame_t * pAbc, int argc, char **argv )
{
    int c, fDelete1 = 0, fDelete2 = 0;
    char ** pArgvNew;
    int nArgcNew;
    Abc_Ntk_t * pNtk1, * pNtk2, * pNtkQbf;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
       switch ( c )
       {
          default:
             goto usage;
       }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if( nArgcNew != 2 )
    {
        printf("Invalid command!\n");
        goto usage;
    }
    if ( !Abc_NtkPrepareTwoNtks( stdout, NULL, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;

    Bmatch_Resync( pNtk1 );
    Bmatch_Resync( pNtk2 );

    // Bmatch_PrintNtkStats( pNtk1 );
    // Bmatch_PrintNtkStats( pNtk2 );
    Bmatch_PrintIO( pNtk1 );
    Bmatch_PrintIO( pNtk2 );

    pNtkQbf = Bmatch_PrepareQbfNtk( pNtk1, pNtk2 );
    // Bmatch_PrintNtkStats( pNtkQbf );
    Bmatch_PrintIO( pNtkQbf );
    // Abc_FrameSetCurrentNetwork( pAbc, pNtkQbf );
    Bmatch_SolveQbf( pNtkQbf, Abc_NtkPiNum( pNtk1 ), 20, 0 );

    // TODO: Functions below

    // construct_qbf( pNtk1, pNtk2 );
    // run_qbf();
    // output

    return 0;

usage:
    Abc_Print( -2, "usage: bmatch <file1> <file2> \n" );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
