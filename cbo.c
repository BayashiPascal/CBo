// *************** CBO.C ***************

// ================= Include ==================
#include "cbo.h"
#if BUILDMODE == 0
#include "cbo-inline.c"
#endif

// ================= Define ==================

// Max length of a line
#define CBOLINE_MAX_LENGTH 79

// Size of one indent level
#define CBOLINE_INDENT_SIZE 2

// ================= Data structures ===================

// Structure to memorize one line of a file
typedef struct CBoLine {

  // Original line
  char* str;

  // Correct indent level
  unsigned int indent;

} CBoLine;

// Enum for the file type
typedef enum CBoFileType {

  CBoFileType_unknown,
  CBoFileType_C_header,
  CBoFileType_C_body

} CBoFileType;

// Structure to memorize the info about one file
typedef struct CBoFile {

  // Pointer to the file path
  const char* filePath;

  // Type of the file
  CBoFileType type;

  // Set of lines
  GSet lines;

  // Set of CBoError
  GSet errors;

} CBoFile;

// Enum for the error type
typedef enum CBoErrorType {

  CBoErrorType_LineLength,
  CBoErrorType_TrailingSpace,
  CBoErrorType_EmptyLineBeforeClosingCurlyBrace,
  CBoErrorType_EmptyLineAfterOpeningCurlyBrace,
  CBoErrorType_EmptyLineAfterClosingCurlyBrace,
  CBoErrorType_SpaceAroundComma,
  CBoErrorType_SpaceAroundSemicolon,
  CBoErrorType_SpaceAroundOperator,
  CBoErrorType_SeveralBlankLine,
  CBoErrorType_NoCurlyBraceAtHead,
  CBoErrorType_NoCurlyBraceAtTail,
  CBoErrorType_CharBeforeDot,
  CBoErrorType_SpaceBeforeOpeningCurlyBrace,
  CBoErrorType_EmptyLineBeforeComment,
  CBoErrorType_IndentLevel,
  CBoErrorType_SeveralArgOnOneLine,
  CBoErrorType_ArgumentsUnaligned,
  CBoErrorType_IndentTab,
  CBoErrorType_EmptyLineBeforeCase,
  CBoErrorType_MacroNameMustBeCapital,

} CBoErrorType;

// Structure to memorize the info about one error
typedef struct CBoError {

  // Pointer to the file containing the error
  CBoFile* file;

  // Line of the error
  CBoLine* line;

  // Index of the line of the error
  unsigned int iLine;

  // Type of the error
  CBoErrorType type;

} CBoError;

// ================= Global variables ===================

// Label for the file types
const char* cboFileTypeStr[] = {

  "unknown",
  "C header",
  "C body",

};

// Label for the error types
const char* cboErrorTypeStr[] = {

  "Line too long",
  "Trailing space(s) or tab(s)",
  "No empty line before closing curly brace",
  "No empty line after opening curly brace",
  "No empty line after closing curly brace",
  "No space after comma, or space before comma",
  "No space after semicolon, or space before semicolon",
  "No space around arythmetic/boolean operator",
  "Several consecutive blank lines",
  "Unbalanced curly brace at head of line",
  "Unbalanced curly brace at tail of line",
  "'.' must be after [a-zA-Z0-9])], or be at head of line",
  "No space before opening curly brace",
  "No empty line before comment",
  "Indent level is incorrect",
  "Several arguments on the same line",
  "Arguments of the function are not correctly aligned",
  "Don't use tab to indent lines",
  "No empty line or comment before case",
  "Macro name must be in capital letters"

};

// ================ Functions declaration ==================

// Function to create a new CBoFile from its file path,
// Return a pointer to the new CBoFile
CBoFile* CBoFileCreate(const char* const filePath);

// Function to free the memory used by the CBoFile 'that'
void CBoFileFree(CBoFile** const that);

// Calculate the proper indentation level of each line of the
// CBoFile 'that'
void CBoFileUpdateIndentLvlLines(CBoFile* const that);

// Function to add the CBoError 'error' to the CBoFile 'that'
void CBoFileAddError(
   CBoFile* const that,
  CBoError* const error);

// Display the errors of the CBoFile 'that' on the FILE 'stream'
void CBoFilePrintErrors(
  CBoFile* const that,
           FILE* stream);

// Function to create a new CBoLine from its content,
// Return a pointer to the new CBoLine
CBoLine* CBoLineCreate(const char* const str);

// Function to free the memory used by the CBoLine 'that'
void CBoLineFree(CBoLine** const that);

// Return the position of the last character 'c' excluding the string
// content, or the length of the string if the character could not be
// found
unsigned int CBoLineGetPosLast(
  const CBoLine* const that,
            const char c);

// Function to create a new CBoError from its file, line, index of line
// and type
// Return a pointer to the new CBoError
CBoError* CBoErrorCreate(
      CBoFile* const file,
      CBoLine* const line,
  const unsigned int iLine,
  const CBoErrorType type);

// Function to free the memory used by the CBoError 'that'
void CBoErrorFree(CBoError** const that);

// Function to display the CBoError 'that' on the stream 'stream'
void CBoErrorPrint(
  const CBoError* const that,
            FILE* const stream);

// Check the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheck(
  CBoFile* const that,
      CBo* const cbo);

// Function to detect the type of a file from its path
// Return a CBoFileType
CBoFileType CBoFileGetTypeFromPath(const char* const filePath);

// Return the number of errors in the CBoFile 'that'
unsigned int CBoFileGetNbError(const CBoFile* const that);

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

// Check there is a space around arythmetic/boolean operators
// in lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundOperator(
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

// Check that '.' are surrounded by [a-zA-Z0-9] or are at head of line
// on the lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckCharBeforeDot(
  CBoFile* const that,
      CBo* const cbo);

// Check there is a space or opening curly brace before opening curly
// braces on the lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceBeforeOpenCurlyBrace(
  CBoFile* const that,
      CBo* const cbo);

// Check there is an empty line before comments of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeComment(
  CBoFile* const that,
      CBo* const cbo);

// Check the indent level of the lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckIndentLevel(
  CBoFile* const that,
      CBo* const cbo);

// Check there is no several arguments on one line of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSeveralArgOnOneLine(
  CBoFile* const that,
      CBo* const cbo);

// Check tab are not used to indent lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckTabIndent(
  CBoFile* const that,
      CBo* const cbo);

