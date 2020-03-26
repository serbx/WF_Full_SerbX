#pragma once
#include "Maths.h"
#include "VirtualUtilite.h"
#include "XorStr.h"
class SSGE;
class I3DEngine;
class IEntitySystem;
class IEntityIt;
class IEntity;
class IEntityRenderProxy;
class IActor;
class IRenderer;
class IGameRules;
class ICharacterInstance;
class ISkeletonPose;
class IGame;
class IPhysicalWorld;
class IPhysicalEntity;
class  CVars;
class ISystem;
class IItem;
class IWeapon;
class gameMEM;
class IMovementController;
class ITimer;
template <typename t>
t f_virtual(PVOID f_base, __int64 f_index)
{
	return (*(t**)f_base)[f_index / 8];
}
#define STATIC_SSGE_Ru									0x141CBBE90		// ai_CompatibilityMode
#define STATIC_IGameFramework_Ru        				0x141FC5768			// Failed to create the GameFramework Interface!
#define STATIC_SSGE									0x141CBBFB0			// ai_CompatibilityMode 
#define STATIC_IGameFramework        				0x141FC5818			// Failed to create the GameFramework Interface! 140FB3239
class SSGE
{
public:
	IGame *pGetGame()// 0xC8 maybe C0
	{return *(IGame**)((DWORD64)this + 0xC8) ;}

	ISystem* getSystem() {return *(ISystem**)((DWORD64)this + 0xE8);}
	IRenderer* pGetRender() { return *(IRenderer**)((DWORD64)this + 0x48); }

	IPhysicalWorld* pGetIPhysicalWorld() { return *(IPhysicalWorld**)((DWORD64)this + 0x70); }
	IEntitySystem* pGetEntitySystem() { return *(IEntitySystem**)((DWORD64)this + 0xD8); };

	I3DEngine* pGet3DEngine() { return *(I3DEngine**)((DWORD64)this + 0xF8); };

	ITimer* pGetITimer() { return *(ITimer**)((DWORD64)this + 0xB8); };

	SSGE* GetSSGE() {return *(SSGE**)(STATIC_SSGE_Ru);}

	gameMEM* getGM() {
		return *(gameMEM**)((DWORD64)this + 0xC0);
	}
}; //Size=0x0100

class I3DEngine
{
public:	

	void SetPostEffectParam(const char* pParam, float fValue, bool bForceValue = false) {
	return f_virtual<void(__thiscall*)(PVOID, const char*, float, bool)>(this, 1144)(this, pParam, fValue, bForceValue);
	}; 

}; //Size=0x0008

class ITimer {
public:
	float GetCurrTime(ETimer which)
	{
		return f_virtual<float(__thiscall*)(PVOID, ETimer)>(this, 0x18)(this, which);
	}
};

class gameMEM {
public:
	void maxfps(gameMEM abc	 , const char* sysmax, int*aa ,DWORD64 dd) {
		f_virtual<void(__thiscall*)(PVOID, gameMEM, const char*, int*, DWORD64)>(this, 56)(this,abc ,sysmax,aa,dd);
	}
};


enum EEntityProxy
{
	ENTITY_PROXY_RENDER,
	ENTITY_PROXY_PHYSICS,
	ENTITY_PROXY_SCRIPT,
	ENTITY_PROXY_SOUND,
	ENTITY_PROXY_AI,
	ENTITY_PROXY_AREA,
	ENTITY_PROXY_BOIDS,
	ENTITY_PROXY_BOID_OBJECT,
	ENTITY_PROXY_CAMERA,
	ENTITY_PROXY_FLOWGRAPH,
	ENTITY_PROXY_SUBSTITUTION,
	ENTITY_PROXY_TRIGGER,
	ENTITY_PROXY_ROPE,
	ENTITY_PROXY_ENTITYNODE,

	ENTITY_PROXY_USER,

	// Always the last entry of the enum.	
	ENTITY_PROXY_LAST
};

enum ESilhouettesParams {
	eAllMap = 524312,
	eLimitDistance = 524296
};


struct IEntityProxy {};

