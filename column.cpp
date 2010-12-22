#include <stdlib.h>
#include <stdio.h>
#include <column.h>
#include <time.h>
#include <io.h>

/*----------------------------------------------------------------------------*/
/*  Constructor by type                                                       */
/*----------------------------------------------------------------------------*/
Column::Column( const char * pszName, int iType, int iSize, int iPrecision )
{
    iDataType = iType;
    iDataPrecision = iPrecision;
    psName = new String( pszName );

    switch ( iDataType )
    {
        case SQLECHR :           // CHAR
            if ( iSize )
            {
                iDataSize = iSize + 1;
            }
            else
            {
                iDataSize = 2;
            }
            break;
        case SQLESMA :           // SMALLINT
            iDataSize = SQLSNUM;
            break;
        case SQLERAW :           // LONG RAW
            iDataSize = 512;
            break;
        case SQLEINT :           // INTEGER
            iDataSize = SQLSNUM;
            break;
        case SQLEVAR :           // VARCHAR
            iDataSize = iSize + 1;
            break;
        case SQLEDEC :           // DECIMAL
            iDataSize = iSize + iPrecision + 3;
            break;
        case SQLEDAT :           // DATE
        case SQLETIM :           // TIME
        case SQLETMS :           // TIMESTAMP
            iDataSize = 254;
            break;
        case SQLELON :           // LONG DATA
            iDataSize = 4096;
            break;
        default :                // unknown type
            iDataSize = 254;
            break;
    }
    pData = new char[ iDataSize ];
    memset( pData, 0x00, iDataSize );
}

/*----------------------------------------------------------------------------*/
/*  Destructor                                                                */
/*----------------------------------------------------------------------------*/
Column::~Column()
{
    delete psName;
    delete pData;
}

/*----------------------------------------------------------------------------*/
/*  Return data                                                               */
/*----------------------------------------------------------------------------*/
Column::operator const SQLTDAP () const
{
    return (SQLTDAP)pData;
}

Column::operator const char * () const
{
    return pData;
}

Column::operator int () const
{
    return (atoi(pData));
}

Column::operator long () const
{
    struct tm tblock;
    switch ( iDataType )
    {
        case SQLEDAT :
            if ( strlen( pData ) < strlen( "YYYY-MM-DD" ) ) return 0;
            tblock.tm_year = atoi( pData+0 ) - 1900;
            tblock.tm_mon  = atoi( pData+5 ) - 1;
            tblock.tm_mday = atoi( pData+8 );
            tblock.tm_hour = 0;
            tblock.tm_min  = 0;
            tblock.tm_sec  = 0;
            tblock.tm_isdst = -1;
            if ( tblock.tm_mon < 0 || tblock.tm_mon > 12 ) tblock.tm_mon = 0;
            return mktime( &tblock );

        case SQLETMS :
            if ( strlen( pData ) < strlen( "YYYY-MM-DD HH:MM:SS" ) ) return 0;
            tblock.tm_year = atoi( pData+0 ) - 1900;
            tblock.tm_mon  = atoi( pData+5 ) - 1;
            tblock.tm_mday = atoi( pData+8 );
            tblock.tm_hour = atoi( pData+11 );
            tblock.tm_min  = atoi( pData+14 );
            tblock.tm_sec  = atoi( pData+17 );
            tblock.tm_isdst = -1;     
            if ( tblock.tm_mon < 0 || tblock.tm_mon > 12 ) tblock.tm_mon = 0;
            return mktime( &tblock );

        default :
            return atol( pData );
    }
}

Column::operator char () const
{
    return ( *pData );
}

Column::operator float () const
{
    return (float)atof( pData );
}

/*----------------------------------------------------------------------------*/
/*  Return Column Name                                                        */
/*----------------------------------------------------------------------------*/
const char * Column::name() const
{
    return *psName;
}

/*----------------------------------------------------------------------------*/
/*  Return data precision                                                     */
/*----------------------------------------------------------------------------*/
SQLTSCA Column::precision() const
{
    return (SQLTSCA)iDataPrecision;
}

/*----------------------------------------------------------------------------*/
/*  Return data buffer size                                                   */
/*----------------------------------------------------------------------------*/
SQLTPDL Column::size() const
{
    return (SQLTPDL)iDataSize;
}

/*----------------------------------------------------------------------------*/
/*  Return data type                                                          */
/*----------------------------------------------------------------------------*/
int Column::type() const
{
    return iDataType;
}

