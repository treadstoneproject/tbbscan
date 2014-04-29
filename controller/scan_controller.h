#ifndef SCAN_TBBSCAN_CONTROLLER_H
#define SCAN_TBBSCAN_CONTROLLER_H

namespace tbb_task{

	template<typename FileController>
	class base_scan{
	public:
		std::vector<FileController> get_file_controller();
	};

	template<typename FileController>
	class scan_controller{

		bool add_file_type(std::vector<FileController>  & file_controller);

	};

}


#endif 