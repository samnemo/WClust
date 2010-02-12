#ifndef POINT_H_
#define POINT_H_

#include "Feature.h"
#include <vector>

namespace damina
{
	
	using namespace std;

	/**
	 * 	Encapsulates a Point for feature vector representaion of data.
	 * 
	 *	We use the Vector Document Model, in which each object is represented
	 * 	by a point in a vector space. So, a point is a features vector.
	 *  
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class Point
	{
		private: 
			/**
			 * 	The point representation
			 * */
			vector<Feature *> point;
			
			/**
			 * 	An iterator over the point
			 * */
			vector<Feature *>::iterator it;
			
			/**
			 * 	Label indicates the class the point belong to
			 * */
			double label;
			
		public:
			
			virtual void add(Feature *);
			virtual void setLabel(double);
			virtual double getLabel();
			virtual Feature *first();
			virtual Feature *last();
			virtual Feature *next();
			virtual Feature *previous();
			
			
			virtual Feature *get(int);
			virtual unsigned long int getSize();
			
			Point();
			Point(double);
			virtual ~Point();
	};

}

#endif /*POINT_H_*/
