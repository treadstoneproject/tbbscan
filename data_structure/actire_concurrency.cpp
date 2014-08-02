/*
* Copyright 2014 Chatsiri Rattana.
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

/*  Titles																								Authors					 Date
 * Goto/Failure function.
 * Sig Engine.
 * Scan Engine.                                        R.Chatsiri      18/06/2014
 *
 */

#include "tbbscan/data_structure/actire_concurrency.hpp"

namespace tbbscan
{
    //_____________________ Goto_function _________________________________
    template<typename SymbolT, bool AllocatorMemType>
    goto_function<SymbolT, AllocatorMemType>::
    goto_function() { }

    template<typename SymbolT, bool AllocatorMemType>
    bool goto_function<SymbolT, AllocatorMemType>::
    create_goto(std::vector<struct utils::meta_sig *> *msig_vec,
            output_function_type&    output_fn)
    {

        logger->write_info("Sig-Engine, Create Goto Function support ",
                hnmav_util::format_type::type_center);

        std::size_t new_state = 0;
        std::size_t  kw_index  = 0;

        if(msig_vec->empty())
            return false;

        std::vector<struct utils::meta_sig *>::iterator iter_msig_vec;

        for(iter_msig_vec = msig_vec->begin();
                iter_msig_vec != msig_vec->end();
                ++iter_msig_vec) {

            struct utils::meta_sig *msig_ptr = *iter_msig_vec;

            //logger->write_info_test("Sig-Engine, Load Signature ",
            //        boost::lexical_cast<std::string>(msig_ptr->sig));

            enter_node(msig_ptr, new_state);

            msig_ptr->keyword_index = kw_index++;
            output_fn[new_state].insert(msig_ptr);
        }//for


        //logger->write_info_test("Sig-Engine, Output function size :",
        //        boost::lexical_cast<std::string>(output_fn.size()));

        return true;
    }

    template<typename SymbolT, bool AllocatorMemType>
    std::size_t goto_function<SymbolT, AllocatorMemType>::
    operator()(std::size_t  state, SymbolT const& symbol) const
    {

        edges_type  const& node = graph_[state];

        typename edges_type::const_iterator const& edge_it = node.find(symbol);

        if(edge_it != node.end()) {
            struct utils::meta_sig msig = edge_it->second;
            return msig.state;
        } else {
            return (state == 0) ? 0 : AC_FAIL_STATE;
        }
    }


    template<typename SymbolT, bool AllocatorMemType>
    void goto_function<SymbolT, AllocatorMemType>::
    enter_node(struct utils::meta_sig *msig_ptr, std::size_t&   new_state)
    {
        std::size_t state = 0;
        uint32_t index = 0;
        edges_type *node;
        struct utils::meta_sig msig_internal;

        for(; index < msig_ptr->sig_size; index++) {
            if(state == graph_.size()) {
                graph_.resize(state + 1);
            }//if

            node = &graph_[state];

            typename edges_type::iterator edge = node->find(msig_ptr->sig[index]);

            if(edge == node->end()) {
                break;
            }//if

            state = edge->second.state;

        }//for

        graph_.resize(graph_.size() + msig_ptr->sig_size - index);
        node = &graph_[state];

        for(; index < msig_ptr->sig_size; index++) {
            (*node)[msig_ptr->sig[index]].state = ++new_state;
            state = new_state;
            node  = &graph_[state];
        }

    }

    template class goto_function<char, true>;

    //_____________________ Failure_function _________________________________
    template<typename SymbolT, bool AllocatorMemType>
    bool failure_function<SymbolT, AllocatorMemType>::
    create_failure(goto_function<SymbolT, AllocatorMemType> const& _goto,
            output_function_type&    output)
    {
        logger->write_info("Sig-Engine, Create Failure Function support ",
                hnmav_util::format_type::type_center);

        if(output.empty())
            return false;

        std::deque<std::size_t>  queue;
        //Queue
        queue_edges(_goto.get_nodes()[0], queue);

        //While
        while(!queue.empty()) {
            std::size_t  r = queue.front();

            queue.pop_front();

            typename goto_function<SymbolT, AllocatorMemType>::edges_type const& node(_goto.get_nodes()[r]);
            typename goto_function<SymbolT, AllocatorMemType>::edges_type::const_iterator edge_it;

            for(edge_it = node.begin(); edge_it != node.end(); ++edge_it) {
                std::pair<SymbolT, struct utils::meta_sig> const& edge(*edge_it);

                SymbolT const& symbol_sig					= edge.first;
                struct utils::meta_sig  state_sig = edge.second;


                queue.push_back(state_sig.state);
                std::size_t state = table_[r];



                while(_goto(state, symbol_sig) == AC_FAIL_STATE) {
                    state  = table_[state];
                }//while

                table_[state_sig.state] = _goto(state, symbol_sig);

                std::size_t state_table = state_sig.state;

                output[state_table].insert(
                        output[table_[state_sig.state]].begin(),
                        output[table_[state_sig.state]].end());

            }//for

        }//while

        return true;
    }

    template<typename SymbolT, bool AllocatorMemType>
    void failure_function<SymbolT, AllocatorMemType>::
    queue_edges(typename goto_function<SymbolT, AllocatorMemType>::edges_type const& node,
            std::deque<std::size_t>&   queue)
    {
        typename goto_function<SymbolT, AllocatorMemType>::edges_type::const_iterator  edge_it;

        for(edge_it = node.begin(); edge_it != node.end(); ++edge_it) {
            std::pair<SymbolT, struct meta_sig> const& edge(*edge_it);
            queue.push_back(edge.second.state);
            table_[edge.second.state] = 0;
        }
    }

