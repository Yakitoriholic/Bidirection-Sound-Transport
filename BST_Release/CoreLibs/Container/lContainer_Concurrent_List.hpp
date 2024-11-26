/* Description: List class modelled after STL with synchronization. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_CONCURRENT_LIST
#define LIB_CONTAINER_CONCURRENT_LIST

#include "lGeneral.hpp"
#include "lContainer_List.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	namespace Concurrent{
		template < typename T, typename A = std::allocator<T> > class List :protected nsContainer::List<T, A>{
			//without spaces, > > will be considered as an operator instead of two brackets.
		public:
			// custom typedefs
			typedef List<T, A>				this_type;
			typedef ListNode<T>				node_type;

			// container typedefs, mandated by the STL standard
			typedef typename A::template rebind<node_type>::other	allocator_type; //diffrent from the C++ STL standard.
			typedef typename T										value_type;
			typedef typename T&										reference;
			typedef typename const T&								const_reference;
			typedef typename T*										pointer;
			typedef typename const T*								const_pointer;
			typedef typename size_t									size_type;
			typedef typename ptrdiff_t								difference_type;

		protected:
			nsBasic::RWLock SyncRoot;

		public:
			template<typename ... Args> List(Args... args) :nsContainer::List<T, A>(args...){
				this->SyncRoot.Initialize();
			}

			//concurrent read/write functions
			inline const nsContainer::List<T, A> *lock_read(){
				this->SyncRoot.Enter_Read();
				return static_cast<const nsContainer::List<T, A> *>(this);
			}
			inline void unlock_read(){ this->SyncRoot.Leave_Read(); }
			inline nsContainer::List<T, A> *lock_write(){
				this->SyncRoot.Enter_Write();
				return static_cast<nsContainer::List<T, A> *>(this);
			}
			inline void unlock_write(){ this->SyncRoot.Leave_Write(); }

			//observer functions
			inline allocator_type get_allocator() const { return Allocator; }

			//capacity functions
			inline bool empty() const {
				this->SyncRoot.Enter_Read();
				bool RetValue = static_cast<const nsContainer::List<T, A> *>(this)->empty();
				this->SyncRoot.Leave_Read();
				return RetValue;
			}
			inline size_type size() const {
				this->SyncRoot.Enter_Read();
				size_type RetValue = static_cast<const nsContainer::List<T, A> *>(this)->size();
				this->SyncRoot.Leave_Read();
				return RetValue;
			}

			//modifiers
			void clear(){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->clear();
				this->SyncRoot.Leave_Write();
			}
			iterator insert(const_iterator Pos, const_reference Value){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->insert(Pos, Value);
				this->SyncRoot.Leave_Write();
			}
			iterator erase(const_iterator Pos){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->erase(Pos);
				this->SyncRoot.Leave_Write();
			}
			void push_back(const_reference Value){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->push_back(Value);
				this->SyncRoot.Leave_Write();
			}
			template <typename ... Args> void emplace_back(Args&& ... args){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->emplace_back(args...);
				this->SyncRoot.Leave_Write();
			}
			void pop_back(){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->pop_back();
				this->SyncRoot.Leave_Write();
			}
			void push_front(const_reference Value){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->push_front(Value);
				this->SyncRoot.Leave_Write();
			}
			template <typename ... Args> void emplace_front(Args&& ... args){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->emplace_front(args...);
				this->SyncRoot.Leave_Write();
			}
			void pop_front(){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::List<T, A> *>(this)->pop_front();
				this->SyncRoot.Leave_Write();
			}

			//operations
			void splice(const_iterator pos, this_type& other, const_iterator it){
				this->SyncRoot.Enter_Write();
				other.SyncRoot.Enter_Write();
				static_cast<const nsContainer::List<T, A> *>(this)->splice(pos, static_cast<const nsContainer::List<T, A>&>(other), it);
				this->SyncRoot.Leave_Write();
				other.SyncRoot.Leave_Write();
			}

			//special query function
			value_type *exclusive_acquire(const_iterator it){
				//returns a pointer to the value which corresponds to the iterator. Once the value is acquired, the value won't be seen from other threads.
				//the pointer must be returned before the destruction of the list.
				this->SyncRoot.Enter_Write();

				node_type *CurNode = const_cast<node_type *>(it.lpNode);
				node_type *PrevNode = CurNode->Prev;
				node_type *NextNode = CurNode->Next;
				NextNode->Prev = PrevNode;
				PrevNode->Next = NextNode;
				
				this->SyncRoot.Leave_Write();
				return &CurNode->Data;
			}
			void exclusive_release(const value_type *lpValue){
				//release a pointer acquired by exclusive_acquire().
				//releasing a pointer of other origin may result in undefined behavior.
				this->SyncRoot.Enter_Write();

				ListNode<T> *NewNode = (ListNode<T> *)((UBINT)lpValue - offsetof(ListNode<T>, Data));

				NewNode->Next = static_cast<node_type *>(&EndPtr);
				NewNode->Prev = EndPtr.Prev;
				EndPtr.Prev->Next = NewNode;
				EndPtr.Prev = NewNode;

				this->SyncRoot.Leave_Write();
			}

			inline ~List(){
				//all concurrent operations must be done before this is called!
				this->SyncRoot.Enter_Write();
				//and the lock will never be released.
				static_cast<nsContainer::List<T, A> *>(this)->clear();
			}
		};
	}
}

#endif