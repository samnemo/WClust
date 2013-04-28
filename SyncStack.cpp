// $Id: SyncStack.cpp,v 1.2 2008/06/13 15:34:41 samn Exp $ 
// SyncStack.cpp: implementation of the CSyncStack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wclust.h"
#include "SyncStack.h"
#include "Sync.h"
#include "StringUtils.h"
#include "Log.h"
#include <stdlib.h>

#define		END_HEADER_LABEL		"%%END_HEADER"
#define		RECORD_FORMAT_LABEL		"%%BEGIN RECORD_FORMAT"
#define		SAMPLE_LABEL			"%Sample.0"
#define		BRACKET_LABEL			"("
#define		FRAME_COUNT_LABEL		"FrameCount"
#define		TIME_STAMP_LABEL		"1msTimeStamp"
#define		ROOM_X_LABEL			"RoomX"
#define		ROOM_Y_LABEL			"RoomY"
#define		ROOM_Z_LABEL			"RoomZ"
#define		ARENA_X_LABEL			"ArenaX"
#define		ARENA_Y_LABEL			"ArenaY"
#define		ARENA_Z_LABEL			"ArenaZ"
#define		ANGLE_LABEL				"Angle"
#define		ANGLEX_LABEL				"AngleX"
#define		ANGLEY_LABEL				"AngleY"
//#define		AVOIDANCE_STATE_LABEL	"AvoidanceState"
#define		FRAME_INFO_LABEL		"FrameInfo"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSyncStack::CSyncStack()
{
	noAdd=0;
	noEmpty=0;
	lastFrame = 0;

	NoArena = 0;
	NoRoom = 0;
	NoTimeStamp = 0;
	LastPrevious = middle;
}

