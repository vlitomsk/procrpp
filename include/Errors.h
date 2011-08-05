#ifndef PROCRPP_ERRORS_H
#define PROCRPP_ERRORS_H

namespace Errors {
	struct CrppError {
		char descr[256];
		int exitcode;
	};

	static CrppError ErrorCamNF = {"Camera not found", 1};

	void errorExit(CrppError err);
}

#endif
