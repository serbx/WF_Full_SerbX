#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <assert.h>      
#include <cmath> 
#include <math.h> 
#include <d3dx9.h>
#include "intrin.h"

#ifdef _WIN64
#define _pdword      PDWORD64
#define _dword       DWORD64
#define _pvoid		 PVOID64
#define _FLOAT		 double
#define _INT		 signed __int64
#define _UINT		 unsigned __int64
#define _Division 8
#else
#define _pdword       PDWORD
#define _dword        DWORD
#define _pvoid		  PVOID
#define _FLOAT		  float
#define _INT		  signed  int
#define _UINT		  unsigned int
#define _Division 4
#endif

#pragma region Defines
#define DEFAULT_HIT_MATERIAL "mat_head" 
#define LOW_SIGNAL_FREQ 250
#define HIGH_SIGNAL_FREQ 500
#define SIGNAL_LENGTH_MS 100
#define ElementPos(y) 7+23*y
#define ElementPosX(x) 7+23*x
#define COLLISION_RAY_PIERCABILITY 10
#define RWI_RAY_PIERCABILITY_MASK  0x0F
#define CRY_ALIGN(bytes) __declspec(align(bytes))
#define ReadMemory(Type, dwAddress) *(Type*)((DWORD)dwAddress)
#define ReadAddress(Type, address) (*(Type**)address)
#define fCall(name, type, offset) void name(type value){ *(type*)((DWORD64)this + offset) = value; }
#define get_iCaller_int 4
#define ILINE __forceinline
#define IF(condition, hint)    if (condition)
#define BIT(x) (1<<(x))
#define CRY_MATH_ASSERT(x)
#define UP_DIRECTION Vec3(0.0f, 0.0f, 1.0f)
#define FORWARD_DIRECTION Vec3(0.0f, 1.0f, 0.0f)
#define VEC_EPSILON (0.05f)
#define RAD_EPSILON (0.01f)
#define DEG2RAD(a)   ((a) * (gf_PI / 180.0f))
#define RAD2DEG(a)   ((a) * (180.0f / gf_PI))
#define DEG2COS(a)   (cos_tpl((a) * (gf_PI / 180.0f)))
#define COS2DEG(a)   (acos_tpl(a) * (180.0f / gf_PI))
#define RAD2HCOS(a)  (cos_tpl((a * 0.5f)))
#define HCOS2RAD(a)  (acos_tpl(a) * 2.0f)
#define DEG2HCOS(a)  (cos_tpl((a * 0.5f) * (gf_PI / 180.0f)))
#define DEG2HSIN(a)  (sin_tpl((a * 0.5f) * (gf_PI / 180.0f)))
#define HCOS2DEG(a)  (acos_tpl(a) * 2.0f * (180.0f / gf_PI))
#define SIGN_MASK(x) ((intptr_t)(x) >> ((sizeof(size_t) * 8) - 1))
#define TANGENT30   0.57735026918962576450914878050196f     // tan(30)
#define TANGENT30_2 0.57735026918962576450914878050196f * 2 // 2*tan(30)
#define LN2         0.69314718055994530941723212145818f     // ln(2)
#define FloatU32ExpMask    (0xFF << 23)
#ifndef CRY_ASSERT_H_
#define CRY_ASSERT_H_
#ifdef CRY_ASSERT_CHECK

#ifndef CRY_ASSERT_FAIL
#define CRY_ASSERT_FAIL(cond) \
        cry_assert_fail(#cond, __FILE__, __LINE__)
#endif

#define CRY_ASSERT_RES(cond, res) do { \
    if (!(cond)) { \
        CRY_ASSERT_FAIL(cond); \
        return (res); \
    } \
} while (0)

#else

#define CRY_ASSERT_RES(cond, res) ((void)0)

#endif /* ~CRY_ASSERT_CHECK */

#define CRY_ASSERT(cond)    CRY_ASSERT_RES(cond, (void)0)
#define CRY_ASSERT2(cond)   CRY_ASSERT_RES(cond, -1)

#endif 
#pragma endregion
#pragma region typedef
typedef unsigned int        EntityId;
typedef signed char         int8;
typedef signed short        int16;
typedef signed int			int32;
typedef signed __int64		int64;
typedef unsigned char		uint8;
typedef unsigned char		BYTE;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned __int64	uint64;
typedef float               f32;
typedef double              f64;
typedef std::map<INT32, INT32[2]> TWeaponMap;
typedef std::map<const char*, const char*> THitMaterialsMap;
#pragma endregion
#pragma region Enums
enum {
	FR_PLANE_NEAR,
	FR_PLANE_FAR,
	FR_PLANE_RIGHT,
	FR_PLANE_LEFT,
	FR_PLANE_TOP,
	FR_PLANE_BOTTOM,
	FRUSTUM_PLANES
};
enum cull {
	CULL_EXCLUSION,     //!< The whole object is outside of frustum.
	CULL_OVERLAP,       //!< The object &  frustum overlap.
	CULL_INCLUSION      //!< The whole object is inside frustum.
};
enum Stance {
	None = -1,
	Stand = 0,
	Crouch,
	Prone,
	Relaxed,
	Stealth,
	Cover_Low,
	Cover_High,
	Swim
};
enum ERMInvocation : DWORD64
{
	eRMI_ToClientChannel = 0x01,
	eRMI_ToOwnClient = 0x02,
	eRMI_ToOtherClients = 0x04,
	eRMI_ToAllClients = 0x08,

	eRMI_ToServer = 0x100,

	eRMI_NoLocalCalls = 0x10000,
	eRMI_NoRemoteCalls = 0x20000,

	eRMI_ToRemoteClients = eRMI_NoLocalCalls | eRMI_ToAllClients
};
enum EActorSpectatorMode
{
	eASM_None = 0,												// normal, non-spectating

	eASM_FirstMPMode,
	eASM_Fixed = eASM_FirstMPMode,				// fixed position camera
	eASM_Free,														// free roaming, no collisions
	eASM_Follow,													// follows an entity in 3rd person
	eASM_Killer,													// Front view of the killer in 3rdperson.
	eASM_LastMPMode = eASM_Killer,

	eASM_Cutscene,												// HUDInterfaceEffects.cpp sets this
};
enum draw_helper_flags {
	pe_helper_collisions = 1,
	pe_helper_geometry = 2,
	pe_helper_bbox = 4,
	pe_helper_lattice = 8
};
enum surface_flags {
	sf_pierceable_mask = 0x0F,
	sf_max_pierceable = 0x0F,
	sf_important = 0x200,
	sf_manually_breakable = 0x400,
	sf_matbreakable_bit = 16
};
enum EEntitySerializeFlags {
	ENTITY_SERIALIZE_PROXIES = BIT(1),    //!< Serialize proxies.
	ENTITY_SERIALIZE_POSITION = BIT(2),    //!< Serialize properties common to all entities (position, rotation, scale).
	ENTITY_SERIALIZE_ROTATION = BIT(3),
	ENTITY_SERIALIZE_SCALE = BIT(4),
	ENTITY_SERIALIZE_GEOMETRIES = BIT(5),
	ENTITY_SERIALIZE_PROPERTIES = BIT(6),
};
enum EEntityGetSetSlotFlags {
	ENTITY_SLOT_ACTUAL = 1 << 31
};
enum EEntityFlags {
	ENTITY_FLAG_CASTSHADOW = BIT(1),
	ENTITY_FLAG_UNREMOVABLE = BIT(2),        //!< This entity cannot be removed using IEntitySystem::RemoveEntity until this flag is cleared.
	ENTITY_FLAG_GOOD_OCCLUDER = BIT(3),
	ENTITY_FLAG_NO_DECALNODE_DECALS = BIT(4),
	ENTITY_FLAG_WRITE_ONLY = BIT(5),
	ENTITY_FLAG_NOT_REGISTER_IN_SECTORS = BIT(6),
	ENTITY_FLAG_CALC_PHYSICS = BIT(7),
	ENTITY_FLAG_CLIENT_ONLY = BIT(8),
	ENTITY_FLAG_SERVER_ONLY = BIT(9),
	ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO = BIT(10),  //!< This entity have special custom view distance ratio (AI/Vehicles must have it).
	ENTITY_FLAG_CALCBBOX_USEALL = BIT(11),  //!< use character and objects in BBOx calculations.
	ENTITY_FLAG_VOLUME_SOUND = BIT(12),  //!< Entity is a volume sound (will get moved around by the sound proxy).
	ENTITY_FLAG_HAS_AI = BIT(13),  //!< Entity has an AI object.
	ENTITY_FLAG_TRIGGER_AREAS = BIT(14),  //!< This entity will trigger areas when it enters them.
	ENTITY_FLAG_NO_SAVE = BIT(15),  //!< This entity will not be saved.
	ENTITY_FLAG_CAMERA_SOURCE = BIT(16),  //!< This entity is a camera source.
	ENTITY_FLAG_CLIENTSIDE_STATE = BIT(17),  //!< Prevents error when state changes on the client and does not sync state changes to the client.
	ENTITY_FLAG_SEND_RENDER_EVENT = BIT(18),  //!< When set entity will send ENTITY_EVENT_RENDER every time its rendered.
	ENTITY_FLAG_NO_PROXIMITY = BIT(19),  //!< Entity will not be registered in the partition grid and can not be found by proximity queries.
	ENTITY_FLAG_PROCEDURAL = BIT(20),  //!< Entity has been generated at runtime.
	ENTITY_FLAG_UPDATE_HIDDEN = BIT(21),  //!< Entity will be update even when hidden.
	ENTITY_FLAG_NEVER_NETWORK_STATIC = BIT(22),  //!< Entity should never be considered a static entity by the network system.
	ENTITY_FLAG_IGNORE_PHYSICS_UPDATE = BIT(23),  //!< Used by Editor only, (don't set).
	ENTITY_FLAG_SPAWNED = BIT(24),  //!< Entity was spawned dynamically without a class.
	ENTITY_FLAG_SLOTS_CHANGED = BIT(25),  //!< Entity's slots were changed dynamically.
	ENTITY_FLAG_MODIFIED_BY_PHYSICS = BIT(26),  //!< Entity was procedurally modified by physics.
	ENTITY_FLAG_OUTDOORONLY = BIT(27),  //!< Same as Brush->Outdoor only.
	ENTITY_FLAG_SEND_NOT_SEEN_TIMEOUT = BIT(28),  //!< Entity will be sent ENTITY_EVENT_NOT_SEEN_TIMEOUT if it is not rendered for 30 seconds.
	ENTITY_FLAG_RECVWIND = BIT(29),  //!< Receives wind.
	ENTITY_FLAG_LOCAL_PLAYER = BIT(30),
	ENTITY_FLAG_AI_HIDEABLE = BIT(31),  //!< AI can use the object to calculate automatic hide points.
};
enum rwi_flags {
	rwi_ignore_terrain_holes = 0x20, rwi_ignore_noncolliding = 0x40, rwi_ignore_back_faces = 0x80, rwi_ignore_solid_back_faces = 0x100,
	rwi_pierceability_mask = 0x0F, rwi_pierceability0 = 0, rwi_stop_at_pierceable = 0x0F,
	rwi_separate_important_hits = sf_important, //!< among pierceble hits, materials with sf_important will have priority
	rwi_colltype_bit = 16,           //!< used to manually specify collision geometry types (default is geom_colltype_ray)
	rwi_colltype_any = 0x400,        //!< if several colltype flag are specified, switches between requiring all or any of them in a geometry
	rwi_queue = 0x800,        //!< queues the RWI request, when done it'll generate EventPhysRWIResult
	rwi_force_pierceable_noncoll = 0x1000,       //!< non-colliding geometries will be treated as pierceable regardless of the actual material
	rwi_update_last_hit = 0x4000,       //!< update phitLast with the current hit results (should be set if the last hit should be reused for a "warm" start)
	rwi_any_hit = 0x8000        //!< returns the first found hit for meshes, not necessarily the closets
};
enum phentity_flags {
	particle_single_contact = 0x01,  //!< Full stop after first contact.
	particle_constant_orientation = 0x02,  //!< Forces constant orientation.
	particle_no_roll = 0x04,  //!< 'sliding' mode; entity's 'normal' vector axis will be alinged with the ground normal.
	particle_no_path_alignment = 0x08,  //!< Unless set, entity's y axis will be aligned along the movement trajectory.
	particle_no_spin = 0x10,  //!< Disables spinning while flying.
	particle_no_self_collisions = 0x100, //!< Disables collisions with other particles.
	particle_no_impulse = 0x200, //!< Particle will not add hit impulse (expecting that some other system will).

