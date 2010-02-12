#ifndef FEATURE_H_
#define FEATURE_H_



namespace damina
{
	
	/**
	 * 	Encapsulates a Feature for feature vector representaion of data.
	 * 
	 * 	Due the sparse matrix representation of input data matrix,
	 * 	the class Feature was implemented in order to incapsulate the index
	 * 	in the original vector and value of the feature. 
	 * 
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class Feature
	{
		private:
			/**
			 * 	Index in the original vector representing a data point
			 * */
			int index;
			
			/**
			 * 	The value of the Feature
			 * */
			double value;
		
		public:
			virtual void set(int, double);
			virtual int getIndex();
			virtual double getValue();
			
			Feature(int, double);
			virtual ~Feature();
	};

}

#endif /*FEATURE_H_*/
