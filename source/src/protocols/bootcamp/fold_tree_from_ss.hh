// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/bootcamp/fold_tree_from_ss.hh
/// @brief A utility function to get fold tree from pose or secondary structure
/// @author Amr ALHOSSARY (aalhossary@wesleyan.edu)

#ifndef INCLUDED_protocols_fold_tree_from_ss_hh
#define INCLUDED_protocols_fold_tree_from_ss_hh

#include <string>
#include <core/pose/Pose.hh>
#include <core/kinematics/FoldTree.hh>

namespace protocols {
namespace bootcamp  {

    utility::vector1< std::pair< core::Size, core::Size > >
    identify_secondary_structure_spans( std::string const & ss_string );
    core::kinematics::FoldTreeOP fold_tree_from_ss_string(std::string secStruct);
    core::kinematics::FoldTreeOP fold_tree_from_ss(core::pose::Pose& pose);
}
}

#endif // INCLUDED_protocols_fold_tree_from_ss_hh
