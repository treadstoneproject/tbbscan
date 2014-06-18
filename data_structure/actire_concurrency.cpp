#include "tbbscan/data_structure/actire_concurrency.hpp"

namespace controller
{
    //_____________________ Goto_function _________________________________
    template<typename SymbolT, typename AllocatorMemType>
    goto_function<SymbolT, AllocatorMemType>::
    goto_function(std::vector<struct utils::meta_sig *> *msig_vec,
            output_function_type&   output_fn)
    {
        std::size_t new_state = 0;
        std::size_t  kw_index  = 0;

        std::vector<struct utils::meta_sig *>::iterator iter_msig_vec;

        for(iter_msig_vec = msig_vec->begin();
                iter_msig_vec != msig_vec->end();
                ++iter_msig_vec) {

            struct utils::meta_sig *msig_ptr = *iter_msig_vec;

            const char *sig_kw_ptr = msig_ptr->sig;
            enter_node(sig_kw_ptr, new_state);
            output_fn[new_state].insert(kw_index++);


        }//for

    }

    template<typename SymbolT, typename AllocatorMemType>
    size::size_t goto_function<SymbolT, AllocatorMemType>::
    operator(std::size_t  state, SymbolT& symbol) const
    {

        edges_type  const node = graph_[state];

        typename edges_type::const_iteartor const& edge_it = node.find(symbol);

        if(edge_it != node.end()) {
            struct utils::node_str *nstr = edge_it->second;
            return nstr->state;
        } else {
            return (state == 0) ? 0 : AC_FAIL_STATE;
        }
    }


    template<typename SymbolT, typename AllocatorMemType>
    void goto_function<SymbolT, AllocatorMemType>::
    enter_node(struct utils::meta_sig * msig, std::size_t  new_state)
    {
				std::size_t state = 0;
				edges_type * node;
				struct node_str * nstr;
        for(uint32_t count_sig = 0; count_sig < msig_ptr->sig_size; count_sig++) {
					  if(state == graph_.size())
						{	
								graph_.resize(state + 1);
						}//if
					
					  node = &graph_[state];

						typename edges_type::iterator edge = node->find(msig_ptr->sig[count_sig]);
						nstr = edge->second;
						if(edge == node->end())
						{
							break;
						}//if

						state = nstr->state;

        }//for

				graph_.resize(graph_.size() + msig_ptr->sig_size; index++)
				node = &graph_[state];
			
				for(uint32_t count_sig = 0; count_sig < msig_ptr->sig_size; count_sig++)
				{
						nstr = (*node)[msig_ptr->sig[count_sig]];
						nstr->state = ++new_state;
			
						state = new_state;
						node  = &graph_[state];			
				} 

    }

};
