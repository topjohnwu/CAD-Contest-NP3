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
#include <vector>
using namespace std;

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
    int c, fDelete1 = 0, fDelete2 = 0, ILP_constraint = 1;
    bool muxOnCir2 = true;
    char ** pArgvNew;
    int nArgcNew, score = 0, maxScore = 0;
    Abc_Ntk_t * pNtk1, * pNtk2, * pNtkQbf;
    Vec_Int_t * vPiValues;
    int fVerification = 0;
    vector<Node> * inGroup = 0 , * outGroup = 0, *constGroup = 0;

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

while(1){
    printf( "ILP = %6d \n", ILP_constraint );
    // Need placing in the beginning to get coherence!
    pNtkQbf = Bmatch_PrepareQbfNtk( pNtk1, pNtk2, ILP_constraint, muxOnCir2 );
    Bmatch_Resync( pNtkQbf );

    Bmatch_Print3Circuit( pNtk1, pNtk2, pNtkQbf );

    /*if( fVerification ){
        printf("Do verification! \n");
    }*/
    // Abc_FrameSetCurrentNetwork( pAbc, pNtkQbf );

    vPiValues = Vec_IntStart( Abc_NtkPiNum(pNtkQbf) );

    if( Bmatch_SolveQbf( pNtkQbf, vPiValues, Abc_NtkPiNum( pNtk1 ), 20, 0 ) )
    {
        inGroup = new vector<Node>[ Abc_NtkPiNum( pNtk1 ) ];
        outGroup = new vector<Node>[ Abc_NtkPoNum( pNtk1 ) ];
        constGroup = new vector<Node>;

        //printf("\nQBF Output: ");
        //Abc_NtkVectorPrintPars( vPiValues, Abc_NtkPiNum( pNtkQbf ) - Abc_NtkPiNum( pNtk1 ) );
        //printf("\n");
        
        Bmatch_Parse( pNtk1, pNtk2, pNtkQbf, vPiValues->pArray, inGroup, outGroup, constGroup, true );
        score = Bmatch_Output( pNtk1, pNtk2, inGroup, outGroup, constGroup, score, "match.out");
        if( score > maxScore ) maxScore = score;
        if( fVerification ){
            printf("score : %6d \n \n", score);
        }
    }

    Vec_IntFree( vPiValues );
    if(inGroup){ delete[] inGroup; inGroup = NULL; }
    if(outGroup){ delete[] outGroup; outGroup = NULL; }
    if(constGroup){ delete constGroup; constGroup = NULL; }
    Abc_NtkDelete(pNtkQbf);

    ++ILP_constraint;
    if( muxOnCir2 && Abc_NtkPoNum( pNtk1 ) + 1 - ILP_constraint == 0) break;
    else if( Abc_NtkPoNum( pNtk2 ) + 1 - ILP_constraint == 0) break;
    printf("\n");
}    
    return 0;

usage:
    Abc_Print( -2, "usage: bmatch <file1> <file2> \n" );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