	lef_push_objects = 0x01, lef_push_players = 0x02, //!< Push objects and players during contacts.
	lef_snap_velocities = 0x04,                          //!< Quantizes velocities after each step (was ised in MP for precise deterministic sync).
	lef_loosen_stuck_checks = 0x08,                          //!< Don't do additional intersection checks after each step (recommended for NPCs to improve performance).
	lef_report_sliding_contacts = 0x10,                          //!< Unless set, 'grazing' contacts are not reported.

	rope_findiff_attached_vel = 0x01,                              //!< Approximate velocity of the parent object as v = (pos1-pos0)/time_interval.
	rope_no_solver = 0x02,                              //!< No velocity solver; will rely on stiffness (if set) and positional length enforcement.
	rope_ignore_attachments = 0x4,                               //!< No collisions with objects the rope is attached to.
	rope_target_vtx_rel0 = 0x08, rope_target_vtx_rel1 = 0x10, //!< Whether target vertices are set in the parent entity's frame.
	rope_subdivide_segs = 0x100,                             //!< Turns on 'dynamic subdivision' mode (only in this mode contacts in a strained state are handled correctly).
	rope_no_tears = 0x200,                             //!< Rope will not tear when it reaches its force limit, but stretch.
	rope_collides = 0x200000,                          //!< Rope will collide with objects other than the terrain.
	rope_collides_with_terrain = 0x400000,                          //!< Rope will collide with the terrain.
	rope_collides_with_attachment = 0x80,                              //!< Rope will collide with the objects it's attached to even if the other collision flags are not set.
	rope_no_stiffness_when_colliding = 0x10000000,                        //!< Rope will use stiffness 0 if it has contacts.
	se_skip_longest_edges = 0x01, //!< the longest edge in each triangle with not participate in the solver
	se_rigid_core = 0x02, //!< soft body will have an additional rigid body core

	ref_use_simple_solver = 0x01,                                //!< use penalty-based solver (obsolete)
	ref_no_splashes = 0x04,                                //!< will not generate EventPhysCollisions when contacting water
	ref_checksum_received = 0x04, ref_checksum_outofsync = 0x08, //!< obsolete
	ref_small_and_fast = 0x100,                               //!< entity will trace rays against alive characters; set internally unless overriden																																									
	aef_recorded_physics = 0x02, //!< specifies a an entity that contains pre-baked physics simulation
	wwef_fake_inner_wheels = 0x08, //!< exclude wheels between the first and the last one from the solver
	pef_parts_traceable = 0x10,                                                      //!< each entity part will be registered separately in the entity grid
	pef_disabled = 0x20,                                                      //!< entity will not be simulated
	pef_never_break = 0x40,                                                      //!< entity will not break or deform other objects
	pef_deforming = 0x80,                                                      //!< entity undergoes a dynamic breaking/deforming
	pef_pushable_by_players = 0x200,                                                     //!< entity can be pushed by playerd
	pef_traceable = 0x400, particle_traceable = 0x400, rope_traceable = 0x400, //!< entity is registered in the entity grid
	pef_update = 0x800,                                                     //!< only entities with this flag are updated if ent_flagged_only is used in TimeStep()
	pef_monitor_state_changes = 0x1000,                                                    //!< generate immediate events for simulation class changed (typically rigid bodies falling asleep)
	pef_monitor_collisions = 0x2000,                                                    //!< generate immediate events for collisions
	pef_monitor_env_changes = 0x4000,                                                    //!< generate immediate events when something breaks nearby
	pef_never_affect_triggers = 0x8000,                                                    //!< don't generate events when moving through triggers
	pef_invisible = 0x10000,                                                   //!< will apply certain optimizations for invisible entities
	pef_ignore_ocean = 0x20000,                                                   //!< entity will ignore global water area
	pef_fixed_damping = 0x40000,                                                   //!< entity will force its damping onto the entire group
	pef_monitor_poststep = 0x80000,                                                   //!< entity will generate immediate post step events
	pef_always_notify_on_deletion = 0x100000,                                                  //!< when deleted, entity will awake objects around it even if it's not referenced (has refcount 0)
	pef_override_impulse_scale = 0x200000,                                                  //!< entity will ignore breakImpulseScale in PhysVars
	pef_players_can_break = 0x400000,                                                  //!< playes can break the entiy by bumping into it
	pef_cannot_squash_players = 0x10000000,                                                //!< entity will never trigger 'squashed' state when colliding with players
	pef_ignore_areas = 0x800000,                                                  //!< entity will ignore phys areas (gravity and water)
	pef_log_state_changes = 0x1000000,                                                 //!< entity will log simulation class change events
	pef_log_collisions = 0x2000000,                                                 //!< entity will log collision events
	pef_log_env_changes = 0x4000000,                                                 //!< entity will log EventPhysEnvChange when something breaks nearby
	pef_log_poststep = 0x8000000,                                                 //!< entity will log EventPhysPostStep events
};
enum entity_query_flags {
	ent_static = 1, ent_sleeping_rigid = 2, ent_rigid = 4, ent_living = 8, ent_independent = 16, ent_deleted = 128, ent_terrain = 0x100,
	ent_all = ent_static | ent_sleeping_rigid | ent_rigid | ent_living | ent_independent | ent_terrain,
	ent_flagged_only = pef_update, ent_skip_flagged = pef_update * 2, //!< "flagged" meas has pef_update set
	ent_areas = 32, ent_triggers = 64,
	ent_ignore_noncolliding = 0x10000,
	ent_sort_by_mass = 0x20000,  //!< sort by mass in ascending order
	ent_allocate_list = 0x40000,  //!< if not set, the function will return an internal pointer
	ent_addref_results = 0x100000, //!< will call AddRef on each entity in the list (expecting the caller call Release)
	ent_water = 0x200,    //!< can only be used in RayWorldIntersection
	ent_no_ondemand_activation = 0x80000,  //!< can only be used in RayWorldIntersection
	ent_delayed_deformations = 0x80000   //!< queues procedural breakage requests; can only be used in SimulateExplosion
};
enum EStance {
	STANCE_NULL = -1,
	STANCE_STAND = 0,
	STANCE_CROUCH,
	STANCE_PRONE,
	STANCE_RELAXED,
	STANCE_STEALTH,
	STANCE_LOW_COVER,
	STANCE_ALERTED,
	STANCE_HIGH_COVER,
	STANCE_SWIM,
	STANCE_ZEROG,

