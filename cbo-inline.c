// *************** CBO-INLINE.C ***************

// ================ Functions implementation ====================

// Get the number of file to check
#if BUILDMODE != 0
inline
#endif
int CBoGetNbFiles(const CBo* const that) {

#if BUILDMODE == 0
  if (that == NULL) {

    CBoErr->_type = PBErrTypeNullPointer;
    sprintf(CBoErr->_msg, "'that' is null");
    PBErrCatch(CBoErr);

  }
#endif
  return GSetNbElem(&(that->filePaths));

}

