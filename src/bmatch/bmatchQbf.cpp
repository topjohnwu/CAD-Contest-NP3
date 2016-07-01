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
#include <vector>
#include <string>
#include <iostream>

using namespace std;

#ifndef _WIN32
#include <unistd.h>
#endif

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////
static int fConst0 = 0;
static vector< Abc_Obj_t * > Output_Pool, Control_Pool;

#ifdef __cplusplus
extern "C" {
#endif

Abc_Ntk_t * Bmatch_PrepareQbfNtk        ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, const int & ILP_constraint, const bool & muxOnCir2 );
void        Bmatch_PrepareNtk1          ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk_Qbf );          
void        Bmatch_CreatePIMUXes        ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf );
void        Bmatch_CreatePOMUXes        ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf );
void        Bmatch_PrepareFinal         ( Abc_Ntk_t * pNtk_Qbf, int ILP_constraint );
void        Bmatch_Construct_MUXes      ( vector< Abc_Obj_t * > & , Abc_Obj_t *& , Abc_Ntk_t *& , int & , int & = fConst0 );
Abc_Obj_t * Bmatch_Construct_ILP        ( vector< Abc_Obj_t * > & , Abc_Ntk_t *& , const int & k );
char *      Bmatch_NameAddPrefix        ( char *& pPrefix, int plength, char * pName );

bool        Bmatch_SolveQbf             ( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nInputs, int nItersMax, int fVerbose );

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

Abc_Ntk_t * Bmatch_PrepareQbfNtk( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, const int & ILP_constraint, const bool & muxOnCir2 )
{
    Abc_Ntk_t * pNtk_Qbf;
    pNtk_Qbf = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    int forILP;

    char * pName = "AIG_for_Qbf"; // the name comes from the user’s application
    pNtk_Qbf->pName = Extra_UtilStrsav( pName );

    Abc_NtkOrderObjsByName( pNtk1, 0 );
    Abc_NtkOrderObjsByName( pNtk2, 0 );
    //printf("= \n");
    Bmatch_PrepareNtk1( pNtk1, pNtk_Qbf );
    //printf("== \n");
    Bmatch_CreatePIMUXes( pNtk1, pNtk2, pNtk_Qbf );
    //printf("=== \n");
    if( muxOnCir2 ){
        Bmatch_CreatePOMUXes( pNtk2, pNtk1, pNtk_Qbf );
        forILP = Abc_NtkPoNum( pNtk1 ) + 1 - ILP_constraint;
    }
    else{
        Bmatch_CreatePOMUXes( pNtk1, pNtk2, pNtk_Qbf );
        forILP = Abc_NtkPoNum( pNtk2 ) + 1 - ILP_constraint;
    }
    //printf("==== \n"); 
    Bmatch_PrepareFinal( pNtk_Qbf, forILP );

    Abc_NtkOrderObjsByName( pNtk_Qbf, 0 );

    if ( !Abc_NtkCheck( pNtk_Qbf ) ) {
        printf( "The AIG construction has failed.\n" );     
        Abc_NtkDelete( pNtk_Qbf );     
        return NULL; 
    }
    return pNtk_Qbf;
}

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Bmatch_PrepareNtk1 function]

  Description [Copy Ntk1 to Qbf Ntk]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_PrepareNtk1( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk_Qbf )
{
    Abc_Obj_t * pObj, * pObjNew, * pNode;
    char * pName;
    int i;

    Abc_AigConst1(pNtk1)->pCopy = Abc_AigConst1(pNtk_Qbf);

    Abc_NtkForEachPi( pNtk1, pObj, i )
    {
        pObjNew = Abc_NtkCreatePi( pNtk_Qbf );
        // remember this PI in the old PIs
        pObj->pCopy = pObjNew;
        // add name

        pName = "z_";
        pName = Bmatch_NameAddPrefix(pName, 2, Abc_ObjName(pObj));
        Abc_ObjAssignName( pObjNew, pName, "_cir1" );
        delete pName;
    }

    assert( Abc_NtkIsDfsOrdered(pNtk1) );

    Abc_AigForEachAnd( pNtk1, pNode, i )
        pNode->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
} 