struct IEntity
{
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual int GetFlags(); //
	virtual void Function7(); //
	virtual void Function8(); //
	virtual void Function9(); //
	virtual void Function10();// 
	virtual void SetName(const char* NewName); //
	virtual const char* GetName() const; //
	virtual void Function13(); //
	virtual void Function14(); //
	virtual void Function15();//
	virtual void Function16(); //
	virtual void Function17(); //
	virtual void Function18(); //
	virtual void Function19(); //
	virtual void Function20(); //
	virtual void Function21();//setPos(const Vec3& vPos); //
	virtual void Function22(); //
	virtual const Matrix34& GetWorldTM(); //
	virtual void Function24(); //
	virtual void Function25();
	virtual void Function26(); //
	virtual void setPos(const Vec3& vPos); //Function27();
	virtual void Function28(); //
	virtual void Function29(); //
	virtual void Function30(); //
	virtual void Function31();
	virtual void Function32();
	virtual void Function33(); //
	virtual void Function34(); //
	virtual void Function35(); //
	virtual void Function36(); //
	virtual void Function37(); //
	virtual void Function38(); //
	virtual void Function39(); //
	virtual void Function40();
	virtual void Function41(); //
	virtual void Function42(); //
	virtual void Function43(); //
	virtual void Function44(); //
	virtual void Function45(); //
	virtual void Function46(); //
	virtual void Function47(); //
	virtual void Function48(); //
	virtual void Function49(); //
	virtual void Function50(); //
	virtual void Function51(); //
	virtual void Function52(); //
	virtual void Function53(); //
	virtual void Function54(); //
	virtual int LoadCharacter(int nSlot, const char* sFilename, int nLoadFlags = 0);; //0x00DC
	virtual void Function56(); //EntityId GetId() const;; //0x00E0
	virtual void Function57(); //
	virtual void Function58(); //
	virtual IEntityProxy* GetProxy(EEntityProxy proxy) const;; //virtual void Function59(); //
	virtual void Function60();//
	virtual void Function61(); //
	virtual void Function62(); //
	virtual void FreeAllSlots(); //
	virtual void Function64(); //
	virtual void Function65(); //
	virtual void Function66(); //
	virtual void Function67(); //
	virtual void Function68(); //
	virtual void Function69(); ////;; //0x0114
	virtual void Function70(); //
	virtual void Function71(); //
	virtual void Function72(); //
	virtual void Function73(); //
	virtual void Function74();//
	virtual void SetFlags(int iFlags);; //0x012C
	virtual void Function76(); //
	virtual void Function77(); //
	virtual void Function78(); //
	virtual void Function79(); //
	virtual void Function80(); //
	virtual void Function81(); //
	virtual ICharacterInstance* GetCharacter(int nSlot);

	__int32 Get_Flags() { return *(int*)((DWORD64)this + 48); };

	int GetID() { return *(int*)((DWORD64)this + 0x18); };

	wchar_t* get_W_name() {
	return f_virtual<wchar_t*(__thiscall*)(PVOID)>(this, 96)(this);
	}
};

class IEntityIt
{
public:
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual IEntity* Next(); //

}; //Size=0x0008


class IEntitySystem
{
public:

	IEntityIt* GetEntityIterator() {
		return f_virtual<IEntityIt * (__thiscall*)(PVOID)>(this, 168)(this);
	}; //

}; //Size=0x0008
struct IEntityRenderProxy
{


	void SetHUDSilhouettesParams(float a, float r, float g, float b){
		return f_virtual<void(__thiscall*)(PVOID, float, float, float, float)>(this, 352)(this, a, r, g, b);
	};

	void SetRndFlags(int Flag) { *(int*)((DWORD64)this + 0x24) = Flag; }


};


class IGFW;

class IActorIterator
{
public:
	IActor* Next() {
		typedef IActor* (__fastcall* aNext)(PVOID64);
		return f_virtual<aNext>(this, 0x8)(this);
	}
};

struct IActorSystem
{
	
	IActor* GetActor(int entityId)
	{
		return f_virtual<IActor * (__thiscall*)(PVOID, int)>(this, 120)(this, entityId);
	};

	void CreateActorIterator(IActorIterator** ActorIterator) {
		using CreateActorIterator = VOID(__fastcall*)(PVOID64, IActorIterator**);
		return f_virtual<CreateActorIterator>(this, 144)(this, ActorIterator);
	};
};
struct IItemSystem
{
	IItem* GetItem(EntityId itemId) { 
		return f_virtual<IItem * (__thiscall*)(PVOID, EntityId)>(this, 0xB0)(this, itemId);
	}
		
