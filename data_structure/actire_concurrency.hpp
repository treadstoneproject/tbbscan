#ifndef TBBSCAN_ACTIRE_CONCURRENCY_HPP
#define TBBSCAN_ACTIRE_CONCURRENCY_HPP

#include "tbb/concurrent_unordered_map.h"
#include "tbb/concurrent_vector.h"

#include "utils/base/system_code.hpp"
#include "utils/base/common.hpp"

namespace tbbscan
{

    static size_t const AC_FAIL_STATE = -1;

    const bool tbb_allocator = true;
    const bool std_allocator = false;

    using utils::meta_sig;

    template<typename SymbolT, bool AllocatorMemType>
    class goto_function;


    //Make Goto Function.
    template<typename SymbolT, bool AllocatorMemType>
    class goto_function
    {

        public:
            typedef tbb::concurrent_unordered_map<std::size_t, std::vector<struct utils::meta_sig *> >
                    output_function_type;

            typedef tbb::concurrent_unordered_map<SymbolT, struct meta_sig>  edges_type;

            typedef tbb::concurrent_vector<edges_type>  node_type;

            goto_function();

            bool create_goto(std::vector<struct utils::meta_sig *> *msig_vec,
                    output_function_type& output_fn);

            std::size_t operator()(std::size_t state, SymbolT const& symbol) const;

            void enter_node(struct utils::meta_sig *msig, std::size_t& new_state);

            node_type const& get_nodes() const {
                return graph_;
            }

            ~goto_function() { };

        private:

            node_type graph_;

    };



    //Make failure Function.
    template<typename SymbolT, bool AllocatorMemType>
    class failure_function
    {
        public:

            typedef tbb::concurrent_unordered_map<std::size_t, std::vector<struct utils::meta_sig *> >
                    output_function_type;

            failure_function(goto_function<SymbolT, AllocatorMemType> const& _goto) :
                table_(_goto.get_nodes().size(), AC_FAIL_STATE) {

            }

            bool create_failure(goto_function<SymbolT, AllocatorMemType> const& _goto,
                    output_function_type&    output);

            std::size_t operator()(std::size_t  state)const {
                return table_[state];
            }


            tbb::concurrent_vector<std::size_t>& get_table() {
                return table_;
            }

            inline void queue_edges(
                    typename goto_function<SymbolT, AllocatorMemType>::edges_type const& node,
                    std::deque<std::size_t>&   queue);


        private:

            tbb::concurrent_vector<std::size_t> table_;

    };

    //Signature Engine.
    template<typename SymbolT, bool AllocatorMemType>
    class actire_sig_engine
    {

        public:
            typedef tbb::concurrent_unordered_map<std::size_t,
                     std::vector<struct utils::meta_sig *> >  output_function_type;

            bool create_engine(std::vector<struct meta_sig *> meta_sig_vec, utils::filetype_code filetype);

            bool filter_sig_support(struct meta_sig *msig);

            goto_function<SymbolT, AllocatorMemType>& get_goto_fn() {
                return *goto_fn;
            }

            failure_function<SymbolT, AllocatorMemType>& get_failure_fn() {
                return *failure_fn;
            }
	
						 output_function_type & get_output_fn(){
								return output_fn;
						}

            /*
            create_goto(msig_vec, output_fn);
            create_failure(goto_fn, output_fn);
            */
        private:
            std::vector<struct meta_sig *> msig_vec_;
            goto_function<SymbolT, AllocatorMemType> *goto_fn;
            failure_function<SymbolT, AllocatorMemType> *failure_fn;
            output_function_type output_fn;
    };


}



//Make Search Engine.

//Make controller for signature and search engine.

#endif /* TBBSCAN_ACTIRE_CONCURRENCY_HPP */
