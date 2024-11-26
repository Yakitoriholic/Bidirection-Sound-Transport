/* Description: AVL Tree Implementation. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_BINARY_TREE
#define LIB_CONTAINER_BINARY_TREE

#include "lGeneral.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	template < typename node_type > const node_type *binary_tree_prev(const node_type *pos){
		if (nullptr != pos->Child[0]){
			pos = static_cast<const node_type *>(pos->Child[0]);
			while (nullptr != pos->Child[1])pos = static_cast<const node_type *>(pos->Child[1]);
			return pos;
		}
		else{
			while (true){
				if (nullptr == pos->Parent)return nullptr;
				else if (static_cast<const node_type *>(pos->Parent->Child[1]) == pos)return static_cast<const node_type *>(pos->Parent);
				else pos = static_cast<const node_type *>(pos->Parent);
			}
		}
	}
	template < typename node_type > node_type *binary_tree_prev(node_type *pos){
		if (nullptr != pos->Child[0]){
			pos = static_cast<const node_type *>(pos->Child[0]);
			while (nullptr != pos->Child[1])pos = static_cast<const node_type *>(pos->Child[1]);
			return pos;
		}
		else{
			while (true){
				if (nullptr == pos->Parent)return nullptr;
				else if (static_cast<const node_type *>(pos->Parent->Child[1]) == pos)return static_cast<const node_type *>(pos->Parent);
				else pos = static_cast<const node_type *>(pos->Parent);
			}
		}
	}
	template < typename node_type > const node_type *binary_tree_next(const node_type *pos){
		if (nullptr != pos->Child[1]){
			pos = static_cast<const node_type *>(pos->Child[1]);
			while (nullptr != pos->Child[0])pos = static_cast<const node_type *>(pos->Child[0]);
			return pos;
		}
		else{
			while (true){
				if (nullptr == pos->Parent)return nullptr;
				else if (static_cast<const node_type *>(pos->Parent->Child[0]) == pos)return static_cast<const node_type *>(pos->Parent);
				else pos = static_cast<const node_type *>(pos->Parent);
			}
		}
	}
	template < typename node_type > node_type *binary_tree_next(node_type *pos){
		if (nullptr != pos->Child[1]){
			pos = static_cast<const node_type *>(pos->Child[1]);
			while (nullptr != pos->Child[0])pos = static_cast<const node_type *>(pos->Child[0]);
			return pos;
		}
		else{
			while (true){
				if (nullptr == pos->Parent)return nullptr;
				else if (static_cast<const node_type *>(pos->Parent->Child[0]) == pos)return static_cast<const node_type *>(pos->Parent);
				else pos = static_cast<const node_type *>(pos->Parent);
			}
		}
	}
}

#endif