	EntityId GiveItem(IActor* pActor, const char* item, bool sound, bool select = true, bool keepHistory = true, const char* setup = NULL, EEntityFlags entityFlags = (EEntityFlags)0)
	{
		return f_virtual<EntityId(__thiscall*)(PVOID, IActor*, const char*, bool, bool, bool, const char*)>(this, 0x118)(this, pActor, item, sound, select, keepHistory, setup);
	};
	void SetActorItemByName(IActor* pActor, const char* item, bool itemHistory) {
		f_virtual<void(__thiscall*)(PVOID, IActor*, const char*, bool)>(this, 0x120)(this, pActor, item, itemHistory);
	}
};
class IGFW
{
public:

	bool GetClientActor(IActor** pAct)
	{
		return f_virtual<bool(__thiscall*)(PVOID, IActor**)>(this, 1136 )(this, pAct);
	};

	IActorSystem* GetIActorSystem() {
		return f_virtual<IActorSystem * (__thiscall*)(PVOID)>(this, 200)(this);
	};

	IItemSystem* GetIItemSystem()
	{
		return f_virtual<IItemSystem * (__thiscall*)(PVOID)>(this, 0xD0)(this);
	}
	
	static IGFW* GetGameFramework() {//The highlited dword_... is the IGameFramework address.
		return *(IGFW**)STATIC_IGameFramework_Ru; //search Failed to create the GameFramework Interface!
	}

	IGameRules* GetIGameRules() { return f_virtual<IGameRules * (__thiscall*)(PVOID)>(this, 0x448)(this);

	}

}; //Size=0x0008

struct CPlayer
{
public:
	Vec3 m_modelOffset;
	char pad_0x000Ñ[0xA5]; //0x00B0
	bool m_ignoreRecoil; //0x00B1

	Quat GetViewRotation() {return m_viewQuatFinal;
	};
	void SetViewRotation(Quat& rotation)
	{
		m_baseQuat = rotation;
		m_viewQuat = rotation;
		m_viewQuatFinal = rotation;
	};
	void SetRotation(const Quat& Ros);

	char pad_0x000C[0x74]; //0x000C 
	Quat m_viewQuat; //0x0080 
	Quat m_viewQuatFinal; //0x0090 
	Quat m_baseQuat; //0x00A0 

};

struct IGameObject
{
	char pad_0x0000[0xB4];
	int m_predictionHandle;

	bool BindToNetwork();
	void DoInvokeRMI(PVOID body, ERMInvocation where, int channel);

		uint32_t TriggerId()
		{
	/*		uintptr_t offset_0 = *(uintptr_t*)(uintptr_t(this + 0x18));
			if (!offset_0)
				return 0;*/

			uintptr_t offset_1 = *(uintptr_t*)(uintptr_t(this + 0x60));
			if (!offset_1)
				return 0;

			uintptr_t offset_2 = *(uintptr_t*)(uintptr_t(offset_1 + 0x20));
			if (!offset_2)
				return 0;

			return *(uint32_t*)(uintptr_t(offset_2 + 0x170));
		}
	

	int32_t TriggerId1(void) {
		uintptr_t us = f_virtual< uintptr_t  (__thiscall*)(PVOID)>(this, 0x60)(this);
		if (us != 0) {
			uintptr_t ut = f_virtual< uintptr_t(__thiscall*)(PVOID)>(this, 0x20)(this);
			if (ut != 0)
				return f_virtual< int32_t(__thiscall*)(PVOID)>(this, 0x170)(this);
		}
		return 0;
	}

};
class IInventory {
public:
	EntityId GetItem(int slotId)
	{
		return CallFunction<EntityId(__thiscall*)(PVOID, int)>(this, 0x120)(this, slotId);
	}
	EntityId GetCurrentItemId()
	{
		return CallFunction<EntityId(__thiscall*)(PVOID)>(this, 0x170)(this);
	}
};
class IActor
{ 
public:

	char pad_0000[16]; //0x0000
	IEntity* m_pEntity; //0x0010
	IGameObject* m_pGameObject; //0x0018
	EntityId m_entityId; //0x0020		
	char pad_0024[3704]; //0x0024
	float claymore; //0x0E9C
	float claymore_activation_delay_mul; //0x0EA0

	EntityId m_EntityID() {
		return (EntityId)((DWORD64)this + 0x20);
	}

	CPlayer* GetPlayer() { return *(CPlayer**)((DWORD64)this + 0xD10);
	}

	IEntity* get_entity()
	{
		return *(IEntity**)((DWORD64)this + 16);
	}

	bool isDead()
	{
		return f_virtual<bool(__thiscall*)(PVOID)>(this, 192)(this);
	}
	__int32 get_team_id()
	{
		return *(__int32*)((DWORD64)this + 992);
	}

