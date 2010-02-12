#include "stdafx.h"
// FILENAME: Node.C
// ------------------------------------------------------------
// Copyright 2003, The University of Maryland at College Park
// ------------------------------------------------------------
// Written by Beomseok Nam (bsnam@cs.umd.edu) based on
// Hybrid-tree code by Chakrabarti Kaushik at the UC, Irvine.
// ------------------------------------------------------------


#include "Node.h"


NodeBase::NodeBase(int nodeType) : type(nodeType), level(-1), inmemory(0) {
}

off_t NodeBase::getOffset()
{
   return offset;
}