	//! This value must be last.
	STANCE_LAST
};
enum EEntityXFormFlags {
	ENTITY_XFORM_POS = BIT(1),
	ENTITY_XFORM_ROT = BIT(2),
	ENTITY_XFORM_SCL = BIT(3),
	ENTITY_XFORM_NO_PROPOGATE = BIT(4),
	ENTITY_XFORM_FROM_PARENT = BIT(5),   //!< When parent changes his transformation.
	ENTITY_XFORM_PHYSICS_STEP = BIT(13),
	ENTITY_XFORM_EDITOR = BIT(14),
	ENTITY_XFORM_TRACKVIEW = BIT(15),
	ENTITY_XFORM_TIMEDEMO = BIT(16),
	ENTITY_XFORM_NOT_REREGISTER = BIT(17),  //!< Optimization flag, when set object will not be re-registered in 3D engine.
	ENTITY_XFORM_NO_EVENT = BIT(18),  //!< Suppresses ENTITY_EVENT_XFORM event.
	ENTITY_XFORM_NO_SEND_TO_ENTITY_SYSTEM = BIT(19),
	ENTITY_XFORM_USER = 0x1000000,
};
enum EEntityUpdatePolicy {
	ENTITY_UPDATE_NEVER,           //!< Never update entity every frame.
	ENTITY_UPDATE_IN_RANGE,        //!< Only update entity if it is in specified range from active camera.
	ENTITY_UPDATE_POT_VISIBLE,     //!< Only update entity if it is potentially visible.
	ENTITY_UPDATE_VISIBLE,         //!< Only update entity if it is visible.
	ENTITY_UPDATE_PHYSICS,         //!< Only update entity if it is need to be updated due to physics.
	ENTITY_UPDATE_PHYSICS_VISIBLE, //!< Only update entity if it is need to be updated due to physics or if it is visible.
	ENTITY_UPDATE_ALWAYS,          //!< Always update entity every frame.
};
//enum EEntityProxy {
//	ENTITY_PROXY_RENDER,
//	ENTITY_PROXY_PHYSICS,
//	ENTITY_PROXY_SCRIPT,
//	ENTITY_PROXY_AUDIO,
//	ENTITY_PROXY_AI,
//	ENTITY_PROXY_AREA,
//	ENTITY_PROXY_BOIDS,
//	ENTITY_PROXY_BOID_OBJECT,
//	ENTITY_PROXY_CAMERA,
//	ENTITY_PROXY_FLOWGRAPH,
//	ENTITY_PROXY_SUBSTITUTION,
//	ENTITY_PROXY_TRIGGER,
//	ENTITY_PROXY_ROPE,
//	ENTITY_PROXY_ENTITYNODE,
//	ENTITY_PROXY_ATTRIBUTES,
//	ENTITY_PROXY_CLIPVOLUME,
//	ENTITY_PROXY_DYNAMICRESPONSE,
//	ENTITY_PROXY_USER,
//	ENTITY_PROXY_LAST
//};
enum EEntityEvent {
	//! Sent when the entity local or world transformation matrix change (position/rotation/scale).
	//! nParam[0] = combination of the EEntityXFormFlags.
	ENTITY_EVENT_XFORM = 0,

	//! Called when the entity is moved/scaled/rotated in the editor. Only send on mouseButtonUp (hence finished).
	ENTITY_EVENT_XFORM_FINISHED_EDITOR,

	//! Sent when the entity timer expire.
	//! nParam[0] = TimerId, nParam[1] = milliseconds.
	ENTITY_EVENT_TIMER,

	//! Sent for unremovable entities when they are respawn.
	ENTITY_EVENT_INIT,

	//! Sent before entity is removed.
	ENTITY_EVENT_DONE,

	//! Sent when the entity becomes visible or invisible.
	//! nParam[0] is 1 if the entity becomes visible or 0 if the entity becomes invisible.
	ENTITY_EVENT_VISIBLITY,

	//! Sent to reset the state of the entity (used from Editor).
	//! nParam[0] is 1 if entering gamemode, 0 if exiting
	ENTITY_EVENT_RESET,

	//! Sent to parent entity after child entity have been attached.
	//! nParam[0] contains ID of child entity.
	ENTITY_EVENT_ATTACH,

	//! Sent to child entity after it has been attached to the parent.
	//! nParam[0] contains ID of parent entity.
	ENTITY_EVENT_ATTACH_THIS,

	//! Sent to parent entity after child entity have been detached.
	//! nParam[0] contains ID of child entity.
	ENTITY_EVENT_DETACH,

	//! Sent to child entity after it has been detached from the parent.
	//! nParam[0] contains ID of parent entity.
	ENTITY_EVENT_DETACH_THIS,

	//! Sent to parent entity after child entity have been linked.
	//! nParam[0] contains IEntityLink ptr.
	ENTITY_EVENT_LINK,

	//! Sent to parent entity before child entity have been delinked.
	//! nParam[0] contains IEntityLink ptr.
	ENTITY_EVENT_DELINK,

	//! Sent when the entity must be hidden.
	ENTITY_EVENT_HIDE,

	//! Sent when the entity must become not hidden.
	ENTITY_EVENT_UNHIDE,

	//! Sent when a physics processing for the entity must be enabled/disabled.
	//! nParam[0] == 1 physics must be enabled if 0 physics must be disabled.
	ENTITY_EVENT_ENABLE_PHYSICS,

	//! Sent when a physics in an entity changes state.
	//! nParam[0] == 1 physics entity awakes, 0 physics entity get to a sleep state.
	ENTITY_EVENT_PHYSICS_CHANGE_STATE,

	//! Sent when script is broadcasting its events.
	//! nParam[0] = Pointer to the ASCIIZ string with the name of the script event.
	//! nParam[1] = Type of the event value from IEntityClass::EventValueType.
	//! nParam[2] = Pointer to the event value depending on the type.
	ENTITY_EVENT_SCRIPT_EVENT,

	//! Sent when triggering entity enters to the area proximity, this event sent to all target entities of the area.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_ENTERAREA,

	//! Sent when triggering entity leaves the area proximity, this event sent to all target entities of the area.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_LEAVEAREA,

	//! Sent when triggering entity is near to the area proximity, this event sent to all target entities of the area.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_ENTERNEARAREA,

	//! Sent when triggering entity leaves the near area within proximity region of the outside area border.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_LEAVENEARAREA,

	//! Sent when triggering entity moves inside the area within proximity region of the outside area border.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_MOVEINSIDEAREA,

	// Sent when triggering entity moves inside an area of higher priority then the area this entity is linked to.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area with low prio, nParam[3] = EntityId of Area with high prio
	//ENTITY_EVENT_EXCLUSIVEMOVEINSIDEAREA,

	//! Sent when triggering entity moves inside the area within the near region of the outside area border.
	//! nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area, fParam[0] = FadeRatio (0-1)
	ENTITY_EVENT_MOVENEARAREA,

	//! Sent when triggering entity enters or leaves an area so all active areas of same group get notified. This event is sent to all target entities of the area.
	ENTITY_EVENT_CROSS_AREA,

	//! Sent when an entity with pef_monitor_poststep receives a poststep notification (the hamdler should be thread safe!)
	//! fParam[0] = time interval
	ENTITY_EVENT_PHYS_POSTSTEP,

	//! Sent when Breakable object is broken in physics.
	ENTITY_EVENT_PHYS_BREAK,

	//! Sent when AI object of the entity finished executing current order/action.
	ENTITY_EVENT_AI_DONE,

	//! Sent when a sound finished or was stopped playing.
	ENTITY_EVENT_SOUND_DONE,

	//! Sent when an entity has not been rendered for a while (the time specified via cvar "es_not_seen_timeout")
	//! \note This is sent only if ENTITY_FLAG_SEND_NOT_SEEN_TIMEOUT is set.
	ENTITY_EVENT_NOT_SEEN_TIMEOUT,

	//! Physical collision.
	ENTITY_EVENT_COLLISION,

	//! Called when entity is rendered (Only if ENTITY_FLAG_SEND_RENDER_EVENT is set).
	//! nParam[0] is a pointer to the current rendering SRenderParams structure.
	ENTITY_EVENT_RENDER,

	//! Called when the pre-physics update is done; fParam[0] is the frame time.
	ENTITY_EVENT_PREPHYSICSUPDATE,

	//! Called when the level loading is complete.
	ENTITY_EVENT_LEVEL_LOADED,

	//! Called when the level is started.
	ENTITY_EVENT_START_LEVEL,

	//! Called when the game is started (games may start multiple times).
	ENTITY_EVENT_START_GAME,

	//! Called when the entity enters a script state.
	ENTITY_EVENT_ENTER_SCRIPT_STATE,

	//! Called when the entity leaves a script state.
	ENTITY_EVENT_LEAVE_SCRIPT_STATE,

	//! Called before we serialized the game from file.
	ENTITY_EVENT_PRE_SERIALIZE,

	//! Called after we serialized the game from file.
	ENTITY_EVENT_POST_SERIALIZE,

	//! Called when the entity becomes invisible.
	ENTITY_EVENT_INVISIBLE,

	//! Called when the entity gets out of invisibility.
	ENTITY_EVENT_VISIBLE,

	//! Called when the entity material change.
	//! nParam[0] = pointer to the new IMaterial.
	ENTITY_EVENT_MATERIAL,

	//! Called when the entitys material layer mask changes.
	ENTITY_EVENT_MATERIAL_LAYER,

	//! Called when the entity gets hits by a weapon.
	ENTITY_EVENT_ONHIT,

	//! Called when an animation event (placed on animations in editor) is encountered.
	//! nParam[0] = AnimEventInstance* pEventParameters.
	ENTITY_EVENT_ANIM_EVENT,

	//! Called from ScriptBind_Entity when script requests to set collidermode.
	//! nParam[0] = ColliderMode
	ENTITY_EVENT_SCRIPT_REQUEST_COLLIDERMODE,

	//! Called to activate some output in a flow node connected to the entity
	//! nParam[0] = Output port index
	//! nParam[1] = TFlowInputData* to send to output
	ENTITY_EVENT_ACTIVATE_FLOW_NODE_OUTPUT,

	//! Called in the editor when some property of the current selected entity changes.
	ENTITY_EVENT_EDITOR_PROPERTY_CHANGED,

	//! Called when a script reloading is requested and done in the editor.
	ENTITY_EVENT_RELOAD_SCRIPT,

	//! Called when the entity is added to the list of entities that are updated.
	ENTITY_EVENT_ACTIVATED,

	//! Called when the entity is removed from the list of entities that are updated.
	ENTITY_EVENT_DEACTIVATED,

	//! Called when the entity should be added to the radar.
	ENTITY_EVENT_ADD_TO_RADAR,

	//! Called when the entity should be removed from the radar.
	ENTITY_EVENT_REMOVE_FROM_RADAR,

