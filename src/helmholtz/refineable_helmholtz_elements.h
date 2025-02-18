// LIC// ====================================================================
// LIC// This file forms part of oomph-lib, the object-oriented,
// LIC// multi-physics finite-element library, available
// LIC// at http://www.oomph-lib.org.
// LIC//
// LIC// Copyright (C) 2006-2021 Matthias Heil and Andrew Hazel
// LIC//
// LIC// This library is free software; you can redistribute it and/or
// LIC// modify it under the terms of the GNU Lesser General Public
// LIC// License as published by the Free Software Foundation; either
// LIC// version 2.1 of the License, or (at your option) any later version.
// LIC//
// LIC// This library is distributed in the hope that it will be useful,
// LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
// LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// LIC// Lesser General Public License for more details.
// LIC//
// LIC// You should have received a copy of the GNU Lesser General Public
// LIC// License along with this library; if not, write to the Free Software
// LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// LIC// 02110-1301  USA.
// LIC//
// LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
// LIC//
// LIC//====================================================================
// Header file for refineable QHelmholtzElement elements

#ifndef OOMPH_REFINEABLE_HELMHOLTZ_ELEMENTS_HEADER
#define OOMPH_REFINEABLE_HELMHOLTZ_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif


// oomph-lib headers
#include "../generic/refineable_quad_element.h"
#include "../generic/refineable_brick_element.h"
#include "../generic/error_estimator.h"
#include "helmholtz_elements.h"
#include "math.h"
#include <complex>

namespace oomph
{
  /// ////////////////////////////////////////////////////////////////////////
  /// ////////////////////////////////////////////////////////////////////////


  //======================================================================
  /// Refineable version of Helmholtz equations
  ///
  ///
  //======================================================================
  template<unsigned DIM>
  class RefineableHelmholtzEquations
    : public virtual HelmholtzEquations<DIM>,
      public virtual RefineableElement,
      public virtual ElementWithZ2ErrorEstimator
  {
  public:
    /// Constructor, simply call other constructors
    RefineableHelmholtzEquations()
      : HelmholtzEquations<DIM>(),
        RefineableElement(),
        ElementWithZ2ErrorEstimator()
    {
    }

    /// Broken copy constructor
    RefineableHelmholtzEquations(
      const RefineableHelmholtzEquations<DIM>& dummy) = delete;

    /// Broken assignment operator
    // Commented out broken assignment operator because this can lead to a
    // conflict warning when used in the virtual inheritence hierarchy.
    // Essentially the compiler doesn't realise that two separate
    // implementations of the broken function are the same and so, quite
    // rightly, it shouts.
    /*void operator=(const RefineableHelmholtzEquations<DIM>&) = delete;*/

    /// Number of 'flux' terms for Z2 error estimation
    unsigned num_Z2_flux_terms()
    {
      return 2 * DIM;
    }

    /// Get 'flux' for Z2 error recovery:  Standard flux.from Helmholtz
    /// equations
    void get_Z2_flux(const Vector<double>& s, Vector<double>& flux)
    {
      Vector<std::complex<double>> actual_flux(DIM);
      this->get_flux(s, actual_flux);
      unsigned count = 0;
      for (unsigned i = 0; i < DIM; i++)
      {
        flux[count] = actual_flux[i].real();
        flux[count + 1] = actual_flux[i].imag();
        count += 2;
      }
    }

    /// Get the function value u in Vector.
    /// Note: Given the generality of the interface (this function
    /// is usually called from black-box documentation or interpolation
    /// routines), the values Vector sets its own size in here.
    void get_interpolated_values(const Vector<double>& s,
                                 Vector<double>& values)
    {
      // Resize nitialise
      values.resize(2, 0.0);

      // Find number of nodes
      unsigned n_node = nnode();

      // Local shape function
      Shape psi(n_node);

      // Find values of shape function
      shape(s, psi);

      // Loop over the local nodes and sum up the values
      for (unsigned l = 0; l < n_node; l++)
      {
        values[0] +=
          this->nodal_value(l, this->u_index_helmholtz().real()) * psi[l];
        values[1] +=
          this->nodal_value(l, this->u_index_helmholtz().imag()) * psi[l];
      }
    }


    /// Get the function value u in Vector.
    /// Note: Given the generality of the interface (this function
    /// is usually called from black-box documentation or interpolation
    /// routines), the values Vector sets its own size in here.
    void get_interpolated_values(const unsigned& t,
                                 const Vector<double>& s,
                                 Vector<double>& values)
    {
      if (t != 0)
      {
        std::string error_message =
          "Time-dependent version of get_interpolated_values() ";
        error_message += "not implemented for this element \n";
        throw OomphLibError(
          error_message,
          "RefineableHelmholtzEquations::get_interpolated_values()",
          OOMPH_EXCEPTION_LOCATION);
      }
      else
      {
        // Make sure that we call this particular object's steady
        // get_interpolated_values (it could get overloaded lower down)
        RefineableHelmholtzEquations<DIM>::get_interpolated_values(s, values);
      }
    }


    ///  Further build: Copy source function pointer from father element
    void further_build()
    {
      this->Source_fct_pt = dynamic_cast<RefineableHelmholtzEquations<DIM>*>(
                              this->father_element_pt())
                              ->source_fct_pt();

      this->K_squared_pt = dynamic_cast<RefineableHelmholtzEquations<DIM>*>(
                             this->father_element_pt())
                             ->k_squared_pt();
    }


  private:
    /// Add element's contribution to elemental residual vector and/or
    /// Jacobian matrix
    /// flag=1: compute both
    /// flag=0: compute only residual vector
    void fill_in_generic_residual_contribution_helmholtz(
      Vector<double>& residuals,
      DenseMatrix<double>& jacobian,
      const unsigned& flag);
  };


