/* -*- C++ -*- $Id$ */
// $Id$
//
// ******  Code generated by the The ACE ORB (TAO) IDL Compiler *******
// TAO and the TAO IDL Compiler have been developed by the Center for
// Distributed Object Computing at Washington University, St. Louis.
//
// Information about TAO is available at:
//                 http://www.cs.wustl.edu/~schmidt/TAO.html


#if !defined (TAO_USE_SEQUENCE_TEMPLATES)

#if !defined (__TAO_UNBOUNDED_SEQUENCE_TAO_UIOPENDPOINTSEQUENCE_CI_)
#define __TAO_UNBOUNDED_SEQUENCE_TAO_UIOPENDPOINTSEQUENCE_CI_

  // = Static operations.
  ACE_INLINE TAO_UIOP_Endpoint_Info *
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (CORBA::ULong size)
  // Allocate storage for the sequence.
  {
    TAO_UIOP_Endpoint_Info *retval = 0;
    ACE_NEW_RETURN (retval, TAO_UIOP_Endpoint_Info[size], 0);
    return retval;
  }

  ACE_INLINE void _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::freebuf (TAO_UIOP_Endpoint_Info *buffer)
  // Free the sequence.
  {
    delete [] buffer;
  }

  ACE_INLINE
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::_TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence (void) // Default constructor.
  {
  }

  ACE_INLINE
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::_TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence (CORBA::ULong maximum) // Constructor using a maximum length value.
    : TAO_Unbounded_Base_Sequence (maximum, _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (maximum))
  {
  }

  ACE_INLINE
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::_TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence (CORBA::ULong maximum,
    CORBA::ULong length,
    TAO_UIOP_Endpoint_Info *data,
    CORBA::Boolean release)
  : TAO_Unbounded_Base_Sequence (maximum, length, data, release)
  {
  }

  ACE_INLINE
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::_TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence (const _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence &rhs)
  // Copy constructor.
    : TAO_Unbounded_Base_Sequence (rhs)
  {
    if (rhs.buffer_ != 0)
    {
      TAO_UIOP_Endpoint_Info *tmp1 = _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (this->maximum_);
      TAO_UIOP_Endpoint_Info * const tmp2 = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info * ACE_CAST_CONST, rhs.buffer_);

      for (CORBA::ULong i = 0; i < this->length_; ++i)
        tmp1[i] = tmp2[i];

      this->buffer_ = tmp1;
    }
    else
    {
      this->buffer_ = 0;
    }
  }

  ACE_INLINE _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence &
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::operator= (const _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence &rhs)
  // Assignment operator.
  {
    if (this == &rhs)
      return *this;

    if (this->release_)
    {
      if (this->maximum_ < rhs.maximum_)
      {
        // free the old buffer
        TAO_UIOP_Endpoint_Info *tmp = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info *, this->buffer_);
        _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::freebuf (tmp);
        this->buffer_ = _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (rhs.maximum_);
      }
    }
    else
      this->buffer_ = _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (rhs.maximum_);

    TAO_Unbounded_Base_Sequence::operator= (rhs);

    TAO_UIOP_Endpoint_Info *tmp1 = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info *, this->buffer_);
    TAO_UIOP_Endpoint_Info * const tmp2 = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info * ACE_CAST_CONST, rhs.buffer_);

    for (CORBA::ULong i = 0; i < this->length_; ++i)
      tmp1[i] = tmp2[i];

    return *this;
  }

  // = Accessors.
  ACE_INLINE TAO_UIOP_Endpoint_Info &
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::operator[] (CORBA::ULong i)
  // operator []
  {
    ACE_ASSERT (i < this->maximum_);
    TAO_UIOP_Endpoint_Info* tmp = ACE_reinterpret_cast(TAO_UIOP_Endpoint_Info*,this->buffer_);
    return tmp[i];
  }

  ACE_INLINE const TAO_UIOP_Endpoint_Info &
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::operator[] (CORBA::ULong i) const
  // operator []
  {
    ACE_ASSERT (i < this->maximum_);
    TAO_UIOP_Endpoint_Info * const tmp = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info* ACE_CAST_CONST, this->buffer_);
    return tmp[i];
  }

  // Implement the TAO_Base_Sequence methods (see Sequence.h)

  ACE_INLINE TAO_UIOP_Endpoint_Info *
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::get_buffer (CORBA::Boolean orphan)
  {
    TAO_UIOP_Endpoint_Info *result = 0;
    if (orphan == 0)
    {
      // We retain ownership.
      if (this->buffer_ == 0)
      {
        result = _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::allocbuf (this->length_);
        this->buffer_ = result;
        this->release_ = 1;
      }
      else
      {
        result = ACE_reinterpret_cast (TAO_UIOP_Endpoint_Info*, this->buffer_);
      }
    }
    else // if (orphan == 1)
    {
      if (this->release_ != 0)
      {
        // We set the state back to default and relinquish
        // ownership.
        result = ACE_reinterpret_cast(TAO_UIOP_Endpoint_Info*,this->buffer_);
        this->maximum_ = 0;
        this->length_ = 0;
        this->buffer_ = 0;
        this->release_ = 0;
      }
    }
    return result;
  }

  ACE_INLINE const TAO_UIOP_Endpoint_Info *
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::get_buffer (void) const
  {
    return ACE_reinterpret_cast(const TAO_UIOP_Endpoint_Info * ACE_CAST_CONST, this->buffer_);
  }

  ACE_INLINE void
  _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::replace (CORBA::ULong max,
  CORBA::ULong length,
  TAO_UIOP_Endpoint_Info *data,
  CORBA::Boolean release)
  {
    this->maximum_ = max;
    this->length_ = length;
    if (this->buffer_ && this->release_ == 1)
    {
      TAO_UIOP_Endpoint_Info *tmp = ACE_reinterpret_cast(TAO_UIOP_Endpoint_Info*,this->buffer_);
      _TAO_Unbounded_Sequence_TAO_UIOPEndpointSequence::freebuf (tmp);
    }
    this->buffer_ = data;
    this->release_ = release;
  }

