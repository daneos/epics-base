
/*
 *  $Id$
 *      Author: Jeffrey O. Hill
 *              hill@luke.lanl.gov
 *              (505) 665 1831
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 */

#include <stdexcept>

#define epicsAssertAuthor "Jeff Hill johill@lanl.gov"

#define epicsExportSharedSymbols
#include "iocinf.h"
#include "syncGroup.h"
#include "oldAccess.h"

syncGroupReadNotify::syncGroupReadNotify ( CASG &sgIn, chid pChan, void *pValueIn ) :
    syncGroupNotify ( sgIn, pChan ), pValue ( pValueIn )
{
}

void syncGroupReadNotify::begin ( unsigned type, arrayElementCount count )
{
    this->chan->read ( type, count, *this, &this->id );
    this->idIsValid = true;
}

syncGroupReadNotify * syncGroupReadNotify::factory ( 
    tsFreeList < class syncGroupReadNotify, 128, epicsMutexNOOP > &freeList, 
    struct CASG &sg, chid chan, void *pValueIn )
{
    return new ( freeList ) syncGroupReadNotify ( sg, chan, pValueIn);
}

void syncGroupReadNotify::destroy ( casgRecycle &recycle )
{
    this->~syncGroupReadNotify ();
    recycle.recycleSyncGroupReadNotify ( * this );
}

syncGroupReadNotify::~syncGroupReadNotify ()
{
    if ( this->idIsValid ) {
        this->chan->ioCancel ( this-> id );
    }
}

void syncGroupReadNotify::completion (
    unsigned type, arrayElementCount count, const void *pData )
{
    if ( this->magic != CASG_MAGIC ) {
        this->sg.printf ( "cac: sync group io_complete(): bad sync grp op magic number?\n" );
        return;
    }

    if ( this->pValue ) {
        size_t size = dbr_size_n ( type, count );
        memcpy ( this->pValue, pData, size );
    }
    this->idIsValid = false;
    this->sg.completionNotify ( *this );
}

void syncGroupReadNotify::exception (
    int status, const char *pContext, unsigned type, arrayElementCount count )
{
   this->sg.exception ( status, pContext, 
        __FILE__, __LINE__, *this->chan, type, count, CA_OP_GET );
    //
    // This notify is left installed at this point as a place holder indicating that
    // all requests have not been completed. This notify is not uninstalled until
    // CASG::block() times out or unit they call CASG::reset().
    //
}

void syncGroupReadNotify::show ( unsigned level ) const
{
    ::printf ( "pending sg read op: pVal=%p\n", this->pValue );
    if ( level > 0u ) {
        this->syncGroupNotify::show ( level - 1u );
    }
}

void * syncGroupReadNotify::operator new ( size_t sizeIn )
{
    return ::operator new ( sizeIn );
}

void syncGroupReadNotify::operator delete ( void * p )
{
    ::operator delete ( p );
}

void * syncGroupReadNotify::operator new ( size_t size, 
    tsFreeList < class syncGroupReadNotify, 128, epicsMutexNOOP > & freeList )
{
    return freeList.allocate ( size );
}

#if defined ( CASG_PLACEMENT_DELETE )
void syncGroupReadNotify::operator delete ( void *pCadaver, 
    tsFreeList < class syncGroupReadNotify, 128, epicsMutexNOOP > &freeList )
{
    freeList.release ( pCadaver, sizeof ( syncGroupReadNotify ) );
}
#endif
