/**CppFile**************************************************************
 
  FileName    [bmatchNewQbf.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Qbf circuit functions for Bmatch]

  Author      [Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#include "base/main/mainInt.h"
#include "bmatch.h"
#include "base/abc/abc.h"
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

Abc_Ntk_t * Bmatch_ConstructQbfNtk 	( PI_PO_INFO * pInformation,
				    				  vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
					    		  	  vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
					    		  	  int & forAllVarsNum );

void 		Bmatch_PreparePIs_POs  	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf,
					    		  	  vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
					    		  	  vector< pair < suppWrap *, vector< suppWrap * > > > & fomap );

void 		Bmatch_PrepareFinalOut 	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtkQbf, int ILP_Constraint = 0 );

void 		Bmatch_ConstructMUX 	( vector< Abc_Obj_t * > & matchPool, Abc_Obj_t * & pObj,
							  		  Abc_Ntk_t * pNtkQbf, int & level, char * & prefix );

#ifdef __cplusplus
}
#endif

Abc_Ntk_t * Bmatch_ConstructQbfNtk ( PI_PO_INFO * pInformation,
								     vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
							  		 vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
							  		 int & forAllVarsNum )
{
	Vec_Ptr_t * cir1Pos, * cir2Pos;
	Abc_Ntk_t * pNtk1, * pNtk2, * pNtkQbf;
	char * pName = "NtkQbf";
	int cir2Count = 0;

	cir1Pos = Vec_PtrStart( fomap.size() );
	for( int i = 0, n = fomap.size(); i < n; ++i ) {
		cir2Count += fomap[i].second.size();
	}
	cir2Pos = Vec_PtrStart( cir2Count );
	cir2Count = 0;

	for( int i = 0, n = fomap.size(); i < n; ++i ) {
		cir1Pos->pArray[i] = fomap[i].first->thisObj;
		for( int j = 0, m = fomap[i].second.size(); j < m; ++j ) {
			cir2Pos->pArray[cir2Count] = fomap[i].second[j]->thisObj;
			++cir2Count;
		}
	}

	pNtk1 = Abc_NtkCreateConeArray( pInformation->_pNtk1, cir1Pos, 0 );
	pNtk2 = Abc_NtkCreateConeArray( pInformation->_pNtk2, cir2Pos, 0 );

	Vec_PtrFree( cir1Pos );
	Vec_PtrFree( cir2Pos );

 Bmatch_PrintIO( pNtk1 );
 Bmatch_PrintIO( pNtk2 );
// Bmatch_PrintObj( pNtk1 );
// Bmatch_PrintObj( pNtk2 );

    pNtkQbf = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    Abc_NtkSetName( pNtkQbf, Extra_UtilStrsav( pName ) );

 cout << "Prepare PI PO" << endl;
	Bmatch_PreparePIs_POs 	   	( pNtk1, pNtk2, pNtkQbf, gimap, fomap );
 cout << "Prepare Final" << endl;
	Bmatch_PrepareFinalOut 		( pNtk1, pNtkQbf );
 cout << "After Final" << endl;

	Abc_NtkOrderObjsByName( pNtkQbf, 0 );
	if ( !Abc_NtkCheck( pNtkQbf ) ) {
        printf( "The AIG construction has failed.\n" );     
        Abc_NtkDelete( pNtkQbf );
    }

Bmatch_PrintIO( pNtkQbf );
// Bmatch_PrintObj( pNtkQbf );
Bmatch_Print3Circuit( pNtk1, pNtk2, pNtkQbf );

    forAllVarsNum = Abc_NtkPiNum( pNtk1 );
	return pNtkQbf;
}

void Bmatch_PreparePIs_POs 	   	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf,
					    		  vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
					    		  vector< pair < suppWrap *, vector< suppWrap * > > > & fomap )
{
	Abc_Obj_t 				* pObj, * pObjNew;
    char 	  				* pName, * prefix, * pSuffix;
    int 		  			  i, level;
    vector< Abc_Obj_t * > 	  matchPool;

    Abc_AigConst1( pNtk1 )->pCopy = Abc_AigConst1( pNtkQbf );

    prefix = "z_";
    pSuffix = "_0\0";
    Abc_NtkForEachPi( pNtk1, pObj, i ) 				// remember to construct all PIs first (including CONST !)
    {
        pObjNew = Abc_NtkCreatePi( pNtkQbf ); 		// remember this PI in the old PIs
        pObj->pCopy = pObjNew;
      
      	// 	add name
        pName = Bmatch_NameAddPrefix( prefix, 2, Abc_ObjName(pObj) );
        Abc_ObjAssignName( pObjNew, pName, pSuffix );
        delete pName;
    }

	assert( Abc_NtkIsDfsOrdered(pNtk1) );
    Abc_AigForEachAnd( pNtk1, pObj, i )
        pObj->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtkQbf->pManFunc, Abc_ObjChild0Copy(pObj), Abc_ObjChild1Copy(pObj) );

    // Next construct PI MUXes !
    prefix = "y_";
    for( int i = 0, n = gimap.size(); i < n; ++i )
    {
    	pObj = Abc_NtkFindCi( pNtk2, Abc_ObjName(gimap[i].first->thisObj) );
    	STATUS  status =  gimap[i].first->status;
    	if( gimap[i].second.size() == 1 && gimap[i].second[0]->status != UNDEFINED ) {
    		if( status == POSITIVE )
    			pObj->pCopy = Abc_NtkFindCi( pNtk1, Abc_ObjName(gimap[i].second[0]->thisObj))->pCopy;
    		else if( status == NEGATIVE )
    			pObj->pCopy = Abc_ObjNot( Abc_NtkFindCi( pNtk1, Abc_ObjName(gimap[i].second[0]->thisObj))->pCopy );
    	}
    	else {
    		level = 1;
			matchPool.push_back( Abc_AigConst1(pNtkQbf) );
			for( int j = 0, m = gimap[i].second.size(); j < m; ++j ) {
				matchPool.push_back( Abc_NtkFindCi( pNtk1, Abc_ObjName(gimap[i].second[j]->thisObj))->pCopy );
			}
    		Bmatch_ConstructMUX( matchPool, pObj, pNtkQbf, level, prefix );
    	}
    }
    // Connect Ntk2
    Abc_AigConst1(pNtk2)->pCopy = Abc_AigConst1(pNtkQbf);

    assert( Abc_NtkIsDfsOrdered(pNtk2) );
    Abc_AigForEachAnd( pNtk2, pObj, i )
        pObj->pCopy = Abc_AigAnd( (Abc_Aig_t *)pNtkQbf->pManFunc, Abc_ObjChild0Copy(pObj), Abc_ObjChild1Copy(pObj) );

    // Next construct PO MUXes !
    prefix = "x_";
    for( int i = 0, n = fomap.size(); i < n; ++i )
    {
    	level = 1;
    	pObj = Abc_NtkFindCo( pNtk1, Abc_ObjName(fomap[i].first->thisObj));
    	for( int j = 0, m = fomap[i].second.size(); j < m; ++j ) {
    		matchPool.push_back( Abc_ObjChild0Copy(Abc_NtkFindCo( pNtk2, Abc_ObjName(fomap[i].second[j]->thisObj))) );
    	}
    	Bmatch_ConstructMUX( matchPool, pObj, pNtkQbf, level, prefix );
    }
}

void Bmatch_PrepareFinalOut 	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtkQbf, int ILP_Constraint )
{
	vector< Abc_Obj_t * > XorOuts;
	Abc_Obj_t * pObj, * pObjTmp;
	char * pName;
	int i;

	Abc_NtkForEachPo( pNtk1, pObj, i ) {
cout << "Print Xor : " << endl;
		Abc_AigPrintNode(pObj->pCopy);
		Abc_AigPrintNode(Abc_ObjChild0Copy(pObj));
cout << "END." << endl;
		pObjTmp = Abc_AigXor( (Abc_Aig_t *)pNtkQbf->pManFunc, pObj->pCopy, Abc_ObjChild0Copy(pObj) );
		XorOuts.push_back( pObjTmp );
	}

	pObjTmp = XorOuts[0];
	for( int i = 1, n = XorOuts.size(); i < n; ++i ) {
		pObjTmp = Abc_AigOr( (Abc_Aig_t *)pNtkQbf->pManFunc, pObjTmp, XorOuts[i] );
	}

	pObj = Abc_NtkCreatePo( pNtkQbf );
	pName = "MainOut";
	Abc_ObjAssignName( pObj, pName, NULL );
	Abc_ObjAddFanin( pObj, Abc_ObjNot( pObjTmp ) );

	Abc_AigCleanup( (Abc_Aig_t *)pNtkQbf->pManFunc );
}

void Bmatch_ConstructMUX 	( vector< Abc_Obj_t * > & matchPool, Abc_Obj_t * & pObjOut,
							  Abc_Ntk_t * pNtkQbf, int & level, char * & prefix )
{
	assert( matchPool.size() >= 1 );

	vector< Abc_Obj_t * > tmpPool;
	Abc_Obj_t * pObjNow, * pObjVar;
	char * pSuffix, * pName;
	pName = Bmatch_NameAddPrefix( prefix, 2, Abc_ObjName(pObjOut) );

	if( matchPool.size() == 1 ) {
		pObjNow = matchPool[0];
		pObjVar = Abc_NtkCreatePi( pNtkQbf );
		pSuffix = "_0\0";

        Abc_ObjAssignName( pObjVar, pName, pSuffix );

        pObjNow = Abc_AigMux( (Abc_Aig_t * )pNtkQbf->pManFunc, pObjVar, pObjNow, Abc_ObjNot( pObjNow ) );
        pObjOut->pCopy = pObjNow;
        matchPool.clear();
	}
	else if( matchPool.size() >= 2 ) {
		pSuffix = new char[10];
        sprintf( pSuffix, "_%d", level);
        ++level;

        pObjVar = Abc_NtkCreatePi( pNtkQbf );
        Abc_ObjAssignName( pObjVar, pName, pSuffix );
        delete pSuffix;

        for( int i = 0, n = matchPool.size() - 1; i < n; i = i + 2 ){
            pObjNow = Abc_AigMux( (Abc_Aig_t * )pNtkQbf->pManFunc, pObjVar, matchPool[i + 1], matchPool[i] );
            tmpPool.push_back( pObjNow );
        }
        if( !(matchPool.size() % 2 == 0) ){
            tmpPool.push_back( matchPool[ matchPool.size() - 1 ] );
        }
        matchPool = tmpPool;
        Bmatch_ConstructMUX( matchPool, pObjOut, pNtkQbf, level, prefix );
	}

	delete pName;
	assert( matchPool.empty() );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END