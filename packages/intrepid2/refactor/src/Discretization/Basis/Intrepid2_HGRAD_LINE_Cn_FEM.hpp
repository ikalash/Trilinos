// @HEADER
// ************************************************************************
//
//                           Intrepid2 Package
//                 Copyright (2007) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact Kyungjoo Kim  (kyukim@sandia.gov), or
//                    Mauro Perego  (mperego@sandia.gov)
//
// ************************************************************************
// @HEADER

/** \file   Intrepid_HGRAD_LINE_Cn_FEM_JACOBI.hpp
    \brief  Header file for the Intrepid2::HGRAD_LINE_Cn_FEM class.
    \author Created by R. Kirby and P. Bochev and D. Ridzal.
            Kokkorized by Kyungjoo Kim
*/

#ifndef __INTREPID2_HGRAD_LINE_CN_FEM_HPP__
#define __INTREPID2_HGRAD_LINE_CN_FEM_HPP__

#include "Kokkos_ViewFactory.hpp"

#include "Intrepid2_Basis.hpp"
#include "Intrepid2_HGRAD_LINE_Cn_FEM_JACOBI.hpp"

#include "Intrepid2_PointTools.hpp"
#include "Teuchos_LAPACK.hpp"

namespace Intrepid2 {
  
  /** \class  Intrepid2::Basis_HGRAD_LINE_Cn_FEM
      \brief  Implementation of the locally H(grad)-compatible FEM basis of variable order
      on the [-1,1] reference line cell, using Lagrange polynomials. 
  
      Implements Lagrange basis of variable order \f$n\f$ on
      the reference [-1,1] line cell.  The distribution of the points
      may be equispaced points with our without the endpoints, the Gauss-Legendre
      or Gauss-Lobatto points.  These points are {x_i}_{i=0}^n. with x_i < x_{i+1}

      The basis has cardinality \f$n+1\f$ and spans a COMPLETE linear polynomial space.
      Basis functions are dual to a unisolvent set of degrees of freedom (DoF)
      n_i( psi ) = psi(x_i).  The DoF are ordered by i.  The DoF at points 
      -1 and 1 (if included in {x_i} are attached to the vertices, and the rest of
      the DoF are attached to the edge itself.
  */

  namespace Impl {

    class Basis_HGRAD_LINE_Cn_FEM {
    public:

      template<EOperator opType>
      struct Serial {
        template<typename outputValueViewType,
                 typename inputPointViewType,
                 typename workViewType,
                 typename vinvViewType>
        KOKKOS_INLINE_FUNCTION
        static void
        getValues(       outputValueViewType outputValues,
                   const inputPointViewType  inputPoints,
                         workViewType        work,
                   const vinvViewType        vinv,
                   const ordinal_type        operatorDn = 0 );
      };

      template<typename ExecSpaceType, ordinal_type numPtsPerEval,
               typename outputValueValueType, class ...outputValueProperties,
               typename inputPointValueType,  class ...inputPointProperties,
               typename vinvValueType,        class ...vinvProperties>
      static void
      getValues(        Kokkos::DynRankView<outputValueValueType,outputValueProperties...> outputValues,
                  const Kokkos::DynRankView<inputPointValueType, inputPointProperties...>  inputPoints,
                  const Kokkos::DynRankView<vinvValueType,       vinvProperties...>        vinv,
                  const EOperator operatorType );
      
      template<typename outputValueViewType,
               typename inputPointViewType,
               typename vinvViewType,
               EOperator opType,
               ordinal_type numPtsEval>
      struct Functor {
              outputValueViewType _outputValues;
        const inputPointViewType  _inputPoints;
        const vinvViewType        _vinv;
        const ordinal_type        _opDn;
        
        KOKKOS_INLINE_FUNCTION
        Functor(       outputValueViewType outputValues_,
                       inputPointViewType  inputPoints_,
                       vinvViewType        vinv_,
                 const ordinal_type        opDn_ = 0 )
          : _outputValues(outputValues_), _inputPoints(inputPoints_), _vinv(vinv_), _opDn(opDn_) {}
        
