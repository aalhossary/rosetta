// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file
/// @brief
/// @author John Karanicolas


#ifndef INCLUDED_basic_MetricValue_hh
#define INCLUDED_basic_MetricValue_hh

#include <basic/MetricValue.fwd.hh>

#include <utility/VirtualBase.hh>
#include <basic/Tracer.hh>
#include <utility/exit.hh>

#include <utility/pointer/owning_ptr.hh>
#include <iosfwd>
#include <string>
#include <sstream>

namespace basic {


class MetricValueBase : public utility::VirtualBase {
public:
	// note: MetricValueBase must have a virtual function so that it will be polymorphic
	// (specifically to allow dynamic casting)
	~MetricValueBase() override {};
	virtual MetricValueBaseOP clone() const = 0;
};


template <class T>
class MetricValue : public MetricValueBase {
public:
	MetricValue() {};
	MetricValue( MetricValue const & metric_value ) : MetricValueBase(), data_(metric_value.value()) {};
	MetricValue( T const & inp ) : data_(inp) {};
	void set( T const & inp ) { data_ = inp; };
	std::string print() const { std::ostringstream ostream; ostream << data_; return ostream.str(); };
	T const & value() const { return data_; };
	MetricValueBaseOP clone() const override { return utility::pointer::make_shared< MetricValue >(*this); }
private:
	T data_;
};


template <class desired_T>
void check_cast( const basic::MetricValueBase * const valptr, const desired_T * const, std::string const & error_msg ) {
	if ( dynamic_cast<const basic::MetricValue<desired_T> * const>(valptr) == nullptr ) {
		basic::Error() << error_msg << std::endl;
		utility_exit();
	}
}


template <class desired_T>
void check_cast( const basic::MetricValueBase * const valptr, std::string const & error_msg ) {
	if ( dynamic_cast<const basic::MetricValue<desired_T> * const>(valptr) == 0 ) {
		basic::Error() << error_msg << std::endl;
		utility_exit();
	}
}


template <class desired_T>
bool check_cast( const basic::MetricValueBase * const valptr, const desired_T * const ) {
	return dynamic_cast<const basic::MetricValue<desired_T> * const>(valptr);
}


template <class desired_T>
bool check_cast( const basic::MetricValueBase * const valptr ) {
	return dynamic_cast<const basic::MetricValue<desired_T> * const>(valptr);
}


} // namespace basic


#endif
