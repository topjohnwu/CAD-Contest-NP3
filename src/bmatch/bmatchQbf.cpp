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
#ifdef __cplusplus
extern "C" {
#endif

Abc_Ntk_t * Bmatch_PrepareQbfNtk        ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );
void        Bmatch_PrepareNtk1          ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk_Qbf );          
void        Bmatch_CreatePIMUXes        ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf );
void        Bmatch_CreatePOMUXesAndPO   ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf );
void        Bmatch_Construct_MUXes      ( vector< Abc_Obj_t * > & , Abc_Obj_t *& , Abc_Ntk_t *& , int & );

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
    Abc_Ntk_t * pNtk_Qbf;
    pNtk_Qbf = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );

    char * pName = "AIG_for_Qbf"; // the name comes from the user’s application
    pNtk_Qbf->pName = Extra_UtilStrsav( pName );

    printf("= \n");
    Bmatch_PrepareNtk1( pNtk1, pNtk_Qbf );
    printf("== \n");
    Bmatch_CreatePIMUXes( pNtk1, pNtk2, pNtk_Qbf );
    printf("=== \n");
    Bmatch_CreatePOMUXesAndPO( pNtk1, pNtk2, pNtk_Qbf );
    printf("==== \n");

<<<<<<< 346cbe30592a9a7c06f9f57471e87f9c31cb329b
=======
    Abc_AigCleanup( (Abc_Aig_t *)pNtk_Qbf->pManFunc );
    Abc_NtkAddDummyPiNames( pNtk_Qbf );
    Abc_NtkAddDummyPoNames( pNtk_Qbf ); 
    Abc_NtkAddDummyBoxNames( pNtk_Qbf );

    Abc_NtkOrderObjsByName( pNtk_Qbf, 0 );

>>>>>>> Add name prefix
    if ( !Abc_NtkCheck( pNtk_Qbf ) ) {
        printf( "The AIG construction has failed.\n" );     
        Abc_NtkDelete( pNtk_Qbf );     
        return NULL; 
    }
    return pNtk_Qbf;
    /* 
    // Construct Testing Circuit.

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
    */
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
        Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), NULL );
        pName = new char[20];
        int k = 0;
        while(1){
            *(pName + 2 + k) = *(Abc_ObjName(pObj) + k);
            if( *(Abc_ObjName(pObj) + k) == '\0') break;
            ++k;
        }
        *pName = 'y';
        *(pName + 1) = '_';

        Abc_ObjAssignName( pObjNew, pName, "_cir1" );
        delete pName;
        Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), NULL );
    }

    assert( Abc_NtkIsDfsOrdered(pNtk1) );

    Abc_AigForEachAnd( pNtk1, pNode, i )
        pNode->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
} 

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Bmatch_CreatePIMUXes function]

  Description [Create PI MUXes from Ntk1 for Ntk2]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_CreatePIMUXes ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf)
{
    Abc_Obj_t * pPi, * pObj;
    int i, level = 1;
    vector< Abc_Obj_t * > Pi_Pool;
    Pi_Pool.push_back( Abc_AigConst1( pNtk_Qbf ) );
    Abc_NtkForEachPi( pNtk1, pPi, i)
    {
        Pi_Pool.push_back( pPi->pCopy );
    }
    // Construct PI MUXes for Ntk2
    Abc_NtkForEachPi( pNtk2, pPi, i)
    {
        Bmatch_Construct_MUXes( Pi_Pool, pPi, pNtk_Qbf, level );
        level = 1;
    }
    // Connect Ntk2
    Abc_AigConst1(pNtk2)->pCopy = Abc_AigConst1(pNtk_Qbf);
    Abc_AigForEachAnd( pNtk2, pObj, i )
        pObj->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtk_Qbf->pManFunc, Abc_ObjChild0Copy(pObj), Abc_ObjChild1Copy(pObj) );
}

