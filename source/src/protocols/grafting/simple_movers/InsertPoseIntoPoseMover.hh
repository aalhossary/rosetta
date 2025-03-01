// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file    protocols/grafting/InsertPoseIntoPoseMover.hh
/// @brief   Base class for graftmovers
/// @author  Jared Adolf-Bryfogle

#ifndef INCLUDED_protocols_grafting_InsertPoseIntoPoseMover_HH
#define INCLUDED_protocols_grafting_InsertPoseIntoPoseMover_HH

#include <protocols/moves/Mover.hh>
#include <protocols/grafting/simple_movers/InsertPoseIntoPoseMover.fwd.hh>
#include <core/pose/Pose.fwd.hh>

namespace protocols {
namespace grafting {
namespace simple_movers {

/// @brief Insert a whole pose into another.  Loops, linkers, whaterver. No modeling here.  Wrapper to utility function insert_pose_into_pose.
/// @details Residues between start + end should be deleted before using this mover if needed.
///
class InsertPoseIntoPoseMover : public  protocols::moves::Mover {

public:

	InsertPoseIntoPoseMover(bool copy_pdbinfo = false);
	InsertPoseIntoPoseMover(core::pose::Pose const & src_pose, std::string const & res_start, std::string const & res_end, bool copy_pdbinfo = false);

	InsertPoseIntoPoseMover( InsertPoseIntoPoseMover const & src);

	~InsertPoseIntoPoseMover() override;

	void
	apply(core::pose::Pose & pose) override;


public:
	void
	src_pose(core::pose::Pose const & src_pose);

	void
	start(std::string const & res_start);

	std::string const &
	start() const;

	void
	end(std::string const & res_end);

	std::string const &
	end() const;

public:

	protocols::moves::MoverOP
	clone() const override;

	void
	parse_my_tag(
		TagCOP tag,
		basic::datacache::DataMap & data
	) override;

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


public: //CitationManager

	/// @brief Provide the citation.
	void provide_citation_info(basic::citation_manager::CitationCollectionList & ) const override;

private:
	std::string start_;
	std::string end_;
	bool copy_pdbinfo_;
	core::pose::PoseCOP src_pose_;
};


}
}
}


#endif  // INCLUDED_protocols_grafting_InsertPoseIntoPoseMover_HH
