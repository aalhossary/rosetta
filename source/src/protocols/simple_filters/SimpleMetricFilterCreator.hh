// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/simple_filters/SimpleMetricFilterCreator.hh
/// @brief A filter takes any RealMetric and applies a set cutoff to filter the model.
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com)

#ifndef INCLUDED_protocols_simple_filters_SimpleMetricFilterCreator_hh
#define INCLUDED_protocols_simple_filters_SimpleMetricFilterCreator_hh

#include <protocols/filters/FilterCreator.hh>
//#include <utility/tag/XMLSchemaGeneration.fwd.hh> //transcluded from FilterCreator.hh

namespace protocols {
namespace simple_filters {

class SimpleMetricFilterCreator : public protocols::filters::FilterCreator {
public:
	protocols::filters::FilterOP create_filter() const override;
	std::string keyname() const override;
	void provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const override;

};

} //protocols
} //simple_filters

#endif //INCLUDED_protocols_simple_filters_SimpleMetricFilter_fwd_hh
