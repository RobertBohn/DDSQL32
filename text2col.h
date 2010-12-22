//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __TEXT2COL_H
#define __TEXT2COL_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

#ifndef __COLUMN_H
#include <column.h>
#endif

#ifndef _STRNG_H
#include <strng.h>
#endif

extern int text2Column( const char *, Column * [] );
extern String& column2String( Column * [] );

#endif // __TEXT2COL_H

