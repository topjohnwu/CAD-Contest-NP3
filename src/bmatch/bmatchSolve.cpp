/**CppFile**************************************************************
 
  FileName    [bmatchSolve.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Solving Bmatch functions]

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
#include <utility>

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

enum METHOD {
	M_ONE_TO_ONE,
	M_ONE_TO_GRP,
	M_GRP_TO_GRP,
	M_ALL
};

void Bmatch_SolveByFunSupp  		( Abc_Frame_t * pAbc, int verbose = 0 );
void Bmatch_GetGroup 				( const vector< suppWrap * > & a, vector< vector< suppWrap * > > & grp );
suppWrap * Bmatch_FindSuppWrap		( const vector< pair< suppWrap *, vector< suppWrap * > > > & a, suppWrap * & b );
bool Bmatch_GetGuess 				( vector< vector< suppWrap * > > & f,
					  				  vector< vector< suppWrap * > > & g,
					  				  vector< pair< suppWrap *, vector< suppWrap * > > > & fo,
					   				  vector< pair< suppWrap *, vector< suppWrap * > > > & gi,
					  				  PI_PO_INFO * pInformation,
					  				  METHOD method );

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

void Bmatch_GetGroup ( const vector< suppWrap * > & a, vector< vector< suppWrap * > > & grp )
{
	vector< suppWrap * > tmpGrp;
	int grpNum;
	grpNum = a[0]->numOfInfluence;
	for( int i = 0, n = a.size(); i < n; ++i ) {
		if( a[i]->numOfInfluence == grpNum ) {
			tmpGrp.push_back(a[i]);
		}
		else{
			grpNum = a[i]->numOfInfluence;
			grp.push_back(tmpGrp);
			tmpGrp.clear();
			tmpGrp.push_back(a[i]);
		}
	}
	grp.push_back( tmpGrp );
}

suppWrap * Bmatch_FindSuppWrap( const vector< pair< suppWrap *, vector< suppWrap * > > > & a, suppWrap * & b )
{
	for( int i = 0, n = a.size(); i < n; ++i ) {
		for( int j = 0, m = a[i].second.size(); j < m; ++j ) {
			if( a[i].second[j] == b ) {
				return a[i].first;
			}
		}
	}
	return NULL;
}

bool Bmatch_GetGuess ( vector< vector< suppWrap * > > & f,
					   vector< vector< suppWrap * > > & g,
					   vector< pair< suppWrap *, vector< suppWrap * > > > & fo,
					   vector< pair< suppWrap *, vector< suppWrap * > > > & gi,
					   PI_PO_INFO * pInformation,
					   METHOD method )
{
	assert( !f.empty() && !g.empty() );
	pair< suppWrap *, vector< suppWrap * > > tmpPair;
	vector< suppWrap * > mapNodes;
	suppWrap * tmpWrap1, * tmpWrap2, * tmpW;
	int piCap = 3, poCap = 1, currentIndex = 0;
	bool done = false, findPair = false, save = false, notGood = false;

    //int capacity = ( Abc_NtkPoNum( tmpInformation->_cir1 ) - Abc_NtkPoNum( tmpInformation->_cir2 ) );
    //capacity = (capacity >= 0) ? capacity + 1 : 1;

	switch( method ) {
		case M_ONE_TO_ONE:
            while( !findPair ) {
                if( currentIndex == f.size() ) return false;
                else { 
                    while( !f[currentIndex].empty() ){
                        tmpWrap2 = f[currentIndex][0];
                        for( int i = 0, n = pInformation->_f_match.size(); i < n; ++i ) {
                            if( tmpWrap2 == pInformation->_f_match[i].first ) {
                                f[currentIndex].erase(f[currentIndex].begin());
                    cout << "Not GOOD " << endl;
                                notGood = true;
                                break;
                            }
                        }
                        if( !notGood ) {
                        for( int i = (currentIndex >= poCap) ? currentIndex - poCap : 0,
                                 n = max((int)g.size(), currentIndex + poCap); i < n; ++i ) {
                            for( int j = 0, m = g[i].size(); j < m; ++j ) {
                                if( !(Bmatch_FindSuppWrap( pInformation->_f_already_try, g[i][j] ) == tmpWrap2) ) {
                                    tmpWrap1 = g[i][j];

                                    for( int k = 0, l = pInformation->_f_already_try.size(); k < l; ++k ) {
                                        if( pInformation->_f_already_try[k].first == tmpWrap2 ) {
                                            pInformation->_f_already_try[k].second.push_back(tmpWrap1);
                                            save = true;
                                            break;
                                        }
                                    }
                                    if( !save ) {
                                        mapNodes.push_back(tmpWrap1);
                                        tmpPair = make_pair(tmpWrap2, mapNodes);
                                        pInformation->_f_already_try.push_back(tmpPair);
                                        mapNodes.clear();
                                    }
                                    save = false;
                                    findPair = true;
                                    break;
                                }
                            }
                            if( findPair ) break;
                        }
                        }
                        notGood = false;
                        if( !findPair ) {
                            f[currentIndex].erase(f[currentIndex].begin());
                        }
                        else break;
                    }
                }
                ++currentIndex;
            }
            //
			// tmpWrap1 = g[0][0]; tmpWrap2 = f[0][0];
			mapNodes.push_back( tmpWrap1 );
			tmpPair = make_pair( tmpWrap2, mapNodes );
			fo.push_back(tmpPair);

			mapNodes.clear();
			for( int i = 0, n = tmpWrap1->funSupp.size(); i < n; ++i ) {
				tmpW = Bmatch_FindSuppWrap( pInformation->_x_match, tmpWrap1->funSupp[i] );
				if( tmpW != NULL ) {
					mapNodes.push_back( tmpW );
					done = true;
				}
				if( !done ) {
					for( int j = 0, m = tmpWrap2->funSupp.size(); j < m; ++j ) {
						if( tmpWrap2->funSupp[j]->numOfInfluence >= tmpWrap1->funSupp[i]->numOfInfluence - poCap &&
							tmpWrap2->funSupp[j]->numOfInfluence <= tmpWrap1->funSupp[i]->numOfInfluence + poCap ) {
							mapNodes.push_back( tmpWrap2->funSupp[j] );
						}
					}
				}
				tmpPair = make_pair( tmpWrap1->funSupp[i], mapNodes );
				gi.push_back(tmpPair);
				mapNodes.clear();
				done = false;
			}
            //
		break;
		default:
		break;
	}
    return true;
}

void Bmatch_SolveByFunSupp ( Abc_Frame_t * pAbc, int verbose )
{
    // First,  pick POs of _f and _g to be solving
    // Second, Assign which of _f PO to be equal to _g's PO.
    //         Assign whick of _f PIs to be equal to _g's PI.
    // Third,  Construct a QBF circuit.
    // Fourth, Solve the circuit and get equal assignments!
    // Fifth,  pick another pairs of POs from _f and _g.
    // Sixth,  Until conflict or end.
	Abc_Ntk_t * pNtkQbf;
    Vec_Int_t * vPiValues;
    int forAllVarsNum;

    PI_PO_INFO * tmpInformation = (PI_PO_INFO *)pAbc->pInformation;

    vector< vector< suppWrap * > > grpf, grpg;
    vector< pair < suppWrap *, vector< suppWrap * > > > fomap, gimap;

    Bmatch_GetGroup( ((PI_PO_INFO*)pAbc->pInformation)->_f, grpf );
    Bmatch_GetGroup( ((PI_PO_INFO*)pAbc->pInformation)->_g, grpg );
    
    while(true) {
    	if( !Bmatch_GetGuess( grpf, grpg, fomap, gimap, tmpInformation, M_ONE_TO_ONE ) ) break;
    	// Need to check fomap, gimap is GOOD !
    	cout << "fomap" << endl;
    	Bmatch_PrintMatchPairs( fomap );
    	cout << "gimap" << endl;
    	Bmatch_PrintMatchPairs( gimap );

    	pNtkQbf = Bmatch_ConstructQbfNtk( tmpInformation, fomap, gimap, forAllVarsNum );

//        Abc_FrameSetCurrentNetwork( pAbc, pNtkQbf );
    	
        cout << "Construct QBF circuit complete !" << endl;

        vPiValues = Vec_IntStart( Abc_NtkPiNum(pNtkQbf) );
        if( Bmatch_SolveQbf( pNtkQbf, vPiValues, forAllVarsNum, 200, 0 ) ) {
            cout << "Solve" << endl;
            Bmatch_UpdateMatchPair( tmpInformation, fomap, gimap, pNtkQbf, vPiValues->pArray );
        }
        fomap.clear();
        gimap.clear();
	}
    cout << "done" << endl;
    // Bmatch_PrintAnswer( pAbc, 0 );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END