  //======================================================================
  /// Refineable version of 2D QHelmholtzElement elements
  ///
  ///
  //======================================================================
  template<unsigned DIM, unsigned NNODE_1D>
  class RefineableQHelmholtzElement
    : public QHelmholtzElement<DIM, NNODE_1D>,
      public virtual RefineableHelmholtzEquations<DIM>,
      public virtual RefineableQElement<DIM>
  {
  public:
    /// Constructor, simply call the other constructors
    RefineableQHelmholtzElement()
      : RefineableElement(),
        RefineableHelmholtzEquations<DIM>(),
        RefineableQElement<DIM>(),
        QHelmholtzElement<DIM, NNODE_1D>()
    {
    }


    /// Broken copy constructor
    RefineableQHelmholtzElement(
      const RefineableQHelmholtzElement<DIM, NNODE_1D>& dummy) = delete;

    /// Broken assignment operator
    /*void operator=(const RefineableQHelmholtzElement<DIM,NNODE_1D>&) =
     * delete;*/

    /// Number of continuously interpolated values: 2
    unsigned ncont_interpolated_values() const
    {
      return 2;
    }

    /// Number of vertex nodes in the element
    unsigned nvertex_node() const
    {
      return QHelmholtzElement<DIM, NNODE_1D>::nvertex_node();
    }

    /// Pointer to the j-th vertex node in the element
    Node* vertex_node_pt(const unsigned& j) const
    {
      return QHelmholtzElement<DIM, NNODE_1D>::vertex_node_pt(j);
    }

    /// Rebuild from sons: empty
    void rebuild_from_sons(Mesh*& mesh_pt) {}

    /// Order of recovery shape functions for Z2 error estimation:
    /// Same order as shape functions.
    unsigned nrecovery_order()
    {
      return (NNODE_1D - 1);
    }

    ///  Perform additional hanging node procedures for variables
    /// that are not interpolated by all nodes. Empty.
    void further_setup_hanging_nodes() {}
  };

  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////


  //=======================================================================
  /// Face geometry for the RefineableQuadHelmholtzElement elements: The spatial
  /// dimension of the face elements is one lower than that of the
  /// bulk element but they have the same number of points
  /// along their 1D edges.
  //=======================================================================
  template<unsigned DIM, unsigned NNODE_1D>
  class FaceGeometry<RefineableQHelmholtzElement<DIM, NNODE_1D>>
    : public virtual QElement<DIM - 1, NNODE_1D>
  {
  public:
    /// Constructor: Call the constructor for the
    /// appropriate lower-dimensional QElement
    FaceGeometry() : QElement<DIM - 1, NNODE_1D>() {}
  };

} // namespace oomph

#endif
