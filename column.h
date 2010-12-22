//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __COLUMN_H
#define __COLUMN_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

#ifndef _STRNG_H
#include <strng.h>
#endif

#ifndef __CURSOR_H
#include <cursor.h>
#endif

class Column
{   
public:
    Column( const char * pszName, int iType, int iSize=0, int iPrecision=0 );
    ~Column( void );
    void Alter( const char * pszName, int iType, int iSize=0, int iPrecision=0 );
    operator const SQLTDAP () const;
    operator const char * () const;
    operator float () const;
    operator long () const;
    operator char () const;
    operator int () const;
    Column& operator=(Column&);
    Column& operator=(int);
    Column& operator=(char);
    Column& operator=(long);
    Column& operator=(float);
    Column& operator=(const char*);
    int setSelectBuffer( Cursor&, int ) const;
    int fetch( Cursor&, int );
    int bind( Cursor&, int ) const;
    int isNull( void ) const;
    const char * name( void ) const;
    int type( void ) const;
    SQLTPDL size( void ) const;
    SQLTSCA precision( void ) const;
private:
    char * pData;
    int iDataSize;
    int iDataType;
    int iDataPrecision;
    String * psName;
};

#endif // __COLUMN_H

