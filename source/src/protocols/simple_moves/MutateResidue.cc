// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @author Spencer Bliven <blivens@u.washington.edu>
/// @date 6/26/2009
/// @edit Yang Hsia <yhsia@uw.edu> added extra option for mutating a residue to itself (to remove TERcards for RotamerLinks)

// Unit headers
#include <protocols/simple_moves/MutateResidue.hh>
#include <protocols/simple_moves/MutateResidueCreator.hh>

// Project Headers
#include <core/types.hh>
#include <core/pose/Pose.hh>

#include <core/conformation/util.hh>
#include <core/conformation/Residue.hh>
#include <core/chemical/AA.hh>
#include <core/select/residue_selector/ResidueSelector.hh>
#include <core/conformation/ResidueFactory.hh>
#include <core/select/residue_selector/util.hh>
//parsing
#include <utility/tag/Tag.hh>
#include <protocols/rosetta_scripts/util.hh>
#include <core/pose/selection.hh>
#include <core/pose/util.hh>
#include <basic/Tracer.hh>
#include <core/conformation/Conformation.hh>
#include <utility/excn/Exceptions.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>


// Utility Headers

// Unit Headers

// C++ headers

namespace protocols {
namespace simple_moves {

using namespace core;
using namespace core::chemical;
using namespace std;

using core::pose::Pose;

static basic::Tracer TR( "protocols.simple_moves.MutateResidue" );

/// @brief default ctor
MutateResidue::MutateResidue() :
	parent()
{}

/// @brief copy ctor
MutateResidue::MutateResidue(MutateResidue const& /*dm*/) = default;

/// @brief Mutate a single residue to a new amino acid
/// @param target The residue index to mutate
/// @param new_res The name of the replacement residue

MutateResidue::MutateResidue( core::Size const target, string const & new_res ) :
	parent(),
	target_(""),
	res_name_(new_res)
{
	set_target( target );
}

MutateResidue::MutateResidue( core::Size const target, int const new_res ) :
	parent(),
	target_(""),
	res_name_( name_from_aa( aa_from_oneletter_code( new_res ) ) )
{
	set_target( target );
}

MutateResidue::MutateResidue( core::Size const target, core::chemical::AA const aa) :
	parent(),
	target_(""),
	res_name_( name_from_aa( aa ))
{
	set_target( target );
}

/**
* @brief Reinitialize this protocols::moves::Mover with parameters from the specified tags.
* @details Parameters recognized:
*  - target_pdb_num or target_res_num. A single target residue to form disulfides to
*  - target_pdb_nums or target_res_nums. A list of possible target residues
*/
void MutateResidue::parse_my_tag( utility::tag::TagCOP tag,
	basic::datacache::DataMap & data
)
{

	// Set target to the residue specified by "target_pdb_num" or "target_res_num":
	if ( !tag->hasOption("target") && !tag->hasOption("residue_selector") ) {
		TR.Error << "You need to define either a target residue using 'target' or a residue selector using 'residue_selector'." << std::endl;
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "");
	}

	if ( tag->hasOption("target") && tag->hasOption("residue_selector") ) {
		TR.Error << "You can only degine a target residue using 'target' or a residue selector using 'residue_selector' but not both." << std::endl;
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "");
	}

	if ( tag->hasOption("target") ) {
		set_target( tag->getOption<string>("target") );
	}
	//set_target( core::pose::parse_resnum( tag->getOption<string>("target"), pose ) );

	//Set the identity of the new residue:
	if ( !tag->hasOption("new_res") ) {
		TR.Error << "no 'new_res' parameter specified." << std::endl;
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, "");
	}
	set_res_name( tag->getOption<string>("new_res") );

	//set if you want to mutate the residue to itself, default false.
	mutate_self_ = tag->getOption< bool >( "mutate_self", false );

	//Set whether the mover should try to preserve atom XYZ coordinates or not.  (Default false).
	set_preserve_atom_coords( tag->getOption<bool>("preserve_atom_coords", false) );

	//Set whether we're updating coordinates of polymer bond-dependent atoms.
	set_update_polymer_dependent( tag->getOption<bool>( "update_polymer_bond_dependent", update_polymer_dependent() ) );

	//Set whether we correctly break disulfide bonds.
	set_break_disulfide_bonds( tag->getOption< bool >( "break_disulfide_bonds", break_disulfide_bonds() ) );

	if ( tag->hasOption("residue_selector") ) {
		set_selector( protocols::rosetta_scripts::parse_residue_selector( tag, data ) );
	}

	return;
}

