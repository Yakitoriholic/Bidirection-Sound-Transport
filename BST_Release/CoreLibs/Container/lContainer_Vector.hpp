/* Description: Vector class modelled after STL.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_VECTOR
#define LIB_CONTAINER_VECTOR

#include "lGeneral.hpp"

#include <iterator>
#include <algorithm>

namespace nsContainer{

	template < typename T, typename A = std::allocator<T> > class Vector{
		//without spaces, > > will be considered as an operator instead of two brackets.
	public:
		// custom typedefs
		typedef Vector<T, A>				this_type;

		// container typedefs, mandated by the STL standard
		typedef A                           allocator_type;
		typedef typename A::value_type      value_type;
		typedef typename A::reference       reference;
		typedef typename A::const_reference const_reference;
		typedef typename A::pointer         pointer;
		typedef typename A::const_pointer   const_pointer;
		typedef typename A::size_type       size_type;
		typedef typename A::difference_type difference_type;

		// container typedefs of iterator, mandated by the STL standard
		typedef pointer iterator;
		typedef const_pointer const_iterator;

	protected:
		iterator ArrPos;
		iterator ArrEnd;
		iterator MemEnd;
		A Allocator;

		void AllocAndFill(size_type n, const_reference Value){
			ArrPos = Allocator.allocate(n);
			std::uninitialized_fill_n(ArrPos, n, Value);
			ArrEnd = ArrPos + n;
			MemEnd = ArrEnd;
		}
	public:
		//constructors
		explicit Vector(const allocator_type& alloc) :ArrPos(0), ArrEnd(0), MemEnd(0){ Allocator = alloc; }
		Vector() : Vector(allocator_type()){}
		explicit Vector(size_type n) : Allocator(){ AllocAndFill(n, T()); }
		explicit Vector(size_type n, const_reference Value) : Allocator(){ AllocAndFill(n, Value); }
		Vector(const this_type &rhs){
			//cannot use keyword explicit. Otherwise the VS (2013) compiler will crash.
			Allocator = rhs.Allocator;
			if (!rhs.empty()){
				ArrPos = Allocator.allocate(rhs.ArrEnd - rhs.ArrPos);
				ArrEnd = std::uninitialized_copy(rhs.ArrPos, rhs.ArrEnd, ArrPos);
			}
			else{
				ArrPos = nullptr;
				ArrEnd = nullptr;
			}
			MemEnd = ArrEnd;
		}
		Vector(this_type &&rhs){
			Allocator = rhs.Allocator;
			ArrPos = rhs.ArrPos;
			ArrEnd = rhs.ArrEnd;
			MemEnd = rhs.MemEnd;
			//get rhs ready for destruct
			rhs.ArrEnd = rhs.ArrPos;
			rhs.MemEnd = rhs.ArrPos;
		}

		//observer functions
		inline allocator_type get_allocator() const { return Allocator; }

		// iterator functions, mandated by the STL standard
		inline iterator begin(){ return ArrPos; }
		inline const_iterator cbegin() const { return (const_iterator)ArrPos; }
		inline iterator end(){ return ArrEnd; }
		inline const_iterator cend() const { return (const_iterator)ArrEnd; }

		//element access functions
		inline reference operator[](size_type n){ return ArrPos[n]; } //ArrPos is a random iterator.
		inline const_reference operator[](size_type n) const { return ArrPos[n]; } //ArrPos is a random iterator.
		inline reference front(){ return *ArrPos; }
		inline const_reference front() const { return *ArrPos; }
		inline reference back(){ return *(ArrEnd - 1); }
		inline const_reference back() const { return *(ArrEnd - 1); }

		//capacity functions
		inline bool empty() const { return ArrPos == ArrEnd; }
		inline size_type size() const { return size_type(ArrEnd - ArrPos); }
		inline size_type capacity() const { return size_type(MemEnd - ArrPos); }

		//modifiers
		inline void clear(){ RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd); ArrEnd = ArrPos; }
		iterator insert(const_iterator pos, const_reference value){
			//single insert is costy and should be avoided for efficiency.
			if (ArrEnd != MemEnd){
				if (ArrEnd - pos > 1){
					Allocator.construct(ArrEnd, back());
					std::copy_backward((iterator)pos, ArrEnd - 1, ArrEnd);
					*(iterator)pos = value; //assign
				}
				else if ((iterator)pos == ArrEnd){
					Allocator.construct(ArrEnd, value);
				}
				else{
					Allocator.construct(ArrEnd, back());
					*(iterator)pos = value;
				}
				ArrEnd++;
				return (iterator)pos;
			}
			else{
				size_type CurSize = this->capacity();
				size_type NewSize;
				if (0 == CurSize)NewSize = 1; else NewSize = 2 * CurSize; //beware unsigned int rollback!

				iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, (iterator)pos, ArrPos_New);
				iterator RetValue = ArrEnd_New;
				Allocator.construct(RetValue, value);
				ArrEnd_New++;
				ArrEnd_New = std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd_New);

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrEnd_New;
				MemEnd = ArrPos + NewSize;

				return RetValue;
			}
		}
		iterator insert(const_iterator pos, T&& value){
			//single insert is costy and should be avoided for efficiency.
			if (ArrEnd != MemEnd){
				if (ArrEnd - pos > 1){
					Allocator.construct(ArrEnd, back());
					std::copy_backward((iterator)pos, ArrEnd - 1, ArrEnd);
					*(iterator)pos = value; //move
				}
				else if ((iterator)pos == ArrEnd){
					Allocator.construct(ArrEnd, value);
				}
				else{
					Allocator.construct(ArrEnd, back());
					*(iterator)pos = value;
				}
				ArrEnd++;
				return (iterator)pos;
			}
			else{
				size_type CurSize = this->capacity();
				size_type NewSize;
				if (0 == CurSize)NewSize = 1; else NewSize = 2 * CurSize; //beware unsigned int rollback!

				iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, (iterator)pos, ArrPos_New);
				iterator RetValue = ArrEnd_New;
				Allocator.construct(RetValue, value);
				ArrEnd_New++;
				ArrEnd_New = std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd_New);

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrEnd_New;
				MemEnd = ArrPos + NewSize;

				return RetValue;
			}
		}
		iterator insert(const_iterator pos, size_type count, const_reference value){
			if (count > 0){
				size_type CurSpaceRemain = (size_type)(MemEnd - ArrEnd);
				if (CurSpaceRemain >= count){
					size_type TailElemCount = ArrEnd - (iterator)pos;
					if (TailElemCount > count){
						std::uninitialized_copy(ArrEnd - count, ArrEnd, ArrEnd);
						std::copy_backward((iterator)pos, ArrEnd - count, ArrEnd);
						std::fill((iterator)pos, (iterator)pos + count, value); //assign
					}
					else{
						std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd + count - TailElemCount);
						std::uninitialized_fill_n(ArrEnd, count - TailElemCount, value);
						std::fill((iterator)pos, ArrEnd, value); //assign
					}
					ArrEnd += count;
					return (iterator)pos;
				}
				else{
					size_type CurSize = this->capacity();
					size_type NewSize;
					if (0 == CurSize)NewSize = count;
					else if (2 * CurSize < this->size() + count)NewSize = this->size() + count; //beware unsigned int rollback!
					else NewSize = 2 * CurSize; //beware unsigned int rollback!

					iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
					iterator ArrEnd_New = std::uninitialized_copy(ArrPos, (iterator)pos, ArrPos_New);
					iterator RetValue = ArrEnd_New;
					ArrEnd_New = std::uninitialized_fill_n(ArrEnd_New, count, value);
					ArrEnd_New = std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd_New);

					RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
					if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

					ArrPos = ArrPos_New;
					ArrEnd = ArrEnd_New;
					MemEnd = ArrPos + NewSize;

					return RetValue;
				}
			}
			else return (iterator)pos;
		}
		template< typename InputIt > iterator insert(const_iterator pos, InputIt first, InputIt last){
			if (first != last){
				size_type count = last - first;
				size_type CurSpaceRemain = (size_type)(MemEnd - ArrEnd);
				if (CurSpaceRemain >= count){
					size_type TailElemCount = ArrEnd - (iterator)pos;
					if (TailElemCount > count){
						std::uninitialized_copy(ArrEnd - count, ArrEnd, ArrEnd);
						std::copy_backward((iterator)pos, ArrEnd - count, ArrEnd);
						std::copy(first, last, (iterator)pos); //assign
					}
					else{
						InputIt TmpIt = first + (count - TailElemCount);
						std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd + count - TailElemCount);
						std::uninitialized_copy(TmpIt, last, ArrEnd);
						std::copy(first, TmpIt, (iterator)pos); //assign
					}
					ArrEnd += count;
					return (iterator)pos;
				}
				else{
					size_type CurSize = this->capacity();
					size_type NewSize;
					if (0 == CurSize)NewSize = count;
					else if (2 * CurSize < this->size() + count)NewSize = this->size() + count; //beware unsigned int rollback!
					else NewSize = 2 * CurSize; //beware unsigned int rollback!

					iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
					iterator ArrEnd_New = std::uninitialized_copy(ArrPos, (iterator)pos, ArrPos_New);
					iterator RetValue = ArrEnd_New;
					ArrEnd_New = std::uninitialized_copy(first, last, ArrEnd_New);
					ArrEnd_New = std::uninitialized_copy((iterator)pos, ArrEnd, ArrEnd_New);

					RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
					if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

					ArrPos = ArrPos_New;
					ArrEnd = ArrEnd_New;
					MemEnd = ArrPos + NewSize;

					return RetValue;
				}
			}
			else return (iterator)pos;
		}
		iterator erase(const_iterator pos){
			iterator Next = (iterator)pos + 1;
			if (Next != ArrEnd)std::copy(Next, ArrEnd, (iterator)pos);
			ArrEnd--;
			Allocator.destroy(ArrEnd);
			return (iterator)pos;
		}
		
		iterator erase(const_iterator first, const_iterator last){
			iterator Next = (iterator)last;
			if (Next == ArrEnd)Next = (iterator)first;
			else Next = std::copy(Next, ArrEnd, (iterator)first);
			RangeDestroyer<A>::Destroy(Allocator, Next, ArrEnd);
			ArrEnd = Next;
			return (iterator)first;
		}
		void push_back(const_reference Value){
			if (ArrEnd != MemEnd){
				Allocator.construct(ArrEnd, Value);
				ArrEnd++;
			}
			else{
				size_type CurSize = this->capacity();
				size_type NewSize;
				if (0 == CurSize)NewSize = 1; else NewSize = 2 * CurSize; //beware unsigned int rollback!
				iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
				Allocator.construct(ArrEnd_New, Value);

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrEnd_New + 1;
				MemEnd = ArrPos + NewSize;
			}
		}
		void push_back(T&& Value){
			if (ArrEnd != MemEnd){
				Allocator.construct(ArrEnd, Value);
				ArrEnd++;
			}
			else{
				size_type CurSize = this->capacity();
				size_type NewSize;
				if (0 == CurSize)NewSize = 1; else NewSize = 2 * CurSize; //beware unsigned int rollback!
				iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
				Allocator.construct(ArrEnd_New, Value);

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrEnd_New + 1;
				MemEnd = ArrPos + NewSize;
			}
		}
		template <typename ... Args> void emplace_back(Args&& ... args){
			if (ArrEnd != MemEnd){
				Allocator.construct(ArrEnd, std::forward<Args>(args)...);
				ArrEnd++;
			}
			else{
				size_type CurSize = this->capacity();
				size_type NewSize;
				if (0 == CurSize)NewSize = 1; else NewSize = 2 * CurSize; //beware unsigned int rollback!
				iterator ArrPos_New = Allocator.allocate(NewSize); //no exception handling is done in this line as function allocate() can throw out std::bad_alloc.
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
				Allocator.construct(ArrEnd_New, std::forward<Args>(args)...);

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrEnd_New + 1;
				MemEnd = ArrPos + NewSize;
			}
		}
		inline void pop_back(){ --ArrEnd; Allocator.destroy(ArrEnd); } //remind that pop_back() doesn't check whether this container is empty!
		void resize(size_type count){
			size_type CurSize = this->size();
			if (count > this->capacity()){
				iterator ArrPos_New = Allocator.allocate(count);
				iterator ArrEnd_New;
				if (count > CurSize){
					ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
					std::uninitialized_fill_n(ArrEnd_New, count - CurSize, T());
				}

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrPos + count;
				MemEnd = ArrEnd;
			}
			else if(count < CurSize){
				if (count < CurSize){
					RangeDestroyer<A>::Destroy(Allocator, ArrPos + count, ArrEnd);
				}
				else if (count > CurSize){
					std::uninitialized_fill_n(ArrEnd, count - CurSize, T());
				}
				ArrEnd = ArrPos + count;
			}
		}
		void resize(size_type count, const_reference value){
			size_type CurSize = this->size();
			if (count > this->capacity()){
				iterator ArrPos_New = Allocator.allocate(count);
				iterator ArrEnd_New;
				if (count > CurSize){
					ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
					std::uninitialized_fill_n(ArrEnd_New, count - CurSize, value);
				}

				RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
				if (0 != CurSize)Allocator.deallocate(ArrPos, CurSize);

				ArrPos = ArrPos_New;
				ArrEnd = ArrPos + count;
				MemEnd = ArrEnd;
			}
			else{
				if (count < CurSize){
					RangeDestroyer<A>::Destroy(Allocator, ArrPos + count, ArrEnd);
				}
				else if (count > CurSize){
					std::uninitialized_fill_n(ArrEnd, count - CurSize, value);
				}
				ArrEnd = ArrPos + count;
			}
		}
		void reserve(size_type count){
			size_type CurSize = this->capacity();
			if (count > CurSize){
				iterator ArrPos_New = Allocator.allocate(count);
				iterator ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);
				if (0 != CurSize){
					RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
					Allocator.deallocate(ArrPos, CurSize);
				}
				ArrEnd = ArrPos_New + (ArrEnd - ArrPos);
				ArrPos = ArrPos_New;
				MemEnd = ArrPos + count;
			}
		}
		void shrink_to_fit(){
			if (ArrEnd != MemEnd){
				if (0 == this->size()){
					Allocator.deallocate(ArrPos, this->capacity());

					ArrPos = nullptr;
					ArrEnd = nullptr;
					MemEnd = nullptr;
				}
				else{
					size_type CurCap = this->capacity();
					size_type NewCap = this->size();

					iterator ArrPos_New = Allocator.allocate(NewCap);
					iterator ArrEnd_New = std::uninitialized_copy(ArrPos, ArrEnd, ArrPos_New);

					RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);

					Allocator.deallocate(ArrPos, CurCap);

					ArrPos = ArrPos_New;
					ArrEnd = ArrEnd_New;
					MemEnd = ArrEnd;
				}
			}
		}
		void swap(this_type &rhs){
			if (std::allocator_traits<A>::propagate_on_container_swap::value)std::swap(this->Allocator, rhs.Allocator);
			std::swap(this->ArrPos, rhs.ArrPos);
			std::swap(this->ArrEnd, rhs.ArrEnd);
			std::swap(this->MemEnd, rhs.MemEnd);
		}

		//operators

		this_type& operator=(const this_type& rhs){
			this_type temp(rhs);
			this->swap(temp);
			return *this;
		}
		this_type& operator=(this_type&& rhs){
			this->Allocator = rhs.Allocator;
			this->ArrPos = rhs.ArrPos;
			this->ArrEnd = rhs.ArrEnd;
			this->MemEnd = rhs.MemEnd;
			//get rhs ready for destruct
			rhs.ArrEnd = rhs.ArrPos;
			rhs.MemEnd = rhs.ArrPos;
			return *this;
		}
		~Vector(){
			RangeDestroyer<A>::Destroy(Allocator, ArrPos, ArrEnd);
			if (MemEnd != ArrPos)Allocator.deallocate(ArrPos, MemEnd - ArrPos);
		}
	};
}

#endif