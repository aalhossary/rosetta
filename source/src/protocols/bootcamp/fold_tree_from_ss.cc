// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/bootcamp/fold_tree_from_ss.cc
/// @brief A utility function to get fold tree from pose or secondary structure
/// @author Amr ALHOSSARY (aalhossary@wesleyan.edu)

#include <iostream>

#include <core/kinematics/FoldTree.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/dssp/Dssp.hh>

namespace protocols {
namespace bootcamp  {

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

    core::Size calcCentroid(core::Size first, core::Size second) {
        return core::Size(double (second + first)/2.0);
    }

    void addEdgeIfNotPresent(core::kinematics::FoldTree &ft, core::Size start, core::Size stop, int label,
                                    std::set<std::pair<core::Size, core::Size>> &cashedEdges) {
        std::pair<core::Size, core::Size> test{start, stop};
        if(cashedEdges.find(test) == cashedEdges.end()){// if not alredy added
            ft.add_edge(start, stop, label);
            cashedEdges.insert(test);
        }
    }

    core::kinematics::FoldTreeOP fold_tree_from_ss_string(std::string secStruct){
        core::kinematics::FoldTreeOP retFoldTree (new core::kinematics::FoldTree());
        core::kinematics::FoldTree & ft = *retFoldTree;
        std::set<core::Size> nodeIds;//used to store already used nodeIds
        std::set<std::pair<core::Size , core::Size>> edges;
        auto ssSegments = identify_secondary_structure_spans(secStruct);

        if (ssSegments.empty()){
            return retFoldTree;
        }

        core::Size firstSsCentroid = calcCentroid(ssSegments[1].first, ssSegments[1].second);

        core::Size currentlow = 1, currentHigh;
        core::Size currentJumpId = 1;
        for (core::Size segId = 1; segId <= ssSegments.size(); ++segId) {
            //in each iteration, we make a SS segment + next loop (if any)
            auto currSegment = ssSegments[segId];
            if (segId != 1) {
                currentlow = currSegment.first;
            }
            core::Size segmentCentroid = calcCentroid(currSegment.first, currSegment.second);
            currentHigh = (segId == ssSegments.size()) ? secStruct.length() : currSegment.second;

            //add SS Segments edges
            if(firstSsCentroid != segmentCentroid)
                addEdgeIfNotPresent(ft, firstSsCentroid, segmentCentroid, currentJumpId++, edges);
            addEdgeIfNotPresent(ft, segmentCentroid, currentlow, core::kinematics::Edge::PEPTIDE, edges);
            addEdgeIfNotPresent(ft, segmentCentroid, currentHigh, core::kinematics::Edge::PEPTIDE, edges);

            if(segId == ssSegments.size())
                break;

            // find next loop params
            currentlow = currentHigh + 1;
            currentHigh = ssSegments[segId + 1].first -1;
            core::Size loopCentroid = calcCentroid(currentlow, currentHigh);

            //add SS Segments edges
            addEdgeIfNotPresent(ft, firstSsCentroid, loopCentroid, currentJumpId++, edges);
            addEdgeIfNotPresent(ft, loopCentroid, currentlow, core::kinematics::Edge::PEPTIDE, edges);
            addEdgeIfNotPresent(ft, loopCentroid, currentHigh, core::kinematics::Edge::PEPTIDE, edges);
        }
        return core::kinematics::FoldTreeOP(retFoldTree);
    }

    core::kinematics::FoldTreeOP fold_tree_from_ss(core::pose::Pose& pose){
        core::scoring::dssp::Dssp dssp(pose);
        std::string secStruct = dssp.get_dssp_secstruct();
        return fold_tree_from_ss_string(secStruct);
    }

}
}
