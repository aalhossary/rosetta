// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/scoring/disulfides/CentroidDisulfidePotential.hh
/// @brief  Centroid Disulfide Energy Potentials
/// @author Spencer Bliven <blivens@u.washington.edu>
/// @date   12/17/08

#ifndef INCLUDED_core_scoring_disulfides_CentroidDisulfidePotential_hh
#define INCLUDED_core_scoring_disulfides_CentroidDisulfidePotential_hh

//Unit headers
#include <core/scoring/disulfides/CentroidDisulfidePotential.fwd.hh>
#include <utility/VirtualBase.hh>

//Project headers
#include <core/types.hh>
#include <core/conformation/Residue.fwd.hh>
#include <core/scoring/func/Func.hh>

//Utility Headers

#include <numeric/interpolation/Histogram.fwd.hh>


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION


namespace core {
namespace scoring {
namespace disulfides {

/**
* @details This class scores centroid disulfide bonds
* It is intended to be a singleton with a single instance held by ScoringManager.
*
* The energy functions are derived from those present in Rosetta++
*/
class CentroidDisulfidePotential : public utility::VirtualBase {
public:
	CentroidDisulfidePotential();
	~CentroidDisulfidePotential() override;

	/**
	* @brief Calculates scoring terms for the disulfide bond specified
	*/
	void
	score_disulfide(
		core::conformation::Residue const & res1,
		core::conformation::Residue const & res2,
		core::Energy & cbcb_distance_score,
		core::Energy & centroid_distance_score,
		core::Energy & cacbcb_angle_1_score,
		core::Energy & cacbcb_angle_2_score,
		core::Energy & cacbcbca_dihedral_score,
		core::Energy & backbone_dihedral_score
	) const;

	/**
	* @brief Calculates scoring terms and geometry
	*/
	void score_disulfide(
		core::conformation::Residue const & res1,
		core::conformation::Residue const & res2,
		core::Real & cbcb_distance_sq,
		core::Real & centroid_distance_sq,
		core::Real & cacbcb_angle_1,
		core::Real & cacbcb_angle_2,
		core::Real & cacbcbca_dihedral,
		core::Real & backbone_dihedral,
		core::Energy & cbcb_distance_score,
		core::Energy & centroid_distance_score,
		core::Energy & cacbcb_angle_1_score,
		core::Energy & cacbcb_angle_2_score,
		core::Energy & cacbcbca_dihedral_score,
		core::Energy & backbone_dihedral_score,
		core::Real & cb_score_factor
	) const;

	/**
	* @brief Decide whether there is a disulfide bond between two residues.
	*
	* Does not require that the residues be cysteines, so if this is important
	* you should check for CYS first. (The relaxed requirements are useful for
	* design.)
	*/
	bool is_disulfide(core::conformation::Residue const & res1,
		core::conformation::Residue const & res2) const;


private:

	/**
	* @brief calculates some degrees of freedom between two centroid cys residues
	*
	* @param cbcb_distance_sq      The distance between Cbetas squared
	* @param centroid_distance_sq  The distance between centroids squared
	* @param cacbcb_angle_1        The Ca1-Cb1-Cb2 planar angle, in degrees
	* @param cacbcb_angle_2        The Ca2-Cb2-Cb1 planar angle, in degrees
	* @param cacbcbca_dihedral     The Ca1-Cb1-Cb2-Ca2 dihedral angle
	* @param backbone_dihedral     The N-Ca1-Ca2-C2 dihedral angle
	*/
	static
	void
	disulfide_params(
		core::conformation::Residue const& res1,
		core::conformation::Residue const& res2,
		core::Real & cbcb_distance_sq,
		core::Real & centroid_distance_sq,
		core::Real & cacbcb_angle_1,
		core::Real & cacbcb_angle_2,
		core::Real & cacbcbca_dihedral,
		core::Real & backbone_dihedral);

private: //Scoring functions
	static core::scoring::disulfides::Cb_Distance_FuncCOP cb_distance_func_;
	static core::scoring::disulfides::Cen_Distance_FuncCOP cen_distance_func_;
	static core::scoring::disulfides::CaCbCb_Angle_FuncCOP cacbcb_angle_func_;
	static core::scoring::disulfides::NCaCaC_Dihedral_FuncCOP ncacac_dihedral_func_;
	static core::scoring::disulfides::CaCbCbCa_Dihedral_FuncCOP cacbcbca_dihedral_func_;