/// @brief Set this mover's target residue index, based on the Rosetta indexing.
///
void MutateResidue::set_target(core::Size const target_in)
{
	std::stringstream target_in_string;
	target_in_string << target_in;
	target_ = target_in_string.str();
}

void MutateResidue::set_res_name( core::chemical::AA const & aa){
	res_name_ = name_from_aa( aa );
}

/// @brief Set whether disulfide bonds are properly broken when
/// mutating *from* a disulfide-bonded cysteine *to* anything else.
/// @details True by default.  If false, the other cysteine will still think that
/// it is disulfide-bonded to something, and scoring will fail.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
void
MutateResidue::set_break_disulfide_bonds(
	bool const setting
) {
	break_disulfide_bonds_ = setting;
}

void MutateResidue::apply( Pose & pose ) {

	// Converting the target string to target residue index must be done at apply time,
	// since the string might refer to a residue in a reference pose.
	if ( selector_ ) {
		core::select::residue_selector::ResidueSubset subset=selector_->apply( pose );

		for ( core::Size ir=1, irmax=pose.total_residue(); ir<=irmax; ++ir ) {

			if ( subset[ir] ) {
				make_mutation(pose,ir);
			}
		}
	} else {
		core::Size const rosetta_target( core::pose::parse_resnum( target(), pose, true /*"true" must be specified to check for refpose numbering when parsing the string*/ ) );
		make_mutation(pose,rosetta_target);
	}
}

std::string MutateResidue::get_name() const {
	return mover_name();
}

std::string MutateResidue::mover_name() {
	return "MutateResidue";
}

void MutateResidue::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute(
		"target", xs_string,
		"The location to mutate. This can be a PDB number (e.g. 31A), a Rosetta index (e.g. 177), "
		"or an index in a reference pose or snapshot stored at a point in a protocol "
		"before residue numbering changed in some way (e.g. refpose(snapshot1,23)). "
		"See the convention on residue indices in the RosettaScripts Conventions documentation for details");

	attlist + XMLSchemaAttribute::required_attribute(
		"new_res", xs_string,
		"The name of the residue to introduce. This string should correspond to "
		"the ResidueType::name() function (eg ASP).");

	attlist + XMLSchemaAttribute::attribute_w_default(
		"mutate_self", xsct_rosetta_bool,
		"If true, will mutate the selected residue to itself, regardless of what new_res "
		"is set to (although new_res is still required). This is useful to \"clean\" residues when "
		"there are Rosetta residue incompatibilities (such as terminal residues) with movers and filters.",
		"false");

	attlist + XMLSchemaAttribute::attribute_w_default(
		"perserve_atom_coords", xsct_rosetta_bool,
		"If true, then atoms in the new residue that have names matching atoms in the old residue "
		"will be placed at the coordinates of the atoms in the old residue, with other atoms "
		"rebuilt based on ideal coordinates. If false, then only the mainchain heavyatoms are "
		"placed based on the old atom's mainchain heavyatoms; the sidechain is built from ideal "
		"coordinates, and sidechain torsion values are then set to the sidechain torsion values "
		"from the old residue. False if unspecified.",
		"false");

	attlist + XMLSchemaAttribute(
		"update_polymer_bond_dependent", xsct_rosetta_bool,
		"Update the coordinates of atoms that depend on polymer bonds");

	attlist + XMLSchemaAttribute( "preserve_atom_coords", xsct_rosetta_bool,
		"Preserve atomic coords as much as possible" );

	attlist + XMLSchemaAttribute::attribute_w_default(
		"break_disulfide_bonds", xsct_rosetta_bool,
		"If true, then disulfide bonds are properly broken when mutating from a disulfide-bonded "
		"cysteine to anything else.  If false, the disulfide partner will still think that "
		"it is bonded to something, and scoring will fail unless the disulfide variant type is "
		"removed by something else.  Setting this to false is strongly NOT recommended.  True by "
		"default.",
		"true"
	);

	core::select::residue_selector::attributes_for_parse_residue_selector(
		attlist, "residue_selector",
		"name of a residue selector that specifies the subset to be mutated" );

	protocols::moves::xsd_type_definition_w_attributes(
		xsd, mover_name(),
		"Change a single residue or a given subset of residues to a different type. For instance, "
		"mutate Arg31 to an Asp, or mutate all Prolines to Alanine.  Note that by default, this mover "
		"breaks disulfide bonds, converting any parters of disulfide-bonded target residues to the reduced "
		"variant type.",
		attlist );
}

