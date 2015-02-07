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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iomanip>

using namespace std;
using namespace boost;

bool dash_a = false;
bool dash_l = false;
bool dash_R = false;
bool haveFileOrFolder = false;

int main(int argc, char** argv)
{
    
    string Flags;
    priority_queue<string, vector<string>, greater<string> > inputFolder;
    for (int j = 1; j < argc; j++) {
        if (argv[j][0] != '-') Flags += "()";
        Flags += argv[j];
    }
    //cout << Flags << endl;
    bool FlagsNext = false;
    char_separator<char> dash("()","- ");
    tokenizer<char_separator<char> > tok(Flags, dash);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        //cout << *iter << endl;
        if (*iter == "-" && !FlagsNext) FlagsNext = true;
        else if (FlagsNext) {
            for (size_t k = 0; k < (*iter).length(); k++) {
                if ((*iter).at(k) == 'a') dash_a = true;
                else if ((*iter).at(k) == 'l') dash_l = true;
                else if ((*iter).at(k) == 'R') dash_R = true;
            }
            FlagsNext = false;
        }
        else {
            haveFileOrFolder = true;
            inputFolder.push(*iter);
            
        }
        
    }
    //cout << inputFolder.size() << endl;
    while (!inputFolder.empty()) {
        //cout << inputFolder.top() << " ";
        inputFolder.pop();
    }
    //cout << argc << dash_a << dash_l << dash_R << haveFileOrFolder << endl << endl << endl;
    
    
    
    
    
    priority_queue<string, vector<string>, greater<string> > fileFolderNames;
    priority_queue<string, vector<string>, greater<string> > fileFolderNamesSorted;
    DIR *currDir = opendir(".");
    
    if (currDir == NULL) {
        perror("error in opendir");
        exit(1);
    }
    
    dirent *entry = readdir(currDir);
    if (entry == NULL) {
        perror("error in readdir");
        exit(1);
    }
    
    size_t column1 = 0;
    size_t column2 = 0;
    size_t column3 = 0;
    do {
        string entryName = entry->d_name;
        if (entryName.at(0) == '.' && !dash_a);
        else {
            fileFolderNames.push(entry->d_name);
        }
    } while ((entry = readdir(currDir)) != NULL);
    closedir(currDir);
    
    while (!fileFolderNames.empty()) {
        string entryName2 = fileFolderNames.top();
        fileFolderNames.pop();
        fileFolderNamesSorted.push(entryName2);
        if ((fileFolderNamesSorted.size() % 3) == 1) {
            if (entryName2.size() > column1) column1 = entryName2.size() + 4;
        }
        else if ((fileFolderNamesSorted.size() % 3) == 2) {
            if (entryName2.size() > column2) column2 = entryName2.size() + 4;
        }
        else if ((fileFolderNamesSorted.size() % 3) == 0) {
            if (entryName2.size() > column3) column3 = entryName2.size() + 4;
        }
    }
    while (!fileFolderNamesSorted.empty() && !(dash_R || dash_l)) {
        cout << setw(column1) << left;
        cout << fileFolderNamesSorted.top();
        fileFolderNamesSorted.pop();
        if (!fileFolderNamesSorted.empty()) {
            cout << setw(column2) << left;
            cout << fileFolderNamesSorted.top();
            fileFolderNamesSorted.pop();
            if (!fileFolderNamesSorted.empty()) {
                cout << setw(column3) << left;
                cout << fileFolderNamesSorted.top();
                fileFolderNamesSorted.pop();
            }
        }
        cout << endl;
    }
    //struct stat currFile;
    //stat()
    return 0;
}
