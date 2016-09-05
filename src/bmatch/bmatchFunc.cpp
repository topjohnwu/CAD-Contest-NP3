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
void Bmatch_PIInformation   ( Abc_Frame_t * pAbc );
void Bmatch_SortOne         ( vector< suppWrap * > & a);

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

if( (verbose & 16) != 0 ) Bmatch_Resync( pNtk1 );
if( (verbose & 16) != 0 ) Bmatch_Resync( pNtk2 );

    Abc_NtkOrderObjsByName( pNtk1, 0 );
    Abc_NtkOrderObjsByName( pNtk2, 0 );
    // Abc_NtkShortNames( pNtk1 );
    // Abc_NtkShortNames( pNtk2 );

    // Bmatch_PrintObj( pNtk1 );
    // Bmatch_PrintObj( pNtk2 );

if( (verbose & 1) != 0 || (verbose & 4) != 0 ) Bmatch_PrintNtkStats( pNtk1 );
if( (verbose & 2) != 0 ) Bmatch_PrintIO( pNtk1 );
if( (verbose & 1) != 0 || (verbose & 4) != 0 ) Bmatch_PrintNtkStats( pNtk2 );
if( (verbose & 2) != 0 ) Bmatch_PrintIO( pNtk1 );

    // Init pInformation
    pInformation = ( PI_PO_INFO * ) pAbc->pInformation;

    Abc_NtkForEachPo( pNtk1, pObj, i ) { 
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_f).push_back( tmpWrap );
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
    tmpWrap = new suppWrap;
    tmpWrap->thisObj = Abc_AigConst1( pNtk1 );
    (pInformation->_x).push_back( tmpWrap );

    Abc_NtkForEachPi( pNtk2, pObj, i ) {
        tmpWrap = new suppWrap;
        tmpWrap->thisObj = pObj;    
        (pInformation->_y).push_back( tmpWrap );
    }
if( (verbose & 8) != 0 ) cout << "complete new" << endl; 
    vResult = Sim_ComputeFunSupp( pNtk1, ((verbose & 1) == 0 ? 0 : 1 ) );
if( (verbose & 1) != 0 ) cout << "Supp information of pNtk1 : " << endl;
    for( int i = 0; i < Abc_NtkPoNum(pNtk1); ++i ) {
    // printf("%d ", ((int *)(vResult->pArray[0]))[i] );
    // cout << "po : " << i << " :: ";
        for( int k = 0, s = (Abc_NtkPiNum(pNtk1) / 32) + (Abc_NtkPiNum(pNtk1) % 32 == 0 ? 0 : 1); k < s; ++k ){
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
// if( (verbose & 8) != 0 ) printf("count i %d k %d \n", i, k);
        }
if( (verbose & 1) != 0 ) cout << endl;
    }
if( (verbose & 8) != 0 ) cout << "complete pNtk1" << endl;
    vResult = Sim_ComputeFunSupp( pNtk2, (((verbose & 1) == 0) ? 0 : 1) );
