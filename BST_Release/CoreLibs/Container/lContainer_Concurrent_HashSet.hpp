/* Description: Hash set class modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_CONCURRENT_HASHSET
#define LIB_CONTAINER_CONCURRENT_HASHSET

#include "lGeneral.hpp"
#include "lContainer_HashSet.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	namespace Concurrent{
		template <typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>, typename A = std::allocator<T> > class HashSet
			:protected nsContainer::HashSet<T, Hash, Pred, A>{
		public:
			// custom typedefs
			typedef HashSet<T, Hash, Pred, A>	this_type;

			// container typedefs, mandated by the STL standard
			typedef typename A::template rebind< HashSetNode<T> >::other	allocator_type; //diffrent from the C++ STL standard.
			typedef typename T												value_type;
			typedef typename T&												reference;
			typedef typename const T&										const_reference;
			typedef typename T*												pointer;
			typedef typename const T*										const_pointer;
			typedef typename size_t											size_type;
			typedef typename ptrdiff_t										difference_type;

			// container typedefs specified for this container, mandated by the STL standard
			typedef typename T					key_type;
			typedef typename Hash				hasher;

			// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		protected:
			nsBasic::RWLock SyncRoot;

		public:
			//constructors
			template<typename ... Args> HashSet(Args... args) :nsContainer::HashSet<T, Hash, Pred, A>(args...){
				this->SyncRoot.Initialize();
			}

			//concurrent read/write functions
			inline const nsContainer::HashSet<T, Hash, Pred, A> *lock_read(){
				this->SyncRoot.Enter_Read();
				return static_cast<const nsContainer::HashSet<T, Hash, Pred, A> *>(this);
			}
			inline void unlock_read(){ this->SyncRoot.Leave_Read(); }
			inline nsContainer::HashSet<T, Hash, Pred, A> *lock_write(){
				this->SyncRoot.Enter_Write();
				return static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this);
			}
			inline void unlock_write(){ this->SyncRoot.Leave_Write(); }

			//observer functions
			inline allocator_type get_allocator() const{ return Allocator; }

			//capacity functions
			//these two functions are questionable. But I don't have any better idea.
			//Anyway, a programmer should not rely on these capacity queries when he/she knows that this data structure is concurrent.
			inline bool empty() const { return NodeCount == 0; }
			inline size_type size() const { return NodeCount; }

			//modifiers
			inline void clear(){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->clear();
				this->SyncRoot.Leave_Write();
			}
			inline void rehash(size_t BucketCount){
				this->SyncRoot.Enter_Write();
				static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->rehash(BucketCount);
				this->SyncRoot.Leave_Write();
			}
			inline std::pair<value_type, bool> insert(const value_type& Value){
				this->SyncRoot.Enter_Write();
				auto RetValue = static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->insert(Value);
				this->SyncRoot.Leave_Write();
				return std::pair<value_type, bool>(*(RetValue.first), RetValue.second);
			}
			template< typename ... Args > inline std::pair<value_type, bool> emplace(Args&& ... args){
				this->SyncRoot.Enter_Write();
				auto RetValue = static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->emplace(args...);
				this->SyncRoot.Leave_Write();
				return std::pair<value_type, bool>(*(RetValue.first), RetValue.second);
			}
			size_type erase(const key_type& Key){
				this->SyncRoot.Enter_Write();
				auto RetValue = static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->erase(Key);
				this->SyncRoot.Leave_Write();
				return RetValue;
			}

			//query functions
			value_type *find(const key_type& Key){
				this->SyncRoot.Enter_Read();
				auto RetValue = static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->find(Key);
				this->SyncRoot.Leave_Read();
				if(RetValue == this->end())return nullptr; else return &(*RetValue);
			}
			const value_type *find(const key_type& Key) const{
				this->SyncRoot.Enter_Read();
				auto RetValue = static_cast<const nsContainer::HashSet<T, Hash, Pred, A> *>(this)->find(Key);
				this->SyncRoot.Leave_Read();
				if(RetValue == this->end())return nullptr; else return &(*RetValue);
			}

			//special query function
			value_type *exclusive_acquire(const key_type& Key){
				//returns a pointer to the value which corresponds to the given Key. Once the value is acquired, the value won't be seen from other threads.
				//This operation doesn't prevent other threads from inserting a same value.
				//the pointer must be returned before the destruction of the set.
				//if no corresponding value is found, nullptr is returned.
				this->SyncRoot.Enter_Write();

				if (BucketVector.empty()){
					this->SyncRoot.Leave_Write();
					return nullptr;
				}
				else{
					size_type BucketIndex = CurHasher(Key) % BucketVector.size();

					HashSetNode<T> **lpCurPtr = &BucketVector[BucketIndex];
					while (nullptr != *lpCurPtr){
						if (CurPredicate(Key, (*lpCurPtr)->Data)){
							HashSetNode<T> *CurPtr = *lpCurPtr;
							*lpCurPtr = (*lpCurPtr)->Next;

							NodeCount--;
							if (0 == NodeCount)static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->clear();
							this->SyncRoot.Leave_Write();
							return &CurPtr->Data;
						}
						lpCurPtr = &(*lpCurPtr)->Next;
					}
					this->SyncRoot.Leave_Write();
					return nullptr;
				}
			}
			void exclusive_release(const value_type *lpValue){
				//release a pointer acquired by exclusive_acquire(). If a same value is present in the set, the returned value will be released.
				//releasing a pointer of other origin may result in undefined behavior.

				if (nullptr != lpValue){
					this->SyncRoot.Enter_Write();

					size_type HashValue = CurHasher(*lpValue);
					size_type BucketIndex;

					HashSetNode<T> *NewNode = (HashSetNode<T> *)((UBINT)lpValue - offsetof(HashSetNode<T>, Data));

					if (!BucketVector.empty()){
						BucketIndex = HashValue	% BucketVector.size();
						HashSetNode<T> *CurPtr = BucketVector[BucketIndex];
						while (nullptr != CurPtr){
							if (CurPredicate(*lpValue, CurPtr->Data)){
								Allocator.destroy(&NewNode->Data); //since C++11
								Allocator.deallocate(NewNode, 1);
								this->SyncRoot.Leave_Write();
								return;
							}
							CurPtr = CurPtr->Next;
						}
					}

					if (NodeCount == BucketVector.size()){
						try{
							if (0 == NodeCount)static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->rehash(4); //bad
							else static_cast<nsContainer::HashSet<T, Hash, Pred, A> *>(this)->rehash(BucketVector.size() * 2); //very bad, need to repair this later
						}
						catch (...){
							Allocator.destroy(&NewNode->Data); //since C++11
							Allocator.deallocate(NewNode, 1);
							this->SyncRoot.Leave_Write();
							throw;
						}
						BucketIndex = HashValue	% BucketVector.size();
					}

					NewNode->Next = BucketVector[BucketIndex];
					BucketVector[BucketIndex] = NewNode;
					NodeCount++;

					this->SyncRoot.Leave_Write();
				}
			}

			~HashSet(){
				//all concurrent operations must be done before this is called!
				this->SyncRoot.Enter_Write();
				//and the lock will never be released.
			}
		};
	}
}

#endif