//////////////// PRIVATE FUNCTIONS ////////////////

/// @brief Actually make a mutation at a position, based on the current configuration of
/// this mover.
void
MutateResidue::make_mutation(
	core::pose::Pose & pose,
	core::Size rosetta_target
) {
	if ( rosetta_target < 1 ) {
		// Do nothing for 0
		return;
	}
	if ( rosetta_target > pose.size() ) {
		TR.Error << "Residue "<< rosetta_target <<" is out of bounds." << std::endl;
		utility_exit();
	}

	if ( mutate_self_ ) {
		TR << "Setting target residue: " << rosetta_target << " to self (" << pose.residue( rosetta_target ).name3() << ")" << std::endl;
		set_res_name( pose.residue( rosetta_target ).name3() ); //sets res_name to the residue of target
	}

	if ( TR.Debug.visible() ) {
		TR.Debug << "Mutating residue " << rosetta_target << " from "
			<< pose.residue( rosetta_target ).name3() << " to " << res_name_ <<" ." << std::endl;
	}

	if ( pose.residue_type(rosetta_target).is_disulfide_bonded() ) {
		if ( break_disulfide_bonds() ) {
			break_a_disulfide( pose, rosetta_target );
		} else {
			//Print a warning if we're not breaking the disulfide bond:
			core::Size const other_res( core::conformation::get_disulf_partner( pose.conformation(), rosetta_target ) );
			TR.Warning << TR.Red << "Warning!  Target residue " << pose.residue_type(rosetta_target).base_name() << rosetta_target << " has a disulfide bond to " << pose.residue_type(other_res).base_name() << other_res << ", but the MutateResidue mover is not configured to break disulfide bonds.  An unbonded residue with the DISULFIDE variant type will be left at position " << other_res << "!" << TR.Reset << std::endl;
		}
	}

	chemical::ResidueTypeCOP new_restype( core::pose::get_restype_for_pose( pose, res_name_, pose.residue_type( rosetta_target ).mode() ) );
	// Create the new residue and replace it
	conformation::ResidueOP new_res = conformation::ResidueFactory::create_residue(
		*new_restype, pose.residue( rosetta_target ),
		pose.conformation());
	// Make sure we retain as much info from the previous res as possible
	conformation::copy_residue_coordinates_and_rebuild_missing_atoms( pose.residue( rosetta_target ),
		*new_res, pose.conformation(), !preserve_atom_coords() );
	pose.replace_residue( rosetta_target, *new_res, false );

	if ( update_polymer_dependent() ) { //Update the coordinates of atoms that depend on polymer bonds:
		pose.conformation().rebuild_polymer_bond_dependent_atoms_this_residue_only( rosetta_target );
	}

}

/// @brief Given that position X is involved in a disulfide bond, break the disulfide between
/// the residue at position X and its partner.
/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
void
MutateResidue::break_a_disulfide(
	core::pose::Pose & pose,
	core::Size const rosetta_target
) const {
	debug_assert( pose.residue_type(rosetta_target).is_disulfide_bonded() );
	core::Size const other_residue( core::conformation::get_disulf_partner( pose.conformation(), rosetta_target ) );
	debug_assert( other_residue > 0 );
	TR << "Breaking disulfide bond between " << pose.residue_type(rosetta_target).base_name() << rosetta_target << " and " << pose.residue_type(other_residue).base_name() << other_residue << "." << std::endl;
	core::conformation::break_disulfide( pose.conformation(), rosetta_target, other_residue );
}

std::string MutateResidueCreator::keyname() const {
	return MutateResidue::mover_name();
}

protocols::moves::MoverOP
MutateResidueCreator::create_mover() const {
	return utility::pointer::make_shared< MutateResidue >();
}

void MutateResidueCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	MutateResidue::provide_xml_schema( xsd );
}

void MutateResidue::set_selector(
	core::select::residue_selector::ResidueSelectorCOP selector_in
) {
	if ( selector_in ) {
		selector_ = selector_in;
	} else {
		utility_exit_with_message("Error in protocols::simple_moves::MutateResidue::set_selector(): Null pointer passed to function!");
	}
	return;
}





} // moves
} // protocols
