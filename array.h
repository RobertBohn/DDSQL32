#ifndef __ARRAY_H
#define __ARRAY_H

#ifndef _Windows
#include <iostream.h>
#endif

template <class T> class Array
{
public:
    Array( int iOwner = 1 );
    ~Array( void );
    void clear( void );
    T& add( T& t );
    T& add( void );
    T& remove( long );
    T& operator [] (long index);
    long count;
    int owner;
private :
    class Node { public : T* item; Node* next; };
    Node* anchor;
    Node* currentNode;
    long  currentIndex;
};

template <class T> Array<T>::Array( int iOwner )
{
    count = 0L;
    owner = iOwner;
    anchor = NULL;
    currentNode = NULL;
}

template <class T> Array<T>::~Array()
{
    clear();
}

template <class T> void Array<T>::clear()
{
    Node* cur = anchor;
    while ( cur )
    {
        Node* nxt = cur->next;           // Save next Node
        if ( owner ) delete cur->item;   // Delete current item
        delete cur;                      // Delete current Node
        cur = nxt;                       // Point to next Node
    }
    count = 0L;
    anchor = NULL;
    currentNode = NULL;
}

template <class T> T& Array<T>::add( T& t )
{
    count++;
    for ( Node** cur = &anchor; *cur; cur = &((*cur)->next) );
    *cur = new Node;
    (*cur)->next = NULL;
    (*cur)->item = &t;
    return *((*cur)->item);
}

template <class T> T& Array<T>::add()
{
    return add( * new T );
}

template <class T> T& Array<T>::remove( long index )
{
    if ( index < 0L || index >= count )
    {
        #ifdef _Windows
            MessageBox( NULL, "Array out of bounds error.", "Array Error", MB_OK );
        #else
            cout << "\n\nArray out of bounds error.\n\n";
        #endif
        return *((T*)NULL);
    }
    
    Node** cur = &anchor;
    for ( long i=0; i<index; i++ ) cur = &((*cur)->next);

    Node* n = *cur;     // Save the Node to be removed
    T* t = n->item;     // Save the item to be removed
    *cur = n->next;     // Adjust the next pointer
    delete n;           // Delete the Node
    currentNode = NULL;
    count--;

    if ( owner ) { delete t; t = NULL; }
    return *t;
}

template <class T> T& Array<T>::operator [] (long index)
{
    if ( index < 0L || index >= count )
    {
        #ifdef _Windows
            MessageBox( NULL, "Array out of bounds error.", "Array Error", MB_OK );
        #else
            cout << "\n\nArray out of bounds error.\n\n";
        #endif
        return *((T*)NULL);
    }

    if ( currentNode == NULL || index < currentIndex )
    {
        currentNode = anchor;
        currentIndex=0;
    }
    for (; currentIndex<index; currentIndex++ ) currentNode = currentNode->next;
    return *(currentNode->item);
}

#endif // __ARRAY_H

