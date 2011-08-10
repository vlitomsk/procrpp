#include "Errors.h"
#include <iostream>
#include <cstdlib>

using namespace Errors;
using namespace std;

void Errors::errorExit(CrppError err) {
    cerr << "Error: " << err.descr << "\nWill now exit! Happy tracing :)\n";
    exit(err.exitcode);
}
