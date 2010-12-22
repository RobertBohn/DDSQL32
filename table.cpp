#include <stdlib.h>
#include <stdio.h>
#include <table.h>

/*----------------------------------------------------------------------------*/
/* Constructor                                                                */
/*----------------------------------------------------------------------------*/
Table::Table( Cursor & theCursor, const char * pszName )
{
    poCursor = &theCursor;
    psName = new String( pszName );
}

/*----------------------------------------------------------------------------*/
/* Destructor                                                                 */
/*----------------------------------------------------------------------------*/
Table::~Table()
{
    if ( psName ) delete psName;
}

/*----------------------------------------------------------------------------*/
/* Create Table - intended to be overidden by derived classes                 */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::create()
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/* Create Table                                                               */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::create( Column * ppColumns[] )
{
    String oCmd( "CREATE TABLE " );
    oCmd += name();
    oCmd += " ( ";

    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i ) oCmd += ", ";
        oCmd += ppColumns[i]->name();

        char work[16];
        switch ( ppColumns[i]->type() )
        {
            case SQLECHR :
                oCmd += " CHAR (";
                sprintf( work, "%d", ppColumns[i]->size() - 1 );
                oCmd += work;
                oCmd += ")";
                break;
            case SQLESMA :
            case SQLEINT :
                oCmd += " INTEGER";
                break;
            case SQLERAW :
                oCmd += " LONG RAW";
                break;
            case SQLEVAR :
                oCmd += " VARCHAR (";
                sprintf( work, "%d", ppColumns[i]->size() - 1 );
                oCmd += work;
                oCmd += ")";
                break;
            case SQLEDEC :
                oCmd += " DECIMAL (";
                sprintf( work, "%d", ppColumns[i]->size() - ppColumns[i]->precision() - 3 );
                oCmd += work;
                oCmd += ",";
                sprintf( work, "%d", ppColumns[i]->precision() );
                oCmd += work;
                oCmd += ")";
                break;
            case SQLEDAT :           // DATE
            case SQLETIM :           // TIME
            case SQLETMS :           // TIMESTAMP
                oCmd += " DATE";
                break;
            default :
                oCmd += " INTEGER";
                break;
        }
    }
    oCmd += " )";

    return poCursor->execute( oCmd );
}

/*----------------------------------------------------------------------------*/
/* Drop Table                                                                 */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::drop()
{
    String oCmd( "DROP TABLE " );
    oCmd += name();
    return poCursor->execute( oCmd );
}

/*----------------------------------------------------------------------------*/
/* Return Table Name                                                          */
/*----------------------------------------------------------------------------*/
const char * Table::name() const
{
    return *psName;
}