	int GetHealth() {
		return f_virtual<int(__thiscall*)(PVOID)>(this, 216)(this);
	};

	void SetHealth(int val) {
		return f_virtual<void(__thiscall*)(PVOID, int)>(this, 224)(this, val);
	}
	int  getMaxHealth() {
		return f_virtual<int(__thiscall*)(PVOID)>(this, 248)(this);
	}
	bool isDeadByH() {
		return GetHealth() <= 0;
	}


	IItem* GetCurrentItem(bool bIncludeVehicle)
	{
		return f_virtual<IItem * (__thiscall*)(PVOID, bool)>(this, 0x1C8)(this, bIncludeVehicle);
	}

	IMovementController* GetMovementController()
	{
		return f_virtual<IMovementController * (__thiscall*)(PVOID)>(this, 0x1E8)(this);
	}

	bool IsPlayer()
	{
		return CallFunction<bool(__thiscall*)(PVOID)>(this, 0x230)(this);
	}

	bool IsClient()
	{
		return CallFunction<bool(__thiscall*)(PVOID)>(this, 0x238)(this);
	}

	IInventory* GetInventory()
	{
		return CallFunction<IInventory * (__thiscall*)(PVOID)>(this, 0x1D8)(this);
	}

	void RecoilAnimation(int playAnim)
	{
		f_virtual<void(__thiscall*)(PVOID, int)>(this, (91*8))(this, playAnim); // 0 - Ignore Recoil
	}
};


struct IRenderer {

	struct w2s_info
	{
		float Posx;
		float Posy;
		float Posz;
		float* Scrnx;
		float* Scrny;
		float* Scrnz;
	};
 
	bool ProjectToScreen(w2s_info* info) {
	return f_virtual<bool(__thiscall*)(PVOID, w2s_info*)>(this, 800 )(this, info);
	};

	Vec3 GetViewCamera() {
		 return *(Vec3*)((DWORD64)this + 0x1700);
	}
};



struct ISkeletonPose
{
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual void Function5(); //
	virtual void Function6(); //
	virtual QuatT& GetBoneByID(int nJointID);//GetAbsJointByID rghtarm01

	EBone GetJointIDByNameParams(const char* boneName)
	{
		return f_virtual<EBone(__thiscall*)(PVOID, const char*)>(this, 0x18)(this, boneName);
	}
};
struct ICharacterInstance
{
	virtual void Function0(); //
	virtual void Function1(); //
	virtual void Function2(); //
	virtual void Function3(); //
	virtual void Function4(); //
	virtual ISkeletonPose* GetISkeletonPose();
};


struct ray_hit
{
	float dist;
	IPhysicalEntity* pCollider;
	int ipart;
	int partid;
	short surface_idx;
	short idmatOrg;			// original material index, not mapped with material mapping
	int foreignIdx;
	int iNode;				// BV tree node that had the intersection; can be used for "warm start" next time
	Vec3 pt;
	Vec3 n;					// surface normal
	int bTerrain;			// global terrain hit
	int iPrim;				// hit triangle index
	ray_hit* next;			// reserved for internal use, do not change
};
#pragma region IPhysicalEntity
#define FUNC_IPhysicalEntity_GetForeignData			0x68
#pragma endregion
class IPhysicalEntity
{
public:
	void* GetForeignData(int itype = 0)
	{
		return f_virtual<void* (__thiscall*)(PVOID, int)>(this, FUNC_IPhysicalEntity_GetForeignData)(this, itype);
	}
};

struct ray_hit_cached		// used in conjunction with rwi_reuse_last_hit
{
	IPhysicalEntity* pCollider;
	int ipart;
	int iNode;

