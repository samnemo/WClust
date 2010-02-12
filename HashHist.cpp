// $Id: HashHist.cpp,v 1.1 2008/08/11 20:45:55 samn Exp $ 
#include "stdafx.h"
#include "HashHist.h"
#include "Hist.h"

HashHist::HashHist()
:m_iSize(0)
{}

HashHist::~HashHist()
{}

void HashHist::Clear()
{
	m_iSize = 0;
	m_oHash.clear();
}

int HashHist::Count()
{
	return m_iSize;
}

float HashHist::Entropy()
{
	HIT IT;
	float dEntrop = 0;
	double sz = m_iSize;
	for(IT=m_oHash.begin();IT!=m_oHash.end();IT++)
	{
		if(IT->second<=0) continue;
		double dv = IT->second / sz;
		dEntrop += dv * log2(dv);
	}
	return -dEntrop;
}


int HashHist::ForceCount()
{
	HIT IT;
	int cnt = 0;
	for(IT=m_oHash.begin();IT!=m_oHash.end();IT++)
		cnt += IT->second;
	return cnt;
}

float HashHist::Prob(const char* str)
{
	HIT IT = m_oHash.find(str);
	if(IT==m_oHash.end())
		return 0.0f;
	return IT->second / static_cast<float>(m_iSize);
}

float HashHist::KLDiv(HashHist& Q)
{
	HIT IT;
	float dKLD = 0.0f;
	float sz = m_iSize;
	for(IT=m_oHash.begin();IT!=m_oHash.end();IT++)
	{
		if(IT->second<=0) continue;
		float pp = IT->second / sz;
		float qp = Q.Prob(IT->first);
		if(pp <= 0.0f || qp <= 0.0f)
			continue;
		dKLD += pp * log2(pp/qp);
	}
	return dKLD;
}

bool HashHist::RemoveVertex(const char* str)
{
	HIT IT = m_oHash.find(str);
	if(IT==m_oHash.end())
		return false;
	if((*IT).second > 1) 
		(*IT).second -= 1; 
	else
		m_oHash.erase(str);
	m_iSize -= 1;
	return true;
}

void HashHist::AddVertex(const char* str)
{
	HIT IT = m_oHash.find(str);
	if(IT==m_oHash.end()) 
		m_oHash.insert(pair<const char*,int>(str,1));
		//H[str]=1;
	else 
		(*IT).second += 1;
	m_iSize += 1;
}

void HashHist::Print2Log()
{
	Write2LogPlain("Hash:");
	HIT IT;
	for(IT=m_oHash.begin();IT!=m_oHash.end();IT++)
		Write2LogPlain("%s : %d",IT->first,IT->second);
	Write2LogPlain("\n");
}
