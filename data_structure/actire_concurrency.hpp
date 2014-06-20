#ifndef TBBSCAN_ACTIRE_CONCURRENCY_HPP
#define TBBSCAN_ACTIRE_CONCURRENCY_HPP

/*
* Copyright 2014 MTSec, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*  Titles			                                          Authors	         Date
 * Goto/Failure function.
 * Sig Engine.
 * Scan Engine.                                        R.Chatsiri      18/06/2014
 * 
 */

#include "tbb/concurrent_unordered_map.h"
#include "tbb/concurrent_vector.h"

#include "utils/logger/clutil_logger.hpp"
#include "utils/base/system_code.hpp"
#include "utils/base/common.hpp"

namespace tbbscan
{

    static size_t const AC_FAIL_STATE = -1;

    const bool tbb_allocator = true;
    const bool std_allocator = false;

    namespace h_util = hnmav_util;
    using utils::meta_sig;

    template<typename SymbolT, bool AllocatorMemType>
    class goto_function;


    //Make Goto Function.
    template<typename SymbolT, bool AllocatorMemType>
    class goto_function
    {

        public:
            typedef tbb::concurrent_unordered_map<std::size_t, std::set<struct utils::meta_sig *> >
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

//logger
            boost::shared_ptr<h_util::clutil_logging<std::string, int> > *logger_ptr;
            h_util::clutil_logging<std::string, int>    *logger;
    };



    //Make failure Function.
    template<typename SymbolT, bool AllocatorMemType>
    class failure_function
    {
        public:

            typedef tbb::concurrent_unordered_map<std::size_t, std::set<struct utils::meta_sig *> >
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

//logger
            boost::shared_ptr<h_util::clutil_logging<std::string, int> > *logger_ptr;
            h_util::clutil_logging<std::string, int>    *logger;
    };
    
    //Signature Engine.
    template<typename SymbolT, bool AllocatorMemType>
    class actire_sig_engine
    {

    public:
        typedef tbb::concurrent_unordered_map<std::size_t,
                std::set<struct utils::meta_sig *> >  output_function_type;

        bool create_engine(std::vector<struct meta_sig *> meta_sig_vec, utils::filetype_code filetype);

        bool filter_sig_support(struct meta_sig *msig);

        goto_function<SymbolT, AllocatorMemType>& get_goto_fn() {
            return *goto_fn;
        }

        failure_function<SymbolT, AllocatorMemType>& get_failure_fn() {
            return *failure_fn;
        }

        output_function_type& get_output_fn() {
            return output_fn;
        }

       private:
        std::vector<struct meta_sig *> msig_vec_;
        goto_function<SymbolT, AllocatorMemType> *goto_fn;
        failure_function<SymbolT, AllocatorMemType> *failure_fn;
        output_function_type output_fn;

//logger
            boost::shared_ptr<h_util::clutil_logging<std::string, int> > *logger_ptr;
            h_util::clutil_logging<std::string, int>    *logger;
    };
    
    //________________________ Result Callback __________________________________
    template<typename KeywordResult>
    struct result_callback {

        result_callback(KeywordResult const& keywords, bool summary = false):
            keywords_(keywords),
            summary_(summary) {


        }

        void operator()(std::size_t what,
                std::size_t where,
                struct utils::meta_sig *msig) {

            if(!summary_) {

								logger->write_info("Search Result", hnmav_util::format_type::type_center);

								logger->write_info("Search-Parallel Engine. Found Virus name", 
										boost::lexical_cast<std::string>(msig->virname));
								logger->write_info("Search-Parallel Engine, Sig matching   ",
                    boost::lexical_cast<std::string>(msig->sig));
								//Vector contains result.
                msig_result_vec.push_back(msig);

            } else {
                std::cout<<".";
            }
        }

        std::vector<struct meta_sig *>&   get_msig_result_vec() {
            return msig_result_vec;
        }

        KeywordResult const& keywords_;
        std::vector<struct meta_sig *> msig_result_vec;
        bool summary_;


//logger
            boost::shared_ptr<h_util::clutil_logging<std::string, int> > *logger_ptr;
            h_util::clutil_logging<std::string, int>    *logger;

    };

    //Make Search Engine.
    /*
    //_________________________ Actire search engine ____________________________
    template<typename SymbolT, bool AllocatorMemType>
    class actire_engine_factory
    {

    public:

        //typedef iactire_engine *(*create_callback)();

        static void register_actire_type(const std::string& type,  create_callback cb);

        static void unregister_actire_type(const std::string& type);

        static iactire_engine *create_actire_engine(const std::string& type);
        */
    //private:

    //typedef std::map<std::string, create_callback> callback_map;
    //static callback_map mapActireEngine;
    //  };

    template<typename SymbolT, bool AllocatorMemType>
    class iactire_engine
    {
        public:
            typedef tbb::concurrent_unordered_map<std::size_t, std::set<struct utils::meta_sig *> >
                    output_function_type;

            virtual bool hex_view_pos(uint64_t start_point_found,
                    uint64_t end_point_found) = 0;

            virtual bool search_parallel(goto_function<SymbolT, AllocatorMemType>& goto_fn,
                    failure_function<SymbolT, AllocatorMemType>& failure_fn,
                    output_function_type& output_fn,
                    struct result_callback<std::vector<std::string> >& result_callback,
                    uint64_t start_point_scan,
                    uint64_t end_point_scan,
                    const char *file_name,
                    tbb::concurrent_vector<char> *binary_hex_input) = 0;

    };


    template<typename SymbolT, bool AllocatorMemType>
class actire_pe_engine : public iactire_engine<SymbolT, AllocatorMemType>
    {
        public:

            typedef tbb::concurrent_unordered_map<std::size_t, std::set<struct utils::meta_sig *> >
                    output_function_type;


            virtual bool hex_view_pos(uint64_t start_point_found,
                    uint64_t end_point_found);

            virtual bool search_parallel(goto_function<SymbolT, AllocatorMemType>& goto_fn,
                    failure_function<SymbolT, AllocatorMemType>& failure_fn,
                    output_function_type& output_fn,
                    struct result_callback<std::vector<std::string> >& result_callback,
                    uint64_t start_point_scan,
                    uint64_t end_point_scan,
                    const char *file_name,
                    tbb::concurrent_vector<char> *binary_hex_input);

//logger
            boost::shared_ptr<h_util::clutil_logging<std::string, int> > *logger_ptr;
            h_util::clutil_logging<std::string, int>    *logger;
    };


}// namespace


//[] Make controller for signature and search engine.

#endif /* TBBSCAN_ACTIRE_CONCURRENCY_HPP */
