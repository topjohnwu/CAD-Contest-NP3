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
#include "manual_api.h"
#include "base/main/mainInt.h"
#include <list>
#include <vector>
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
// For new structure !!
extern void Bmatch_PrepNtks    			( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );
/////
class Node;

extern void Bmatch_Resync               ( Abc_Ntk_t * &pNtk );
extern void Bmatch_PrintNtkStats        ( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintIO              ( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintObj 			( Abc_Ntk_t * pNtk );
extern void Bmatch_Print3Circuit   	    ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtk3 );
extern Abc_Ntk_t * Bmatch_PrepareQbfNtk ( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, const int & ILP_constraint, const bool & muxOnCir2 );
extern bool        Bmatch_SolveQbf      ( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nInputs, int nItersMax, int fVerbose );
extern void Bmatch_Parse               	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, bool outMuxOn2 );
extern int  Bmatch_Output				( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, const int & maxScore, const char* filename );

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////
///                       CLASS DEFINITIONS                          ///
////////////////////////////////////////////////////////////////////////
struct suppWrap {
	int 					numOfInfluence;
	Abc_Obj_t * 			thisObj;
	vector< suppWrap * > 	outputs;
	vector< suppWrap * > 	unates;
	vector< suppWrap * > 	binates; 
};

typedef struct PI_PO_INFO_ {
	list< Abc_Obj_t * > 		* _f_match, * _x_match; // For answer !!

	suppWrap 			 		* _f, * _g, * _x, * _y;
	~PI_PO_INFO_(){
		delete [] _f_match; delete [] _x_match;
		delete [] _f; delete [] _g; delete [] _x; delete [] _y;
	}
} PI_PO_INFO;

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

