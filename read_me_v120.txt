V120 : Changes made in EditSpikes.cpp:

void CEditSpikes::FillClList()

moved the line 
list->SetExtendedStyle( LVS_EX_CHECKBOXES  );

from begining of function to end of function.

reason: when it was in the beginning of function, it was used to check mark all the colors. 
now after moving the line. it still has checkboxes for colors but without check mark.

