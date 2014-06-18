# debug, optimized
AM_CXXFLAGS   = -I$(top_srcdir)/src/ @AM_CXXFLAGS@

# Make src file 
# libarries for scanning with TBB(Concurren Sig, Parallel Searching)
lib_LIBRARIES = libtbbscan.a

#@install header to system
libtbbscan_adir = $(includedir)/tbbscan

#Header
libtbbscan_a_HEADERS = data_structure/actire_concurrency.hpp

# library
libtbbscan_a_SOURCES = $(libtbbscan_a_HEADERS)  data_structure/actire_concurrency.hpp