	//! Last entity event in list.
	ENTITY_EVENT_LAST,
};
enum EEntitySlotFlags {
	ENTITY_SLOT_RENDER = 0x0001,
	ENTITY_SLOT_RENDER_NEAREST = 0x0002,
	ENTITY_SLOT_RENDER_WITH_CUSTOM_CAMERA = 0x0004,
	ENTITY_SLOT_IGNORE_PHYSICS = 0x0008,
	ENTITY_SLOT_BREAK_AS_ENTITY = 0x0010,
	ENTITY_SLOT_RENDER_AFTER_POSTPROCESSING = 0x0020,
	ENTITY_SLOT_BREAK_AS_ENTITY_MP = 0x0040
};
//enum ESilhouettesParams {
//	eAllMap = 524312,
//	eLimitDistance = 524296
//};
enum ESilhouettesColors {
	eGreen = 16724940,
	eOrange = -13434676,
	ePurple = -16606208,
	eWhite = 0xFFFFFFFF,
	eGreenLime = 0x54FF0000,
	eRed = 0xFF000000,
	ePink = 0xFF00B600,
	ePurpleHex = 0xBB00FF00,
	eYellow = 0xFFFF0000,
	eColorZero = 0x00000000
};
enum EFlags {
	FLAG_CUSTOM_POST_EFFECT = BIT(0), // Has custom post effect ID stored in custom data
	FLAG_BBOX_VALID_LOCAL = BIT(1),
	FLAG_BBOX_FORCED = BIT(2),
	FLAG_BBOX_INVALID = BIT(3),
	FLAG_HIDDEN = BIT(4), // If render proxy is hidden.
	FLAG_UPDATE = BIT(5),  // If render proxy needs to be updated.
	FLAG_NOW_VISIBLE = BIT(6),  // If render proxy currently visible.
	FLAG_REGISTERED_IN_3DENGINE = BIT(7),  // If render proxy have been registered in 3d engine.
	FLAG_POST_INIT = BIT(8),  // If render proxy have received Post init event.
	FLAG_HAS_LIGHTS = BIT(9),  // If render proxy has some lights.
	FLAG_GEOMETRY_MODIFIED = BIT(10), // Geometry for this render slot was modified at runtime.
	FLAG_HAS_CHILDRENDERNODES = BIT(11), // If render proxy contains child render nodes
	FLAG_HAS_PARTICLES = BIT(12), // If render proxy contains particle emitters
	FLAG_SHADOW_DISSOLVE = BIT(13), // If render proxy requires dissolving shadows
	FLAG_FADE_CLOAK_BY_DISTANCE = BIT(14), // If render proxy requires fading cloak by distance
	FLAG_IGNORE_HUD_INTERFERENCE_FILTER = BIT(15), // HUD render proxy ignores hud interference filter post effect settings
	FLAG_IGNORE_HEAT_VALUE = BIT(16), // Don't appear hot in nano vision
	FLAG_POST_3D_RENDER = BIT(17), // Render proxy in post 3D pass
	FLAG_IGNORE_CLOAK_REFRACTION_COLOR = BIT(18), // Will ignore cloak refraction color
	FLAG_HUD_REQUIRE_DEPTHTEST = BIT(19), // If 3D HUD Object requires to be rendered at correct depth (i.e. behind weapon)
	FLAG_CLOAK_INTERFERENCE = BIT(20), // When set the cloak will use the cloak interference parameters
	FLAG_CLOAK_HIGHLIGHTS = BIT(21), // When set the cloak will use the cloak highlight parameters
	FLAG_HUD_DISABLEBLOOM = BIT(22), // Allows 3d hud object to disable bloom (Required to avoid overglow and cutoff with alien hud ghosted planes)
	FLAG_ANIMATE_OFFSCREEN_SHADOW = BIT(23), // Update the animation if object drawn in the shadow pass
	FLAG_DISABLE_MOTIONBLUR = BIT(24), // Disable motion blur
	FLAG_EXECUTE_AS_JOB_FLAG = BIT(25), // set if this CRenderProxy can be executed as a Job from the 3DEngine
	FLAG_RECOMPUTE_EXECUTE_AS_JOB_FLAG = BIT(26), // set if the slots changed, to recheck if this renderproxy can execute as a job
};
//enum KeyCodes :INT {
//
//	BACKSPACE = 8,
//	TAB = 9,
//	ENTER = 13,
//	SHIFT = 16,
//	CTRL = 17,
//	ALT = 18,
//	PAUSE_BREAK = 19,
//	CAPS_LOCK = 20,
//	ESCAPE = 27,
//	PAGE_UP = 33,
//	PAGE_DOWN = 34,
//	END = 35,
//	HOME = 36,
//	LEFT_ARROW = 37,
//	UP_ARROW = 38,
//	RIGHT_ARROW = 39,
//	DOWN_ARROW = 40,
//	INSERT = 45,
//	DELETE_ = 46,
//	_0 = 48,
//	_1 = 49,
//	_2 = 50,
//	_3 = 51,
//	_4 = 52,
//	_5 = 53,
//	_6 = 54,
//	_7 = 55,
//	_8 = 56,
//	_9 = 57,
//	A = 65,
//	B = 66,
//	C = 67,
//	D = 68,
//	E = 69,
//	F = 70,
//	G = 71,
//	H = 72,
//	I = 73,
//	J = 74,
//	K = 75,
//	L = 76,
//	M = 77,
//	N = 78,
//	O = 79,
//	P = 80,
//	Q = 81,
//	R = 82,
//	S = 83,
//	T = 84,
//	U = 85,
//	V = 86,
//	W = 87,
//	X = 88,
//	Y = 89,
//	Z = 90,
//	LEFT_WINDOW = 91,
//	RIGHT_WINDOW = 92,
//	SELECT_KEY = 93,
//	NUMPAD_0 = 96,
//	NUMPAD_1 = 97,
//	NUMPAD_2 = 98,
//	NUMPAD_3 = 99,
//	NUMPAD_4 = 100,
//	NUMPAD_5 = 101,
//	NUMPAD_6 = 102,
//	NUMPAD_7 = 103,
//	NUMPAD_8 = 104,
//	NUMPAD_9 = 105,
//	MULTIPLY = 106,
//	ADD = 107,
//	SUBTRACT = 109,
//	DECIMAL_POINT = 110,
//	DIVIDE = 111,
//	F1 = 112,
//	F2 = 113,
//	F3 = 114,
//	F4 = 115,
//	F5 = 116,
//	F6 = 117,
//	F7 = 118,
//	F8 = 119,
//	F9 = 120,
//	F10 = 121,
//	F11 = 122,
//	F12 = 123,
//	NUM_LOCK = 144,
//	SCROLL_LOCK = 145,
//	SEMI_COLON = 186,
//	EQUAL_SIGN = 187,
//	COMMA = 188,
//	DASH = 189,
//	PERIOD = 190,
//	FORWARD_SLASH = 191,
//	GRAVE_ACCENT = 192,
//	OPEN_BRACKET = 219,
//	BACK_SLASH = 220,
//	CLOSE_BRAKET = 221,
//	SINGLE_QUOTE = 222
//};
enum pe_type {
	PE_NONE = 0,
	PE_STATIC = 1,
	PE_RIGID = 2,
	PE_WHEELEDVEHICLE = 3,
	PE_LIVING = 4,
	PE_PARTICLE = 5,
	PE_ARTICULATED = 6,
	PE_ROPE = 7,
	PE_SOFT = 8,
	PE_AREA = 9
};
enum EBone : unsigned int
{
	Bone_Pelvis = 1,
	Bone_Locomotion,
	Bone_Spine,
	Bone_L_Thigh,
	Bone_R_Thigh,
	Bone_Groin_Back,
	Bone_Groin_Front,
	Bone_Spine1,
	Bone_Spine2,
	Bone_Spine3,
	Bone_Wep_Law,
	Bone_Neck,
	Bone_Head,
	Bone_L_Clavicle,
	Bone_R_Clavicle,
	Bone_L_Eye,
	Bone_R_Eye,
	Bone_Camera,
	Bone_L_Eye_01,
	Bone_R_Eye_01,
	Bone_HNeck,
	Bone_Camera_01,
	Bone_HNeck_End,
	Bone_L_UpperArm,
	Bone_L_Forearm,
	Bone_L_Hand,
	Bone_L_ForeTwist,
	Bone_L_Finger0,
	Bone_L_Finger1,
	Bone_L_Finger2,
	Bone_L_Finger3,
	Bone_L_Finger4,
	Bone_Wep_Alt,
	Bone_L_Hand_Push,
	Bone_L_Finger01,
	Bone_L_Finger02,
	Bone_L_Finger11,
	Bone_L_Finger12,
	Bone_L_Finger21,
	Bone_L_Finger22,
	Bone_L_Finger31,
	Bone_L_Finger32,
	Bone_L_Finger41,
	Bone_L_Finger42,
	Bone_L_ForeTwist_1,
	Bone_L_ForeTwist_2,
	Bone_R_UpperArm,
	Bone_R_Forearm,
	Bone_R_Hand,
	Bone_R_ForeTwist,
	Bone_R_Finger0,
	Bone_R_Finger1,
	Bone_R_Finger2,
	Bone_R_Finger3,
	Bone_R_Finger4,
	Bone_WepBone,
	Bone_R_Hand_Push,
	Bone_R_Finger01,
	Bone_R_Finger02,
	Bone_R_Finger11,
	Bone_R_Finger12,
	Bone_R_Finger21,
	Bone_R_Finger22,
	Bone_R_Finger31,
	Bone_R_Finger32,
	Bone_R_Finger41,
	Bone_R_Finger42,
	Bone_R_ForeTwist_1,
	Bone_R_ForeTwist_2,
	Bone_L_Calf,
	Bone_L_Foot,
	Bone_L_Toe0,
	Bone_L_Heel,
	Bone_L_Heel01,
	Bone_L_Toe0Nub,
	Bone_L_Toe0Nub01,
	Bone_R_Calf,
	Bone_R_Foot,
	Bone_R_Toe0,
	Bone_R_Heel,
	Bone_R_Heel01,
	Bone_R_Toe0Nub,
	Bone_R_Toe0Nub01,
	Bone_Groin_Back_End,
	Bone_Groin_Front_End,
	Bone_Locator_Collider
};
enum IWeaponSystemId : DWORD {
	eWS_NoiseLevel = 4,//Уровень шума
	eWS_MaxAmmoClip = 5,//кол-во патронов в обойме
	eWS_Zatvor = 17,//скорость затвора
	eWS_MaxDamage = 27,//максимальный урон
	eWS_Distance1 = 28,//Дальность оружия
	eWS_MinDamage = 30,//минимальный урон
	eWS_FastChange = 33,//Скорость смены оружия
	eWS_SpeedPricel = 43,//скорость входа в прицел
	eWS_MeleeWeaponDamage = 55,//Урон в ближнем бою
	eWS_KnifeLKM = 56,//скорость удара ножом лкм
	eWS_Distance2 = 58,//дальность удара ножом лкм
	eWS_Distance3 = 63,//дальность удара ножом пкм
	eWS_MinRecoil = 66,//Минимальная Отдача
	eWS_MaxRecoil = 67,//Максимальная Отдача
	eWS_SpeedReload = 72,//Скорость перезарядки
	eWS_FireRate = 80,//Темп стрельбы
	eWS_KnifePKM = 83,//скорость удара ножов пкм
	eWS_MaxTo4nost = 95,//Минимальная точность
	eWS_MinTo4nost = 96,//Максимальная точность
	eWS_Zoom = 108,//дальность зума
};
enum GameModes
{
	Lobby = 0,  //лобби
	eDeathmatch, //команды
	eOccupation, //захват
	PVE,        //ПВЕ
	eUndermining,//подрыв
	eStorm,      //Штурм
	eChopper,    //Мясорубка
	eDestruction,//уничтожение
	eDomination, //доминация
	eSurvival    //выживание
};
enum ERWIFlagsShot
{
	eAmmo_Flags = 0x31F,
	eMode_16_Ammo_WallShot = 0x10,
	eMode_20_Ammo_WallShot = 0x14,
	eAmmoWallShotF = 0x75,
	eAmmoWallShotE = 0x7E
};
enum ClassId
{
	Sturm = 0,
	Medic = 3,
	Engineer = 4,
	Sniper = 2
};
enum ETimer
{
	ETIMER_GAME = 0, // Pausable, serialized, frametime is smoothed/scaled/clamped.
	ETIMER_UI,       // Non-pausable, non-serialized, frametime unprocessed.
	ETIMER_LAST
};
enum AimObjectsMode {
	Center,
	Max
};
enum IGameFunction
{
	ClimbExtraHeight,
	SlideMultDistance,
	AntiAfk,
};