/*----------------------------------------------------------------------------*/
/*  Return True if NULL, False if not NULL                                    */
/*----------------------------------------------------------------------------*/
int Column::isNull() const
{
    return( (int)( pData[0] == 0x00 ) );
}

/*----------------------------------------------------------------------------*/
/*  Fetch Data Into the Column                                                */
/*----------------------------------------------------------------------------*/
int Column::fetch( Cursor& oCursor, int iColumnNumber )
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/*  Set Select Buffer                                                         */
/*----------------------------------------------------------------------------*/
int Column::setSelectBuffer( Cursor& csr, int iColumnNumber ) const
{
    if ( iDataType == SQLERAW )
        return odefinps( csr,0,iColumnNumber,(unsigned char*)pData,MINSB4MAXVAL,SQLT_LBI,0,NULL,NULL,0,0,NULL,NULL,0,0,0,0);
    else
        return odefin( csr, iColumnNumber, (unsigned char*)pData, iDataSize, STRING_TYPE, -1, 0, NULL, -1, -1, 0, 0 ) ? ((Cda_Def *)csr)->rc : 0;
}

/*----------------------------------------------------------------------------*/
/*  Bind to Variable                                                          */
/*----------------------------------------------------------------------------*/
int Column::bind( Cursor& csr, int iColumnNumber ) const
{
    char buf[10];
    sprintf( buf, ":%d", iColumnNumber );

    if ( iDataType == SQLERAW )
        return obindps( csr,0,(unsigned char*)buf,-1,(unsigned char*)pData,MINSB4MAXVAL,SQLT_LBI,0,NULL,NULL,NULL,0,0,0,0,0,NULL,NULL,0,0 );
    else
        return obndrv( csr, (unsigned char*)buf, -1, (unsigned char*)pData, -1, STRING_TYPE, -1, 0, NULL, -1, -1 ) ? ((Cda_Def *)csr)->rc : 0;
}

/*----------------------------------------------------------------------------*/
/*  Update Column Data                                                        */
/*----------------------------------------------------------------------------*/
Column& Column::operator=(Column& c)
{
    *this = (const char*)c;
    return *this;
}

Column& Column::operator=(int i)
{
    sprintf( pData, "%d", i );
    return *this;
}

Column& Column::operator=(long l)
{
    struct tm * tblock;
    switch ( iDataType )
    {
        case SQLEDAT :
            strcpy( pData, "" );
            tblock = localtime( &l );
            if ( l ) sprintf( pData, "%4.4d-%2.2d-%2.2d",
                ( tblock->tm_year + 1900 ),
                ( tblock->tm_mon + 1 ),
                ( tblock->tm_mday ) );
            break;

        case SQLETMS :
            strcpy( pData, "" );
            tblock = localtime( &l );
            if ( l ) sprintf( pData, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d",
                ( tblock->tm_year + 1900 ),
                ( tblock->tm_mon + 1 ),
                ( tblock->tm_mday ),
                ( tblock->tm_hour ),
                ( tblock->tm_min  ),
                ( tblock->tm_sec  ) );
            break;

        default :
            sprintf( pData, "%ld", l );
            break;
    }
    return *this;
}

Column& Column::operator=(char c)
{
    *pData = c;
    return *this;
}
Column& Column::operator=(float f)
{
    sprintf( pData, "%.*f", iDataPrecision, f );
    return *this;
}