	ray_hit_cached() { pCollider = 0; ipart = 0; }
	ray_hit_cached(const ray_hit& hit) { pCollider = hit.pCollider; ipart = hit.ipart; iNode = hit.iNode; }
	ray_hit_cached& operator=(const ray_hit& hit) { pCollider = hit.pCollider; ipart = hit.ipart; iNode = hit.iNode; return *this; }
};
struct EventPhysRWIResult;
class SRWIParams
{
public:
	void* pForeignData = 0;
	__int64 iForeignData = 0;
	int (*OnEvent)(const EventPhysRWIResult*);
	Vec3 org;
	Vec3 dir;
	__int32 objTypes;
	unsigned int flags;
	ray_hit* hits;
	__int32 nMaxHits;
	ray_hit_cached* phitLast = 0;
	__int32 nSkipEnts = 0;
	IPhysicalEntity** pSkipEnts = 0;
};
#pragma region IPhysicalworld
#define FUNC_IPhysicalWorld_RayWorldIntersection	0x118		// RayWorldIntersection(Game)
#define FUNC_IPhysicalWorld_RayWorldIntersectionParams			0x308
#pragma endregion
class IPhysicalWorld
{
public:
	int GetEntitiesAround(const Vec3& ptmin, const Vec3& ptmax, IPhysicalEntity**& pList, int objtypes, IPhysicalEntity* pPetitioner = NULL, int szListPrealloc = 0, int iCaller = 4);
	int RayWorldIntersection(const Vec3& org, const Vec3& dir, int objtypes, unsigned int flags, ray_hit* hits, int nMaxHits, IPhysicalEntity** pSkipEnts = 0, int nSkipEnts = 0, void* pForeignData = 0, int iForeignData = 0, const char* pNameTag = "RayWorldIntersection(Game)", ray_hit_cached* phitLast = 0, int iCaller = 4)
	{
		using oRayWorldIntersection = int(__fastcall*)(PVOID, const Vec3&, const Vec3&, int, unsigned int, ray_hit*, int, IPhysicalEntity**, int, void*, int, const char*, ray_hit_cached*, int);
		return f_virtual<oRayWorldIntersection>(this, FUNC_IPhysicalWorld_RayWorldIntersection)(this, org, dir, objtypes, flags, hits, nMaxHits, pSkipEnts, nSkipEnts, pForeignData, iForeignData, pNameTag, phitLast, iCaller);
	}

	int RayWorldIntersectionParams(const Vec3& org, const Vec3& dir, int objtypes, unsigned int flags, ray_hit* hits, int nMaxHits, IPhysicalEntity** pSkipEnts = 0, int nSkipEnts = 0, void* pForeignData = 0, int iForeignData = 0, const char* pNameTag = "RayWorldIntersection(Game)", ray_hit_cached* phitLast = 0, int iCaller = 4)
	{
		using oRayWorldIntersection = int(__fastcall*)(PVOID, const Vec3&, const Vec3&, int, unsigned int, ray_hit*, int, IPhysicalEntity**, int, void*, int, const char*, ray_hit_cached*, int);
		return f_virtual<oRayWorldIntersection>(this, FUNC_IPhysicalWorld_RayWorldIntersectionParams)(this, org, dir, objtypes, flags, hits, nMaxHits, pSkipEnts, nSkipEnts, pForeignData, iForeignData, pNameTag, phitLast, iCaller);
	}
	Vec3 GetMPos(Vec3 BoneID, Vec3 CamPos, FLOAT Correct)
	{
		Vec3 vOut = BoneID - CamPos;
		FLOAT sqr = (FLOAT)sqrt(vOut.x * vOut.x + vOut.y * vOut.y + vOut.z * vOut.z);
		vOut /= sqr;
		vOut *= sqr - Correct;
		return vOut;
	}
	BOOL IsVisible(Vec3 BoneID, FLOAT Correct, Vec3 MyGetViewCamera)
	{
		ray_hit tmpHit;
		//Vec3 vOut(GetMPos(BoneID, MyGetViewCamera, Correct));
		return !this->RayWorldIntersectionParams(MyGetViewCamera, BoneID - MyGetViewCamera /*vOut*/, 0x100 | 1, 0xA | 0x400, &tmpHit, 1);
	}
	INT IsVisible(Vec3 EnemyPos, Vec3 MyPos)
	{
		Vec3 vTemp;
		ray_hit tmpHit;
		vTemp.x = EnemyPos.x - MyPos.x;
		vTemp.y = EnemyPos.y - MyPos.y;
		vTemp.z = EnemyPos.z - MyPos.z;
		return !this->RayWorldIntersection(MyPos, vTemp, 779, (10 & 0x0F), &tmpHit, 1);
	}
	/*void WallShot() {
		ray_hit tmpHit;
		Vec3 vOut, v,v1;
		this->RayWorldIntersection(v, v1, 16, 0xA | 0x400, &tmpHit, 1);
	}*/
};

class CVars {
public:

	VOID i_pelletsDisp(FLOAT Value) {
		*(FLOAT*)((DWORD64)this + 0x124) = Value;
	}

	CVars* GetCVars() { return *(CVars**)(0x141F22C50); }
};

class ISystem {
public:

