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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );

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

void Bmatch_PrepNtks        ( Abc_Frame_t * pAbc, Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 )
{
    pAbc->pSave1    = new PI_PO_INFO;
    PI_PO_INFO      * pInformation;
    Abc_Obj_t       * pObj;
    char            * pName;
    int             i;

    pName = "cir1";
    Abc_NtkSetName( pNtk1, pName);
    Abc_NtkSetStep( pNtk1, 1 );
    pName = "cir2";
    Abc_NtkSetName( pNtk2, pName);
    Abc_NtkSetStep( pNtk2, 2 );

    Abc_NtkSetBackup( pNtk1, pNtk2 );

    Abc_NtkOrderObjsByName( pNtk1, 0 );
    Abc_NtkOrderObjsByName( pNtk2, 0 );

    Bmatch_PrintNtkStats(pNtk1);
    Bmatch_PrintNtkStats(pNtk2);

    pInformation = ( PI_PO_INFO * ) pAbc->pSave1;
    pInformation->_f_match = new list< Abc_Obj_t * > [ Abc_NtkPoNum(pNtk1) ];
    pInformation->_x_match = new list< Abc_Obj_t * > [ Abc_NtkPiNum(pNtk1) + 1];
    pInformation->_f = new suppWrap [ Abc_NtkPoNum(pNtk1) ];
        Abc_NtkForEachPo( pNtk1, pObj, i ) { ((pInformation->_f)[i]).thisObj = pObj; }
    pInformation->_g = new suppWrap [ Abc_NtkPoNum(pNtk2) ];
        Abc_NtkForEachPo( pNtk2, pObj, i ) { ((pInformation->_g)[i]).thisObj = pObj; }
    pInformation->_x = new suppWrap [ Abc_NtkPiNum(pNtk1) ];
        Abc_NtkForEachPi( pNtk1, pObj, i ) { ((pInformation->_x)[i]).thisObj = pObj; }
    pInformation->_y = new suppWrap [ Abc_NtkPiNum(pNtk2) ];
        Abc_NtkForEachPi( pNtk1, pObj, i ) { ((pInformation->_y)[i]).thisObj = pObj; }

    Abc_FrameSetCurrentNetwork( pAbc, pNtk1 );
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
