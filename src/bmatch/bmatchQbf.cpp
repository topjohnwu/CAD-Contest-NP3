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
    int i;
    Abc_AigConst1(pNtk1)->pCopy = Abc_AigConst1(pNtk_Qbf);

    Abc_NtkForEachPi( pNtk1, pObj, i )
    {
        pObjNew = Abc_NtkCreatePi( pNtk_Qbf );
        // remember this PI in the old PIs
        pObj->pCopy = pObjNew;
        // add name
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
        Abc_ObjAssignName( pObjA, Abc_ObjName(Abc_NtkPo(pNtk2, j)), pSuffix );

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
    char * pSuffix;

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
        Abc_ObjAssignName( pObjA, Abc_ObjName(pObj2), pSuffix );

        pObj = Abc_AigMux( (Abc_Aig_t * )pNtk_Qbf->pManFunc, pObjA, Pi_Pool[0], Abc_ObjNot( Pi_Pool[0] ) );
        pObj2->pCopy = pObj;
    }
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
