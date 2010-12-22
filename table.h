//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __TABLE_H
#define __TABLE_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

#ifndef __COLUMN_H
#include <column.h>
#endif

class Table
{
public:
    Table( Cursor &, const char * pszName = "No Name" );
    ~Table( void );

    SQLTRCD create( void );
    SQLTRCD create( Column * [] );
    SQLTRCD addIndex( const char *, Column * [], int iUnique = 0 );
    SQLTRCD addIndex( const char *, Column &, int iUnique = 0 );
    SQLTRCD drop( void );
    SQLTRCD insert( Column * [] );
    SQLTRCD insert( const char *, Column * [] );
    SQLTRCD bind( Column * [] );
    SQLTRCD deleteRow( Column & );
    SQLTRCD deleteRow( Column * [] );
    SQLTRCD updateRow( Column &, Column * [] );
    SQLTRCD updateRow( Column * [], Column * [] );
    SQLTRCD updateRow( Column &, Column & );
    SQLTRCD updateRow( Column * [], Column & );
    SQLTRCD select( Column &, Column * [] );
    SQLTRCD select( Column * [], Column * [] );
    SQLTRCD select( const char *, Column * [] );
    SQLTRCD select( Column * [] );
    SQLTRCD select( Column &, Column & );
    SQLTRCD select( Column * [], Column & );
    SQLTRCD select( const char *, Column & );
    SQLTRCD select( Column & );
    SQLTRCD setSelectBuffers( Column * [] );
    SQLTRCD fetch( Column * [] );
    SQLTRCD fetch( Column & );
    SQLTRCD execute( void );
    SQLTRCD setName( const char * );
    const char * name( void ) const;

private:

    Cursor * poCursor;
    String * psName;
};

#endif // __TABLE_H

