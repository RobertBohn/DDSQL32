#include <strng.h>
#include <connect.h>

/*----------------------------------------------------------------------------*/
/*  Constructors                                                              */
/*----------------------------------------------------------------------------*/
Connection::Connection( const char * pszConnectString )
{
    String User, Psw, Service;
    memset( &lda, '\0', sizeof(lda) );
    memset( &hda, '\0', sizeof(hda) );

    while ( *pszConnectString )
    {
        if ( *pszConnectString == '/' )
        {
            pszConnectString++;
            while ( *pszConnectString )
            {
                if ( *pszConnectString == '/' )
                {
                    Service = ++pszConnectString;
                    olog( &lda, hda, (unsigned char*)(const char*)User, -1, (unsigned char*)(const char*)Psw, -1, (unsigned char*)(const char*)Service, -1, OCI_LM_DEF );
                    return;
                }
                Psw += *(pszConnectString++);
            }                            
        }
        User += *(pszConnectString++);
    }
}

Connection::Connection( const char * pszUser, const char * pszPsw, const char * pszService )
{
    memset( &lda, '\0', sizeof(lda) );
    memset( &hda, '\0', sizeof(hda) );
    olog( &lda, hda, (unsigned char*)pszUser, -1, (unsigned char*)pszPsw, -1, (unsigned char*)pszService, -1, OCI_LM_DEF );
}

/*----------------------------------------------------------------------------*/
/*  Destructor                                                                */
/*----------------------------------------------------------------------------*/
Connection::~Connection()
{
    ologof( &lda );
}

/*----------------------------------------------------------------------------*/
/*  Return LogOn Data Area                                                    */
/*----------------------------------------------------------------------------*/
Connection::operator Lda_Def * ()
{
    return &lda;
}

/*----------------------------------------------------------------------------*/
/*  Return Status                                                             */
/*----------------------------------------------------------------------------*/
int Connection::status() const
{
    return lda.rc;
}

