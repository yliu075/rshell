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
string currLocation = ".";
priority_queue<string, vector<string>, greater<string> > folderNamesR;
priority_queue<string, vector<string>, greater<string> > inputFile;
priority_queue<string, vector<string>, greater<string> > inputFolder;


bool isFolder(string &fileName) {
    string tempName = currLocation;
    if (currLocation != ".") tempName += fileName;
    else tempName = fileName;
    struct stat s;
    if (lstat(tempName.c_str(), &s) == -1) {
        perror("error in lstat");
        exit(1);
    }
    if (S_ISDIR(s.st_mode)) {
        fileName += '/';
        return true;
    }
    return false;
}

bool isExecutable(string &fileName) {
    string tempName = currLocation;
    if (currLocation != ".") tempName += fileName;
    else tempName = fileName;
    struct stat s;
    if (stat(tempName.c_str(), &s) == -1) {
        perror("error in stat");
        exit(1);
    }
    if ((s.st_mode & S_IXUSR) && (s.st_mode & S_IXGRP) && (s.st_mode & S_IXOTH)) {
        fileName += '*';
        return true;
    }
    return false;
}

void LS_Out(priority_queue<string, vector<string>, greater<string> > fileFolderNamesSorted, size_t column1, size_t column2, size_t column3) {
    while (!fileFolderNamesSorted.empty() && !dash_l) {
        string topName = fileFolderNamesSorted.top();
        if (isFolder(topName)) {
            cout << "\x1b[94m";
            if (topName.at(0) == '.') cout << "\x1b[100m";
            cout  << setw(column1) << left;
        }
        else if (isExecutable(topName)) cout << "\x1b[92;49m" << setw(column1) << left;
        else cout << "\x1b[39;49m" << setw(column1) << left;
        cout << topName;
        fileFolderNamesSorted.pop();
    
        if (!fileFolderNamesSorted.empty()) {
            topName = fileFolderNamesSorted.top();
            if (isFolder(topName)) {
                cout << "\x1b[94m";
                if (topName.at(0) == '.') cout << "\x1b[100m";
                cout  << setw(column2) << left;
            }
            else if (isExecutable(topName)) cout << "\x1b[92;49m" << setw(column2) << left;
            else cout << "\x1b[39;49m" << setw(column2) << left;
            cout << topName;
            fileFolderNamesSorted.pop();
            
            if (!fileFolderNamesSorted.empty()) {
                topName = fileFolderNamesSorted.top();
                if (isFolder(topName)) {
                    cout << "\x1b[94m";
                    if (topName.at(0) == '.') cout << "\x1b[100m";
                    cout  << setw(column3) << left;
                }
                else if (isExecutable(topName)) cout << "\x1b[92;49m" << setw(column3) << left;
                else cout << "\x1b[39;49m" << setw(column3) << left;
                cout << topName;
                fileFolderNamesSorted.pop();
            }
        }
        cout << "\x1b[39;49m" << endl;
    }
}


void LS_Check_Flags(int argc, char** argv)
{
    string Flags;
    priority_queue<string, vector<string>, greater<string> > inputFileFolder;
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
            inputFileFolder.push(*iter);
        }
    }
    if (haveFileOrFolder)  {
        while (!inputFileFolder.empty()) {
            string tempFileFolder = inputFileFolder.top();
            inputFileFolder.pop();
            if (isFolder(tempFileFolder)) inputFolder.push(currLocation + "/" + tempFileFolder);
            else inputFile.push(tempFileFolder);
        }
    }
    //cout << argc << dash_a << dash_l << dash_R << haveFileOrFolder << endl << endl << endl;
}

void LS_Organize(priority_queue<string, vector<string>, greater<string> > fileFolderNames) {
    priority_queue<string, vector<string>, greater<string> > fileFolderNamesSorted;
    size_t column1 = 0;
    size_t column2 = 0;
    size_t column3 = 0;
    while (!fileFolderNames.empty()) {
        string entryName2 = fileFolderNames.top();
        fileFolderNames.pop();
        fileFolderNamesSorted.push(entryName2);
        string entryName3 = entryName2;
        if (dash_R && isFolder(entryName3)) folderNamesR.push(currLocation + "/" + entryName3);
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
    LS_Out (fileFolderNamesSorted, column1, column2, column3);
    // return 0;
}

void my_LS(const char * location) {
    currLocation = location;
    priority_queue<string, vector<string>, greater<string> > fileFolderNames;
    DIR *currDir = opendir(location);
    
    if (currDir == NULL) {
        perror("error in opendir");
        exit(1);
    }
    
    dirent *entry = readdir(currDir);
    if (entry == NULL) {
        perror("error in readdir");
        exit(1);
    }
    do {
        string entryName = entry->d_name;
        if (entryName.at(0) == '.' && !dash_a);
        else {
            fileFolderNames.push(entry->d_name);
        }
    } while ((entry = readdir(currDir)) != NULL);
    closedir(currDir);
    LS_Organize(fileFolderNames);
}

void LS_R() {
    cout << endl;
    string currFolder = folderNamesR.top();
    folderNamesR.pop();
    cout << currFolder << ":" << endl;
    
    currLocation = currFolder;
    my_LS(currLocation.c_str());
    cout << endl;
    if (!folderNamesR.empty()) LS_R();
}

int main(int argc, char** argv)
{
    LS_Check_Flags(argc, argv);
    if (haveFileOrFolder) {
        if (!inputFile.empty()) {
            LS_Organize(inputFile);
            cout << endl;
        }
        if (!inputFolder.empty()) {
            folderNamesR = inputFolder;
            LS_R();
        }
    }
    else if (dash_R) {
        cout << currLocation << ":" << endl;
        my_LS(".");
        LS_R();
        return 0;
    }
    if (!(dash_R || dash_l || haveFileOrFolder)) my_LS(".");
    return 0;
}