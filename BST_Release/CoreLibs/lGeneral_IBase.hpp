/* Description: Basic object interface for large-scale object-oriented programming. DO NOT include this header directly.
 * Language:C++
 * Author:***
 *
 * This header provides an interface standard which faciliate the communication between instances of different class types.
 * Every class that uses this interface should inherit the struct nsBasic::IBase, which can be compared to the IUnknown interface in COM.
 *
 */

namespace nsBasic{
	//Simplified definition:

	//struct IBase{
	//	const void *(*_GetInterface)(IBase * const, const void *);
	//	bool(*on_Link_From)(IBase * const, IBase * const, void *);
	//	bool(*on_Link_Twrd)(IBase * const, IBase * const, void *);
	//	void(*on_UnLink_From)(IBase * const, IBase * const, void *);
	//	void(*on_UnLink_Twrd)(IBase * const, IBase * const, void *);
	//	UBINT RefCounter;
	//	UBINT UserData;
	//	void(*Destroy)(IBase * const);
	//};

	struct IBase{
		//struct SceneNode is a POD.
	public:
		const void *(*_GetInterface)(IBase * const, UBINT);
		//When the interface ID is given, this function returns the required interface of the current class.
		//An interface is a pointer to a structure which has functions as members and an IBase * as data, thus establish the connection between data and methods.

		//The following functions can be used to build up the relation between different class instances.
		//Imagine that every instance is a node in a directed graph. IBase provides function Link() and Unlink() to add and remove edges in the graph. Read the code for detailed implementatation.

		bool(*on_Link_From)(IBase * const, IBase * const, void *);
		//return true for success. can be nullptr when operation Link_From is prohibited.
		bool(*on_Link_Twrd)(IBase * const, IBase * const, void *);
		//return true for success. can be nullptr when operation Link_Twrd is prohibited.
		void(*on_UnLink_From)(IBase * const, IBase * const, void *);
		//must not fail. can be nullptr when no operation is required.
		void(*on_UnLink_Twrd)(IBase * const, IBase * const, void *);
		//must not fail. can be nullptr when no operation is required.
		UBINT RefCounter;
		//A counter which indicates that number of edges (references) incident to this instance.
		//(UBINT)-1 indicates that the reference counter is not available.

		template <typename T> inline T *GetInterface(){ return (T *)this->_GetInterface(this, __typeid(T)); }
		inline void ref_inc(){ if ((UBINT)-1 != this->RefCounter)this->RefCounter++; }
		inline void ref_dec(){
			if ((UBINT)-1 != this->RefCounter){
				if (0 == this->RefCounter)throw std::exception("Try to decrease the reference pointer when no reference exists.");
				this->RefCounter--;
			}
		}
		static bool Link(IBase *From, IBase *Twrd, void *ExtInfo){
			//suppose that [From] and [Twrd] are valid pointers
			if (nullptr != From->on_Link_From && nullptr != Twrd->on_Link_Twrd){
				if (true == From->on_Link_From(From,Twrd, ExtInfo)){
					if (true == Twrd->on_Link_Twrd(Twrd, From, ExtInfo)){
						Twrd->ref_inc();
						//a backward reference counter should be implemented in function on_Link_Twrd.
						return true;
					}
					else{
						From->on_UnLink_From(From, Twrd, ExtInfo);
						return false;
					}
				}
				else return false;
			}
			else return false;
		}
		static void UnLink(IBase *From, IBase *Twrd, void *ExtInfo){
			//we do not check whether this operation is valid here.
			if (nullptr != From->on_UnLink_From)From->on_UnLink_From(From, Twrd, ExtInfo);
			if (nullptr != Twrd->on_UnLink_Twrd)Twrd->on_UnLink_Twrd(Twrd, From, ExtInfo);
			Twrd->ref_dec();
		}
		static void DerefAndDestroy(IBase *From, IBase *Twrd, void *ExtInfo){
			if (nullptr != Twrd->on_UnLink_Twrd)Twrd->on_UnLink_Twrd(Twrd, From, ExtInfo);
			Twrd->ref_dec();
			if (0 == Twrd->RefCounter)Twrd->Destroy(Twrd);
		}

		UBINT UserData;

		void (*Destroy)(IBase * const);
		//this function encapsules the delete operator.
	};

#define INTERFACE_CALL(Interface,FuncName) if (nullptr != Interface && nullptr != Interface->FuncName)Interface->FuncName
}

