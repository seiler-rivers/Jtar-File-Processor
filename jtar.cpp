#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include "file.cpp"
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
using namespace std;

int checkParams(int argc, char* argv[]);
void cf(int argc, char* argv[]);
void listAll(string s);
File makeFile(string name);
void listAll(string s, vector <string> &filenames);
void tf(char* argv[]);
void xf(char * argv[]);

int main(int argc, char * argv[])
{
    checkParams(argc, argv);
    return 0;
}

// a method to parse command line args and check for errors
//if failed, return 0.
int checkParams(int argc, char* argv[]){

    // no option specified, throw error
    if(argc < 2){
        cout << "jtar: You must specify one of the options. " << endl << "Try 'jtar --help' for more information." << endl;
        return 0;
    }
    else if(argv[1][0] != '-'){     //not a valid tar command
        cout << "You must specify one of the options" << endl << "Try 'jtar --help' for more information." << endl;
        return 0;
    }
    else if(string(argv[1]) == "--help"){   // describe the program options
        cout << "'jtar' saves many files together into a single tape or disk archive, and " << endl;
        cout << "can restore individual files from the archive. \n" << endl;
        cout << "Usage: jtar [OPTION]... [FILE]... \n" << endl;
        cout << "Option Examples:" << endl;
        cout << "  tar -cf archive.tar foo bar  # Create archive.tar from files foo and bar." << endl;
        cout << "  tar -tf archive.tar          # List all files in archive.tar verbosely. " << endl;
        cout << "  tar -xf archive.tar          # Extract all files from archive.tar." << endl;
    }
    else if(string (argv[1]) == "-cf"){     // not enough params for cf
        if(argc < 4){
            cout << "jtar: Invalid format" << endl << "Try 'jtar --help' for more information." << endl;
            return 0;
        }
        else
            cf(argc, argv);                 // call cf to build tar file
            return 1;
    }
    else if(string (argv[1]) == "-tf"){    // not enough params for tf    
        if(argc < 3){
            cout << "jtar: Invalid format " << endl << "Try 'jtar --help' for more information." << endl;
            return 0;   
        }
        else
            tf(argv);                       // call tf to list files
            return 1;
    }
    else if(string (argv[1]) == "-xf"){     // not enough params for xf
        if(argc < 3){
            cout << "jtar: Invalid format " << endl << "Try 'jtar --help' for more information." << endl;
            return 0;   
        }
        else
            xf(argv);                       // call xf to extract files
            return 1;
    }
    else {                                  // any unrecognizable command
        cout << "jtar: Invalid option (" << argv[1] << ") " << endl << "Try 'jtar --help' for more information." << endl;
        return 0;
    }
    return 1;
}

// a method to build a tar file with files from command line
void cf(int argc, char* argv[]){

    //check files and get names
    vector <string> filesnames;     // vector to hold names from comman line
    vector <File> files;            // vector to hold file objects
    for(int i = 3; i<argc; i++){    // start when files listed on command line, insert names into vector
        listAll(argv[i], filesnames);
    }

    // create a vector of file objects
    for(auto elem : filesnames){
        files.push_back(makeFile(elem));
    }

    fstream tarfile (argv[2], ios::out | ios::binary);  //fstream object to write File objects out to binary file
    int size =  files.size();
    tarfile.write(reinterpret_cast<const char *>(&size), sizeof(size));          //write out number of files to beginning of tarfile

    for(auto elem : files){
        tarfile.write((char*) &elem, sizeof(File));   //write file object

        if(!elem.isADir()){                  // if not a directory, write text to tarfile char by char
            fstream inText (elem.getName(), ios::in);
            char ch;
            while(inText.get(ch)){
                tarfile.write((char*) &ch, sizeof(char));
            }
            inText.close();
        }
    }
    tarfile.close();

}

// a function to show all the files contained in a tar file
void tf(char* argv[]){

    // open the tar file, throw err if not found and quit
    fstream tarfile(argv[2], ios::in | ios::binary);
    if(tarfile.fail()){
        cout<<"### Cannot open file "<<argv[2]<<endl;
        exit(1);
    }
    int numFiles;
    tarfile.read((char*) &numFiles, sizeof(int));   //get number of files/directories in tar file

    //parse the tar file, loop for each file 
    for(int i = 0; i<numFiles; i++){
        File myFile;
        tarfile.read((char*) &myFile, sizeof(File));
        cout << myFile.getName() << endl;

        // if not directory, skip all the text
        if(!myFile.isADir()){
            tarfile.seekg(stoi(myFile.getSize()), ios::cur);
        }
    }

}

