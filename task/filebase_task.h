#ifndef TBBSCAN_FILEBASE_TASK_HPP
#define TBBSCAN_FILEBASE_TASK_HPP

#include "tbb/task.h"

namespace tbbscan{

	template<typename use_tbbmalloc, typename mappfile_structure>
	class filebase_task : public tbb::task {
	public:

		virtual mappfile_structure * pre_filter() = 0;

		virtual mappfile_structure * post_filter() = 0;

		virtual tbb::task * execute() = 0;

	};

}

#endif
