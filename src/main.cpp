#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

using namespace std;

int main(int argc, char **argv)
{
    /*
    char *loginname = getlogin();
    char *hostname;
    size_t len;
    gethostname(hostname, len);
    cout << loginname << '@' << hostname << '$'<< endl;
    */
    char *test = "ls";
    const char *cmd;
    string cmdstring;
    
    cout << "$ ";
    cin >> cmdstring;
    
    cmd = cmdstring.c_str();
    int pid = fork();
    
    if (cmdstring == "exit") {
        exit(0);
    }
    if (pid == -1) {
        perror("fork fail");
    }
    else if (pid == 0) {
        if (execvp(cmd,argv) != 0) {
            perror("error in execvp");
        }
    }
    else {
        cout << "DONE" << endl;
        exit(0);
    }
}