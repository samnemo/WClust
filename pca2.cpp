// $Id: pca2.cpp,v 1.4 2008/08/11 20:54:36 samn Exp $ 
#include "stdafx.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include "diag.hpp"
#include "pca2.hpp"
//#include "gzstream.hpp"
//#include "svd.hpp"
#include "nr\nr.h"
#include "WCMath.h"
#include "CDNAClust\cluster.h"
#include "Log.h"
using namespace std;

PCA::PCA(int dim) {
  init(dim);
}

void PCA::init(int dim)
{
  dim_=dim;
  mean_=vector<double>(dim_,0.0);
  //covariance_.Init(dim_,dim_);
  covariance_=A2D<double>(dim_,dim_);
  covariance_.Fill(0.0);
  counter_=0;
}

void PCA::save(const string &filename) const{
  /*ogzstream ofs; ofs.open(filename.c_str());
  if(!ofs.good()) {
    ERR << "[PCA::save] Cannot write PCA to '" << filename << "'."<< endl;
  } else {
    ofs << dim_ << " " <<dim_ << endl;
    ofs << "mean "  ;
    for(int i=0;i<dim_;++i) {
      ofs << mean_[i] << " "; }
    ofs << endl;

    for(int y=0;y<dim_;++y) {
      ofs << y;
      for(int x=0;x<dim_;++x) {
        ofs << " " << covariance_[y][x];
      }
      ofs << endl;
    }
    ofs << -1 << endl;
  }*/
}

void PCA::load(const string &filename) {
  /*igzstream ifs; ifs.open(filename.c_str());
  int dimy, dimx, posy;

  if(!ifs.good()) {
    ERR << "[PCA::load] Cannot open '"<<filename<<"' to read PCA." << endl;
  } else {
    ifs >> dimx >> dimy;
    if(dimx==dimy) dim_=dimx;
    else ERR << "[PCA::load] Strange: PCA not square: dimx="<< dimx << " dimy=" << dimy << endl;
    ::std::string tmpstr;
    ifs >> tmpstr;
    mean_=vector<PCA_T>(dimy);
    for(int i=0;i<dimy;++i) {
      ifs >> mean_[i];
    }
    covariance_=vector< vector<PCA_T> >(dimy,vector<PCA_T>(dimx,0.0));
    for(int y=0;y<dimy;++y) {
      ifs >> posy;
      if(posy!=y) {ERR << "[PCA::load] Strangeness in reading PCA." << endl;}
      for(int x=0;x<dimx;++x) {
        ifs >> covariance_[y][x];
      }
    }
    ifs >> posy;
    if(posy!=-1) {ERR << "[PCA::load] Reading PCA was strange. Did not end with -1." << endl;}
  }*/
}

void PCA::dataEnd() {
  int i,j;
  
  for(i=0;i<dim_;++i) mean_[i]/=counter_;

  for(i=0;i<dim_;++i)
    for(j=0;j<dim_;++j) 
      covariance_[i][j]/=counter_;

  for(i=0;i<dim_;++i) 
    for(j=0;j<dim_;++j) 
      covariance_[i][j]-=mean_[i]*mean_[j];
}

void PCA::calcPCA() {

  vector<double> w(dim_);
  A2D<double> v(dim_,dim_);
  v.Fill(0.0);
/*
  double** cov = covariance_.GetP();
  Write2LogPlain("covariance:\n");
  for(int i=0;i<dim_;i++){ for(int j=0;j<dim_;j++){
	  Write2LogPlain("%.4f ",cov[i][j]);
  } Write2LogPlain("\n");}
*/
  int ierr = 0;
  svd(w.size(),w.size(),covariance_.GetP(),&w[0],v.GetP(),&ierr);

  //negate PCs so will be consistent with Matlab
  for(int i=0;i<dim_;i++) for(int j=0;j<dim_;j++){
	  covariance_[i][j]=-covariance_[i][j];
  }

  //NR::svdcmp(covariance_, w, v);
/*
  Write2LogPlain("Eigenvalues: ");
  for(unsigned int i=0;i<w.size();++i) {
    Write2LogPlain("%g ",w[i]);
  } Write2LogPlain("\n");
  Write2LogPlain("sum of eigenvalues = %g",Sum(w));

  Write2LogPlain("transform before sorting:\n");
  for(int i=0;i<dim_;++i) {
    for(int j=0;j<dim_;++j) {
	  Write2LogPlain(" %g",covariance_[i][j]);
  }Write2LogPlain("\n");
  }
*/
  vector< pair<double, vector<double> > > toSort;
  for(unsigned int i=0;i<w.size();++i) {
    vector<double> tmp(dim_);
    for(int j=0;j<dim_;++j) tmp[j]=covariance_[j][i];
    toSort.push_back(pair<double,vector<double> >(w[i],tmp));
  }

  sort(toSort.rbegin(),toSort.rend());

  for(unsigned int i=0;i<toSort.size();++i) {
    copy(toSort[i].second.begin(),toSort[i].second.end(),covariance_[i]);
    w[i]=toSort[i].first;
  }
/*
  Write2LogPlain("Eigenvalues: ");
  for(unsigned int i=0;i<w.size();++i) {
    Write2LogPlain("%g ",w[i]);
  } Write2LogPlain("\n");
  Write2LogPlain("sum of eigenvalues = %g",Sum(w));

  Write2LogPlain("transform after sorting:\n");
  for(int i=0;i<dim_;++i) {
    for(int j=0;j<dim_;++j) {
	  Write2LogPlain(" %g",covariance_[i][j]);
  }Write2LogPlain("\n");
  }
*/
  const bool bSave = false;
  if(bSave)
  {	FILE* fp = fopen("__pcs.txt","w");
	for(int r=0;r<covariance_.Rows();r++){
	  for(int c=0;c<covariance_.Cols();c++)
		  fprintf(fp,"%g ",covariance_[r][c]);
	  fprintf(fp,"\n");
	}
	fclose(fp);
  }

  eigenvalues_=w;

}

const int PCA::dim() const {
  return dim_;
}

const vector<double> & PCA::mean() const {
  return mean_;
}

const A2D<double>& PCA::covariance() const {
  return covariance_;
}

const int PCA::counter() const {
  return counter_;
}