    template class failure_function<char, true>;

    //_____________________________ Actire_sig_engine __________________________________

    template<typename SymbolT, bool AllocatorMemType>
    bool actire_sig_engine<SymbolT, AllocatorMemType>::
    create_engine(std::vector<struct meta_sig *> _msig_vec,
            utils::filetype_code filetype)
    {
        logger->write_info("Sig-Engine, Start Sig-Engine ", hnmav_util::format_type::type_center);

        typename std::vector<struct meta_sig *>::iterator iter_msig_vec;

        if(_msig_vec.empty())
            return false;

        for(iter_msig_vec = _msig_vec.begin();
                iter_msig_vec != _msig_vec.end();
                ++iter_msig_vec) {
            struct utils::meta_sig *msig = *iter_msig_vec;

            if(filetype == msig->sig_type)
                msig_vec_.push_back(msig);
        }

        //initial goto function;
        goto_fn = new goto_function<SymbolT, AllocatorMemType>();

        if(!goto_fn->create_goto(&msig_vec_, output_fn)) {
            return false;
        }

        //initial failure function
        failure_fn = new failure_function<SymbolT, AllocatorMemType>(*goto_fn);

        if(!failure_fn->create_failure(*goto_fn, output_fn)) {
            return false;
        }

        return true;
    }

    template<typename SymbolT, bool AllocatorMemType>
    bool actire_sig_engine<SymbolT, AllocatorMemType>::
    filter_sig_support(struct meta_sig *msig)
    {

    }

    template class actire_sig_engine<char, true>;

    //___________________ Actire_PE_engine _____________________________
    template<typename SymbolT, bool AllocatorMemType>
    bool actire_pe_engine<SymbolT, AllocatorMemType>::
    hex_view_pos(uint64_t start_point_found,
            uint64_t end_point_found)
    {


    }


    template<typename SymbolT, bool AllocatorMemType>
    bool actire_pe_engine<SymbolT, AllocatorMemType>::
    search_parallel(goto_function<SymbolT, AllocatorMemType>& goto_fn,
            failure_function<SymbolT, AllocatorMemType>& failure_fn,
            output_function_type& output_fn,
            struct result_callback<std::vector<std::string> >& result_callback,
            uint64_t start_point_scan,
            uint64_t end_point_scan,
            const char *file_name,
            tbb::concurrent_vector<char> *binary_hex_input)
    {
        std::size_t state_  = 0;
        std::size_t where_  = 0;
        file_name_ = file_name;

        
        for(int index = start_point_scan; index < end_point_scan; index++) {
            char const& input = binary_hex_input->at(index);
            {
                std::size_t next;

                while((next = goto_fn(state_, input)) == AC_FAIL_STATE) {
                    state_ = failure_fn(state_);
                }//while

                state_ = next;
            } // input
            {

                std::set<struct meta_sig *> msig_vec = output_fn[state_];
                typename std::set<struct meta_sig *>::iterator iter_msig_vec;

                for(iter_msig_vec = msig_vec.begin();
                        iter_msig_vec != msig_vec.end();
                        ++iter_msig_vec) {
                    struct utils::meta_sig *msig = *iter_msig_vec;
                    msig->file_name = file_name_;// Set file name found;
                    //TODO: Plan-00004 : Callback not support with multithread concurrency.
                    //result_callback.infected_file(msig);
                    logger->write_info("Search Result", hnmav_util::format_type::type_center);

                    logger->write_info("Search-Parallel Engine. Infected File",
                            boost::lexical_cast<std::string>(msig->file_name));

                    logger->write_info("Search-Parallel Engine. Found Virus name",
                            boost::lexical_cast<std::string>(msig->virname));

                    logger->write_info("Search-Parallel Engine, Sig matching   ",
                            boost::lexical_cast<std::string>(msig->sig));

                    logger->write_info("Infected file found on search-engine. Done!");
                }
            }//for
        }//In check data.

        return true;
    }//for



    template class actire_pe_engine<char, true>;

    //________________________ Actire_Engine_Factory ______________________________________
    template<typename SymbolT, bool AllocatorMemType>
    typename actire_engine_factory<SymbolT, AllocatorMemType>::callback_map
    actire_engine_factory<SymbolT, AllocatorMemType>::mapActireEngine;

    //ACtire concurrency engine factory.
    template<typename SymbolT, bool AllocatorMemType>
    void actire_engine_factory<SymbolT, AllocatorMemType>::
    register_actire_type(const std::string& type, create_callback cb)
    {
        mapActireEngine[type] = cb;
    }

    //ACtire concurrency engine factory.
    template<typename SymbolT, bool AllocatorMemType>
    void actire_engine_factory<SymbolT, AllocatorMemType>::
    unregister_actire_type(const std::string& type)
    {
        mapActireEngine.erase(type);
    }

    template<typename SymbolT, bool AllocatorMemType>
    iactire_engine<SymbolT, AllocatorMemType> *actire_engine_factory<SymbolT, AllocatorMemType>::
    create_actire_engine(const std::string& type)
    {
        typename callback_map::iterator iter_engine =   mapActireEngine.find(type);

        if(iter_engine != mapActireEngine.end())
            return (iter_engine->second)();
    }

    template class actire_engine_factory<char, true>;

}//namespace
