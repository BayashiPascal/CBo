// *************** CBO.C ***************

// ================= Include ==================
#include "cbo.h"
#if BUILDMODE == 0
#include "cbo-inline.c"
#endif

// ================ Functions declaration ==================

// Check the file at path 'filePath'
// Return true if there was no problem, else false
bool CBoCheckOneFile(
  CBo* const that,
  const char* const filePath);

// ================ Functions implementation ==================

// Function to create a new CBo,
// Return a pointer toward the new CBo
CBo* CBoCreate(void) {

  // Allocate memory for the CBo
  CBo* that = PBErrMalloc(CBoErr, sizeof(CBo));

  // Init the properties
  that->filePaths = GSetStrCreateStatic();

  // Return the new CBo
  return that;

}

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** const that) {

  if (that == NULL || *that == NULL) return;

  // Free memory used by properties
  GSetFlush(&((*that)->filePaths));

  // Free the memory
  free(*that);

  // Set the pointer to null
  *that = NULL;

}

// Process the arguments from the command line
// Return true if the arguments were correct, else false
bool CBoProcessCmdLineArguments(
          CBo* const that,
           const int argc,
  const char** const argv) {

  // Loop on arguments
  for (int iArg = 1;
       iArg < argc;
       ++iArg) {

    if (strcmp(argv[iArg], "-help") == 0) {

      printf("cbo\n");
      printf("[-help] : print the help message\n");
      printf("\n");

    } else {

      // Any other arguments is considered to be a path to a file to check

      // Try to open the file to check the path
      FILE* f = fopen(argv[iArg], "r");

      // If the path is correct
      if (f != NULL) {

        fclose(f);
        // Add the path to the list of files to check
        GSetAppend(
          &(that->filePaths),
          (char*)(argv[iArg]));
        
      // Else the path is incorrect
      } else {

        printf(
          "The path [%s] is incorrect\n",
          argv[iArg]);
        return false;

      }
      
    }

  }

  // Return the successfull code
  return true;

}

// Check the files
// Return true if there was no problem, else false
bool CBoCheckAllFiles(CBo* const that) {

  // Declare a variable to memorize if all the file were correct
  bool allCorrect = true;

  // If there are files to check
  if (CBoGetNbFiles(that) > 0) {

    // Loop on the files
    GSetIterForward iterFilePath = 
      GSetIterForwardCreateStatic(&(that->filePaths));
    do {

      // Get the file path
      const char* filePath = GSetIterGet(&iterFilePath);

      // Check the file
      allCorrect &= 
        CBoCheckOneFile(
          that,
          filePath);

    } while (GSetIterStep(&iterFilePath) == true);

  }

  // Return the flag
  return allCorrect;

}

// Check the file at path 'filePath'
// Return true if there was no problem, else false
bool CBoCheckOneFile(
  CBo* const that,
  const char* const filePath) {

  // Display an info message
  printf(
    "=== Check file [%s] ===\n",
    filePath);

  // Load the file into memory
  

  // Return the successfull code
  return true;

}
