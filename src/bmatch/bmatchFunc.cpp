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
#include "base/abc/abc.h"
#include <iostream>
#include <algorithm>

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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int verbose = 0 );
void Bmatch_SortInformation ( Abc_Frame_t * pAbc, bool verbose );

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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int verbose )
{
    pAbc->pInformation  = new PI_PO_INFO;
    PI_PO_INFO          * pInformation;
    Abc_Obj_t           * pObj;
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

    // Bmatch_PrintObj( pNtk1 );
    // Bmatch_PrintObj( pNtk2 );

if( (verbose & 1) != 0 ) Bmatch_PrintNtkStats( pNtk1 );
if( (verbose & 2) != 0 ) Bmatch_PrintIO( pNtk1 );
if( (verbose & 1) != 0 ) Bmatch_PrintNtkStats( pNtk2 );
if( (verbose & 2) != 0 ) Bmatch_PrintIO( pNtk1 );

    // Init pInformation
    pInformation = ( PI_PO_INFO * ) pAbc->pInformation;
    pInformation->_f_match = new list< Abc_Obj_t * > [ Abc_NtkPoNum(pNtk1) ];
    pInformation->_x_match = new list< Abc_Obj_t * > [ Abc_NtkPiNum(pNtk1) + 1];
    
    Abc_NtkForEachPo( pNtk1, pObj, i ) { 
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_f).push_back(tmpWrap);
    }
    Abc_NtkForEachPo( pNtk2, pObj, i ) {
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_g).push_back( tmpWrap ); 
    }
    Abc_NtkForEachPi( pNtk1, pObj, i ) {
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_x).push_back( tmpWrap );
    }
    Abc_NtkForEachPi( pNtk2, pObj, i ) {
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_y).push_back( tmpWrap );
    }
    
    vResult = Sim_ComputeFunSupp( pNtk1, ((verbose & 1) == 0 ? 0 : 1 ) );
if( (verbose & 1) != 0 ) cout << "Supp information of pNtk1 : " << endl;
    for( int i = 0; i < Abc_NtkPoNum(pNtk1); ++i ) {
    // printf("%d ", ((int *)(vResult->pArray[0]))[i] );
    // cout << "po : " << i << " :: ";
        for( int k = 0; k < (Abc_NtkPiNum(pNtk1) / 32) + 1; ++k ){
            unsigned n;
            n = ((unsigned *)(vResult->pArray[i]))[k];
            for( int j = 0, num = (k == (Abc_NtkPiNum(pNtk1) / 32)) ? (( Abc_NtkPiNum(pNtk1) % 32 == 0 ) ? \
                                   32 : ( Abc_NtkPiNum(pNtk1) % 32 )) : 32; j < num; ++j ){
                if (n & 1){
if( (verbose & 1) != 0 ) printf("1");
                    tmpWrap = ((pInformation->_x)[k * 32 + j]);
                    (((pInformation->_f)[i])->funSupp).push_back( tmpWrap );
                    ++(((pInformation->_f)[i])->numOfInfluence);
                    tmpWrap = ((pInformation->_f)[i]);
                    (((pInformation->_x)[k * 32 + j])->funSupp).push_back( tmpWrap );
                    ++(((pInformation->_x)[k * 32 + j])->numOfInfluence);
                }
else{ if( (verbose & 1) != 0 ) printf("0"); }
                n >>= 1;
            }
        }
if( (verbose & 1) != 0 ) cout << endl;
    }

    vResult = Sim_ComputeFunSupp( pNtk2, (((verbose & 1) == 0) ? 0 : 1) );
if( (verbose & 1) != 0 ) cout << "Supp information of pNtk2 : " << endl;
    for( int i = 0; i < Abc_NtkPoNum(pNtk2); ++i ) {
    // printf("%d ", ((int *)(vResult->pArray[0]))[i] );
    // cout << "po : " << i << " :: ";
        for( int k = 0; k < (Abc_NtkPiNum(pNtk2) / 32) + 1; ++k ){
            unsigned n;
            n = ((unsigned *)(vResult->pArray[i]))[k];
            for( int j = 0, num = (k == (Abc_NtkPiNum(pNtk2) / 32)) ? (( Abc_NtkPiNum(pNtk2) % 32 == 0 ) ? \
                                   32 : (Abc_NtkPiNum(pNtk2) % 32 )) : 32; j < num; ++j ){
                if (n & 1){
if( (verbose & 1) != 0 ) printf("1");
                    tmpWrap = ((pInformation->_y)[k * 32 + j]);
                    (((pInformation->_g)[i])->funSupp).push_back( tmpWrap );
                    ++(((pInformation->_g)[i])->numOfInfluence);
                    tmpWrap = ((pInformation->_g)[i]);
                    (((pInformation->_y)[k * 32 + j])->funSupp).push_back( tmpWrap );
                    ++(((pInformation->_y)[k * 32 + j])->numOfInfluence);
                }
else{ if( (verbose & 1) != 0 ) printf("0"); }
                n >>= 1;
            }
        }
if( (verbose & 1) != 0 ) cout << endl;
    }
    Abc_FrameSetCurrentNetwork( pAbc, pNtk2 );
    Abc_FrameSetCurrentNetwork( pAbc, pNtk1 );

Bmatch_SortInformation( pAbc, ( ((verbose & 4) == 4) ? 1 : 0 ) );

cout << "Init two circuits complete!" << endl;
}

void Bmatch_SortInformation ( Abc_Frame_t * pAbc, bool verbose )
{
    PI_PO_INFO *        tmpInformation;
    tmpInformation = (PI_PO_INFO *)pAbc->pInformation; 
    sort( (tmpInformation->_f).begin(), (tmpInformation->_f).end(), suppWrapComp );
    sort( (tmpInformation->_g).begin(), (tmpInformation->_g).end(), suppWrapComp );
    sort( (tmpInformation->_x).begin(), (tmpInformation->_x).end(), suppWrapComp );
    sort( (tmpInformation->_y).begin(), (tmpInformation->_y).end(), suppWrapComp );
    if( verbose ) {
        printf("Function supp information for two circuits : \n");
        printf("  cir1   supp     cir2   supp   \n");
        for( int i = 0; i < tmpInformation->_f.size() || i < tmpInformation->_g.size(); ++i ) {
            if( i < tmpInformation->_f.size() ) {
                printf("%6s%7d   ", Abc_ObjName((tmpInformation->_f)[i]->thisObj), \
                                     (tmpInformation->_f)[i]->numOfInfluence );
            }
            else printf("                ");
            if( i < tmpInformation->_g.size() ) {
                printf("%6s%7d", Abc_ObjName((tmpInformation->_g)[i]->thisObj), \
                                  (tmpInformation->_g)[i]->numOfInfluence );
            }
            else printf("             ");
            printf("\n");
        }
    }
}

Abc_Ntk_t * Bmatch_GetCone  ( Abc_Ntk_t * pNtk, int * PoNums, int length )
{ 
    return NULL;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
