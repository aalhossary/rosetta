// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file src/core/import_pose/pose_stream/LazySilentFilePoseInputStream.hh
/// @brief
/// @author James Thompson

// libRosetta headers

#include <core/types.hh>

#include <core/chemical/ResidueTypeSet.fwd.hh>

#include <core/pose/Pose.hh>
#include <core/pose/datacache/CacheableDataType.hh>

#include <core/import_pose/pose_stream/LazySilentFilePoseInputStream.hh>
#include <core/pose/extra_pose_info_util.hh>

#include <utility/vector1.hh>
#include <utility/file/FileName.hh>

#include <core/io/silent/SilentStruct.hh>
#include <core/io/silent/SilentFileData.hh>
#include <core/io/silent/SilentFileOptions.hh>

#include <basic/Tracer.hh>
#include <basic/datacache/BasicDataCache.hh>
#include <basic/datacache/CacheableString.hh>

#include <basic/options/option.hh>
#include <basic/options/keys/run.OptionKeys.gen.hh>

// C++ headers
#include <string>

//Auto Headers

namespace core {
namespace import_pose {
namespace pose_stream {

static basic::Tracer tr( "core.io.pose_stream.lazy_silent_file" );

using string = std::string;
using FileName = utility::file::FileName;

LazySilentFilePoseInputStream::LazySilentFilePoseInputStream( utility::vector1< FileName > const & fns ):
	filenames_( fns ),
	current_filename_( filenames_.begin() ),
	sfd_( core::io::silent::SilentFileOptions() )
{
	sfd_.read_file( *current_filename_ );
	current_struct_   = sfd_.begin();
}

LazySilentFilePoseInputStream::LazySilentFilePoseInputStream() :
	sfd_( core::io::silent::SilentFileOptions() )
{
	LazySilentFilePoseInputStream::reset(); // Virtual dispatch doesn't work fully during constructor
}

bool LazySilentFilePoseInputStream::has_another_pose() {
	bool has_another(true);
	if ( current_filename_ == filenames_.end() && current_struct_ == sfd_.end() ) {
		has_another = false;
	}
	return has_another;
}

void LazySilentFilePoseInputStream::fill_pose(
	core::pose::Pose & pose,
	core::chemical::ResidueTypeSet const & residue_set,
	bool const metapatches /*= true*/
) {
	// check to make sure that we have more poses!
	if ( !has_another_pose() ) {
		utility_exit_with_message(
			"LazySilentFilePoseInputStream: called fill_pose, but I have no more Poses!"
		);
	}


	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	if ( option[ run::debug ]() ) {
		core::Real debug_rmsd = current_struct_->get_debug_rmsd();
		tr.Debug << "RMSD to original coordinates for tag "
			<< current_struct_->decoy_tag() << " = " << debug_rmsd << std::endl;
	}

	current_struct_->fill_pose( pose, residue_set, metapatches );

	using namespace basic::datacache;
	pose.data().set(
		core::pose::datacache::CacheableDataType::JOBDIST_OUTPUT_TAG,
		utility::pointer::make_shared< basic::datacache::CacheableString >( current_struct_->decoy_tag() )
	);
	tr.Debug << "decoy_tag() == " << current_struct_->decoy_tag() << std::endl;

	core::pose::setPoseExtraScore( pose, "silent_score", current_struct_->get_energy( "score" ) );
	preprocess_pose( pose );

	// increment counters
	++current_struct_;
	if ( current_struct_ == sfd_.end() && current_filename_ != filenames_.end() ) {
		++current_filename_;
		if ( current_filename_ != filenames_.end() ) {
			sfd_.clear();
			tr.Debug << "reading silent-file named " << *current_filename_ << std::endl;
			sfd_.read_file( *current_filename_ );
			current_struct_ = sfd_.begin();
		}
	}

	tr.flush_all_channels();
} // fill_pose

void LazySilentFilePoseInputStream::fill_pose(
	core::pose::Pose & pose,
	bool const metapatches /*= true*/
) {
	// check to make sure that we have more poses!
	if ( !has_another_pose() ) {
		utility_exit_with_message(
			"LazySilentFilePoseInputStream: called fill_pose, but I have no more Poses!"
		);
	}


	using namespace basic::options;
	using namespace basic::options::OptionKeys;
	if ( option[ run::debug ]() ) {
		core::Real debug_rmsd = current_struct_->get_debug_rmsd();
		tr.Debug << "RMSD to original coordinates for tag "
			<< current_struct_->decoy_tag() << " = " << debug_rmsd << std::endl;
	}

	current_struct_->fill_pose( pose, metapatches );

	using namespace basic::datacache;
	pose.data().set(
		core::pose::datacache::CacheableDataType::JOBDIST_OUTPUT_TAG,
		utility::pointer::make_shared< basic::datacache::CacheableString >( current_struct_->decoy_tag() )
	);
	tr.Debug << "decoy_tag() == " << current_struct_->decoy_tag() << std::endl;

	core::pose::setPoseExtraScore( pose, "silent_score", current_struct_->get_energy( "score" ) );
	preprocess_pose( pose );

	// increment counters
	++current_struct_;
	if ( current_struct_ == sfd_.end() && current_filename_ != filenames_.end() ) {
		++current_filename_;
		if ( current_filename_ != filenames_.end() ) {
			sfd_.clear();
			tr.Debug << "reading silent-file named " << *current_filename_ << std::endl;
			sfd_.read_file( *current_filename_ );
			current_struct_ = sfd_.begin();
		}
	}

	tr.flush_all_channels();
} // fill_pose

/// @brief Get a string describing the last pose and where it came from.
/// @details Input tag + filename.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
std::string
LazySilentFilePoseInputStream::get_last_pose_descriptor_string() const {
	core::io::silent::SilentFileData::iterator temp_iterator( current_struct_ );
	--temp_iterator; //Temporarily shift current position back one.
	std::string const decoy_str(temp_iterator->decoy_tag());
	std::string const outstr( decoy_str.empty() ? sfd_.filename() : decoy_str + "_" + sfd_.filename() );
	return outstr;
}

void LazySilentFilePoseInputStream::reset() {
	filenames_        = utility::vector1< FileName >();
	current_filename_ = filenames_.end();
	current_struct_   = sfd_.end();
}

utility::vector1< FileName > const & LazySilentFilePoseInputStream::filenames() const {
	return filenames_;
}

} // pose_stream
} // import_pose
} // core
