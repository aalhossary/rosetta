// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/jd3/pose_outputters/ScoreFileOutputSpecification.cc
/// @brief  Method definitions of the %ScoreFileOutputSpecification class
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Unit headers
#include <protocols/jd3/pose_outputters/ScoreFileOutputSpecification.hh>

// Package headers
#include <protocols/jd3/pose_outputters/ScoreFileOutputter.hh>

// Project headers

#ifdef    SERIALIZATION
// Utility serialization headers
#include <utility/serialization/serialization.hh>

// Cereal headers
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>
#endif // SERIALIZATION

namespace protocols {
namespace jd3 {
namespace pose_outputters {

ScoreFileOutputSpecification::ScoreFileOutputSpecification()
{
	outputter_type( ScoreFileOutputter::keyname() );
}

ScoreFileOutputSpecification::ScoreFileOutputSpecification(
	JobResultID const & result_id,
	JobOutputIndex const & output_index
) :
	PoseOutputSpecification( result_id, output_index )
{
	outputter_type( ScoreFileOutputter::keyname() );
}

ScoreFileOutputSpecification::~ScoreFileOutputSpecification() = default;

std::string
ScoreFileOutputSpecification::out_fname() const
{
	return out_fname_ + suffix_from_jd_with_sep();
}

void
ScoreFileOutputSpecification::out_fname( std::string const & setting )
{
	out_fname_ = setting;
}

std::string const &
ScoreFileOutputSpecification::pose_tag() const
{
	return pose_tag_;
}

void
ScoreFileOutputSpecification::pose_tag( std::string const & setting )
{
	pose_tag_ = setting;
}


} // namespace pose_outputters
} // namespace jd3
} // namespace protocols


#ifdef    SERIALIZATION

/// @brief Automatically generated serialization method
template< class Archive >
void
protocols::jd3::pose_outputters::ScoreFileOutputSpecification::save( Archive & arc ) const {
	arc( cereal::base_class< protocols::jd3::pose_outputters::PoseOutputSpecification >( this ) );
	arc( CEREAL_NVP( out_fname_ ) ); // std::string
	arc( CEREAL_NVP( pose_tag_ ) ); // std::string
}

/// @brief Automatically generated deserialization method
template< class Archive >
void
protocols::jd3::pose_outputters::ScoreFileOutputSpecification::load( Archive & arc ) {
	arc( cereal::base_class< protocols::jd3::pose_outputters::PoseOutputSpecification >( this ) );
	arc( out_fname_ ); // std::string
	arc( pose_tag_ ); // std::string
}

SAVE_AND_LOAD_SERIALIZABLE( protocols::jd3::pose_outputters::ScoreFileOutputSpecification );
CEREAL_REGISTER_TYPE( protocols::jd3::pose_outputters::ScoreFileOutputSpecification )

CEREAL_REGISTER_DYNAMIC_INIT( protocols_jd3_pose_outputters_ScoreFileOutputSpecification )
#endif // SERIALIZATION
