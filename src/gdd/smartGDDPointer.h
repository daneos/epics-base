/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/

//
// Smart Pointer Class for GDD
// ( automatically takes care of referencing / unreferening a gdd each time that it is used )
//
// Author: Jeff Hill
//

#ifndef smartGDDPointer_h
#define smartGDDPointer_h

#include "gdd.h"
#include "shareLib.h"

//
// smartConstGDDPointer
//
// smart pointer class which auto ref/unrefs a const GDD
//
class smartConstGDDPointer {
public:

	smartConstGDDPointer ();
	smartConstGDDPointer (const gdd &valueIn);
	smartConstGDDPointer (const gdd *pValueIn);
	smartConstGDDPointer (const smartConstGDDPointer &ptrIn);
	~smartConstGDDPointer ();
	epicsShareFunc void set (const gdd *pNewValue);
	smartConstGDDPointer operator = (const gdd *rhs);
	smartConstGDDPointer operator = (const smartConstGDDPointer &rhs) ;
	const gdd *operator -> () const;
	const gdd & operator * () const;
	bool operator ! () const;
    bool valid () const;
    //
    // see Meyers, "more effective C++", for explanation on
    // why conversion to dumb pointer is ill advised here
    //
	//operator const gdd * () const;
protected:
    union {
	    gdd *pValue;
	    const gdd *pConstValue;
    };
};

//
// smartGDDPointer
//
// smart pointer class which auto ref/unrefs the GDD
//
class smartGDDPointer : public smartConstGDDPointer {
public:
    smartGDDPointer ();
	smartGDDPointer ( gdd &valueIn );
	smartGDDPointer ( gdd *pValueIn );
	smartGDDPointer ( const smartGDDPointer &ptrIn );
	void set ( gdd *pNewValue );
	smartGDDPointer operator = ( gdd *rhs );
	smartGDDPointer operator = ( const smartGDDPointer &rhs );
	gdd *operator -> () const;
	gdd & operator * () const;
    //
    // see Meyers, "more effective C++", for explanation on
    // why conversion to dumb pointer is ill advised here
    //
	//operator gdd * () const;
};

//
// smartConstGDDReference
//
// smart reference class which auto ref/unrefs a const GDD
//
// Notes: 
// 1) must be used with "->" operator and not "." operator
// 2) must be used with "*" operator as an L-value
//
class smartConstGDDReference {
public:
	smartConstGDDReference (const gdd *pValueIn);
	smartConstGDDReference (const gdd &valueIn);
	smartConstGDDReference (const smartConstGDDReference &refIn);
	~smartConstGDDReference ();
    const gdd *operator -> () const;
	const gdd & operator * () const;
protected:
    union {
	    gdd *pValue;
	    const gdd *pConstValue;
    };
};

//
// smartGDDReference
//
// smart reference class which auto ref/unrefs a const GDD
//
// Notes: 
// 1) must be used with "->" operator and not "." operator
// 2) must be used with "*" operator as an L-value
//
class smartGDDReference : public smartConstGDDReference {
public:
	smartGDDReference (gdd *pValueIn);
    smartGDDReference (gdd &valueIn);
	smartGDDReference (const smartGDDReference &refIn);
	gdd *operator -> () const;
	gdd & operator * () const;
};

inline smartConstGDDPointer::smartConstGDDPointer () :
	pConstValue (0) {}

inline smartConstGDDPointer::smartConstGDDPointer (const gdd &valueIn) :
	pConstValue (&valueIn)
{
	gddStatus status;
	status = this->pConstValue->reference ();
	assert (!status);
}

inline smartConstGDDPointer::smartConstGDDPointer (const gdd *pValueIn) :
	pConstValue (pValueIn)
{
	if ( this->pConstValue != NULL ) {
		gddStatus status;
		status = this->pConstValue->reference ();
		assert (!status);
	}
}

inline smartConstGDDPointer::smartConstGDDPointer (const smartConstGDDPointer &ptrIn) :
	pConstValue (ptrIn.pConstValue)
{
	if ( this->pConstValue != NULL ) {
		gddStatus status;
		status = this->pConstValue->reference();
		assert(!status);
	}
}

