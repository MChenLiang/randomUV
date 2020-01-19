#pragma once
#include <maya/MStatus.h>

#define MCheckStatus(status,message)	\
		if( MStatus::kSuccess != status ) {	\
			cerr << message << "\n";		\
			return status;					\
		}