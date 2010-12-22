#include <stdlib.h>
#include <stdio.h>
#include <strng.h>
#include <table.h>
#include <errsql.h>
#include <text2col.h>

/*----------------------------------------------------------------------------*/
/*  Populate Columns With Text Data                                           */
/*----------------------------------------------------------------------------*/
int text2Column( const char * pszData, Column * ppCols [] )
{
    int iOffset = 0;
    String* pString;
    for ( int iCol = 0; ppCols[iCol]; iCol++ )
    {
        if ( pszData[iOffset] == 0x00 )
        {
            // Too Few Fields
            return PRSENEV;
        }
        if ( pszData[iOffset] == '"' )
        {
            iOffset++;
            for ( pString = new String; ( ( pszData[iOffset] != '"' ) && ( pszData[iOffset] != 0x00 ) ); iOffset++ )
            {
                *pString += pszData[iOffset];
            }
            if ( pszData[iOffset] == '"' ) iOffset++;
        }
        else
        {
            for ( pString = new String; ( ( pszData[iOffset] != ',' ) && ( pszData[iOffset] != 0x00 ) ); iOffset++ )
            {
                *pString += pszData[iOffset];
            }
        }
        pString->stripTrailingBlanks();
        *(ppCols[iCol]) = *pString;
        delete pString;
        if ( pszData[iOffset] == ',' ) iOffset++;
    }
    if ( pszData[iOffset] )
    {
        // Too Many Fields
        return PRSETMV;
    }
    return 0;
}

/*----------------------------------------------------------------------------*/
/* Create a Comma Delimited String From an Array of Columns                   */
/*----------------------------------------------------------------------------*/
String& column2String( Column * ppCols [] )
{
    String * psString = new String( "" );
    for ( int iCol = 0; ppCols[iCol]; iCol++ )
    {
        if ( iCol )
        {
            *(psString) += ",";
        }
        if ( ppCols[iCol]->isNull() )
        {
            *(psString) += "\"\"";
        }
        else
        {
            String * psCol = new String( (const char *)(*(ppCols[iCol])) );
            if ( ppCols[iCol]->type() == SQLEVAR || ppCols[iCol]->type() == SQLEDAT )
            {
                psCol->enQuote( '"' );
            }
            *psString += *psCol;
            delete psCol;
        }
    }
    return *psString;
}
