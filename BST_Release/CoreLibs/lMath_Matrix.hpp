/* Description: Matrix class.
* Language:C++
* Author:***
*/

#ifndef LIB_MATH_MATRIX
#define LIB_MATH_MATRIX

#include "lMath.hpp"

namespace nsMath{
	class Matrix_Base{
	protected:
		UBINT _TypeID;
		UBINT _DimCount;
		UBINT *_DimArr;
		void *lpData;

		template<typename ... Args> void Variant_Set(UBINT *lpDest, UBINT Value, Args... args);
		template<typename ... Args> UBINT Variant_Multiply(UBINT Value, Args... args);
		template<typename ... Args> void Variant_GetIndex(UBINT *lpDest, UBINT *lpResult, UBINT Value, Args... args);
	public:
		//constructors
		Matrix_Base() = default;
		Matrix_Base(UBINT _DummyVal);
		Matrix_Base(const Matrix_Base &rhs);
		Matrix_Base(Matrix_Base &&rhs);

		//capacity functions
		inline UBINT dim_count() const { return this->_DimCount; }
		inline const UBINT dim_size(UBINT i) const { return this->_DimArr[i]; }
		inline const UBINT type_size() const { UBINT TypeSize = 1 << (this->_TypeID & 0xF); if (this->_TypeID & 0x40)TypeSize *= 2; return TypeSize; }
		inline const UBINT size() const { if (_DimCount > 0){ UBINT RetValue = 1; for (UBINT i = 0; i < this->_DimCount; i++)RetValue *= _DimArr[i]; return RetValue; } else return 0; }
		inline const UBINT byte_size() const { return this->size() * this->type_size(); }
		inline const bool empty() const { return 0 == this->_DimCount; }

		//element access functions
		inline void *data(){ return this->lpData; }
		inline const void *data() const { return (const void *)this->lpData; }

		//modifiers
		void swap(Matrix_Base &rhs);
		void clear();

		//operators
		Matrix_Base& operator=(const Matrix_Base& rhs);
		Matrix_Base& operator=(Matrix_Base&& rhs);

		inline ~Matrix_Base(){ this->clear(); }
	};

