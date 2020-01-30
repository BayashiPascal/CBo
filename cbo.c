// *************** CBO.C ***************

// ================= Include ==================
#include "cbo.h"
#if BUILDMODE == 0
#include "cbo-inline.c"
#endif

// Label for the file types
const char* cboFileTypeStr[] = {

  "unknown",
  "C header",
  "C body"

};

// ================ Functions declaration ==================

// Check the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheck(
  CBoFile* const that,
  CBo* const cbo);

// Function to detect the type of a file from its path
// Return a CBoFileType
CBoFileType CBoFileGetTypeFromPath(const char* const filePath);

// Check the length of line on the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckLineLength(
  CBoFile* const that,
  CBo* const cbo);

// Check there is no trailing spaces on the lines of the CBoFile 'that'
// with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckTrailingSpace(
  CBoFile* const that,
  CBo* const cbo);

// Check there is an empty line before closing curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeClosingCurlyBrace(
  CBoFile* const that,
  CBo* const cbo);

// Check there is an empty line after opening curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineAfterOpeningCurlyBrace(
  CBoFile* const that,
  CBo* const cbo);

// Check there is an empty line after closing curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineAfterClosingCurlyBrace(
  CBoFile* const that,
  CBo* const cbo);

// Check there is no several blank lines in the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSeveralBlankLines(
  CBoFile* const that,
  CBo* const cbo);

// Check there is a space after a comma and no before in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundComma(
  CBoFile* const that,
  CBo* const cbo);

// Check there is no space before semicolon in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundSemicolon(
  CBoFile* const that,
  CBo* const cbo);

// Check there is no opening curly brace on the head of lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckNoCurlyBraceAtHead(
  CBoFile* const that,
  CBo* const cbo);

// Check there is no closing curly brace on the tail of lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckNoCurlyBraceAtTail(
  CBoFile* const that,
  CBo* const cbo);

// Function to check if a line is a comment
// Return true if it's a comment, else false
bool CBoLineIsComment(const CBoLine* const that);

// Return the position of the first character different of space or tab
// or 0 if the line is empty
unsigned int CBoLineGetPosHead(const CBoLine* const that);

// Function to get the length of a line
unsigned int CBoLineGetLength(const CBoLine* const that);

// Function to get the position of the closing character from the
// opening character at position 'from'
// Return the position if ofund, or 'from' if not found
unsigned int CBoLineGetPosCloseCharFrom(
  const CBoLine* const that,
  const unsigned int from);

// Function to get the position of the opening character from the
// closing character at position 'from'
// Return the position if found, or 'from' if not found
unsigned int CBoLineGetPosOpenCharFrom(
  const CBoLine* const that,
  const unsigned int from);

// Return the position of the last character 'c' excluding the string
// content, or the length of the string if the character could not be
// found
unsigned int CBoLineGetPosLast(
  const CBoLine* const that,
  const char c);

// ================ Functions implementation ==================

