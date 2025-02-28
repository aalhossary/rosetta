// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/geometric_solvation/OccludedHbondSolEnergy_onebody.hh
/// @brief  Solvation model based on penalizing potential for Hbonding to solvent
/// @author John Karanicolas


#ifndef INCLUDED_core_scoring_geometric_solvation_OccludedHbondSolEnergy_onebody_hh
#define INCLUDED_core_scoring_geometric_solvation_OccludedHbondSolEnergy_onebody_hh

#include <core/types.hh>

// Unit Headers
#include <core/energy_methods/OccludedHbondSolEnergy_onebody.fwd.hh>

// Package headers

#include <core/scoring/methods/ContextDependentOneBodyEnergy.hh>
#include <core/scoring/methods/EnergyMethodOptions.fwd.hh>
#include <core/scoring/geometric_solvation/DatabaseOccSolEne.fwd.hh>

// Project headers
#include <core/pose/Pose.fwd.hh>

#include <utility/vector1.hh>


//#include <core/scoring/EnergyMap.hh>

namespace core {
namespace energy_methods {


class OccludedHbondSolEnergy_onebody : public core::scoring::methods::ContextDependentOneBodyEnergy  {
public:
	typedef core::scoring::methods::ContextDependentOneBodyEnergy  parent;

public:

	OccludedHbondSolEnergy_onebody( core::scoring::methods::EnergyMethodOptions const & options, bool const verbose = false );

	OccludedHbondSolEnergy_onebody( OccludedHbondSolEnergy_onebody const & src );

	core::scoring::methods::EnergyMethodOP
	clone() const override;

	void setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & ) const override;

	void setup_for_packing(pose::Pose & pose, utility::vector1< bool > const &, utility::vector1< bool > const & ) const override;

	void setup_for_derivatives( pose::Pose &pose, core::scoring::ScoreFunction const &  ) const override;

	void setup_for_minimizing(pose::Pose & pose, core::scoring::ScoreFunction const & , kinematics::MinimizerMapBase const &) const override;

	void residue_energy(
		conformation::Residue const & polar_rsd,
		pose::Pose const & pose,
		core::scoring::EnergyMap & emap
	) const override;

	void indicate_required_context_graphs( utility::vector1< bool > &  ) const override {};

	// note intrares_energy *is* included, but just as part of residue_energy and not as a separate function
	virtual bool defines_intrares_energy( core::scoring::EnergyMap const & ) const { return false; };

	virtual Distance atomic_interaction_cutoff() const;

private:

	Real
	res_res_occ_sol_one_way(
		conformation::Residue const & polar_rsd,
		conformation::Residue const & occ_rsd ) const;

	void
	get_atom_atom_occ_solvation(
		Size const don_h_atom,
		Size const don_base_atom,
		conformation::Residue const & don_rsd,
		Size const occ_atom,
		conformation::Residue const & occ_rsd,
		Real & energy
	) const;

	Real
	get_cos_angle(
		Vector const & base_atom_xyz,
		Vector const & polar_atom_xyz,
		Vector const & occluding_atom_xyz ) const;

	bool
	atom_is_donor_h( conformation::Residue const & rsd, Size const atom ) const;

	bool
	atom_is_acceptor( conformation::Residue const & rsd, Size const atom ) const;

	bool
	atom_is_valid_base( conformation::Residue const & rsd, Size const atom ) const;


private:

	// const-ref to scoring database
	scoring::geometric_solvation::DatabaseOccSolEne const & occ_hbond_sol_database_;

	bool const verbose_;
	core::Size version() const override;

};

} // scoring
} // core

#endif