enum type_zero { ZERO };
enum type_min { VMIN };
enum type_max { VMAX };
enum type_identity { IDENTITY };
#pragma endregion
#pragma region Const
const f32 gf_PI = f32(3.14159265358979323846264338327950288419716939937510);
//const f64 g_PI = 3.14159265358979323846264338327950288419716939937510;       //!< pi

const f32 gf_PI2 = f32(3.14159265358979323846264338327950288419716939937510 * 2.0);
const f64 g_PI2 = 3.14159265358979323846264338327950288419716939937510 * 2.0; //!< 2*pi

const f64 sqrt2 = 1.4142135623730950488016887242097;
const f64 sqrt3 = 1.7320508075688772935274463415059;
#pragma endregion

union USpectatorModeData
{
	struct SFixed
	{
		EntityId  location;
	}
	fixed;
	struct SFree
	{
		uint8  _dummy_;
	}
	free;
	struct SFollow
	{
		EntityId  spectatorTarget;			// which player we are following
		float     invalidTargetTimer;		// how long have we been looking at an invalid spectator target
	}
	follow;
	struct SKiller
	{
		EntityId  spectatorTarget;		// which killer we are following
		float     startTime;					// Time that we enetered Killer view.
	}
	killer;

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};
struct SSpectatorInfo
{
	USpectatorModeData	dataU;
	float								yawSpeed;
	float								pitchSpeed;
	bool								rotateSpeedSingleFrame;
	uint8								mode;
	uint8								state;

	void Reset()
	{
		uint8  oldstate = state;
		memset(this, 0, sizeof(*this));
		state = oldstate;
		yawSpeed = 0.f;
		pitchSpeed = 0.f;
		rotateSpeedSingleFrame = false;
	}

	EntityId* GetOtherEntIdPtrForCurMode()
	{
		EntityId* ptr = NULL;
		switch (mode)
		{
		case EActorSpectatorMode::eASM_Fixed:	ptr = &dataU.fixed.location; break;
		case EActorSpectatorMode::eASM_Follow:	ptr = &dataU.follow.spectatorTarget; break;
		case EActorSpectatorMode::eASM_Killer:	ptr = &dataU.killer.spectatorTarget; break;
		}
		return ptr;
	}

	EntityId GetOtherEntIdForCurMode()
	{
		EntityId  e = 0;
		if (EntityId* ptr = GetOtherEntIdPtrForCurMode())
			e = (*ptr);
		return e;
	}

	void SetOtherEntIdForCurMode(EntityId e)
	{
		if (EntityId* ptr = GetOtherEntIdPtrForCurMode())
		{
			(*ptr) = e;
		}
	}
};
struct SAimAccelerationParams
{
	SAimAccelerationParams();

	float angle_min;
	float angle_max;
};
struct SPlayerRotationParams
{
	enum EAimType
	{
		EAimType_NORMAL,
		EAimType_CROUCH,
		EAimType_SLIDING,
		EAimType_SPRINTING,
		EAimType_SWIM,
		EAimType_MOUNTED_GUN,
		EAimType_TOTAL
	};

	SAimAccelerationParams m_horizontalAims[2][EAimType_TOTAL];
	SAimAccelerationParams m_verticalAims[2][EAimType_TOTAL];

	ILINE const SAimAccelerationParams& GetHorizontalAimParams(SPlayerRotationParams::EAimType aimType, bool firstPerson) const
	{
		CRY_ASSERT((aimType >= 0) && (aimType < EAimType_TOTAL));

		return m_horizontalAims[firstPerson ? 0 : 1][aimType];
	}

	ILINE const SAimAccelerationParams& GetVerticalAimParams(SPlayerRotationParams::EAimType aimType, bool firstPerson) const
	{
		CRY_ASSERT((aimType >= 0) && (aimType < EAimType_TOTAL));

		return m_verticalAims[firstPerson ? 0 : 1][aimType];
	}
};

//static FLOAT clamp(FLOAT X, FLOAT Min, FLOAT Max)
//{
//	X = (X + Max - fabsf(X - Max)) * 0.5f;
//	X = (X + Min + fabsf(X - Min)) * 0.5f;
//
//	return X;
//}
static DOUBLE clamp(DOUBLE X, DOUBLE Min, DOUBLE Max)
{
	X = (X + Max - abs(X - Max)) * 0.5f;
	X = (X + Min + abs(X - Min)) * 0.5f;

	return X;
}
//static INT FloatU32(const FLOAT x)
//{
//	union { INT ui; FLOAT f; } cvt;
//	cvt.f = x;
//
//	return cvt.ui;
//}
//static BOOLEAN NumberValid(const FLOAT& x)
//{
//	INT i = FloatU32(x);
//	INT expmask = FloatU32ExpMask;
//	INT iexp = i & expmask;
//	BOOLEAN invalid = (iexp == expmask);
//
//	if (invalid)
//	{
//		INT i = 0x7F800001;
//		FLOAT fpe = *(FLOAT*)(&i);
//	}
//
//	return !invalid;
//}

ILINE f32 _sqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                     // floating point bit level hacking [sic]
	i = 0x5f3759df - (i >> 1);             // Newton's approximation
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y)); // 1st iteration
	y = y * (threehalfs - (x2 * y * y)); // 2nd iteration
	y = y * (threehalfs - (x2 * y * y)); // 3rd iteration

	return 1 / y;
}
ILINE f64 isqrt_tpl(f64 op) { return 1.0 / sqrt(op); }
//ILINE f32   fabs_tpl(f32 op) { return fabsf(op); }
ILINE f64   fabs_tpl(f64 op) { return fabs(op); }
ILINE int32 fabs_tpl(int32 op) { int32 mask = op >> 31; return op + mask ^ mask; }
ILINE void  sincos_tpl(f32 angle, f32* pSin, f32* pCos) { *pSin = f32(sin(angle));  *pCos = f32(cos(angle)); }
ILINE void  sincos_tpl(f64 angle, f64* pSin, f64* pCos) { *pSin = f64(sin(angle));  *pCos = f64(cos(angle)); }
ILINE f64 isqrt_safe_tpl(f64 op) { return 1.0 / sqrt(op + DBL_MIN); }
//ILINE f32   asin_tpl(f32 op) { return asinf(clamp(op, -1.0f, +1.0f)); }
ILINE f64   asin_tpl(f64 op) { return asin(clamp(op, -1.0, +1.0)); }
ILINE f32   atan_tpl(f32 op) { return atanf(op); }
ILINE f64   atan_tpl(f64 op) { return atan(op); }
//ILINE f32   atan2_tpl(f32 op1, f32 op2) { return atan2f(op1, op2); }
ILINE f64   atan2_tpl(f64 op1, f64 op2) { return atan2(op1, op2); }

