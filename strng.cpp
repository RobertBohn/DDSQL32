#include <windows.h>
#include <strng.h>
#include <stdlib.h>
#include <string.h>


String::String( const char *aPtr )
{
    if ( aPtr == NULL ) aPtr = "";
    len = strlen( aPtr ) + 1;
    theString = new char[ len ];
    strcpy( theString, aPtr );
}

String::String( const String& sourceString )
{
    len = sourceString.len;
    theString = new char[ len ];
    strcpy( theString, sourceString.theString );
}

String& String::operator =( const char * sourceString )
{
    len = strlen( sourceString ) + 1;
    char* s = new char[ len ];
    strcpy( s, sourceString );
    delete theString;
    theString = s;
    return *this;
}

String& String::operator =( const String& sourceString )
{
    len = strlen( sourceString ) + 1;
    char* s = new char[ len ];
    strcpy( s, sourceString );
    delete theString;
    theString = s;
    return *this;
}

// Constructor allowing string size specificaton.
// Eliminates the requirement of using NULL terminated string initializers.
String::String( const char * ps, int iSize )
{
    len = iSize + 1;
    theString = new char[ len ];
    strncpy( theString, ps, iSize );
    theString[ iSize ] = 0x00;
}

// Allow appending a character to the String, expanding the size by one.
String& String::operator +=( char sourceChar )
{
    char* newString = new char[ len+1 ];
    strcpy( newString, theString );
    newString[len-1] = sourceChar;
    newString[len++] = '\0';
    delete theString;
    theString = newString;
    return *this;
}

// Concatinate a string
String& String::operator +=( const char * s )
{
    len += strlen(s);
    char* pszNewString = new char[ len ];
    strcpy( pszNewString, theString );
    strcat( pszNewString, s );
    delete theString;
    theString = pszNewString;
    return *this;
}

// Shorten string by trimming trailing blanks
void String::stripTrailingBlanks()
{
    int i = strlen( theString ) - 1;
    for (;;)
    {
        if ( i < 0 ) break;
        if ( theString[i] != ' ' ) break;
        theString[i] = '\0';
        i--;
    }
    len = strlen( theString ) + 1;
}

// Shorten string by trimming leading blanks
void String::stripLeadingBlanks()
{
    if ( len > 1 )
    {
        char* tmp = new char[ len+1 ];
        strcpy( tmp, theString );
        for ( char *s = tmp; *s == ' '; s++ );
        strcpy( theString, s );
        delete tmp;
    }
    len = strlen( theString ) + 1;
}

// Remove enclosing ' or " characters
void String::deQuote()
{
    if (( len > 2 ) &&
    (( *theString == 0x27 && theString[(len-2)] == 0x27 )  ||
    ( *theString == 0x22 && theString[(len-2)] == 0x22 )))
    {
        theString[(len-2)] = 0x00;
        len -= 2;
        char* pszNewString = new char[ len ];
        strcpy( pszNewString, theString+1 );
        delete theString;
        theString = pszNewString;
    }
}

// Set to upper case
void String::upper()
{
    _strupr( theString );
}

// Enclose the string with a given character
void String::enQuote( char cValue )
{
    len += 2;
    char* pszNewString = new char[ len ];
    strcpy( pszNewString+1, theString );
    *pszNewString = cValue;
    pszNewString[ len - 2 ] = cValue;
    pszNewString[ len - 1 ] = 0x00;
    delete theString;
    theString = pszNewString;
}

// Find and replace one sub-string with another sub-string
void String::findReplace( const char * pszFind, const char * pszReplace )
{
    int iFindLen = strlen( pszFind );
    int iReplaceLen = strlen( pszReplace );
    if ( iFindLen == 0 ) return;
    char* pszMark = theString;
    while ( strstr( pszMark, pszFind ) != NULL )
    {
        int iBase = (int)( strstr( pszMark, pszFind ) - theString );

        len += iReplaceLen - iFindLen;
        char* pszNewString = new char[ len ];

        memcpy( pszNewString, theString, iBase );
        strcpy( pszNewString+iBase, pszReplace );
        strcat( pszNewString, theString+(iBase + iFindLen) );
        delete theString;
        theString = pszNewString;
        pszMark = theString+(iBase + iReplaceLen);
    }
}

String::operator const char * () const
{
    return theString;
}                   

