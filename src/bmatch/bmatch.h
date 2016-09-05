/**CppFile**************************************************************

  FileName    [bmatch.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Command file.]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#ifndef __BMATCH_H__
#define __BMATCH_H__


////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "base/abc/abc.h"
#include "base/main/main.h"
#include "manual_api.h"
#include <vector>
#include <list>

using namespace std;

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////
 
////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

enum STATUS {
	UNDEFINED,
	POSITIVE,
	NEGATIVE
};

// For new structure !!
extern void Bmatch_PrepNtks    			( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int verbose );
/////
class Node;
struct suppWrap {
	int 					numOfInfluence;
	Abc_Obj_t * 			thisObj;
	vector< suppWrap * > 	funSupp;
	STATUS 					status;
	// vector< suppWrap * > 	unates;
	// vector< suppWrap * > 	binates;
	suppWrap(){ numOfInfluence = 0; thisObj = NULL; status = UNDEFINED; }
	inline bool operator > ( const suppWrap & b ) const {
		return this->numOfInfluence > b.numOfInfluence;
	}
};

typedef struct PI_PO_INFO_ {
	unsigned 						_maxScore;
	unsigned 						_currentScore;
	vector< pair< suppWrap *, vector< suppWrap * > > >
								 	_f_match, _x_match; // For answer !!

	vector< pair< suppWrap *, vector< suppWrap * > > >
								 	_f_already_try; // For picking pairs !!

	vector< suppWrap * >			_f, _g, _x, _y;
	Abc_Ntk_t 						* _pNtk1, * _pNtk2;

	PI_PO_INFO_() { _maxScore = 0;
					_currentScore = 0;
					_pNtk1 = NULL; _pNtk2 = NULL; }
	~PI_PO_INFO_() {
		for( int i = 0; i < _f.size(); ++i ) { delete _f[i]; }
		for( int i = 0; i < _g.size(); ++i ) { delete _g[i]; }
		for( int i = 0; i < _x.size(); ++i ) { delete _x[i]; }
		for( int i = 0; i < _y.size(); ++i ) { delete _y[i]; }
	}
} PI_PO_INFO;

extern void Bmatch_Resync               	( Abc_Ntk_t * &pNtk );
extern void Bmatch_PrintNtkStats        	( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintIO              	( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintObj 				( Abc_Ntk_t * pNtk );
extern void Bmatch_Print3Circuit   	    	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk3 );
extern Abc_Ntk_t * Bmatch_PrepareQbfNtk 	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, const int & ILP_constraint, const bool & muxOnCir2 );
extern bool        Bmatch_SolveQbf      	( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nInputs, int nItersMax, int fVerbose );
extern void Bmatch_Parse               		( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, bool outMuxOn2 );
extern int  Bmatch_Output					( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, const int & maxScore, const char* filename );

extern void Bmatch_SolveByFunSupp  			( Abc_Frame_t * pAbc, int verbose );
extern Abc_Ntk_t * Bmatch_ConstructQbfNtk 	( PI_PO_INFO * pInformation,
							  				  vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
							  				  vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
							  				  int & forAllVarsNum );
extern void Bmatch_PrintMatchPairs 			( vector< pair < suppWrap *, vector< suppWrap * > > > & a );
extern char * Bmatch_NameAddPrefix 			( char * pPrefix, int plength, char * pName );

extern void Bmatch_UpdateMatchPair          ( PI_PO_INFO * pInformation,
                                              vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
                                              vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
                                              Abc_Ntk_t * pNtkQbf, int * results );

extern void Bmatch_PrintAnswer( Abc_Frame_t * pAbc, int verbose );
extern void Bmatch_PrintMatchPairs ( vector< pair < suppWrap *, vector< suppWrap * > > > & a );

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////
///                       CLASS DEFINITIONS                          ///
////////////////////////////////////////////////////////////////////////

inline bool suppWrapComp ( suppWrap * & a, suppWrap * & b ) { return (*a) > (*b); }

class Node
{
public:
    unsigned id;
    bool inv;
    Node( unsigned s, bool b) : id(s), inv(b) {};
};


#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

