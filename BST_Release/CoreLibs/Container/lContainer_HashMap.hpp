/* Description: Hash map class modelled after STL. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_HASHMAP
#define LIB_CONTAINER_HASHMAP

#include "lContainer_Hash.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{
	template < typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>, typename A = std::allocator<std::pair<const Key, T>> > class HashMap;

	template < typename Key, typename T > struct HashMapNode{
		HashMapNode *			Next;
		std::pair<const Key, T> Data;
	};

	template < typename Key, typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>, typename A = std::allocator<T> > struct HashMapIterator{
		// custom typedefs
		typedef HashMapIterator<Key, T, Hash, Pred, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag	iterator_category; //#0

		typedef typename std::pair<const Key, T>	value_type; //#1
		typedef typename ptrdiff_t					difference_type; //#2
		typedef typename value_type*				pointer; //#3
		typedef typename value_type&				reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		HashMapNode<Key, T>* lpNode;
		HashMap<Key, T, Hash, Pred, A>* lpHashMap;

		HashMapIterator(HashMapNode<Key, T>* Node, HashMap<Key, T, Hash, Pred, A>* HashMap) : lpNode(Node), lpHashMap(HashMap){}
		HashMapIterator() {}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		this_type& operator++(){
			if (nullptr == lpNode->Next){
				size_t BucketIndex = lpHashMap->CurHasher(lpNode->Data.first) % lpHashMap->BucketVector.size();
				lpNode = nullptr;
				do{
					BucketIndex++;
					if (BucketIndex >= lpHashMap->BucketVector.size())break;
					lpNode = lpHashMap->BucketVector[BucketIndex];
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

	template < typename Key, typename T, typename Hash = std::hash<T>, typename Pred = std::equal_to<T>, typename A = std::allocator<T> > struct HashMapIterator_Const{
		// custom typedefs
		typedef HashMapIterator_Const<Key, T, Hash, Pred, A>	this_type;

		// iterator typedefs, mandated by the STL standard
		typedef typename std::forward_iterator_tag iterator_category; //#0

		typedef typename std::pair<const Key, T>	value_type; //#1
		typedef typename ptrdiff_t					difference_type; //#2
		typedef typename const value_type*			pointer; //#3
		typedef typename const value_type&			reference; //#4
		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		HashMapNode<Key, T>* lpNode;
		HashMap<Key, T, Hash, Pred, A>* lpHashMap;

		HashMapIterator_Const(const HashMapNode<Key, T>* Node, const HashMap<Key, T, Hash, Pred, A>* HashMap) : lpNode(const_cast<decltype(lpNode)>(Node)), lpHashMap(const_cast<decltype(lpHashMap)>(HashMap)){}
		HashMapIterator_Const() {}
		HashMapIterator_Const(const HashMapIterator<Key, T, Hash, Pred, A>& rhs) : lpNode(rhs.lpNode), lpHashMap(rhs.lpHashMap){}

		inline bool operator==(const this_type& rhs) const { return this->lpNode == rhs.lpNode; }
		inline bool operator!=(const this_type& rhs) const { return this->lpNode != rhs.lpNode; }

		inline reference operator*() const { return lpNode->Data; }
		inline pointer operator->() const { return &(lpNode->Data); }

		this_type& operator++(){
			if (nullptr == lpNode->Next){
				size_t BucketIndex = lpHashMap->CurHasher(lpNode->Data.first) % lpHashMap->BucketVector.size();
				lpNode = nullptr;
				do{
					BucketIndex++;
					if (BucketIndex >= lpHashMap->BucketVector.size())break;
					lpNode = lpHashMap->BucketVector[BucketIndex];
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

	template < typename Key, typename T, typename Hash, typename Pred, typename A > class HashMap{
	public:
		// custom typedefs
		typedef HashMap<Key, T, Hash, Pred, A>	this_type;

		// container typedefs, mandated by the STL standard
		typedef typename A::template rebind< HashMapNode<Key, T> >::other	allocator_type; //diffrent from the C++ STL standard.
		typedef typename std::pair<const Key, T>							value_type;
		typedef typename value_type&										reference;
		typedef typename const value_type&									const_reference;
		typedef typename value_type*										pointer;
		typedef typename const value_type*									const_pointer;
		typedef typename size_t												size_type;
		typedef typename ptrdiff_t											difference_type;

		typedef typename Key		key_type;
		typedef typename Hash		hasher;

		// You have no other choice. The allocator<T> is not used directly but used through rebinding.

		// container typedefs of iterator, mandated by the STL standard
		typedef HashMapIterator<Key, T, Hash, Pred, A>		iterator;
		typedef HashMapIterator_Const<Key, T, Hash, Pred, A> const_iterator;

		friend struct HashMapIterator<Key, T, Hash, Pred, A>;
		friend struct HashMapIterator_Const<Key, T, Hash, Pred, A>;

	protected:
		typedef typename A::template rebind< HashMapNode<Key, T>* >::other	bucket_allocator;

		allocator_type	Allocator;
		hasher			CurHasher;
		Pred			CurPredicate;
		size_type		NodeCount;
		Vector<HashMapNode<Key, T>*, bucket_allocator> BucketVector;

		//HashMap<Key, T>& operator =(const HashMap<Key, T>&) = delete;
	public:
		//constructors
		explicit HashMap(size_type BucketCount, const Hash& HashFunc = Hash(), const Pred& EqFunc = Pred(), const allocator_type& _Allocator = allocator_type())
			:Allocator(_Allocator), CurHasher(HashFunc), CurPredicate(EqFunc), BucketVector(BucketCount, nullptr){
			this->NodeCount = 0;
		}
		HashMap() :HashMap(0){}

		//observer functions
		inline allocator_type get_allocator() const{ return Allocator; }

		// iterator functions, mandated by the STL standard
		iterator begin(){
			for (size_type i = 0; i < BucketVector.size(); i++){
				if (nullptr != BucketVector[i])return iterator(BucketVector[i], this);
			}
			return iterator(nullptr, this);
		}
		const_iterator cbegin() const{
			for (size_type i = 0; i < BucketVector.size(); i++){
				if (nullptr != BucketVector[i])return const_iterator(BucketVector[i], this);
			}
			return const_iterator(nullptr, this);
		}
		inline iterator end(){ return iterator(nullptr, this); }
		inline const_iterator cend() const{ return const_iterator(nullptr, this); }

		//capacity functions
		inline bool empty() const { return NodeCount == 0; }
		inline size_type size() const { return NodeCount; }

		//modifiers
		void clear(){
			for (size_type i = 0; i < BucketVector.size(); i++){
				HashMapNode<key_type, T> *CurPtr = BucketVector[i];
				while (nullptr != CurPtr){
					HashMapNode<key_type, T> *NextPtr = CurPtr->Next;
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
			Vector<HashMapNode<key_type, T>*, bucket_allocator> TmpVector(BucketCount, nullptr);
			for (size_type i = 0; i < BucketVector.size(); i++){
				HashMapNode<key_type, T> *CurPtr = BucketVector[i];
				while (nullptr != CurPtr){
					HashMapNode<key_type, T> *NextPtr = CurPtr->Next;
					size_type NewBucketIndex = CurHasher(CurPtr->Data.first) % BucketCount;
					CurPtr->Next = TmpVector[NewBucketIndex];
					TmpVector[NewBucketIndex] = CurPtr;
					CurPtr = NextPtr;
				}
			}
			BucketVector.swap(TmpVector);
		}
		std::pair<iterator, bool> insert(const value_type& Value){
			size_type HashValue = CurHasher(Value.first);
			size_type BucketIndex;

			if (!BucketVector.empty()){
				BucketIndex = HashValue	% BucketVector.size();
				HashMapNode<key_type, T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(Value.first, CurPtr->Data.first))return std::pair<iterator, bool>(iterator(CurPtr, this), false);
					CurPtr = CurPtr->Next;
				}
			}

			HashMapNode<key_type, T> *NewNode = Allocator.allocate(1);
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

			HashMapNode<key_type, T> *NewNode = Allocator.allocate(1);
			Allocator.construct(&NewNode->Data, std::forward<Args>(args)...); //since C++11
			HashValue = CurHasher(NewNode->Data.first);

			if (!BucketVector.empty()){
				BucketIndex = HashValue	% BucketVector.size();
				HashMapNode<key_type, T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(NewNode->Data.first, CurPtr->Data.first)){
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
		template< typename ... Args > std::pair<iterator, bool> try_emplace(const key_type& key, Args&&... args){
			size_type HashValue = CurHasher(key);
			size_type BucketIndex;

			if (!BucketVector.empty()){
				BucketIndex = HashValue	% BucketVector.size();
				HashMapNode<key_type, T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(key, CurPtr->Data.first))return std::pair<iterator, bool>(iterator(CurPtr, this), false);
					CurPtr = CurPtr->Next;
				}
			}

			HashMapNode<key_type, T> *NewNode = Allocator.allocate(1);

			new((void *)&(NewNode->Data.first)) key_type(key);
			new((void *)&(NewNode->Data.second)) T(args...);

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
		size_type erase(const key_type& key){
			if (!BucketVector.empty()){
				size_type BucketIndex = CurHasher(key) % BucketVector.size();

				HashMapNode<key_type, T> **lpCurPtr = &BucketVector[BucketIndex];
				while (nullptr != *lpCurPtr){
					if (CurPredicate(key, (*lpCurPtr)->Data.first)){
						HashMapNode<key_type, T> *CurPtr = *lpCurPtr;
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
			const HashMapNode<key_type, T> *TgtPtr = Pos.lpNode;
			if (nullptr != TgtPtr && !BucketVector.empty()){
				size_type BucketIndex = CurHasher(TgtPtr->Data.first) % BucketVector.size();

				HashMapNode<key_type, T> **lpCurPtr = &BucketVector[BucketIndex];
				while (nullptr != *lpCurPtr){
					if (TgtPtr == *lpCurPtr){
						iterator RetValue = iterator(*lpCurPtr, this);
						RetValue++;

						*lpCurPtr = TgtPtr->Next;
						Allocator.destroy(&TgtPtr->Data); //since C++11
						Allocator.deallocate(const_cast<HashMapNode<key_type, T>*>(TgtPtr), 1);
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
		iterator find(const key_type& key){
			if (!BucketVector.empty()){
				size_type BucketIndex = CurHasher(key) % BucketVector.size();
				HashMapNode<key_type, T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(key, CurPtr->Data.first))return iterator(CurPtr, this);
					CurPtr = CurPtr->Next;
				}
				return iterator(nullptr, this);
			}
			else return iterator(nullptr, this);
		}
		const_iterator find(const key_type& key) const{
			if (!BucketVector.empty()){
				size_type BucketIndex = CurHasher(key) % BucketVector.size();
				HashMapNode<key_type, T> *CurPtr = BucketVector[BucketIndex];
				while (nullptr != CurPtr){
					if (CurPredicate(key, CurPtr->Data.first))return const_iterator(CurPtr, this);
					CurPtr = CurPtr->Next;
				}
				return const_iterator(nullptr, this);
			}
			else return const_iterator(nullptr, this);
		}

		//element access functions
		inline T& operator[](const key_type& key){ return this->find(key)->second; }
		inline const T& operator[](const key_type& key) const{ return this->find(key)->second; }
		inline ~HashMap(){ clear(); }
	};
}

#endif