/*----------------------------------------------------------------------------*/
/* Select statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::select( const char * pszCommand, Column * ppColumns[] )
{
    SQLTRCD iRetCode = poCursor->compile( pszCommand );
    if ( iRetCode == 0 )
    {
        iRetCode = setSelectBuffers( ppColumns );
    }
    if ( iRetCode == 0 )
    {
        iRetCode = poCursor->execute();
    }
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Set Select Buffers                                                         */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::setSelectBuffers( Column * ppColumns[] )
{
    int iRetCode = 0;
    if ( ppColumns )
    {
        for ( int i = 0; ppColumns[i] && iRetCode == 0; i++ )
        {
            iRetCode = ppColumns[i]->setSelectBuffer( *poCursor, (i+1) );
        }
    }
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Fetch a row from the table                                                 */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::fetch( Column * ppColumns[] )
{
    FILE* f;
    char* path;

    int rc = poCursor->fetch();

    while ( rc == RAWSELCT )
    {
        Cda_Def* cda = *poCursor;
        unsigned char piece;
        unsigned char raw[512];
        unsigned long size= sizeof(raw);
        unsigned long iteration;
        unsigned long psql;

        ogetpi( cda, &piece, (void**)&path, &iteration, &psql );

        if ( piece == SQL_RAW_FIRST )
        {
            if ( strlen(path) == 0 ) strcpy( path, _tempnam( getenv("TEMP"), "~RAW" ) );
            f = fopen( path, "wb+" );
            if ( !f ) return RAWERROR;
        }

        osetpi( cda, piece, raw, &size );
        ofetch( cda );

        if ( fwrite( raw, 1, size, f ) != size )
        {
            fclose( f );
            return RAWERROR;
        }   

        rc = cda->rc;

        if ( rc != RAWSELCT ) fclose( f );
    }

    if ( ppColumns )
        for ( int i = 0; ppColumns[i] && rc == 0; i++ )
            rc = ppColumns[i]->fetch( *poCursor, (i+1) );

    return rc;
}

/*----------------------------------------------------------------------------*/
/* Fetch a row from the table                                                 */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::fetch( Column & col )
{
    Column* pp[] = { &col, NULL };
    return fetch( pp );
}

/*----------------------------------------------------------------------------*/
/* Execute the Tables Cursor                                                  */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::execute()
{
    int rc = poCursor->execute();

    FILE * f = NULL;

    while ( rc == RAWUPDTE )
    {
        char * path = NULL;
        Cda_Def * cda = *poCursor;
        unsigned char piece;
        unsigned char raw[512];
        long size = sizeof(raw);
        unsigned long iteration;
        unsigned long psql;

        ogetpi( cda, &piece, (void**)&path, &iteration, &psql );

        if ( !f )
        {
            if ( path ) f = fopen( path, "rb" );
            if ( !f )
            {
                rc = RAWERROR;  // open error
                break;
            }
        }

        size = fread( raw, 1, sizeof(raw), f );
        if ( size < 0 )
        {
            fclose( f );
            rc = RAWERROR;    // read error
            break;
        }

        if ( size < sizeof(raw) ) piece = SQL_RAW_LAST;
        osetpi( cda, piece, raw, (unsigned long*)&size );
        oexec( cda );
        rc = cda->rc;
        if ( rc != RAWUPDTE ) fclose( f );
    }

    return rc;
}

/*----------------------------------------------------------------------------*/
/* Insert statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::insert( const char * pszCommand, Column * ppColumns[] )
{    
    SQLTRCD rc = poCursor->compile( pszCommand );
    if ( rc == 0 ) rc = bind( ppColumns );
    if ( rc == 0 ) rc = execute();
    return rc;
}

/*----------------------------------------------------------------------------*/
/* Bind variables by column number                                            */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::bind( Column * ppColumns[] )
{
    int iRetCode = 0;
    if ( ppColumns )
    {
        for ( int i = 0; ppColumns[i] && iRetCode == 0; i++ )
        {
            iRetCode = ppColumns[i]->bind( *poCursor, (i+1) );
        }
    }
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Insert statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::insert( Column * ppColumns[] )
{
    String oInsert( "INSERT INTO " );
    String oValues( " VALUES " );
    char pszWork[64];
    oInsert += name();
    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i == 0 )
        {
            oInsert += " ( ";
            oValues += " ( ";
        }
        else
        {
            oInsert += ", ";
            oValues += ", ";
        }

        if ( ppColumns[i]->type() == SQLEDAT )
            sprintf( pszWork, "TO_DATE(:%d,'YYYY-MM-DD')", (i+1) );
        else if ( ppColumns[i]->type() == SQLETMS )
            sprintf( pszWork, "TO_DATE(:%d,'YYYY-MM-DD HH24:MI:SS')", (i+1) );
        else
            sprintf( pszWork, ":%d", (i+1) );

        oInsert += ppColumns[i]->name();
        oValues += pszWork;
    }
    oInsert += ") ";
    oValues += ')';
    oInsert += oValues;
    return ( insert( (const char *)oInsert, ppColumns ) );
}