// Function to create a new CBo,
// Return a pointer toward the new CBo
CBo* CBoCreate(void) {

  // Allocate memory for the CBo
  CBo* that = PBErrMalloc(CBoErr, sizeof(CBo));

  // Init the properties
  that->filePaths = GSetStrCreateStatic();
  that->files = GSetCreateStatic();

  // Return the new CBo
  return that;

}

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** const that) {

  if (that == NULL || *that == NULL) return;

  // Free memory used by properties
  GSetFlush(&((*that)->filePaths));
  while (GSetNbElem(&((*that)->files)) > 0) {

    CBoFile* file = GSetPop(&((*that)->files));
    CBoFileFree(&file);

  }

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

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Loop on arguments
  for (int iArg = 1;
       iArg < argc;
       ++iArg) {

    if (strcmp(argv[iArg], "-help") == 0) {

      printf("cbo\n");
      printf("[-help] : print the help message\n");
      printf("\n");

    } else {

      // Any other arguments is considered to be a path
      // to a file to check

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

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize if all the file were loaded
  bool allLoaded = true;

  // Declare a variable to memorize if all the file were correct
  bool allCorrect = true;

  // If there are files to check
  if (CBoGetNbFiles(that) > 0) {

    // Create a progress bar
    ProgBarTxt progBar = ProgBarTxtCreateStatic();

    // Loop on the files
    GSetIterForward iterFilePath =
      GSetIterForwardCreateStatic(&(that->filePaths));
    int iFilePath = 0;
    do {

      // Update and display the progress bar
      ProgBarTxtSet(
        &progBar,
        (float)iFilePath / (float)CBoGetNbFiles(that));
      printf(
        "Loading %d files... %s\r",
        CBoGetNbFiles(that),
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the file path
      const char* filePath = GSetIterGet(&iterFilePath);

      // Load the file
      CBoFile* file = CBoFileCreate(filePath);

      // If we could load the file
      if (file != NULL) {

        // Add the file to the list of files
        GSetAppend(
          &(that->files),
          file);

      // Else, we couldn't load the file
      } else {

        // Display a message
        printf(
          "\nFailed to load [%s]\n",
          filePath);

        // Update the flag
        allLoaded = false;

      }

      ++iFilePath;

    } while (GSetIterStep(&iterFilePath) == true);

    // Update and display the progress bar
    ProgBarTxtSet(
      &progBar,
      (float)iFilePath / (float)CBoGetNbFiles(that));
    printf(
      "Loaded %d files     %s\n",
      CBoGetNbFiles(that),
      ProgBarTxtGet(&progBar));
    fflush(stdout);

    // Loop on the loaded files
    GSetIterForward iterFile =
      GSetIterForwardCreateStatic(&(that->files));
    do {

      // Get the file
      CBoFile* file = GSetIterGet(&iterFile);

      // Check the file
      allCorrect &=
        CBoFileCheck(
          file,
          that);

    } while (GSetIterStep(&iterFile) == true);

  }

  // Return the flag
  return allLoaded & allCorrect;

}

// Function to create a new CBoFile from its file path,
// Return a pointer toward the new CBoFile
CBoFile* CBoFileCreate(const char* const filePath) {

#if BUILDMODE == 0
  if (filePath == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'filePath' is null");
    PBErrCatch(CBoErr);

  }

#endif
  // Create the new CBoFile
  CBoFile* that = (CBoFile*)malloc(sizeof(CBoFile));

  // Initialise properties
  that->filePath = filePath;
  that->type = CBoFileGetTypeFromPath(filePath);
  that->lines = GSetCreateStatic();

  // Declare a variable to manage error while reading the lines
  int ret = !EOF;

  // If it's a known type
  if (that->type != CBoFileType_unknown) {

    // Open the file
    FILE* fp =
      fopen(
        filePath,
        "r");

    // Declare a buffer to read one line
    char buffer[1000];

    // Loop on the lines of the file
    while (!feof(fp) &&
           ret != EOF) {

      // Create a pointer on the char of the line
      char* ptr = buffer;
      *ptr = '\0';

      // Loop one the char of the line until the end of the line or
      // an error occured or we reach the end of the file or the end
      // of the buffer
      while (*ptr != '\n' &&
             !feof(fp) &&
             ret != EOF &&
             ptr < buffer + 999) {

        // Read one char
        ret =
          fscanf(
            fp,
            "%c",
            ptr);

        // If we could read the character and it wasn't the end
        // of the line
        if (ret != EOF &&
            *ptr != '\n') {

          // Move to the next char
          ++ptr;

        }

      }

      // If there was an error
      if ((!feof(fp) && ret == EOF) ||
          ptr >= buffer + 999) {

        ret = EOF;

      // Else there was no error while reading the line
      } else {

        // Add the null character to the end of the string
        *ptr = '\0';

        // Create the CBoLine
        CBoLine* line = CBoLineCreate(buffer);

        // If we couldn't create the CBoLine
        if (line != NULL) {

          GSetAppend(
            &(that->lines),
            line);

        } else {

          ret = EOF;

        }

      }

    }

    // If there has been an error
    if (!feof(fp) &&
        ret == EOF) {

      CBoFileFree(&that);

    }

    // Close the file
    fclose(fp);

  }

  // Return the new CBoFile
  return that;

}

// Function to detect the type of a file from its path
// Return a CBoFileType
CBoFileType CBoFileGetTypeFromPath(const char* const filePath) {

#if BUILDMODE == 0
  if (filePath == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'filePath' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Get the position of the last '.'
  char* ptr = strrchr(filePath, '.');

  // If we could find the last '.'
  if (ptr != NULL) {

    if (strcmp(ptr, ".c") == 0) {

      return CBoFileType_C_body;

    } else if (strcmp(ptr, ".h") == 0) {

      return CBoFileType_C_header;

    }

  }

  // By default return 'unknown'
  return CBoFileType_unknown;

}

// Function to free the memory used by the CBoFile 'that'
void CBoFileFree(CBoFile** const that) {

  if (that == NULL || *that == NULL) return;

  // Free the lines
  while (GSetNbElem(&((*that)->lines)) > 0) {

    CBoLine* line = GSetPop(&((*that)->lines));
    CBoLineFree(&line);

  }

  // Free the CBoFile
  free(*that);
  *that = NULL;

}

// Function to create a new CBoLine from its content,
// Return a pointer toward the new CBoLine
CBoLine* CBoLineCreate(const char* const str) {

#if BUILDMODE == 0
  if (str == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'str' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare the new CBoLine
  CBoLine* that = (CBoLine*)malloc(sizeof(CBoLine));

  if (that != NULL) {

    // Copy the content
    that->str = strdup(str);

    // If we couldn't copy the content
    if (that->str == NULL) {

      free(that);
      that = NULL;

    }

  }

  // Return the CBoLine
  return that;

}

// Function to free the memory used by the CBoLine 'that'
void CBoLineFree(CBoLine** const that) {

  if (that == NULL || *that == NULL) return;

  // Free the line content
  free((*that)->str);

  // Free the CBoLine
  free(*that);
  *that = NULL;

}

// Check the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheck(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Display an info message
  char* msg =
    SGRString(
      SGR_ColorFG(125, 125, 255, "=== Check file [%s] as %s ==="));
  printf(
    msg,
    that->filePath,
    cboFileTypeStr[that->type]);
  printf("\n");
  fflush(stdout);
  free(msg);

  // Declare a variable to memorize the success
  bool success = true;

  // Check the rules according to the type of file
  if (that->type == CBoFileType_C_header ||
      that->type == CBoFileType_C_body) {

    success &=
      CBoFileCheckLineLength(
        that,
        cbo);
    success &=
      CBoFileCheckTrailingSpace(
        that,
        cbo);
    success &=
      CBoFileCheckEmptyLineBeforeClosingCurlyBrace(
        that,
        cbo);
    success &=
      CBoFileCheckEmptyLineAfterOpeningCurlyBrace(
        that,
        cbo);
    success &=
      CBoFileCheckEmptyLineAfterClosingCurlyBrace(
        that,
        cbo);
    success &=
      CBoFileCheckSpaceAroundComma(
        that,
        cbo);
    success &=
      CBoFileCheckSpaceAroundSemicolon(
        that,
        cbo);
    success &=
      CBoFileCheckSeveralBlankLines(
        that,
        cbo);
    success &=
      CBoFileCheckNoCurlyBraceAtHead(
        that,
        cbo);
    success &=
      CBoFileCheckNoCurlyBraceAtTail(
        that,
        cbo);

  }

  // Return the successfull code
  return success;

}

// Check the length of line on the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckLineLength(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

    // Loop on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckLineLength %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // If the length is too long
      if (length > CBOLINE_MAX_LENGTH) {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0, "%s:%d Line too long."));
        char* errLine =
          SGRString(
            SGR_ColorBG(50, 50, 50, "%s"));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        printf(
          errLine,
          line->str);
        printf("\n");
        free(errMsg);
        free(errLine);
        fflush(stdout);

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckLineLength %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check theere is no trailing spaces on the lines of the CBoFile 'that'
// with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckTrailingSpace(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

    // Loop on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckTrailingSpace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // If the last char of the line is a space or a tab
      if (length > 0 &&
          (line->str[length - 1] == ' ' ||
           line->str[length - 1] == '\t')) {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0, "%s:%d Trailing space(s) or tab(s)."));
        char* errLine =
          SGRString(
            SGR_ColorBG(50, 50, 50, "%s"));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        printf(
          errLine,
          line->str);
        printf("\n");
        free(errMsg);
        free(errLine);
        fflush(stdout);

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckTrailingSpace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is an empty line before closing curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeClosingCurlyBrace(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Variable to memorize the previous non comment line
    CBoLine* prevLine = NULL;

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckEmptyLineBeforeClosingCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Get the position of the head of line and previous line
      unsigned int posHead = CBoLineGetPosHead(line);

      // If the line is not empty and starts with a closing curly
      // brace and the previous line is not empty and not a comment
      if (length > 0 &&
          prevLine != NULL &&
          CBoLineGetLength(prevLine) != 0 &&
          CBoLineIsComment(prevLine) == false &&
          line->str[posHead] == '}') {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0,
              "%s:%d Missing empty line before closing curly brace."));
        char* errLine =
          SGRString(
            SGR_ColorBG(50, 50, 50, "%s"));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        printf(
          errLine,
          prevLine->str);
        printf("\n");
        printf(
          errLine,
          line->str);
        printf("\n");
        free(errMsg);
        free(errLine);
        fflush(stdout);

      }

      // If the line is not a comment
      if (CBoLineIsComment(line) == false) {

        // Update the previous line
        prevLine = line;

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckEmptyLineBeforeClosingCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is an empty line after opening curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineAfterOpeningCurlyBrace(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Variable to memorize the previous line
    CBoLine* prevLine = NULL;

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Skip the first line
    prevLine = GSetIterGet(&iter);

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckEmptyLineAfterOpeningCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the previous line ends with a closing curly brace
      // and the line is not empty
      if (prevLength > 0 &&
          prevLine->str[prevLength - 1] == '{' &&
          length != 0) {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0,
              "%s:%d Missing empty line after opening curly brace."));
        char* errLine =
          SGRString(
            SGR_ColorBG(50, 50, 50, "%s"));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        printf(
          errLine,
          prevLine->str);
        printf("\n");
        printf(
          errLine,
          line->str);
        printf("\n");
        free(errMsg);
        free(errLine);
        fflush(stdout);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckEmptyLineAfterOpeningCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is an empty line after closing curly braces of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineAfterClosingCurlyBrace(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Variable to memorize the previous line
    CBoLine* prevLine = NULL;

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Skip the first line
    prevLine = GSetIterGet(&iter);

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckEmptyLineAfterClosingCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the line is a closing curly brace and the previous line
      // is not empty or a comment
      if (prevLength > 0 &&
          CBoLineIsComment(prevLine) == false &&
          prevLine->str[prevLength - 1] == '}' &&
          length != 0) {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0,
              "%s:%d Missing empty line after closing curly brace."));
        char* errLine =
          SGRString(
            SGR_ColorBG(50, 50, 50, "%s"));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        printf(
          errLine,
          prevLine->str);
        printf("\n");
        printf(
          errLine,
          line->str);
        printf("\n");
        free(errMsg);
        free(errLine);
        fflush(stdout);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckEmptyLineAfterClosingCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is no several blank lines in the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSeveralBlankLines(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Variable to memorize the previous line
    CBoLine* prevLine = NULL;

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Skip the first line
    prevLine = GSetIterGet(&iter);

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckSeveralBlankLine %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the line is a closing curly brace and the previous line
      // is not empty or a comment
      if (prevLength == 0 &&
          length == 0) {

        // Update the success flag
        success = false;

        // Display an error message
        char* errMsg =
          SGRString(
            SGR_ColorFG(255, 0, 0,
              "%s:%d Several blank lines."));
        printf("\n");
        printf(
          errMsg,
          that->filePath,
          iLine + 1);
        printf("\n");
        free(errMsg);
        fflush(stdout);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckSeveralBlankLine %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is a space after a comma and no before in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundComma(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckSpaceAroundComma %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // If the line is not a comment
      if (CBoLineIsComment(line) == false) {

        // Get the length of the line
        unsigned int length = CBoLineGetLength(line);

        // Declare two flags to memorize the strings in the code
        bool flagQuote = false;
        bool flagDoubleQuote = false;

        // Loop on the char of the line
        for (unsigned int iChar = 0;
             iChar < length;
             ++iChar) {

          if (line->str[iChar] == '\'') {

            if (flagDoubleQuote == false) {

              flagQuote = !flagQuote;

            }

          } else if (line->str[iChar] == '"') {

            if (flagQuote == false) {

              flagDoubleQuote = !flagDoubleQuote;

            }

          }

          // If the char is a comma and the previous one is a space
          if (flagQuote == false &&
              flagDoubleQuote == false &&
              line->str[iChar] == ',' &&
              (iChar == 0 ||
               line->str[iChar - 1] == ' ' ||
               line->str[iChar - 1] == '\t')) {

            // Update the success flag
            success = false;

            // Display an error message
            char* errMsg =
              SGRString(
                SGR_ColorFG(255, 0, 0,
                  "%s:%d Space before comma."));
            char* errLine =
              SGRString(
                SGR_ColorBG(50, 50, 50, "%s"));
            printf("\n");
            printf(
              errMsg,
              that->filePath,
              iLine + 1);
            printf("\n");
            printf(
              errLine,
              line->str);
            printf("\n");
            free(errMsg);
            free(errLine);
            fflush(stdout);

            // Skip the end of the line
            iChar = length;

          // If the char is a comma and the next one is a space
          } else if (flagQuote == false &&
                     flagDoubleQuote == false &&
                     line->str[iChar] == ',' &&
                     line->str[iChar + 1] != ' ' &&
                     iChar != length - 1) {

            // Update the success flag
            success = false;

            // Display an error message
            char* errMsg =
              SGRString(
                SGR_ColorFG(255, 0, 0,
                  "%s:%d No space after comma."));
            char* errLine =
              SGRString(
                SGR_ColorBG(50, 50, 50, "%s"));
            printf("\n");
            printf(
              errMsg,
              that->filePath,
              iLine + 1);
            printf("\n");
            printf(
              errLine,
              line->str);
            printf("\n");
            free(errMsg);
            free(errLine);
            fflush(stdout);

            // Skip the end of the line
            iChar = length;

          }

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckSpaceAroundComma %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is no space before semicolon in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundSemicolon(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckSpaceAroundSemicolon %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Declare two flags to memorize the strings in the code
      bool flagQuote = false;
      bool flagDoubleQuote = false;

      // Loop on the char of the line
      for (unsigned int iChar = 0;
           iChar < length;
           ++iChar) {

        if (line->str[iChar] == '\'') {

          if (flagDoubleQuote == false) {

            flagQuote = !flagQuote;

          }

        } else if (line->str[iChar] == '"') {

          if (flagQuote == false) {

            flagDoubleQuote = !flagDoubleQuote;

          }

        }

        // If the char is a semicolon and the previous one is a space
        if (flagQuote == false &&
            flagDoubleQuote == false &&
            line->str[iChar] == ';' &&
            (iChar == 0 ||
             line->str[iChar - 1] == ' ' ||
             line->str[iChar - 1] == '\t')) {

          // Update the success flag
          success = false;

          // Display an error message
          char* errMsg =
            SGRString(
              SGR_ColorFG(255, 0, 0,
                "%s:%d Space before semicolon."));
          char* errLine =
            SGRString(
              SGR_ColorBG(50, 50, 50, "%s"));
          printf("\n");
          printf(
            errMsg,
            that->filePath,
            iLine + 1);
          printf("\n");
          printf(
            errLine,
            line->str);
          printf("\n");
          free(errMsg);
          free(errLine);
          fflush(stdout);

          // Skip the end of the line
          iChar = length;

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckSpaceAroundSemicolon %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is no opening curly brace on the head without its
// closing curly brace on the same line of lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckNoCurlyBraceAtHead(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckNoCurlyBraceAtHead %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the position of the head of the line
      unsigned int posHead = CBoLineGetPosHead(line);

      // If the line starts with an opening curly brace
      // and its closing brace is not on the same line
      if (line->str[posHead] == '{') {

        // Get the position of its closing brace
        unsigned int posCloseBrace =
          CBoLineGetPosCloseCharFrom(
            line,
            posHead);

        // If the closing brace is not on the same line
        if (posCloseBrace == posHead) {

          // Update the success flag
          success = false;

          // Display an error message
          char* errMsg =
            SGRString(
              SGR_ColorFG(255, 0, 0,
                "%s:%d Opening curly brace at head of line."
                ""));
          char* errLine =
            SGRString(
              SGR_ColorBG(50, 50, 50, "%s"));
          printf("\n");
          printf(
            errMsg,
            that->filePath,
            iLine + 1);
          printf("\n");
          printf(
            errLine,
            line->str);
          printf("\n");
          free(errMsg);
          free(errLine);
          fflush(stdout);

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckNoCurlyBraceAtHead %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Check there is no closing curly brace on the tail of lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckNoCurlyBraceAtTail(
  CBoFile* const that,
  CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 1) {

    // Declare an iterator on the lines
    GSetIterForward iter =
      GSetIterForwardCreateStatic(&(that->lines));

    // Loop on the lines
    unsigned int iLine = 0;
    do {

      // Update and display the ProgBar
      ProgBarTxtSet(
        &progBar,
        (float)iLine / (float)GSetNbElem(&(that->lines)));
      printf(
        "CheckNoCurlyBraceAtTail %s\r",
        ProgBarTxtGet(&progBar));
      fflush(stdout);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Get the position of the head of the line
      unsigned int posHead = CBoLineGetPosHead(line);

      // Get the position of the last closing curly brace
      unsigned int posLastCloseBrace =
        CBoLineGetPosLast(
          line,
          '}');

      // If the last closing brace is not at the head of the line
      if (posLastCloseBrace != length &&
          posHead != posLastCloseBrace) {

        // Get the position of its opening brace
        unsigned int posOpenBrace =
          CBoLineGetPosOpenCharFrom(
            line,
            posLastCloseBrace);

        // If the closing brace is not on the same line
        if (posOpenBrace == posLastCloseBrace) {

          // Update the success flag
          success = false;

          // Display an error message
          char* errMsg =
            SGRString(
              SGR_ColorFG(255, 0, 0,
                "%s:%d Closing curly brace at tail of line."));
          char* errLine =
            SGRString(
              SGR_ColorBG(50, 50, 50, "%s"));
          printf("\n");
          printf(
            errMsg,
            that->filePath,
            iLine + 1);
          printf("\n");
          printf(
            errLine,
            line->str);
          printf("\n");
          free(errMsg);
          free(errLine);
          fflush(stdout);

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    printf(
      "CheckNoCurlyBraceAtTail %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      printf(" OK");

    }

    printf("\n");
    fflush(stdout);

  }

  // Return the successfull code
  return success;

}

// Function to check if a line is a comment
// Return true if it's a comment, else false
bool CBoLineIsComment(const CBoLine* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Get the position of the head of the line
  unsigned int posHead = CBoLineGetPosHead(that);

  // If the line starts with '/'
  if (that->str[posHead] == '/') {

    // The line is a comment
    return true;

  // Else, the line doesn't start with '/'
  } else {

    // The line is not a comment
    return false;

  }

}

// Return the position of the first character different of space or tab
// or 0 if the line is empty
unsigned int CBoLineGetPosHead(const CBoLine* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the position of the head
  unsigned int posHead = 0;

  // Loop until the end of the line or a charcter different of space
  // or tab
  while (that->str[posHead] != '\0' &&
         (that->str[posHead] == ' ' ||
          that->str[posHead] == '\t')) {

    ++posHead;

  }

  // Return the position of the head
  return posHead;

}

// Function to get the length of a line
unsigned int CBoLineGetLength(const CBoLine* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Return the length of the line
  return strlen(that->str);

}

// Function to get the position of the closing character from the
// opening character at position 'from'
// Return the position if found, or 'from' if not found
unsigned int CBoLineGetPosCloseCharFrom(
  const CBoLine* const that,
  const unsigned int from) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the current position in the line
  unsigned int pos = from;

  // Declare a variable to memorize the block level
  unsigned int lvl = 0;

  // Get the length of the line
  unsigned int length = CBoLineGetLength(that);

  // Get the closing character corresponding to the opening one
  unsigned char closeChar = ' ';
  switch (that->str[from]) {

    case '{':
      closeChar = '}';
      break;
    case '(':
      closeChar = ')';
      break;
    case '[':
      closeChar = ']';
      break;
    default:
      break;

  }

  // Flag to escape the strings
  bool flagQuote = false;
  bool flagDoubleQuote = false;

  // Loop on the char of the line
  do {

    // If it's the beginning of a string
    if (flagQuote == false &&
        that->str[pos] == '"' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (flagDoubleQuote == false &&
        that->str[pos] == '\'' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (flagQuote == false &&
        flagDoubleQuote == false) {

      // If the character at current position is an opening char
      if (that->str[pos] == that->str[from]) {

        // Increment the block level
        ++lvl;

      // Else, if the character at current position is a closing char
      } else if (that->str[pos] == closeChar) {

        // Decrement the block level
        --lvl;

      }

    }

    ++pos;

  } while (pos < length &&
           lvl != 0);

  // If we have found the closing char
  if (lvl == 0) {

    // Return its position
    return pos - 1;

  // Else, we haven't found the closing char
  } else {

    // Return the 'from' position
    return from;

  }

}

// Function to get the position of the opening character from the
// closing character at position 'from'
// Return the position if found, or 'from' if not found
unsigned int CBoLineGetPosOpenCharFrom(
  const CBoLine* const that,
  const unsigned int from) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the current position in the line
  int pos = from;

  // Declare a variable to memorize the block level
  unsigned int lvl = 0;

  // Get the closing character corresponding to the opening one
  unsigned char openChar = ' ';
  switch (that->str[from]) {

    case '}':
      openChar = '{';
      break;
    case ')':
      openChar = '(';
      break;
    case ']':
      openChar = '[';
      break;
    default:
      break;

  }

  // Flag to escape the strings
  bool flagQuote = false;
  bool flagDoubleQuote = false;

  // Loop on the char of the line
  do {

    // If it's the beginning of a string
    if (flagQuote == false &&
        that->str[pos] == '"' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (flagDoubleQuote == false &&
        that->str[pos] == '\'' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (flagQuote == false &&
        flagDoubleQuote == false) {

      // If the character at current position is a closing char
      if (that->str[pos] == that->str[from]) {

        // Increment the block level
        ++lvl;

      // Else, if the character at current position is an opening char
      } else if (that->str[pos] == openChar) {

        // Decrement the block level
        --lvl;

      }

    }

    --pos;

  } while (pos >= 0 &&
           lvl != 0);

  // If we have found the opening char
  if (lvl == 0) {

    // Return its position
    return pos + 1;

  // Else, we haven't found the closing char
  } else {

    // Return the 'from' position
    return from;

  }

}

// Return the position of the last character 'c' excluding the string
// content, or the length of the string if the character could not be
// found
unsigned int CBoLineGetPosLast(
  const CBoLine* const that,
  const char c) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Get the length of the line
  int length = CBoLineGetLength(that);

  // Declare a variable to memorize the current position
  int pos = length - 1;

  // Flag to escape the strings
  bool flagQuote = false;
  bool flagDoubleQuote = false;

  // Loop on the char of the line
  do {

    // If it's the beginning of a string
    if (flagQuote == false &&
        that->str[pos] == '"' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (flagDoubleQuote == false &&
        that->str[pos] == '\'' &&
        pos > 0 &&
        that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (flagQuote == false &&
        flagDoubleQuote == false) {

      // If the character at the curren tposition is the searched
      // character 'c'
      if (that->str[pos] == c) {

        // Return the position
        return pos;

      }

    }

    --pos;

  } while (pos >= 0);

  // If we reach here, we haven't found the char 'c'
  return length;

}
