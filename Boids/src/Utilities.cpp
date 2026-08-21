#include "Utilities.h"

void makeLowerCase(std::string* string)
{
	for (auto ch : *string)
	{
		ch = std::tolower(ch);
	}
}