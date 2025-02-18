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
// Header file for Tri/Tet axisym linear elasticity elements
#ifndef OOMPH_TAXISYM_LINEAR_ELASTICITY_ELEMENTS_HEADER
#define OOMPH_TAXISYM_LINEAR_ELASTICITY_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif


// OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/oomph_utilities.h"
#include "../generic/Telements.h"
#include "axisym_linear_elasticity_elements.h"
#include "../generic/error_estimator.h"


namespace oomph
{
  /// //////////////////////////////////////////////////////////////////////
  /// //////////////////////////////////////////////////////////////////////
  // axisym TLinearElasticityElement
  /// /////////////////////////////////////////////////////////////////////
  /// /////////////////////////////////////////////////////////////////////


  //======================================================================
  /// TAxisymAxisymmetricLinearElasticityElement<NNODE_1D> elements are
  /// isoparametric triangular AxisymmetricLinearElasticity elements with
  /// NNODE_1D nodal points along each
  /// element edge. Inherits from TElement and
  /// AxisymmetricLinearElasticityEquations
  //======================================================================
  template<unsigned NNODE_1D>
  class TAxisymmetricLinearElasticityElement
    : public virtual TElement<2, NNODE_1D>,
      public virtual AxisymmetricLinearElasticityEquations,
      public virtual ElementWithZ2ErrorEstimator
  {
  public:
    /// Constructor: Call constructors for TElement and
    /// AxisymmetricLinearElasticity equations
    TAxisymmetricLinearElasticityElement()
      : TElement<2, NNODE_1D>(), AxisymmetricLinearElasticityEquations()
    {
    }


    /// Broken copy constructor
    TAxisymmetricLinearElasticityElement(
      const TAxisymmetricLinearElasticityElement<NNODE_1D>& dummy) = delete;

    /// Broken assignment operator
    void operator=(const TAxisymmetricLinearElasticityElement<NNODE_1D>&) =
      delete;

    /// Output function:
    void output(std::ostream& outfile)
    {
      AxisymmetricLinearElasticityEquations::output(outfile);
    }

    ///  Output function:
    void output(std::ostream& outfile, const unsigned& nplot)
    {
      AxisymmetricLinearElasticityEquations::output(outfile, nplot);
    }


    /// C-style output function:
    void output(FILE* file_pt)
    {
      AxisymmetricLinearElasticityEquations::output(file_pt);
    }

    ///  C-style output function:
    void output(FILE* file_pt, const unsigned& n_plot)
    {
      AxisymmetricLinearElasticityEquations::output(file_pt, n_plot);
    }

    /// Number of vertex nodes in the element
    unsigned nvertex_node() const
    {
      return TElement<2, NNODE_1D>::nvertex_node();
    }

    /// Pointer to the j-th vertex node in the element
    Node* vertex_node_pt(const unsigned& j) const
    {
      return TElement<2, NNODE_1D>::vertex_node_pt(j);
    }

    /// Order of recovery shape functions for Z2 error estimation:
    /// Same order as shape functions.
    unsigned nrecovery_order()
    {
      return NNODE_1D - 1;
    }

    /// Number of 'flux' terms for Z2 error estimation
    unsigned num_Z2_flux_terms()
    {
      return 6;
    }

    /// Get 'flux' for Z2 error recovery:   Upper triangular entries
    /// in strain tensor.
    void get_Z2_flux(const Vector<double>& s, Vector<double>& flux)
    {
#ifdef PARANOID
      unsigned num_entries = 6;
      if (flux.size() != num_entries)
      {
        std::ostringstream error_message;
        error_message << "The flux vector has the wrong number of entries, "
                      << flux.size() << ", whereas it should be " << num_entries
                      << std::endl;
        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Get strain matrix
      DenseMatrix<double> strain(3);
      this->get_strain(s, strain);

      // Pack into flux Vector
      unsigned icount = 0;

      // Start with diagonal terms
      for (unsigned i = 0; i < 3; i++)
      {
        flux[icount] = strain(i, i);
        icount++;
      }

      // Off diagonals row by row
      for (unsigned i = 0; i < 3; i++)
      {
        for (unsigned j = i + 1; j < 3; j++)
        {
          flux[icount] = strain(i, j);
          icount++;
        }
      }
    }
  };

  //=======================================================================
  /// Face geometry for the TAxisymmetricLinearElasticityElement elements: The
  /// spatial dimension of the face elements is one lower than that of the bulk
  /// element but they have the same number of points along their 1D edges.
  //=======================================================================
  template<unsigned NNODE_1D>
  class FaceGeometry<TAxisymmetricLinearElasticityElement<NNODE_1D>>
    : public virtual TElement<1, NNODE_1D>
  {
  public:
    /// Constructor: Call the constructor for the
    /// appropriate lower-dimensional QElement
    FaceGeometry() : TElement<1, NNODE_1D>() {}
  };


} // namespace oomph

#endif
