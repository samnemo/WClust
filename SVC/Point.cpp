#include "stdafx.h"
#include "Point.h"

namespace damina
{

	/**
	 * 	Constructor
	 * */
	Point::Point() {
	}
	
	
	/**
	 * 	Destructor. 	
	 * 	Before destroy a point we have to destroy its features first.
	 * */
	Point::~Point() {
		vector<Feature *>::iterator dataIterator;
		
		/* iterating over the features to destory them	*/
		for (dataIterator == point.begin(); dataIterator < point.end(); dataIterator++) {
			delete *dataIterator;
		}
	}
	
	/**
	 * 	Append a feature to the point
	 * 
	 *	@param	f	The feature to add
	 * */
	void Point::add(Feature *f) {
		point.push_back(f);
	}
	
	
	/**
	 * 	Set the label (class) for the point
	 * 	
	 * 	@param	l	The label to set
	 * */
	void Point::setLabel(double l) {
		label = l;
	}
	
	/**
	 * 	Returns the label currently assigned to the point
	 * 
	 * 	@return	A double precision floating point value representing the label
	 * */
	double Point::getLabel() {
		return label;
	}
	
	
	/**
	 * 	Returns the first feature of the point
	 * 
	 * 	@return	The reference to the first feature of the point
	 * */
	Feature *Point::first() {
		it = point.begin();
		return *it;
	}
	
	/**
	 * 	Returns the last feature of the point
	 * 
	 * 	@return	The reference to the last feature of the point
	 * */
	Feature *Point::last() {
		it = point.end();
		return *it;
	}
	
	/**
	 * 	Increments the point iterator and return the corresponding feature
	 * 
	 * 	@return	The reference to the next feature of the point
	 * */
	Feature *Point::next() {
		it++;
		return *it;
	}
	
	/**
	 * 	Decrements the point iterator and return the corresponding feature
	 * 
	 * 	@return	The reference to the previous feature of the point
	 * */
	Feature *Point::previous() {
		it--;
		return *it;
	}
	
	
	/**
	 * 	Returns the number of the elements in the point
	 * 
	 * 	@return	The number of features representing the point
	 * */
	unsigned long int Point::getSize() {
		return this->point.size();
	}
	
	/**
	 * 	Returns	the specified feature of the point
	 * 
	 * 	@param	index	The position occupied in the point of the feature to get
	 * 
	 * 	@return	 The feature in the "index" position of the point
	 * */
	Feature *Point::get(int index) {
		return this->point[index];
	} 
}
