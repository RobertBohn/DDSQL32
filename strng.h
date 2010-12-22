#ifndef _STRNG_H
#define _STRNG_H

#ifndef __STRING_H
#include <string.h>
#endif // __STRING_H

class String
{
public:
    String( const String& );
    String( const char * = "" );
    String( const char *, int );
    ~String() { delete theString; }

    String& operator =( const char * );
    String& operator =( const String& );
    operator const char *() const;
    String& operator +=( char );
    String& operator +=( const char * );
    void stripTrailingBlanks( void );
    void stripLeadingBlanks( void );
    void upper( void );
    void deQuote( void );
    void enQuote( char );
    void findReplace( const char *, const char * );
private:
    char *theString;
    int len;
};

inline int operator ==( const String& test1, const String& test2 )
{
    return (strcmp(test1,test2)==0);
}
inline int operator !=( const String& test1, const String& test2 )
{
    return (strcmp(test1,test2)!=0);
}
inline int operator ==( const String& test1, const char * test2 )
{
    return (strcmp(test1,test2)==0);
}
inline int operator !=( const String& test1, const char * test2 )
{
    return (strcmp(test1,test2)!=0);
}

#endif // _STRNG_H


