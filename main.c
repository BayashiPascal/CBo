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

    // Open the stream for output
    FILE* stream =
      fopen(
        "/dev/stdout",
        "w");

    // Check all the files
    success =
      CBoCheckAllFiles(
        cbo,
        stream);

    // If all the files were correct
    if (success == true) {

      fprintf(
        stream,
        "All %u files were checked successfully. Nice job guy !\n",
        CBoGetNbFiles(cbo));

    // Else, at least one file contained error
    } else {

      fprintf(
        stream,
        "%u error(s) in %u out of %u file(s) . Don't give up !\n",
        CBoGetNbErrors(cbo),
        CBoGetNbFilesWithError(cbo),
        CBoGetNbFiles(cbo));

      // Set the return code
      retCode = 1;

    }

    // Close the stream
    fclose(stream);

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
