#ifndef CONTROLLER_ACTIRE_CONCURRENCY_HPP
#define CONTROLLER_ACTIRE_CONCURRENCY_HPP

#include "tbb/concurrent_unordered_map.h"

const bool tbbmalloc = true;
const bool stdmalloc = false;

namespace controller{

//Make Goto Function.
template<typename SymbolT, typename AllocatorMemType>
class goto_function
{
		
		public:
		typedef tbb::concurrent_unordered_map<std::size_t, std::vector<struct node_str*> >  output_function_type;

		typedef tbb::concurrent_unordered_map<SymbolT, struct node_str* >  edges_type;

		typedef tbb::concurrent_vector<edges_type>  node_type;

		goto_function(std::vector<struct utils::meta_sig*> msig_vec, output_function_type & output_fn);

		std::size_t operator()(std::size_t state, SymbolT & symbol) const;

		void enter_node(struct utils::meta_sig * msig, std::size_t  new_state);

		~goto_function();

		private:

		node_type graph_; 

};


}




//Make failure Function.


//Make Failure Function.


//Signature Engine.


#endif /* CONTROLLER_ACTIRE_CONCURRENCY_HPP */


