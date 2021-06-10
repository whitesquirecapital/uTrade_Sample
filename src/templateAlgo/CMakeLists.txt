add_library( templateAlgo MODULE
	../wscCommon/sysZTime.cpp
	types.cpp
	externalInterface.cpp
	template.cpp
)
include_directories(../common)
include_directories(../wscCommon)
