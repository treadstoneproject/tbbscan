# debug, optimized
AM_CXXFLAGS   = -I$(top_srcdir)/src/ @AM_CXXFLAGS@

# Make src file 
# libarries for scanning with TBB(Concurren Sig, Parallel Searching)
lib_LIBRARIES = libtbbscan.a

#@install header to system
libtbbscan_adir = $(includedir)/tbbscan

#Header
libtbbscan_a_HEADERS = data_structure/actire_concurrency.hpp \
data_structure/bloomfliter_concurrency.hpp \
data_structure/filter_policy.hpp \
data_structure/slice.hpp

# library
libtbbscan_a_SOURCES = $(libtbbscan_a_HEADERS)  data_structure/actire_concurrency.cpp \
data_structure/bloomfliter_concurrency.cpp \
data_structure/filter_policy.cpp
