/* Description: Hash set class modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_HASHSET
#define LIB_CONTAINER_HASHSET

#include "lContainer_Hash.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	template <typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>, typename A = std::allocator<T> > class HashSet;

	template < typename T > struct HashSetNode{
		HashSetNode*	Next;
		T				Data;
	};

	template < typename T, typename Hash, typename Pred, typename A  > struct HashSetIterator_Const{
		// custom typedefs
		typedef HashSetIterator_Const<T, Hash, Pred, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag iterator_category; //#0

		typedef typename T			value_type; //#1
		typedef typename ptrdiff_t	difference_type; //#2
		typedef typename const T*	pointer; //#3
		typedef typename const T&	reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		const HashSetNode<T>* lpNode;
		const HashSet<T, Hash, Pred, A>* lpHashSet;

		HashSetIterator_Const(const HashSetNode<T>* Node, const HashSet<T, Hash, Pred, A>* const HashSet) : lpNode(Node), lpHashSet(HashSet){}
		HashSetIterator_Const() {}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		this_type& operator++(){
			if (nullptr == lpNode->Next){
				size_t BucketIndex = lpHashSet->CurHasher(lpNode->Data) % lpHashSet->BucketVector.size();
				lpNode = nullptr;
				do{
					BucketIndex++;
					if (BucketIndex >= lpHashSet->BucketVector.size())break;
					lpNode = lpHashSet->BucketVector[BucketIndex];
				} while (nullptr == lpNode);
			}
			else lpNode = lpNode->Next;
			return *this;
		}
		this_type operator++(int){
			this_type TmpIt = *this;
			++*this;
			return TmpIt;
		}
	};

	template < typename T, typename Hash, typename Pred, typename A > class HashSet{
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

		// container typedefs of iterator, mandated by the STL standard
		typedef HashSetIterator_Const<T, Hash, Pred, A>		iterator;
		typedef HashSetIterator_Const<T, Hash, Pred, A> const_iterator;

		friend struct HashSetIterator_Const<T, Hash, Pred, A>;

	protected:
		typedef typename A::template rebind< HashSetNode<T>* >::other	bucket_allocator;

		allocator_type	Allocator;
		hasher			CurHasher;
		Pred			CurPredicate;
		size_type		NodeCount;
		Vector<HashSetNode<T>*, bucket_allocator> BucketVector;

		//HashSet<T>& operator =(const HashSet<T>&) = delete;
	public:
		//constructors
		explicit HashSet(size_type BucketCount, const Hash& HashFunc = Hash(), const Pred& EqFunc = Pred(), const allocator_type& _Allocator = allocator_type())
			:Allocator(_Allocator), CurHasher(HashFunc), CurPredicate(EqFunc), BucketVector(BucketCount, nullptr){
			this->NodeCount = 0;
		}
		HashSet() :HashSet(0){}

		//observer functions
		inline allocator_type get_allocator() const{ return Allocator; }

		// iterator functions, mandated by the STL standard
		iterator begin(){
			for (size_type i = 0; i < BucketVector.size(); i++){
				if (nullptr != BucketVector[i])return iterator(BucketVector[i], this);
			}
			return iterator(nullptr, this);
		}
		inline iterator end(){ return iterator(nullptr, this); }

		//capacity functions
		inline bool empty() const { return NodeCount == 0; }
		inline size_type size() const { return NodeCount; }

		//modifiers
		void clear(){
			for (size_type i = 0; i < BucketVector.size(); i++){
				HashSetNode<T> *CurPtr = BucketVector[i];
				while (nullptr != CurPtr){
					HashSetNode<T> *NextPtr = CurPtr->Next;
					Allocator.destroy(&CurPtr->Data); //since C++11
					Allocator.deallocate(CurPtr, 1);
					CurPtr = NextPtr;
				}
			}
			BucketVector.resize(0);
			NodeCount = 0;
		}
		void rehash(size_t BucketCount){
			if (BucketCount < NodeCount)BucketCount = NodeCount;
			Vector<HashSetNode<T>*, bucket_allocator> TmpVector(BucketCount, nullptr);
			for (size_type i = 0; i < BucketVector.size(); i++){
				HashSetNode<T> *CurPtr = BucketVector[i];
				while (nullptr != CurPtr){
					HashSetNode<T> *NextPtr = CurPtr->Next;
					size_type NewBucketIndex = CurHasher(CurPtr->Data) % BucketCount;
					CurPtr->Next = TmpVector[NewBucketIndex];
					TmpVector[NewBucketIndex] = CurPtr;
					CurPtr = NextPtr;
				}
			}
			BucketVector.swap(TmpVector);
		}
		std::pair<iterator, bool> insert(const value_type& Value){
			size_type HashValue = CurHasher(Value);
			size_type BucketIndex;

			if (!BucketVector.empty()){
				BucketIndex = HashValue	% BucketVector.size();
				HashSetNode<T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(Value, CurPtr->Data))return std::pair<iterator, bool>(iterator(CurPtr, this), false);
					CurPtr = CurPtr->Next;
				}
			}

			HashSetNode<T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, Value); //since C++11

			if (NodeCount == BucketVector.size()){
				try{
					rehash(_Next_HashBucket_Count(BucketVector.size()));
				}
				catch (...){
					Allocator.destroy(&NewNode->Data); //since C++11
					Allocator.deallocate(NewNode, 1);
					throw;
				}
				BucketIndex = HashValue	% BucketVector.size();
			}

			NewNode->Next = BucketVector[BucketIndex];
			BucketVector[BucketIndex] = NewNode;
			NodeCount++;
			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}
		template< typename ... Args > std::pair<iterator, bool> emplace(Args&& ... args){
			size_type HashValue, BucketIndex;

			HashSetNode<T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11
			HashValue = CurHasher(NewNode->Data);

			if (!BucketVector.empty()){
				BucketIndex = HashValue	% BucketVector.size();
				HashSetNode<T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(NewNode->Data, CurPtr->Data)){
						Allocator.destroy(&NewNode->Data); //since C++11
						Allocator.deallocate(NewNode, 1);
						return std::pair<iterator, bool>(iterator(CurPtr, this), false);
					}
					CurPtr = CurPtr->Next;
				}
			}

			if (NodeCount == BucketVector.size()){
				try{
					rehash(_Next_HashBucket_Count(BucketVector.size()));
				}
				catch (...){
					Allocator.destroy(&NewNode->Data); //since C++11
					Allocator.deallocate(NewNode, 1);
					throw;
				}
				BucketIndex = HashValue	% BucketVector.size();
			}

			NewNode->Next = BucketVector[BucketIndex];
			BucketVector[BucketIndex] = NewNode;
			NodeCount++;
			return std::pair<iterator, bool>(iterator(NewNode, this), true);
		}
		size_type erase(const key_type& Key){
			if (!BucketVector.empty()){
				size_type BucketIndex = CurHasher(Key) % BucketVector.size();

				HashSetNode<T> **lpCurPtr = &BucketVector[BucketIndex];
				while (nullptr != *lpCurPtr){
					if (CurPredicate(Key, (*lpCurPtr)->Data)){
						HashSetNode<T> *CurPtr = *lpCurPtr;
						*lpCurPtr = (*lpCurPtr)->Next;
						Allocator.destroy(&CurPtr->Data); //since C++11
						Allocator.deallocate(CurPtr, 1);
						NodeCount--;

						if (0 == NodeCount)clear();
						return 1;
					}
					lpCurPtr = &(*lpCurPtr)->Next;
				}
				return 0;
			}
			else return 0;
		}
		iterator erase(const_iterator Pos){
			const HashSetNode<T> *TgtPtr = Pos.lpNode;
			if (nullptr != TgtPtr && !BucketVector.empty()){
				size_type BucketIndex = CurHasher(TgtPtr->Data) % BucketVector.size();

				HashSetNode<T> **lpCurPtr = &BucketVector[BucketIndex];
				while (nullptr != *lpCurPtr){
					if (TgtPtr == *lpCurPtr){
						iterator RetValue = iterator(*lpCurPtr, this);
						RetValue++;

						*lpCurPtr = TgtPtr->Next;
						Allocator.destroy(&TgtPtr->Data); //since C++11
						Allocator.deallocate(const_cast<HashSetNode<T>*>(TgtPtr), 1);
						NodeCount--;

						if (0 == NodeCount)clear();
						return RetValue;
					}
					lpCurPtr = &(*lpCurPtr)->Next;
				}
				return iterator(nullptr, this);
			}
			else return iterator(nullptr, this);
		}

		//query functions
		const_iterator find(const key_type& Key) const{
			if (!BucketVector.empty()){
				size_type BucketIndex = CurHasher(Key) % BucketVector.size();
				HashSetNode<T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(Key, CurPtr->Data))return const_iterator(CurPtr, this);
					CurPtr = CurPtr->Next;
				}
				return const_iterator(nullptr, this);
			}
			else return const_iterator(nullptr, this);
		}

		inline ~HashSet(){ clear(); }
	};
}

#endif