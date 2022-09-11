// Decomposes the file to decos, so that it is easier to work with file

#ifndef DecomposeH
#define DecomposeH
//#include "General.h"
#include "Common Files\\File.h"

class Decompose
{
public:	  
	Pack pack;

private:  
	String spaces;
	String operator_chars;

	String GetMainPart(SimpleFile& file);

public:
	Decompose(WString simple_file_address);
};

#endif
