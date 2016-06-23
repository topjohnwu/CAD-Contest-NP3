/**CppFile**************************************************************
 
  FileName    [bmatchOut.cpp] 

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Boolean Matching NP3]
  
  Synopsis    [Output Ntk Stats]

  Author      [topjohnwu / Danny]
   
  Affiliation [NTUEE]

  Date        []

  Revision    []

***********************************************************************/

#include "bmatch.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
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

void Bmatch_Output( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results );
class Node;

#ifdef __cplusplus
}
#endif
////////////////////////////////////////////////////////////////////////
///                       CLASS DEFINITIONS                          ///
////////////////////////////////////////////////////////////////////////
class Node
{
public:
    string name;
    bool inv;
    Node( string s, bool b) : name(s), inv(b) {};
};

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_Output( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results )
{
    cout << "Type\tName\tSpec\tMatch\t" << endl;
    int i;
    string pre = "";
    unsigned num = 0, bit = 0;
    bool x = true;

    Abc_Obj_t * pObj;
    vector<Node>* inGroup = new vector<Node>[ Abc_NtkPiNum( pNtk1 ) ];
    vector<Node>* outGroup = new vector<Node>[ Abc_NtkPoNum( pNtk1 ) ];
    vector<Node> constGroup;

    Abc_NtkForEachPi( pNtkQbf, pObj, i )
    {
        string objStr = Abc_ObjName( pObj );
        string objName = objStr.substr( 2, objStr.find_last_of('_') - 2 );
        bool b = results[i];
        if( pre == "" ) pre = objName;
        if( pre == objName )
            num += ( b << bit++ );
        else
        {
            if(x) {
                if(num & 1) constGroup.push_back( Node(pre, num & 2) );
                else inGroup[ (num >> 2) ].push_back( Node(pre, num & 2) );
            }
            else
                if( !(num & 1) ) outGroup[ (num >> 2) ].push_back( Node(pre, num & 2) );
            cout << (x?"in\t":"out\t") << pre + "\t" << ((num & 1)?(x?"CONST\t":"NULL\t"): "\t") << ((num & 2)?"!":"") << (num >> 2) << endl;
            pre = "";
            num = bit = 0;
            if (objStr[0] == 'y' ) x = false;
            if (objStr[0] == 'z' ) break;
            --i;
        }
    }
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
