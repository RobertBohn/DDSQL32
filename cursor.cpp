#include <cursor.h>
#include <column.h>

#define DEFER_PARSE  1
#define VERSION_7    2

/*----------------------------------------------------------------------------*/
/*  Constructor                                                               */
/*----------------------------------------------------------------------------*/
Cursor::Cursor( Lda_Def * lda )
{
    oopen( &cda, lda, NULL, -1, -1, NULL, -1 );
}

/*----------------------------------------------------------------------------*/
/*  Destructor                                                                */
/*----------------------------------------------------------------------------*/
Cursor::~Cursor()
{
    oclose( &cda );
}

/*----------------------------------------------------------------------------*/
/*  Return cursor value                                                       */
/*----------------------------------------------------------------------------*/
Cursor::operator Cda_Def * ()
{
    return &cda;
}

/*----------------------------------------------------------------------------*/
/*  Commit cursor                                                             */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::commit()
{
    return execute( "COMMIT" );
}

/*----------------------------------------------------------------------------*/
/*  Roll Back Cursor                                                          */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::rollBack()
{
    return execute( "ROLLBACK" );
}

/*----------------------------------------------------------------------------*/
/*  Compile an SQL statement                                                  */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::compile( const char * pszCmd )
{
    return oparse( &cda, (unsigned char*)pszCmd, -1, DEFER_PARSE, VERSION_7 ) ? cda.rc : 0;
}

/*----------------------------------------------------------------------------*/
/*  Execute the current SQL statement                                         */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::execute()
{
    return oexec( &cda ) ? cda.rc : 0;
}

/*----------------------------------------------------------------------------*/
/*  Execute a specified SQL statement                                         */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::execute( const char * pszCmd )
{
    int iRC = compile( pszCmd );
    if ( iRC == 0 )
    {
        iRC = execute();
    }
    return iRC;
}

/*----------------------------------------------------------------------------*/
/*  Perform a Fetch on the Cursor                                             */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::fetch()
{
    ofetch( &cda );
    return cda.rc == 1405 ? 0 : cda.rc;
}

/*----------------------------------------------------------------------------*/
/* Fetch a row from the table                                                 */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::fetch( Column * ppColumns[] )
{
    int iRetCode = fetch();
    if ( ppColumns )
    {
        for ( int i = 0; ppColumns[i] && iRetCode == 0; i++ )
        {
            iRetCode = ppColumns[i]->fetch( *this, (i+1) );
        }
    }
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Describe a Column                                                          */
/*----------------------------------------------------------------------------*/
SQLTRCD Cursor::describe( int col, long* dbsize, int* dbtype, char* buf,
long* buflen, long* dsize, int* precision, int* scale, int* nullok )
{
    return odescr( &cda, col, dbsize, (short *)dbtype, (signed char *)buf, buflen, dsize, (short *)precision, (short *)scale, (short *)nullok );
}

