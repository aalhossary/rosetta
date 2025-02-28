// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/moves/mover_schemas.cc
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

// Unit header
#include <protocols/moves/mover_schemas.hh>
#include <protocols/moves/MoverFactory.hh>

// Package headers
#include <utility/tag/XMLSchemaGeneration.hh>
#include <basic/Tracer.hh>

namespace protocols {
namespace moves {

static basic::Tracer TR("protocols.moves.mover_schemas");

std::string
complex_type_name_for_mover( std::string const & mover_name )
{
	return "mover_" + mover_name + "_type";
}

void
xsd_type_definition_w_attributes(
	utility::tag::XMLSchemaDefinition & xsd,
	std::string const & mover_type,
	std::string const & description,
	utility::tag::AttributeList const & attributes
)
{
	// If the developer has already added a name attribute, do not add another one.
	utility::tag::AttributeList local_attrs( attributes );
	if ( ! utility::tag::attribute_w_name_in_attribute_list( "name", local_attrs ) ) {
		local_attrs + utility::tag::optional_name_attribute();
	}

	std::string citation_string;
	if ( xsd.include_citation_info() ) {
		citation_string = "\n\n" + protocols::moves::MoverFactory::get_instance()->get_citation_humanreadable( mover_type );
	}

	utility::tag::XMLSchemaComplexTypeGenerator ct_gen;
	ct_gen.complex_type_naming_func( & complex_type_name_for_mover )
		.element_name( mover_type )
		.description( description + citation_string )
		.add_attributes( local_attrs )
		.write_complex_type_to_schema( xsd );
}

void
xsd_type_definition_w_attributes_and_repeatable_subelements(
	utility::tag::XMLSchemaDefinition & xsd,
	std::string const & mover_type,
	std::string const & description,
	utility::tag::AttributeList const & attributes,
	utility::tag::XMLSchemaSimpleSubelementList const & subelements
)
{
	// If the developer has already added a name attribute, do not add another one.
	utility::tag::AttributeList local_attrs( attributes );
	if ( ! utility::tag::attribute_w_name_in_attribute_list( "name", local_attrs ) ) {
		local_attrs + utility::tag::optional_name_attribute();
	}

	std::string citation_string;
	if ( xsd.include_citation_info() ) {
		citation_string = "\n\n" + protocols::moves::MoverFactory::get_instance()->get_citation_humanreadable( mover_type );
	}

	utility::tag::XMLSchemaComplexTypeGenerator ct_gen;
	ct_gen.complex_type_naming_func( & complex_type_name_for_mover )
		.element_name( mover_type )
		.description( description + citation_string )
		.add_attributes( local_attrs )
		.set_subelements_repeatable( subelements )
		.write_complex_type_to_schema( xsd );
}

void
xsd_type_definition_w_attributes_and_single_subelement(
	utility::tag::XMLSchemaDefinition & xsd,
	std::string const & mover_type,
	std::string const & description,
	utility::tag::AttributeList const & attributes,
	utility::tag::XMLSchemaSimpleSubelementList const & subelement
){
	// If the developer has already added a name attribute, do not add another one.
	utility::tag::AttributeList local_attrs( attributes );
	if ( ! utility::tag::attribute_w_name_in_attribute_list( "name", local_attrs ) ) {
		local_attrs + utility::tag::optional_name_attribute();
	}

	utility::tag::XMLSchemaComplexTypeGenerator ct_gen;
	ct_gen.complex_type_naming_func( & complex_type_name_for_mover )
		.element_name( mover_type )
		.description( description )
		.add_attributes( local_attrs )
		.set_subelements_single_appearance_optional( subelement )
		.write_complex_type_to_schema( xsd );
}

}  // namespace moves
}  // namespace protocols
