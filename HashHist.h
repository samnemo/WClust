// $Id: HashHist.h,v 1.1 2008/08/11 20:45:45 samn Exp $ 
#ifndef HASH_HIST_H
#define HASH_HIST_H

#include <hash_map>
using namespace std;

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2);
  }
};

typedef hash_map< const char *, int ,hash_compare<const char*, eqstr> > myhash;
typedef myhash::iterator HIT;

class HashHist
{
	myhash m_oHash;
	int m_iSize;
public:
	void Print2Log();
	int Count();
	int ForceCount();
	float Entropy();
	bool RemoveVertex(const char* str);
	void AddVertex(const char* str);
	float Prob(const char* str);
	void Clear();
	HashHist();
	virtual ~HashHist();
	float KLDiv(HashHist& Q); // KLDiv from this distribution to Q
};

#endif