        KOKKOS_INLINE_FUNCTION
        void operator()(const size_type iter) const {
          const auto ptBegin = Util<ordinal_type>::min(iter*numPtsEval,    _inputPoints.dimension(0));
          const auto ptEnd   = Util<ordinal_type>::min(ptBegin+numPtsEval, _inputPoints.dimension(0));
          
          const auto ptRange = Kokkos::pair<ordinal_type,ordinal_type>(ptBegin, ptEnd);
          const auto input   = Kokkos::subview( _inputPoints, ptRange, Kokkos::ALL() );

          typedef typename outputValueViewType::value_type outputValueType;
          typedef typename outputValueViewType::pointer_type outputPointerType;

          constexpr ordinal_type bufSize = (Parameters::MaxOrder+1)*numPtsEval;
          outputValueType buf[bufSize];
          Kokkos::DynRankView<outputValueType,
            Kokkos::Impl::ActiveExecutionMemorySpace,Kokkos::MemoryUnmanaged> 
            work((outputPointerType)&buf[0], bufSize);
          
          switch (opType) {
          case OPERATOR_VALUE : {
            auto output = Kokkos::subview( _outputValues, Kokkos::ALL(), ptRange );
            Serial<opType>::getValues( output, input, work, _vinv );
            break;
          }
          case OPERATOR_Dn : {
            auto output = Kokkos::subview( _outputValues, Kokkos::ALL(), ptRange, Kokkos::ALL() );
            Serial<opType>::getValues( output, input, work, _vinv, _opDn );
            break;
          }
          default: {
            INTREPID2_TEST_FOR_ABORT( true,
                                      ">>> ERROR: (Intrepid2::Basis_HGRAD_LINE_Cn_FEM::Functor) operator is not supported");
            
          }
          }
        }
      };
    };
  }
  
  template<typename ExecSpaceType = void,
           typename outputValueType = double,
           typename pointValueType = double>
  class Basis_HGRAD_LINE_Cn_FEM
    : public Basis<ExecSpaceType,outputValueType,pointValueType> {
  public:
    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::ordinal_type_array_1d_host ordinal_type_array_1d_host;
    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::ordinal_type_array_2d_host ordinal_type_array_2d_host;
    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::ordinal_type_array_3d_host ordinal_type_array_3d_host;

    /** \brief  Constructor.
     */
    Basis_HGRAD_LINE_Cn_FEM(const ordinal_type order,
                            const EPointType   pointType = POINTTYPE_EQUISPACED);  

    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::outputViewType outputViewType;
    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::pointViewType  pointViewType;
    typedef typename Basis<ExecSpaceType,outputValueType,pointValueType>::scalarViewType  scalarViewType;

    using Basis<ExecSpaceType,outputValueType,pointValueType>::getValues;

    virtual
    void
    getValues(       outputViewType outputValues,
               const pointViewType  inputPoints,
               const EOperator operatorType = OPERATOR_VALUE ) const {
#ifdef HAVE_INTREPID2_DEBUG
      Intrepid2::getValues_HGRAD_Args(outputValues,
                                      inputPoints,
                                      operatorType,
                                      this->getBaseCellTopology(),
                                      this->getCardinality() );
#endif
      constexpr ordinal_type numPtsPerEval = 1;
      Impl::Basis_HGRAD_LINE_Cn_FEM::
        getValues<ExecSpaceType,numPtsPerEval>( outputValues,
                                                inputPoints,
                                                this->vinv_,
                                                operatorType );
    }

    virtual
    void
    getDofCoords( scalarViewType dofCoords ) const {
#ifdef HAVE_INTREPID2_DEBUG
      // Verify rank of output array.
      INTREPID2_TEST_FOR_EXCEPTION( dofCoords.rank() != 2, std::invalid_argument,
                                    ">>> ERROR: (Intrepid2::Basis_HGRAD_LINE_Cn_FEM::getDofCoords) rank = 2 required for dofCoords array");
      // Verify 0th dimension of output array.
      INTREPID2_TEST_FOR_EXCEPTION( static_cast<ordinal_type>(dofCoords.dimension(0)) != this->getCardinality(), std::invalid_argument,
                                    ">>> ERROR: (Intrepid2::Basis_HGRAD_LINE_Cn_FEM::getDofCoords) mismatch in number of dof and 0th dimension of dofCoords array");
      // Verify 1st dimension of output array.
      INTREPID2_TEST_FOR_EXCEPTION( dofCoords.dimension(1) != this->getBaseCellTopology().getDimension(), std::invalid_argument,
                                    ">>> ERROR: (Intrepid2::Basis_HGRAD_LINE_Cn_FEM::getDofCoords) incorrect reference cell (1st) dimension in dofCoords array");
#endif
      Kokkos::deep_copy(dofCoords, this->dofCoords_);
    }

    void
    getVandermondeInverse( scalarViewType vinv ) const {
      // has to be same rank and dimensions
      Kokkos::deep_copy(vinv, this->vinv_);      
    }
    
    virtual
    const char*
    getName() const {
      return "Intrepid2_HGRAD_LINE_Cn_FEM";
    }

  private:

    /** \brief inverse of Generalized Vandermonde matrix, whose columns store the expansion
               coefficients of the nodal basis in terms of phis_ */
    Kokkos::DynRankView<typename scalarViewType::value_type,ExecSpaceType> vinv_;
  };

}// namespace Intrepid2

#include "Intrepid2_HGRAD_LINE_Cn_FEMDef.hpp"

#endif