void Bmatch_CreatePOMUXesAndPO( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk_Qbf )
{
    Abc_Obj_t * pPo, * pObj, * pObjA, * pOutput;
    int i, level = 1;
    char * pSuffix = new char[3]; pSuffix = "00\0"; 
    vector< Abc_Obj_t * > Po_Pool, output_Pool, control_Pool;

    char * pName;

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

    Abc_NtkForEachPo( pNtk2, pPo, i)
    {
        pObj = Abc_AigXor( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pPo->pCopy, Abc_ObjChild0Copy(pPo) );
        output_Pool.push_back( pObj );
    }
    for( int j = 0; j < output_Pool.size(); ++j){
        pObjA = Abc_NtkCreatePi( pNtk_Qbf );

        pName = new char[20];
        int k = 0;
        while(1){
            *(pName + 2 + k) = *(Abc_ObjName(Abc_NtkPo(pNtk2, j)) + k);
            if( *(Abc_ObjName(Abc_NtkPo(pNtk2, j)) + k) == '\0') break;
            ++k;
        }
        *(pName + 2) = *Abc_ObjName(Abc_NtkPo(pNtk2, j));
        *pName = 'x';
        *(pName + 1) = '_';

        Abc_ObjAssignName( pObjA, pName, pSuffix );
        delete pName;

        pObj = Abc_AigMux( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pObjA, output_Pool[j], Abc_ObjNot(Abc_AigConst1(pNtk_Qbf)) );
        control_Pool.push_back( pObj );
    }

    pObj = control_Pool[0];
    if( control_Pool.size() > 1){
        for( int j = 0; j < control_Pool.size() - 1; ++j){
            pObj = Abc_AigOr( (Abc_Aig_t *)pNtk_Qbf->pManFunc, pObj, control_Pool[j + 1]);
        }
    }

    pOutput = Abc_NtkCreatePo( pNtk_Qbf );
    Abc_ObjAddFanin( pOutput, pObj );

    Abc_AigCleanup( (Abc_Aig_t *)pNtk_Qbf->pManFunc );
    Abc_NtkAddDummyPiNames( pNtk_Qbf );
    Abc_NtkAddDummyPoNames( pNtk_Qbf ); 
    Abc_NtkAddDummyBoxNames( pNtk_Qbf );
}

void Bmatch_Construct_MUXes( vector< Abc_Obj_t * > & Pi_Pool, Abc_Obj_t *& pObj2, Abc_Ntk_t *& pNtk_Qbf, int &level)
{
    vector< Abc_Obj_t * > new_Pi_Pool;
    Abc_Obj_t * pObj, * pObjA;
    char * pSuffix, * pName;

    assert( Pi_Pool.size() >= 1);
    if( Pi_Pool.size() >= 2){
        pSuffix = new char[6];
        sprintf( pSuffix, "%d", level);
        ++level;

        pObjA = Abc_NtkCreatePi( pNtk_Qbf );
        Abc_ObjAssignName( pObjA, Abc_ObjName(pObj2), pSuffix );

        for( int i = 0; i < Pi_Pool.size() - 1; i = i + 2 ){
            pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Pi_Pool[i], Pi_Pool[i + 1] );
            new_Pi_Pool.push_back( pObj );
        }
        if( !(Pi_Pool.size() % 2 == 0) ){
            new_Pi_Pool.push_back( Pi_Pool[ Pi_Pool.size() - 1 ] );
        }
        Bmatch_Construct_MUXes( new_Pi_Pool, pObj2, pNtk_Qbf, level );
    }
    else{
        pSuffix = new char[2];
        sprintf( pSuffix, "%d", 0);

        pObjA = Abc_NtkCreatePi( pNtk_Qbf );

        pName = new char[20];
        int k = 0;
        while(1){
            *(pName + 2 + k) = *(Abc_ObjName(pObj2) + k);
            if( *(Abc_ObjName(pObj2) + k) == '\0') break;
            ++k;
        }
        *pName = 'x';
        *(pName + 1) = '_';

        Abc_ObjAssignName( pObjA, pName, pSuffix );
        delete pName;

        pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Pi_Pool[0], Abc_ObjNot( Pi_Pool[0] ) );
        pObj2->pCopy = pObj;
    }
}


// NOTE: Unmodified yet

