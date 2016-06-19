/**CppFile**************************************************************

  FileName    [manual_api.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [ Boolean Matching NP3 ]
  
  Synopsis    [ Create an API for usage ]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/
#ifndef MANUAL_API
#define MANUAL_API


////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "base/abc/abc.h"
#include "sat/cnf/cnf.h"

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_HEADER_START

int Abc_NtkDSat( Abc_Ntk_t * pNtk, ABC_INT64_T nConfLimit, ABC_INT64_T nInsLimit, int nLearnedStart, int nLearnedDelta, int nLearnedPerce, int fAlignPol, int fAndOuts, int fNewSolver, int fVerbose );
Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );

void Abc_NtkModelToVector( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues );
void Abc_NtkVectorClearPars( Vec_Int_t * vPiValues, int nPars );
void Abc_NtkVectorClearVars( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nPars );
void Abc_NtkVectorPrintPars( Vec_Int_t * vPiValues, int nPars );
void Abc_NtkVectorPrintVars( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues, int nPars );

ABC_NAMESPACE_HEADER_END

#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////