#include <stdlib.h>
#include <stdio.h>
#include "cbo.h"

int main(
  int argc, 
  const char** const argv) {

  // Declare a variable to memorize the returned code
  int retCode = 0;

  // Create an instance of CBo
  CBo* cbo = CBoCreate();

  // Process the arguments from the command line
  bool success = 
    CBoProcessCmdLineArguments(
      cbo, 
      argc, 
      argv);

  // If we could process the command line arguments
  if (success == true) {

    // Check all the files
    success = CBoCheckAllFiles(cbo);

    // If all the files were correct
    if (success == true) {
      
      printf("All files were checked successfully. Nice job guy !\n");
      
    // Else, at least one file contained error
    } else {

      // Set the return code
      retCode = 1;

    }

  // Else, the command line is incorrect
  } else {

    // Set the return code
    retCode = 1;

  }

  // Free the instance of CBo
  CBoFree(&cbo);

  // Return success code
  return retCode;

}