if( (verbose & 1) != 0 ) cout << "Supp information of pNtk2 : " << endl;
    for( int i = 0; i < Abc_NtkPoNum(pNtk2); ++i ) {
    // printf("%d ", ((int *)(vResult->pArray[0]))[i] );
    // cout << "po : " << i << " :: ";
        for( int k = 0, s = (Abc_NtkPiNum(pNtk2) / 32) + (Abc_NtkPiNum(pNtk2) % 32 == 0 ? 0 : 1); k < s; ++k ){
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
if( (verbose & 8) != 0 ) cout << "complete pNtk2" << endl;
    pInformation->_pNtk1 = pNtk1;
    pInformation->_pNtk2 = pNtk2;
    Abc_FrameSetCurrentNetwork( pAbc, pNtk2 );
    Abc_FrameSetCurrentNetwork( pAbc, pNtk1 );

    Bmatch_SortInformation( pAbc, ( ((verbose & 4) == 4) ? 1 : 0 ) );
    if( (verbose & 32) != 0 ) Bmatch_PIInformation ( pAbc );

cout << "Init two circuits complete!" << endl;
}

void Bmatch_SortInformation ( Abc_Frame_t * pAbc, bool verbose )
{
    PI_PO_INFO *        tmpInformation;
    tmpInformation = (PI_PO_INFO *)pAbc->pInformation;
    Bmatch_SortOne( tmpInformation->_f );
    Bmatch_SortOne( tmpInformation->_g );
    Bmatch_SortOne( tmpInformation->_x );
    Bmatch_SortOne( tmpInformation->_y );

    if( verbose ) {
        int total1 = 0, total2 = 0;
        printf("Function supp information for two circuits : \n");
        printf("  cir1   supp     cir2   supp   \n");
        for( int i = 0; i < tmpInformation->_f.size() || i < tmpInformation->_g.size(); ++i ) {
            if( i < tmpInformation->_f.size() ) {
                total1 += (tmpInformation->_f)[i]->numOfInfluence;
                printf("%6s%7d   ", Abc_ObjName((tmpInformation->_f)[i]->thisObj), \
                                     (tmpInformation->_f)[i]->numOfInfluence );
            }
            else printf("                ");
            if( i < tmpInformation->_g.size() ) {
                total2 += (tmpInformation->_g)[i]->numOfInfluence;
                printf("%6s%7d", Abc_ObjName((tmpInformation->_g)[i]->thisObj), \
                                  (tmpInformation->_g)[i]->numOfInfluence );
            }
            else printf("             ");
            printf("\n");
        }
        printf(" total %6d    total %6d   \n", total1, total2 );
    }
}

void Bmatch_SortOne ( vector< suppWrap * > & a ) {
    sort( (a).begin(), (a).end(), suppWrapComp );
    for( int i = 0; i < a.size(); ++i) {
        sort( a[i]->funSupp.begin(), a[i]->funSupp.end(), suppWrapComp );
    };
}

void Bmatch_PIInformation ( Abc_Frame_t * pAbc )
{
    int total1 = 0, total2 = 0;
    PI_PO_INFO *        tmpInformation;
    tmpInformation = (PI_PO_INFO *)pAbc->pInformation;
    char       *        ioInformation1, * ioInformation2;
    ioInformation1 = new char[tmpInformation->_f.size() + 1];
    ioInformation2 = new char[tmpInformation->_g.size() + 1];
    ioInformation1[tmpInformation->_f.size()] = '\0';
    ioInformation2[tmpInformation->_g.size()] = '\0';

        for( int i = 0; i < tmpInformation->_f.size() || i < tmpInformation->_g.size(); ++i ) {
            if( i < tmpInformation->_f.size() )
                printf("%6s:                              ", Abc_ObjName((tmpInformation->_f)[i]->thisObj));
            else
                printf("                                     ");
            if( i < tmpInformation->_g.size() )
                printf("%6s:", Abc_ObjName((tmpInformation->_g)[i]->thisObj));
            else
                printf("       ");
            printf("\n");
            for( int j = 0; j < (tmpInformation->_f)[i]->numOfInfluence || j < (tmpInformation->_g)[i]->numOfInfluence; ++j ) {
                if( j < (tmpInformation->_f)[i]->numOfInfluence ) {
                    for( int r = 0; r < (tmpInformation->_f).size(); ++r ) {
                        for( int u = 0; u < (tmpInformation->_f)[i]->funSupp[j]->funSupp.size(); ++u ) {
                            if( (tmpInformation->_f)[i]->funSupp[j]->funSupp[u] == (tmpInformation->_f)[r] ) {
                                ioInformation1[r] = '1';
                            }
                        }
                    }
                    for( int r = 0; r < (tmpInformation->_f).size(); ++r ) {
                        if( ioInformation1[r] != '1' ) {
                            ioInformation1[r] = '0';
                        }
                    }
                    printf("        %28s", ioInformation1 );
                    for( int r = 0; r < (tmpInformation->_f).size(); ++r ) {
                        ioInformation1[r] = '0';
                    }
                }
                else printf("                                    ");
                if( j < (tmpInformation->_g)[i]->numOfInfluence ) {
                    for( int r = 0; r < (tmpInformation->_g).size(); ++r ) {
                        for( int u = 0; u < (tmpInformation->_g)[i]->funSupp[j]->funSupp.size(); ++u ) {
                            if( (tmpInformation->_g)[i]->funSupp[j]->funSupp[u] == (tmpInformation->_g)[r] ) {
                                ioInformation2[r] = '1';
                            }
                        }
                    }
                    for( int r = 0; r < (tmpInformation->_g).size(); ++r ) {
                        if( ioInformation2[r] != '1' ) {
                            ioInformation2[r] = '0';
                        }
                    }
                    printf("        %28s", ioInformation2 );
                    for( int r = 0; r < (tmpInformation->_g).size(); ++r ) {
                        ioInformation2[r] = '0';
                    }
                }
                printf("\n");
            }
        }
    delete [] ioInformation1;
    delete [] ioInformation2;

//
    printf("PI supp information for two circuits : \n");
    printf("  cir1   supp     cir2   supp   \n");
    for( int i = 0; i < tmpInformation->_x.size() || i < tmpInformation->_y.size(); ++i ) {
        if( i < tmpInformation->_x.size() ) {
            total1 += (tmpInformation->_x)[i]->numOfInfluence;
            printf("%6s%7d   ", Abc_ObjName((tmpInformation->_x)[i]->thisObj), \
                                 (tmpInformation->_x)[i]->numOfInfluence );
        }
        else printf("                ");
        if( i < tmpInformation->_y.size() ) {
            total2 += (tmpInformation->_y)[i]->numOfInfluence;
            printf("%6s%7d", Abc_ObjName((tmpInformation->_y)[i]->thisObj), \
                              (tmpInformation->_y)[i]->numOfInfluence );
        }
        else printf("             ");
        printf("\n");
    }
    printf(" total %6d    total %6d   \n", total1, total2 );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
