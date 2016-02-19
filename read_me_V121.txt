fixed: V121

1. POsition reg dialougue with default value : DlgPositionReg.cpp

 Jaini : Commented on 10/7/15 : 

When position registration window opens up, 
the default value should be "Without changes(from BPF)". 
Commented part has default value of "Use Sync" and "Without changes(from BPF)" is disabled.

Also, to make sure everytime when you open it just has "Without changes(from BPF)" 
selected, forced it to go back to m_type = 4 under "OnOK" function.

Changed default value of "m_type" in line21 from "0" to "4" & commented line67 with "case 0".
/*
{
 if ( !(m_MainDataStack->InBpfExist & 3) )
	{
        	CWnd::GetDlgItem(IDC_RADIO_WITHOUT)->EnableWindow(false);
		((CButton*) CWnd::GetDlgItem(IDC_RADIO_SYNC))->SetCheck(true);
	}
	else
	{
		CWnd::GetDlgItem(IDC_RADIO_WITHOUT)->EnableWindow(true);
		((CButton*) CWnd::GetDlgItem(IDC_RADIO_WITHOUT))->SetCheck(true);
	}
		bFirst=false;  
	}
*/ Commented this and wrote below



	{
		if ( !(m_MainDataStack->InBpfExist & 3) )
		{ 
			CWnd::GetDlgItem(IDC_RADIO_WITHOUT)->EnableWindow(true);
			((CButton*) CWnd::GetDlgItem(IDC_RADIO_WITHOUT))->SetCheck(true);
		}
	}

Jaini : Below changes Made : 10/13/15

Added MessageBox in OK function with warning/alert message if anything else 
chosen other than default



2. Missing room and Arena frames : SyncStack.cpp

made below chnges in LoadRoomFromDAT and LoadArenaFromDAT functions

Jaini : DATE MODIFIED : 10/6/15 and 10/13/15.
	
Changed the condition in while loop. You can use equal sign in condition only if
biValuedFrame=0. If you make it 1, then remove equal sign. Otherwise it will add
twice as many times x and y values for missing frames when comparing BPF to DAT


//} while ( j <= 1 && stop == 0);   */  Commented this and wrote below.

  } while ( j <= biValuedFrame && stop == 0);   

