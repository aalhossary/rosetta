// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/SpanAngleEnergy.hh
/// @brief  penalize spans with unnatural angles with the membrane normal
/// @details natural TM spans cross the membrane in a rather narrow distribution of angles
/// this energy term favoures TMs spanning in this angles. as there are many more conformation available at
/// flatter angles, this term is required to keep sampling of span angles in the natural distribution.
/// @author Jonathan Weinstein


#ifndef INCLUDED_core_scoring_membrane_MPSpanAngleEnergy_hh
#define INCLUDED_core_scoring_membrane_MPSpanAngleEnergy_hh

// Package headers
#include <core/scoring/methods/WholeStructureEnergy.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <core/scoring/ScoreFunction.fwd.hh>

#include <utility/vector1.hh>
#include <core/energy_methods/MPSpanInsertionEnergy.hh>

//Objexx headers


// Utility headers


namespace core {
namespace energy_methods {


class MPSpanAngleEnergy : public core::scoring::methods::WholeStructureEnergy  {
public:
	typedef core::scoring::methods::WholeStructureEnergy  parent;

public:

	MPSpanAngleEnergy();

	//clone
	core::scoring::methods::EnergyMethodOP
	clone() const override;


	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	finalize_total_energy(
		pose::Pose & pose,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap & totals
	) const override;

	void
	setup_for_derivatives(
		pose::Pose &,
		core::scoring::ScoreFunction const &
	)
	const override;

	void
	setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & ) const override;

	void
	eval_atom_derivative(
		id::AtomID const &,
		pose::Pose const &,
		kinematics::DomainMap const &,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap const &,
		Vector &,// F1,
		Vector & // F2
	) const override;

	utility::vector1< core::Real >
	compute(
		pose::Pose const & pose,
		std::ostream & out,
		bool report
	) const;

	utility::vector1< numeric::xyzVector < core::Real > >
	find_helix_vector( core::pose::Pose const & pose, core::Size start, core::Size end ) const;

	core::Real
	calc_ang_score(
		core::Real ang
	) const;

	core::Size version() const override;

	void
	indicate_required_context_graphs(
		utility::vector1< bool > & /*context_graphs_required*/
	) const override {}

private:
	core::energy_methods::MPSpanInsertionEnergy mp_span_ins_;

};

} //scoring
} //core

#endif // INCLUDED_core_scoring_membrane_MPSpanAngleEnergy_HH
