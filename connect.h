//  Oracle API Class Library
//  Copyright (c) Datadesigns 1997
//  All Rights Reserved.

#ifndef __CONNECT_H
#define __CONNECT_H

#ifndef __ORACLE__
#define __ORACLE__
#endif

#ifndef __SQL_H
#include <sql.h>
#endif

class Connection
{
public:
    Connection( const char * user, const char * psw, const char * service );
    Connection( const char * connectString );
    ~Connection( void );
    operator Lda_Def * ();
    int status( void ) const;
private:
    Lda_Def lda;
    unsigned char hda[256];
};

#endif // __CONNECT_H

