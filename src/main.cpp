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
    
    //cout << cmdstring;
    
    vector<string> tokens;
    tokenizer<> tok(cmdstring);
    for (tokenizer<>::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        tokens.push_back(*iter);
    //    cout << *iter << endl;
    }
    
    //const char *str = cmdstring.c_str();
    //char *cmd;
    // char **arg = NULL; 
    // cmd = strtok (const_cast<char*>(str)," ;");
    // arg[0] = strtok (const_cast<char*>(str)," ;"); 
    
    //arg = arg + ' ' + strtok (const_cast<char*>(str)," ;"); 
    
    
    
    
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