#endif /* end #if !defined */


#endif /* !TAO_USE_SEQUENCE_TEMPLATES */

#if !defined (_TAO_UIOPENDPOINTSEQUENCE_CI_)
#define _TAO_UIOPENDPOINTSEQUENCE_CI_

// *************************************************************
// Inline operations for class TAO_UIOPEndpointSequence_var
// *************************************************************

ACE_INLINE
TAO_UIOPEndpointSequence_var::TAO_UIOPEndpointSequence_var (void) // default constructor
  : ptr_ (0)
{}

ACE_INLINE
TAO_UIOPEndpointSequence_var::TAO_UIOPEndpointSequence_var (TAO_UIOPEndpointSequence *p)
  : ptr_ (p)
{}

ACE_INLINE
TAO_UIOPEndpointSequence_var::TAO_UIOPEndpointSequence_var (const ::TAO_UIOPEndpointSequence_var &p) // copy constructor
{
  if (p.ptr_)
    ACE_NEW (this->ptr_, ::TAO_UIOPEndpointSequence (*p.ptr_));
  else
    this->ptr_ = 0;
}

ACE_INLINE
TAO_UIOPEndpointSequence_var::~TAO_UIOPEndpointSequence_var (void) // destructor
{
  delete this->ptr_;
}

ACE_INLINE TAO_UIOPEndpointSequence_var &
TAO_UIOPEndpointSequence_var::operator= (TAO_UIOPEndpointSequence *p)
{
  delete this->ptr_;
  this->ptr_ = p;
  return *this;
}

