#include "stdafx.h"
#include "Feature.h"

namespace damina
{

	/**
	 * 	Constructor
	 * 	
	 * 	@param	index The index in the original vector representing the data point
	 * 	@param	value The value of the feature
	 * */
	Feature::Feature(int index, double value) {
		this->index = index;
		this->value = value;
	}
	
	/**
	 * 	Default destructor
	 * */
	Feature::~Feature() {
		
	}
	
	
	/**
	 * 	Set the feature
	 * 
	 * 	@param	index The index in the original vector representing the data point
	 * 	@param	value The value of the feature
	 * */
	void Feature::set(int index, double value) {
		this->index = index;
		this->value = value;
	}
	
	
	/**
	 *	Return the index in the original vector represeting the data point
	 * 
	 * 	@return	An integer value representing the index in the original vector represeting the data point
	 * */
	int Feature::getIndex() {
		return index;
	}
	
	/**
	 * 	Return the value of the feature
	 * 
	 * 	@return The feature's value
	 * */
	double Feature::getValue() {
		return value;
	}
}