template <typename F> struct Vec3_tpl {
	F x, y, z;
	ILINE Vec3_tpl(type_zero) : x(0), y(0), z(0) {}
	ILINE Vec3_tpl(type_min);
	ILINE Vec3_tpl(type_max);
	explicit ILINE Vec3_tpl(F f) : x(f), y(f), z(f) { }
	ILINE Vec3_tpl() {
		Vec3_tpl(0, 0, 0);
	}
	ILINE Vec3_tpl(F xt, F yt, F zt) {
		x = xt;
		y = yt;
		z = zt;
	}
	ILINE F GetLength() const { return (F)_sqrt(x * x + y * y + z * z); }
	template<typename T> ILINE F sum(T a) {
		F s = 0;
		for (int i = 0; i < a.size; ++i) s += a[i];
		return s;
	}
	template<typename T> ILINE F dot(T a, T b) { return sum(a * b); }

	ILINE F  distance(Vec3_tpl VecB) { return (F)_sqrt(((x - VecB.x) * (x - VecB.x)) + ((y - VecB.y) * (y - VecB.y)) + ((z - VecB.z) * (z - VecB.z))); }

	ILINE F& operator[](int index) { return ((F*)this)[index]; }
	ILINE F  operator[](int index) const { return ((F*)this)[index]; }

	void operator += (F f) { x += f; y += f; z += f; }
	void operator -= (F f) { x -= f; y -= f; z -= f; }
	ILINE Vec3_tpl<F> operator*(F k) const {
		const Vec3_tpl<F> v = *this;
		return Vec3_tpl<F>(v.x * k, v.y * k, v.z * k);
	}
	ILINE Vec3_tpl operator /  (F f) { return Vec3_tpl(x / f, y / f, z / f); }
	ILINE Vec3_tpl& operator = (const Vec3_tpl& source) { x = source.x; y = source.y; z = source.z; return *this; }
	ILINE Vec3_tpl& operator = (const F value) { x = value; y = value; z = value;   return *this; }
	ILINE Vec3_tpl& operator *= (F f) { x *= f; y *= f; z *= f; return *this; }
	ILINE Vec3_tpl& operator /= (F f) { x /= f; y /= f; z /= f; return *this; }

	ILINE friend Vec3_tpl operator + (const Vec3_tpl& vec1, const Vec3_tpl& vec2) { return Vec3_tpl(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z); }
	ILINE friend Vec3_tpl operator + (const Vec3_tpl& vec1, const F* vec2) { return vec1 + Vec3_tpl(vec2); }
	ILINE friend Vec3_tpl operator + (const F* vec1, const Vec3_tpl& vec2) { return Vec3_tpl(vec1) + vec2; }

	ILINE friend Vec3_tpl operator - (const Vec3_tpl& vec1, const Vec3_tpl& vec2) { return Vec3_tpl(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z); }
	ILINE friend Vec3_tpl operator - (const Vec3_tpl& vec1, const F* vec2) { return vec1 - Vec3_tpl(vec2); }
	ILINE friend Vec3_tpl operator - (const F* vec1, const Vec3_tpl& vec2) { return Vec3_tpl(vec1) - vec2; }

	ILINE Vec3_tpl<F>& Set(const F xval, const F yval, const F zval) {
		x = xval; y = yval; z = zval;
		return *this;
	}

	ILINE Vec3_tpl& normalize() {
		F len2 = x * x + y * y + z * z;
		if (len2 > (F)1e-20f) {
			F rlen = _sqrt(len2);
			x *= rlen;
			y *= rlen;
			z *= rlen;
		}
		else Set(0, 0, 1);

		return *this;
	}
	ILINE Vec3_tpl<F> GetNormalized() const {
		FLOAT fInvLen = (FLOAT)isqrt_safe_tpl(x * x + y * y + z * z);
		Vec3_tpl<F> out = *this;
		out.x *= fInvLen;
		out.y *= fInvLen;
		out.z *= fInvLen;
		return out;
	}
};
template <typename F> struct Vec4_tpl {
	F x, y, z, w;
	ILINE Vec4_tpl(type_zero) : x(0), y(0), z(0), w(0) {}
	ILINE Vec4_tpl(type_min);
	ILINE Vec4_tpl(type_max);
	explicit ILINE Vec4_tpl(F f) : x(f), y(f), z(f), w(f) { }
	ILINE Vec4_tpl() {
		Vec4_tpl(0, 0, 0, 0);
	}
	ILINE Vec4_tpl(F xt, F yt, F zt, F wt) {
		x = xt;
		y = yt;
		z = zt;
		w = wt;
	}
};
template <typename F> struct Quat_tpl {
	Vec3_tpl<F> v;
	F w;
	ILINE  Quat_tpl() {}
	ILINE  Quat_tpl(F angle, const Vec3_tpl<F>& axis) : w(angle), v(axis) {};
	ILINE  Quat_tpl(type_identity) : w(1), v(0, 0, 0) {}

	ILINE  bool operator!=(const Quat_tpl<F>& q) const { return !(*this == q); }
	ILINE  Quat_tpl<F>	operator - () const { return Quat_tpl<F>(-w, -v); };

	ILINE  bool IsEquivalent(const Quat_tpl<F>& q, F e = VEC_EPSILON) const {
		Quat_tpl<F> p = -q;
		bool t0 = (fabs_tpl(v.x - q.v.x) <= e) && (fabs_tpl(v.y - q.v.y) <= e) && (fabs_tpl(v.z - q.v.z) <= e) && (fabs_tpl(w - q.w) <= e);
		bool t1 = (fabs_tpl(v.x - p.v.x) <= e) && (fabs_tpl(v.y - p.v.y) <= e) && (fabs_tpl(v.z - p.v.z) <= e) && (fabs_tpl(w - p.w) <= e);
		t0 |= t1;
		return t0;
	}
	ILINE   void  SetRotationVDir(const Vec3_tpl<F>& vdir) {
		w = (0.70710676908493042f);
		v.x = (vdir.z * 0.70710676908493042f);
		v.y = (0.0f);
		v.z = (0.0f);

		F l = (F)_sqrt(vdir.x * vdir.x + vdir.y * vdir.y);

		if (l > (0.00001)) {
			Vec3_tpl<F> hv;
			hv.x = vdir.x / l;
			hv.y = vdir.y / l + 1.0f;
			hv.z = l + 1.0f;

			F r = (F)_sqrt(hv.x * hv.x + hv.y * hv.y);
			F s = (F)_sqrt(hv.z * hv.z + vdir.z * vdir.z);
			F hacos0 = 0.0;
			F hasin0 = -1.0;

			if (r > (0.00001)) {
				hacos0 = hv.y / r;
				hasin0 = -hv.x / r;
			}
			F hacos1 = hv.z / s;
			F hasin1 = vdir.z / s;
			w = (hacos0 * hacos1);
			v.x = (hacos0 * hasin1);
			v.y = (hasin0 * hasin1);
			v.z = (hasin0 * hacos1);
		}
	}
	static Quat_tpl<F> CreateRotationVDir(const Vec3_tpl<F>& vdir) {
		Quat_tpl<F> q;
		q.SetRotationVDir(vdir);
		return q;
	}
};
template <typename F> struct QuatT_tpl {
	Quat_tpl<F>   q;
	Vec3_tpl<F> t;
	ILINE  QuatT_tpl() {};
	ILINE  QuatT_tpl(const Vec3_tpl<F>& _t, const Quat_tpl<F>& _q) { q = _q; t = _t; }
	ILINE  QuatT_tpl(type_identity) { q.w = 1; q.v.x = 0; q.v.y = 0; q.v.z = 0; t.x = 0; t.y = 0; t.z = 0; }

};
template <typename F> struct Matrix33_tpl {
	F m00, m01, m02;
	F m10, m11, m12;
	F m20, m21, m22;
	ILINE  Matrix33_tpl() {};
	explicit ILINE Matrix33_tpl<F>(const Quat_tpl<F>& q) {
		Vec3_tpl<F> v2 = q.v + q.v;
		F xx = 1 - v2.x * q.v.x;
		F yy = v2.y * q.v.y;
		F xw = v2.x * q.w;
		F xy = v2.y * q.v.x;
		F yz = v2.z * q.v.y;
		F yw = v2.y * q.w;
		F xz = v2.z * q.v.x;
		F zz = v2.z * q.v.z;
		F zw = v2.z * q.w;
		m00 = 1 - yy - zz;
		m01 = xy - zw;
		m02 = xz + yw;
		m10 = xy + zw;
		m11 = xx - zz;
		m12 = yz - xw;
		m20 = xz - yw;
		m21 = yz + xw;
		m22 = xx - yy;
	}
	template<typename F1> ILINE  Matrix33_tpl(const Matrix33_tpl<F1>& m) {
		m00 = F(m.m00);	m01 = F(m.m01);	m02 = F(m.m02);
		m10 = F(m.m10);	m11 = F(m.m11);	m12 = F(m.m12);
		m20 = F(m.m20);	m21 = F(m.m21);	m22 = F(m.m22);
	}
	ILINE  Matrix33_tpl& operator = (const Matrix33_tpl<F>& m) {
		m00 = m.m00;	m01 = m.m01;	m02 = m.m02;
		m10 = m.m10;	m11 = m.m11;	m12 = m.m12;
		m20 = m.m20;	m21 = m.m21;	m22 = m.m22;
		return *this;
	}
};
template <typename F> struct Matrix34_tpl {
	F m00, m01, m02, m03;
	F m10, m11, m12, m13;
	F m20, m21, m22, m23;

	ILINE Matrix34_tpl() {};
	template<class F1> explicit Matrix34_tpl(const QuatT_tpl<F1>& q) {
		Vec3_tpl<F1> v2 = q.q.v + q.q.v;
		F1 xx = 1 - v2.x * q.q.v.x;		F1 yy = v2.y * q.q.v.y;		F1 xw = v2.x * q.q.w;
		F1 xy = v2.y * q.q.v.x;			F1 yz = v2.z * q.q.v.y;		F1 yw = v2.y * q.q.w;
		F1 xz = v2.z * q.q.v.x;			F1 zz = v2.z * q.q.v.z;		F1 zw = v2.z * q.q.w;
		m00 = F(1 - yy - zz);			m01 = F(xy - zw);			m02 = F(xz + yw);			m03 = F(q.t.x);
		m10 = F(xy + zw);				m11 = F(xx - zz);			m12 = F(yz - xw);			m13 = F(q.t.y);
		m20 = F(xz - yw);				m21 = F(yz + xw);			m22 = F(xx - yy);			m23 = F(q.t.z);
	}
	ILINE Vec3_tpl <F>GetTranslation() const { return Vec3_tpl <F>(m03, m13, m23); }

	ILINE Matrix34_tpl <F>SetTranslation(Vec3_tpl<F>vPos) {
		m03 = F(vPos.x);
		m13 = F(vPos.y);
		m23 = F(vPos.z);
		return *this;
	}
};
template<typename F> struct Matrix44_tpl {
	F m00, m01, m02, m03;
	F m10, m11, m12, m13;
	F m20, m21, m22, m23;
	F m30, m31, m32, m33;

	ILINE Matrix44_tpl() {};
	ILINE void SetZero() {
		m00 = 0;
		m01 = 0;
		m02 = 0;
		m03 = 0;
		m10 = 0;
		m11 = 0;
		m12 = 0;
		m13 = 0;
		m20 = 0;
		m21 = 0;
		m22 = 0;
		m23 = 0;
		m30 = 0;
		m31 = 0;
		m32 = 0;
		m33 = 0;
	}
	ILINE void SetIdentity()
	{
		m00 = 1;
		m01 = 0;
		m02 = 0;
		m03 = 0;
		m10 = 0;
		m11 = 1;
		m12 = 0;
		m13 = 0;
		m20 = 0;
		m21 = 0;
		m22 = 1;
		m23 = 0;
		m30 = 0;
		m31 = 0;
		m32 = 0;
		m33 = 1;
	}
	ILINE Matrix44_tpl(type_zero) { SetZero(); }
	ILINE F& operator[](int index) { return ((F*)this)[index]; }
	ILINE F  operator[](int index) const { return ((F*)this)[index]; }
};
template<typename F>ILINE F CallFunction(_pvoid BaseClass, INT vIndex)
{
	_pdword* vPointer = (_pdword*)BaseClass;
	_pdword vFunction = *vPointer;
	_dword dwAddress = vFunction[vIndex / _Division];
	return (F)(dwAddress);
}
template<typename F>ILINE F Function(void* base, _dword index)
{
	return (*(F**)base)[index];
}


