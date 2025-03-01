// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file utility/io/GeneralFileManager.cc
/// @brief A singleton class for managing arbitrary files to ensure that they are loaded once and only once from disk.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org)
/// @author Jack Maguire

#include <utility/io/GeneralFileManager.hh>

// Unit headers

// Utility headers
#include <utility/pointer/memory.hh>
#include <utility/thread/threadsafe_creation.hh>
#include <utility/string_util.hh>

// Basic headers
//#include <basic/Tracer.hh>

// C++ headers
#include <string>
#include <fstream>

// Boost headers
#include <functional>

// Construct tracer.
//static basic::Tracer TR( "utility.io.GeneralFileManager" );

namespace utility {
namespace io {


// GeneralFileContents Public methods /////////////////////////////////////////////////////////////
/// @brief File contents constructor.
GeneralFileContents::GeneralFileContents( std::string const & filename) :
	utility::VirtualBase()
{
	file_contents_ = utility::file_contents( filename );
}

/// @brief Destructor.
GeneralFileContents::~GeneralFileContents() {}

/// @brief Clone function: make a copy of this object and return an owning pointer to the copy.
GeneralFileContentsOP
GeneralFileContents::clone() const {
	return utility::pointer::make_shared< GeneralFileContents >(*this);
}

// GeneralFileManager Public methods /////////////////////////////////////////////////////////////
// Static constant data access

std::string const &
GeneralFileManager::get_file_contents(
	std::string const & filename
) const {
	std::function< GeneralFileContentsOP () > creator( std::bind( &GeneralFileManager::create_instance, std::cref( filename ) ) );
	auto ptr = utility::thread::safely_check_map_for_key_and_insert_if_absent( creator, SAFELY_PASS_MUTEX( io_script_mutex_ ), filename, filename_to_filecontents_map_ );
	return ptr->get_file_contents();
}

// GeneralFileManager Private methods ////////////////////////////////////////////////////////////

/// @brief Empty constructor.
GeneralFileManager::GeneralFileManager() :
	SingletonBase< GeneralFileManager >(),
	filename_to_filecontents_map_()
#ifdef MULTI_THREADED
	,
	io_script_mutex_()
#endif //MULTI_THREADED
{}

/// @brief Create an instance of a GeneralFileContents object, by owning pointer.
/// @details Needed for threadsafe creation.  Loads data from disk.  NOT for repeated calls!
/// @note Not intended for use outside of GeneralFileManager.
GeneralFileContentsOP
GeneralFileManager::create_instance(
	std::string const & filename
) {
	return utility::pointer::make_shared< GeneralFileContents >( filename );
}







//////////////////////////////////////////////
// GeneralFileContentsVector Public methods /////////////////////////////////////////////////////////////
/// @brief File contents constructor.
GeneralFileContentsVector::GeneralFileContentsVector( std::string const & filename ) :
	utility::VirtualBase()
{
	std::string contents = utility::file_contents( filename );
	file_contents_ = utility::string_split_simple(contents, '\n');
}

/// @brief Destructor.
GeneralFileContentsVector::~GeneralFileContentsVector() {}

/// @brief Clone function: make a copy of this object and return an owning pointer to the copy.
GeneralFileContentsVectorOP
GeneralFileContentsVector::clone() const {
	return utility::pointer::make_shared< GeneralFileContentsVector >(*this);
}

// GeneralFileManager Public methods /////////////////////////////////////////////////////////////
// Static constant data access

utility::vector1<std::string> const &
GeneralFileManagerVector::get_file_contents(
	std::string const & filename
) const {
	std::function< GeneralFileContentsVectorOP () > creator( std::bind( &GeneralFileManagerVector::create_instance, std::cref( filename ) ) );
	auto ptr = utility::thread::safely_check_map_for_key_and_insert_if_absent( creator, SAFELY_PASS_MUTEX( io_script_mutex_ ), filename, filename_to_filecontents_map_ );
	return ptr->get_file_contents();
}

// GeneralFileManagerVector Private methods ////////////////////////////////////////////////////////////

/// @brief Empty constructor.
GeneralFileManagerVector::GeneralFileManagerVector() :
	SingletonBase< GeneralFileManagerVector >(),
	filename_to_filecontents_map_()
#ifdef MULTI_THREADED
	,
	io_script_mutex_()
#endif //MULTI_THREADED
{}

/// @brief Create an instance of a GeneralFileContents object, by owning pointer.
/// @details Needed for threadsafe creation.  Loads data from disk.  NOT for repeated calls!
/// @note Not intended for use outside of GeneralFileManager.
GeneralFileContentsVectorOP
GeneralFileManagerVector::create_instance(
	std::string const & filename
) {
	return utility::pointer::make_shared< GeneralFileContentsVector >( filename );
}

} //utility
} //io
