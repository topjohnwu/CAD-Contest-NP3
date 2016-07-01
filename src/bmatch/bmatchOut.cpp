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

void Bmatch_Parse( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, bool outMuxOn2 );
int Bmatch_Output( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, const int & maxScore, const char* filename );

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Bmatch_Parse( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkQbf, int * results, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, bool outMuxOn2 )
{
    string previous = "";
    unsigned num = 0, bit = 0, cur = 0;
    bool x = true;

    // For loops
    int i;
    Abc_Obj_t * pObj;

    //cout << "Type\tName\t\tResult\t\t"
    // << "Spec\tMatch\tc1_id\tc2_id"
    //<< endl;

    Abc_NtkForEachPi( pNtkQbf, pObj, i )
    {
        string objStr = Abc_ObjName( pObj );
        string objName = objStr.substr( 2, objStr.find_last_of('_') - 2 );
        bool b = results[i];
        if( previous == "" ) previous = objName;
        if( previous == objName )
            num += ( b << bit++ );
        else
        {
            unsigned id;
            if( x ) {
                id = ((num >> 2) >= Abc_NtkPiNum( pNtk1 )) ? (Abc_NtkPiNum( pNtk1 ) - 1) : (num >> 2);
                if(num & 1) (*constGroup).push_back( Node(cur++, num & 2) );
                else inGroup[ id ].push_back( Node(cur++, num & 2) );
            }
            else {
                id = ((num >> 2) >= Abc_NtkPoNum( pNtk1 )) ? (Abc_NtkPiNum( pNtk1 ) - 1) : (num >> 2);
                if( outMuxOn2 ) {
                    if( !(num & 1) ) outGroup[ cur ].push_back( Node(id, num & 2) );
                    ++cur;
                }
                else if( !(num & 1) ) outGroup[ id ].push_back( Node(cur++, num & 2) );
            }

            // Debug message
            //cout 
            //<< (x?"PI\t":"PO\t")
            //<< (x||!outMuxOn2?"Cir 2:\t":"Cir 1:\t")
            //<< previous + "\t" 
            //<< (x||!outMuxOn2?"Cir 1:\t":"Cir 2:\t") << ((num & 2)?"!":"") << ((num & 1)?(x?"CONST0\t":"NULL\t"): Abc_ObjName(x?(Abc_NtkPi(pNtk1,id)):(Abc_NtkPo(outMuxOn2?pNtk2:pNtk1,id))))
            // << ((num & 1)?(x?"CONST\t":"NULL\t"): "\t") 
            // << ((num & 2)?"!":"") << id << "\t" 
            // << ((x||!outMuxOn2)?id:(cur-1)) << "\t" 
            // << ((!x&&outMuxOn2)?id:(cur-1)) << "\t"
            //<< endl;

            previous = "";
            num = bit = 0;
            if ( x && objStr[0] == 'y' ) { x = false; cur = 0; }
            if (objStr[0] == 'z' ) break;
            --i;
        }
    }
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int Bmatch_Output( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, vector<Node> * inGroup, vector<Node> * outGroup, vector<Node> * constGroup, const int & maxScore, const char* filename )
{    
    // For loops
    int i, score = 0;
    Abc_Obj_t * pObj;
 
    Abc_NtkForEachPo( pNtk1, pObj, i )
    {
        if(outGroup[i].size()){
            score += 11 + outGroup[i].size();
        }
    }    
    if( score <= maxScore )
        return score;

    ofstream f(filename);
    // ostream & f = cout; // For debugging
    
    Abc_NtkForEachPo( pNtk1, pObj, i )
    {
        if(outGroup[i].size())
        {
            f << "OUTGROUP" << endl;
            f << "1 + " << Abc_ObjName( pObj ) << endl;
            for (int j = 0; j < outGroup[i].size(); ++j)
                f << "2 " << (outGroup[i][j].inv ? "- " : "+ ") <<  Abc_ObjName( Abc_NtkPo(pNtk2, outGroup[i][j].id) ) << endl;
            f << "END" << endl;
        }
    }

    Abc_NtkForEachPi( pNtk1, pObj, i )
    {
        if(inGroup[i].size())
        {
            f << "INGROUP" << endl;
            f << "1 + " << Abc_ObjName( pObj ) << endl;
            for (int j = 0; j < inGroup[i].size(); ++j)
                f << "2 " << (inGroup[i][j].inv ? "- " : "+ ") <<  Abc_ObjName( Abc_NtkPi(pNtk2, inGroup[i][j].id) ) << endl;
            f << "END" << endl;
        }
    }

    if((*constGroup).size())
    {
        f << "CONST0GROUP" << endl;
        for (int j = 0; j < (*constGroup).size(); ++j)
            f << "2 " << ((*constGroup)[j].inv ? "- " : "+ ") << Abc_ObjName( Abc_NtkPi(pNtk2, (*constGroup)[j].id) ) << endl;
        f << "END" << endl;
    }

    if (f.is_open()) cout << "Create \"" << filename << "\" success!" << endl;
    f.close();
    return score;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

ABC_NAMESPACE_IMPL_END