/*----------------------------------------------------------------------------*/
/* Select statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::select( Column * ppColumns[] )
{
    Column none( "", SQLEINT );
    return( select( none, ppColumns ) );
}

/*----------------------------------------------------------------------------*/
/* Select statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::select( Column & oKey, Column * ppColumns[] )
{
    char pszWork[256];
    String oStatement( "SELECT " );
    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += ", ";
        }

        if ( ppColumns[i]->type() == SQLEDAT )
            sprintf( pszWork, "TO_CHAR(%s,'YYYY-MM-DD')", (const char*)ppColumns[i]->name() );
        else if ( ppColumns[i]->type() == SQLETMS )
            sprintf( pszWork, "TO_CHAR(%s,'YYYY-MM-DD HH24:MI:SS')", (const char*)ppColumns[i]->name() );
        else
            strcpy( pszWork, ppColumns[i]->name() );

        oStatement += pszWork;
    }
    oStatement += " FROM ";
    oStatement += name();

    if ( strlen( oKey.name() ) )
    {
        oStatement += " WHERE ";
        oStatement += oKey.name();
        oStatement += " = :1";
    }

    SQLTRCD iRetCode = poCursor->compile( (const char *)oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    if ( strlen( oKey.name() ) )
    {
        iRetCode = oKey.bind( *poCursor, 1 );
        if ( iRetCode )
        {
            return iRetCode;
        }
    }
    iRetCode = setSelectBuffers( ppColumns );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = poCursor->execute();
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = fetch( ppColumns );
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Delete statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::deleteRow( Column & oKey )
{
    String oStatement( "DELETE FROM " );
    oStatement += name();
    oStatement += " WHERE ";
    oStatement += oKey.name();
    oStatement += " = :1";

    SQLTRCD iRetCode = poCursor->compile( oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = oKey.bind( *poCursor, 1 );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = poCursor->execute();
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Update statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::updateRow( Column & oKey, Column * ppColumns[] )
{
    String oStatement( "UPDATE " );
    oStatement += name();
    oStatement += " SET ";
    char pszWork[256];
    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += ", ";
        }
        oStatement += ppColumns[i]->name();

        if ( ppColumns[i]->type() == SQLEDAT )
            sprintf( pszWork, " = TO_DATE(:%d,'YYYY-MM-DD')", (i+1) );
        else if ( ppColumns[i]->type() == SQLETMS )
            sprintf( pszWork, " = TO_DATE(:%d,'YYYY-MM-DD HH24:MI:SS')", (i+1) );
        else
            sprintf( pszWork, " = :%d", (i+1) );

        oStatement += pszWork;
    }
    oStatement += " WHERE ";
    oStatement += oKey.name();
    sprintf( pszWork, " = :%d", ( i + 1 ) );
    oStatement += pszWork;

    SQLTRCD iRetCode = poCursor->compile( oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = bind( ppColumns );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = oKey.bind( *poCursor, ( i + 1 ) );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = execute();

    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Update statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::updateRow( Column * ppKey[], Column * ppColumns[] )
{
    String oStatement( "UPDATE " );
    oStatement += name();
    oStatement += " SET ";
    char pszWork[10];
    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += ", ";
        }
        oStatement += ppColumns[i]->name();
        sprintf( pszWork, " = :%d", ( i + 1 ) );
        oStatement += pszWork;
    }

    oStatement += " WHERE ";
    for ( int x = 0; ppKey[x]; x++ )
    {
        if ( x > 0 )
        {
            oStatement += " AND ";
        }
        oStatement += ppKey[x]->name();
        sprintf( pszWork, " = :%d", (i + x + 1) );
        oStatement += pszWork;
    }

    SQLTRCD iRetCode = poCursor->compile( oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = bind( ppColumns );
    if ( iRetCode )
    {
        return iRetCode;
    }

    for ( x = 0; ppKey[x]; x++ )
    {
        iRetCode = ppKey[x]->bind( *poCursor, ( i + x + 1 ) );
        if ( iRetCode )
        {
            return iRetCode;
        }
    }

    iRetCode = execute();
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Add an Index                                                               */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::addIndex( const char * pszName, Column * ppColumns[], int iUnique )
{
    String oCmd( "CREATE " );
    if ( iUnique ) oCmd += "UNIQUE ";
    oCmd += "INDEX ";
    oCmd += pszName;
    oCmd += " ON ";
    oCmd += name();
    oCmd += " ( ";

    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i ) oCmd += ", ";
        oCmd += ppColumns[i]->name();
    }
    oCmd += " )";

    return poCursor->execute( oCmd );
}

