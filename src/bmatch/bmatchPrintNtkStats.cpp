/**CppFile**************************************************************
 
  FileName    [bmatchPrintNtkStats.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Print Ntk Stats]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#include "bmatch.h"

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

void Bmatch_PrintNtkStats       ( Abc_Ntk_t * pNtk );
void Bmatch_PrintIO             ( Abc_Ntk_t * pNtk );

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

void Bmatch_PrintNtkStats( Abc_Ntk_t * pNtk )
{
    int nPIs, nPOs, nNodes;
    Abc_Obj_t * pObj;
    int i;
    nPIs = Abc_NtkPiNum( pNtk );
    nPOs = Abc_NtkPoNum( pNtk );
    nNodes = Abc_NtkNodeNum( pNtk );
    printf("number of PIs   = %d \n", nPIs);
    printf("number of POs   = %d \n", nPOs);
    printf("number of Nodes = %d \n", nNodes);

    Abc_NtkForEachObj( pNtk, pObj, i){
        printf("Name %15s : ", Abc_ObjName( pObj ));
        Abc_ObjPrint( stdout, pObj );
    }
}

void Bmatch_PrintIO( Abc_Ntk_t * pNtk )
{
    int i;
    Abc_Obj_t * pObj;

    Abc_NtkForEachPi( pNtk, pObj, i){
        printf("PI %15s : ", Abc_ObjName( pObj) );
        Abc_ObjPrint( stdout, pObj );
    }

    Abc_NtkForEachPo( pNtk, pObj, i){
        printf("PO %15s : ", Abc_ObjName( pObj) );
        Abc_ObjPrint( stdout, pObj );
    }
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
