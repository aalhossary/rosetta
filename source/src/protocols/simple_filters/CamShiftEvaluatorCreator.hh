// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/simple_filters/CamShiftEvaluatorCreator.hh
/// @brief  Header for CamShiftEvaluatorCreator
/// @author Matthew O'Meara

#ifndef INCLUDED_protocols_simple_filters_CamShiftEvaluatorCreator_hh
#define INCLUDED_protocols_simple_filters_CamShiftEvaluatorCreator_hh

// Unit Headers
#include <protocols/evaluation/EvaluatorCreator.hh>

#include <protocols/evaluation/PoseEvaluator.fwd.hh>



namespace protocols {
namespace simple_filters {

/// @brief creator for the CamShiftEvaluatorCreator class
class CamShiftEvaluatorCreator : public evaluation::EvaluatorCreator
{
public:
	CamShiftEvaluatorCreator() : options_registered_(false) {};
	~CamShiftEvaluatorCreator() override;

	virtual void register_options();

	void add_evaluators( evaluation::MetaPoseEvaluator & eval ) const override;

	std::string type_name() const override;

private:
	bool options_registered_;
};

} //namespace
} //namespace

#endif