/**Function*************************************************************

  Synopsis    [Bmatch_CreatePIMUXes function]

  Description [Create PI MUXes from Ntk1 for Ntk2]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_CreatePIMUXes ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf)
{
    Abc_Obj_t * pPi, * pObj;
    int i, level = 1, fForPi = 1;
    vector< Abc_Obj_t * > Pi_Pool;

    Abc_NtkForEachPi( pNtk1, pPi, i)
        Pi_Pool.push_back( pPi->pCopy );

    // Construct PI MUXes for Ntk2
    Abc_NtkForEachPi( pNtk2, pPi, i)
    {
        Bmatch_Construct_MUXes( Pi_Pool, pPi, pNtk_Qbf, level, fForPi );
        level = 1;
    }
    // Connect Ntk2
    Abc_AigConst1(pNtk2)->pCopy = Abc_AigConst1(pNtk_Qbf);

    Abc_AigForEachAnd( pNtk2, pObj, i )
        pObj->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Abc_ObjChild0Copy(pObj), Abc_ObjChild1Copy(pObj) );
}

/**Function*************************************************************

  Synopsis    [Bmatch_PrepareNtk1 function]

  Description [Create PO MUXes]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/


void Bmatch_CreatePOMUXes( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf )
{
    Abc_Obj_t * pPo, * pObj, * pObjA;
    int i, level = 1;
    char * pSuffix = new char[3]; pSuffix = "_*\0"; 
    vector< Abc_Obj_t * > Po_Pool;
    char * pName;

    assert( Output_Pool.size() == 0 && Control_Pool.size() == 0 );

    Abc_NtkForEachPo( pNtk1, pPo, i)
    {
        Po_Pool.push_back( Abc_ObjChild0Copy(pPo) );  
    }

    // Construct PO MUXes for Ntk2
    Abc_NtkForEachPo( pNtk2, pPo, i)
    {
        Bmatch_Construct_MUXes( Po_Pool, pPo, pNtk_Qbf, level );
        level = 1;
    }
    Po_Pool.clear();
    
    Abc_NtkForEachPo( pNtk2, pPo, i)
    {
        pObj = Abc_AigXor( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pPo->pCopy, Abc_ObjChild0Copy(pPo) );
        Po_Pool.push_back( pObj );
    }

    for( int j = 0; j < Po_Pool.size(); ++j){
        pObjA = Abc_NtkCreatePi( pNtk_Qbf );

        pName = "y_";
        pName = Bmatch_NameAddPrefix(pName, 2, Abc_ObjName(Abc_NtkPo(pNtk2, j)));
        Abc_ObjAssignName( pObjA, pName, pSuffix );
        delete pName;

        pObj = Abc_AigMux( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pObjA, Abc_ObjNot(Abc_AigConst1(pNtk_Qbf)), Po_Pool[j] );
        Output_Pool.push_back( pObj );
        Control_Pool.push_back( pObjA );
    }
}

void Bmatch_PrepareFinal( Abc_Ntk_t * pNtk_Qbf, int ILP_constraint )
{
    Abc_Obj_t * pObj, * pILP, * pOutput;

    assert( Output_Pool.size() >= 1 && Control_Pool.size() >= 1);
    pObj = Output_Pool[0];
    if( Output_Pool.size() > 1){
        for( int j = 0; j < Output_Pool.size() - 1; ++j){
            pObj = Abc_AigOr( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pObj, Output_Pool[j + 1]);
        }
    }

    pILP = Bmatch_Construct_ILP( Control_Pool, pNtk_Qbf, ILP_constraint);

    pObj = Abc_AigAnd( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Abc_ObjNot(pILP), Abc_ObjNot(pObj) );
    pOutput = Abc_NtkCreatePo( pNtk_Qbf );
    Abc_ObjAddFanin( pOutput, pObj );

    Output_Pool.clear();
    Control_Pool.clear();

    Abc_AigCleanup( (Abc_Aig_t *)pNtk_Qbf->pManFunc );
    // Abc_NtkAddDummyPiNames( pNtk_Qbf );
    Abc_NtkAddDummyPoNames( pNtk_Qbf ); 
    Abc_NtkAddDummyBoxNames( pNtk_Qbf );
}

/**Function*************************************************************

  Synopsis    [Bmatch_Construct_MUXes]

  Description [Construct MUXes]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/


void Bmatch_Construct_MUXes( vector< Abc_Obj_t * > & Pi_Pool, Abc_Obj_t *& pObj2, Abc_Ntk_t *& pNtk_Qbf, int & level, int & fForPi)
{
    vector< Abc_Obj_t * > new_Pi_Pool;
    Abc_Obj_t * pObj, * pObjA;
    char * pSuffix, * pName;

    assert( Pi_Pool.size() >= 1);
    if( Pi_Pool.size() >= 2){
        pSuffix = new char[10];
        sprintf( pSuffix, "_%d", level);
        ++level;

        pObjA = Abc_NtkCreatePi( pNtk_Qbf );

        if( fForPi == 1 ){
        pName = "x_";
        }
        else{
            pName = "y_";
        }
        pName = Bmatch_NameAddPrefix(pName, 2, Abc_ObjName(pObj2));
        Abc_ObjAssignName( pObjA, pName, pSuffix );
        delete pName;

        for( int i = 0; i < Pi_Pool.size() - 1; i = i + 2 ){
            pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Pi_Pool[i + 1], Pi_Pool[i] );
            new_Pi_Pool.push_back( pObj );
        }
        if( !(Pi_Pool.size() % 2 == 0) ){
            new_Pi_Pool.push_back( Pi_Pool[ Pi_Pool.size() - 1 ] );
        }
        Bmatch_Construct_MUXes( new_Pi_Pool, pObj2, pNtk_Qbf, level, fForPi );
    }
    else{
        pObj = Pi_Pool[0];
        if( fForPi == 1 ){
            pSuffix = "_*\0";
            pObjA = Abc_NtkCreatePi( pNtk_Qbf );

            if( fForPi == 1 ){
                pName = "x_";
            }
            else{
                pName = "y_";
            }
            pName = Bmatch_NameAddPrefix(pName, 2, Abc_ObjName(pObj2));
            Abc_ObjAssignName( pObjA, pName, pSuffix );
            delete pName;
            
            pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Abc_ObjNot(Abc_AigConst1(pNtk_Qbf)), Pi_Pool[0] );
        }
        pSuffix = "_0\0";
        pObjA = Abc_NtkCreatePi( pNtk_Qbf );

        if( fForPi == 1 ){
            pName = "x_";
        }
        else{
            pName = "y_";
        }
        pName = Bmatch_NameAddPrefix(pName, 2, Abc_ObjName(pObj2));
        Abc_ObjAssignName( pObjA, pName, pSuffix );
        delete pName;

        pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Abc_ObjNot( pObj ), pObj );
        pObj2->pCopy = pObj;
    }
}

/**Function*************************************************************

  Synopsis    [Bmatch_Construct_ILP]

  Description [Construct ILP circuit]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/


Abc_Obj_t * Bmatch_Construct_ILP( vector< Abc_Obj_t * > & Pool, Abc_Ntk_t *& pNtk_Qbf, const int & k )
{
    int count = k;
    assert( count > 0 );

    vector< Abc_Obj_t * > mux_Pool(Pool.size() + 1, Abc_AigConst1( pNtk_Qbf ));
    mux_Pool[0] = Abc_ObjNot( Abc_AigConst1( pNtk_Qbf ) );
    while(count > 0){
        for(int i = 0; i < Pool.size() - k + 1; ++i){
            mux_Pool[i + 1] =
            Abc_AigMux( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Pool[i + k - count], mux_Pool[i + 1], mux_Pool[i] );
        }
        --count;
    }
    return mux_Pool[Pool.size() - k + 1];
}

char * Bmatch_NameAddPrefix( char *& pPrefix, int plength, char * pName )
{
    char * pReturn = new char[30];
    int k = 0;
    while(1){
        *(pReturn + plength + k) = *(pName + k);
        if( *(pName + k) == '\0') break;
        ++k;
    }
    for(int i = 0; i < plength; ++i){
        *(pReturn + i) = *(pPrefix + i);
    }
    return pReturn;
}

/**Function*************************************************************

  Synopsis    [Bmatch_SolveQbf]

  Description [Solve QBF of the network]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

bool Bmatch_SolveQbf ( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nInputs, int nItersMax, int fVerbose )
{
    Abc_Ntk_t * pNtkVer, * pNtkSyn, * pNtkSyn2, * pNtkTemp;
    abctime clkTotal = Abc_Clock(), clkS, clkV;
    int nIters, nPars, RetValue, fFound = 0;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( Abc_NtkIsComb(pNtk) );
    assert( Abc_NtkPoNum(pNtk) == 1 );
    assert( nInputs > 0 && nInputs < Abc_NtkPiNum(pNtk) );
    // assert( Abc_NtkPiNum(pNtk)-nPars < 32 );
    nPars = Abc_NtkPiNum(pNtk) - nInputs;

    // create random init value
    {
    int i;
    srand( time(NULL) );
    for ( i = nPars; i < Abc_NtkPiNum(pNtk); i++ )
        Vec_IntWriteEntry( vPiValues, i, rand() & 1 );
    }

    Abc_NtkVectorClearPars( vPiValues, nPars );
    pNtkSyn = Abc_NtkMiterCofactor( pNtk, vPiValues );
    if ( fVerbose )
    {
        printf( "Iter %2d : ", 0 );
        printf( "AIG = %6d  ", Abc_NtkNodeNum(pNtkSyn) );
        Abc_NtkVectorPrintVars( pNtk, vPiValues, nPars );
        printf( "\n" );
    }

    // iteratively solve
    for ( nIters = 0; nIters < nItersMax; nIters++ )
    {
        // solve the synthesis instance
        clkS = Abc_Clock();
        // RetValue = Abc_NtkMiterSat( pNtkSyn, 0, 0, 0, NULL, NULL );
        RetValue = Abc_NtkDSat( pNtkSyn, (ABC_INT64_T)0, (ABC_INT64_T)0, 0, 0, 0, 1, 0, 0, 0 );
        clkS = Abc_Clock() - clkS;
        if ( RetValue == 0 )  // SAT
            Abc_NtkModelToVector( pNtkSyn, vPiValues );
        if ( RetValue == 1 )  // UNSAT
        {
            break;
        }
        if ( RetValue == -1 )  // TIMEOUT
        {
            printf( "Synthesis timed out.\n" );
            break;
        }
        // there is a counter-example

        // construct the verification instance
        Abc_NtkVectorClearVars( pNtk, vPiValues, nPars );
        pNtkVer = Abc_NtkMiterCofactor( pNtk, vPiValues );
        // complement the output
        Abc_ObjXorFaninC( Abc_NtkPo(pNtkVer,0), 0 );

        // solve the verification instance
        clkV = Abc_Clock();
        RetValue = Abc_NtkMiterSat( pNtkVer, 0, 0, 0, NULL, NULL );
        clkV = Abc_Clock() - clkV;
        if ( RetValue == 0 )
            Abc_NtkModelToVector( pNtkVer, vPiValues );
        Abc_NtkDelete( pNtkVer );
        if ( RetValue == 1 )
        {
            fFound = 1;
            break;
            // int nZeros = Vec_IntCountZero( vPiValues );
            // printf( "Parameters: " );
            // Abc_NtkVectorPrintPars( vPiValues, nPars );
            // printf( "  Statistics: 0=%d 1=%d\n", nZeros, Vec_IntSize(vPiValues) - nZeros );
            // printf( "Solved after %d interations.  \n", nIters );
            // fFound = 0;
        }
        if ( RetValue == -1 )
        {
            printf( "Verification timed out.\n" );
            break;
        }
        // there is a counter-example

        // create a new synthesis network
        Abc_NtkVectorClearPars( vPiValues, nPars );
        pNtkSyn2 = Abc_NtkMiterCofactor( pNtk, vPiValues );
        // add to the synthesis instance
        pNtkSyn = Abc_NtkMiterAnd( pNtkTemp = pNtkSyn, pNtkSyn2, 0, 0 );
        Abc_NtkDelete( pNtkSyn2 );
        Abc_NtkDelete( pNtkTemp );

        if ( fVerbose )
        {
            printf( "Iter %2d : ", nIters+1 );
            printf( "AIG = %6d  ", Abc_NtkNodeNum(pNtkSyn) );
            Abc_NtkVectorPrintVars( pNtk, vPiValues, nPars );
            printf( "  " );
            ABC_PRT( "Syn", clkS );
            // ABC_PRT( "Ver", clkV );
        }
        if ( nIters+1 == nItersMax )
            break;
    }
    Abc_NtkDelete( pNtkSyn );
    // report the results
    ABC_PRT( "Total runtime", Abc_Clock() - clkTotal );
    if ( fFound )
    {
        // int nZeros = Vec_IntCountZero( vPiValues );
        // printf( "Parameters: " );
        // Abc_NtkVectorPrintPars( vPiValues, nPars );
        // printf( "  Statistics: 0=%d 1=%d\n", nZeros, Vec_IntSize(vPiValues) - nZeros );
        printf( "Solved after %d iterations.  ", nIters );
        return true;
    }
    else if ( nIters == nItersMax )
    {
        printf( "Unsolved after %d iterations.  ", nIters );
        printf( "Quit after %d iterations.  ", nItersMax );
    }
    else
        printf( "Implementation does not exist.  " );
    return false;    
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
