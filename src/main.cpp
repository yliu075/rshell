#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <string>
#include <boost/tokenizer.hpp>
#include <vector>

using namespace std;
using namespace boost;

void rshell()
{
    //char host[64];
    //char *login = getlogin();
    //gethostname(host,sizeof host);
    
    string cmdstring;
    
    //cout << login << '@' << host << "$ ";
    cout << "$ ";
    getline(cin, cmdstring);
    if (cmdstring.size() == 0) rshell();
    if (cmdstring.find('#') != string::npos) {
        cmdstring.erase(cmdstring.find('#'), cmdstring.size() - 1);
    }
    while (cmdstring.at(0) == ' ') {
        cmdstring.erase(0,1);
    }
    if (cmdstring.size() > 3 && cmdstring.substr(0,4) == "exit") exit(0);
    string tempString;
    for (size_t t = 0; t < cmdstring.size(); t++) {
        if (cmdstring.at(t) != ' ') {
            tempString = tempString + cmdstring.at(t);
        }
    }
    cmdstring = tempString;
    cout << "cmdstring: "<< cmdstring << endl;
    vector<string> tokens;
    char_separator<char> dash("","-");
    tokenizer<char_separator<char> > tok(cmdstring, dash);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        tokens.push_back(*iter);
        cout << *iter << endl;
    }
    for (size_t i = 0; i < tokens.size(); i++) {
        cout << tokens.at(i) << endl;
    }
    //const char *str = cmdstring.c_str();
    //char *cmd;
    // char **arg = NULL; 
    // cmd = strtok (const_cast<char*>(str)," ;");
    // arg[0] = strtok (const_cast<char*>(str)," ;"); 
    //arg = arg + ' ' + strtok (const_cast<char*>(str)," ;"); 
    vector<string> tokensWithFlags;
    
    for (size_t h = 0; h < tokens.size(); h++) {
        if (tokens.at(h) == "-" &&  (h + 1) < tokens.size()) {
            string temp = tokens.at(h) + tokens.at(h + 1);
            tokensWithFlags.push_back(temp);
            h++;
        }
        else tokensWithFlags.push_back(tokens.at(h));
    }
    for (size_t i = 0; i < tokensWithFlags.size(); i++) {
        cout << tokensWithFlags.at(i) << endl;
    }
    //const char *cmd = tokens.at(0).c_str();
    char **ARGV = new char*[tokensWithFlags.size() + 1];
    
    for (size_t j = 0; j < tokensWithFlags.size(); j++) {
        ARGV[j] = new char[tokensWithFlags.at(j).size() + 1];
        strcpy(ARGV[j], tokensWithFlags.at(j).c_str());
        ARGV[tokensWithFlags.size()] = 0;
    }
    string CMD = ARGV[0];
    cout << "CMD: " << CMD << endl;
    int pass = execvp(ARGV[0], ARGV);
    perror("error in execvp");
    cout << pass;
    
    
    // int pid = fork();
    
    // if (cmdstring == "exit") {
    //     exit(0);
    // }
    // if (pid == -1) {
    //     perror("fork fail");
    // }
    // else if (pid == 0) {
    //     if (execvp(cmd, argv) != 0) {
    //         perror("error in execvp");
    //     }
    // }
    // else {
    //     cout << "DONE" << endl;
    //     exit(0);
    // }
    
}

int main(int argc, char** argv)
{
    rshell();
    return 0;
}