template<class F1, class F2>ILINE Vec3_tpl<F1> operator*(const Matrix33_tpl<F2>& m, const Vec3_tpl<F1>& p) {
	Vec3_tpl<F1> tp;
	tp.x = F1(m.m00 * p.x + m.m01 * p.y + m.m02 * p.z);
	tp.y = F1(m.m10 * p.x + m.m11 * p.y + m.m12 * p.z);
	tp.z = F1(m.m20 * p.x + m.m21 * p.y + m.m22 * p.z);
	return tp;
}
template<class F, class F2> Vec3_tpl<F> operator*(const QuatT_tpl<F>& q, const Vec3_tpl<F2>& v) {
	Vec3_tpl<F> out, r2;
	r2.x = (q.q.v.y * v.z - q.q.v.z * v.y) + q.q.w * v.x;
	r2.y = (q.q.v.z * v.x - q.q.v.x * v.z) + q.q.w * v.y;
	r2.z = (q.q.v.x * v.y - q.q.v.y * v.x) + q.q.w * v.z;
	out.x = (r2.z * q.q.v.y - r2.y * q.q.v.z);
	out.x += out.x + v.x + q.t.x;
	out.y = (r2.x * q.q.v.z - r2.z * q.q.v.x);
	out.y += out.y + v.y + q.t.y;
	out.z = (r2.y * q.q.v.x - r2.x * q.q.v.y);
	out.z += out.z + v.z + q.t.z;
	return out;
}
template<class F> Vec3_tpl<F> operator*(const Matrix34_tpl<F>& m, const Vec3_tpl<F>& p) {
	Vec3_tpl<F> tp;
	tp.x = m.m00 * p.x + m.m01 * p.y + m.m02 * p.z + m.m03;
	tp.y = m.m10 * p.x + m.m11 * p.y + m.m12 * p.z + m.m13;
	tp.z = m.m20 * p.x + m.m21 * p.y + m.m22 * p.z + m.m23;
	return tp;
}
template <class F> Matrix34_tpl<F> operator * (const Matrix34_tpl<F>& l, const Matrix34_tpl<F>& r) {
	Matrix34_tpl<F> m;
	m.m00 = l.m00 * r.m00 + l.m01 * r.m10 + l.m02 * r.m20;
	m.m10 = l.m10 * r.m00 + l.m11 * r.m10 + l.m12 * r.m20;
	m.m20 = l.m20 * r.m00 + l.m21 * r.m10 + l.m22 * r.m20;
	m.m01 = l.m00 * r.m01 + l.m01 * r.m11 + l.m02 * r.m21;
	m.m11 = l.m10 * r.m01 + l.m11 * r.m11 + l.m12 * r.m21;
	m.m21 = l.m20 * r.m01 + l.m21 * r.m11 + l.m22 * r.m21;
	m.m02 = l.m00 * r.m02 + l.m01 * r.m12 + l.m02 * r.m22;
	m.m12 = l.m10 * r.m02 + l.m11 * r.m12 + l.m12 * r.m22;
	m.m22 = l.m20 * r.m02 + l.m21 * r.m12 + l.m22 * r.m22;
	m.m03 = l.m00 * r.m03 + l.m01 * r.m13 + l.m02 * r.m23 + l.m03;
	m.m13 = l.m10 * r.m03 + l.m11 * r.m13 + l.m12 * r.m23 + l.m13;
	m.m23 = l.m20 * r.m03 + l.m21 * r.m13 + l.m22 * r.m23 + l.m23;
	return m;
}

//typedef Vec3_tpl<float>			Vec3;
typedef Vec4_tpl<float>			Vec4;
//typedef Quat_tpl<float>			Quat;
//typedef QuatT_tpl<float>		QuatT;
typedef Quat_tpl<float>			quaternionf;
//typedef Matrix33_tpl<float>		Matrix33;
//typedef Matrix34_tpl<float>		Matrix34;
typedef Matrix44_tpl<FLOAT>		Matrix44;
//struct AABB {
//	Vec3 min;
//	Vec3 max;
//	ILINE Vec3 GetCenter() const { return (min + max) * 0.5f; }
//	ILINE bool IsReset() const { return min.x > max.x; }
//	ILINE void Reset() { min = Vec3(1e15f);  max = Vec3(-1e15f); }
//};

#ifndef _STL_UTILS_HEADER_
#define _STL_UTILS_HEADER_

#define USE_HASH_MAP
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#if (_MSC_VER >= 1400) && !defined(_STLP_BEGIN_NAMESPACE) // Visual Studio 2005 without STLPort
#include <hash_map>
#undef std__hash_map
#define std__hash_map stdext::hash_map
#elif defined(LINUX)
#include "platform.h"
#include <ext/hash_map>
#define std__hash_map __gnu_cxx::hash_map
#else
#include <hash_map>
#endif

#ifndef std__hash_map
#define std__hash_map std::hash_map
#endif

/*
{
	typename Map::const_iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}

// searches the given entry in the map by key, and if there is none, returns the default value
// The values are taken/returned in REFERENCEs rather than values
template <typename Map>
inline typename Map::mapped_type& find_in_map_ref(Map& mapKeyToValue, typename Map::key_type key, typename Map::mapped_type& valueDefault)
{
	typename Map::iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}
*/

// auto-cleaner: upon destruction, calls the clear() method
template <class T>
class CAutoClear
{
public:
	CAutoClear(T* p) : m_p(p) {}
	~CAutoClear() { m_p->clear(); }
protected:
	T* m_p;
};


template <class Container>
unsigned sizeofArray(const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type) * arr.size());
}

template <class Container>
unsigned sizeofVector(const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type) * arr.capacity());
}

template <class Container>
unsigned sizeofArray(const Container& arr, unsigned nSize)
{
	return arr.empty() ? 0u : (unsigned)(sizeof(typename Container::value_type) * nSize);
}

template <class Container>
unsigned capacityofArray(const Container& arr)
{
	return (unsigned)(arr.capacity() * sizeof(arr[0]));
}

template <class T>
unsigned countElements(const std::vector<T>& arrT, const T& x)
{
	unsigned nSum = 0;
	for (typename std::vector<T>::const_iterator iter = arrT.begin(); iter != arrT.end(); ++iter)
		if (x == *iter)
			++nSum;
	return nSum;
}

