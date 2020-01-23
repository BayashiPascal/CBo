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

// ================= Define ==================

// ================= Data structures ===================

// Structure of the CBo code style checker
typedef struct CBo {
  // List of path to the checked files
  GSetStr filePaths;
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

// Check the files
// Return true if there was no problem, else false
bool CBoCheckAllFiles(CBo* const that);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "cbo-inline.c"
#endif

#endif
