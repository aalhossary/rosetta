// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief
/// @author Oliver Lange


#ifndef INCLUDED_protocols_environment_claims_CutBiasClaim_hh
#define INCLUDED_protocols_environment_claims_CutBiasClaim_hh


// Unit Headers
#include <protocols/environment/claims/CutBiasClaim.fwd.hh>

// Package Headers
#include <core/environment/FoldTreeSketch.hh>

#include <protocols/environment/claims/EnvClaim.hh>
#include <protocols/environment/claims/BrokerElements.hh>

// Project Headers
#include <core/fragment/SecondaryStructure.hh>

// ObjexxFCL Headers

// Utility headers
#include <basic/datacache/DataMap.fwd.hh>

//// C++ headers
#include <string>
#include <iosfwd>
#include <utility>
#include <map>

namespace protocols {
namespace environment {
namespace claims {

class CutBiasClaim : public EnvClaim {
	typedef EnvClaim Parent;
	typedef core::environment::FoldTreeSketch FoldTreeSketch;
	typedef core::environment::LocalPosition LocalPosition;

public:
	CutBiasClaim( ClientMoverOP owner,
		utility::tag::TagCOP tag,
		basic::datacache::DataMap const & );

	CutBiasClaim( ClientMoverOP owner,
		std::string const & label,
		core::fragment::SecondaryStructure const & ss );

	CutBiasClaim( ClientMoverOP owner,
		std::string const & label,
		std::map< LocalPosition, core::Real > const & biases );

	CutBiasClaim( ClientMoverOP owner,
		std::string const & label,
		std::pair< core::Size, core::Size > const & range,
		core::Real bias );

	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
	static std::string class_name();

	void yield_elements( FoldTreeSketch const& fts, CutBiasElements& elements ) const override;

	EnvClaimOP clone() const override;

	std::string type() const override;

	void show( std::ostream& os ) const override;

private:
	std::string label_;
	std::map< LocalPosition, core::Real > biases_;

}; //class CutBiasClaim


}
}
}

#endif
