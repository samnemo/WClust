// $Id: svd.hpp,v 1.1 2008/05/06 02:27:09 samn Exp $ 
#include <vector>

typedef int u16;
typedef ::std::vector< ::std::vector<double> > Matrix;
typedef ::std::vector<double> Vector;

#include "Lapack.hh"

const double svdThreshold=1.0e-12;
const long SMLSIZ=100;

void svd(Matrix &u_, Vector &w_,Matrix& v_);
