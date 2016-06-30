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

class Node;

extern void Bmatch_Resync               ( Abc_Ntk_t * &pNtk );
extern void Bmatch_PrintNtkStats        ( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintIO              ( Abc_Ntk_t * pNtk );
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

