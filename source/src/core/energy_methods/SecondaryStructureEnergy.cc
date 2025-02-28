// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/energy_methods/SecondaryStructureEnergy.cc
/// @brief  Statistically derived rotamer pair potential class implementation
/// @author Phil Bradley
/// @author Andrew Leaver-Fay


// Unit headers
#include <core/energy_methods/SecondaryStructureEnergy.hh>
#include <core/energy_methods/SecondaryStructureEnergyCreator.hh>

// Package headers
#include <core/scoring/SecondaryStructurePotential.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoringManager.hh>
#include <core/scoring/methods/EnergyMethodOptions.hh>

// Project headers
#include <core/pose/Pose.hh>

#include <utility/vector1.hh>


namespace core {
namespace energy_methods {



/// @details This must return a fresh instance of the SecondaryStructureEnergy class,
/// never an instance already in use
core::scoring::methods::EnergyMethodOP
SecondaryStructureEnergyCreator::create_energy_method(
	core::scoring::methods::EnergyMethodOptions const &
) const {
	return utility::pointer::make_shared< SecondaryStructureEnergy >();
}

core::scoring::ScoreTypes
SecondaryStructureEnergyCreator::score_types_for_method() const {
	using namespace core::scoring;
	ScoreTypes sts;
	sts.push_back( hs_pair );
	sts.push_back( ss_pair );
	sts.push_back( rsigma );
	sts.push_back( sheet );
	return sts;
}


/// c-tor
SecondaryStructureEnergy::SecondaryStructureEnergy() :
	parent( utility::pointer::make_shared< SecondaryStructureEnergyCreator >() ),
	potential_( core::scoring::ScoringManager::get_instance()->get_SecondaryStructurePotential() )
{}


/// c-tor
SecondaryStructureEnergy::SecondaryStructureEnergy( SecondaryStructureEnergy const & /*src*/ ) = default;


/// clone
core::scoring::methods::EnergyMethodOP
SecondaryStructureEnergy::clone() const
{
	return utility::pointer::make_shared< SecondaryStructureEnergy >( *this );
}


/////////////////////////////////////////////////////////////////////////////
// scoring
/////////////////////////////////////////////////////////////////////////////


void
SecondaryStructureEnergy::setup_for_scoring( pose::Pose & pose, core::scoring::ScoreFunction const & ) const
{
	//std::cout << "SecondaryStructureEnergy::setup_for_scoring" << std::endl;
	pose.update_residue_neighbors();
	potential_.setup_for_scoring( pose );
}


/// all scoring happens here
void
SecondaryStructureEnergy::finalize_total_energy(
	pose::Pose & pose,
	core::scoring::ScoreFunction const & scorefxn,
	core::scoring::EnergyMap & totals
) const
{
	//std::cout << "SecondaryStructureEnergy::finalize_total_energy" << std::endl;
	Real hs_score, ss_score, rsigma_score, sheet_score;

	potential_.score(
		pose, scorefxn.energy_method_options().secondary_structure_weights(),
		hs_score, ss_score, rsigma_score, sheet_score );

	totals[ core::scoring::hs_pair ] = hs_score;
	totals[ core::scoring::ss_pair ] = ss_score;
	totals[ core::scoring::rsigma ]  = rsigma_score;
	totals[ core::scoring::sheet ]   = sheet_score;
	//std::cout << "hs_score: " << hs_score << " ss_score: " <<  ss_score << " rsigma_score: " << rsigma_score << " sheet_score: " << sheet_score << std::endl;
}


/// @brief SecondaryStructureEnergy distance cutoff
Distance
SecondaryStructureEnergy::atomic_interaction_cutoff() const
{
	return 6.0; /// now subtracted off 6.0 from cutoffs in centroid params files
	//  return 0.0; /// since all the cutoffs for centroid mode are rolled into the cendist check
}

/// @brief SecondaryStructureEnergy
void
SecondaryStructureEnergy::indicate_required_context_graphs( utility::vector1< bool > & /*context_graphs_required*/ ) const
{
}
core::Size
SecondaryStructureEnergy::version() const
{
	return 1; // Initial versioning
}


}
}