// Check there is an empty line or comment before 'case' in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeCase(
  CBoFile* const that,
      CBo* const cbo);

// Check that macro names are all capital in lines of the  CBoFile 'that'
// with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckMacroNameAllCapital(
  CBoFile* const that,
      CBo* const cbo);

// Check the alignment of arguments in function declaration of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckAlignmentArg(
  CBoFile* const that,
      CBo* const cbo);

// Function to check if a line is a comment
// Return true if it's a comment, else false
bool CBoLineIsComment(const CBoLine* const that);

// Function to check if a line is a precompilation command
// Return true if it's a comment, else false
bool CBoFileIsLinePrecompilCmd(
  const CBoFile* const that,
    const unsigned int iLine);

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
// Return a pointer to the new CBo
CBo* CBoCreate(void) {

  // Allocate memory for the CBo
  CBo* that =
    PBErrMalloc(
      CBoErr,
      sizeof(CBo));

  // Init the properties
  that->filePaths = GSetStrCreateStatic();
  that->files = GSetCreateStatic();
  that->filesWithError = GSetCreateStatic();
  that->flagListFileError = false;

  // By default, set the output stream to stdout
  that->stream =
    fopen(
      "/dev/stdout",
      "w");

  // Return the new CBo
  return that;

}

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** const that) {

  if (that == NULL || *that == NULL) return;

  // Close the stream
  fclose((*that)->stream);

  // Free memory used by properties
  GSetFlush(&((*that)->filePaths));
  GSetFlush(&((*that)->filesWithError));
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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Loop on arguments
  for (
    int iArg = 1;
    iArg < argc;
    ++iArg) {

    int retStrCmp =
      strcmp(
        argv[iArg],
        "-help");
    if (retStrCmp == 0) {

      printf("cbo\n");
      printf("[-help] : print the help message\n");
      printf(
        "[-listFile] : print only the list of file(s) with error(s)\n");
      printf("\n");

    // Else, if the argument is -listFile
    } else {

      unsigned int retStrCmp =
        strcmp(
          argv[iArg],
          "-listFile");
      if (retStrCmp == 0) {

        // Update the flag
        that->flagListFileError = true;

      // Else, any other arguments is considered to be a path
      // to a file to check
      } else {

        // Try to open the file to check the path
        FILE* f =
          fopen(
            argv[iArg],
            "r");

        // If the path is correct
        if (f != NULL) {

          // Close the stream
          fclose(f);

          // Add the path to the list of files to check
          GSetAppend(
            &(that->filePaths),
            (char*)(argv[iArg]));

        // Else the path is incorrect
        } else {

          fprintf(
            that->stream,
            "The path [%s] is incorrect\n",
            argv[iArg]);
          return false;

        }

      }

    }

  }

  // Return the successfull code
  return true;

}

// Function to get the number of file with error in the CBo 'that'
unsigned int CBoGetNbFilesWithError(const CBo* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  return GSetNbElem(&(that->filesWithError));

}

// Function to get the total number of errors in the CBo 'that'
unsigned int CBoGetNbErrors(const CBo* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the number of errors
  unsigned int nb = 0;

  // If there is files
  if (CBoGetNbFiles(that) > 0) {

    // Loop on files
    GSetIterForward iter = GSetIterForwardCreateStatic(&(that->files));
    do {

      // Get the file
      CBoFile* file = GSetIterGet(&iter);

      // Add the number of errors on this file
      nb += CBoFileGetNbError(file);

    } while(GSetIterStep(&iter));

  }

  // Return the number of errors
  return nb;

}

// Check the files of the CBo 'that'
// Return true if there was no problem, else false
bool CBoCheckAllFiles(CBo* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize if all the file were loaded
  bool allLoaded = true;

  // Declare a variable to memorize if all the file were correct
  bool allCorrect = true;

  // Reset the set of files with error
  GSetFlush(&(that->filesWithError));

  // Declare a variable to memorize the current stream
  FILE* stream = that->stream;

  // If we display only the list of files with errors
  if (that->flagListFileError == true) {

    // Set the output stream to null
    that->stream =
      fopen(
        "/dev/null",
        "w");

  }

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
      fprintf(
        that->stream,
        "Loading %d files... %s\r",
        CBoGetNbFiles(that),
        ProgBarTxtGet(&progBar));
      fflush(that->stream);

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
        fprintf(
          stream,
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
    fprintf(
      that->stream,
      "Loaded %d files     %s\n",
      CBoGetNbFiles(that),
      ProgBarTxtGet(&progBar));
    fflush(that->stream);

    // Loop on the loaded files
    GSetIterForward iterFile =
      GSetIterForwardCreateStatic(&(that->files));
    do {

      // Get the file
      CBoFile* file = GSetIterGet(&iterFile);

      // Check the file
      bool correct =
        CBoFileCheck(
          file,
          that);

      // If the file has error(s)
      if (correct == false) {

        // If we display only the list of files with errors
        if (that->flagListFileError == true) {

          // Print the file path
          fprintf(
            stream,
            "%s\n",
            file->filePath);

        } else {

          // Display the errors of the file
          CBoFilePrintErrors(
            file,
            that->stream);

        }

        // Add it to the list of files with errors
        GSetAppend(
          &(that->filesWithError),
          file);

      }

      // Update the global flag
      allCorrect &= correct;

    } while (GSetIterStep(&iterFile) == true);

  }

  // If we display only the list of files with errors
  if (that->flagListFileError == true) {

    // Close the temporary null stream
    fclose(that->stream);

    // Reset the output stream
    that->stream = stream;

  }

  // Return the success flag
  return allLoaded & allCorrect;

}

