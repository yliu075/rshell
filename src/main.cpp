#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <string>
#include <boost/tokenizer.hpp>
#include <vector>
#include <queue>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>

using namespace std;
using namespace boost;

void sigFun(int sig) 
{
    if (sig == SIGINT);
}

void my_CD(string newDir, char currDir[], vector<string> &pathNames)
{
    string currLoc = currDir;
    // cout << "NOW: " << currDir << 
    if (chdir((currDir + '/' + newDir).c_str()) == -1) {
        perror("error in chdir");
        // exit(1);
    }
    if (getcwd(currDir, 128) == NULL) {
        perror("error in getcwd");
        exit(1);
    }
    pathNames.pop_back();
    pathNames.push_back(currDir);
}

void my_Pipes(char **ARGV1, char **ARGV2, vector<string> pathNames)
{
    // cout << "ARGV1: " << ARGV1[0] << " ARGV2: " << ARGV2[0] << endl;
    int status = 0;
    int pid;
    int pid2;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("error in pipe");
        exit(1);
    }
    if ((pid = fork()) == -1) {
        perror("error in fork");
        exit(1);
    }
    else if (pid == 0) {
        // cout << "IN PIPE_1 ARGV1: " << ARGV1[0] << endl
        if (close(fd[0]) == -1) {
            perror("error in close");
            exit(1);
        }
        if (dup2(fd[1], 1) == -1) {
            perror("error in dup2");
            exit(1);
        }
        if (close(fd[1]) == -1) {
            perror("error in close");
            exit(1);
        }
        string ARGV0 = ARGV1[0];
        if (signal(SIGINT, NULL) == SIG_ERR) {
            perror("error in signal");
            exit(1);
        }
        for (size_t k = 0; k < pathNames.size(); k++) {
            if (execv((pathNames[k] + '/' + ARGV0).c_str(), ARGV1) != 0 && k == (pathNames.size() - 1)) {
                perror("error in execv");
                exit(1);
            }
        }

    }
    else {
        // cout << "IN PIPE_1 ARGV1: " << ARGV1[0] << endl
        if ((pid2 = fork()) == -1) {
            perror("error in fork");
            exit(1);
        }
        else if (pid2 == 0) {
            if (close(fd[1]) == -1) {
                perror("error in close");
                exit(1);
            }
            if (dup2(fd[0], 0) == -1) {
                perror("error in dup2");
                exit(1);
            }
            if (close(fd[0]) == -1) {
                perror("error in close");
                exit(1);
            }
            string ARGV0 = ARGV2[0];
            if (signal(SIGINT, NULL) == SIG_ERR) {
                perror("error in signal");
                exit(1);
            }
            for (size_t k = 0; k < pathNames.size(); k++) {
                if (execv((pathNames[k] + '/' + ARGV0).c_str(), ARGV2) != 0 && k == (pathNames.size() - 1)) {
                    perror("error in execv");
                    exit(1);
                }
            }
        }
        if (close(fd[0]) == -1) {
            perror("error in close");
            exit(1);
        }
        if (close(fd[1]) == -1) {
            perror("error in close");
            exit(1);
        }
        if (waitpid (pid2, &status, 0) == -1) {
            perror("error in wait");
            exit(1);
        }
    }
}


