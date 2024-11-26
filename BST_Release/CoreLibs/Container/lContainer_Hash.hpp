/* Description: Basic functions for hash containers. DO NOT include this header directly.
 * Language: C++11
 * Author: ***
 *
 */

#ifndef LIB_CONTAINER_HASH
#define LIB_CONTAINER_HASH

#include "lGeneral.hpp"

namespace nsContainer{
#if defined LIBENV_SYS_INTELX64
	const size_t _HashBucket_Count_Candidate_ArrSize = 63;
	const size_t _HashBucket_Count_Candidate[] = {
		0x3, 0x7, 0xD, 0x1F, 0x3D, 0x7F, 0xFB, 0x1FD, 0x3FD, 0x7F7, 0xFFD, 0x1FFF, 0x3FFD,
		0x7FED, 0xFFF1, 0x1FFFF, 0x3FFFB, 0x7FFFF, 0xFFFFD, 0x1FFFF7, 0x3FFFFD, 0x7FFFF1,
		0xFFFFFD, 0x1FFFFD9, 0x3FFFFFB, 0x7FFFFD9, 0xFFFFFC7, 0x1FFFFFFD, 0x3FFFFFDD,
		0x7FFFFFFF, 0xFFFFFFFB,
		0x1FFFFFFF7, 0x3FFFFFFD7, 0x7FFFFFFE1, 0xFFFFFFFFB, 0x1FFFFFFFE7, 0x3FFFFFFFD3,
		0x7FFFFFFFF9, 0xFFFFFFFFA9, 0x1FFFFFFFFEB, 0x3FFFFFFFFF5, 0x7FFFFFFFFC7,
		0xFFFFFFFFFEF, 0x1FFFFFFFFFC9, 0x3FFFFFFFFFEB, 0x7FFFFFFFFF8D, 0xFFFFFFFFFFC5,
		0x1FFFFFFFFFFAF, 0x3FFFFFFFFFFE5, 0x7FFFFFFFFFF7F, 0xFFFFFFFFFFFD1,
		0x1FFFFFFFFFFF91, 0x3FFFFFFFFFFFDF, 0x7FFFFFFFFFFFC9, 0xFFFFFFFFFFFFFB,
		0x1FFFFFFFFFFFFF3, 0x3FFFFFFFFFFFFE5, 0x7FFFFFFFFFFFFC9, 0xFFFFFFFFFFFFFA3,
		0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFC7, 0x7FFFFFFFFFFFFFE7, 0xFFFFFFFFFFFFFFC5
	};
	
#elif defined LIBENV_SYS_INTELX86
	const size_t _HashBucket_Count_Candidate_ArrSize = 31;
	const size_t _HashBucket_Count_Candidate[] = {
		0x3, 0x7, 0xD, 0x1F, 0x3D, 0x7F, 0xFB, 0x1FD, 0x3FD, 0x7F7, 0xFFD, 0x1FFF, 0x3FFD,
		0x7FED, 0xFFF1, 0x1FFFF, 0x3FFFB, 0x7FFFF, 0xFFFFD, 0x1FFFF7, 0x3FFFFD, 0x7FFFF1,
		0xFFFFFD, 0x1FFFFD9, 0x3FFFFFB, 0x7FFFFD9, 0xFFFFFC7, 0x1FFFFFFD, 0x3FFFFFDD,
		0x7FFFFFFF, 0xFFFFFFFB
	};
#endif

	size_t _Next_HashBucket_Count(size_t CurBucketCount){
		UBINT Lo_Ptr = 0, Hi_Ptr = _HashBucket_Count_Candidate_ArrSize;

		if (CurBucketCount < _HashBucket_Count_Candidate[0])return _HashBucket_Count_Candidate[0];
		while (Lo_Ptr + 1 != Hi_Ptr){
			UBINT Mid_Ptr = (Lo_Ptr + Hi_Ptr) / 2;
			if (CurBucketCount == _HashBucket_Count_Candidate[Mid_Ptr])return _HashBucket_Count_Candidate[Mid_Ptr] + 1;
			else if (CurBucketCount > _HashBucket_Count_Candidate[Mid_Ptr])Lo_Ptr = Mid_Ptr;
			else Hi_Ptr = Mid_Ptr;
		}
		return _HashBucket_Count_Candidate[Hi_Ptr];
	}
}

#endif