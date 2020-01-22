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
  int a;
} CBo;

// ================ Functions declaration ====================

// Function to create a new CBo,
// Return a pointer toward the new CBo
CBo* CBoCreate(void);

// Function to free the memory used by the CBo 'that'
void CBoFree(CBo** that);

// ================ static inliner ====================

#if BUILDMODE != 0
#include "cbo-inline.c"
#endif

#endif
