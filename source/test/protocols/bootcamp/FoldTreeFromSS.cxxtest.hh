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

// Test headers
#include <cxxtest/TestSuite.h>

#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

// Utility headers
#include <utility/vector1.hh>

/// Project headers
#include <core/types.hh>

#include "../../../src/core/kinematics/FoldTree.hh"
#include "../../../src/core/scoring/dssp/Dssp.hh"
#include "../../../src/core/pose/Pose.hh"
#include "../../../src/core/types.hh"
#include "../../../src/utility/vector1.hh"
#include "../../../external/cxxtest/cxxtest/TestSuite.h"

// C++ headers

//Auto Headers


// --------------- Test Class --------------- //

class FoldTreeFromSSTests : public CxxTest::TestSuite {

public:

    utility::vector1< std::pair< core::Size, core::Size > >
    identify_secondary_structure_spans( std::string const & ss_string )
    {
        utility::vector1< std::pair< core::Size, core::Size > > ss_boundaries;
        core::Size strand_start = -1;
        for ( core::Size ii = 0; ii < ss_string.size(); ++ii ) {
            if ( ss_string[ ii ] == 'E' || ss_string[ ii ] == 'H'  ) {
                if ( int( strand_start ) == -1 ) {
                    strand_start = ii;
                } else if ( ss_string[ii] != ss_string[strand_start] ) {
                    ss_boundaries.push_back( std::make_pair( strand_start+1, ii ) );
                    strand_start = ii;
                }
            } else {
                if ( int( strand_start ) != -1 ) {
                    ss_boundaries.push_back( std::make_pair( strand_start+1, ii ) );
                    strand_start = -1;
                }
            }
        }
        if ( int( strand_start ) != -1 ) {
            // last residue was part of a ss-eleemnt
            ss_boundaries.push_back( std::make_pair( strand_start+1, ss_string.size() ));
        }
        for ( core::Size ii = 1; ii <= ss_boundaries.size(); ++ii ) {
            std::cout << "SS Element " << ii << " from residue "
                      << ss_boundaries[ ii ].first << " to "
                      << ss_boundaries[ ii ].second << std::endl;
        }
        return ss_boundaries;
    }

    // Shared initialization goes here.
	void setUp() {
		core_init();
	}

	// Shared finalization goes here.
	void tearDown() {
	}


	// --------------- Test Cases --------------- //
    void test_hello_world(){
        TS_ASSERT(true);
    }

    void test_sample1(){
        std::string sample = "   EEEEE   HHHHHHHH  EEEEE   IGNOR EEEEEE   HHHHHHHHHHH  EEEEE  HHHH   ";
        auto result = identify_secondary_structure_spans(sample);
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

    core::kinematics::FoldTreeOP fold_tree_from_ss_string(std::string secStruct){

        //TODO complete
        return core::kinematics::FoldTreeOP(nullptr);
    }

    core::kinematics::FoldTreeOP fold_tree_from_ss(core::pose::Pose pose){
        core::scoring::dssp::Dssp dssp(pose);
        std::string secStruct = dssp.get_dssp_secstruct();
        return fold_tree_from_ss_string(secStruct);
    }

};
