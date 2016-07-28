Version 123 changes:: Jaini Shah :: 2.19.16

In all of the below functions, replaced 63/32000 with CVertex::FREQ/CVertex::SAMPLES
OnPaint, OnLButtonDown, OnMouseMove, OnSelchangeComboprop, OnSelchangeCmbSelParam, 

Reason for change : Milenna had started recordings where Spike Sampling Freq was 48000 and 
Tetrode record format was 1440. 63/32000 was displaying maximum of 2ms of data. 
where as for Milenna's recording we needed 1440/48000 = 30ms of data display. So, now with 
CVertex::FREQ/CVertex::SAMPLES, it gets FREQ and SAMPLES values from header file. 
Vertex.cpp=>CVerxStack:GetBPFHeader_SetupInformation and 
Vertex.cpp=>CVerxStack::GetBPFHeader


Also, bpf.h has all the labels from header file. each label has a name defined. 
search by these names, if looking to make any changes in any of them. 

vertex.cpp has some static variables for CVertex class
	static int FREQ;
	static int SAMPLES;
	static int RESOLUTION;
	static int CHANNELS;

and int	SAMPLE_FREQ;		// Frequency of electrode signal
    int	EEG_SAMPLE_FREQ;	// Frequency of eeg signal
    int	NUM_CHANNELS;
    int	NUM_SAMPLES;  are local to CVerxStack class.