	Vec3 CamPos()
	{
		return *(Vec3*)((DWORD64)this + 0x7F0);
	}

	bool Update(int updateFlags = 0, int nPauseMode = 0) {
		return f_virtual<bool(__fastcall*)(PVOID, int, int)>(this,4)(this, updateFlags, nPauseMode);
		
	};
};

class IWeaponParamsOne
{
public:
	char _0x00[0x10];
	__int32 m_iPellets;				// 0x10
};
class IWeaponParamsTwo {};
class IWeaponParamsThree
{
public:
	char pad_0000[8]; //0x0000
	bool m_bUnlimitedAmmo; //0x0008
	char pad_0009[3]; //0x0009
	uint32_t m_iAmmo; //0x000C
};
class IWeaponPTR
{
public:

	char pad_0000[56]; //0x0000
	IItem* m_pItem; //0x0038
	char pad_0040[8]; //0x0040
	IWeaponParamsOne* GetIWeaponParamOne; //0x0048
	char pad_0050[16]; //0x0050
	IWeaponParamsTwo* GetWeaponParamsTwo; //0x0060
	IWeaponParamsThree* GetIWeaponParamThree; //0x0068
	void GetShutter(int8 Value)
	{
		*(int8*)((DWORD64)this + 0xD2) = Value;
	}

};

#define FUNC_IWeapon_StartFire						0x58		// CScriptBind_Weapon::AutoShoot (first)
#define FUNC_IWeapon_StopFire						0x60		// CScriptBind_Weapon::AutoShoot (second)
#define FUNC_IWeapon_Reload							0x98		// Reload
#define FUNC_IWeapon_GetCrosshairVisibility			0xC8		// 
class IWeapon {
public:
	void Reload(bool force)
	{
		f_virtual<void(__thiscall*)(PVOID, bool)>(this, 0x98)(this, force);
	}
	IWeaponPTR* pWeaponPTR()
	{
		return (IWeaponPTR*)*(DWORD64*)((DWORD64)this + 0x8);
	}
	void SetFiringPos(Vec3 vPos)
	{
		if (DWORD64 v0 = *(DWORD64*)((DWORD64)this + 0x18))*(Vec3*)(v0 + 0x1C) = vPos;
	}

	void StartFire()
	{
		CallFunction<void(__thiscall*)(PVOID)>(this, FUNC_IWeapon_StartFire)(this);
	}

	void StopFire()
	{
		CallFunction<void(__thiscall*)(PVOID)>(this, FUNC_IWeapon_StopFire)(this);
	}

	bool GetCrosshairVisibility()
	{
		return CallFunction<bool(__thiscall*)(PVOID)>(this, FUNC_IWeapon_GetCrosshairVisibility)(this);
	}
};
class IItem {
public:
	IWeapon* GetIWeapon()
	{
		return f_virtual<IWeapon * (__thiscall*)(PVOID)>(this, 0x278)(this);
	}
	DWORD getItemId() { return m_dwItemId; }
private:
	char pad_0x0000[0x10];			//0x0000
	IEntity* m_pEntity;				//0x0010
	IGameObject* m_pGameObject;		//0x0018
	EntityId m_entityId;			//0x0020
	char pad_0x0024[0x28];			//0x0024
	DWORD m_dwItemId;				//0x004C
};

class IGame {
public:
	void SetPlayer(int GameFunction, float Value)
	{
		switch (GameFunction)
		{
		case ClimbExtraHeight:
			if (DWORD64 v0 = *(DWORD64*)(*(DWORD64*)(*(DWORD64*)((DWORD64)this + 0x290) + 0x8) + 0x8))
			{
				*(float*)(v0 + 0x1C) = 2.25f + Value;
			}
			break;
		case SlideMultDistance:
			if (DWORD64 v0 = *(DWORD64*)(*(DWORD64*)(*(DWORD64*)((DWORD64)this + 0x290) + 0x8) + 0x10))
			{
				*(float*)(v0 + 0x54) = Value;
			}
			break;
		case AntiAfk:
			if (DWORD64 v0 = *(DWORD64*)(*(DWORD64*)((DWORD64)this + 0x320) + 0x10))
			{
				*(float*)(v0) = Value;
			}
			break;
		}
	}

};

