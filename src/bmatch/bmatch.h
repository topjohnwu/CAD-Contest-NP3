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

extern void Bmatch_Resync				( Abc_Ntk_t * &pNtk );
extern void Bmatch_PrintNtkStats		( Abc_Ntk_t * pNtk );
extern void Bmatch_PrintIO				( Abc_Ntk_t * pNtk );
extern Abc_Ntk_t * Bmatch_PrepareQbfNtk	( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );
extern bool        Bmatch_SolveQbf      ( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nInputs, int nItersMax, int fVerbose );
extern void Bmatch_Output				( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results );

#ifdef __cplusplus
}
#endif

#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