// Function to create a new CBoFile from its file path,
// Return a pointer to the new CBoFile
CBoFile* CBoFileCreate(const char* const filePath) {

#if BUILDMODE == 0
  if (filePath == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'filePath' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Create the new CBoFile
  CBoFile* that = (CBoFile*)malloc(sizeof(CBoFile));

  // Initialise properties
  that->filePath = filePath;
  that->type = CBoFileGetTypeFromPath(filePath);
  that->lines = GSetCreateStatic();
  that->errors = GSetCreateStatic();

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
    while (!feof(fp) && ret != EOF) {

      // Create a pointer on the char of the line
      char* ptr = buffer;
      *ptr = '\0';

      // Loop on the char of the line until the end of the line or
      // an error occured or we reach the end of the file or the end
      // of the buffer
      while (
        *ptr != '\n' &&
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
        if (ret != EOF && *ptr != '\n') {

          // Move to the next char
          ++ptr;

        }

      }

      // If there was an error
      if ((!feof(fp) && ret == EOF) || ptr >= buffer + 999) {

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
    if (!feof(fp) && ret == EOF) {

      CBoFileFree(&that);

    }

    // Close the file
    fclose(fp);

  }

  // Calculate the proper indentation level of each line of the file
  CBoFileUpdateIndentLvlLines(that);

  // Return the new CBoFile
  return that;

}

// Function to detect the type of a file from its path
// Return a CBoFileType
CBoFileType CBoFileGetTypeFromPath(const char* const filePath) {

#if BUILDMODE == 0
  if (filePath == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'filePath' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Get the position of the last '.'
  char* ptr =
    strrchr(
      filePath,
      '.');

  // If we could find the last '.'
  if (ptr != NULL) {

    unsigned int retStrCmp =
      strcmp(
        ptr,
        ".c");
    if (retStrCmp == 0) {

      return CBoFileType_C_body;

    } else {

      retStrCmp =
        strcmp(
          ptr,
          ".h");
      if (retStrCmp == 0) {

        return CBoFileType_C_header;

      }

    }

  }

  // By default return 'unknown'
  return CBoFileType_unknown;

}

// Return the number of errors in the CBoFile 'that'
unsigned int CBoFileGetNbError(const CBoFile* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Return the number of error
  return GSetNbElem(&(that->errors));

}

// Function to free the memory used by the CBoFile 'that'
void CBoFileFree(CBoFile** const that) {

  if (that == NULL || *that == NULL) return;

  // Free the lines
  while (GSetNbElem(&((*that)->lines)) > 0) {

    CBoLine* line = GSetPop(&((*that)->lines));
    CBoLineFree(&line);

  }

  // Free the errors
  while (GSetNbElem(&((*that)->errors)) > 0) {

    CBoError* error = GSetPop(&((*that)->errors));
    CBoErrorFree(&error);

  }

  // Free the CBoFile
  free(*that);
  *that = NULL;

}

// Calculate the proper indentation level of each line of the
// CBoFile 'that'
void CBoFileUpdateIndentLvlLines(CBoFile* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the current indent level
  unsigned int indent = 0;

  // Declare a variable to memorise the index of the current line
  unsigned int iLine = 0;

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

    // Declare a flag to memorize the multiline lines
    bool flagMultiline = false;

    // Declare a flag to memorize the line inside parenthesis
    bool flagInParenthesis = false;

    // Declare flag to detect for (...) and case ...
    bool flagFor = false;
    bool flagCase = false;

    // Loop on the lines of the file
    GSetIterForward iter = GSetIterForwardCreateStatic(&(that->lines));
    do {

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Get the position of the head of the line
      unsigned int posHead = CBoLineGetPosHead(line);

      // Check if we are on a for(...)
      char* ptrFor =
        strstr(
          line->str,
          "for (");
      if (ptrFor == line->str + posHead) {

        flagFor = true;

      }

      // Check if we are on a case ...: or default:
      char* ptrCase =
        strstr(
          line->str,
          "case ");
      char* ptrDefault =
        strstr(
          line->str,
          "default:");
      if (
        ptrCase == line->str + posHead ||
        ptrDefault == line->str + posHead) {

        flagCase = true;

      }

      // If the line starts with '}' and is not a precompiler command
      bool isPrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);
      if (
        line->str[posHead] == '}' &&
        isPrecompilCmd == false) {

        // Decrement the indent level
        indent -= CBOLINE_INDENT_SIZE;

      }

      // Update the indent level of the line
      line->indent = indent;

      // If the line is not empty and not a comment and not a precompiler
      // command
      if (
        length > 0 &&
        CBoLineIsComment(line) == false &&
        isPrecompilCmd == false) {

        // If the line ends with '{'
        if (line->str[length - 1] == '{') {

          // If we are not on a line inside parenthesis
          if (flagInParenthesis == false) {

            // Increment the indent level
            indent += CBOLINE_INDENT_SIZE;

          }

          // Update the flag
          flagInParenthesis = false;
          flagFor = false;

        // If the line ends with '('
        } else if (line->str[length - 1] == '(') {

          // Increment the indent level
          indent += CBOLINE_INDENT_SIZE;

          // Update the flag
          flagInParenthesis = true;

        // If the line ends with '='
        } else if (line->str[length - 1] == '=') {

          // Update the multiline flag
          flagMultiline = true;

          // Increment the indent level
          indent += CBOLINE_INDENT_SIZE;

        // Else if the line ends with ';'
        } else if (line->str[length - 1] == ';') {

          // If we are on a multiline
          if (flagMultiline == true) {

            // Update the multiline flag
            flagMultiline = false;

            // Decrement the indent level
            indent -= CBOLINE_INDENT_SIZE;

          }

          // If we are inside parenthesis but not in a for (...)
          if (
            flagInParenthesis == true &&
            flagFor == false) {

            // Update the flag
            flagInParenthesis = false;

            // Decrement the indent level
            indent -= CBOLINE_INDENT_SIZE;

          }

          // If we are in a case ...
          if (flagCase == true) {

            // If the line is the break line of the case
            char* ptrBreak =
              strstr(
                line->str,
                "break;");
            if (ptrBreak == line->str + posHead) {

              // Update the flag
              flagCase = false;

              // Decrement the indent level
              indent -= CBOLINE_INDENT_SIZE;

            }

          }

        // Else if the line ends with ':'
        } else if (line->str[length - 1] == ':') {

          // Update the flag
          flagCase = true;

          // Decrement the indent level
          indent += CBOLINE_INDENT_SIZE;

        }

      }

      // Increment the index of the current line
      ++iLine;

    } while (GSetIterStep(&iter));

  }

}

// Function to add the CBoError 'error' to the CBoFile 'that'
void CBoFileAddError(
   CBoFile* const that,
  CBoError* const error) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (error == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'error' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Add the error sorted on the line index
  GSetAddSort(
    &(that->errors),
    error,
    error->iLine);

}

// Display the errors of the CBoFile 'that' on the FILE 'stream'
void CBoFilePrintErrors(
  CBoFile* const that,
           FILE* stream) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (stream == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'stream' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // If there are errors
  if (GSetNbElem(&(that->errors)) > 0) {

    // Loop on errors
    GSetIterForward iter = GSetIterForwardCreateStatic(&(that->errors));
    do {

      // Get the error
      CBoError* error = GSetIterGet(&iter);

      // Print the error
      CBoErrorPrint(
        error,
        stream);

    } while(GSetIterStep(&iter));

  }

}

// Function to create a new CBoLine from its content,
// Return a pointer to the new CBoLine
CBoLine* CBoLineCreate(const char* const str) {

#if BUILDMODE == 0
  if (str == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'str' is null");
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

    // Init the indent level
    that->indent = 0;

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

// Function to create a new CBoError from its file, line, index of line
// and type
// Return a pointer to the new CBoError
CBoError* CBoErrorCreate(
      CBoFile* const file,
      CBoLine* const line,
  const unsigned int iLine,
  const CBoErrorType type) {

#if BUILDMODE == 0
  if (file == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'file' is null");
    PBErrCatch(CBoErr);

  }

  if (line == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'line' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare the new CBoError
  CBoError* that = (CBoError*)malloc(sizeof(CBoError));

  if (that != NULL) {

    // Init properties
    that->file = file;
    that->iLine = iLine;
    that->line = line;
    that->type = type;

  }

  // Return the CBoLine
  return that;

}

// Function to free the memory used by the CBoError 'that'
void CBoErrorFree(CBoError** const that) {

  if (that == NULL || *that == NULL) return;

  // Free the CBoError
  free(*that);
  *that = NULL;

}

// Function to display the CBoError 'that' on the stream 'stream'
void CBoErrorPrint(
  const CBoError* const that,
            FILE* const stream) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (stream == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'stream' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Create the coloring pattern
  char* errMsg =
    SGRString(SGR_ColorFG(
      255,
      0,
      0,
      "%s:%u %s."));
  char* errLine =
    SGRString(SGR_ColorBG(
      50,
      50,
      50,
      "%s"));

  // Print the error message
  fprintf(
    stream,
    errMsg,
    that->file->filePath,
    that->iLine,
    cboErrorTypeStr[that->type]);
  fprintf(
    stream,
    "\n");

  // Print the error line
  fprintf(
    stream,
    errLine,
    that->line->str);
  fprintf(
    stream,
    "\n");

  // Free memory
  free(errMsg);
  free(errLine);

  // Flush the stream
  fflush(stream);

}

// Check the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheck(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // If the user hasn't requested to display only the list of
  // file(s) with error(s)
  if (cbo->flagListFileError == false) {

    // Display an info message
    char* msg =
      SGRString(SGR_ColorFG(
        125,
        125,
        255,
        "=== Check file [%s] as %s ==="));
    fprintf(
      cbo->stream,
      msg,
      that->filePath,
      cboFileTypeStr[that->type]);
    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);
    free(msg);

  }

  // Declare a variable to memorize the success
  bool success = true;

  // Check the rules according to the type of file
  if (
    that->type == CBoFileType_C_header ||
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
      CBoFileCheckTabIndent(
        that,
        cbo);
    success &=
      CBoFileCheckMacroNameAllCapital(
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
      CBoFileCheckSpaceAroundOperator(
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
    success &=
      CBoFileCheckCharBeforeDot(
        that,
        cbo);
    success &=
      CBoFileCheckSpaceBeforeOpenCurlyBrace(
        that,
        cbo);
    success &=
      CBoFileCheckEmptyLineBeforeComment(
        that,
        cbo);
    success &=
      CBoFileCheckSeveralArgOnOneLine(
        that,
        cbo);
    success &=
      CBoFileCheckAlignmentArg(
        that,
        cbo);
    success &=
      CBoFileCheckEmptyLineBeforeCase(
        that,
        cbo);

    // If there is still no problem, check the indentation only
    if (success == true) {

      success &=
        CBoFileCheckIndentLevel(
          that,
          cbo);

    }

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckLineLength %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // If the length is too long
      if (length > CBOLINE_MAX_LENGTH) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_LineLength);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckLineLength %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is no trailing spaces on the lines of the CBoFile 'that'
// with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckTrailingSpace(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckTrailingSpace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // If the last char of the line is a space or a tab
      if (
        length > 0 &&
        (line->str[length - 1] == ' ' ||
        line->str[length - 1] == '\t')) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_TrailingSpace);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckTrailingSpace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckEmptyLineBeforeClosingCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Get the length of the previous line
      unsigned int lengthPrevLine = 0;
      if (prevLine != NULL)
        lengthPrevLine = CBoLineGetLength(prevLine);

      // Get the position of the head of line and previous line
      unsigned int posHead = CBoLineGetPosHead(line);

      // If the line is not empty and starts with a closing curly
      // brace and the previous line is not empty and not a comment and
      // not terminating by antislash
      if (
        length > 0 &&
        prevLine != NULL &&
        CBoLineGetLength(prevLine) != 0 &&
        CBoLineIsComment(prevLine) == false &&
        line->str[posHead] == '}' &&
        (prevLine == NULL || prevLine->str[lengthPrevLine- 1] != '\\')) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_EmptyLineBeforeClosingCurlyBrace);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

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
    fprintf(
      cbo->stream,
      "CheckEmptyLineBeforeClosingCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckEmptyLineAfterOpeningCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the previous line is not a comment and ends with a
      // closing curly brace and the line is not empty
      if (
        prevLength > 0 &&
        CBoLineIsComment(prevLine) == false &&
        prevLine->str[prevLength - 1] == '{' &&
        length != 0) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_EmptyLineAfterOpeningCurlyBrace);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckEmptyLineAfterOpeningCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckEmptyLineAfterClosingCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the line is a closing curly brace and the previous line
      // is not empty or a comment
      if (
        prevLength > 0 &&
        CBoLineIsComment(prevLine) == false &&
        prevLine->str[prevLength - 1] == '}' &&
        length != 0) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_EmptyLineAfterClosingCurlyBrace);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckEmptyLineAfterClosingCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckSeveralBlankLine %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line and previous line
      unsigned int length = CBoLineGetLength(line);
      unsigned int prevLength = CBoLineGetLength(prevLine);

      // If the line is a closing curly brace and the previous line
      // is not empty or a comment
      if (prevLength == 0 && length == 0) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_SeveralBlankLine);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      prevLine = line;
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckSeveralBlankLine %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckSpaceAroundComma %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

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
        for (
          unsigned int iChar = 0;
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

          // If we are not in a string
          if (flagQuote == false && flagDoubleQuote == false) {

            // If the char is a comma and the previous one is a space
            if (
              line->str[iChar] == ',' &&
              (iChar == 0 ||
              line->str[iChar - 1] == ' ' ||
              line->str[iChar - 1] == '\t')) {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  line,
                  iLine + 1,
                  CBoErrorType_SpaceAroundComma);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

              // Skip the end of the line
              iChar = length;

            // If the char is a comma and the next one is a space
            } else if (
              line->str[iChar] == ',' &&
              line->str[iChar + 1] != ' ' &&
              iChar != length - 1) {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  line,
                  iLine + 1,
                  CBoErrorType_SpaceAroundComma);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

              // Skip the end of the line
              iChar = length;

            }

          }

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckSpaceAroundComma %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckSpaceAroundSemicolon %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // Declare two flags to memorize the strings in the code
      bool flagQuote = false;
      bool flagDoubleQuote = false;

      // Loop on the char of the line
      for (
        unsigned int iChar = 0;
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

        // If we are not in a string
        if (flagQuote == false && flagDoubleQuote == false) {

          // If the char is a semicolon and the previous one is a space
          if (
            line->str[iChar] == ';' &&
            (iChar == 0 ||
            line->str[iChar - 1] == ' ' ||
            line->str[iChar - 1] == '\t')) {

            // Update the success flag
            success = false;

            // Create the error
            CBoError* error =
              CBoErrorCreate(
                that,
                line,
                iLine + 1,
                CBoErrorType_SpaceAroundSemicolon);

            // Add the error to the file
            CBoFileAddError(
              that,
              error);

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
    fprintf(
      cbo->stream,
      "CheckSpaceAroundSemicolon %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is a space around arythmetic/boolean operators
// in lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceAroundOperator(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // Declare a variable to memorize the operators
  // TODO Exclude '*' and '&' due to complication when used as
  // pointer operator
  // TODO better rule for '-' also due to complication when used
  // as pointer operator
  char* operators = "+-/|";

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckSpaceAroundOperator %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the length of the line
      unsigned int length = CBoLineGetLength(line);

      // If the line is not a comment or a precompiler command
      bool isPrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);
      if (
        CBoLineIsComment(line) == false &&
        isPrecompilCmd == false) {

        // Declare two flags to memorize the strings in the code
        bool flagQuote = false;
        bool flagDoubleQuote = false;

        // Loop on the char of the line
        for (
          unsigned int iChar = 0;
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

          // If we are not in a string
          if (flagQuote == false && flagDoubleQuote == false) {

            // Search for the character in the possible operators
            char* ptr =
              strchr(
                operators,
                line->str[iChar]);

            // If the char is an operator
            if (ptr != NULL) {

              // If there is not the needed space before the operator
              if (
                line->str[iChar] != '-' &&
                iChar > 0 &&
                line->str[iChar - 1] != ' ' &&
                line->str[iChar - 1] != line->str[iChar] &&
                line->str[iChar + 1] != line->str[iChar]) {

                // Update the success flag
                success = false;

                // Create the error
                CBoError* error =
                  CBoErrorCreate(
                    that,
                    line,
                    iLine + 1,
                    CBoErrorType_SpaceAroundOperator);

                // Add the error to the file
                CBoFileAddError(
                  that,
                  error);

                // Skip the end of the line
                iChar = length;

              // If there is not the needed space after the operator '-'
              } else if (
                line->str[iChar] == '-' &&
                line->str[iChar + 1] != ' ' &&
                line->str[iChar + 1] != line->str[iChar] &&
                line->str[iChar + 1] != '=' &&
                line->str[iChar + 1] < '0' &&
                line->str[iChar + 1] > '9' &&
                line->str[iChar + 1] != '>') {

                // Update the success flag
                success = false;

                // Create the error
                CBoError* error =
                  CBoErrorCreate(
                    that,
                    line,
                    iLine + 1,
                    CBoErrorType_SpaceAroundOperator);

                // Add the error to the file
                CBoFileAddError(
                  that,
                  error);

                // Skip the end of the line
                iChar = length;

              // If there is not the needed space after the operator
              // other than '-'
              } else if (
                line->str[iChar] != '-' &&
                iChar < length - 1 &&
                line->str[iChar + 1] != ' ' &&
                line->str[iChar + 1] != line->str[iChar] &&
                line->str[iChar + 1] != '=' &&
                line->str[iChar - 1] != line->str[iChar]) {

                // Update the success flag
                success = false;

                // Create the error
                CBoError* error =
                  CBoErrorCreate(
                    that,
                    line,
                    iLine + 1,
                    CBoErrorType_SpaceAroundOperator);

                // Add the error to the file
                CBoFileAddError(
                  that,
                  error);

                // Skip the end of the line
                iChar = length;

              }

            }

          }

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckSpaceAroundOperator %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckNoCurlyBraceAtHead %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

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

          // Create the error
          CBoError* error =
            CBoErrorCreate(
              that,
              line,
              iLine + 1,
              CBoErrorType_NoCurlyBraceAtHead);

          // Add the error to the file
          CBoFileAddError(
            that,
            error);

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckNoCurlyBraceAtHead %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckNoCurlyBraceAtTail %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

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
      // and the line is not a comment
      if (
        CBoLineIsComment(line) == false &&
        posLastCloseBrace != length &&
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

          // Create the error
          CBoError* error =
            CBoErrorCreate(
              that,
              line,
              iLine + 1,
              CBoErrorType_NoCurlyBraceAtTail);

          // Add the error to the file
          CBoFileAddError(
            that,
            error);

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckNoCurlyBraceAtTail %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check that '.' are surrounded by [a-zA-Z0-9])] or are at head of line
// on the lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckCharBeforeDot(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckCharBeforeDot %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // If the line is not a comment
      if (CBoLineIsComment(line) == false) {

        // Get the position of the head of the line
        unsigned int posHead = CBoLineGetPosHead(line);

        // Get the length of the line
        unsigned int length = CBoLineGetLength(line);

        // Declare two variables to manage strings
        bool flagQuote = false;
        bool flagDoubleQuote = false;

        // Loop on the char in the line
        for (
          unsigned int iChar = 0;
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

          // If we are not in a string
          if (flagQuote == false && flagDoubleQuote == false) {

            // If the dot is not at the head of the line and
            // the char before the '.' is invalid
            if (
              line->str[iChar] == '.' &&
              !(iChar > 0 && line->str[iChar - 1] == '.') &&
              line->str[iChar + 1] != '.' &&
              (iChar == 0 ||
              (iChar != posHead &&
              !(line->str[iChar - 1] >= 'a' && line->str[iChar - 1] <= 'z') &&
              !(line->str[iChar - 1] >= 'A' && line->str[iChar - 1] <= 'Z') &&
              !(line->str[iChar - 1] >= '0' && line->str[iChar - 1] <= '9') &&
              line->str[iChar - 1] != ']' &&
              line->str[iChar - 1] != ')'))) {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  line,
                  iLine + 1,
                  CBoErrorType_CharBeforeDot);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

              // Skip the end of the line
              iChar = length;

            }

          }

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckCharBeforeDot %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is a space or opening curly brace before opening curly
// braces on the lines of the CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSpaceBeforeOpenCurlyBrace(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckSpaceBeforeOpenCurlyBrace %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // If the line is not a comment
      if (CBoLineIsComment(line) == false) {

        // Get the length of the line
        unsigned int length = CBoLineGetLength(line);

        // Declare two variables to manage strings
        bool flagQuote = false;
        bool flagDoubleQuote = false;

        // Loop on the char in the line
        for (
          unsigned int iChar = 0;
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

          // If we are not in a string
          if (flagQuote == false && flagDoubleQuote == false) {

            // If the char is an opening curly brace and not preceded
            // by another curly brace or space
            if (
              line->str[iChar] == '{' &&
              iChar > 0 &&
              line->str[iChar - 1] != ' ' &&
              line->str[iChar - 1] != '{') {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  line,
                  iLine + 1,
                  CBoErrorType_SpaceBeforeOpeningCurlyBrace);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

              // Skip the end of the line
              iChar = length;

            }

          }

        }

      }

      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckSpaceBeforeOpenCurlyBrace %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is an empty line before comments of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeComment(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
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
      fprintf(
        cbo->stream,
        "CheckEmptyLineBeforeComment %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // If the line is a comment and the previous line is not empty and
      // not a comment and we are not in a macro definition
      bool isLinePrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);
      if (
        prevLine != NULL &&
        CBoLineGetLength(prevLine) != 0 &&
        CBoLineIsComment(prevLine) == false &&
        CBoLineIsComment(line) == true &&
        isLinePrecompilCmd == false) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_EmptyLineBeforeComment);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      // Update the previous line
      prevLine = line;

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckEmptyLineBeforeComment %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check the indent level of the lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckIndentLevel(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckIndentLevel %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the next line
      GSetIterForward iterNext = iter;
      GSetIterStep(&iterNext);
      CBoLine* nextLine = GSetIterGet(&iterNext);

      // Get the position of the head of the line
      unsigned int posHead = CBoLineGetPosHead(line);

      // Get the length of the line
      unsigned int length = strlen(line->str);

      // Check if the line is a precompilation command
      bool isPrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);

      // If the line is a function declaration
      if (
        CBoLineIsComment(line) == false &&
        isPrecompilCmd == false &&
        posHead == 0 &&
        length > 0 &&
        line->str[length - 1] == '(') {

        // Variable to memorize the level in parenthesis
        unsigned int lvlPar = 1;

        // Skip the lines until the end of the function argument list
        do {

          // Get the line of the current argument
          line = GSetIterGet(&iter);

          // Declare a variable to memorize the position in the line
          unsigned int pos = 0;

          // Loop on the char of the line
          do {

            // If it's a opening parenthesis
            if (line->str[pos] == '(') {

              // Increase the level of parenthesis
              ++lvlPar;

            // Else if it's a closing parenthesis
            } else if (line->str[pos] == ')') {

              // Decrease the level of parenthesis
              --lvlPar;

            }

            // Move to the next character
            ++pos;

          } while(line->str[pos] != '\0');

        } while (
          lvlPar > 0 &&
          GSetIterStep(&iter));

      // Else, if the line is a comment
      } else if (CBoLineIsComment(line) == true) {

        // If the line is not indented as the next line
        if (
          nextLine != NULL &&
          posHead != CBoLineGetPosHead(nextLine)) {

          // Update the success flag
          success = false;

          // Create the error
          CBoError* error =
            CBoErrorCreate(
              that,
              line,
              iLine + 1,
              CBoErrorType_IndentLevel);

          // Add the error to the file
          CBoFileAddError(
            that,
            error);

        }

      // Else, the line is not a comment or precompiler command,
      // if the line is not correctly indented and not empty
      } else if (
        isPrecompilCmd == false &&
        posHead != length &&
        posHead != line->indent) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_IndentLevel);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckIndentLevel %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check tab are not used to indent lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckTabIndent(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckTabIndent %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the position of the head of the line
      unsigned int posHead = CBoLineGetPosHead(line);

      // Search for tab in the indentation
      char tmp = line->str[posHead];
      line->str[posHead] = '\0';
      char* posTab =
        strchr(
          line->str,
          '\t');
      line->str[posHead] = tmp;

      // If there are tabs in the indentation
      if (posTab != NULL) {

        // Update the success flag
        success = false;

        // Create the error
        CBoError* error =
          CBoErrorCreate(
            that,
            line,
            iLine + 1,
            CBoErrorType_IndentTab);

        // Add the error to the file
        CBoFileAddError(
          that,
          error);

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckTabIndent %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is an empty line or comment before 'case' in lines of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckEmptyLineBeforeCase(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckEmptyLineBeforeCase %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the next line
      GSetIterForward iterNext = iter;
      GSetIterStep(&iterNext);
      CBoLine* nextLine = GSetIterGet(&iterNext);

      // If there is a next line
      if (nextLine != NULL) {

        // Get the position of the head of the next line
        unsigned int posHead = CBoLineGetPosHead(nextLine);

        // Check if the next line starts with 'case ' or 'default:'
        char *posCase =
          strstr(
            nextLine->str + posHead,
            "case ");
        if (posCase == NULL) {

          posCase =
            strstr(
              nextLine->str + posHead,
              "default:");

        }

        bool startsWithCase = (posCase == nextLine->str + posHead);
        bool isPrecompilerCmd =
          CBoFileIsLinePrecompilCmd(
            that,
            iLine);

        // If the next line starts with 'case ' and the current line
        // is nor a blank line nor a comment nor a precompiled command
        if (
          CBoLineIsComment(line) == false &&
          isPrecompilerCmd == false &&
          line->str[0] != '\0' &&
          startsWithCase == true) {

          // Update the success flag
          success = false;

          // Create the error
          CBoError* error =
            CBoErrorCreate(
              that,
              nextLine,
              iLine + 2,
              CBoErrorType_EmptyLineBeforeCase);

          // Add the error to the file
          CBoFileAddError(
            that,
            error);

        }

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckEmptyLineBeforeCase %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check that macro names are all capital in lines of the  CBoFile 'that'
// with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckMacroNameAllCapital(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckMacroNameAllCapital %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the position of the head of thet line
      unsigned int posHead = CBoLineGetPosHead(line);

      // Check if the next line starts with '#define '
      char *posDefine =
        strstr(
          line->str + posHead,
          "#define ");

      bool startsWithDefine = (posDefine == line->str + posHead);

      // If the line starts with '#define'
      if (startsWithDefine == true) {

        // Loop on the macro name
        char* ptrName = posDefine + strlen("#define ");
        while (*ptrName != ' ' && *ptrName != '(' &&
          *ptrName != '\0' && success == true) {

          // If the name contains lower letter
          if (*ptrName >= 'a' && *ptrName <= 'z') {

            // Update the success flag
            success = false;

            // Create the error
            CBoError* error =
              CBoErrorCreate(
                that,
                line,
                iLine + 1,
                CBoErrorType_MacroNameMustBeCapital);

            // Add the error to the file
            CBoFileAddError(
              that,
              error);

          }

          ptrName++;

        }

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckMacroNameAllCapital %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check there is no several arguments on one line of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckSeveralArgOnOneLine(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CheckSeveralArgOnOneLine %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // If the line is not a comment and not a precompiler command
      bool isPrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);

      if (
        CBoLineIsComment(line) == false &&
        isPrecompilCmd == false) {

        // Flag to escape the strings
        bool flagQuote = false;
        bool flagDoubleQuote = false;

        // Variable to memorize the level in parenthesis, brackets and braces
        unsigned int lvlBracket = 0;
        unsigned int lvlBrace = 0;
        unsigned int lvlPar = 0;

        // Flag to remember if we are in a macro argument list
        bool macroArgument = false;

        // Declare a variable to memorize the position in the line
        unsigned int pos = 0;

        // Loop on the char of the line
        do {

          // If it's the beginning of a string
          if (
            flagQuote == false &&
            line->str[pos] == '"' &&
            pos > 0 &&
            line->str[pos - 1] != '\\') {

            // Update the flag
            flagDoubleQuote = !flagDoubleQuote;

          } else if (
            flagDoubleQuote == false &&
            line->str[pos] == '\'' &&
            pos > 0 &&
            line->str[pos - 1] != '\\') {

            // Update the flag
            flagQuote = !flagQuote;

          }

          // If we are not in a string
          if (flagQuote == false && flagDoubleQuote == false) {

            // If the character at current position is an opening
            // bracket
            if (line->str[pos] == '[') {

              // Update the level
              ++lvlBracket;

            // Else, if the character at current position is a closing
            // bracket
            } else if (line->str[pos] == ']') {

              // Update the level
              --lvlBracket;

            // If the character at current position is an opening
            // parenthesis
            } else if (line->str[pos] == '(') {

              // If we are at the first level
              if (lvlPar == 0) {

                // Check if the word before is a macro name (ie, all capital
                // letters)
                int posBack = pos - 1;
                bool allCapital = true;
                while (posBack >= 0 &&
                  allCapital == true &&
                  line->str[posBack] != ' ' &&
                  line->str[posBack] != '[' &&
                  line->str[posBack] != '{') {

                  if (line->str[posBack] >= 'a' && line->str[posBack] <= 'z') {

                    allCapital = false;

                  }

                  posBack--;

                }

                if (allCapital == true) macroArgument = true;

              }

              // Update the level
              ++lvlPar;

            // Else, if the character at current position is a closing
            // parenthesis
            } else if (line->str[pos] == ')') {

              // Update the level
              --lvlPar;

              // If we are back at the first level, reset the flag for
              // macro arguments
              if (lvlPar == 0) macroArgument = false;

            // Else, if the character at current position is an opening
            // brace
            } else if (line->str[pos] == '{') {

              // Update the level
              ++lvlBrace;

            // Else, if the character at current position is a closing
            // brace
            } else if (line->str[pos] == '}') {

              // Update the level
              --lvlBrace;

            // Else, if the character at current position is a comma
            // and we are outside of brackets and braces and the comma
            // is not at the tail of the line and we are not in a macro
            // arguments
            } else if (
              line->str[pos] == ',' &&
              lvlBracket == 0 &&
              lvlBrace == 0 &&
              line->str[pos + 1] != '\0' &&
              macroArgument == false) {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  line,
                  iLine + 1,
                  CBoErrorType_SeveralArgOnOneLine);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

            }

          }

          // Move to the next character
          ++pos;

        } while (line->str[pos] != '\0');

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CheckSeveralArgOnOneLine %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

  }

  // Return the successfull code
  return success;

}

// Check the alignment of arguments in function declaration of the
// CBoFile 'that' with the CBo 'cbo'
// Return true if there was no problem, else false
bool CBoFileCheckAlignmentArg(
  CBoFile* const that,
      CBo* const cbo) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

  if (cbo == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'cbo' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the success
  bool success = true;

  // Create a progress bar
  ProgBarTxt progBar = ProgBarTxtCreateStatic();

  // If the file is not empty
  if (GSetNbElem(&(that->lines)) > 0) {

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
      fprintf(
        cbo->stream,
        "CBoFileCheckAlignmentArg %s\r",
        ProgBarTxtGet(&progBar));
      fflush(cbo->stream);

      // Get the line
      CBoLine* line = GSetIterGet(&iter);

      // Get the position of the head and tail of the line
      unsigned int posHead = CBoLineGetPosHead(line);
      unsigned int lineLength = CBoLineGetLength(line);

      // If the line is not a comment and not a precompiler command
      // and is a function declaration with several arguments
      bool isPrecompilCmd =
        CBoFileIsLinePrecompilCmd(
          that,
          iLine);
      if (
        CBoLineIsComment(line) == false &&
        isPrecompilCmd == false &&
        posHead == 0 &&
        lineLength > 0 &&
        line->str[lineLength - 1] == '(') {

        // Variable to memorize the level in parenthesis
        unsigned int lvlPar = 1;

        // Declare a new iterator to iterate on the arguments
        GSetIterForward iterArg = iter;
        GSetIterStep(&iterArg);

        // Declare a variable to memorize the max position of the
        // last space
        unsigned int posMaxLastSpace = 0;

        // Loop until the end of the function argument list
        do {

          // Get the line of the current argument
          CBoLine* lineArg = GSetIterGet(&iterArg);

          // Declare a variable to memorize the position in the line
          unsigned int pos = 0;

          // Loop on the char of the line
          do {

            // If it's a opening parenthesis
            if (lineArg->str[pos] == '(') {

              // Increase the level of parenthesis
              ++lvlPar;

            // Else if it's a closing parenthesis
            } else if (lineArg->str[pos] == ')') {

              // Decrease the level of parenthesis
              --lvlPar;

            // Else if it's a space inside the parenthesis
            } else if (
              lvlPar > 0 &&
              lineArg->str[pos] == ' ') {

              if (pos > posMaxLastSpace) {

                posMaxLastSpace = pos;

              }

            }

            // Move to the next character
            ++pos;

          } while(lineArg->str[pos] != '\0');

        } while (
          lvlPar > 0 &&
          GSetIterStep(&iterArg));

        // Reset the level in parenthesis
        lvlPar = 1;

        // Reset the iterator to iterate on the arguments
        iterArg = iter;
        GSetIterStep(&iterArg);

        // Memorize the index of the argument line
        unsigned int iLineArg = iLine + 1;

        // Loop once again on the argument list to check the position
        // of the last space in each line against the max position in
        // all lines
        do {

          // Declare a variable to memorize the position of the
          // last space
          unsigned int posLastSpace = 0;

          // Get the line of the current argument
          CBoLine* lineArg = GSetIterGet(&iterArg);

          // Declare a variable to memorize the position in the line
          unsigned int pos = 0;

          // Loop on the char of the line
          do {

            // If it's a opening parenthesis
            if (lineArg->str[pos] == '(') {

              // Increase the level of parenthesis
              ++lvlPar;

            // Else if it's a closing parenthesis
            } else if (lineArg->str[pos] == ')') {

              // Decrease the level of parenthesis
              --lvlPar;

            // Else if it's a space inside the parenthesis
            } else if (
              lvlPar > 0 &&
              lineArg->str[pos] == ' ') {

              if (pos > posLastSpace) {

                posLastSpace = pos;

              }

            }

            // Move to the next character
            ++pos;

          } while(lineArg->str[pos] != '\0');

          // If the position of the last space is not equal to the
          // max position in the list of arguments
          if (posLastSpace != posMaxLastSpace) {

              // Update the success flag
              success = false;

              // Create the error
              CBoError* error =
                CBoErrorCreate(
                  that,
                  lineArg,
                  iLineArg + 1,
                  CBoErrorType_ArgumentsUnaligned);

              // Add the error to the file
              CBoFileAddError(
                that,
                error);

          }

          // Move to the next argument line
          ++iLineArg;

        } while (
          lvlPar > 0 &&
          GSetIterStep(&iterArg));

      }

      // Move to the next line
      ++iLine;

    } while (GSetIterStep(&iter));

    // Update and display the ProgBar
    ProgBarTxtSet(
      &progBar,
      1.0);
    fprintf(
      cbo->stream,
      "CBoFileCheckAlignmentArg %s",
      ProgBarTxtGet(&progBar));
    if (success == true) {

      fprintf(
        cbo->stream,
        " OK");

    }

    fprintf(
      cbo->stream,
      "\n");
    fflush(cbo->stream);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
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

// Function to check if a line is a precompilation command
// Return true if it's one, else false
bool CBoFileIsLinePrecompilCmd(
  const CBoFile* const that,
    const unsigned int iLine) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Get the line
  CBoLine* line =
    GSetGet(
      &(that->lines),
      iLine);

  // Get the position of the head of the line
  unsigned int posHead = CBoLineGetPosHead(line);

  // If the line starts with '#'
  if (line->str[posHead] == '#') {

    // The line is a precompiler command
    return true;

  // Else, the line doesn't start with '#'
  } else {

    // If the previous line is an unterminated precompilation command
    if (iLine > 0) {

      CBoLine* prevLine =
        GSetGet(
          &(that->lines),
          iLine - 1);
      unsigned int length = CBoLineGetLength(prevLine);
      if (
        length > 0 &&
        prevLine->str[length - 1] == '\\') {

        return
          CBoFileIsLinePrecompilCmd(
            that,
            iLine - 1);

      }

    }

    // The line is not a precompiler command
    return false;

  }

}

// Return the position of the first character different of space or tab
// or 0 if the line is empty
unsigned int CBoLineGetPosHead(const CBoLine* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(
      CBoErr->_msg,
      "'that' is null");
    PBErrCatch(CBoErr);

  }

#endif

  // Declare a variable to memorize the position of the head
  unsigned int posHead = 0;

  // Loop until the end of the line or a charcter different of space
  // or tab
  while (
    that->str[posHead] != '\0' &&
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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
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
    if (
      flagQuote == false &&
      that->str[pos] == '"' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (
      flagDoubleQuote == false &&
      that->str[pos] == '\'' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (flagQuote == false && flagDoubleQuote == false) {

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

  } while (pos < length && lvl != 0);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
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
    if (
      flagQuote == false &&
      that->str[pos] == '"' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (
      flagDoubleQuote == false &&
      that->str[pos] == '\'' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (
      flagQuote == false &&
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

  } while (pos >= 0 && lvl != 0);

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
    sprintf(
      CBoErr->_msg,
      "'that' is null");
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
    if (
      flagQuote == false &&
      that->str[pos] == '"' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagDoubleQuote = !flagDoubleQuote;

    } else if (
      flagDoubleQuote == false &&
      that->str[pos] == '\'' &&
      pos > 0 &&
      that->str[pos - 1] != '\\') {

      // Update the flag
      flagQuote = !flagQuote;

    }

    // If we are not in a string
    if (
      flagQuote == false &&
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
