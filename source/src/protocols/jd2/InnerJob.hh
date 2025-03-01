// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/jd2/InnerJob.hh
/// @brief  header file for Job classes, part of August 2008 job distributor as
/// planned at RosettaCon08.  This file is responsible for three ideas: "inner"
/// jobs, "outer" jobs (with which the job distributor works) and job container
/// (currently just typdefed in the .fwd.hh)
/// @author Steven Lewis smlewi@gmail.com

#ifndef INCLUDED_protocols_jd2_InnerJob_hh
#define INCLUDED_protocols_jd2_InnerJob_hh

//unit headers
#include <protocols/jd2/InnerJob.fwd.hh>

//project headers
#include <core/pose/Pose.fwd.hh>

#include <protocols/jd2/JobInputter.fwd.hh> //for friendship
//#include <protocols/jd2/Parser.fwd.hh> //for friendship
#include <protocols/jd2/JobDistributor.fwd.hh> //for friendship

//utility headers
#include <utility/VirtualBase.hh>
#include <utility/pointer/deep_copy.hh>
#include <core/types.hh>

//C++ headers
#include <string>



namespace protocols {
namespace jd2 {

/// @details The InnerJob class is responsible for knowing input requirements
/// for a given job - how many nstruct, and what the input is.  InnerJobs are
/// relatively heavy; there is no need to duplicate a series of InnerJobs for
/// each index into nstruct.  The companion Job class handles the nstruct index
/// and has a pointer to an InnerJob (which is shared across many Jobs).
/// InnerJob also holds a PoseOP to maintain the unmodified input pose for that
/// job.
class InnerJob : public utility::VirtualBase {

public:
	/// @brief ctor.  Note that it takes only the input tag and max nstruct,
	/// pose instantiation is deferred until the pose is needed
	InnerJob( std::string const & input_tag, core::Size nstruct_max ); // move-constructing the string

	/// @brief ctor.  Note that it takes only the input tag and max nstruct,
	/// pose instantiation is deferred until the pose is needed
	InnerJob( core::pose::PoseCOP, std::string const & input_tag, core::Size nstruct_max ); // move-constructing the string

	~InnerJob() override;

	/// @brief Return an owning pointer to a copy of this object.
	///
	InnerJobOP clone() const;

	/// @brief Mutual comparison of this inner job to the other inner job
	/// so that if either one thinks it's not the same as the other, then
	/// it returns false.  Invokes the same() function on both this and other
	///
	/// @details Note: only compare if the pointers to the poses are to the
	/// same location
	bool
	operator == ( InnerJob const & other ) const;

	bool
	operator != (InnerJob const & other ) const;

	/// @brief returns true if this is the same as other;
	/// does not call other.same()
	virtual
	bool
	same( InnerJob const & other ) const;

	virtual
	void
	show( std::ostream & out ) const;

	friend
	std::ostream &
	operator<< ( std::ostream & out, const InnerJob & inner_job );

	/// @brief return the input tag (a string of space separated PDB filenames)
	std::string const & input_tag() const;

	/// @brief returns a job name controlled by the mover. Must be turned with the optional_output_name out flag. This flag is typically used in conjuction with a mover
	std::string const & optional_output_name() const {
		return optional_output_name_;
	}

	/// @brief sets a job name controlled by the mover
	void optional_output_name(std::string output_name){
		optional_output_name_ = output_name;
	}

	/// @brief
	core::Size nstruct_max() const;

	/// @brief return a COP to the input pose
	/// DO NOT USE OUTSIDE OF JD2 NAMESPACE
	core::pose::PoseCOP get_pose() const;

	void set_bad( bool value = true ) {
		bad_ = value;
	}

	bool bad() const {
		return bad_;
	}

protected:
	/// @brief set the input pose.
	/// this function is deliberately heavily protected.  Parser uses it to
	/// re-load the pose after adding constraints.  JobInputter is its primary
	/// client (using it to load poses).  JobDistributor uses it to set the PoseCOP
	/// to NULL when done with the input.
	void set_pose( core::pose::PoseCOP pose );

	//friend class protocols::jd2::Parser;
	friend class protocols::jd2::JobInputter;
	friend class protocols::jd2::JobDistributor;

private:
	std::string const input_tag_;
	std::string optional_output_name_;
	core::Size const nstruct_max_;
	utility::pointer::DeepCopyOP< core::pose::Pose const > pose_;
	bool bad_;
}; // InnerJob

} // namespace jd2
} // namespace protocols

#endif //INCLUDED_protocols_jd2_InnerJob_HH
