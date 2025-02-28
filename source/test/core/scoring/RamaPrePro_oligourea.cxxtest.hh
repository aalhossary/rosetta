// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  core/scoring/RamaPrePro.cxxtest.hh
/// @brief  Unit tests for the RamaPrePro energy.
/// @author Vikram K. Mulligan (vmullig@u.washington.edu)


// Test headers
#include <test/core/scoring/RamaPrePro_util.hh>
#include <cxxtest/TestSuite.h>

// Project Headers

// Core Headers

// Protocol Headers

// Basic Headers
#include <basic/Tracer.hh>

#include <core/init_util.hh> // AUTO IWYU For core_init_with_additional_options

static basic::Tracer TR("RamaPreProOligoureaTests");


class RamaPreProTests_oligourea : public CxxTest::TestSuite {
	//Define Variables

public:

	void setUp(){
		core_init_with_additional_options("-output_virtual true -write_all_connect_info true");
	}

	void tearDown(){
	}

	/// @brief Test the drawing of random mainchain torsion values from the
	/// Ramachandran probability distribution.
	void test_random_tors_canonical() {
		do_ramaprepro_test("AX[OU3_ALA]AA", false, false);
	}

	/// @brief Test the drawing of random mainchain torsion values from the
	/// Ramachandran probability distribution for a pre-proline amino acid.
	void test_random_tors_canonical_prepro() {
		do_ramaprepro_test("AX[OU3_ALA]PA", false, false);
	}

	/// @brief Test the drawing of random mainchain torsion values from the
	/// Ramachandran probability distribution for a D-amino acid.
	void test_random_tors_canonical_d_aa() {
		do_ramaprepro_test("AX[OU3_ALA]AA", false, true);
	}

	/// @brief Test the drawing of random mainchain torsion values from the
	/// Ramachandran probability distribution for a D-pre-proline amino acid.
	void test_random_tors_canonical_d_aa_prepro() {
		do_ramaprepro_test("AX[OU3_ALA]PA", false, true);
	}

};