CSyncStack::~CSyncStack()
{
	SetEmpty();
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::AddSync(CSync *toStore)
{
	SyncStack.push_back(toStore);
	NoTimeStamp++;
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::GetTimeStamp(int position)
{
	MY_SYNC_STACK::iterator index = SyncStack.begin()+position;
	CSync *m = (CSync*)*index;
	return m->GetTimeStamp(); 
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::SetEmpty()
{
	// Remove syncs
	MY_SYNC_STACK::iterator Index;
	for (Index=SyncStack.begin();Index!=SyncStack.end();Index++)
	{
		delete *Index;
	}
	SyncStack.clear();
	TimeIntStack.clear();
	LastPrevious = middle;
	NoArena = 0;
	NoRoom = 0;
	NoTimeStamp = 0;

	noAdd=0;
	noEmpty=0;
	lastFrame = 0;
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::LoadSync(CFile *from)
{
	vector<short> SyncData(1000*sizeof(float));
	int InitTimeStamp;

	from->Read(&InitTimeStamp,4);
	from->Read(&SyncData[0],2000);
//FILE *f = fopen("a.a","a+");
//fprintf(f,"%d ", InitTimeStamp );
//fclose(f);
	if (LastPrevious == middle)
		LastPrevious = SyncData[0];
	else
	{
		if ( (LastPrevious > middle && SyncData[0] < middle) || (LastPrevious < middle && SyncData[0] > middle))
		{
			//newSync = new CSync(InitTimeStamp);
			//SyncStack.push_back(newSync);
			TimeIntStack.push_back(InitTimeStamp);
			LastPrevious = SyncData[0];
		}
	}
	for (int i = 1; i< 1000; i++)
	{
		if ( (SyncData[i] > middle) && (SyncData[i-1] < middle) || (SyncData[i] < middle) && (SyncData[i-1] > middle))
		{
			TimeIntStack.push_back(InitTimeStamp + 10000*i/2000);
		}
	}
	LastPrevious = SyncData[999];

	MY_SYNC_STACK::iterator index;
	for (index =  SyncStack.begin(); index != SyncStack.end(); index++ )
	{
		int TS = (*index)->GetTimeStamp();
	}
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::GetLoadedTS(int position)
{
	MY_SYNC_STACK::iterator index = SyncStack.begin()+position;
	CSyncBPF *m = (CSyncBPF*) *index;
	return m->GetLoadedTS();
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::LoadArenaFromDAT(FILE *from)
{
	if (SyncStack.size() < 2) return 3;//make sure at least 2 entries & if not, return error (NB: 1 entry created when BPF opened)
	const int iBufSz = 16384;
	int		isEOF = 0;
	char	loadText[iBufSz];
	char	endHeader = 0;
	char	*pdest;
	int	Items = 0;
	// Indices from header ( -1 indicates not present )
	int	I_FrameCount   = -1;
	int	I_1msTimeStamp = -1;
	int	I_RoomX		   = -1;
	int	I_RoomY		   = -1;
	int	I_RoomZ		   = -1;
	int	I_FrameInfo	   = -1;
	int	I_Angle		   = -1;
	int I_AngleX       = -1;
	int I_AngleY       = -1;
	do
	{	isEOF = fscanf(from,"%s",loadText);
		if ( isEOF == EOF ) return 2;
		// Checking for RECORD_FORMAT
		if ( strstr(loadText, SAMPLE_LABEL) != NULL )
		{	// '('
			if ( strstr(loadText,"(") == NULL )
			{	isEOF = fscanf(from,"%s",loadText);
				if ( isEOF == EOF ) return 2;
			}
			if ( strstr(loadText, FRAME_COUNT_LABEL) != NULL )
			{	I_FrameCount = Items;
				Items++;
			}
			// Extracting index of parameters from header
			char ExtractDone = 0;
			do 
			{	isEOF = fscanf(from,"%s",loadText);
				if ( isEOF == EOF ) return 2;

				if ( strstr(loadText, FRAME_COUNT_LABEL) != NULL ) I_FrameCount = Items;
				if ( strstr(loadText, TIME_STAMP_LABEL) != NULL )  I_1msTimeStamp = Items;
				if ( strstr(loadText, ROOM_X_LABEL) != NULL )      I_RoomX = Items;
				if ( strstr(loadText, ROOM_Y_LABEL) != NULL )      I_RoomY = Items;
				if ( strstr(loadText, ROOM_Z_LABEL) != NULL )      I_RoomZ = Items;
				if ( strstr(loadText, ARENA_X_LABEL) != NULL )     I_RoomX = Items;
				if ( strstr(loadText, ARENA_Y_LABEL) != NULL )     I_RoomY = Items;
				if ( strstr(loadText, ARENA_Z_LABEL) != NULL )     I_RoomZ = Items;
				
				if ( strstr(loadText, ANGLEX_LABEL) != NULL )       I_AngleX = Items; // these 3 separate due to substrings
				else if ( strstr(loadText, ANGLEY_LABEL) != NULL )  I_AngleY = Items;
				else if ( strstr(loadText, ANGLE_LABEL) != NULL )   I_Angle = Items;

				if ( strstr(loadText, FRAME_INFO_LABEL) != NULL )  I_FrameInfo = Items;
		
				Items++;

				// Is end of brackets				
				pdest = strstr(loadText,")");
				if ( pdest != NULL )
				{	ExtractDone = 1;
					Items--;
				}
			} while ( ExtractDone == 0);
		} // END of RECORD_FORMAT
		// Is found HEADER_END?
		pdest = strstr(loadText, END_HEADER_LABEL);
		if ( pdest != NULL ) endHeader = 1;
	}while (endHeader == 0 && isEOF != EOF);

	// see arena file *.dat 
	int FrameCount, lastFrameCount;
	int TimeStamp,lastTimeStamp;
	int ArenaX=0,lastArenaX;
	int ArenaY=0,lastArenaY;
	int ArenaZ=0,lastArenaZ;	
	short ArenaAng = 0;
	short ArenaAngX = 0, ArenaAngY = 0;

	CSyncBPF *sync;
	MY_SYNC_STACK::iterator index = SyncStack.begin();
//	int TimeStamp0 = ((CSync*)*index)->GetTimeStamp();
	NoArena = 0;
	FrameCount=0; TimeStamp = -10000; ArenaX=ArenaY=ArenaZ=0;
	char firstFrame = 0;
	char secondFrame = 1;
	char biValuedFrame = 0;
	char cBuf[iBufSz];
	vector<string> vstr;
	
	do {
		sync = (CSyncBPF*) *index;
		lastFrameCount = FrameCount;
		lastTimeStamp = TimeStamp;
		lastArenaX = ArenaX;
		lastArenaY = ArenaY;
		lastArenaZ = ArenaZ;

		cBuf[0]=0;
		fgets(cBuf,iBufSz,from);
		Split(cBuf,string(" \t"),vstr);
		int vsz = (int) vstr.size();

		if(I_FrameCount>-1 && I_FrameCount<vsz) FrameCount=atoi(vstr[I_FrameCount].c_str());
		if(I_1msTimeStamp>-1 && I_1msTimeStamp<vsz) TimeStamp=10*atoi(vstr[I_1msTimeStamp].c_str());
		if(I_RoomX>-1 && I_RoomX<vsz) ArenaX=atoi(vstr[I_RoomX].c_str());
		if(I_RoomY>-1 && I_RoomY<vsz) ArenaY=atoi(vstr[I_RoomY].c_str());
		if(I_RoomZ>-1 && I_RoomZ<vsz) ArenaZ=atoi(vstr[I_RoomZ].c_str());
		if(I_Angle>-1 && I_Angle<vsz) ArenaAng=atoi(vstr[I_Angle].c_str());
		if(I_AngleX>-1 && I_AngleX<vsz) ArenaAngX=atoi(vstr[I_AngleX].c_str());
		if(I_AngleY>-1 && I_AngleY<vsz) ArenaAngY=atoi(vstr[I_AngleY].c_str());

		if(feof(from)) isEOF = EOF;
		
		if ( firstFrame && secondFrame && isEOF != EOF  )
		{	// 3th ...... nth
			if ( FrameCount - lastFrameCount <= 1 )
			{	// is in order i.e. 123... or 11223344...
				noAdd++;
				sync->SetArena(ArenaX,ArenaY,ArenaZ,ArenaAng,ArenaAngX,ArenaAngY);
				sync->SetLoadedTS(TimeStamp);
				NoArena++;
				index++;
				}
			else {
				char stop = 0;
				int i = lastFrameCount+1;
				do {
					char j = 0;
					do {
						noEmpty++;
						sync->SetArena(0,0,0,0,0,0);
						sync->SetLoadedTS(0);
						NoArena++;
						index++;
						if ( index == SyncStack.end() )
						{	stop = 1;
						}
						else
						{	sync = (CSyncBPF*)*index;
						}
						j++;
					} while ( j <= biValuedFrame && stop == 0);
					i++;
				} while ( i < FrameCount && stop == 0);
				if ( index != SyncStack.end() )
				{	noAdd++;
					sync->SetArena(ArenaX,ArenaY,ArenaZ,ArenaAng,ArenaAngX,ArenaAngY);
					sync->SetLoadedTS(TimeStamp);
					lastFrameCount = FrameCount;
					NoArena++;
					index++;
				}
			}
		}
		if ( secondFrame == 0 && isEOF != EOF )
		{	noAdd++;
			secondFrame = 1;
			sync->SetArena(ArenaX,ArenaY,ArenaZ,ArenaAng,ArenaAngX,ArenaAngY);
			sync->SetLoadedTS(TimeStamp);
			NoArena++;
			index++;
			if (lastFrameCount == FrameCount)
				biValuedFrame = 1;
		}
		if ( firstFrame == 0 && isEOF != EOF )
		{	noAdd++;
			firstFrame = 1;
			secondFrame = 0;
			sync->SetArena(ArenaX,ArenaY,ArenaZ,ArenaAng,ArenaAngX,ArenaAngY);
			sync->SetLoadedTS(TimeStamp);
			NoArena++;
			index++;
		}
	} while ( isEOF != EOF && index != SyncStack.end() );
	lastFrame = FrameCount;	
//	Save();
	if( !feof(from) && index == SyncStack.end() ) // Write to log that SyncStack does not match dat file
		Write2Log("Warning: SycnStack size is %d, NoArena=%d, extra entries at end of DAT file.",SyncStack.size(),NoArena);
	return 0; // success
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::LoadRoomFromDAT(FILE *from)
{
	if (SyncStack.size() < 2) return 3;//make sure at least 2 entries & if not, return error (NB: 1 entry created when BPF opened)
	const int iBufSz = 16384;
	int		isEOF;
	char	loadText[iBufSz];
	char	endHeader = 0;
	char	*pdest;
	int	Items = 0;
	// Indices from header ( -1 indicates not present )
	int	I_FrameCount   = -1;
	int	I_1msTimeStamp = -1;
	int	I_RoomX		   = -1;
	int	I_RoomY		   = -1;
	int	I_RoomZ		   = -1;
	int	I_Angle		   = -1;
	int	I_AngleX	   = -1;
	int	I_AngleY	   = -1;
	int	I_FrameInfo	   = -1;

	do
	{	isEOF = fscanf(from,"%s",loadText);
		if ( isEOF == EOF ) return 2;

		// Checking for RECORD_FORMAT
		if ( strstr(loadText, SAMPLE_LABEL) != NULL )
		{	// '('
			if ( strstr(loadText,"(") == NULL )
			{	isEOF = fscanf(from,"%s",loadText);
				if ( isEOF == EOF ) return 2;
			}
			if ( strstr(loadText, FRAME_COUNT_LABEL) != NULL )
			{	I_FrameCount = Items;
				Items++;
			}
			// Extracting index of parameters from header
			char ExtractDone = 0;
			do {
				isEOF = fscanf(from,"%s",loadText);
				if ( isEOF == EOF ) return 2;

				if ( strstr(loadText, FRAME_COUNT_LABEL) != NULL ) I_FrameCount = Items;
				if ( strstr(loadText, TIME_STAMP_LABEL) != NULL )  I_1msTimeStamp = Items;
				if ( strstr(loadText, ROOM_X_LABEL) != NULL )      I_RoomX = Items;
				if ( strstr(loadText, ROOM_Y_LABEL) != NULL )      I_RoomY = Items;
				if ( strstr(loadText, ROOM_Z_LABEL) != NULL )      I_RoomZ = Items;

				if ( strstr(loadText, ANGLEX_LABEL) != NULL )       I_AngleX = Items; // these 3 separate due to substrings
				else if ( strstr(loadText, ANGLEY_LABEL) != NULL )  I_AngleY = Items;
				else if ( strstr(loadText, ANGLE_LABEL) != NULL )   I_Angle = Items;

				if ( strstr(loadText, FRAME_INFO_LABEL) != NULL )  I_FrameInfo = Items;
		
				Items++;

				// Is end of brackets				
				pdest = strstr(loadText,")");
				if ( pdest != NULL )
				{	ExtractDone = 1;
					Items--;
				}
			} while ( ExtractDone == 0);
		} // END of RECORD_FORMAT
		// Is found HEADER_END?
		pdest = strstr(loadText, END_HEADER_LABEL);
		if ( pdest != NULL ) endHeader = 1;
	}while (endHeader == 0 && isEOF != EOF);

	CSyncBPF	*sync;

	// see arena file *.dat 
	int		FrameCount, lastFrameCount;
	int		TimeStamp,lastTimeStamp;
	int		RoomX=0,lastRoomX;
	int		RoomY=0,lastRoomY;
	int		RoomZ=0,lastRoomZ;
	short	RoomAng = 0;
	short   RoomAngX = 0, RoomAngY = 0;

	short ArenaAng = 0;
	MY_SYNC_STACK::iterator index = SyncStack.begin();
	int TimeStamp0 = ((CSync*)*index)->GetTimeStamp();
	index = SyncStack.begin(); // why reassigning to same value again??
	NoRoom = 0;
	
	FrameCount=0; TimeStamp = -10000; RoomX=RoomY=RoomZ=0;
	char firstFrame = 0;
	char secondFrame = 1;
	char biValuedFrame = 0;
	char cBuf[iBufSz];
	vector<string> vstr;

	do {
		sync = (CSyncBPF*)*index;
		lastFrameCount = FrameCount;
		lastTimeStamp = TimeStamp;
		lastRoomX = RoomX;
		lastRoomY = RoomY;
		lastRoomZ = RoomZ;
		
		cBuf[0]=0;
		fgets(cBuf,iBufSz,from);
		Split(cBuf,string(" \t"),vstr);
		int vsz = (int) vstr.size();

		if(I_FrameCount>-1 && I_FrameCount<vsz) FrameCount=atoi(vstr[I_FrameCount].c_str());
		if(I_1msTimeStamp>-1 && I_1msTimeStamp<vsz) TimeStamp=10*atoi(vstr[I_1msTimeStamp].c_str());
		if(I_RoomX>-1 && I_RoomX<vsz) RoomX=atoi(vstr[I_RoomX].c_str());
		if(I_RoomY>-1 && I_RoomY<vsz) RoomY=atoi(vstr[I_RoomY].c_str());
		if(I_RoomZ>-1 && I_RoomZ<vsz) RoomZ=atoi(vstr[I_RoomZ].c_str());
		if(I_Angle>-1 && I_Angle<vsz) RoomAng=atoi(vstr[I_Angle].c_str());
		if(I_AngleX>-1 && I_AngleX<vsz) RoomAngX=atoi(vstr[I_AngleX].c_str());
		if(I_AngleY>-1 && I_AngleY<vsz) RoomAngY=atoi(vstr[I_AngleY].c_str());

		if(feof(from)) isEOF = EOF;

		if ( firstFrame && secondFrame && isEOF != EOF )
		{	// 3th ...... nth
			if ( FrameCount - lastFrameCount <= 1 )
			{	// is in order i.e. 123... or 11223344...
				noAdd++;
				sync->SetRoom(RoomX,RoomY,RoomZ,RoomAng,RoomAngX,RoomAngY);
				sync->SetLoadedTS(TimeStamp);
				NoRoom++;
				index++;
			}
			else {
				char stop = 0;
				int i = lastFrameCount+1;
				do {
					char j = 0;
					do {
						noEmpty++;
						sync->SetRoom(0,0,0,0,0,0);
						sync->SetLoadedTS(0);
						NoRoom++;
						index++;
						if ( index == SyncStack.end() )
						{	stop = 1;
						}
						else
						{	sync = (CSyncBPF*)*index;
						}
						j++;
					} while ( j <= 1 && stop == 0);
					i++;
				} while ( i < FrameCount && stop == 0);
				if ( index != SyncStack.end() )
				{	noAdd++;
					//sync->SetArena(RoomX,RoomY,RoomZ,RoomAng); // shouldn't it be SetRoom ???
					sync->SetRoom(RoomX,RoomY,RoomZ,RoomAng,RoomAngX,RoomAngY);
					sync->SetLoadedTS(TimeStamp);
					lastFrameCount = FrameCount;
					NoRoom++;
					index++;
				}
			}
		}
		if ( secondFrame == 0 && isEOF != EOF )
		{	noAdd++;
			secondFrame = 1;
			sync->SetRoom(RoomX,RoomY,RoomZ,RoomAng,RoomAngX,RoomAngY);
			sync->SetLoadedTS(TimeStamp);
			NoRoom++;
			index++;
			if (lastFrameCount == FrameCount)
				biValuedFrame = 1;
		}
		if ( firstFrame == 0 && isEOF != EOF )
		{	noAdd++;
			firstFrame = 1;
			secondFrame = 0;
			sync->SetRoom(RoomX,RoomY,RoomZ,RoomAng,RoomAngX,RoomAngY);
			sync->SetLoadedTS(TimeStamp);
			NoRoom++;
			index++;
		}
	} while ( isEOF != EOF && index != SyncStack.end() );
	lastFrame = FrameCount;
	if( !feof(from) && index == SyncStack.end() ) // Write to log that SyncStack does not match dat file
		Write2Log("Warning: SycnStack size is %d, NoRoom=%d, extra entries at end of DAT file.",SyncStack.size(),NoRoom);
	return 0;
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::Save()
{
	// This is only for TESTING. In the final version this will NOT be used
	FILE *fptr;
	fptr = fopen("a.c","w");
	MY_SYNC_STACK::iterator index;
	CSyncBPF *ms;
	int old = -1000;
	index = SyncStack.begin();
	fprintf(fptr,"%d\n",NoArena);
	int TS0 = ((CSync*)*SyncStack.begin())->GetTimeStamp();
	for (int i=0;i<NoArena;i++)
	{
		ms = (CSyncBPF*)*index;
//		if ( ms->GetTimeStamp() - old < 10 )
//		{ MessageBeep(0);}
//		old = ms->GetTimeStamp();
		fprintf(fptr,"t:%d\tl:%d\td:%d\tD:%d\tax:%d\tay:%d\trx:%d\try:%d\n",
			ms->GetTimeStamp(),
			ms->GetLoadedTS(),
			ms->GetLoadedTS() - ms->GetTimeStamp(),
			ms->GetLoadedTS() - (ms->GetTimeStamp()-TS0),
			(unsigned char) ms->GetArenaX(),
			(unsigned char) ms->GetArenaY(),
			(unsigned char) ms->GetRoomX(),
			(unsigned char) ms->GetRoomY());
		index++;
	}
	fclose(fptr);
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::SaveArenaFirst(CFile *fptr)
{
	CSyncBPF *ms;
	int TS, i;       // timestamp, loop index
	unsigned char L; // location
	short Ang,AngX,AngY;       // angle
	MY_SYNC_STACK::iterator index = SyncStack.begin(); // record
	for (i = 0; i< NoArena; i++,index++)
	{	ms = (CSyncBPF*)*index;
		fptr->Write("A",1); // Arena record identifier
		TS = ms->GetTimeStamp(); fptr->Write(&TS,4);
		L = ms->GetArenaX(); 	 fptr->Write(&L,1);
		L = ms->GetArenaY();	 fptr->Write(&L,1);
		L = ms->GetArenaZ();	 fptr->Write(&L,1);
		Ang = ms->GetArenaAng(); fptr->Write(&Ang,2);
		AngX = ms->GetArenaAngX(); fptr->Write(&AngX,2);
		AngY = ms->GetArenaAngY(); fptr->Write(&AngY,2);
	}
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::SaveRoomFirst(CFile *fptr)
{
	CSyncBPF *ms;	
	int TS, i;       // timestamp, loop index
	unsigned char L; // location
	short Ang,AngX,AngY;       // angle
	MY_SYNC_STACK::iterator index = SyncStack.begin(); // record
	for (i = 0; i< NoRoom; i++,index++)
	{	ms = (CSyncBPF*)*index;
		fptr->Write("R",1); // Room record identifier
		TS = ms->GetTimeStamp(); fptr->Write(&TS,4);
		L = ms->GetRoomX();      fptr->Write(&L,1);
		L = ms->GetRoomY();      fptr->Write(&L,1);
		L = ms->GetRoomZ();      fptr->Write(&L,1);
		Ang = ms->GetRoomAng();  fptr->Write(&Ang,2);
		AngX = ms->GetRoomAngX();  fptr->Write(&AngX,2);
		AngY = ms->GetRoomAngY();  fptr->Write(&AngY,2);
	}
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::LoadArena(CFile *from, bool hasZ, bool hasAngXAngY)
{
	int TS;
	unsigned char X,Y,Z=0;
	short Ang,AngX=0,AngY=0;
	
	from->Read(&TS,4);
	from->Read(&X,1);
	from->Read(&Y,1);
	if(hasZ) from->Read(&Z,1);
	from->Read(&Ang,2);
	if(hasAngXAngY) 
	{
		from->Read(&AngX,2);
		from->Read(&AngY,2);
	}

	CSyncBPF *sync;
	if (NoRoom > NoArena)
	{
		sync = (CSyncBPF*) *(SyncStack.begin() + NoArena);
		sync->SetArena(X,Y,Z,Ang,AngX,AngY);
		sync->SetLoadedTS(TS);
		NoArena++;
	}
	else
	{
		sync = (CSyncBPF*) new CSyncBPF(TS);
		sync->SetArena(X,Y,Z,Ang,AngX,AngY);
		sync->SetLoadedTS(TS);
		AddSync(sync);
		NoArena++;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
int CSyncStack::LoadRoom(CFile *from,bool hasZ, bool hasAngXAngY)
{
	int TS;
	unsigned char X,Y,Z=0;
	short Ang,AngX=0,AngY=0;

	from->Read(&TS,4);
	from->Read(&X,1);
	from->Read(&Y,1);
	if(hasZ) from->Read(&Z,1);
	from->Read(&Ang,2);
	if(hasAngXAngY) 
	{
		from->Read(&AngX,2);
		from->Read(&AngY,2);
	}
	
	CSyncBPF *sync;
	if (NoArena > NoRoom)
	{
		sync = (CSyncBPF*) *(SyncStack.begin() + NoRoom);
		sync->SetRoom(X,Y,Z,Ang,AngX,AngY);
		sync->SetLoadedTS(TS);
		NoRoom++;
	}
	else
	{
		sync = (CSyncBPF*) new CSyncBPF(TS);
		sync->SetRoom(X,Y,Z,Ang,AngX,AngY);
		sync->SetLoadedTS(TS);
		AddSync(sync);
		NoRoom++;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::NewPosFromUFF(int TS, unsigned char RedX, unsigned char RedY, unsigned char GreenX, unsigned char GreenY)
{
	CSyncUFF *sync;
	sync = (CSyncUFF*) new CSyncUFF(TS);
	sync->SetRed( RedX, RedY );
	sync->SetGreen(GreenX, GreenY);
	sync->SetCluster(0);
	AddSync(sync);
	NoArena++;
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::PrepareClustering()
{
	cIndex = 0;
	if ( cIndex < SyncStack.size() )
		((CSync*)*(SyncStack.begin() + cIndex))->SetCluster(0);
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::Clustering(int TS, unsigned char newCl, unsigned char newOrigCl)
{
	if ( cIndex < SyncStack.size() )
	{
		while ( cIndex < SyncStack.size() && ((CSync*)*(SyncStack.begin() + cIndex))->GetTimeStamp() < TS )
		{
			cIndex++;
			if ( cIndex < SyncStack.size() )
			{
				((CSync*)*(SyncStack.begin() + cIndex))->SetCluster(0);
				((CSync*)*(SyncStack.begin() + cIndex))->SetOrigCluster(0);
			}
		}
		if ( cIndex < SyncStack.size() )
		{
			if (newCl == 255)
				newCl = 0;
			if (newOrigCl = 255)	//????????????????
				newOrigCl = 0;

			((CSync*)*(SyncStack.begin() + cIndex))->SetCluster(newCl);
			((CSync*)*(SyncStack.begin() + cIndex))->SetOrigCluster(newOrigCl);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::FinishClustering()
{
	while ( cIndex < SyncStack.size() )
	{
		cIndex++;
		if ( cIndex < SyncStack.size() )
		{
			((CSync*)*(SyncStack.begin() + cIndex))->SetCluster(0);
			((CSync*)*(SyncStack.begin() + cIndex))->SetOrigCluster(0);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::DrawArenaRoom(CDC *pDC, CRect drawWin, int firstTS, int timeWin, char params, unsigned char *clView, int whichDraw)
{
	// find the FIRST frame
	timeWin *= 10;
	int minT,maxT;	
	int indexFrame = SyncStack.size() / 2 ; 
	int increment = indexFrame;
	minT = 0; maxT = SyncStack.size() - 1;
	char finishedFind = 0;
	while (increment > 0)
	{
		if ( firstTS > ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
		{
			minT = indexFrame;
			increment = (maxT - minT)/2;
			indexFrame += increment;
		}
		else{
			if ( firstTS < ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
			{
				maxT = indexFrame;
				increment = (maxT - minT)/2;
				indexFrame -= increment;
			}
			else{
				// ==
				finishedFind = 1;
				increment = 0;
			}
		}
	}
	
	if ( ! finishedFind )
	{
		indexFrame = minT;
		if ( firstTS > ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() )
		{
			indexFrame = maxT;
		}
		else indexFrame = minT;
	}
	
	
	unsigned char x,y;
	unsigned char cluster, oldClust;
	char fine = 0;

	if (indexFrame >= SyncStack.size())
	{
		fine = 1;
	}
	if ( ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() >= firstTS )
	{ 
		int ai = 0;
		while ( ! fine && ((CSync*) *(SyncStack.begin() + indexFrame))->GetTimeStamp() < firstTS + timeWin )
		{
			if ( params & 2 )
			{	// Arena
				if ( typeOfData == 1 )
				{	// BPF
					x = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetArenaX() /2;
					y = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetArenaY() /2;
				}
				if ( typeOfData == 2 )
				{	// UFF
					x = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetRedX() /2;
					y = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetRedY() /2;
				}

			}
			if ( params & 4 )
			{	// Room
				x = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetRoomX() /2;
				y = ((CSyncBPF*) *(SyncStack.begin() + indexFrame))->GetRoomY() /2;
			}
			cluster = ((CSync*) *(SyncStack.begin() + indexFrame))->GetCluster();
			oldClust = ((CSync*) *(SyncStack.begin() + indexFrame))->GetOrigCluster();

			char draw = ( whichDraw == 0 ) ? ( clView[cluster] & (params & 7)) : ( clView[oldClust] & (params & 7));

			if ( x != 0 && y != 0 &&  draw )
			{
				unsigned char colorIndex = ( whichDraw == 0 ) ? cluster : oldClust;
				switch ( (params & 0x38) / 8 )
				{
				case 0:	// points
					pDC->SetPixel(drawWin.left + x, drawWin.top + y, m_PalStack->GetSColor(colorIndex));
					break;
				case 1:	// circles
					pDC->SetPixel(drawWin.left + x + 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
					pDC->SetPixel(drawWin.left + x - 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
					pDC->SetPixel(drawWin.left + x,drawWin.top + y + 1,m_PalStack->GetSColor(colorIndex));
					pDC->SetPixel(drawWin.left + x,drawWin.top + y - 1,m_PalStack->GetSColor(colorIndex));
					break;
				case 2:	// vectors
					if ( typeOfData == 1)
					{
						pDC->SetPixel(drawWin.left + x + 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x - 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x,drawWin.top + y + 1,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x,drawWin.top + y - 1,m_PalStack->GetSColor(colorIndex));
						//	TODO
					}
					if ( typeOfData == 2 )
					{
						pDC->SetPixel(drawWin.left + x + 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x - 1,drawWin.top + y,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x,drawWin.top + y + 1,m_PalStack->GetSColor(colorIndex));
						pDC->SetPixel(drawWin.left + x,drawWin.top + y - 1,m_PalStack->GetSColor(colorIndex));
						
						unsigned char x2 = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetGreenX() /2;
						unsigned char y2 = ((CSyncUFF*) *(SyncStack.begin() + indexFrame))->GetGreenX() /2;
						if ( x2 != 0 && y2 != 0 )
						{
							CPen newPen(PS_SOLID,1, m_PalStack->GetSColor( colorIndex ));
							CPen *pOldPen = pDC->SelectObject(&newPen);
							pDC->MoveTo(drawWin.left + x, drawWin.top + y);
							pDC->LineTo(drawWin.left + x2, drawWin.top + y2);
							pDC->SelectObject(pOldPen);
						}
					}
					break;
				}
			}
			indexFrame++;
			if (indexFrame >= SyncStack.size())
			{
				indexFrame--;
				fine = 1;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::CreateSync()
{
	MY_INT_STACK::iterator indexInt;

	int TS;
	CSyncBPF *sync;
	for (indexInt =  TimeIntStack.begin(); indexInt != TimeIntStack.end(); indexInt++ )
	{
		TS = *indexInt;
		sync = (CSyncBPF*) new CSyncBPF( TS );
		AddSync(sync);
	}
}

//////////////////////////////////////////////////////////////////////
void CSyncStack::OrganizeSync(char type, float param)
{
	MY_SYNC_STACK::iterator	iSync;
	MY_INT_STACK::iterator iInt;
	float fTS;
	switch (type)
	{
	case 1:	// Sync
		iSync = SyncStack.begin();
		iInt = TimeIntStack.begin();
		while ( iSync != SyncStack.end() && iInt != TimeIntStack.end() )
		{
			(*iSync)->SetTimeStamp( *iInt );
			iSync++;
			iInt++;
		}
		break;
	case 2:	// 16.7, 20, 33.3, 40, 100 ms
		iSync = SyncStack.begin();
		iInt = TimeIntStack.begin();
		fTS = *iInt;
		while ( iSync != SyncStack.end() )
		{
			(*iSync)->SetTimeStamp( (int) fTS );
			iSync++;

			if ( param == 16 )
			{
				fTS += 10000 /60;
			}
			if ( param == 20 )
			{
				fTS += 10000 /50;
			}
			if ( param == 33 )
			{
				fTS += 10000 /30;
			}
			if ( param == 40 )
			{
				fTS += 10000 /25;
			}
			if ( param == 100 )
			{
				fTS += 10000 /10;
			}
		}
		break;
	case 3:	// other
		iSync = SyncStack.begin();
		iInt = TimeIntStack.begin();
		fTS = (float) *iInt;
		param = param * 10; // 100 us
		
		while ( iSync != SyncStack.end() )
		{
			(*iSync)->SetTimeStamp( (int) fTS );
			iSync++;
			fTS += param;
		}

		break;
	case 4:	// from BPF (loaded)
		iSync = SyncStack.begin();
		while ( iSync != SyncStack.end() && iInt != TimeIntStack.end() )
		{
			(*iSync)->SetTimeStamp( ((CSyncBPF*)*iSync)->GetLoadedTS() );
			iSync++;
		}

		break;
	}
}
