#include "stdafx.h"
#include "GKWGenerator.h"
#include <cmath>
#include "L2Distance.h"

namespace damina
{

	/**
	 * 	Constructor
	 * 
	 * 
	 * 	@param	svc	The Support Vector Clustering instance which perform the domain description
	 * 	@param	eps	The precision for the secant line slope
	 * */
	GKWGenerator::GKWGenerator(SVClustering *svc, double eps) {
		this->svc = svc;
		this->eucDist = new L2Distance();
		this->runs = 0;
		this->eps = eps;
		this->softening = 1;
		
		this->initialKW = initialKernelWidth();
		
		/**
		 * 	These are because the Lemma 2.2 in the 
		 * 
		 *  <em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Generator for Support Vector Clustering," 
	 	 * 	in Advances in Bioinformatics and Its Applications, 2005, pp. 151-162.</em>
	 	 * 
		 * */
		this->oldRadius = 0;
		this->oldKW = 0;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 
	 * 	@param	svc	The Support Vector Clustering instance which perform the domain description
	 * 	@param	eps	The precision for the secant line slope
	 * 	@param	initialKW	The initial kernel width value
	 * */
	GKWGenerator::GKWGenerator(SVClustering *svc, double eps, double initialKW) {
		this->svc = svc;
		this->eucDist = new L2Distance();
		this->runs = 0;
		this->eps = eps;
		this->softening = 1;
		
		this->initialKW = initialKW;
		
		/**
		 * 	These are because the Lemma 2.2 in the 
		 * 
		 *  <em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Generator for Support Vector Clustering," 
	 	 * 	in Advances in Bioinformatics and Its Applications, 2005, pp. 151-162.</em>
	 	 * 
		 * */
		this->oldRadius = 0;
		this->oldKW = 0;
	}
	
	/**
	 * 	Destructor
	 * */
	GKWGenerator::~GKWGenerator() {
		delete eucDist;	
	}


	/*	PUBLIC METHODS FOLLOW	******************************************************************************/

	/**
	 *	Initialize again the Gaussian Kernel Width Generator
	 * */	
	void GKWGenerator::reset() {
		this->runs = 0;
		
		/**
		 * 	These are because the Lemma 2.2 in the 
		 * 
		 *  <em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Generator for Support Vector Clustering," 
	 	 * 	in Advances in Bioinformatics and Its Applications, 2005, pp. 151-162.</em>
	 	 * 
		 * */
		this->oldRadius = 0;
		this->oldKW = 0;
	}

	/**
	 *	Implements the process of kernel width generation.
	 *
	 *	Implements the generation of the next kernel width value using the theory and algorithm proposed in
     *
	 * 	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Selection and Fast Cluster Labeling for 
	 * 	Support Vector Clustering," Department of Computer Science, University of Massachussets Lowell, 2005. </em>
	 *
	 * 	@return	The next useful kernel width value (-1 if no more valid values can be generated)		
	 * */	
	double GKWGenerator::getNextKernelWidthValue() {
		if (runs == 0) {
			newKW = initialKW;		
			runs++;
			return newKW;
		}
		
		newRadius = svc->getSphereRadius();
	
		double x_diff;
		double radiusDiff = newRadius - oldRadius;
		double kwDiff = newKW - oldKW;
		
		// if the line for the two points (newKW, newRadius) and (oldKW, oldRadius) has a slope close to zero
		// or the number of SV became equal to the number of total points
		// stop the 'q' generation process  
		if (((double)(radiusDiff / kwDiff) < this->eps) || (svc->getProblem()->l == svc->getModel()->l)) {
			return -1;
		}
		

		if ((newRadius >= 1) || (newRadius < 0.5)) {
			if (x_max_1 == NULL) {
				maxDistance();	//	also find the two farthest points each other
			}

			/* applying Corollary 4.1.10 in 
             *
			 * S. Lee and K. M. Daniels, "Gaussian Kernel Width Selection and Fast Cluster Labeling for 
             * Support Vector Clustering," Department of Computer Science, University of Massachussets Lowell, 2005.
             *
			 * */
			x_diff = kernelfunction(x_max_1, x_max_2, svc->getModel()->param);
			newRadius = (1 - x_diff) / 2;
			//oldRadius = 0;
			//oldKW = 0;
		}
		else {
			while (newRadius <= oldRadius) {	
				forceMonotonicity();
			}	
		}

		double q,y;
		
		intersectLines(oldKW, oldRadius, newKW, newRadius, 0, 1 - (1/svc->getProblem()->l), 1, 1 - (1/svc->getProblem()->l), q, y);
		
		oldKW = newKW;
		newKW = q * softening;
		
		oldRadius = newRadius;
		
		runs++;
		
		return newKW; 
	}
	

	/**
	 * 	Set a softening value for the growing of the kernel width value in order to avoid to skip some important values.
	 *
	 *  @param	s	The softening value 
	 * */
	void GKWGenerator::setGrowthSoftening(double s) {
		this->softening = s;
	}
	
	/**
	 * 	Get the softening value for the growing of the kernel width value.
	 *
	 *  @return		The current softening value 
	 * */
	double GKWGenerator::getGrowthSoftening() {
		return softening;
	}


	/**
     * 	Set the Euclidean Distance to use for first kernel width calculus
	 *  
	 *	@param	ed	The euclidean distance
	 * */
	void GKWGenerator::setEuclideanDistance(EuclideanDistance *ed) {
		eucDist = ed;
	}
	
	/*	PRIVATE METHODS FOLLOW	******************************************************************************/

	/**
	 * 	Enforce the monotonicity in the process of increasing kernel width values generation
	 *  
	 * */
	void GKWGenerator::forceMonotonicity() {
		newKW += eps / eps;
		svc->setKernelWidth(newKW);
		svc->learn();
		newRadius = svc->getSphereRadius();
	}
	
	/**
	 *	Basic implementation of the geometry for intersecting two lines.
	 *
	 *	@param	x0, y0	The first point of the first line
	 *	@param	x1, y1	The second point of the first line
	 *	@param	x2, y2	The first point of the second line
	 *	@param	x3, y3	The second point of the second line
	 *	@param	xi, yi	The incidence point resulting from intersection
	 * */	
	void GKWGenerator::intersectLines(double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3, double &xi,double &yi) {
                     		
		// this function computes the intersection of the sent lines
		// and returns the intersection point, note that the function assumes
		// the lines intersect. the function can handle vertical as well
		// as horizontal lines. note the function isn't very clever, it simply
		// applies the math
		
		double a1,b1,c1, // constants of linear equations
		      a2,b2,c2,
		      det_inv,  // the inverse of the determinant of the coefficient matrix
		      m1,m2;    // the slopes of each line
		
		// compute slopes, note the cludge for infinity, however, this will
		// be close enough
		
		if ((x1-x0)!=0)
		   m1 = (y1-y0)/(x1-x0);
		else
		   m1 = (double)1e+10;   // close enough to infinity
		
		if ((x3-x2)!=0)
		   m2 = (y3-y2)/(x3-x2);
		else
		   m2 = (double)1e+10;   // close enough to infinity
		
		// compute constants
		
		a1 = m1;
		a2 = m2;
		
		b1 = -1;
		b2 = -1;
		
		c1 = (y0-m1*x0);
		c2 = (y2-m2*x2);
		
		// compute the inverse of the determinate
		
		det_inv = 1/(a1*b2 - a2*b1);
		
		// use Kramers rule to compute xi and yi
		
		xi = ((b1*c2 - b2*c1)*det_inv);
		yi = ((a2*c1 - a1*c2)*det_inv);
	
	} // end 


	/**
	 *	Calculate the distance between the two farthest points each other.
	 *
	 *	Calculate the distance between the two farthest points each other and store those points for future utilizations.
	 *
	 * 	@return The max distance among all pair of points
	 * */
	double GKWGenerator::maxDistance() {
		struct svm_problem *p = svc->getProblem();
		int N = p->l;
		
		double max = eucDist->calculateDistance(p->x[0], p->x[1]);
		double dist;
		
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				dist = eucDist->calculateDistance(p->x[i], p->x[j]);
				if (dist > max) {
					max = dist;
					x_max_1 = p->x[i];
					x_max_2 = p->x[j];
				}
			}
		}

		/*double max = 2 - 2 * kernelfunction(p->x[0], p->x[1], *(svc->getParameters()));
		double dist;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				dist = 2 - 2 * kernelfunction(p->x[i], p->x[j], *(svc->getParameters()));
				if (dist > max) {
					max = dist;
					x_max_1 = p->x[i];
					x_max_2 = p->x[j];
				}
			}
		}*/
		
		return max;
	}

	/**
	 * 	Computes the initial gaussian kernel width, as proposed in
	 * 	
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, 
	 * 	"Support Vector Clustering," Journal of Machine 
	 * 	Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 * 	At this value, the soft margin constraint (C) have to be 1 (no outliers needed)
	 * 	and it is expected to yield a clustering result of one cluster.
	 * 
	 *  @return The initial value for the gaussian kernel width
	 * */
	double GKWGenerator::initialKernelWidth() {
		double initial = 1 / maxDistance();			
		svc->setKernelWidth(initial);
		return (initial);
	}

}
