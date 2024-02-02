# Jtar-File-Processor
 The goal of this project is to write a version of the Linux tar command. This project let the user create a tar file that contains text files and/or directories, list the files contained in the tar file, and recreate them with the same potection and timestamps.

`jtar' saves many files together into a single tape or disk archive, and
can restore individual files from the archive.

Usage: tar [OPTION]... [FILE]...

Option Examples:
 
  1. tar -cf archive.tar foo bar   -------  # Create archive.tar from files foo and bar.
   
  2. tar -tf archive.tar        -------     # List all files in archive.tar verbosely.
  
 3.  tar -xf archive.tar       -------     # Extract all files from archive.tar.



## THE FILE CLASS:
    In this project, a File class was built to store information about each file. The attributes of the File class are as follows:
        name    - to identify the path to the file, not longer than 80 chars
        pmode   - to preserve the protection 
        size    - how many bytes are in the file
        stamp   - what time the file was modified, in the form YrMonthDayHrMin

## TAR OPTIONS:

    CF:
        A method to build a tar file given files/directories on the command line
        create two vectors:
            one to hold file names from command line
            one to hold file objects complete with attributes listed above
        create temp file to pipe output of ls -1R to using system command
        Parse through temp file and add each path to the vector of file names
        parse vector of file names and call build file function to add File objects to vector

    TF:
        A function to show all the files contained in a tar File
        use fstream object to read from binary tar file.
        Skip text from non-directories
        print out file paths to the screen
    
    XF:
        check if tarfile exists, throw error if not
        Parse the tar file for the ammount of files contained inside (number listed at front of file)
        Loop through each file, check if they alreaady exist and skip if so
        restore the text to the files by parsing the tar file.
        Use the linux commands touch -t, chmod, touch, and mkdir to accurately recreate the files based on the attributes preserved in the File class.
        The File class is essential because it tracks the attributes we want to preserve and links them to a particular file path.
        When we recreate files using -xf, we simply pull the file objects from the tar file to reference these attributes.
