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

#include "base/main/mainInt.h"
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

void Bmatch_UpdateMatchPair ( PI_PO_INFO * pInformation,
                              vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
                              vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
                              Abc_Ntk_t * pNtkQbf, int * results );
void Bmatch_PrintAnswer     ( Abc_Frame_t * pAbc, int verbose );
void Bmatch_OutputToMatchOut         ( Abc_Frame_t * pAbc, const char *, int verbose );
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

void Bmatch_UpdateMatchPair ( PI_PO_INFO * pInformation,
                              vector< pair < suppWrap *, vector< suppWrap * > > > & fomap,
                              vector< pair < suppWrap *, vector< suppWrap * > > > & gimap,
                              Abc_Ntk_t * pNtkQbf, int * results )
{
    pair < suppWrap *, vector< suppWrap * > > tmpPair;
    vector < suppWrap * > tmpVector;
    string activeObjName = "";
    Abc_Obj_t * pObj;
    int i, pickNum = 0, bits = 0;
    char state = 'x';
    bool value, firFlag = false, secFlag = false, answerSign;
    suppWrap * answer;

cout << "results : ";
for( int i = 0, n = Abc_NtkPiNum(pNtkQbf); i < n; ++i ) {
    cout << *(results + i);
}
cout << endl;

    Abc_NtkForEachPi( pNtkQbf, pObj, i ) {
        string objStr = Abc_ObjName( pObj );
        string objName = objStr.substr( 2, objStr.find_last_of('_') - 2 );
        value = results[i];
        if( activeObjName == "" )       activeObjName = objName;
        if( activeObjName == objName && state == objStr[0] )  pickNum += ( value << bits++ );
        else {
cout << "pickNum : " << pickNum << endl;
            answerSign = pickNum % 2;
            pickNum   = pickNum / 2;
cout << "Name  : " << activeObjName << endl;
cout << "state : " << state << endl;
cout << "answerSign : " << answerSign << endl;
cout << "pickNum : " << pickNum << endl;
// Abc_AigNodeIsConst
            if( state == 'x' ) {
                for( int i = 0, n = fomap.size(); i < n; ++i ) {
                    string nodeName = Abc_ObjName( fomap[i].first->thisObj );
                    if( activeObjName == nodeName ) {

                cout << "Got ans 1" << endl;
                        firFlag = true;
                        answer = fomap[i].second[pickNum];
                        answer->status = (answerSign ? POSITIVE : NEGATIVE );
                        for( int j = 0, m = pInformation->_f_match.size(); j < m; ++j ) {
                            string storeName = Abc_ObjName( pInformation->_f_match[j].first->thisObj );
                            if( activeObjName == storeName ) {
                                secFlag = true;
                                pInformation->_f_match[j].second.push_back( answer );
                            }
                        }
                        if( !secFlag ) {
                            secFlag = true;
                            tmpVector.push_back( answer );
                            tmpPair = make_pair( fomap[i].first, tmpVector );
                            pInformation->_f_match.push_back(tmpPair);
                            tmpVector.clear();
                        }
                    }
                }
            }
            else if( state == 'y' ) {
                for( int i = 0, n = gimap.size(); i < n; ++i ) {
                    string nodeName = Abc_ObjName( gimap[i].first->thisObj );
                    if( activeObjName == nodeName ) {
                        firFlag = true;
                        answer = (pickNum == 0) ? pInformation->_x[pInformation->_x.size() - 1] :
                                                ( pickNum - 1 > gimap[i].second.size() ? gimap[i].second[gimap[i].second.size() - 1] : gimap[i].second[pickNum - 1] );
                        string answerName = Abc_ObjName( answer->thisObj );
                        gimap[i].first->status = (answerSign ? POSITIVE : NEGATIVE );
                        for( int j = 0, m = pInformation->_x_match.size(); j < m; ++j ) {
                            string storeName = Abc_ObjName( pInformation->_x_match[j].first->thisObj );
                            if( answerName == storeName ) {
                                secFlag = true;
                                pInformation->_x_match[j].second.push_back( gimap[i].first );
                            }
                        }
                        if( !secFlag ) {
                            secFlag = true;
                            tmpVector.push_back( gimap[i].first );
                            tmpPair = make_pair( answer, tmpVector );
                            pInformation->_x_match.push_back(tmpPair);
                            tmpVector.clear();
                        }
                    }
                }
            }

            assert( firFlag && secFlag );
            firFlag = false; secFlag = false;
            if( objStr[0] == 'z' ) break;
            if( objStr[0] == 'y' ) state = 'y';
            activeObjName.clear();
            bits = 0;
            pickNum = ( value << bits++ );
        }
    }
}


void Bmatch_PrintAnswer( Abc_Frame_t * pAbc, int verbose )
{
    cout << "Print cir1 PO fi matchings: " << endl;
    Bmatch_PrintMatchPairs( ((PI_PO_INFO *)pAbc->pInformation)->_f_match );
    cout << "Print cir1 PI xi matchings: " << endl;
    Bmatch_PrintMatchPairs( ((PI_PO_INFO *)pAbc->pInformation)->_x_match );
}

void Bmatch_OutputToMatchOut         ( Abc_Frame_t * pAbc, const char * fileName, int verbose )
{
    PI_PO_INFO * pInformation = (PI_PO_INFO *)pAbc->pInformation;
    fstream file;
    file.open( fileName, ios::out );
    if( file.is_open() ) {
        for( int i = 0, n = pInformation->_f_match.size(); i < n; ++i ) {
            file << "OUTGROUP" << endl;
            file << "1 + " << Abc_ObjName( pInformation->_f_match[i].first->thisObj ) << endl;
            for ( int j = 0, m = pInformation->_f_match[i].second.size(); j < m; ++j ) {
                file << "2 " << ( (pInformation->_f_match[i].second[j]->status == NEGATIVE) ? "- " : "+ ")
                             <<  Abc_ObjName( pInformation->_f_match[i].second[j]->thisObj ) << endl;
            }
            file << "END" << endl;
        }
        for( int i = 0, n = pInformation->_x_match.size(); i < n; ++i ) {
        if( pInformation->_x_match[i].first == pInformation->_x[pInformation->_x.size() - 1] ) {
            file << "CONST0GROUP" << endl;
            for ( int j = 0, m = pInformation->_x_match[i].second.size(); j < m; ++j ) {
                file << "2 " << ( (pInformation->_x_match[i].second[j]->status == NEGATIVE) ? "+ " : "- ")
                             <<  Abc_ObjName( pInformation->_x_match[i].second[j]->thisObj ) << endl;
            }
            file << "END" << endl;
        }
        else {
            file << "INGROUP" << endl;
            file << "1 + " << Abc_ObjName( pInformation->_x_match[i].first->thisObj ) << endl;
            for ( int j = 0, m = pInformation->_x_match[i].second.size(); j < m; ++j ) {
                file << "2 " << ( (pInformation->_x_match[i].second[j]->status == NEGATIVE) ? "- " : "+ ")
                             <<  Abc_ObjName( pInformation->_x_match[i].second[j]->thisObj ) << endl;
            }
            file << "END" << endl;
        }
        }
    }
}

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