struct SStanceState
{
	Vec3 pos;//0-8
	Vec3 entityDirection;//c-14
	Vec3 animationBodyDirection;//18-20
	Vec3 upDirection;//24-2C
	Vec3 weaponPosition;//30-38
	Vec3 aimDirection;//3C-44
	Vec3 fireDirection;//48-50
	Vec3 eyePosition;//54-5C
	Vec3 eyeDirection;//60-68
	float lean;//6C
	float peekOver;//70
	AABB m_StanceSize;//74-88
	AABB m_ColliderSize;//8C-94,98-A0
};

struct SMovementState : public SStanceState
{
	DWORD64 Unk;
	EStance stance;
	Vec3 animationEyeDirection;
	Vec3 movementDirection, lastMovementDirection;
	float desiredSpeed;
	float minSpeed;
	float normalSpeed;
	float maxSpeed;
	float slopeAngle;
	bool atMoveTarget;
	bool isAlive;
	bool isAiming;
	bool isFiring;
	bool isVisible;
	bool isMoving;
};
class CMovementRequest {};

class IMovementController
{
public:
	virtual ~IMovementController() {};
	virtual bool RequestMovement(CMovementRequest& request) = 0;
	virtual void GetMovementState(SMovementState& state) = 0;
	virtual bool GetStanceState() = 0;
};


struct HitInfo
{
public:
	EntityId shooterId; //0x0000 
	EntityId targetId; //0x0004 
	char pad_0008[8]; //0x0008 
	int material = -1; //0x0010 
	int type; //0x0014 
	int bulletType = -1; //0x0018 
	int partId; //0x001C 
	Vec3 pos; //0x0020 
	Vec3 dir; //0x002C 
	Vec3 shootPos; //0x0038 
	bool meleeArmorAbsorb; //0x0044 
	char pad_0045[3]; //0x0045 
	DWORD itemId = 0; //0x0048 
	DWORD projectileId = 0; //0x004C 
	WORD itemType = -1; // 0x0050 
	char pad_0052[6]; //0x0052 
	Vec3 normal; //0x0058 
	char pad_0064[4]; //0x0064 
	DWORD64 dw_0x68 = (unsigned int)(signed int)0.0f; //0x0068 
	bool b_0x70; //0x0070 
	char pad_0071[7]; //0x0071 
	float distance; //0x0078 
	DWORD dw_0x7C; //0x0078 
	float timeStamp; //0x0080 
	WORD w_0x84 = 256; //0x0084 
};

class IGameRules {
public:

	int GetHitMaterialId(const char* materialName)
	{
		return f_virtual<int(__thiscall*)(PVOID, const char*)>(this, 0x388)(this, materialName);
	}
	int GetHitTypeId(const char* type)
	{
		return f_virtual<int(__thiscall*)(PVOID, const char*)>(this, 0x140)(this, type);
	}
	void RequestHit(HitInfo* pHitInfo)
	{
		f_virtual<void(__thiscall*)(PVOID, HitInfo*)>(this, 0x4D8)(this, pHitInfo);
	}
	void RequestHit1(HitInfo* pHitInfo)
	{
		f_virtual<void(__thiscall*)(PVOID, HitInfo*)>(this, 155)(this, pHitInfo);
	}
	void RenamePlayer(EntityId PlayerID , const char * name)
	{
		f_virtual<void(__thiscall*)(PVOID, EntityId, const char*)>(this, 88)(this, PlayerID,name);
	}
	
};

