// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/task_operations/DsspDesignOperation.hh
/// @brief  Design residues with selected amino acids depending on DSSP secondary structure
/// assignment. All functionality here is included in the LayerDesign task operation, but
/// this filter has significantly reduced overhead by avoiding slow SASA calculations.
/// @author Brian Koepnick (koepnick@uw.edu)


#ifndef INCLUDED_protocols_task_operations_DsspDesignOperation_hh
#define INCLUDED_protocols_task_operations_DsspDesignOperation_hh

// Unit headers
#include <protocols/task_operations/DsspDesignOperation.fwd.hh>

// Core headers
#include <core/pack/task/operation/TaskOperation.hh>
#include <core/pack/task/PackerTask.fwd.hh>

// Protocol headers
#include <protocols/parser/BluePrint.fwd.hh>

// Utility headers
#include <utility/tag/XMLSchemaGeneration.fwd.hh>
#include <utility/vector1.hh>

// C++ headers
#include <string>
#include <map>

class DsspDesignOperationTests; // Forward declaration of test class for friendship

namespace protocols {
namespace task_operations {

/// @brief TaskOperation that can be used to restrict AAs allowed at each position
/// based on DSSP-assigned secondary structure
class DsspDesignOperation : public core::pack::task::operation::TaskOperation {
	friend class ::DsspDesignOperationTests;

public:

	typedef core::pack::task::operation::TaskOperation parent;
	typedef core::pack::task::operation::TaskOperationOP TaskOperationOP;

	typedef protocols::parser::BluePrint BluePrint;
	typedef protocols::parser::BluePrintOP BluePrintOP;

	typedef std::map< std::string, std::string > SecStructResidues;
	typedef std::pair< std::string, std::string > SecStruct;

public:

	/// @brief default constructor
	DsspDesignOperation();

	/// @brief copy constructor
	DsspDesignOperation( DsspDesignOperation const & rval );

	/// @brief destructor
	~DsspDesignOperation() override;

	/// @brief make clone
	TaskOperationOP clone() const override;

public:

	/// @brief define secondary structure from a blueprint
	void set_blueprint( BluePrintOP const bp );

	/// @brief define allowed residues for some SSE
	void set_restrictions_aa( std::string const & sse, std::string const & aas );

	/// @brief append allowed residues for some SSE
	void set_restrictions_append( std::string const & sse, std::string const & aas );

	/// @brief exclude allowed residues for some SSE
	void set_restrictions_exclude( std::string const & sse, std::string const & aas );

	static void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );
	static std::string keyname() { return "DsspDesign"; }

private:

	/// @brief map sets of allowed residues to secondary structure elements
	SecStructResidues sse_residues_;

	BluePrintOP blueprint_;

	std::string pose_secstruct_;

	/// @brief initialize allowed residue sets with defaults
	void set_default_sse_residues();

	/// @brief get the set of allowed residues for some secondary structure element
	utility::vector1< bool > get_restrictions( std::string const & ss_type ) const;

public:

	/// @brief parse RosettaScripts XML
	void parse_tag( TagCOP tag , DataMap & ) override;

	/// @brief apply
	void apply( Pose const & pose, core::pack::task::PackerTask & task ) const override;

};

} // task_operations
} // protocols

#endif  //INCLUDED_protocols_task_operations_DsspDesignOperation_hh
