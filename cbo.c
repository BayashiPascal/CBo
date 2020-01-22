// *************** CBO.C ***************

// ================= Include ==================
#include "cbo.h"
#if BUILDMODE == 0
#include "cbo-inline.c"
#endif

// ================ Functions implementation ==================

// Function to create a new CBo,
// Return a pointer toward the new CBo
CBo* CBoCreate(void) {
  // Allocate memory for the CBo
  CBo* that = PBErrMalloc(CBoErr, sizeof(CBo));
  // Return the new CBo
  return that;
}

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** that) {
  if (that == NULL || *that == NULL) return;
  // Free the memory
  free(*that);
  // Set the pointer to null
  *that = NULL;
}