// read a tar file, recreate files within
void xf(char * argv[]){
    // tarfile to read from, check if it exists and throw error if not
    fstream tarfile(argv[2], ios::in | ios::binary);
    if(tarfile.fail()){
        cout<<"### Cannot open file "<<argv[2]<<endl;
        exit(1);
    }
    int numFiles;
    tarfile.read((char*) &numFiles, sizeof(int));   //get number of files/directories in tar file

    //parse the tar file, loop for each file 
    for(int i = 0; i<numFiles; i++){
        File myFile;
        tarfile.read((char*) &myFile, sizeof(File));    //get first file

        // check if the file/directory already exists. if so, do nothing.
        fstream ff ( myFile.getName().c_str(), ios:: in);
        if(!ff.fail()){
            cout << "The file " << myFile.getName() << " already exists. Cannot recreate." << endl;
            continue;
        }
        ff.close();

        // if not directory, create text file and insert contents
        if(!myFile.isADir()){
            // write text out to file so no content is lost
            fstream create ( myFile.getName().c_str(), ios:: out);
            char ch;
            for(int i=0; i < stoi(myFile.getSize()); i++){
                tarfile.read((char*) &ch, sizeof(char));
                create.write((char*) &ch, sizeof(char));
            } 
            create.close();
            // preserve timestamp and privacy settings
            string command = "touch " + myFile.getName();
            system(command.c_str());
            command = "chmod " + myFile.getPmode() + " " + myFile.getName();
            system(command.c_str());
            command = "touch -t " + myFile.getStamp() + " " + myFile.getName();
            system(command.c_str());

        }
        else if(myFile.isADir()){
            string command = "mkdir " + myFile.getName();       // make the directory
            system(command.c_str());
            command = "chmod " + myFile.getPmode() + " " + myFile.getName();
            system(command.c_str());
            command = "touch -t " + myFile.getStamp() + " " + myFile.getName();
            system(command.c_str());
        }
    }
    tarfile.close();
}


// a method to list the path of a file and insert it into a vector
void listAll(string s, vector <string> &filenames)
{
    fstream infile(s, ios::in);  //file object to test if file exists
    if (infile.fail())
    {
        cout<<"### Cannot open file "<<s<<endl;
        exit(1);
    }
    infile.close();

    struct stat buf;                        // stat struct to determine if directory
    lstat (s.c_str(), &buf);

    string command = "ls -1R >temp " + s;  // system command to get all files in directory and put paths in a file called temp
    system(command.c_str());
    fstream temp("temp", ios::in);       // filestream object to read paths from temp
    string line, path;
    filenames.push_back(s);                 // add file to vector

    if (S_ISDIR(buf.st_mode)){              // if directory, read in path from temp file     
        while(getline (temp, line, '\n' )){  
            if(line.back()==':'){           // : menas directory
                line.pop_back();            // pop the : off the string
                path = line;                // save the directory to the path
            }
            else if(line == ""){            // skip new line
                continue;
            }
            else{                           // add onto path
                line = path + "/" + line;
                filenames.push_back(line);
            }
        }
    }
    system("rm temp");                      //delete temp file
    temp.close();
}

//function to construct file object from parameters
//code taken from utility.cpp
File makeFile(string name){
    struct stat buf;
    lstat (name.c_str(), &buf);

    //set directory flag
    bool isDirectory;
    if (S_ISREG(buf.st_mode))
        isDirectory = false;
    else if (S_ISDIR(buf.st_mode))
        isDirectory = true;

    // set protection and size
    string protection, size;
    protection = (to_string((buf.st_mode & S_IRWXU) >> 6) + to_string((buf.st_mode & S_IRWXG) >> 3) + to_string(buf.st_mode & S_IRWXO));
    size = to_string(buf.st_size);

    char stamp[16];
    strftime(stamp, 16, "%Y%m%d%H%M.%S", localtime(&buf.st_mtime));

    // instantiate file object
    File myFile(name.c_str(), protection.c_str(), size.c_str(), stamp);

    // flag directory
    if(isDirectory)
        myFile.flagAsDir();

    return myFile;
}