inline void KillAura(IActor* MActor,IEntity* pTarget, DWORD dwProjectileId, int nPartId, Vec3 vPos , IGFW* pGameFwork,SSGE* pkSSGE,const char* HTp = XorString("bullet"))
 {
	IGameRules* pGameRules = pGameFwork->GetIGameRules();
	if (!pGameRules) return;

	IItem* pCurItem = MActor->GetCurrentItem(false);
	if (!pCurItem) return;
	IMovementController* pMovementController = MActor->GetMovementController();
	if (!pMovementController) return;

	SMovementState movement;
	pMovementController->GetMovementState(movement);


	HitInfo hitInfo;
	hitInfo.shooterId = MActor->m_entityId;
	hitInfo.targetId = pTarget->GetID();
	hitInfo.projectileId = dwProjectileId;
	hitInfo.material = pGameRules->GetHitMaterialId(XorString("mat_head"));
	hitInfo.type = pGameRules->GetHitTypeId(HTp);
	hitInfo.partId = nPartId;
	hitInfo.pos = vPos;
	hitInfo.dir = hitInfo.pos - movement.weaponPosition;
	hitInfo.distance = /*hitInfo.dir.GetLength()*/ 1.f;
	hitInfo.dir = hitInfo.dir / hitInfo.distance;
	hitInfo.shootPos = movement.weaponPosition;
	hitInfo.itemId = pCurItem->getItemId();
	hitInfo.itemType = 710;
	hitInfo.normal = Vec3{ hitInfo.dir.x * -1.f,hitInfo.dir.y * -1.f,hitInfo.dir.z * -1.f };
	hitInfo.timeStamp = pkSSGE->pGetITimer()->GetCurrTime(ETIMER_GAME) * 1000.f;
	pGameRules->RequestHit(&hitInfo);
}
inline void KillAura1(IActor* MActor, IEntity* pTarget, DWORD dwProjectileId, int nPartId, Vec3 vPos, IGFW* pGameFwork, const char * mat, SSGE* pkSSGE, const char* HTp = XorString("bullet"),int bull_ID = 710)
{
	IGameRules* pGameRules = pGameFwork->GetIGameRules();
	if (!pGameRules) return;

	IItem* pCurItem = MActor->GetCurrentItem(false);
	if (!pCurItem) return;
	IMovementController* pMovementController = MActor->GetMovementController();
	if (!pMovementController) return;

	SMovementState movement;
	pMovementController->GetMovementState(movement);


	HitInfo hitInfo;
	hitInfo.shooterId = MActor->m_entityId;
	hitInfo.targetId = pTarget->GetID();
	hitInfo.projectileId = dwProjectileId;
	hitInfo.material = pGameRules->GetHitMaterialId(mat);
	hitInfo.type = pGameRules->GetHitTypeId(HTp);
	hitInfo.partId = nPartId;
	hitInfo.pos = vPos;
	hitInfo.dir = hitInfo.pos - movement.weaponPosition;
	hitInfo.distance = /*hitInfo.dir.GetLength()*/ 1.f;
	hitInfo.dir = hitInfo.dir / hitInfo.distance;
	hitInfo.shootPos = movement.weaponPosition;
	hitInfo.itemId = pCurItem->getItemId();
	hitInfo.itemType = bull_ID;
	hitInfo.normal = Vec3{ hitInfo.dir.x * -1.f,hitInfo.dir.y * -1.f,hitInfo.dir.z * -1.f };
	hitInfo.timeStamp = pkSSGE->pGetITimer()->GetCurrTime(ETIMER_GAME) * 1000.f;
	pGameRules->RequestHit(&hitInfo);
}
inline __int8 Find_AliveID(std::vector<IActor*> ACtList) {
	int ret = -1;
	for (size_t i = 0; i < ACtList.size(); i++)
	{
		if (ACtList[i]->GetHealth() > 0) {
			ret = i;
			break;
		}
		else continue;
	}
	return ret;
}

inline __int8 Find_AliveID(std::vector<IEntity*> EntList,IGFW* pgf) {
	int ret = -1;
	for (size_t i = 0; i < EntList.size() ; i++)
	{
		if (pgf->GetIActorSystem()->GetActor(EntList[i]->GetID())->GetHealth()>0) {
			ret = i;
			break;
		}
		else continue;
	}
	return ret;
}
class WeaponRender {
public:
	char* name;
	Vec3 Pos , W2sPos, CamPos;
};

class NameRender {
public:
	const char* name;
	int x, y;
};

inline bool isEnemyTeam(int EntTeam, int Myteam) {
	return (EntTeam != Myteam || (Myteam == 0 && EntTeam == 0));
}


inline void DrawT(ID3DXFont* pfont, int x, int y, DWORD color, wchar_t* text)
{
	RECT rect;
	SetRect(&rect, x, y, x, y);
	pfont->DrawTextW(0, text, -1, &rect, DT_NOCLIP | DT_LEFT, color);
};

#include <clocale>
#include <locale>
#include <vector>

inline std::string wchar_t_2_str(std::wstring w_srt) {
	std::setlocale(LC_ALL, "");
	const std::wstring ws = w_srt; // ħëłlö
	const std::locale locale("");
	typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
	const converter_type& converter = std::use_facet<converter_type>(locale);
	std::vector<char> to(ws.length() * converter.max_length());
	std::mbstate_t state;
	const wchar_t* from_next;
	char* to_next;
	const converter_type::result result = converter.out(state, ws.data(), ws.data() + ws.length(), from_next, &to[0], &to[0] + to.size(), to_next);
	if (result == converter_type::ok || result == converter_type::noconv) {
		const std::string s(&to[0], to_next);
		return s;
	}
	return nullptr;
}

