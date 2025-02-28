// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/loops/FoldTreeFromLoops.cc
/// @brief
/// @author Sarel Fleishman (sarelf@u.washington.edu)

// Unit headers
#include <protocols/loops/FoldTreeFromLoopsWrapper.hh>
#include <protocols/loops/FoldTreeFromLoopsWrapperCreator.hh>

// Package headers
#include <protocols/loops/Loop.hh>
#include <protocols/loops/Loops.hh>
#include <protocols/loops/loops_main.hh>
#include <protocols/loops/loops_definers/util.hh>
#include <protocols/loops/loops_definers/LoopsDefiner.hh>

// Project headers
#include <core/kinematics/FoldTree.hh>
#include <core/pose/Pose.hh>

// Utility headers
#include <basic/Tracer.hh>
#include <utility/tag/Tag.hh>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

namespace protocols {
namespace loops {

static basic::Tracer TR( "protocols.loops.FoldTreeFromLoopsWrapper" );

FoldTreeFromLoops::FoldTreeFromLoops() :
	Mover( FoldTreeFromLoops::mover_name() )
{}

FoldTreeFromLoops::~FoldTreeFromLoops() = default;

protocols::moves::MoverOP FoldTreeFromLoops::clone() const
{
	return utility::pointer::make_shared< FoldTreeFromLoops >( *this );
}

protocols::moves::MoverOP FoldTreeFromLoops::fresh_instance() const
{
	return utility::pointer::make_shared< FoldTreeFromLoops >();
}

void
FoldTreeFromLoops::apply( core::pose::Pose & pose )
{
	LoopsOP local_loops = loops( pose );
	if ( local_loops->empty() ) {
		utility_exit_with_message( "No loops were specified");
	}
	core::kinematics::FoldTree f;
	fold_tree_from_loops( pose, *local_loops, f );
	TR<<"old foldtree "<<pose.fold_tree()<<"\nNew foldtree ";
	pose.fold_tree( f );
	TR<<pose.fold_tree()<<std::endl;
	if ( add_cp_variants_ ) {
		protocols::loops::add_cutpoint_variants(pose);
	}
}


void
FoldTreeFromLoops::parse_my_tag(
	TagCOP const tag,
	basic::datacache::DataMap & data
) {

	loop_definer_ = loops_definers::load_loop_definitions(tag, data);
	add_cutpoint_variants(tag->getOption<bool>("add_cp_variants", add_cp_variants_));
}

void FoldTreeFromLoops::loops( LoopsOP const l )
{
	loops_ = l;
}

LoopsOP FoldTreeFromLoops::loops( core::pose::Pose const & pose ) const
{
	if ( loops_ != nullptr ) {
		return loops_->clone();
	}
	runtime_assert( loop_definer_ != nullptr );
	return utility::pointer::make_shared< Loops >(loop_definer_->apply(pose));
}



std::string FoldTreeFromLoops::get_name() const {
	return mover_name();
}

std::string FoldTreeFromLoops::mover_name() {
	return "FoldTreeFromLoops";
}

void FoldTreeFromLoops::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	loops_definers::attributes_for_load_loop_definitions( attlist );
	attlist + XMLSchemaAttribute("add_cp_variants", xsct_rosetta_bool, "Add cutpoint variants used by the Chainbreak energy term? default false");

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "Helper mover that looks for loop definitions and sets up the fold tree.", attlist );
}

std::string FoldTreeFromLoopsCreator::keyname() const {
	return FoldTreeFromLoops::mover_name();
}

void
FoldTreeFromLoops::add_cutpoint_variants( bool add_cp_variants ){
	add_cp_variants_ = add_cp_variants;
}

protocols::moves::MoverOP
FoldTreeFromLoopsCreator::create_mover() const {
	return utility::pointer::make_shared< FoldTreeFromLoops >();
}

void FoldTreeFromLoopsCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	FoldTreeFromLoops::provide_xml_schema( xsd );
}


} // namespace loops
} // namespace protocols