int rshell(vector<string> pathNames, char currDir[])
{
    signal(SIGINT, sigFun);
    
    char *login = getpwuid(getuid())->pw_name;
    char host[64];
    gethostname(host,sizeof host);
    // cout << login << '@' << host << "$ ";
    printf("%s\n%s@%s$ ", currDir, login, host);
    // cout << "$ ";
    // delete[] login;
    string cmdstring;
    getline(cin, cmdstring);
    if (cmdstring.size() == 0) {
        return rshell(pathNames, currDir);
    }
    else if (cmdstring == "exit") exit(0);
    if (cmdstring.find('#') != string::npos) {
        if (cmdstring.find('#') == 0) {
            return rshell(pathNames, currDir);
        }
        cmdstring.erase(cmdstring.find('#'), cmdstring.size() - 1);
    }
    while (cmdstring.at(0) == ' ') {
        cmdstring.erase(0,1);
    }
    if (cmdstring.size() > 3 && cmdstring.substr(0,4) == "exit") {
        exit(0);
    }
    string tempString;
    
    for (size_t t = 0; t < cmdstring.size(); t++) {
        if (cmdstring.at(t) != ' ') {
            tempString = tempString + cmdstring.at(t);
        }
        else tempString = tempString + "~";
    }
    
    cmdstring = tempString;
    // cout << "cmdstring: "<< cmdstring << endl;
    // cout << "last char: " << cmdstring.at(cmdstring.size() - 1)<< endl;
    while (cmdstring.size() > 0 && (cmdstring.at(cmdstring.size() - 1) == ';' || cmdstring.at(cmdstring.size() - 1) == '|' || cmdstring.at(cmdstring.size() - 1) == '&' || cmdstring.at(cmdstring.size() - 1) == ' ')) {
        cmdstring.erase(cmdstring.size() - 1, 1);
    }
    if (cmdstring.size() == 0) {
        return rshell(pathNames, currDir);
    }
    vector<string> tokens;
    char_separator<char> dash("~","-|&; ");
    // char_separator<char> dash(" ","-|&;<>");
    tokenizer<char_separator<char> > tok(cmdstring, dash);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        tokens.push_back(*iter);
        // cout << *iter << endl;
    }
    vector<string> tokensWithFlags;
    //vector<vector<string> > totalCMD;
    queue<vector<string> > totalCMD;
    size_t cmdPos = 0;
    for (size_t h = 0; h < tokens.size(); h++) {
        // cerr << "tok now: " << tokens.at(h) << endl;
        if (tokens.at(h).at(0) == '"' && tokens.at(h).at(tokens.at(h).size() - 1) == '"') {
            string temp = tokens.at(h).substr(1, tokens.at(h).size() - 2);
            tokensWithFlags.push_back(temp);
        }
        else if (tokens.at(h).at(0) == '"') {
            string temp = tokens.at(h).substr(1, tokens.at(h).size() - 1);
            // int quoteCounter = 1;
            do {
                h++;
                temp += ' ' + tokens.at(h);
            } while (h + 1 < tokens.size() && (tokens.at(h).find('"') == string::npos));
            if (!(h < tokens.size())) return rshell(pathNames, currDir);
            if (temp.at(temp.size() - 1) == '"') temp.erase(temp.size() - 1);
            // cout << "temp: " << temp << endl;
            tokensWithFlags.push_back(temp);
        }
        else if (tokens.at(h) == "-" &&  (h + 1) < tokens.size()) {
            string temp = tokens.at(h) + tokens.at(h + 1);
            tokensWithFlags.push_back(temp);
            h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == ";") {
            // cout << "CMD; Added " << tokensWithFlags.at(0) <<endl;
            if (tokensWithFlags.at(0) == "exit") exit(0);
            //totalCMD.push_back(tokensWithFlags);
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "&" && tokens.at(h + 1) == "&") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("&&");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "|" && tokens.at(h + 1) == "|") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("||");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "|") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("|");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
        }
        else if ((h + 1) < tokens.size() && (tokens.at(h) == ">" || tokens.at(h) == "1>")) {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back(">");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        else if ((h + 1) < tokens.size() && (tokens.at(h) == ">>" || tokens.at(h) == "1>>")) {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back(">>");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        //////////
        else if ((h + 1) < tokens.size() && tokens.at(h) == "2>") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("2>");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "2>>") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("2>>");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        //////////
        else if ((h + 1) < tokens.size() && tokens.at(h) == "<") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("<");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "<<<") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("<<<");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            // h++;
            tokensWithFlags.push_back(tokens.at(h));
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            // h++;
        }
        else if ((h + 1) < tokens.size() && (tokens.at(h) == "cd" || tokens.at(h) == "CD" || tokens.at(h) == "Cd" || tokens.at(h) == "cD")) {
            my_CD(tokens.at(h + 1), currDir, pathNames);
            h++;
        }
        else tokensWithFlags.push_back(tokens.at(h));
    }
    if (tokensWithFlags.empty()) return rshell(pathNames, currDir);
    if (tokensWithFlags.at(0) == "exit") exit(0);
    //totalCMD.push_back(tokensWithFlags);
    totalCMD.push(tokensWithFlags);
    //for (size_t m = 0; m <= totalCMD.size(); m++) {
    ////////////////////////////////////////////////////////////////////////////
    // bool nextPipe = false;
    string CMDtoPipe;
    while (!(totalCMD.empty())) {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("error in pipe");
            exit(1);
        }
        int pid;
        if ((pid = fork()) == -1) {
            perror("error in fork");
            exit(1);
        }
        //tokensWithFlags = totalCMD.back();
        //totalCMD.pop_back();
        tokensWithFlags = totalCMD.front();
        totalCMD.pop();
        vector<string> nextToken;
        bool isAND = false;
        bool isOR = false;
        bool isOut1 = false;
        bool isOut2 = false;
        bool outErr = false;
        bool isIn1 = false;
        bool isIn3 = false;
        bool nextPipe = false;
        // const char *outFile;
        // const char *inFile;
        string outFile;
        string inFile;
        string inString;
        
        // char buff;
        
        int fdIn;
        int fdOut;
        int fdo;
        int fdi;
        for (int m = 0; m < 2 && !(totalCMD.empty()); m++) {
        // if (!(totalCMD.empty())) {
            nextToken = totalCMD.front();
            // cout << "Next Tok: " << endl;
            if (nextToken.front() == "&&") {
                isAND = true;
                // cout << "AND TRUE" << endl;
                totalCMD.pop();
            }
            if (nextToken.front() == "||") {
                isOR = true;
                // cout << "OR TRUE" << endl;
                totalCMD.pop();
            }
            if (nextToken.front() == "<") {
                isIn1 = true;
                // cout << "is <" << endl;
                totalCMD.pop();
                totalCMD.pop();
                if (!(totalCMD.empty())) {
                    inFile = totalCMD.front().front().c_str();
                    totalCMD.pop();
                }
                else return rshell(pathNames, currDir);
            }
            if (nextToken.front() == ">" || nextToken.front() == "1>" || nextToken.front() == "2>") {
                if (nextToken.front() == "2>") outErr = true;
                isOut1 = true;
                // cout << "is >" << endl;
                totalCMD.pop();
                totalCMD.pop();
                if (!(totalCMD.empty())) {
                    outFile = totalCMD.front().front().c_str();
                    // cout << "OUTFILE: " << outFile << endl;
                    totalCMD.pop();
                }
                else return rshell(pathNames, currDir);
            }
            if (nextToken.front() == ">>" || nextToken.front() == "1>>" || nextToken.front() == "2>>") {
                if (nextToken.front() == "2>>") outErr = true;
                isOut2 = true;
                // cout << "is >>" << endl;
                totalCMD.pop();
                totalCMD.pop();
                if (!(totalCMD.empty())) {
                    outFile = totalCMD.front().front().c_str();
                    // cout << "OUTFILE: " << outFile << endl;
                    totalCMD.pop();
                }
                else return rshell(pathNames, currDir);
            }
            if (nextToken.front() == "|") {
                nextPipe = true;
                // cout << "is |" << endl;
                totalCMD.pop();
                // totalCMD.pop();
                if (!(totalCMD.empty())) {
                    CMDtoPipe = totalCMD.front().front();
                    // cout << "CMDtoPipe: " << CMDtoPipe << endl;
                    // totalCMD.pop();
                }
                else return rshell(pathNames, currDir);
            }
            if (nextToken.front() == "<<<") {
                isIn3 = true;
                // cout << "is <<<" << endl;
                totalCMD.pop();
                totalCMD.pop();
                if (!(totalCMD.empty())) {
                    inString = totalCMD.front().front();
                    totalCMD.pop();
                }
                else return rshell(pathNames, currDir);
            }
            // cout <<isOut1<<isOut2<<isIn1<<isIn3<<nextPipe<<endl;
        }
        char **ARGV2;
        char **ARGV = new char*[tokensWithFlags.size() + 1];
        
        for (size_t j = 0; j < tokensWithFlags.size(); j++) {
            // ARGV[j] = new char[tokensWithFlags.at(j).size() + 1];
            // strcpy(ARGV[j], tokensWithFlags.at(j).c_str());
            ARGV[j] = (char*)tokensWithFlags.at(j).c_str();
        }
        ARGV[tokensWithFlags.size()] = 0;
        if (nextPipe) {
            
            tokensWithFlags = totalCMD.front();
            totalCMD.pop();
            ARGV2 = new char*[tokensWithFlags.size() + 1];
            for (size_t j = 0; j < tokensWithFlags.size(); j++) {
                // ARGV2[j] = new char[tokensWithFlags.at(j).size() + 1];
                // strcpy(ARGV2[j], tokensWithFlags.at(j).c_str());
                ARGV2[j] = (char*)tokensWithFlags.at(j).c_str();
            }
            ARGV2[tokensWithFlags.size()] = 0;
            // cout << "ARGV2 NOW: " << ARGV2[0] << " CMDtoPipe: " << CMDtoPipe << endl;
        }
        string CMD = ARGV[0];
        // cout << "CMD: " << CMD << endl;
        // cout << "PID: " << pid << endl;
        int status = 0;
        if (pid == -1) {
            perror("error in fork");
            exit(1);
            //return 1;
        }
        else if (pid == 0 && nextPipe && !isIn3) { // | PIPING
            my_Pipes(ARGV, ARGV2, pathNames);
            // if (totalCMD.empty()) cout << "totalCMD is empty" << endl;
            // else cout << "totalCMD is not empty" << endl;
            delete[] ARGV2;
            // cout << "DONE PIPING" << endl;
            if (wait(&status) == -1) {
                perror("error in wait");
                exit(1);
            }
            if (totalCMD.empty()) return rshell(pathNames, currDir);
        }
        else if (pid == 0 && !nextPipe && isIn3) { // <<<
            
            char *ARGV3[3];
            ARGV3[0] = new char[5];
            ARGV3[0] = (char*)"echo";
            ARGV3[1] = new char[inString.size() + 1];
            ARGV3[1] = (char*)inString.c_str();
            ARGV3[2] = 0;
            
            
            my_Pipes(ARGV3, ARGV, pathNames);
            // delete[] ARGV3;
            // if (totalCMD.empty()) cout << "totalCMD is empty" << endl;
            // else cout << "totalCMD is not empty" << endl;
            // cout << "DONE PIPING" << endl;
            if (wait(&status) == -1) {
                perror("error in wait");
                exit(1);
            }
            if (totalCMD.empty()) return rshell(pathNames, currDir);
        }
        else if ((pid == 0) && (CMD != CMDtoPipe) && !nextPipe && !isIn3) {////////////////////////////////////////////////////
            // cout << "CHILD: " <<CMD << " CMDtoPipe: " << CMDtoPipe << " nextPipe: " << nextPipe<<endl;
            if (isIn1) { // <
                // cout << "isIn1" << endl;
                // cout << "isIn1 "<< inFile << endl;
                if ((fdIn = dup(0)) == -1) {
                    perror("error in dup");
                    exit(1);
                }
                if ((fdi = open(inFile.c_str(), O_RDONLY)) == -1) {
                    perror("error in open");
                    exit(1);
                }
                if (dup2(fdi, 0) == -1) {
                    perror("error in dup2");
                    exit(1);
                }
            }
            
            if (isOut1) { // >
                // cout << "isOut1 "<< outFile << endl;
                int OutFD = 1;
                if (outErr) OutFD = 2;
                if ((fdOut = dup(OutFD)) == -1) {
                    perror("error in dup");
                    exit(1);
                }
                if (close(OutFD) == -1) {
                    perror("error in close");
                    exit(1);
                }
                if ((fdo = open(outFile.c_str(), O_WRONLY | O_CREAT, 0666)) == -1) {
                    perror("error in open");
                    exit(1);
                }
                if (truncate(outFile.c_str(), 0) == -1) {
                    perror("error in truncate");
                    exit(1);
                }
                if (dup2(fdo, OutFD) == -1) {
                    perror("error in dup2");
                    exit(1);
                }
            }
            else if (isOut2) { // >>
                // cout << "isOut2 " << outFile  << endl;
                int OutFD = 1;
                if (outErr) OutFD = 2;
                if ((fdOut = dup(OutFD)) == -1) {
                    perror("error in dup");
                    exit(1);
                }
                if ((fdo = open(outFile.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
                    perror("error in open");
                    exit(1);
                }
                if (dup2(fdo, OutFD) == -1) {
                    perror("error in dup2");
                    exit(1);
                }
            }
            
            if (!nextPipe) {
                string ARGV0 = ARGV[0];
                // cout << "EXE NOW: " << ARGV[0] << endl;
                if (signal(SIGINT, NULL) == SIG_ERR) {
                    perror("error in signal");
                    exit(1);
                }
                for (size_t k = 0; k < pathNames.size(); k++) {
                    if (execv((pathNames[k] + '/' + ARGV0).c_str(), ARGV) != 0 && k == (pathNames.size() - 1)) {
                        // cerr << "ERR CMD: " << ARGV[0] << endl;
                        perror("error in execv");
                        isOR = false;
                        isAND = false;
                        // if ((wait(&status) == -1)) 
                        exit(1);
                    }
                }
            }
            return 0;
        }
        
        else if (CMD != CMDtoPipe && wait(&status) == -1) {
            perror("error in wait");
            exit(1);
        }
        // }
        else if ((isIn3 || nextPipe) && totalCMD.empty()) exit(0);
        // else if (!nextPipe && totalCMD.empty()) return rshell(pathNames, currDir);
        
        if (isOR && status == 0) {
            // cout << "POP OR" << endl;
            if (!(totalCMD.empty())) totalCMD.pop();
        }
        if (isAND && !(status == 0)) {
            // cout << "POP AND" << endl;
            if (!(totalCMD.empty())) totalCMD.pop();
        }
        delete[] ARGV;
    }
    
    return rshell(pathNames, currDir);
}

int main(int argc, char** argv)
{
    char currDir[128];
    if (getcwd(currDir, 128) == NULL) {
        perror("error in getcwd");
        exit(1);
    }
    string envPATH = getenv("PATH");
    // cout << envPATH << endl << endl;
    vector<string> pathNames;
    char_separator<char> colon(":","");
    tokenizer<char_separator<char> > tok(envPATH, colon);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        pathNames.push_back(*iter);
        // cout << *iter << endl;
    }
    pathNames.push_back(currDir);
    // cout << endl << pathNames.size() << endl;
    
    return rshell(pathNames, currDir);
}
