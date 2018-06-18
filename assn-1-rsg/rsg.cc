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

int main(int argc, char *argv[])
{
    if (argc == 1) {
        cerr << "You need to specify the name of a grammar file." << endl;
        cerr << "Usage: rsg <path to grammar text file>" << endl;
        return 1; // non-zero return value means something bad happened
    }

    ifstream grammarFile(argv[1]);
    if (grammarFile.fail()) {
        cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
        return 2; // each bad thing has its own bad return value
    }

    // things are looking good...
    map<string, Definition> grammar;
    readGrammar(grammarFile, grammar);
    generateAndPrintContent(grammar);


    return 0;
}
