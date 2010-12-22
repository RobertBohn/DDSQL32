//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __CURSOR_H
#define __CURSOR_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

#ifndef __SQL_H
#include <sql.h>
#endif

class Cursor
{
public:
    Cursor( Lda_Def * lda );
    ~Cursor( void );
    operator Cda_Def * ();
    SQLTRCD commit( void );
    SQLTRCD rollBack( void );
    SQLTRCD compile( const char * );
    SQLTRCD execute( const char * );
    SQLTRCD execute( void );
    SQLTRCD fetch( void );
    SQLTRCD fetch( class Column * [] );
    SQLTRCD describe( int col, long* dbsize, int* dbtype, char* buf, long* buflen, long* dsize, int* precision, int* scale, int* nullok );
private:
    Cda_Def cda;
};

#endif // __CURSOR_H

