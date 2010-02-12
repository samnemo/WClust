// $Id: A2D.h,v 1.6 2009/06/24 18:03:55 samn Exp $ 
#pragma once

template < typename T >
T **Allocate2DArray( int nRows, int nCols)
{
	if(nRows<=0 || nCols<=0) return 0x0;

    T **ppi;
    T *pool;
    T *curPtr;
    //(step 1) allocate memory for array of elements of column

    ppi = new T*[nRows];

    //(step 2) allocate memory for array of elements of each row
    pool = new T [nRows * nCols];

    // Now point the pointers in the right place
    curPtr = pool;
    for( int i = 0; i < nRows; i++)
    {
        *(ppi + i) = curPtr;
         curPtr += nCols;
    }
    return ppi;
}

template < typename T >
void Free2DArray(T** Array)
{
    delete [] *Array;
    delete [] Array;
}

template < class T >
class AutoFree2D
{
	T** m_p;
public:
	AutoFree2D(T** p)
		:m_p(p)
	{
	}
	~AutoFree2D()
	{
		Free2DArray(m_p);
	}
};

template < class T >
class A2D
{
	T** m_p;
	int m_iRows;
	int m_iCols;
	//A2D(const A2D&);

public:
	//get # rows
	int Rows() const { return m_iRows; }
	//get # cols
	int Cols() const { return m_iCols; }
	//construct empty
	A2D(T** p,int iRows,int iCols)
		:m_p(p),
		 m_iRows(iRows),
		 m_iCols(iCols)
	{
	}
	A2D()
		:m_p(0),
		m_iRows(0),
		m_iCols(0)
	{
	}
	A2D(const A2D& r)
	{	m_p=0;
		Init(r.Rows(),r.Cols());
		int i,j;
		for(i=0;i<m_iRows;i++)
			for(j=0;j<m_iCols;j++)
				m_p[i][j]=r[i][j];
	}
	A2D& operator=(const A2D& r)
	{	if(&r==this) return *this;
		Init(r.Rows(),r.Cols());
		int i,j;
		for(i=0;i<m_iRows;i++)
			for(j=0;j<m_iCols;j++)
				m_p[i][j]=r[i][j];
		return *this;
	}
	//init with specified size
	bool Init(int iRows,int iCols)
	{
		Clear();

		m_p = Allocate2DArray<T>(iRows,iCols);
		
		if(!m_p)
			return false;
		
		m_iRows = iRows;
		m_iCols = iCols;
		
		return true;
	}
	//construct with specified size
	A2D(int iRows,int iCols)
		:m_p(0)
	{
		Init(iRows,iCols);
	}
	//destructor
	virtual ~A2D()
	{
		Clear();
	}
	//free memory
	void Clear()
	{
		if(m_p)
			Free2DArray<T>(m_p);
		m_p=0;
		m_iRows=m_iCols=0;
	}
	//get 2D pointer
	T** GetP()
	{
		return m_p;
	}
	//get row pointer
	T* operator[](int i)
	{
		return m_p[i];
	}
	T* operator[](int i) const
	{
		return m_p[i];
	}
	//fill with val
	void Fill(const T& val)
	{
		int x,y;
		for(y=0;y<m_iRows;y++)
			for(x=0;x<m_iCols;x++)
				m_p[y][x]=val;
	}
};
