// @HEADER
//
// ***********************************************************************
//
//             Xpetra: A linear algebra interface package
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Andrey Prokopenko (aprokop@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#ifndef XPETRA_EPETRAVECTOR_HPP
#define XPETRA_EPETRAVECTOR_HPP

/* this file is automatically generated - do not edit (see script/epetra.py) */

#include "Xpetra_EpetraConfigDefs.hpp"

#include "Xpetra_Vector.hpp"
#include "Xpetra_EpetraMultiVector.hpp"
#include "Xpetra_EpetraMap.hpp" //TMP
#include "Xpetra_Utils.hpp"
#include "Xpetra_EpetraImport.hpp"
#include "Xpetra_EpetraExport.hpp"
#include "Xpetra_Exceptions.hpp"

#include <Epetra_Vector.h>

namespace Xpetra {

  // TODO: move that elsewhere
  template<class GlobalOrdinal>
  Epetra_Vector & toEpetra(Vector<double, int, GlobalOrdinal> &);

  template<class GlobalOrdinal>
  const Epetra_Vector & toEpetra(const Vector<double, int, GlobalOrdinal> &);
  //

  template<class EpetraGlobalOrdinal>
  class EpetraVectorT
    : public virtual Vector<double,int,EpetraGlobalOrdinal>, public EpetraMultiVectorT<EpetraGlobalOrdinal>
  {

    typedef double Scalar;
    typedef int LocalOrdinal;
    typedef EpetraGlobalOrdinal GlobalOrdinal;
    typedef KokkosClassic::DefaultNode::DefaultNodeType Node;

  public:

    using EpetraMultiVectorT<GlobalOrdinal>::dot;          // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::norm1;        // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::norm2;        // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::normInf;      // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::normWeighted; // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::meanValue;    // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::replaceGlobalValue;    // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::sumIntoGlobalValue;    // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::replaceLocalValue;    // overloading, not hiding
    using EpetraMultiVectorT<GlobalOrdinal>::sumIntoLocalValue;    // overloading, not hiding

    //! @name Constructor/Destructor Methods
    //@{

    //! Sets all vector entries to zero.
    explicit EpetraVectorT(const RCP< const Map< LocalOrdinal, GlobalOrdinal, Node > > &map, bool zeroOut=true);

    // Commenting out since no definition provided in cpp.
    //! Vector copy constructor.
    // EpetraVectorT(const Vector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &source);

    //! Destructor.
    virtual ~EpetraVectorT() { }

    //@}

    //! @name Post-construction modification routines
    //@{

    //! Replace current value at the specified location with specified value.
    void replaceGlobalValue(GlobalOrdinal globalRow, const Scalar &value);

    //! Adds specified value to existing value at the specified location.
    void sumIntoGlobalValue(GlobalOrdinal globalRow, const Scalar &value);

    //! Replace current value at the specified location with specified values.
    void replaceLocalValue(LocalOrdinal myRow, const Scalar &value);

    //! Adds specified value to existing value at the specified location.
    void sumIntoLocalValue(LocalOrdinal myRow, const Scalar &value);

    //@}

    //! @name Mathematical methods
    //@{

    //! Computes dot product of this Vector against input Vector x.
    Scalar dot(const Vector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &a) const;

    //! Return 1-norm of this Vector.
    Teuchos::ScalarTraits< Scalar >::magnitudeType norm1() const;

    //! Compute 2-norm of this Vector.
    Teuchos::ScalarTraits< Scalar >::magnitudeType norm2() const;

    //! Compute Inf-norm of this Vector.
    Teuchos::ScalarTraits< Scalar >::magnitudeType normInf() const;

    //! Compute Weighted 2-norm (RMS Norm) of this Vector.
    Teuchos::ScalarTraits< Scalar >::magnitudeType normWeighted(const Vector< Scalar, LocalOrdinal, GlobalOrdinal, Node > &weights) const;

    //! Compute mean (average) value of this Vector.
    Scalar meanValue() const;

    //@}

    //! @name Overridden from Teuchos::Describable
    //@{

    //! Return a simple one-line description of this object.
    std::string description() const;

    //! Print the object with some verbosity level to an FancyOStream object.
    void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const;

    //@}

    //! @name Xpetra specific
    //@{

    //! EpetraMultiVectorT constructor to wrap a Epetra_Vector object
    EpetraVectorT(const Teuchos::RCP<Epetra_Vector> &vec) : EpetraMultiVectorT<GlobalOrdinal>(vec) { } // TODO: removed const of Epetra::Vector

    //! Get the underlying Epetra vector
    Epetra_Vector * getEpetra_Vector() const { return (*this->EpetraMultiVectorT<GlobalOrdinal>::getEpetra_MultiVector())(0); }
    //RCP<Epetra_Vector> getEpetra_Vector() const { return this->EpetraMultiVectorT<GlobalOrdinal>::getEpetra_MultiVector()->getVectorNonConst(0); }

    //! This constructor creates a Vector which is a view of column j of the MultiVector 'mv'.
    //! It implements the logic of MultiVector::getVector/getVectorNonConst() for Epetra MultiVector.
    //! The newly created Xpetra::EpetraVectorT will remain valid after the disappearance of the references to 'mv' in user code.
    EpetraVectorT(const RCP<Epetra_MultiVector> &mv, size_t j);

#ifdef HAVE_XPETRA_KOKKOS_REFACTOR
    typedef typename Xpetra::MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node>::unmanaged_host_view_type unmanaged_host_view_type;
    typedef typename Xpetra::MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node>::unmanaged_device_view_type unmanaged_device_view_type;

    unmanaged_host_view_type getHostLocalView () const {
      return this->EpetraMultiVectorT<GlobalOrdinal>::getHostLocalView();
    }

    unmanaged_device_view_type getDeviceLocalView() const {
      throw std::runtime_error("Epetra does not support device views!");
      unmanaged_device_view_type ret;
      return ret; // make compiler happy
    }
#endif

    //@}

  private:

    // This private member is only used by the constructor EpetraVectorT(const RCP<EpetraMultiVectorT<GlobalOrdinal> > &mv, size_t j). The actual private member holding the Epetra vector (vec_) is in the base class (Xpetra:EpetraMultiVectorT)
    const RCP<const Epetra_MultiVector> internalRefToBaseMV_;

  }; // EpetraVectorT class

#ifndef XPETRA_EPETRA_NO_32BIT_GLOBAL_INDICES
  typedef EpetraVectorT<int> EpetraVector;
#endif

#ifdef HAVE_XPETRA_INT_LONG_LONG
  typedef EpetraVectorT<long long> EpetraVector64;
#endif

} // Xpetra namespace

#endif // XPETRA_EPETRAVECTOR_HPP
