/**CppFile**************************************************************
 
  FileName    [bmatchFunc.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Several important functions]

  Author      [Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#include "base/main/mainInt.h"
#include "bmatch.h"
#include "opt/sim/sim.h"
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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );

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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 )
{
    pAbc->pInformation  = new PI_PO_INFO;
    PI_PO_INFO          * pInformation;
    Abc_Obj_t           * pObj;
    Abc_Ntk_t           * pNtkTmp;
    char                * pName;
    int                 i;
    Vec_Ptr_t           * vResult;
    suppWrap            * tmpWrap;

    pName = "cir1";
    Abc_NtkSetName( pNtk1, Extra_UtilStrsav( pName ) );
    pName = "cir2";
    Abc_NtkSetName( pNtk2, Extra_UtilStrsav( pName ) );

    Abc_NtkOrderObjsByName( pNtk1, 0 );
    Abc_NtkOrderObjsByName( pNtk2, 0 );

    Abc_NtkShortNames( pNtk1 );
    Abc_NtkShortNames( pNtk2 );
    Bmatch_PrintIO( pNtk1 );
    // Bmatch_PrintObj( pNtk1 );
    // Bmatch_PrintObj( pNtk2 );
    Bmatch_PrintNtkStats( pNtk1 );
    Bmatch_PrintNtkStats( pNtk2 );

    pInformation = ( PI_PO_INFO * ) pAbc->pInformation;
    pInformation->_f_match = new list< Abc_Obj_t * > [ Abc_NtkPoNum(pNtk1) ];
    pInformation->_x_match = new list< Abc_Obj_t * > [ Abc_NtkPiNum(pNtk1) + 1];
    pInformation->_f = new suppWrap [ Abc_NtkPoNum(pNtk1) ];
        Abc_NtkForEachPo( pNtk1, pObj, i ) { ((pInformation->_f)[i]).thisObj = pObj; }
    pInformation->_g = new suppWrap [ Abc_NtkPoNum(pNtk2) ];
        Abc_NtkForEachPo( pNtk2, pObj, i ) { ((pInformation->_g)[i]).thisObj = pObj; }
    pInformation->_x = new suppWrap [ Abc_NtkPiNum(pNtk1) ];
        Abc_NtkForEachPi( pNtk1, pObj, i ) { ((pInformation->_x)[i]).thisObj = pObj; }
    pInformation->_y = new suppWrap [ Abc_NtkPiNum(pNtk2) ];
        Abc_NtkForEachPi( pNtk1, pObj, i ) { ((pInformation->_y)[i]).thisObj = pObj; }
    
    vResult = Sim_ComputeFunSupp( pNtk1, 1 );
    cout << "Supp information : " << endl;
        for( int i = 0; i < Abc_NtkPoNum(pNtk1); ++i ) {
        //    printf("%d ", ((int *)(vResult->pArray[0]))[i] );
        // cout << "po : " << i << " :: ";
            for( int k = 0; k < (Abc_NtkPiNum(pNtk1) / 32) + 1; ++k ){
            unsigned n;
            n = ((unsigned *)(vResult->pArray[i]))[k];
            for( int j = 0, num = (k == (Abc_NtkPiNum(pNtk1) / 32)) ? (( Abc_NtkPiNum(pNtk1) % 32 == 0 ) ? \
                                   32 : ( Abc_NtkPiNum(pNtk1) % 32 )) : 32; j < num; ++j ){
                if (n & 1){
                    printf("1");
                    tmpWrap = & ((pInformation->_x)[k * 32 + j]);
                    ((pInformation->_f)[i]).funSupp.push_back( tmpWrap );
                }
                else{
                    printf("0");
                }
                n >>= 1;
            }
        }
            cout << endl;
        }
    
    /*
    Abc_NtkForEachPo( pNtk1, pObj, i) {
        pNtkTmp = Abc_NtkCreateCone( pNtk1, Abc_ObjFanin0( pObj ), Abc_ObjName( pObj ), 0 );
        Bmatch_PrintNtkStats( pNtkTmp );
        Bmatch_PrintIO( pNtkTmp );
        cout << "before : " << endl;
        vResult = Sim_ComputeFunSupp( pNtkTmp, 1 );
    
        printf("Result:");
        unsigned n;
        for( int i = 0; i < Abc_NtkPoNum(pNtkTmp); ++i ) {
        //    printf("%d ", ((int *)(vResult->pArray[0]))[i] );
            n = ((unsigned *)(vResult->pArray[i]))[0];
            for( int j = 0; j < 33 ; ++j ){
                if (n & 1)
                    printf("1");
                else
                    printf("0");
                n >>= 1;
            }
            printf(" ");
            n = ((unsigned *)(vResult->pArray[i]))[1];
            for( int j = 0; j < 32; ++j ){
                if (n & 1)
                    printf("1");
                else
                    printf("0");
                n >>= 1;
            }
        }
        printf("\n");
    }*/

    Abc_FrameSetCurrentNetwork( pAbc, pNtk2 );
    Abc_FrameSetCurrentNetwork( pAbc, pNtk1 );
}

Abc_Ntk_t * Bmatch_GetCone  ( Abc_Ntk_t * pNtk, int * PoNums, int length )
{
    

    return NULL;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
