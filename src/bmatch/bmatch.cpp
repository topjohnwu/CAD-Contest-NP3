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
    Vec_Int_t * vPiValues;
    int fVerification = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerification ^= 1;
            break;  
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

    // Need placing in the beginning to get coherence!
    printf("Before Pre! \n");
    pNtkQbf = Bmatch_PrepareQbfNtk( pNtk1, pNtk2 );
    printf("After \n");
    Bmatch_Resync( pNtkQbf );

    printf("Cir 1:\n");
    Bmatch_PrintIO( pNtk1 );
    printf("\nCir 2:\n");
    Bmatch_PrintIO( pNtk2 );
    printf("\nCir QBF:\n");
    Bmatch_PrintIO( pNtkQbf );
    printf("\n");

    if( fVerification ){
        printf("Do verification! \n");
    }
    // Abc_FrameSetCurrentNetwork( pAbc, pNtkQbf );

    vPiValues = Vec_IntStart( Abc_NtkPiNum(pNtkQbf) );

    if( Bmatch_SolveQbf( pNtkQbf, vPiValues, Abc_NtkPiNum( pNtk1 ), 50, 0 ) )
    {
        printf("\nQBF Output: ");
        Abc_NtkVectorPrintPars( vPiValues, Abc_NtkPiNum( pNtkQbf ) - Abc_NtkPiNum( pNtk1 ) );
        printf("\n");
        Bmatch_Output( pNtk1, pNtk2, pNtkQbf, vPiValues->pArray );
    }

    Vec_IntFree( vPiValues );
    return 0;

usage:
    Abc_Print( -2, "usage: bmatch <file1> <file2> \n" );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
