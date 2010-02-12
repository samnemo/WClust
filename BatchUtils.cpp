// $Id: BatchUtils.cpp,v 1.8 2009/06/25 04:03:33 samn Exp $ 
#include "stdafx.h"
#include "BatchUtils.h"
#include "FileUtils.h"
#include "StringUtils.h"

using namespace std;

void ParseRatings(vector<string>& vstr,vector<double>& vrate)
{
	vrate.push_back(0); //for background cluster
	int i = 0, isz = vstr.size();
	for(i=0;i<isz;i++)
		vrate.push_back(atof(vstr[i].c_str()));
}

void ParseRatings(string& str,vector<string>& vstr)
{
	string strDelim("[],");
	Split(str,strDelim,vstr);
}

void ParseRatings(string& str,vector<double>& vrate)
{
	vector<string> vstr;
	ParseRatings(str,vstr);
	ParseRatings(vstr,vrate);
}

bool ParseQBatchLines(vector<string>& vstr, vector<BatchRec>& vqb,int& iBatchMode)
{
	iBatchMode = BATCH_QUAL_USER; //quality of user-made clusters

	int iSz = vstr.size(), i = 0;
	if(!iSz) return false;
	bool bRead = false;
	string strDelim("\t");
	
	string strBaseDir(vstr[0]);//first line is base directory from which all else is relative
	if(strBaseDir[strBaseDir.size()-1] != '\\')
		strBaseDir += '\\';
	
	for(i=1;i<iSz;i++)
	{
		vector<string> vtoks;
		Split(vstr[i],strDelim,vtoks);
		if(vtoks.size()>=6)
		{
			bRead = true;
			BatchRec oB;
			oB.strBPF =   strBaseDir + vtoks[0];
			if(!stricmp(vtoks[0].c_str(),"orig"))//quality of original cluster values 
				iBatchMode = BATCH_QUAL_ORIG;    //(auto-clustered, or saved by user without .cl)
			oB.strCL =    strBaseDir + vtoks[1];
			oB.iTetrode = atoi(vtoks[2].c_str());
			oB.dPrct =    atof(vtoks[3].c_str());
			oB.outCL =    strBaseDir + vtoks[4];
			ParseRatings(vtoks[5],oB.vRatings);
			oB.bReadyToRun = true;
			vqb.push_back(oB);
		}
	}
	return bRead;
}

bool ParseTSBatchLines(vector<string>& vstr, vector<BatchRec>& vb)
{
	int iSz = vstr.size(), i = 0;
	if(!iSz) return false;
	bool bRead = false;
	string strDelim("\t");
	
	string strBaseDir(vstr[0]);//first line is base directory from which all else is relative
	if(strBaseDir[strBaseDir.size()-1] != '\\')
		strBaseDir += '\\';
	
	for(i=1;i<iSz;i++)
	{
		vector<string> vtoks;
		Split(vstr[i],strDelim,vtoks);
		if(vtoks.size()>=2)
		{
			bRead = true;
			BatchRec oB;
			oB.strBPF =   strBaseDir + vtoks[0]; //input BPF
			oB.outBPF =    strBaseDir + vtoks[1]; //output BPF
			oB.bReadyToRun = true;
			vb.push_back(oB);
		}
	}
	return bRead;
}

bool ParseACBatchLines(vector<string>& vstr, vector<BatchRec>& vb)
{
	int iSz = vstr.size(), i = 0;
	if(!iSz) return false;
	bool bRead = false;
	string strDelim("\t");
	
	string strBaseDir(vstr[0]);//first line is base directory from which all else is relative
	if(strBaseDir[strBaseDir.size()-1] != '\\')
		strBaseDir += '\\';
	
	for(i=1;i<iSz;i++)
	{	vector<string> vtoks;
		Split(vstr[i],strDelim,vtoks);		
		if(vtoks.size()>=7)
		{	BatchRec oB;
			oB.strBPF =   strBaseDir + vtoks[0]; //input BPF
			oB.outBPF =    strBaseDir + vtoks[1]; //output BPF
			oB.iACType = atoi(vtoks[2].c_str());
			if(oB.iACType==CLUST_KM || oB.iACType==CLUST_KK)
			{		
				oB.iMinClust = atoi(vtoks[3].c_str());
				oB.iMaxClust = atoi(vtoks[4].c_str());
				oB.iIters = atoi(vtoks[5].c_str());
				if(oB.iACType==CLUST_KM)
					oB.iDBIters = atoi(vtoks[6].c_str());
				else if(oB.iACType==CLUST_KK)
					oB.iMaxKKClust = atoi(vtoks[6].c_str());
				oB.bAutoC = true;
				oB.bReadyToRun = true;
				vb.push_back(oB);
				bRead = true;
			}
		}
	}
	return bRead;
}
