#include "stdafx.h"
#include "DataSet.h"

namespace damina
{

	/**
	 * 	Constructor
	 * */
	DataSet::DataSet() {
		
	}
	
	/**
	 * 	Destructor
	 * 
	 * 	We need to destroy all Points in dataset before destroy dataset itself
	 * */
	DataSet::~DataSet() {
		vector<Point *>::iterator dataIterator;
		
		for (dataIterator == data.begin(); dataIterator < data.end(); dataIterator++) {
			delete *dataIterator;
		}
	}
	
	/**
	 * 	Add a Point to the dataset
	 * 
	 * 	@param	p	The point to add
	 * */
	void DataSet::add(Point *p) {
		data.push_back(p);
	}
	
	
	/**
	 * 	Returns the first point in dataset
	 * 
	 * 	@return The first point in dataset
	 * */
	Point *DataSet::first() {
		it = data.begin();
		return *it;
	}
	
	/**
	 * 	Returns the last point in dataset
	 * 
	 * 	@return The last point in dataset
	 * */
	Point *DataSet::last() {
		it = data.end();
		return *it;
	}
	
	/**
	 * 	Increments the Dataset iterator and return the corresponding point
	 * 
	 * 	@return	The reference to the next point in the dataset
	 * */
	Point *DataSet::next() {
		it++;
		return *it; 
	}
	
	/**
	 * 	Decrementes the Dataset iterator and return the corresponding point
	 * 
	 * 	@return	The reference to the previous point in the dataset
	 * */
	Point *DataSet::previous() {
		it--;
		return *it;
	}
	
	
	/**
	 * 	Returns the Dataset Size
	 * 
	 * 	@return	An integer value corresponding to the size
	 * */
	unsigned long int DataSet::getSize() {
		return data.size();
	}
	
	/**
	 * 	Returns	the specified point in the dataset
	 * 
	 * 	@param	index	The position occupied in the dataset of the point to get
	 * 
	 * 	@return	 The point in the "index" position of the dataset
	 * */
	Point *DataSet::get(int index) {
		return data[index];
	}

}
