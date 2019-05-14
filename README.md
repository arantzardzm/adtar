# adtar

### Operating Sytems - NYUAD
Jaisal Friedman & Arantza Rodriguez  

### Description:

For our Operating Systems course, we developed an archiving application (adtar) whose objective is to "flatten" the logical hierarchies of a filesystem. The program stores and recreates the hierarchies from a single file with the postfix **.ad**.

### Files Included:

 * README.txt
 * Makefile
 * adtar.c
 * func.c
 * func.h
 * struct.c
 * struct.h
 * common.c
 * common.h

### Run Instructions:

The program is compiled by typing `make` in the command line. The program is then invoked with the following syntax:
`adtar {-c|-a|-x|-m|-p| -o NUMBER} <archive-file> <file/directory list>`.

  * `-c`: store in the archive file <archive-file> (appropriately designated with postfix .ad), the files and/or directories
provided by the list <file/directory list>. If other additional files/directories exist in <directory list>, then all this
content is recursively stored in the <archive-file>.
  * `-a`: append filesystem entities indicated in the <file/directory list> in the archived file <archive-file> that already
exists. If additional files and directories exist in <file/directory list>, they are recursively appended in the
designated with <archive-file>.
  * `-x`: extract all files and catalogs that have been archived in file <archive-file>. If `-o` is specified, then extract the files with NUMBERth occurrence of each file. If option `-o` is not specified it will extract the latest occurrence.
  * `-m`: print out the meta-data (owner, group, rights, type of file) for all files/directories that have been archived in the <archive-file>.
  * `-p`: display the hierarchy(-ies) of the files and directories stored in the <archive-file>. Do this in a way that can be
readily understood by the user.
