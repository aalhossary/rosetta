// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

#include "core/pose/variant_util.hh"
#include <iostream>

#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <core/conformation/Residue.hh>
#include <core/import_pose/import_pose.hh>
#include <core/kinematics/MoveMap.hh>
#include <core/optimization/MinimizerOptions.hh>
#include <core/optimization/AtomTreeMinimizer.hh>
#include <core/pack/pack_rotamers.hh>
#include <core/pack/task/PackerTask.hh>
#include <core/pack/task/TaskFactory.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <devel/init.hh>
#include <numeric/random/random.hh>
#include <protocols/moves/MonteCarlo.hh>
#include <utility/pointer/owning_ptr.hh>
#include <core/conformation/Conformation.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>


static constexpr const int NUM_ITEERATIONS = 1000;

int main(int argc, char **argv) {
    std::cout << "Hello World!" << std::endl;

    devel::init(argc, argv);
    utility::vector1<std::string> filenames = basic::options::option[basic::options::OptionKeys::in::file::s].value();
    if (filenames.size() > 0) {
        std::cout << "You entered: " << filenames[1] << " as the PDB file to be read" << std::endl;
    } else {
        std::cout << "You didn’t provide a PDB file with the -in::file::s option" << std::endl;
        return 1;
    }
    core::pose::PoseOP mypose = core::import_pose::pose_from_file(filenames[1]);
    core::scoring::ScoreFunctionOP sfxn = core::scoring::get_score_function();
    core::Size N = mypose->size();
    core::Real score = sfxn->score(*mypose);
    std::cout << "score =" << score << std::endl;

    auto conf = mypose->conformation();
    conf.fold_tree(*protocols::bootcamp::fold_tree_from_ss(*mypose));
    core::pose::correctly_add_cutpoint_variants(*mypose);
    sfxn->set_weight(core::scoring::ScoreType::linear_chainbreak, 1.0);

    protocols::moves::MonteCarlo monteCarlo = protocols::moves::MonteCarlo(*mypose, *sfxn, 1.);
    core::kinematics::MoveMap mm;
    mm.set_bb( true );
    mm.set_chi( true );
    core::optimization::AtomTreeMinimizer atm;
    core::optimization::MinimizerOptions min_opts( "lbfgs_armijo_atol", 0.01, true );

    core::pose::Pose copy_pose;
    core::Size acceptanceCounter = 0, hundredIterationsAcceptanceCounter = 0;
    for (int step = 0; step < NUM_ITEERATIONS; ++step) {
        core::Size randres = static_cast< core::Size > ( numeric::random::uniform() * N + 1 );
        core::Real pert1 = numeric::random::gaussian();
        core::Real pert2 = numeric::random::gaussian();
        core::Real orig_phi = mypose->phi( randres );
        core::Real orig_psi = mypose->psi( randres );

        //perturbation
        mypose->set_phi( randres, orig_phi + pert1 );
        mypose->set_psi( randres, orig_psi + pert2 );

        //packing
        core::pack::task::PackerTaskOP repack_task = core::pack::task::TaskFactory::create_packer_task( *mypose );
        repack_task->restrict_to_repacking();
        core::pack::pack_rotamers( *mypose, *sfxn, repack_task );

        //minimization
//        atm.run( *mypose, mm, *sfxn, min_opts );
        copy_pose = *mypose;
        atm.run( copy_pose, mm, *sfxn, min_opts );
        *mypose = copy_pose;

        bool accepted = monteCarlo.boltzmann(*mypose);
        if(accepted){
            acceptanceCounter++;
            hundredIterationsAcceptanceCounter++;
            std::cout << step <<"\t new score " << sfxn->score(*mypose) << std::endl;
        }

        if (step && !(step%100)){
            core::Real localAcceptanceRate = static_cast<core::Real>(hundredIterationsAcceptanceCounter) / 100.;
            std::cout << step <<"\t Current Acceptance Rate \t" << localAcceptanceRate << "%" << std::endl;
            hundredIterationsAcceptanceCounter = 0;
        }

    }
    core::Real acceptanceRate = static_cast<core::Real>(acceptanceCounter) * 100. / NUM_ITEERATIONS;
    std::cout << "Overall Acceptance Rate \t" << acceptanceRate << "%" << std::endl;


    return 0;

} 


