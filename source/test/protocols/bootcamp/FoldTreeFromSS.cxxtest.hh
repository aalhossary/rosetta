// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   test/protocols/match/ProteinSCSampler.cxxtest.hh
/// @brief
/// @author Andrew Leaver-Fay (aleaverfay@gmail.com)

#include <utility>
#include <iostream>
#include <string>

// Test headers
#include <cxxtest/TestSuite.h>

#include <util/pose_funcs.hh>
#include <core/init_util.hh>

// Utility headers
#include <utility/vector1.hh>

/// Project headers
#include <core/types.hh>

#include <core/kinematics/FoldTree.hh>
#include <core/scoring/dssp/Dssp.hh>
#include <core/pose/Pose.hh>
#include <core/types.hh>
#include <utility/vector1.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>

// C++ headers

//Auto Headers


// --------------- Test Class --------------- //

class FoldTreeFromSSTests : public CxxTest::TestSuite {

public:

    // Shared initialization goes here.
	void setUp() override {
		core_init();
	}

	// Shared finalization goes here.
	void tearDown() override {
	}


	// --------------- Test Cases --------------- //
    void test_hello_world(){
        TS_ASSERT(true);
    }

    void test_sample1(){
        std::string sample = "   EEEEE   HHHHHHHH  EEEEE   IGNOR EEEEEE   HHHHHHHHHHH  EEEEE  HHHH   ";
        auto result = protocols::bootcamp::identify_secondary_structure_spans(sample);
        TS_ASSERT_EQUALS(result.size(), 7)
        std::pair< core::Size, core::Size > result_i;
        result_i = result[1];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 4)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 8)

        result_i = result[2];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 12)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 19)

        result_i = result[3];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 22)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 26)

        result_i = result[4];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 36)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 41)

        result_i = result[5];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 45)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 55)

        result_i = result[6];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 58)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 62)

        result_i = result[7];
        TS_ASSERT_EQUALS(std::get<0>(result_i), 65)
        TS_ASSERT_EQUALS(std::get<1>(result_i), 68)


//        TS_ASSERT(queue1->is_empty())
//        TS_ASSERT_EQUALS(queue1->size(), 0)
//        TS_ASSERT_THROWS_ANYTHING(queue1->dequeue())
    }


    void test_fold_tree_from_ss_string(){
        core::kinematics::FoldTreeOP foldTree(nullptr);


        std::cout << "Testing empty" << std::endl;
        foldTree = protocols::bootcamp::fold_tree_from_ss_string("");
        TS_ASSERT(foldTree->empty())

        std::cout << "Testing no SS" << std::endl;
        foldTree = protocols::bootcamp::fold_tree_from_ss_string("AAAAAAAAAAAMMMMMMRRRRRRRRR");
        TS_ASSERT(foldTree->empty())

        std::cout << "Testing example" << std::endl;
        foldTree = protocols::bootcamp::fold_tree_from_ss_string("   EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ");
        std::string expected = "FOLD_TREE  EDGE 7 1 -1  EDGE 7 10 -1  EDGE 7 12 1  EDGE 12 11 -1  EDGE 12 14 -1  EDGE 7 18 2  EDGE 18 15 -1  EDGE 18 21 -1  EDGE 7 26 3  EDGE 26 22 -1  EDGE 26 30 -1  EDGE 7 35 4  EDGE 35 31 -1  EDGE 35 39 -1  EDGE 7 41 5  EDGE 41 40 -1  EDGE 41 43 -1  EDGE 7 48 6  EDGE 48 44 -1  EDGE 48 53 -1  EDGE 7 55 7  EDGE 55 54 -1  EDGE 55 56 -1  EDGE 7 59 8  EDGE 59 57 -1  EDGE 59 62 -1  EDGE 7 67 9  EDGE 67 63 -1  EDGE 67 71 -1  EDGE 7 76 10  EDGE 76 72 -1  EDGE 76 80 -1  EDGE 7 85 11  EDGE 85 81 -1  EDGE 85 89 -1  EDGE 7 92 12  EDGE 92 90 -1  EDGE 92 99 -1 ";
        TS_ASSERT_EQUALS(foldTree->size(), 38)

        std::stringstream buffer;
        buffer << *foldTree /*<< std::endl*/;
        std::cout << "Generated\n[[" << *foldTree << "]]" << std::endl;
        std::cout << "Expected \n[[" << expected << "]]" << std::endl;
        TS_ASSERT_EQUALS(buffer.str(), expected)

    }

    void test_test_in_pdb(){
        core::pose::Pose pose = create_test_in_pdb_pose();
        core::kinematics::FoldTreeOP foldTree = protocols::bootcamp::fold_tree_from_ss(pose);
        TS_ASSERT(foldTree->check_fold_tree())
    }
};
