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
#include <grp.h>
#include <pwd.h>
#include <dirent.h>
#include <iomanip>
#include <time.h>

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
int recurr = 0;

void checkUser(string fileName) {
    struct stat s;
    if (stat(fileName.c_str(), &s) == -1) {
        perror("error in stat");
        exit(1);
    }
    cout << ((s.st_mode & S_IRUSR) ? "r" : "-");
    cout << ((s.st_mode & S_IWUSR) ? "w" : "-");
    cout << ((s.st_mode & S_IXUSR) ? "x" : "-");
}

void checkGroup(string fileName) {
    struct stat s;
    if (stat(fileName.c_str(), &s) == -1) {
        perror("error in stat");
        exit(1);
    }
    cout << ((s.st_mode & S_IRGRP) ? "r" : "-");
    cout << ((s.st_mode & S_IWGRP) ? "w" : "-");
    cout << ((s.st_mode & S_IXGRP) ? "x" : "-");
}

void checkOther(string fileName) {
    struct stat s;
    if (stat(fileName.c_str(), &s) == -1) {
        perror("error in stat");
        exit(1);
    }
    cout << ((s.st_mode & S_IROTH) ? "r" : "-");
    cout << ((s.st_mode & S_IWOTH) ? "w" : "-");
    cout << ((s.st_mode & S_IXOTH) ? "x" : "-");
}

bool isFolder(string &fileName) {
    string tempName = currLocation;
    if (currLocation != "." && fileName.find(currLocation) == string::npos) tempName += fileName;
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
    if (currLocation != "." && fileName.find(currLocation) == string::npos) tempName += fileName;
    else tempName = fileName;
    struct stat s;
    // cout << "fileName: " << fileName << endl;
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

void LS_l(priority_queue<string, vector<string>, greater<string> > fileFolderNamesSorted, size_t columnUser, size_t columnGroup) {
    string currName = fileFolderNamesSorted.top();
    fileFolderNamesSorted.pop();
    struct stat file;
    string timeMod;
    // if (haveFileOrFolder && (currName.substr(0,1) == "./")) {
    //     currName = currName.substr(2, currName.size() - 1);
    // }
    // cout << "currName: " << currName << " LOC: " << currLocation << endl;
    if (stat((currName).c_str(), &file) == -1) {
        perror("error in stat");
        exit(1);
    }
    // cout << ((isFolder(currName)) ? "d" : "-" );
    if (isFolder((currName))) {
        cout << "d";
        currName.erase(currName.size() - 1);
    }
    else cout << "-";
    checkUser(currName);
    checkGroup(currName);
    checkOther(currName);
    cout << " " << setw(4) << right << file.st_nlink << " " << setw(columnUser) << left << getpwuid(file.st_uid)->pw_name;
    cout << " " << setw(columnGroup) << left << (getgrgid(file.st_gid))->gr_name << " " << setw(6) << right << file.st_size;
    timeMod = ctime(&file.st_mtime);
    timeMod.erase(timeMod.size() - 1);
    cout << " " << timeMod << " ";
    if (isFolder(currName)) {
        cout << "\x1b[94m";
        if (currName.at(0) == '.') cout << "\x1b[100m";
    }
    else if (isExecutable(currName)) cout << "\x1b[92;49m";
    else cout << "\x1b[39;49m";
    cout << currName;
    cout << "\x1b[39;49m";
    cout << endl;
    if (!fileFolderNamesSorted.empty()) LS_l(fileFolderNamesSorted, columnUser, columnGroup);
}

void LS_Check_Flags(int argc, char** argv)
{
    string Flags;
    priority_queue<string, vector<string>, greater<string> > inputFileFolder;
    for (int j = 1; j < argc; j++) {
        if (argv[j][0] != '-') Flags += "()";
        Flags += argv[j];
    }
    // cout << Flags << endl;
    bool FlagsNext = false;
    char_separator<char> dash("()","- ");
    tokenizer<char_separator<char> > tok(Flags, dash);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        // cout << *iter << endl;
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
            if (isFolder(tempFileFolder) && tempFileFolder.find(currLocation) == string::npos) {
                inputFolder.push(tempFileFolder);
            }
            else inputFile.push(tempFileFolder);
        }
    }
}

void LS_Organize(priority_queue<string, vector<string>, greater<string> > fileFolderNames) {
    priority_queue<string, vector<string>, greater<string> > fileFolderNamesSorted;
    size_t column1 = 0;
    size_t column2 = 0;
    size_t column3 = 0;
    size_t columnUser = 0;
    size_t columnGroup = 0;
    blksize_t totalBlock = 0;
    while (!fileFolderNames.empty()) {
        string entryName2 = fileFolderNames.top();
        fileFolderNames.pop();
        if (dash_R && entryName2.find(currLocation) == string::npos) {
            if (currLocation.at(currLocation.size() - 1) == '/') entryName2 = currLocation + entryName2;
            else if (currLocation != ".") entryName2 = currLocation + "/" + entryName2;
        }
        else if (haveFileOrFolder && dash_l) {
            entryName2 = currLocation + entryName2;
        }
        fileFolderNamesSorted.push(entryName2);
        string entryName3 = entryName2;
        if (dash_R && isFolder(entryName3) ) {
            if ((entryName3.find(currLocation) == string::npos) ||
                (entryName3.substr(0, currLocation.size() ) == currLocation))
                folderNamesR.push(entryName3);
            else folderNamesR.push(currLocation + "/" + entryName3);
        }
        if (dash_l) {
            struct stat file;
            // cout << "EN2: " << entryName2 << " LOC: "<< currLocation << endl;
            if (stat((entryName2).c_str(), &file) == -1) {
                perror("error in stat");
                exit(1);
            }
            if (strlen((getpwuid(file.st_uid))->pw_name) > columnUser) columnUser = strlen((getpwuid(file.st_uid))->pw_name) + 2; 
            if (strlen((getgrgid(file.st_gid))->gr_name) > columnGroup) columnGroup = strlen((getgrgid(file.st_gid))->gr_name) + 2;
            totalBlock += file.st_blocks;
        }
        if (!dash_l && ((fileFolderNamesSorted.size() % 3) == 1)) {
            if (entryName2.size() > column1) column1 = entryName2.size() + 4;
        }
        else if (!dash_l && ((fileFolderNamesSorted.size() % 3) == 2)) {
            if (entryName2.size() > column2) column2 = entryName2.size() + 4;
        }
        else if (!dash_l && ((fileFolderNamesSorted.size() % 3) == 0)) {
            if (entryName2.size() > column3) column3 = entryName2.size() + 4;
        }
    }
    
    if (dash_l) {
        cout << "total: " << totalBlock/2 << endl;
        LS_l(fileFolderNamesSorted, columnUser, columnGroup);
    }
    else if (!(dash_l)) LS_Out (fileFolderNamesSorted, column1, column2, column3);
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
    if (closedir(currDir) == -1) {
        perror("error in closedir");
        exit(1);
    }
    LS_Organize(fileFolderNames);
}

void LS_R() {
    cout << endl;
    string currFolder;
    currFolder = folderNamesR.top();
    folderNamesR.pop();
    cout << currFolder << ":" << endl;
    
    currLocation = currFolder;
    my_LS(currLocation.c_str());
    cout << endl;
    recurr++;
    if (!folderNamesR.empty() && recurr < 10) LS_R();
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
            currLocation = inputFolder.top();
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
    if (!dash_R && !haveFileOrFolder) my_LS(".");
    return 0;
}