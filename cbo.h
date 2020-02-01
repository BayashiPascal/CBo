// *************** CBO.H ***************
#ifndef CBO_H
#define CBO_H

// ================= Include ==================
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "pberr.h"
#include "gset.h"
#include "respublish.h"
#include "pbcextension.h"

// ================= Data structures ===================

// Structure of the CBo code style checker
typedef struct CBo {

  // Set of path to the checked files
  GSetStr filePaths;
  // Set of CBoFile
  GSet files;
  // Set of CBoFile with error
  GSet filesWithError;
  // Stream on which the output is printed
  FILE* stream;
  // Flag to remember if we print only the list of files with errors
  bool flagListFileError;

} CBo;

// ================ Functions declaration ====================

// Function to create a new CBo,
// Return a pointer toward the new CBo
CBo* CBoCreate(void);

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** const that);

// Process the arguments from the command line
// Return true if the arguments were correct, else false
bool CBoProcessCmdLineArguments(
          CBo* const that,
           const int argc,
  const char** const argv);

// Check the files of the CBo 'that'
// Return true if there was no problem, else false
bool CBoCheckAllFiles(CBo* const that);

// Get the number of file to check
#if BUILDMODE != 0
inline
#endif
int CBoGetNbFiles(const CBo* const that);

// Function to get the number of file with error in the CBo 'that'
unsigned int CBoGetNbFilesWithError(const CBo* const that);

// Function to get the total number of errors in the CBo 'that'
unsigned int CBoGetNbErrors(const CBo* const that);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "cbo-inline.c"
#endif

#endif
