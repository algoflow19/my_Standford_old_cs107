/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */
 
#include <map>
#include <fstream>
#include "definition.h"
#include "production.h"
#include<stdlib.h>
#include<cstring>
using namespace std;

const int maxLineLength=55;


/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream& infile, map<string, Definition>& grammar)
{
  while (true) {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof()) return;  // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}

static void storeWords(string non_terminal, map<string, Definition>& grammar,std::vector<string> & resultList){
    if(non_terminal[0]!='<'||non_terminal[non_terminal.length()-1]!='>'){
        cerr<<"Error non_terminal format!"<<endl;
        exit(3);
    }
    if(grammar.count(non_terminal)==0){
        cerr<<non_terminal<<" is used without def"<<endl;
        exit(4);
    }
    Production randomProduction=grammar.at(non_terminal).getRandomProduction();
    for( vector<string>::iterator it=randomProduction.begin();it!=randomProduction.end();it++ ){
        if(it->at(0)=='<')
            storeWords(*it,grammar,resultList);
        else {
            resultList.push_back(*it);
        }
    }
}

static void printHead(int versionNum){
    cout<<"Version #"<<versionNum<<": ---------------------------"<<endl;
}

static void generateAndPrintContent(map<string, Definition>& grammar){
    std::vector<string> resultList;

    int wordInLine;
    for(int i=0;i<3;i++){
        resultList.clear();
        storeWords("<start>",grammar,resultList);

        printHead(i+1);
        cout<<"   ";
        wordInLine=3;
        for(string s : resultList) {
            if(s.length()+wordInLine>maxLineLength){
                cout<<endl;
                wordInLine=0;
            }
            if(s!=","&&s!="."){
                cout<<" ";
                wordInLine++;
            }
            cout<<s;
            wordInLine+=s.length();
        }
        cout<<endl<<endl;
    }

}

/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  You're to update and decompose the main function to print
 * three randomly generated sentences, as illustrated by the sample
 * application.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */

void swap(void* vp1,void* vp2,int size){
    void* tmpv=malloc(size);
    memcpy(tmpv,vp1,size);
    memcpy(vp1,vp2,size);
    memcpy(vp2,tmpv,size);
}

int lsearch(void* target,void* toSearch,int n,int elemSize){
    for(int i=0;i<n;i++){
        void* tmpV=(char*)toSearch+i*elemSize;
        if(memcmp(target,tmpV,elemSize)==0)
            return i;
    }
    return -1;
}

struct cVector
{
public:
    cVector(int elementSize):count{0},capacity{4},elemSize{elementSize}
    {
        storePlace=malloc(capacity*elementSize);
    }

    void push_back(void* newElement){
        if(count==capacity){
            void* tmpStorePlace=malloc(elemSize*capacity*2);
            memcpy(tmpStorePlace,storePlace,capacity*elemSize);
            free(storePlace);
            storePlace=tmpStorePlace;
        }
        memcpy((char*)storePlace+count*elemSize,newElement,elemSize);
        count++;
    }
    void eraserAt(int index){
        if(index+1>count||index<0){
            cerr<<" call to eraser the "<<index<<endl;
        }
        if(count!=index+1)
        memcpy((char*)storePlace+index*elemSize,(char*)storePlace+(index+1)*elemSize,elemSize*(count-index-1));
        count--;
        if(count<=capacity/4){
            void* tmpStorePlace=malloc(elemSize*capacity/2);
            memcpy(tmpStorePlace,storePlace,count*elemSize);
            free(storePlace);
            storePlace=tmpStorePlace;
        }

    }
    int find(void * toMatch){
        return lsearch(toMatch,storePlace,count,elemSize);
    }
    int size(){
        return count;
    }



private:

    void* storePlace;
    int count;
    int capacity;
    int elemSize;
};

int main(int argc, char *argv[])
{
    int a=3;
    int b=5;
    int zero=0;
    /*
    swap(&a,&b,sizeof(int));
    cout<<a<<endl;
    cout<<b<<endl;

    int toSearch[]={1,2,3,4,5};
    cout<<lsearch(&b,toSearch,5,sizeof(int))<<endl;
    cout<<lsearch(&a,toSearch,5,sizeof(int))<<endl;
    cout<<lsearch(&zero,toSearch,5,sizeof(int))<<endl;
    */
    int toAssign;
    cVector v(sizeof(int));

    v.push_back( &(toAssign=1));
    v.push_back( &(toAssign=2));
    v.push_back( &(toAssign=3));
    v.push_back( &(toAssign=4));
    v.push_back( &(toAssign=5));
    v.push_back( &(toAssign=6));

    cout<<v.size()<<endl;
    cout<<v.find(&(toAssign=1))<<endl;
    cout<<v.find(&(toAssign=2))<<endl;
    cout<<v.find(&(toAssign=3))<<endl;
    cout<<v.find(&(toAssign=4))<<endl;
    cout<<v.find(&(toAssign=5))<<endl;
    cout<<v.find(&(toAssign=6))<<endl;


    cout<<"testEraser"<<endl;
    v.eraserAt(0);
    v.eraserAt(0);
    v.eraserAt(0);
    v.eraserAt(0);
    v.eraserAt(0);
    cout<<v.find(&(toAssign=6))<<endl;

    return 0;
}
