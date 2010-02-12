#ifndef DATASET_H_
#define DATASET_H_


#include <vector>
#include "Point.h"


using namespace std;


namespace damina
{

	/**
	 * 	A class which is useful for manage both Training Sets and Test Sets.
	 * 
	 * 	Both Training Set and Test Set are data sets formed by points and them relative labels.
	 * 	So, this class is just implemented as a vector of Point, because each Point embeds its own
	 * 	label.
	 * 
	 * 	@see Point
	 * */
	class DataSet
	{
		private: 
			/**
			 * 	The data
			 * */			
			vector<Point *> data;
			
			/**
			 * 	The iterator over the data
			 * */
			vector<Point *>::iterator it;
		
		public:
			virtual void add(Point *);
			virtual Point *first();
			virtual Point *last();
			virtual Point *next();
			virtual Point *previous();
			
			virtual Point *get(int);
			virtual unsigned long int getSize();
		
			DataSet();
			virtual ~DataSet();
		};

}

#endif /*DATASET_H_*/
