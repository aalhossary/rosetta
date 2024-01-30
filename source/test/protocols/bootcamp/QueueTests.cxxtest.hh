// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  protocols/bootcamp/QueueTests.cxxtest.hh
/// @brief  Testing the Queue class for the bootcamp
/// @author Amr ALHOSSARY (aalhossary@wesleyan.edu)


// Test headers
#include <test/UMoverTest.hh>
#include <test/UTracer.hh>
#include <cxxtest/TestSuite.h>
#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

// Project Headers


// Core Headers
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>

// Utility, etc Headers
#include <basic/Tracer.hh>

#include <protocols/bootcamp/Queue.hh>

static basic::Tracer TR("QueueTests");


class QueueTests : public CxxTest::TestSuite {
	//Define Variables
    protocols::bootcamp::QueueOP queue1;
    protocols::bootcamp::QueueOP queue2;

public:

	void setUp() {
		core_init();
        queue1 = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue());
        queue2 = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue());
	}

	void tearDown() {

	}



	void test_first() {
        TS_TRACE( "Running my first unit test!" );
        TS_ASSERT( true );
	}

    void test_empty(){
        TS_ASSERT(queue1->is_empty())
        queue1->enqueue("1");
        TS_ASSERT(! queue1->is_empty())
        queue1->enqueue("2");
        TS_ASSERT(! queue1->is_empty())
        queue1->dequeue();
        queue1->dequeue();
        TS_ASSERT(queue1->is_empty())
    }

    void test_size(){
        TS_ASSERT_EQUALS(queue1->size(), 0)
        queue1->enqueue("A");
        TS_ASSERT_EQUALS(queue1->size(), 1)
        queue1->enqueue("B");
        TS_ASSERT_EQUALS(queue1->size(), 2)
        queue1->dequeue();
        TS_ASSERT_EQUALS(queue1->size(), 1)
        queue1->dequeue();
        TS_ASSERT_EQUALS(queue1->size(), 0)
    }

    void test_canNotDequeEmpty(){
        assert(queue1->is_empty());
        TS_ASSERT_THROWS_ANYTHING(queue1->dequeue())
    }

    void test_FIFO(){
        queue1->enqueue("1");
        queue1->enqueue("2");
        queue1->enqueue("3");
        TS_ASSERT_EQUALS(queue1->dequeue(), "1")
        TS_ASSERT_EQUALS(queue1->dequeue(), "2")
        TS_ASSERT_EQUALS(queue1->dequeue(), "3")
    }

};