inline smartConstGDDPointer::~smartConstGDDPointer ()
{
	if ( this->pConstValue != NULL ) {
		gddStatus status;
		status = this->pConstValue->unreference();
		assert (!status);
	}
}

inline smartConstGDDPointer smartConstGDDPointer::operator = (const gdd *rhs) 
{
	this->set (rhs);
	return *this;
}

inline smartConstGDDPointer smartConstGDDPointer::operator = (const smartConstGDDPointer &rhs) 
{
	this->set (rhs.pConstValue);
	return *this;
}

inline const gdd *smartConstGDDPointer::operator -> () const
{
	return this->pConstValue;
}

inline const gdd & smartConstGDDPointer::operator * () const
{
	return *this->pConstValue;
}

inline bool smartConstGDDPointer::operator ! () const // X aCC 361
{
    if (this->pConstValue) {
        return false;
    }
    else {
        return true;
    }
}

inline bool smartConstGDDPointer::valid () const // X aCC 361
{
    if ( this->pConstValue ) {
        return true;
    }
    else {
        return false;
    }
}

inline smartGDDPointer::smartGDDPointer () : 
    smartConstGDDPointer () {}

inline smartGDDPointer::smartGDDPointer (gdd &valueIn) :
    smartConstGDDPointer (valueIn) {}

inline smartGDDPointer::smartGDDPointer (gdd *pValueIn) :
    smartConstGDDPointer (pValueIn) {}

inline smartGDDPointer::smartGDDPointer (const smartGDDPointer &ptrIn) :
    smartConstGDDPointer (ptrIn) {}

inline void smartGDDPointer::set (gdd *pNewValue) 
{
    this->smartConstGDDPointer::set (pNewValue);
}

inline smartGDDPointer smartGDDPointer::operator = (gdd *rhs) 
{
    this->smartGDDPointer::set (rhs);
	return *this;
}

inline smartGDDPointer smartGDDPointer::operator = (const smartGDDPointer &rhs) 
{
    this->smartGDDPointer::set (rhs.pValue);
	return *this;
}

inline gdd *smartGDDPointer::operator -> () const
{
	return this->pValue;
}

inline gdd & smartGDDPointer::operator * () const
{
	return *this->pValue;
}

inline smartConstGDDReference::smartConstGDDReference (const gdd *pValueIn) :
	pConstValue (pValueIn)
{
    assert (this->pConstValue);
	gddStatus status;
	status = this->pConstValue->reference ();
	assert (!status);
}

inline smartConstGDDReference::smartConstGDDReference (const gdd &valueIn) :
	pConstValue (&valueIn)
{
	gddStatus status;
	status = this->pConstValue->reference ();
	assert (!status);
}

inline smartConstGDDReference::smartConstGDDReference (const smartConstGDDReference &refIn) :
	pConstValue (refIn.pConstValue)
{
	gddStatus status;
	status = this->pConstValue->reference();
	assert (!status);
}

inline smartConstGDDReference::~smartConstGDDReference ()
{
	gddStatus status;
	status = this->pConstValue->unreference();
	assert (!status);
}

inline const gdd *smartConstGDDReference::operator -> () const
{
	return this->pConstValue;
}

inline const gdd & smartConstGDDReference::operator * () const
{
	return *this->pConstValue;
}

inline smartGDDReference::smartGDDReference (gdd *pValueIn) :
  smartConstGDDReference (pValueIn) {}

inline smartGDDReference::smartGDDReference (gdd &valueIn) :
    smartConstGDDReference (valueIn) {}

inline smartGDDReference::smartGDDReference (const smartGDDReference &refIn) :
    smartConstGDDReference (refIn) {}

inline gdd *smartGDDReference::operator -> () const
{
	return this->pValue;
}

inline gdd & smartGDDReference::operator * () const
{
	return *this->pValue;
}

#endif // smartGDDPointer_h

