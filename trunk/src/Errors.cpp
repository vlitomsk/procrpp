#include <Errors.h>
#include <iostream>
#include <cstdlib>

using namespace Errors;

void Errors::errorExit(CrppError err) {
	std::cerr << "Error: " << err.descr << "\nWill now exit! Happy tracing :)\n";
	exit(err.exitcode);
}