// [Timur]
/** Contain extensions for STL library.
*/
namespace stl
{
	//////////////////////////////////////////////////////////////////////////
	//! Searches the given entry in the map by key, and if there is none, returns the default value
	//////////////////////////////////////////////////////////////////////////
	template <typename Map>
	inline typename Map::mapped_type find_in_map(const Map& mapKeyToValue, const typename Map::key_type& key, typename Map::mapped_type valueDefault)
	{
		typename Map::const_iterator it = mapKeyToValue.find(key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Inserts and returns a reference to the given value in the map, or returns the current one if it's already there.
	//////////////////////////////////////////////////////////////////////////
	template <typename Map>
	inline typename Map::mapped_type& map_insert_or_get(Map& mapKeyToValue, const typename Map::key_type& key, const typename Map::mapped_type& defValue = typename Map::mapped_type())
	{
		std::pair<typename Map::iterator, bool> iresult = mapKeyToValue.insert(typename Map::value_type(key, defValue));
		return iresult.first->second;
	}

	// searches the given entry in the map by key, and if there is none, returns the default value
	// The values are taken/returned in REFERENCEs rather than values
	template <typename Key, typename mapped_type, typename Traits, typename Allocator>
	inline mapped_type& find_in_map_ref(std::map<Key, mapped_type, Traits, Allocator>& mapKeyToValue, const Key& key, mapped_type& valueDefault)
	{
		typedef std::map<Key, mapped_type, Traits, Allocator> Map;
		typename Map::iterator it = mapKeyToValue.find(key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	template <typename Key, typename mapped_type, typename Traits, typename Allocator>
	inline const mapped_type& find_in_map_ref(const std::map<Key, mapped_type, Traits, Allocator>& mapKeyToValue, const Key& key, const mapped_type& valueDefault)
	{
		typedef std::map<Key, mapped_type, Traits, Allocator> Map;
		typename Map::const_iterator it = mapKeyToValue.find(key);
		if (it == mapKeyToValue.end())
			return valueDefault;
		else
			return it->second;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of map.
	//////////////////////////////////////////////////////////////////////////
	template <class Map, class Vector>
	inline void map_to_vector(const Map& theMap, Vector& array)
	{
		array.resize(0);
		array.reserve(theMap.size());
		for (typename Map::const_iterator it = theMap.begin(); it != theMap.end(); ++it)
		{
			array.push_back(it->second);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Fills vector with contents of set.
	//////////////////////////////////////////////////////////////////////////
	template <class Set, class Vector>
	inline void set_to_vector(const Set& theSet, Vector& array)
	{
		array.resize(0);
		array.reserve(theSet.size());
		for (typename Set::const_iterator it = theSet.begin(); it != theSet.end(); ++it)
		{
			array.push_back(*it);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find and erase element from container.
	// @return true if item was find and erased, false if item not found.
	//////////////////////////////////////////////////////////////////////////
	template <class Container, class Value>
	inline bool find_and_erase(Container& container, const Value& value)
	{
		typename Container::iterator it = std::find(container.begin(), container.end(), value);
		if (it != container.end())
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find and erase all elements matching value from container.
	// Assume that this will invalidate any exiting iterators.
	// Commonly used for removing NULL pointers from collections.
	//////////////////////////////////////////////////////////////////////////
	template <class Container>
	inline void find_and_erase_all(Container& container, const typename Container::value_type& value)
	{
		// Shuffles all elements != value to the front and returns the start of the removed elements.
		typename Container::iterator endIter(container.end());
		typename Container::iterator newEndIter(std::remove(container.begin(), endIter, value));

		// Delete the removed range at the back of the container (low-cost for vector).
		container.erase(newEndIter, endIter);
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find and erase element from map.
	// @return true if item was find and erased, false if item not found.
	//////////////////////////////////////////////////////////////////////////
	template <class Container, class Key>
	inline bool member_find_and_erase(Container& container, const Key& key)
	{
		typename Container::iterator it = container.find(key);
		if (it != container.end())
		{
			container.erase(it);
			return true;
		}
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container, class Value>
	inline bool push_back_unique(Container& container, const Value& value)
	{
		if (std::find(container.begin(), container.end(), value) == container.end())
		{
			container.push_back(value);
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Push back to container unique element.
	// @return true if item added, false overwise.
	template <class Container, class Iter>
	inline void push_back_range(Container& container, Iter begin, Iter end)
	{
		for (Iter it = begin; it != end; ++it)
		{
			container.push_back(*it);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in container.
	// @return true if item found.
	template <class Container, class Value>
	inline bool find(Container& container, const Value& value)
	{
		return std::find(container.begin(), container.end(), value) != container.end();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	//
	template <class Iterator, class T>
	inline Iterator binary_find(Iterator first, Iterator last, const T& value)
	{
		Iterator it = std::lower_bound(first, last, value);
		return (it == last || value != *it) ? last : it;
	}

	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// @return true if item was inserted.
	template <class Container, class Value>
	inline bool binary_insert_unique(Container& container, const Value& value)
	{
		typename Container::iterator it = std::lower_bound(container.begin(), container.end(), value);
		if (it != container.end())
		{
			if (*it == value)
				return false;
			container.insert(it, value);
		}
		else
			container.insert(container.end(), value);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	//! Find element in a sorted container using binary search with logarithmic efficiency.
	// and erases if element found.
	// @return true if item was erased.
	template <class Container, class Value>
	inline bool binary_erase(Container& container, const Value& value)
	{
		typename Container::iterator it = std::lower_bound(container.begin(), container.end(), value);
		if (it != container.end() && *it == value)
		{
			container.erase(it);
			return true;
		}
		return false;
	}

	struct container_object_deleter
	{
		template<typename T>
		void operator()(const T* ptr) const
		{
			delete ptr;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Convert arbitary class to const char*
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
	inline const char* constchar_cast(const Type& type)
	{
		return type;
	}

	//! Specialization of string to const char cast.
	template <>
	inline const char* constchar_cast(const std::string& type)
	{
		return type.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	//! Case sensetive less key for any type convertable to const char*.
	//////////////////////////////////////////////////////////////////////////
	template <class Type>
	struct less_strcmp : public std::binary_function<Type, Type, bool>
	{
		bool operator()(const Type& left, const Type& right) const
		{
			return strcmp(constchar_cast(left), constchar_cast(right)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case insensetive less key for any type convertable to const char*.
	template <class Type>
	struct less_stricmp : public std::binary_function<Type, Type, bool>
	{
		bool operator()(const Type& left, const Type& right) const
		{
			return _stricmp(constchar_cast(left), constchar_cast(right)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// Hash map usage:
	// typedef stl::hash_map<string,int, stl::hash_stricmp<string> > StringToIntHash;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// useful when the key is already the result of an hash function
	// key needs to be convertible to size_t
	//////////////////////////////////////////////////////////////////////////
	template <class Key>
	class hash_simple
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};// min_buckets = 2 ^^ N, 0 < N

		size_t operator()(const Key& key) const
		{
			return size_t(key);
		};
		bool operator()(const Key& key1, const Key& key2) const
		{
			return key1 < key2;
		}
	};

	// simple hash class that has the avalanche property (a change in one bit affects all others)
	// ... use this if you have uint32 key values!
	class hash_uint32
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8  // min_buckets = 2 ^^ N, 0 < N
		};

		inline size_t operator()(unsigned int a) const
		{
			a = (a + 0x7ed55d16) + (a << 12);
			a = (a ^ 0xc761c23c) ^ (a >> 19);
			a = (a + 0x165667b1) + (a << 5);
			a = (a + 0xd3a2646c) ^ (a << 9);
			a = (a + 0xfd7046c5) + (a << 3);
			a = (a ^ 0xb55a4f09) ^ (a >> 16);
			return a;
		};
		bool operator()(unsigned int key1, unsigned int key2) const
		{
			return key1 < key2;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case sensitive string hash map compare structure.
	//////////////////////////////////////////////////////////////////////////
	template <class Key>
	class hash_strcmp
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};// min_buckets = 2 ^^ N, 0 < N

		size_t operator()(const Key& key) const
		{
			unsigned int h = 0;
			const char* s = constchar_cast(key);
			for (; *s; ++s) h = 5 * h + *(unsigned char*)s;
			return size_t(h);

		};
		bool operator()(const Key& key1, const Key& key2) const
		{
			return strcmp(constchar_cast(key1), constchar_cast(key2)) < 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//! Case insensitive string hash map compare structure.
	template <class Key>
	class hash_stricmp
	{
	public:
		enum {	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8
		};// min_buckets = 2 ^^ N, 0 < N

		size_t operator()(const Key& key) const
		{
			unsigned int h = 0;
			const char* s = constchar_cast(key);
			for (; *s; ++s) h = 5 * h + tolower(*(unsigned char*)s);
			return size_t(h);

		};
		bool operator()(const Key& key1, const Key& key2) const
		{
			return stricmp(constchar_cast(key1), constchar_cast(key2)) < 0;
		}
	};

#ifdef USE_HASH_MAP
	//C:\Work\Main\Code\SDKs\STLPORT\stlport

	// Support for both Microsoft and SGI kind of hash_map.

#ifdef _STLP_HASH_MAP 
	// STL Port
	template <class _Key, class _Predicate = std::less<_Key> >
	struct hash_compare
	{
		enum
		{	// parameters for hash table
			bucket_size = 4,	// 0 < bucket_size
			min_buckets = 8   // min_buckets = 2 ^^ N, 0 < N
		};

		size_t operator()(const _Key& _Keyval) const
		{
			// return hash value.
			uint32 a = _Keyval;
			a = (a + 0x7ed55d16) + (a << 12);
			a = (a ^ 0xc761c23c) ^ (a >> 19);
			a = (a + 0x165667b1) + (a << 5);
			a = (a + 0xd3a2646c) ^ (a << 9);
			a = (a + 0xfd7046c5) + (a << 3);
			a = (a ^ 0xb55a4f09) ^ (a >> 16);
			return a;
		}

		// Less then function.
		bool operator()(const _Key& _Keyval1, const _Key& _Keyval2) const
		{	// test if _Keyval1 ordered before _Keyval2
			_Predicate comp;
			return (comp(_Keyval1, _Keyval2));
		}
	};

	template <class Key, class HashFunc>
	struct stlport_hash_equal
	{
		// Equal function.
		bool operator()(const Key& k1, const Key& k2) const
		{
			HashFunc less;
			// !(k1 < k2) && !(k2 < k1)
			return !less(k1, k2) && !less(k2, k1);
		}
	};

	template <class Key, class Value, class HashFunc = hash_compare<Key>, class Alloc = std::allocator< std::pair<Key, Value> > >
	struct hash_map : public std__hash_map<Key, Value, HashFunc, stlport_hash_equal<Key, HashFunc>, Alloc>
	{
		hash_map() : std__hash_map<Key, Value, HashFunc, stlport_hash_equal<Key, HashFunc>, Alloc>(HashFunc::min_buckets) {}
	};

	/*
	//template <class Key,class Value,class HashFunc=hash_compare<Key>,class Alloc = std::allocator< std::pair<Key,Value> > >
	template <class Key>
	public hash_map1
	{
	public:
		hash_map1() {};
		//hash_map1() : std__hash_map<Key,Value,HashFunc<Key>,HashFunc<Key>,Alloc>( HashFunc::min_buckets ) {}
	};
	*/
#else
	// MS STL
	using stdext::hash_compare;
	using stdext::hash_map;
#endif

#else // USE_HASH_MAP

#endif //USE_HASH_MAP




	//////////////////////////////////////////////////////////////////////////
	template<class T>
	class intrusive_linked_list_node
	{
	public:
		intrusive_linked_list_node() { link_to_intrusive_list(static_cast<T*>(this)); }
		// Not virtual by design
		~intrusive_linked_list_node() { unlink_from_intrusive_list(static_cast<T*>(this)); }

		static T* get_intrusive_list_root() { return m_root_intrusive; };

		static void link_to_intrusive_list(T* pNode)
		{
			if (m_root_intrusive)
			{
				// Add to the beginning of the list.
				T* head = m_root_intrusive;
				pNode->m_prev_intrusive = 0;
				pNode->m_next_intrusive = head;
				head->m_prev_intrusive = pNode;
				m_root_intrusive = pNode;
			}
			else
			{
				m_root_intrusive = pNode;
				pNode->m_prev_intrusive = 0;
				pNode->m_next_intrusive = 0;
			}
		}
		static void unlink_from_intrusive_list(T* pNode)
		{
			if (pNode == m_root_intrusive) // if head of list.
			{
				m_root_intrusive = pNode->m_next_intrusive;
				if (m_root_intrusive)
				{
					m_root_intrusive->m_prev_intrusive = 0;
				}
			}
			else
			{
				if (pNode->m_prev_intrusive)
				{
					pNode->m_prev_intrusive->m_next_intrusive = pNode->m_next_intrusive;
				}
				if (pNode->m_next_intrusive)
				{
					pNode->m_next_intrusive->m_prev_intrusive = pNode->m_prev_intrusive;
				}
			}
			pNode->m_next_intrusive = 0;
			pNode->m_prev_intrusive = 0;
		}

	public:
		static T* m_root_intrusive;
		T* m_next_intrusive;
		T* m_prev_intrusive;
	};
}

#define DEFINE_INTRUSIVE_LINKED_LIST( Class ) \
	template<> Class* stl::intrusive_linked_list_node<Class>::m_root_intrusive = 0;

#endif