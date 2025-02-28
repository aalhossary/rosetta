// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/stepwise/sampler/rna/RNA_KinematicCloser.hh
/// @brief Close a RNA loop with Kinematic Closer (KIC).
/// @author Rhiju Das
/// @author Fang-Chieh Chou

#ifndef INCLUDED_protocols_sampler_rna_RNA_KinematicCloser_HH
#define INCLUDED_protocols_sampler_rna_RNA_KinematicCloser_HH

// Unit headers
#include <protocols/stepwise/sampler/rna/RNA_KinematicCloser.fwd.hh>

// Package headers
#include <protocols/stepwise/sampler/StepWiseSamplerSized.hh>

// Project headers
#ifdef WIN32
#include <core/id/DOF_ID.hh>
#include <core/id/NamedAtomID.hh>
#else
#include <core/id/NamedAtomID.fwd.hh>
#endif

#include <core/id/DOF_ID.fwd.hh>

// Utility headers
#include <utility/fixedsizearray1.hh>

#include <utility/vector1.hh> // AUTO IWYU For vector1

namespace protocols {
namespace stepwise {
namespace sampler {
namespace rna {

/// @brief The RNA de novo structure modeling protocol
class RNA_KinematicCloser: public StepWiseSamplerSized {
public:
	RNA_KinematicCloser(
		core::pose::Pose const & init_pose,
		core::Size const moving_suite,
		core::Size const chainbreak_suite,
		bool const is_TNA_ = false
	);

	~RNA_KinematicCloser() override;

	/// @brief Class name
	std::string get_name() const override { return "RNA_KinematicCloser"; }

	/// @brief Type of class (see enum in toolbox::SamplerPlusPlusTypes.hh)
	toolbox::SamplerPlusPlusType type() const override { return toolbox::RNA_KINEMATIC_CLOSER; }

	/// @brief Initialization
	void init() override;
	void init( const Pose & pose_current, const Pose & pose_closed, bool use_pose_closed = true );

	using StepWiseSamplerSized::apply;

	/// @brief Apply the i-th rotamer to pose
	void apply( core::pose::Pose & pose, core::Size const id ) override;

	/// @brief Get the total number of rotamers in sampler
	core::Size size() const override { return nsol_; }

	/// @brief Set verbose
	void set_verbose( bool const setting ) { verbose_ = setting; }

	void set_calculate_jacobians( bool const & setting ){ calculate_jacobians_ = setting; }
	bool calculate_jacobians() const { return calculate_jacobians_; }

	/// @brief Calculate the jacobian
	core::Real get_jacobian(Pose & pose) const;

	utility::vector1< core::Real > get_all_jacobians() const { return all_jacobians_; }

private:

	void figure_out_dof_ids_and_offsets( core::pose::Pose const & pose );

	void figure_out_offset( core::id::DOF_ID const & dof_id,
		core::Real const original_torsion_value,
		core::pose::Pose const & pose );

	void fill_chainTORS( core::pose::Pose const & pose );

	void output_chainTORS() const;

	//Disable copy constructor and assignment
	RNA_KinematicCloser( const RNA_KinematicCloser & );
	void operator=( const RNA_KinematicCloser & );

	core::pose::Pose const & init_pose_; // must specify
	core::Size const moving_suite_, chainbreak_suite_;
	bool verbose_ = false;
	core::Size nsol_ = 0;

	utility::vector1< core::id::NamedAtomID > atom_ids_, pivot_ids_;
	utility::vector1< core::Real > offset_save_, all_jacobians_;
	utility::vector1< core::id::DOF_ID > dof_ids_;

	utility::vector1< utility::vector1< core::Real > > t_ang_, b_ang_, b_len_;
	utility::vector1< utility::fixedsizearray1< core::Real, 3 > > atoms_;
	utility::vector1< core::Real > dt_ang_, db_len_, db_ang_;

	bool calculate_jacobians_ = false;
	bool is_TNA_ = false;
};


} //rna
} //sampler
} //stepwise
} //protocols

#endif