Column& Column::operator=(const char * s)
{

    if ( iDataType == SQLELON )
    {
        if ( s == NULL )
        {
            strcpy( pData, "" );
            return *this;
        }
        if ( pData ) delete pData;
        pData = new char[ ( strlen(s) ) + 1 ];
        strcpy( pData, s );
        return *this;
    }

    if ( s == NULL )
    {
        strcpy( pData, "" );
    }
    else
    {
        strncpy( pData, s, ( iDataSize - 1 ) );
        pData[ ( iDataSize - 1 ) ] = 0x00;
        if ( isNull() ) return *this;
        switch ( iDataType )
        {
            case SQLESMA :
            {
                sprintf( pData, "%d", atoi( pData ) );
                break;
            }

            case SQLEINT :
            {
                sprintf( pData, "%ld", atol( pData ) );
                break;
            }

            case SQLEDEC :
            {
                sprintf( pData, "%.*f", iDataPrecision, atof( pData ) );
                break;
            }

            case SQLEDAT :
            {
                //  When Assigining a Date With a String Support These Formats
                //  yyyy-m-d   m/d/yy(yy)   yyyymmdd   mmddyyyy   d-mon-yy(yy)

                int day, mon, yr;
                String str( s );
                str.findReplace( " ", "" );
                str.upper();

                // 1) yyyy-m-d

                yr = atoi( str );
                const char* pMon = strstr( str, "-" );
                if ( pMon )
                {
                    mon = atoi( ++pMon );
                    const char* pDay = strstr( pMon, "-" );
                    if ( pDay )
                    {
                        day = atoi( ++pDay );
                        if ( mon > 0 && mon < 13 && day > 0 && day < 32 && yr > 1900 && yr < 2100 )
                        {
                            sprintf( pData, "%4.4d-%2.2d-%2.2d", yr, mon, day );
                            return *this;
                        }
                    }
                }

                // 2) m/d/yy(yy)

                mon = atoi( str );
                const char* pDay = strstr( str, "/" );
                if ( pDay )
                {
                    day = atoi( ++pDay );
                    const char* pYr = strstr( pDay, "/" );
                    if ( pYr )
                    {
                        yr = atoi( ++pYr );
                        if ( yr > 0 && yr < 100 ) yr += 1900;

                        if ( mon > 0 && mon < 13 && day > 0 && day < 32 && yr > 1900 && yr < 2100 )
                        {
                            sprintf( pData, "%4.4d-%2.2d-%2.2d", yr, mon, day );
                            return *this;
                        }
                    }
                }

                // 3) yyyymmdd

                long l = atol( str );
                yr  = l / 10000;
                mon = (l / 100) % 100;
                day = l % 100;
                if ( mon > 0 && mon < 13 && day > 0 && day < 32 && yr > 1900 && yr < 2100 )
                {
                    sprintf( pData, "%4.4d-%2.2d-%2.2d", yr, mon, day );
                    return *this;
                }

                // 4) mmddyyyy

                l = atol( str );
                yr  = l % 10000;
                mon = l / 1000000;
                day = ( l / 10000 ) % 100;
                if ( mon > 0 && mon < 13 && day > 0 && day < 32 && yr > 1900 && yr < 2100 )
                {
                    sprintf( pData, "%4.4d-%2.2d-%2.2d", yr, mon, day );
                    return *this;
                }

                // 5) d-mon-yy(yy)

                day = atoi( str );
                pMon = strstr( str, "-" );
                if ( pMon )
                {
                    const char* months[] = { "XXX","JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC",NULL };

                    for ( mon = 0; months[mon]; mon++ )
                        if ( !strncmp( pMon+1, months[mon], 3 ) )
                            break;

                    if ( mon < 13 )
                    {
                        const char* pYr = strstr( pMon+4, "-" );
                        if ( pYr )
                        {
                            yr = atoi( ++pYr );
                            if ( yr > 0 && yr < 100 ) yr += 1900;

                            if ( mon > 0 && mon < 13 && day > 0 && day < 32 && yr > 1900 && yr < 2100 )
                            {
                                sprintf( pData, "%4.4d-%2.2d-%2.2d", yr, mon, day );
                                return *this;
                            }
                        }
                    }
                }
                break;
            }

            default :
            {
                break;
            }
        }
    }
    return *this;
}

void Column::Alter( const char * pszName, int iType, int iSize, int iPrecision )
{
    if ( psName ) delete psName;
    if ( pData  ) delete pData;

    iDataType = iType;
    iDataPrecision = iPrecision;
    psName = new String( pszName );

    switch ( iDataType )
    {
        case SQLECHR :           // CHAR
            if ( iSize )
            {
                iDataSize = iSize + 1;
            }
            else
            {
                iDataSize = 2;
            }
            break;
        case SQLESMA :           // SMALLINT
            iDataSize = SQLSNUM;
            break;
        case SQLERAW :           // LONG RAW
            iDataSize = 512;
            break;
        case SQLEINT :           // INTEGER
            iDataSize = SQLSNUM;
            break;
        case SQLEVAR :           // VARCHAR
            iDataSize = iSize + 1;
            break;
        case SQLEDEC :           // DECIMAL
            iDataSize = iSize + iPrecision + 3;
            break;
        case SQLEDAT :           // DATE
        case SQLETIM :           // TIME
        case SQLETMS :           // TIMESTAMP
            iDataSize = 254;
            break;
        case SQLELON :           // LONG DATA
            iDataSize = 4096;
            break;
        default :                // unknown type
            iDataSize = 254;
            break;
    }
    pData = new char[ iDataSize ];
    memset( pData, 0x00, iDataSize );
}