	/// @brief the Cysteines with cb dist scores less than this threshold are
	/// very likely (99%) to be disulfide bonded.
	static const core::Real disulfide_cb_dist_cutoff;

}; // CentroidDisulfidePotential


/**
* @brief Score based on the distance between Cb
*
* Based on the rosetta++ scores by Bill Schief in 2002.
*
* Uses a sum of three gaussians.
*/
class Cb_Distance_Func : public core::scoring::func::Func
{
public:
	Cb_Distance_Func();
	~Cb_Distance_Func() override;
	core::scoring::func::FuncOP clone() const override { return utility::pointer::make_shared< core::scoring::disulfides::Cb_Distance_Func >( *this ); }
	bool operator == ( Func const & other ) const override;
	bool same_type_as_me( Func const & other ) const override;
	core::Real func( core::Real const ) const override;
	core::Real dfunc( core::Real const ) const override;
private:
	//Parameters for the 3 gaussians
	static const Real means_[3];
	static const Real sds_[3];
	//Weighting between the 3 gaussians
	static const Real weights_[3];
	//Score at infinity; The gaussians are subtracted from this base.
	static const Real base_score_;

#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

/**
* Score based on the distance between the two residues' centroids
*/
class Cen_Distance_Func : public core::scoring::func::Func
{
public:
	Cen_Distance_Func();
	~Cen_Distance_Func() override;
	core::scoring::func::FuncOP clone() const override { return utility::pointer::make_shared< core::scoring::disulfides::Cen_Distance_Func >( *this ); }
	bool operator == ( Func const & other ) const override;
	bool same_type_as_me( Func const & other ) const override;
	core::Real func( core::Real const ) const override;
	core::Real dfunc( core::Real const ) const override;
private:
	static numeric::interpolation::HistogramCOP<core::Real,core::Real>::Type centroid_dist_scores_;
#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

/**
* Score based on the angle Ca Cb Cb
*/
class CaCbCb_Angle_Func : public core::scoring::func::Func
{
public:
	CaCbCb_Angle_Func();
	~CaCbCb_Angle_Func() override;
	core::scoring::func::FuncOP clone() const  override{ return utility::pointer::make_shared< core::scoring::disulfides::CaCbCb_Angle_Func >( *this ); }
	bool operator == ( Func const & other ) const override;
	bool same_type_as_me( Func const & other ) const override;
	core::Real func( core::Real const ) const override;
	core::Real dfunc( core::Real const ) const override;
private:
	static numeric::interpolation::HistogramCOP<core::Real,core::Real>::Type CaCbCb_angle_scores_;
#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

/**
* Score based on the dihedral formed by the two Ca and Cb
*/
class NCaCaC_Dihedral_Func : public core::scoring::func::Func
{
public:
	NCaCaC_Dihedral_Func();
	~NCaCaC_Dihedral_Func() override;
	core::scoring::func::FuncOP clone() const override { return utility::pointer::make_shared< core::scoring::disulfides::NCaCaC_Dihedral_Func >( *this ); }
	bool operator == ( Func const & other ) const override;
	bool same_type_as_me( Func const & other ) const override;
	core::Real func( core::Real const ) const override;
	core::Real dfunc( core::Real const ) const override;
private:
	static numeric::interpolation::HistogramCOP<core::Real,core::Real>::Type backbone_dihedral_scores_;
#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

/**
* Score based on the dihedral formed by N Ca Ca C
*/
class CaCbCbCa_Dihedral_Func : public core::scoring::func::Func
{
public:
	CaCbCbCa_Dihedral_Func();
	~CaCbCbCa_Dihedral_Func() override;
	core::scoring::func::FuncOP clone() const override { return utility::pointer::make_shared< core::scoring::disulfides::CaCbCbCa_Dihedral_Func >( *this ); }
	bool operator == ( Func const & other ) const override;
	bool same_type_as_me( Func const & other ) const override;
	core::Real func( core::Real const ) const override;
	core::Real dfunc( core::Real const ) const override;
private:
	static numeric::interpolation::HistogramCOP<core::Real,core::Real>::Type CaCbCbCa_dihedral_scores_;
#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

} // disulfides
} // scoring
} // core

#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_scoring_disulfides_CentroidDisulfidePotential )
#endif // SERIALIZATION


#endif