/*----------------------------------------------------------------------------*/
/* Add an Index                                                               */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::addIndex( const char * pszName, Column & col, int iUnique )
{
    Column* pp[] = { &col, NULL };
    return addIndex( pszName, pp, iUnique );
}

/*----------------------------------------------------------------------------*/
/* Select statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::select( Column * ppKey[], Column * ppColumns[] )
{
    char pszWork[256];
    String oStatement( "SELECT " );
    for ( int i = 0; ppColumns[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += ", ";
        }             

        if ( ppColumns[i]->type() == SQLEDAT )
            sprintf( pszWork, "TO_CHAR(%s,'YYYY-MM-DD')", (const char*)ppColumns[i]->name() );
        else if ( ppColumns[i]->type() == SQLETMS )
            sprintf( pszWork, "TO_CHAR(%s,'YYYY-MM-DD HH24:MI:SS')", (const char*)ppColumns[i]->name() );
        else
            strcpy( pszWork, ppColumns[i]->name() );

        oStatement += pszWork;
    }
    oStatement += " FROM ";
    oStatement += name();

    oStatement += " WHERE ";
    for ( i=0; ppKey[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += " AND ";
        }
        char pszWork[10];
        sprintf( pszWork, " = :%d", ( i + 1 ) );
        oStatement += ppKey[i]->name();
        oStatement += pszWork;
    }

    SQLTRCD iRetCode = poCursor->compile( (const char *)oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = bind( ppKey );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = setSelectBuffers( ppColumns );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = poCursor->execute();
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = fetch( ppColumns );
    return iRetCode;
}

/*----------------------------------------------------------------------------*/
/* Delete statement                                                           */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::deleteRow( Column * ppKey[] )
{
    String oStatement( "DELETE FROM " );
    oStatement += name();
    oStatement += " WHERE ";

    for ( int i=0; ppKey[i]; i++ )
    {
        if ( i > 0 )
        {
            oStatement += " AND ";
        }

        char pszWork[256];
        if ( ppKey[i]->type() == SQLEDAT )
            sprintf( pszWork, " = TO_DATE(:%d,'YYYY-MM-DD')", (i+1) );
        else if ( ppKey[i]->type() == SQLETMS )
            sprintf( pszWork, " = TO_DATE(:%d,'YYYY-MM-DD HH24:MI:SS')", (i+1) );
        else
            sprintf( pszWork, " = :%d", (i+1) );

        oStatement += ppKey[i]->name();
        oStatement += pszWork;
    }

    SQLTRCD iRetCode = poCursor->compile( (const char *)oStatement );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = bind( ppKey );
    if ( iRetCode )
    {
        return iRetCode;
    }
    iRetCode = poCursor->execute();
    if ( iRetCode )
    {
        return iRetCode;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/
/* Change internal name of table                                              */
/*----------------------------------------------------------------------------*/
SQLTRCD Table::setName( const char * pszName )
{
    if ( psName ) delete psName;
    psName = new String( pszName );
    return 0;
}

SQLTRCD Table::updateRow( Column & oKey, Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return updateRow( oKey, pp );
}

SQLTRCD Table::updateRow( Column * ppKey[], Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return updateRow( ppKey, pp );
}

SQLTRCD Table::select( Column & oKey, Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return select( oKey, pp );
}

SQLTRCD Table::select( Column * ppKey[], Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return select( ppKey, pp );
}

SQLTRCD Table::select( const char * psz, Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return select( psz, pp );
}

SQLTRCD Table::select( Column & oCol )
{
    Column* pp[] = { &oCol, NULL };
    return select( pp );
}

