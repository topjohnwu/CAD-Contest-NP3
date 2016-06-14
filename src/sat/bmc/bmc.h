/**CFile****************************************************************

  FileName    [bmc.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [SAT-based bounded model checking.]

  Synopsis    [External declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: bmc.h,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/
 
#ifndef ABC___sat_bmc_BMC_h
#define ABC___sat_bmc_BMC_h


////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "aig/saig/saig.h"
#include "aig/gia/gia.h"

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_HEADER_START

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

// unrolling manager 
typedef struct Unr_Man_t_ Unr_Man_t;

typedef struct Saig_ParBmc_t_ Saig_ParBmc_t;
struct Saig_ParBmc_t_
{
    int         nStart;         // starting timeframe
    int         nFramesMax;     // maximum number of timeframes 
    int         nConfLimit;     // maximum number of conflicts at a node
    int         nConfLimitJump; // maximum number of conflicts after jumping
    int         nFramesJump;    // the number of tiemframes to jump
    int         nTimeOut;       // approximate timeout in seconds
    int         nTimeOutGap;    // approximate timeout in seconds since the last change
    int         nTimeOutOne;    // timeout per output in multi-output solving
    int         nPisAbstract;   // the number of PIs to abstract
    int         fSolveAll;      // does not stop at the first SAT output
    int         fStoreCex;      // enable storing CEXes in the MO mode
    int         fUseBridge;     // use bridge interface
    int         fDropSatOuts;   // replace sat outputs by constant 0
    int         nFfToAddMax;    // max number of flops to add during CBA
    int         fSkipRand;      // skip random decisions
    int         fNoRestarts;    // disables periodic restarts
    int         nLearnedStart;  // starting learned clause limit
    int         nLearnedDelta;  // delta of learned clause limit
    int         nLearnedPerce;  // ratio of learned clause limit
    int         fVerbose;       // verbose 
    int         fNotVerbose;    // skip line-by-line print-out 
    char *      pLogFileName;   // log file name
    int         fSilent;        // completely silent
    int         iFrame;         // explored up to this frame
    int         nFailOuts;      // the number of failed outputs
    int         nDropOuts;      // the number of dropped outputs
    abctime     timeLastSolved; // the time when the last output was solved
    int(*pFuncOnFail)(int,Abc_Cex_t*); // called for a failed output in MO mode
};

 
typedef struct Bmc_AndPar_t_ Bmc_AndPar_t;
struct Bmc_AndPar_t_
{
    int         nStart;         // starting timeframe
    int         nFramesMax;     // maximum number of timeframes 
    int         nFramesAdd;     // the number of additional frames
    int         nConfLimit;     // maximum number of conflicts at a node
    int         nTimeOut;       // timeout in seconds
    int         nLutSize;       // LUT size for cut computation
    int         fLoadCnf;       // dynamic CNF loading
    int         fDumpFrames;    // dump unrolled timeframes
    int         fUseSynth;      // use synthesis
    int         fUseOldCnf;     // use old CNF construction
    int         fVerbose;       // verbose 
    int         fVeryVerbose;   // very verbose 
    int         fNotVerbose;    // skip line-by-line print-out 
    int         iFrame;         // explored up to this frame
    int         nFailOuts;      // the number of failed outputs
    int         nDropOuts;      // the number of dropped outputs
};
  
typedef struct Bmc_BCorePar_t_ Bmc_BCorePar_t;
struct Bmc_BCorePar_t_
{
    int         iFrame;         // timeframe
    int         iOutput;        // property output
    int         nTimeOut;       // timeout in seconds
    char *      pFilePivots;    // file name with AIG IDs of pivot objects
    char *      pFileProof;     // file name to write the resulting proof
    int         fVerbose;       // verbose output
};

typedef struct Bmc_MulPar_t_ Bmc_MulPar_t;
struct Bmc_MulPar_t_
{
    int         TimeOutGlo;
    int         TimeOutLoc;
    int         TimeOutInc;
    int         TimeOutGap;
    int         TimePerOut;
    int         fUseSyn;
    int         fDumpFinal;
    int         fVerbose;
    int         fVeryVerbose;
};

typedef struct Bmc_ParFf_t_ Bmc_ParFf_t;
struct Bmc_ParFf_t_
{
    char *     pFileName;
    char *     pFormStr;
    int        Algo;
    int        fComplVars;
    int        fStartPats;
    int        nTimeOut;
    int        nIterCheck;
    int        nCardConstr;
    int        fNonStrict;
    int        fBasic;
    int        fFfOnly;
    int        fDump;
    int        fDumpDelay;
    int        fDumpUntest;
    int        fDumpNewFaults;
    int        fVerbose;
};

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== bmcBCore.c ==========================================================*/
extern void              Bmc_ManBCorePerform( Gia_Man_t * pGia, Bmc_BCorePar_t * pPars );
/*=== bmcBmc.c ==========================================================*/
extern int               Saig_ManBmcSimple( Aig_Man_t * pAig, int nFrames, int nSizeMax, int nBTLimit, int fRewrite, int fVerbose, int * piFrame, int nCofFanLit );
/*=== bmcBmc2.c ==========================================================*/
extern int               Saig_BmcPerform( Aig_Man_t * pAig, int nStart, int nFramesMax, int nNodesMax, int nTimeOut, int nConfMaxOne, int nConfMaxAll, int fVerbose, int fVerbOverwrite, int * piFrames, int fSilent );
/*=== bmcBmc3.c ==========================================================*/
extern void              Saig_ParBmcSetDefaultParams( Saig_ParBmc_t * p );
extern int               Saig_ManBmcScalable( Aig_Man_t * pAig, Saig_ParBmc_t * pPars );
/*=== bmcBmcAnd.c ==========================================================*/
extern int               Gia_ManBmcPerform( Gia_Man_t * p, Bmc_AndPar_t * pPars );
/*=== bmcCexCare.c ==========================================================*/
extern Abc_Cex_t *       Bmc_CexCareExtendToObjects( Gia_Man_t * p, Abc_Cex_t * pCex, Abc_Cex_t * pCexCare );
extern Abc_Cex_t *       Bmc_CexCareMinimize( Aig_Man_t * p, Abc_Cex_t * pCex, int fCheck, int fVerbose );
extern void              Bmc_CexCareVerify( Aig_Man_t * p, Abc_Cex_t * pCex, Abc_Cex_t * pCexMin, int fVerbose );
/*=== bmcCexCut.c ==========================================================*/
extern Gia_Man_t *       Bmc_GiaTargetStates( Gia_Man_t * p, Abc_Cex_t * pCex, int iFrBeg, int iFrEnd, int fCombOnly, int fGenAll, int fAllFrames, int fVerbose );
extern Aig_Man_t *       Bmc_AigTargetStates( Aig_Man_t * p, Abc_Cex_t * pCex, int iFrBeg, int iFrEnd, int fCombOnly, int fGenAll, int fAllFrames, int fVerbose );
/*=== bmcCexMin.c ==========================================================*/
extern Abc_Cex_t *       Saig_ManCexMinPerform( Aig_Man_t * pAig, Abc_Cex_t * pCex );
/*=== bmcCexTool.c ==========================================================*/
extern void              Bmc_CexPrint( Abc_Cex_t * pCex, int nInputs, int fVerbose );
extern int               Bmc_CexVerify( Gia_Man_t * p, Abc_Cex_t * pCex, Abc_Cex_t * pCexCare );
/*=== bmcICheck.c ==========================================================*/
extern void              Bmc_PerformICheck( Gia_Man_t * p, int nFramesMax, int nTimeOut, int fEmpty, int fVerbose );
extern Vec_Int_t *       Bmc_PerformISearch( Gia_Man_t * p, int nFramesMax, int nTimeOut, int fReverse, int fBackTopo, int fDump, int fVerbose );
/*=== bmcUnroll.c ==========================================================*/
extern Unr_Man_t *       Unr_ManUnrollStart( Gia_Man_t * pGia, int fVerbose );
extern Gia_Man_t *       Unr_ManUnrollFrame( Unr_Man_t * p, int f );
extern void              Unr_ManFree( Unr_Man_t * p );


ABC_NAMESPACE_HEADER_END



#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