void Bmatch_SolveQbf( Abc_Ntk_t * pNtk, int nPars, int nItersMax, int fDumpCnf, int fVerbose )
{
    Abc_Ntk_t * pNtkVer, * pNtkSyn, * pNtkSyn2, * pNtkTemp;
    Vec_Int_t * vPiValues;
    abctime clkTotal = Abc_Clock(), clkS, clkV;
    int nIters, nInputs, RetValue, fFound = 0;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( Abc_NtkIsComb(pNtk) );
    assert( Abc_NtkPoNum(pNtk) == 1 );
    assert( nPars > 0 && nPars < Abc_NtkPiNum(pNtk) );
//    assert( Abc_NtkPiNum(pNtk)-nPars < 32 );
    nInputs = Abc_NtkPiNum(pNtk) - nPars;

    if ( fDumpCnf )
    {
        // original problem: \exists p \forall x \exists y.  M(p,x,y)
        // negated problem:  \forall p \exists x \exists y. !M(p,x,y)
        //extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
        Aig_Man_t * pMan = Abc_NtkToDar( pNtk, 0, 0 );
        Cnf_Dat_t * pCnf = Cnf_Derive( pMan, 0 );
        Vec_Int_t * vVarMap, * vForAlls, * vExists;
        Aig_Obj_t * pObj;
        char * pFileName;
        int i, Entry;
        // create var map
        vVarMap = Vec_IntStart( pCnf->nVars );
        Aig_ManForEachCi( pMan, pObj, i )
            if ( i < nPars )
                Vec_IntWriteEntry( vVarMap, pCnf->pVarNums[Aig_ObjId(pObj)], 1 );
        // create various maps
        vForAlls = Vec_IntAlloc( nPars );
        vExists = Vec_IntAlloc( Abc_NtkPiNum(pNtk) - nPars );
        Vec_IntForEachEntry( vVarMap, Entry, i )
            if ( Entry )
                Vec_IntPush( vForAlls, i );
            else
                Vec_IntPush( vExists, i );
        // generate CNF
        pFileName = Extra_FileNameGenericAppend( pNtk->pSpec, ".qdimacs" );
        Cnf_DataWriteIntoFile( pCnf, pFileName, 0, vForAlls, vExists );
        Aig_ManStop( pMan );
        Cnf_DataFree( pCnf );
        Vec_IntFree( vForAlls );
        Vec_IntFree( vExists );
        Vec_IntFree( vVarMap );
        printf( "The 2QBF formula was written into file \"%s\".\n", pFileName );
        return;
    }

    // initialize the synthesized network with 0000-combination
    vPiValues = Vec_IntStart( Abc_NtkPiNum(pNtk) );

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
//        RetValue = Abc_NtkMiterSat( pNtkSyn, 0, 0, 0, NULL, NULL );
        RetValue = Abc_NtkDSat( pNtkSyn, (ABC_INT64_T)0, (ABC_INT64_T)0, 0, 0, 0, 1, 0, 0, 0 );
clkS = Abc_Clock() - clkS;
        if ( RetValue == 0 )
            Abc_NtkModelToVector( pNtkSyn, vPiValues );
        if ( RetValue == 1 )
        {
            break;
        }
        if ( RetValue == -1 )
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
//            ABC_PRT( "Ver", clkV );
        }
        if ( nIters+1 == nItersMax )
            break;
    }
    Abc_NtkDelete( pNtkSyn );
    // report the results
    if ( fFound )
    {
        int nZeros = Vec_IntCountZero( vPiValues );
        printf( "Parameters: " );
        Abc_NtkVectorPrintPars( vPiValues, nPars );
        printf( "  Statistics: 0=%d 1=%d\n", nZeros, Vec_IntSize(vPiValues) - nZeros );
        printf( "Solved after %d interations.  ", nIters );
    }
    else if ( nIters == nItersMax )
        printf( "Unsolved after %d interations.  ", nIters );
    else if ( nIters == nItersMax )
        printf( "Quit after %d interatios.  ", nItersMax );
    else
        printf( "Implementation does not exist.  " );
    ABC_PRT( "Total runtime", Abc_Clock() - clkTotal );
    Vec_IntFree( vPiValues );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
