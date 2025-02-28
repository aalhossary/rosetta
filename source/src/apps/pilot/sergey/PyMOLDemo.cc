// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
///
/// @brief
/// @author Sergey Lyskov

#include <basic/Tracer.hh>
#include <protocols/moves/PyMOLMover.hh>
#include <core/pose/Pose.hh>
//#include <core/pose/PDBInfo.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <devel/init.hh>


//#include <unistd.h>


#include <utility/excn/Exceptions.hh>

#include <core/kinematics/MoveMap.hh> // AUTO IWYU For MoveMap


static basic::Tracer TR( "PyMOLDemo" );


#include <unistd.h>

//Auto Headers
#include <core/import_pose/import_pose.hh>


int main( int argc, char * argv [] )
{

	try {

		using namespace core;

		devel::init(argc, argv);


		core::pose::Pose pose;
		core::import_pose::pose_from_file(pose,"src/python/bindings/test/data/1a6t.pdb", core::import_pose::PDB_file);
		//core::import_pose::pose_from_file(pose,"/Users/xlong3/lab_work_data/raw_files/antibody_file/1a6t/1a6t.pdb",core::import_pose::PDB_file);


		protocols::moves::AddPyMOLObserver( pose );
		/*
		for(int j=0; j<32; j++) {
		pose.set_phi(70, pose.phi(70) + 1. );
		usleep(500000);
		}
		*/
		core::scoring::ScoreFunctionOP scorefxn = core::scoring::get_score_function();
		scorefxn->score(pose);
		//pose.energies().residue_total_energies(1);
		//T("Scoring done!") << "---------------------" << std::endl;

		protocols::moves::PyMOLMover pymol;

		//pymol.keep_history(true);
		//pymol.update_interval(.1);
		//dat << std::right << std::setw(size);

		core::Real a = 0.;

		pymol.print("Hi PyMOL!\n");

		for ( int j=0; j<1; j++ ) {
			pose.set_phi(50, a);
			a += 2.;

			std::ostringstream msg;
			msg << "Tottal energy=" << scorefxn->score(pose);
			pymol.print(msg.str());

			//TR << "Sending pose..." << std::endl;
			pymol.apply(pose);
			//TR << "Sending energies..." << std::endl;
			pymol.send_energy(pose);

			usleep(500000);
			//TR << a << std::endl ;
		}

		/*
		for(int j=0; j<2; j++) {
		for(unsigned int r=1; r<=10; r++) {
		//constuct a map from residue numbers to a color index which code what color
		std::map<int, int> C = utility::tools::make_map(int(r), int(protocols::moves::XC_white), int(1+pose.size() - r), int(protocols::moves::XC_red) );
		pymol.send_colors(pose, C );
		usleep(500000);

		}
		}*/

		pymol.label_energy(pose,"total_score");
		std::cout<<"finished the pymol.label energy\n";
		pymol.send_hbonds(pose);
		std::cout<<"finished the pymol.send_hbonds\n";
		pymol.send_ss(pose, "");
		std::cout<<"finished the send_ss\n";
		pymol.send_polars(pose);
		std::cout<<"finished the send_polars\n";
		//second argument is movemap
		core::kinematics::MoveMap movemap;
		std::cout<<"finished instantiating a movemap\n";
		movemap.set_bb(true);
		movemap.set_chi(false);
		pymol.send_movemap(pose, movemap);
		std::cout<<"finished send_movemap function\n";

		//second argument is a foldtree
		core::kinematics::FoldTree fold_tree;
		//std::cout<<" finished instantiating a fold tree"<<std::endl;
		fold_tree = pose.fold_tree();
		//std::cout<<"finished assign the pose fold tree to the instantiated one"<<std::endl;
		pymol.send_foldtree(pose, fold_tree);
		std::cout<<"finished send_foldtree\n";

	} catch (utility::excn::Exception const & e ) {
		e.display();
		return -1;
	}

	//~pymol;
}

/*

int _main( int argc, char * argv [] )
{
using namespace core;

devel::init(argc, argv);

protocols::moves::UDPSocketClient s("127.0.0.1", 65000);

std::string msg;
for(int i=0; i<65536; i++) msg.push_back('_');

msg = msg + "Qwe...";

std::ostringstream ostringstream_;
zlib_stream::zip_ostream zipper(ostringstream_, true);
zipper << msg;
//zipper.zflush();
zipper.zflush_finalize();

s.sendMessage(ostringstream_.str());
// ___Qwe...') 65542



sockaddr_in addr;
memset(&addr, '\0', sizeof(sockaddr_in));


addr.sin_family = AF_INET;       // host byte order
addr.sin_port = htons(65000);     // short, network byte order
//addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_addr.s_addr = inet_addr("127.0.0.1");

int s_id = socket(AF_INET, SOCK_DGRAM, 0);

char * buffer = "Some message here...\0";
int error;// = sendto(s_id, buffer, strlen(buffer),0 , (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

if( error == -1 ) {
printf("Cannot connect... Exiting.\n");
return(1);
}
return 0;
} */