ACE_INLINE TAO_UIOPEndpointSequence_var &
TAO_UIOPEndpointSequence_var::operator= (const ::TAO_UIOPEndpointSequence_var &p) // deep copy
{
  if (this != &p)
  {
    delete this->ptr_;
    ACE_NEW_RETURN (this->ptr_, ::TAO_UIOPEndpointSequence (*p.ptr_), *this);
  }
  return *this;
}

ACE_INLINE const ::TAO_UIOPEndpointSequence *
TAO_UIOPEndpointSequence_var::operator-> (void) const
{
  return this->ptr_;
}

ACE_INLINE ::TAO_UIOPEndpointSequence *
TAO_UIOPEndpointSequence_var::operator-> (void)
{
  return this->ptr_;
}

ACE_INLINE
TAO_UIOPEndpointSequence_var::operator const ::TAO_UIOPEndpointSequence &() const // cast
{
  return *this->ptr_;
}

ACE_INLINE
TAO_UIOPEndpointSequence_var::operator ::TAO_UIOPEndpointSequence &() // cast
{
  return *this->ptr_;
}

ACE_INLINE
TAO_UIOPEndpointSequence_var::operator ::TAO_UIOPEndpointSequence &() const // cast
{
  return *this->ptr_;
}

// variable-size types only
ACE_INLINE
TAO_UIOPEndpointSequence_var::operator ::TAO_UIOPEndpointSequence *&() // cast
{
  return this->ptr_;
}

ACE_INLINE TAO_UIOP_Endpoint_Info &
TAO_UIOPEndpointSequence_var::operator[] (CORBA::ULong index)
{
  return this->ptr_->operator[] (index);
}

ACE_INLINE const ::TAO_UIOPEndpointSequence &
TAO_UIOPEndpointSequence_var::in (void) const
{
  return *this->ptr_;
}

ACE_INLINE ::TAO_UIOPEndpointSequence &
TAO_UIOPEndpointSequence_var::inout (void)
{
  return *this->ptr_;
}

// mapping for variable size
ACE_INLINE ::TAO_UIOPEndpointSequence *&
TAO_UIOPEndpointSequence_var::out (void)
{
  delete this->ptr_;
  this->ptr_ = 0;
  return this->ptr_;
}

ACE_INLINE ::TAO_UIOPEndpointSequence *
TAO_UIOPEndpointSequence_var::_retn (void)
{
  ::TAO_UIOPEndpointSequence *tmp = this->ptr_;
  this->ptr_ = 0;
  return tmp;
}

ACE_INLINE ::TAO_UIOPEndpointSequence *
TAO_UIOPEndpointSequence_var::ptr (void) const
{
  return this->ptr_;
}

#endif /* end #if !defined */

ACE_INLINE CORBA::Boolean operator<< (TAO_OutputCDR &strm, const TAO_UIOP_Endpoint_Info &_tao_aggregate)
{
  if (
    (strm << _tao_aggregate.rendezvous_point.in ()) &&
    (strm << _tao_aggregate.priority)
  )
    return 1;
  else
    return 0;

}

ACE_INLINE CORBA::Boolean operator>> (TAO_InputCDR &strm, TAO_UIOP_Endpoint_Info &_tao_aggregate)
{
  if (
    (strm >> _tao_aggregate.rendezvous_point.out ()) &&
    (strm >> _tao_aggregate.priority)
  )
    return 1;
  else
    return 0;

}


#if !defined _TAO_CDR_OP_TAO_UIOPEndpointSequence_I_
#define _TAO_CDR_OP_TAO_UIOPEndpointSequence_I_

CORBA::Boolean TAO_Export operator<< (
    TAO_OutputCDR &,
    const TAO_UIOPEndpointSequence &
  );
CORBA::Boolean TAO_Export operator>> (
    TAO_InputCDR &,
    TAO_UIOPEndpointSequence &
  );

#endif /* _TAO_CDR_OP_TAO_UIOPEndpointSequence_I_ */