	template <typename T> class Matrix :public Matrix_Base{
	public:
		//constructors
		Matrix(){
			this->_TypeID = __typeid(T);
			this->_DimCount = 0;
			this->_DimArr = nullptr;
			this->lpData = nullptr;
		}
		template <typename ... Args> Matrix(UBINT dim_size_1, Args... dim_size_rest){
			this->_TypeID = __typeid(T);

			this->_DimCount = 1 + sizeof...(dim_size_rest);
			this->_DimArr = nsBasic::GlobalMemAlloc_Arr<UBINT>(this->_DimCount);
			if (nullptr == this->_DimArr)throw std::bad_alloc();
			this->Variant_Set(this->_DimArr, dim_size_1, dim_size_rest...);

			UBINT MatSize = this->Variant_Multiply(dim_size_1, dim_size_rest...);
			this->lpData = nsBasic::GlobalMemAlloc_Arr_SIMD<T>(MatSize);
			if (nullptr == this->lpData){
				nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
				throw std::bad_alloc();
			}
		}
		Matrix(const Matrix<T> &rhs){
			this->_TypeID = rhs._TypeID;
			this->_DimCount = rhs._DimCount;
			this->_DimArr = nsBasic::GlobalMemAlloc_Arr<UBINT>(this->_DimCount);
			if (nullptr == this->_DimArr)throw std::bad_alloc();
			memcpy(this->_DimArr, rhs._DimArr, sizeof(UBINT)* this->_DimCount);

			UBINT MatSize = sizeof(T) * rhs.size();
			this->lpData = nsBasic::GlobalMemAlloc_SIMD(MatSize);
			if (nullptr == this->lpData){
				nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
				throw std::bad_alloc();
			}
			memcpy(this->lpData, rhs.lpData, MatSize);
		}
		Matrix(Matrix<T> &&rhs){
			this->_TypeID = rhs._TypeID;
			this->_DimCount = rhs._DimCount;
			this->_DimArr = rhs._DimArr;
			this->lpData = rhs.lpData;
			//get rhs ready for destruct
			rhs._DimCount = 0;
			rhs._DimArr = nullptr;
			rhs.lpData = nullptr;
		}

		//capacity functions
		inline UBINT dim_count() const { return this->_DimCount; }
		inline const UBINT dim_size(UBINT i) const { return this->_DimArr[i]; }
		inline const UBINT size() const { if (_DimCount > 0){ UBINT RetValue = 1; for (UBINT i = 0; i < this->_DimCount; i++)RetValue *= _DimArr[i]; return RetValue; } else return 0; }
		inline const bool empty() const { return 0 == this->_DimCount; }

		//element access functions
		inline T *data(){ return (T *)this->lpData; }
		inline const T *data() const { return (const T *)this->lpData; }
		template <typename ... Args> T& operator()(UBINT dim_size_1, Args... dim_size_rest){
			UBINT Index = 0;
			this->Variant_GetIndex(this->_DimArr, &Index, dim_size_1, dim_size_rest...);
			for (UBINT i = sizeof...(dim_size_rest) + 1; i < this->_DimCount; i++)Index *= this->_DimArr[i];
			return ((T *)this->lpData)[Index];
		}
		template <typename ... Args> const T& operator()(UBINT dim_size_1, Args... dim_size_rest) const {
			UBINT Index = 0;
			this->Variant_GetIndex(this->_DimArr, &Index, dim_size_1, dim_size_rest...);
			for (UBINT i = sizeof...(dim_size_rest) + 1; i < this->_DimCount; i++)Index *= this->_DimArr[i];
			return ((T *)this->lpData)[Index];
		}

		//modifiers
		template <typename ... Args> void resize(UBINT dim_size_1, Args... dim_size_rest){
			UBINT _DimCount_New = 1 + sizeof...(dim_size_rest);
			UBINT *_DimArr_New = nsBasic::GlobalMemAlloc_Arr<UBINT>(_DimCount_New);
			if (nullptr == _DimArr_New)throw std::bad_alloc();
			this->Variant_Set(_DimArr_New, dim_size_1, dim_size_rest...);

			UBINT MatSize = this->Variant_Multiply(dim_size_1, dim_size_rest...);
			T *lpData_New = nsBasic::GlobalMemAlloc_Arr_SIMD<T>(MatSize);
			if (nullptr == lpData_New){
				nsBasic::GlobalMemFree_Arr<UBINT>(_DimArr_New, _DimCount_New);
				throw std::bad_alloc();
			}

			if (this->_DimCount > 0){
				nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
				nsBasic::GlobalMemFree_Arr_SIMD<T>((T *)this->lpData, this->size());
			}

			this->_DimCount = _DimCount_New;
			this->_DimArr = _DimArr_New;
			this->lpData = lpData_New;
		}
		void clear(){
			if (this->_DimCount > 0){
				nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
				nsBasic::GlobalMemFree_Arr_SIMD<T>((T *)this->lpData, this->size());
			}
		}

		//operators
		Matrix<T>& operator=(const Matrix<T>& rhs){
			Matrix<T> temp(rhs);
			this->swap(temp);
			return *this;
		}
		inline Matrix<T>& operator=(Matrix<T>&& rhs){ return static_cast<Matrix<T>&>(this->Matrix_Base::operator=(rhs)); }
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template <typename ... Args> void Matrix_Base::Variant_Set(UBINT *lpDest, UBINT Value, Args... args){ *lpDest = Value; Variant_Set(++lpDest, args...); }
	template <> inline void Matrix_Base::Variant_Set<>(UBINT *lpDest, UBINT Value){ *lpDest = Value; }
	template <typename ... Args> inline UBINT Matrix_Base::Variant_Multiply(UBINT Value, Args... args){ return Value * Variant_Multiply(args...); }
	template <> inline UBINT Matrix_Base::Variant_Multiply<>(UBINT Value){ return Value; }
	template <typename ... Args> void Matrix_Base::Variant_GetIndex(UBINT *lpDest, UBINT *lpResult, UBINT Value, Args... args){ *lpResult += Value; lpDest++; *lpResult *= (*lpDest); Variant_GetIndex(lpDest, lpResult, args...); }
	template <> inline void Matrix_Base::Variant_GetIndex<>(UBINT *lpDest, UBINT *lpResult, UBINT Value){ *lpResult += Value; }
	Matrix_Base::Matrix_Base(UBINT _DummyVal){
		this->_TypeID = 0;
		this->_DimCount = 0;
		this->_DimArr = nullptr;
		this->lpData = nullptr;
	}
	Matrix_Base::Matrix_Base(const Matrix_Base &rhs){
		this->_TypeID = rhs._TypeID;
		this->_DimCount = rhs._DimCount;
		this->_DimArr = nsBasic::GlobalMemAlloc_Arr<UBINT>(this->_DimCount);
		if (nullptr == this->_DimArr)throw std::bad_alloc();
		memcpy(this->_DimArr, rhs._DimArr, sizeof(UBINT) * this->_DimCount);

		UBINT MatSize = rhs.byte_size();
		this->lpData = nsBasic::GlobalMemAlloc_SIMD(MatSize);
		if (nullptr == this->lpData){
			nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
			throw std::bad_alloc();
		}
		memcpy(this->lpData, rhs.lpData, MatSize);
	}
	Matrix_Base::Matrix_Base(Matrix_Base &&rhs){
		this->_TypeID = rhs._TypeID;
		this->_DimCount = rhs._DimCount;
		this->_DimArr = rhs._DimArr;
		this->lpData = rhs.lpData;
		//get rhs ready for destruct
		rhs._DimCount = 0;
		rhs._DimArr = nullptr;
		rhs.lpData = nullptr;
	}
	inline void Matrix_Base::swap(Matrix_Base &rhs){
		std::swap(this->_TypeID, rhs._TypeID);
		std::swap(this->_DimCount, rhs._DimCount);
		std::swap(this->_DimArr, rhs._DimArr);
		std::swap(this->lpData, rhs.lpData);
	}
	Matrix_Base& Matrix_Base::operator=(const Matrix_Base& rhs){
		Matrix_Base temp(rhs);
		this->swap(temp);
		return *this;
	}
	Matrix_Base& Matrix_Base::operator=(Matrix_Base&& rhs){
		this->_TypeID = rhs._TypeID;
		this->_DimCount = rhs._DimCount;
		this->_DimArr = rhs._DimArr;
		this->lpData = rhs.lpData;
		//get rhs ready for destruct
		rhs._DimCount = 0;
		rhs._DimArr = nullptr;
		rhs.lpData = nullptr;
		return *this;
	}
	void Matrix_Base::clear(){
		if (this->_DimCount > 0){
			nsBasic::GlobalMemFree_SIMD(this->lpData, this->byte_size());
			nsBasic::GlobalMemFree_Arr<UBINT>(this->_DimArr, this->_DimCount);
		}
	}
}
#endif