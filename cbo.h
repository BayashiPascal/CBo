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

// ================= Define ==================

#define CBOLINE_MAX_LENGTH 72

// ================= Data structures ===================

// Structure to memorize one line of a file
typedef struct CBoLine {
  // Original line 
  char* str;
} CBoLine;

// Enum for the file type
typedef enum CBoFileType {
  CBoFileType_unknown,
  CBoFileType_C_header,
  CBoFileType_C_body
} CBoFileType;

// Label for the file types
extern const char* cboFileTypeStr[];

// Structure to memorize the content of one file
typedef struct CBoFile {
  // Pointer to the file path
  const char* filePath;
  // Type of the file
  CBoFileType type;
  // Set of lines
  GSet lines;
} CBoFile;

// Structure of the CBo code style checker
typedef struct CBo {
  // Set of path to the checked files
  GSetStr filePaths;
  // Set of loaded path
  GSet files;
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

// Get the number of file to check
#if BUILDMODE != 0
inline
#endif 
int CBoGetNbFiles(const CBo* const that);

// Function to create a new CBoFile from its file path,
// Return a pointer toward the new CBoFile
CBoFile* CBoFileCreate(const char* const filePath);

// Function to free the memory used by the CBoFile 'that'
void CBoFileFree(CBoFile** const that);

// Function to create a new CBoLine from its content,
// Return a pointer toward the new CBoLine
CBoLine* CBoLineCreate(const char* const str);

// Function to free the memory used by the CBoLine 'that'
void CBoLineFree(CBoLine** const that);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "cbo-inline.c"
#endif

#endif
