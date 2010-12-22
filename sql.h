//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __SQL_H
#define __SQL_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

extern "C"
{
#include <stdio.h>
#include <oratypes.h>
#include <ociapr.h>
#include <ocidem.h>
}

// Gutpa Equivelant Definitions
#define SQLTRCD int             // Return Code
#define SQLTDAP unsigned char * // Buffer Pointer
#define SQLTPDL int             // Decimal Places
#define SQLTSCA int             // Scale
#define SQLSNUM  12             // numeric program buffer size
#define SQLSCDA  26             // character date-time size
#define SQLTLSI unsigned long   // long size
#define SQLTDAL unsigned int    // data length

// Data Types
#define SQLEINT   1     // INTEGER
#define SQLESMA   2     // SMALLINT
#define SQLECHR   4     // CHAR
#define SQLEVAR   5     // VARCHAR
#define SQLELON   6     // LONG DATA
#define SQLEDEC   7     // DECIMAL
#define SQLEDAT   8     // DATE
#define SQLETIM   9     // TIME
#define SQLETMS  10     // TIMESTAMP
#define SQLERAW  11     // RAW LONG

// Long Raw Bind Pieces
#define SQL_RAW_FIRST    1
#define SQL_RAW_NEXT     2
#define SQL_RAW_LAST     3

// Error Codes
#define ROWENUD     1   // Insert/update of unique constrained columns with duplicate data
#define FETEEOF  1403   // End of Fetch
#define PRSENEV   947   // Number of INSERT values not equal to number of target columns
#define PRSETMV   913   // Number of INSERT values exceeds number of target columns
#define EXEEIVN  1722   // Invalid number
#define EVAENOF  1438   // Numeric overflow
#define EXEEVIN  1400   // INSERT/UPDATE value is NULL & target column cannot contain nulls
#define EXEEDNN  1858   // Data is not numeric
#define EXEEDND  1843   // Data is not a date
#define EXEENIR  1847   // Numeric date value not in range
#define DICEITN   942   // Table <table name> has not been created
#define DLUEICN   904   // Invalid column name <column name>
#define DICEICN  1007   // Invalid column number
#define EVAEIVD  1839   // Invalid day value
#define EFORMAT  1830   // Bad Date Format
#define RAWUPDTE 3129   // Insert/Update Long Raw Data
#define RAWSELCT 3130   // Fetch Long Raw Data
#define RAWERROR 1098   // Error During Long Raw Data I/O Operation

#endif // __SQL_H    

