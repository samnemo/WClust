// $Id: pca.h,v 1.3 2008/05/06 02:27:33 samn Exp $ 
/*********************************/
/* Principal Components Analysis */
/*********************************/

#include <vector>
#include "A2D.h"

using namespace std;

typedef float PCA_T;

//typedef vector< vector<PCA_T> > d2d;

/**  Correlation matrix: creation  ***********************************/
void corcol(vector< vector<PCA_T> >& data,int n,int m,vector< vector<PCA_T> >& symmat, vector<PCA_T>& mean, vector<PCA_T>& stddev, PCA_T& SqrtN);
/**  Correlation matrix: creation  ***********************************/
void corcol(A2D<PCA_T>& data,int n,int m,vector< vector<PCA_T> >& symmat, vector<PCA_T>& mean, vector<PCA_T>& stddev, PCA_T& SqrtN);
/**  Variance-covariance matrix: creation  *****************************/
void covcol(vector< vector<PCA_T> >&  data,int n,int m,vector< vector<PCA_T> >& symmat);
/**  Variance-covariance matrix: creation  *****************************/
void covcol(A2D<PCA_T>& data,int n,int m,vector< vector<PCA_T> >& symmat,vector<PCA_T>& mean);
/**  Sums-of-squares-and-cross-products matrix: creation  **************/
void scpcol(vector< vector<PCA_T> >& data,int n,int m,vector< vector<PCA_T> >& symmat);
/**  Error handler  **************************************************/
void erhand(char* err_msg);
/**  Allocation of vector storage  ***********************************/
PCA_T *p_vector(int n);
/**  Allocation of PCA_T matrix storage  *****************************/
PCA_T **p_matrix(int n,int m);
/**  Deallocate vector storage  *********************************/
void free_p_vector(PCA_T* v,int n);
/**  Deallocate PCA_T matrix storage  ***************************/
void free_p_matrix(PCA_T** mat,int n,int m);
/**  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */
void tred2(vector< vector<PCA_T> >& a,int n,vector<PCA_T>& d,vector<PCA_T>& e);
/**  Tridiagonal QL algorithm -- Implicit  **********************/
void tqli(vector<PCA_T>& d,vector<PCA_T>& e,int n,vector< vector<PCA_T> >& z);

