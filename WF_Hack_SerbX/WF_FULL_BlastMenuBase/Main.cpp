#include "stdafx.h"
#include<iostream>
#include<fstream>
#include"XorStr.h"


void Log(const char* fmt, ...)
{
	if (!fmt)	return;

	char		text[4096];
	va_list		ap;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

	std::ofstream logfile(XorString("C:\\SerbX\\Wf_log.txt"), std::ios::app);
	if (logfile.is_open() && text)	logfile << text << std::endl;
	logfile.close();
}

float ScreenCX = 1882 / 2;
float ScreenCY = 1013 / 2;
SSGE* pSSGE = pSSGE->GetSSGE();
IGFW* pFrameWork = IGFW::GetGameFramework();
Vec3 GetPlayerPos(IEntity* pEntit)
{
	Vec3 vOffset = Vec3();
	Matrix34 pWorld = pEntit->GetWorldTM();
	vOffset = pWorld.GetTranslation();
	return vOffset;
}

Vec3 GetBonePositionByID(IEntity* pEnt, int BoneID)
{
	char* getBoneName;
	ICharacterInstance* pCharacterInstance = pEnt->GetCharacter(0);
	ISkeletonPose* pSkeletonPose = pCharacterInstance->GetISkeletonPose();
	int sBoneID = BoneID;
	Matrix34 World = pEnt->GetWorldTM();
	Matrix34 SkeletonAbs = Matrix34(pSkeletonPose->GetBoneByID(sBoneID));
	Matrix34 matWorld = World;
	float m03, m13, m23;
	m03 = (World.m00 * SkeletonAbs.m03) + (World.m01 * SkeletonAbs.m13) + (World.m02 * SkeletonAbs.m23) + World.m03;
	matWorld.m03 = m03;
	m13 = (World.m10 * SkeletonAbs.m03) + (World.m11 * SkeletonAbs.m13) + (World.m12 * SkeletonAbs.m23) + World.m13;
	matWorld.m13 = m13;
	m23 = (World.m20 * SkeletonAbs.m03) + (World.m21 * SkeletonAbs.m13) + (World.m22 * SkeletonAbs.m23) + World.m23;
	matWorld.m23 = m23;

	return matWorld.GetTranslation();
}

bool WorldToScreen(Vec3 vEntPos, Vec3* sPos)
{
	IRenderer::w2s_info info;
	info.Posx = vEntPos.x;
	info.Posy = vEntPos.y;
	info.Posz = vEntPos.z;

	info.Scrnx = &sPos->x;
	info.Scrny = &sPos->y;
	info.Scrnz = &sPos->z;

	pSSGE->pGetRender()->ProjectToScreen(&info);

	if (sPos->z < 0.0f || sPos->z > 1.0f) {
		return 0;
	}

	sPos->x *= (1882 / 100.0f);
	sPos->y *= (1013 / 100.0f);
	sPos->z *= 1.0f;

	return (sPos->z < 1.0f);
}
float GetDistance(float Xx, float Yy, float xX, float yY)
{
	return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
}
bool infov(float aimfov, Vec3 enemy) {
	//ScreenCX = viewport.Width / 2; ScreenCY = viewport.Height / 2;
	float radiusx = (aimfov) * (ScreenCX / 100);
	float radiusy = (aimfov) * (ScreenCY / 100);

	if (aimfov <= 0)
	{
		radiusx = 2.0f * (ScreenCX / 100);
		radiusy = 2.0f * (ScreenCY / 100);
	}

	if (enemy.x >= ScreenCX - radiusx && enemy.x <= ScreenCX + radiusx && enemy.y >= ScreenCY - radiusy && enemy.y <= ScreenCY + radiusy)
		return true;
	else
		return false;
}
Menu g_menu;

std::string	framerate = XorString("xxx Fps");

// Menu Values
int p = 1;
int fp = 1;
int selectedpvp = -1;
 int		CH_back = 2;		    // Background
 bool	CH_Aimb = false;
 bool	CH_Otline = false;
 bool	CH_vis = false;
 bool	CH_fastflag = false;
 bool   CH_WPS = false;
 bool ch_heavykill = false;
 bool	WP_noReload = false;
 bool	WP_norecoil = false;
 bool	WP_unlammo = false;
 bool   wp_autoshot = false;

 bool CH_AFK = false;
 bool ch_names = false;
 bool ch_wpsn = false;

 int	IN_numbers = 69;
 std::vector<Vec3> EntsHead;
 std::vector<Vec3> MyTvPos;
 std::vector<IEntity*> MyTeamV;
 std::vector<IEntity*> AllEnt;
 std::vector<IActor*> EnmTeamV;
 std::vector<IEntity*> EnmTeamV1;
 std::vector< WeaponRender> WeaponRen;
 std::vector<float> nameDis;
 std::vector<NameRender> NameRen;
 bool f4_r = false;
 bool onceVP = false;
// Menu Options
 OptionValue opt_Aura = { { XorString("Off"), XorString("Pve")/*, XorString("PvP fool pgdn"),XorString("PvP Suicide"),XorString("Kick My Team"),XorString("PVP kick Enemies")*/ }, 0 };

void BuildMenu()
{
	
	g_menu = Menu();

	MenuGroup cheats = MenuGroup(XorString("Cheats"));
	cheats.AddTextArray(XorString("AuraKill"), &opt_Aura);
    cheats.AddBool(XorString("kill_Heavy"), &ch_heavykill);
	cheats.AddBool(XorString("Vis"), &CH_vis);
	cheats.AddBool(XorString("0utlin"), &CH_Otline);
	cheats.AddBool(XorString("Slde clmb"), &CH_fastflag);
	cheats.AddBool(XorString("AiB"), &CH_Aimb);
    cheats.AddNumber(XorString("aipos"), &p, 17, 1);
	cheats.AddNumber(XorString("A_F0v"), &fp, 17, 1);
	cheats.AddBool(XorString("AFFKK"), &CH_AFK);
    cheats.AddBool(XorString("Enmi N3mS"), &ch_names);
    cheats.AddBool(XorString("WPS"), &CH_WPS);
    cheats.AddBool(XorString("WPS_Name"), &ch_wpsn);

	MenuGroup weapons = MenuGroup(XorString("Weapons"));
	weapons.AddBool(XorString("No Rel"), &WP_noReload);
	weapons.AddBool(XorString("No Rec"), &WP_norecoil);
	weapons.AddBool(XorString("Unled Amo"), &WP_unlammo);
    weapons.AddBool(XorString("Autoshot"), &wp_autoshot);

	MenuGroup info = MenuGroup(XorString("Info"));
	info.AddText(XorString("F/S"), &framerate);


	g_menu.Add(cheats);
	g_menu.Add(weapons);
	g_menu.Add(info);

}
float Distance(Vec3 VecA, Vec3 VecB)
{
	return sqrt(((VecA.x - VecB.x) * (VecA.x - VecB.x)) +
		((VecA.y - VecB.y) * (VecA.y - VecB.y)) +
		((VecA.z - VecB.z) * (VecA.z - VecB.z)));
}
std::vector<std::string> Names;
std::vector<Vec3> Ents;

bool once = false;
Font m_enm_font, mt2_font , m_my_menu_font;
std::string aa;
ID3DXFont* pfont;
//IDirect3DDevice9* pDevice;
float dist = 500;
void Direct3D9Capture::ShowNMS(Renderer* rr) {
	if (!once )
	{
		m_enm_font = Font(XorString("Kremlin"), 17);
		m_my_menu_font = Font(XorString("Helvetica Neue, Roboto, Arial, Droid Sans, sans - serif"), 17);
		mt2_font = Font(XorString("Helvetica Neue, Roboto, Arial, Droid Sans, sans - serif"), 13);
		once = true;
	}
	rr->SetFontColor(0xFF13FF00);
	
	int siz = Names.size();
	for (int i = 0; i < siz; i++){
		if (siz != Names.size())
			break;
		//std::string str = wchar_t_2_str(Names[i]);
		//std::string str1 = std::to_string(nameDis[i]);
		//str1.append(" "); str1.append(Names[i]);
		rr->AddText(mt2_font, Ents[i].x, Ents[i].y, Names[i]);
	}
	if (ch_wpsn) {
		siz = WeaponRen.size();
		for (int i = 0; i < siz; i++)
		{
			if (siz != WeaponRen.size())
				break;
			dist = Distance(WeaponRen[i].CamPos, WeaponRen[i].Pos);
			if (dist < 100.0f)
				{
					aa.append(std::to_string((int)dist));
					aa.append("  ");
					aa.append(WeaponRen[i].name);
					rr->AddText(mt2_font, WeaponRen[i].W2sPos.x, WeaponRen[i].W2sPos.y, aa );
					aa.clear();
				}
		}
	}

	if(CH_WPS && p == 5)
		for (int i = 0; i < NameRen.size(); i++)
		{
			aa.clear();
			aa.append(NameRen[i].name);
			rr->AddText(mt2_font, NameRen[i].x, NameRen[i].y, aa);
		}
}
//std::string workornot = "Checking / or Expired";;
std::string abx;
std::string haha = "";
void Direct3D9Capture::ShowStateXX(Renderer* rrr) {
	if (g_menu.m_minimized)
		return;
       /*if (WSN)
       workornot = "is Working";
	   
       abx = "Working State: " + workornot;*/
	    /*rrr->SetFontColor(0xFFFFFFFF);
		rrr->AddText(m_my_menu_font, 5, 5, abx);
		rrr->SetFontColor(0xFFAF8FBF);
		if (opt_Aura.Selected == 2 || opt_Aura.Selected == 1) {
			haha = "";
			haha.append(XorString("selected player number: "));
			haha.append(std::to_string(selectedpvp));
			rrr->AddText(m_my_menu_font, 15, 25, haha);
			
			
			int ssi = MyTeamV.size();
			if ((ssi > 0 && !opt_Aura.Selected == 1) || (opt_Aura.Selected == 1&& selectedpvp > -1))
			{
				rrr->AddText(m_my_menu_font, 530, 180, XorString("My Team"));
				for (size_t i = 0; i < MyTeamV.size(); i++)
				{
					haha = ""; haha.append(std::to_string(i)); haha.append("  ");
					if (ssi != MyTeamV.size())break;
					haha.append(MyTeamV[i]->GetName());
					rrr->AddText(m_my_menu_font, 500, 200 + (i * 20), haha);
				}
			}
			if (opt_Aura.Selected == 1)
				return;
			ssi = EnmTeamV.size();
			if (ssi > 0) {
				rrr->AddText(m_my_menu_font, 380, 180 , XorString("Enemy Team"));
				for (size_t i = 0; i < EnmTeamV.size(); i++)
				{
					haha = ""; haha.append(std::to_string(i)); haha.append("  ");
					if (ssi != EnmTeamV.size())break;
					haha.append(EnmTeamV[i]->get_entity()->GetName());
					rrr->AddText(m_my_menu_font, 350, 200 + (i * 20), haha);
				}
			}

		}*/
	
}

void RunStuff()
{
	g_menu.Navigate();
}

void AKK() {
	while (true)
	{
		Sleep(1000);
		if (CH_AFK)
		{

			for (__int8 i = 0; i < 40; i++)
			{
				__int8 x = (rand() % 3);
				__int8 y = (rand() % 3);

				mouse_event(MOUSEEVENTF_MOVE, x, 0, 0, NULL);
				Sleep(50);
			}


		}
	}
};
bool autoAura = false;;
void ara()
{
	
	while (true)
	{
		if (opt_Aura.Selected == 2 || opt_Aura.Selected == 1)
		{
			if (GetKeyState(VK_NUMPAD0) & 0x800) selectedpvp = 0;
			if (GetKeyState(VK_NUMPAD1) & 0x800) selectedpvp = 1;
			if (GetKeyState(VK_NUMPAD2) & 0x800) selectedpvp = 2;
			if (GetKeyState(VK_NUMPAD3) & 0x800) selectedpvp = 3;
			if (GetKeyState(VK_NUMPAD4) & 0x800) selectedpvp = 4;
			if (GetKeyState(VK_NUMPAD9) & 0x800) selectedpvp = -1;
		}
		if (GetKeyState(VK_CONTROL) & 0x8000 && GetKeyState(VK_PRIOR) & 0x8000)
			while (true) {
				Sleep(500);
				if (!autoAura)
					autoAura = true;
				if (GetKeyState(VK_LSHIFT) & 0x8000 && GetKeyState(VK_PRIOR) & 0x8000)
					break;
			}


	}
}
int GetBoneObject(ISkeletonPose* m_pSkeleton, const char* Name, const char* ClassName) {
//	if (strstr(ClassName, "SEDSoldier")) return m_pSkeleton->GetJointIDByNameParams("Bip01 L Calf");
	if (strstr(Name, XorString("HeavyTurret"))) return m_pSkeleton->GetJointIDByNameParams(XorString("bone_hitpoint"));
	if (strstr(Name, XorString("HeavyDroid"))) return 61;
	if (strstr(Name, XorString("JP_Flamethrower"))) return m_pSkeleton->GetJointIDByNameParams(XorString("Bip01 Neck"));
	if (strstr(Name, "Iron")) return m_pSkeleton->GetJointIDByNameParams(XorString("weaponPos_law"));

	if (strstr(Name, XorString("Helicopter"))) return m_pSkeleton->GetJointIDByNameParams(XorString("weapon_bone"));//weaponPos_law
	//if (strstr(ClassName, "Helicopter")) return m_pSkeleton->GetJointIDByNameParams("weapon_bone");//weaponPos_law
	//if (!strcmp(ClassName, "Helicopter")) return m_pSkeleton->GetJointIDByNameParams("weapon_bone");//weaponPos_law

	//if (strstr(ClassName, "Turret")) return m_pSkeleton->GetJointIDByNameParams("bone_head");
	if (strstr(Name, XorString("Cyborg")) || strstr(Name, XorString("Destroyer"))) return m_pSkeleton->GetJointIDByNameParams(XorString("Bip01 Head"));
     
	return -1;
}
__int8 array_Bones[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,19,20,21,23,24,25,26,27,28,33,47,48,49,50,53,61,70,71,72,79,80 };
void RunChees() {
	//if (GetKeyState(VK_SPACE) & 0x800)
	//{
	//	DWORD64 *BaseAddress = (DWORD64*)GetModuleHandleA("Game.exe");
	//	Log("BaseAddress 0x%llx", BaseAddress);

	//}
	if (!onceVP) {
		ScreenCX = viewport.Width / 2; ScreenCY = viewport.Height / 2;
		once = true;
	}
	float nearTarg = 43686;
	IPhysicalWorld* ppsw = NULL;
	IEntitySystem* pEntSys = NULL;
	IEntityIt* pEntIt = NULL;
	IRenderer* pren = NULL;
	I3DEngine* p3DEngine = NULL;
	IEntityRenderProxy* pEntityRenderProxy = NULL;
	IGameRules* pGRL = NULL;
	IEntity* MEntity = NULL;
	IActor* MPlayer = NULL;
	IActor* LocPlayer = NULL;
	IActor* LocPlayer1 = NULL;
	CVars* pCvar = NULL;
	//IInventory *inventory = NULL;

	Names.clear();
	Ents.clear();
	MyTeamV.clear();
	EnmTeamV.clear();
	EnmTeamV1.clear();
	MyTvPos.clear();
	//AllEnt.clear();
	WeaponRen.clear();
	NameRen.clear();
	//nameDis.clear();
	pEntSys = pSSGE->pGetEntitySystem();
	p3DEngine = pSSGE->pGet3DEngine();
	ppsw = pSSGE->pGetIPhysicalWorld();
	pren = pSSGE->pGetRender();

	if (!pEntSys || !p3DEngine || !ppsw || !pren)
		return;

	pEntIt = pEntSys->GetEntityIterator();
	pGRL = pFrameWork->GetIGameRules();

	if (!pEntIt || !pGRL)
		return;


	if (!(pFrameWork->GetClientActor(&MPlayer)))
		return;

	//if (GetKeyState(VK_CAPITAL)& 0x800)
	//{
	//	//pFrameWork->GetIItemSystem()->GetItem(65528);
	//	//pFrameWork->GetIItemSystem()->GiveItem(MPlayer, "kn20", 0, 0, 0, 0, EEntityFlags::ENTITY_FLAG_NO_SAVE);
	//	MPlayer->GetInventory()->GetItem(4649);
	//	pFrameWork->GetIItemSystem()->GiveItem(MPlayer, "sr09", 0, 0, 0, 0, EEntityFlags::ENTITY_FLAG_NO_SAVE);
	//	pFrameWork->GetIItemSystem()->SetActorItemByName(MPlayer, "sr09", 1);
	//	Sleep(500);
	//}
	//if (GetKeyState(VK_TAB) & 0x800)
	//{
	//	MPlayer->GetInventory()->GetItem(4732);
	//	pFrameWork->GetIItemSystem()->GetItem(4732);
	//	pFrameWork->GetIItemSystem()->GiveItem(MPlayer, "ar32", 0, 0, 0, 0, EEntityFlags::ENTITY_FLAG_NO_SAVE);
	//	pFrameWork->GetIItemSystem()->SetActorItemByName(MPlayer, "ar32", 0);
	//	Sleep(500);
	//	//int x = MPlayer->GetInventory()->GetCurrentItemId();
	////	Log("CurrentItemID %i", x);
	//}
	while (IEntity* pEnt = pEntIt->Next())
	{

		bool vis = false;
		unsigned __int8  red = 255, green = 0, blue = 255, bone = 8;
		__int8 Myteam = 0;
		__int8 EntTeam = 0;
		DWORD proj = rand();
		if (!pEnt /*|| pEnt == MEntity*/)
			continue;

		if(CH_WPS && p == 5)
			if (const char* nmm = pEnt->GetName()) {
				//AllEnt.push_back(pEnt);
				Vec3 N_out; NameRender ren;
				WorldToScreen(GetPlayerPos(pEnt),&N_out);
				ren.x = N_out.x;
				ren.y = N_out.y;
				ren.name = nmm;
				NameRen.push_back(ren);
			}

		/*if (CH_WPS && !MPlayer->isDeadByH() && MPlayer->GetHealth() < (MPlayer->getMaxHealth()-1)) {
			int hl = MPlayer->GetHealth();
			MPlayer->SetHealth(hl+1);
		}*/
			
		

		const char* entname;

		if (entname = pEnt->GetName()) {

			if (ch_heavykill)
				if (strstr(entname, XorString("Arena_IronMan")))
				{
					if (!(LocPlayer = pFrameWork->GetIActorSystem()->GetActor(pEnt->GetID())))
						continue;
					for (__int8 i = 0; i < 2; i++) {
						KillAura1(MPlayer, pEnt, (DWORD)rand(), 0, GetPlayerPos(pEnt), pFrameWork, XorString("weaponPos_law"), pSSGE, XorString("bullet"));
					}
					continue;
				}
				else;
			else;

			if (strstr(entname, XorString("Turret")) ||
				strstr(entname, XorString("IronMan")) ||
				strstr(entname, XorString("Cyborg")) ||
				/*strstr(entname, XorString("SEDSoldier")) ||*/
				strstr(entname, XorString("Mantis")) ||
				strstr(entname, XorString("mech")) ||
				strstr(entname, XorString("Drone")) ||
				strstr(entname, XorString("Flagsh")) ||
				strstr(entname, XorString("Helicopter"))) {
				green = 255; red = 255; blue = 45;
				goto outline;
			}

			if (CH_WPS)
				if (strstr(entname, XorString("@"))) 
				{
					green = 255; red = 255; blue = 255;
					if (ch_wpsn) {
						WeaponRender WR_temp; Vec3 PosWp;
						WR_temp.name = (char*)entname;
						WR_temp.Pos = PosWp = GetPlayerPos(pEnt);
						WR_temp.CamPos = pren->GetViewCamera();
						Vec3 outttt;
						WorldToScreen(PosWp, &outttt);
						WR_temp.W2sPos = outttt;
						WeaponRen.push_back(WR_temp);
					}
					goto outline;
				}
				else if (strstr(entname, XorString("Loot")))
				{
					green = 255; red = 0; blue = 0;
					goto outline;
				}
			
		}
		else continue;

		//if (strstr(pEnt->GetName(), "capsule") && strstr(pEnt->GetName(), "turret")) {
		//	AllEnt.push_back(pEnt);
		//}


		if (!(LocPlayer = pFrameWork->GetIActorSystem()->GetActor(pEnt->GetID())))
			continue;
		if (!(pFrameWork->GetClientActor(&MPlayer)))
			continue;

		if (MEntity = MPlayer->get_entity());
		else continue;

		Myteam = MPlayer->get_team_id();
		EntTeam = LocPlayer->get_team_id();



		if (Myteam != EntTeam)
			EnmTeamV.push_back(LocPlayer);
		else {
			//MyTvPos.push_back(GetBonePositionByID(pEnt, 13));
			MyTeamV.push_back(pEnt);
		}

		if (MPlayer->isDead())
			continue;

		if (LocPlayer->GetHealth() <= 0)
			continue;

		if (WP_noReload) {
			if (auto m_pItem = MPlayer->GetCurrentItem(0))
				if (auto weapon = m_pItem->GetIWeapon())
					if (auto wptr = weapon->pWeaponPTR()) {
						wptr->GetShutter(0);
						//wptr->GetIWeaponParamOne->m_iPellets = 15;
						if (WP_unlammo) {
							wptr->GetIWeaponParamThree->m_bUnlimitedAmmo = true;
							if (GetKeyState(0x42) & 0x800) {
								wptr->GetIWeaponParamThree->m_iAmmo = 0;
								weapon->Reload(true);
							}
						}
						else wptr->GetIWeaponParamThree->m_bUnlimitedAmmo = false;
						if (GetKeyState(VK_RCONTROL) & 0X800) {
							weapon->StartFire();
							weapon->StopFire();
						}

					}

		}

		if (CH_fastflag) {
			if (auto pGam = pSSGE->pGetGame()) {
				pGam->SetPlayer(ClimbExtraHeight, 20.0f);
				pGam->SetPlayer(SlideMultDistance, 14.f);
				//pGam->SetPlayer(AntiAfk, 9999.f);
			}
		}

		if (WP_norecoil != f4_r) {
			//MPlayer->GetPlayer()->m_ignoreRecoil = WP_norecoil;
			
			f4_r = WP_norecoil;
		}
		if (WP_norecoil)
		MPlayer->RecoilAnimation(0);



		if (LocPlayer == MPlayer)
			continue;


		if (isEnemyTeam(EntTeam,Myteam)) { // 0 and 0 is for free for all

			Vec3 Out1 ;

			if (!WorldToScreen(GetPlayerPos(pEnt), &Out1))
				continue;

			if (ch_names) {
				//float ndis = Distance(pren->GetViewCamera(), GetPlayerPos(pEnt));
				//nameDis.push_back(ndis);
				Names.push_back(pEnt->GetName());
				Ents.push_back(Out1);
			}


			if (CH_vis) {

				if (p == 1)
					bone = pEnt->GetCharacter(0)->GetISkeletonPose()->GetJointIDByNameParams(XorString("Bip01 Head"));
				else
					bone = p;
				//Vec3 camp = pren->GetViewCamera();
				if (!(LocPlayer->GetHealth() <= 0) && !(MPlayer->GetHealth() <= 0)) {
					MPlayer->claymore = 1000.f;
					Vec3 camp1 = pSSGE->pGetRender()->GetViewCamera();
					//Vec3 mPOS = GetBonePositionByID(MEntity, 13);

					Vec3 ePOS = GetBonePositionByID(pEnt, bone), EXPOS;


					if (GetKeyState(VK_RSHIFT) & 0x8000) {
						if (opt_Aura.Selected == 1)
						{
							KillAura(MPlayer, pEnt, proj, bone, ePOS, pFrameWork, pSSGE);
						}
						if (opt_Aura.Selected == 5)
						{
							if(LocPlayer->IsPlayer())
							KillAura(LocPlayer, MPlayer->get_entity(), proj, bone, ePOS, pFrameWork, pSSGE);
						}
					}
				

					if(bone == 4 || !CH_Aimb)
					for (__int8 i = 1; i <= 25; i++)
					{
						__int8 radid = array_Bones[rand() % 37];

						ePOS = GetBonePositionByID(pEnt, radid);

						if (ppsw->IsVisible(ePOS, 0.f, camp1)) {
							blue = 0;
							vis = true;
							Out1.z = 1;
							bone = radid;

							break;
						}
					}
					else
						if (ppsw->IsVisible(ePOS, 0.f, camp1)) {
							blue = 0;
							vis = true;
							//EXPOS.z = 1;
							Out1.z = 1;
						}
					
					//EntsHead.push_back(EXPOS);
				}


			}

		

			if (CH_Aimb) {

				if (!pEnt)
					continue;
				Vec3 OUTT, IDPOS;
				if (LocPlayer->isDeadByH())
					continue;
				IDPOS = GetBonePositionByID(pEnt, bone);

				if (WorldToScreen(IDPOS, &OUTT)) {
					bool aim;
					if (CH_vis)
						aim = (infov(fp * 2, OUTT) && vis) ? true : false;
					else
						aim = infov(fp * 2, OUTT) ? true : false;
					Vec3 norm = pren->GetViewCamera();
					/*Vec3 aimPos = (vis && Fk) ? IDPOS : norm;*/
					if (aim)
					{
						float crosshairdis = GetDistance(OUTT.x, OUTT.y, ScreenCX, ScreenCY);
						float DistX = OUTT.x - ScreenCX;
						float DistY = OUTT.y - ScreenCY;

						if (crosshairdis <= nearTarg)
						{
							if (GetKeyState(0x46)& 0x800) //F key 
							{
									mouse_event(MOUSEEVENTF_MOVE, (int)DistX/4 , (int)DistY/4 , 0, NULL);
								nearTarg = crosshairdis;
							}
						}

					}

				}
			}

		outline:
			EnmTeamV1.push_back(pEnt);
			if (CH_Otline) {
				pEntityRenderProxy = NULL;
				pEntityRenderProxy = (IEntityRenderProxy*)(pEnt->GetProxy(ENTITY_PROXY_RENDER));
				if (!pEntityRenderProxy)
					continue;
				pEntityRenderProxy->SetRndFlags(eAllMap);
				pEntityRenderProxy->SetHUDSilhouettesParams(255, red, green, blue);
			}

		}


	}

	if(wp_autoshot)
		if (auto m_pItem = MPlayer->GetCurrentItem(0))
			if (auto weapon = m_pItem->GetIWeapon())
			{
				weapon->StartFire();
				weapon->StopFire();
			}

	//if (GetKeyState(VK_NEXT) & 0x8000 && opt_Aura.Selected == 2) {
	//	int en = 0;

	//	if (selectedpvp >= 0)
	//	{
	//		en = selectedpvp; goto jhere;
	//	}
	//	if (EnmTeamV.size() > 0)
	//		en = rand() % EnmTeamV.size();
	//	if (EnmTeamV[en]->isDeadByH())
	//		en = Find_AliveID(EnmTeamV);
	//	if (en == -1)
	//		return;
	//jhere:

	//	try {
	//		for (__int8 i = 0; i < EnmTeamV.size(); i++)
	//		{
	//			if (EnmTeamV.size() < 0) break;
	//			if (!pFrameWork->GetIActorSystem()->GetActor(MyTeamV[i]->GetID())->isDeadByH() && !EnmTeamV[en]->isDeadByH())
	//				KillAura(EnmTeamV[en], MyTeamV[i], (DWORD)rand(), 13, GetBonePositionByID(MyTeamV[i], 13), pFrameWork, pSSGE);
	//		}
	//	}
	//	catch (const std::out_of_range & oor) {};
	//	//Sleep(25);
	//}

	//if (GetKeyState(VK_DELETE) & 0x8000 && opt_Aura.Selected >= 3) {
	//	int en = 0;
	//	int siz = EnmTeamV.size();
	//	if (EnmTeamV.size() > 0)
	//		en = rand() % EnmTeamV.size();
	//	else return;
	//	if (siz != EnmTeamV.size())
	//		return;
	//	if (EnmTeamV.size() > 0 && EnmTeamV[en]->isDeadByH())
	//		en = Find_AliveID(EnmTeamV);
	//	if (en == -1)
	//		return;

	//	try {
	//		
	//		for (__int8 i = 0; i < MyTeamV.size(); i++)
	//		{
	//			if (/*!pFrameWork->GetIActorSystem()->GetActor(MyTeamV[i]->GetID())->isDead() &&*/ !EnmTeamV[en]->isDead())
	//				if (opt_Aura.Selected == 5) // pvp kick
	//					KillAura(EnmTeamV[en], EnmTeamV[en]->get_entity(), (DWORD)rand(), 13, GetBonePositionByID(MyTeamV[i], 13), pFrameWork, pSSGE);
	//				else if (opt_Aura.Selected == 3) //pvp suicide;
	//					KillAura(EnmTeamV[en], EnmTeamV[en]->get_entity(), (DWORD)rand(), 13, GetBonePositionByID(EnmTeamV[en]->get_entity(), 13), pFrameWork, pSSGE);
	//				else if (opt_Aura.Selected == 4) // kick my team
	//					if (auto Actor = pFrameWork->GetIActorSystem()->GetActor(MyTeamV[i]->GetID()))
	//						if (Actor != MPlayer && EnmTeamV[en] != Actor)
	//							if (int id = EnmTeamV[en]->get_entity()->GetCharacter(0)->GetISkeletonPose()->GetJointIDByNameParams(XorString("Bip01 Head")))
	//								KillAura(Actor, EnmTeamV[en]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(Actor->get_entity(), id), pFrameWork, pSSGE);
	//		}
	//	}
	//	catch (const std::out_of_range & oor) {};
	//	Sleep(150);
	//}

	if (GetKeyState(VK_PRIOR) & 0x8000 && opt_Aura.Selected > 0) {

		IActor* MyTa = MPlayer;
		try {
			for (__int8 i = 0; i < EnmTeamV.size(); i++)
			{

				//int en = 0;
				//if (selectedpvp == -1) {
				//	if (MyTeamV.size() > 0)
				//		en = rand() % MyTeamV.size();

				//	if (MyTa = pFrameWork->GetIActorSystem()->GetActor(MyTeamV[en]->GetID()))
				//		if (MyTa->isDeadByH())
				//			en = Find_AliveID(MyTeamV, pFrameWork);
				//	if (en == -1)
				//		break;
				//}
				//else { 
				//	en = selectedpvp;
				//    MyTa = pFrameWork->GetIActorSystem()->GetActor(MyTeamV[en]->GetID());
				//}

				const char* entname;
				if (entname = EnmTeamV[i]->get_entity()->GetName()) {
					if (strstr(entname, XorString("Turret")) ||
						strstr(entname, XorString("IronMan")) ||
						strstr(entname, XorString("Cyborg")) ||
						strstr(entname, XorString("SEDSoldier")) ||
						strstr(entname, XorString("Mantis")) ||
						strstr(entname, XorString("mech")) ||
						strstr(entname, XorString("Drone")) ||
						strstr(entname, XorString("Flagsh")))
						continue;
				}
				if (!MyTa->isDeadByH() && !EnmTeamV[i]->isDeadByH())
					if (int id = EnmTeamV[i]->get_entity()->GetCharacter(0)->GetISkeletonPose()->GetJointIDByNameParams(XorString("Bip01 Head")))
						if (Distance(pren->GetViewCamera(), GetPlayerPos(EnmTeamV[i]->get_entity())) <= 3.0f)
							for (__int8 j = 0; j < 10; j++)
								if (!MyTa->isDeadByH() && !EnmTeamV[i]->isDeadByH())
									KillAura(MyTa, EnmTeamV[i]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(EnmTeamV[i]->get_entity(), id), pFrameWork, pSSGE, XorString("melee"));
								else;
						else
							KillAura(MyTa, EnmTeamV[i]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(EnmTeamV[i]->get_entity(), id), pFrameWork, pSSGE);

				
					
			}
		}
		catch (const std::out_of_range & oor) {};
	}

	if ( GetAsyncKeyState(VK_HOME) && opt_Aura.Selected == 1) {

		IActor* MyTa = MPlayer;
		try {
			for (__int8 i = 0; i < EnmTeamV1.size(); i++)
			{
				IActor* AAA = NULL;
				if (!(AAA = pFrameWork->GetIActorSystem()->GetActor(EnmTeamV1[i]->GetID())))
					continue;
				int en = 0;
				
				//if (selectedpvp == -1) {
				//	if (MyTeamV.size() > 0)
				//		en = rand() % MyTeamV.size();

				//	if (MyTa = pFrameWork->GetIActorSystem()->GetActor(MyTeamV[en]->GetID()))
				//		if (MyTa->isDeadByH())
				//			en = Find_AliveID(MyTeamV, pFrameWork);
				//	if (en == -1)
				//		break;
				//}
				//else {
				//	en = selectedpvp;
				//	MyTa = pFrameWork->GetIActorSystem()->GetActor(MyTeamV[en]->GetID());
				//}
				
				const char* entname;
				const char* mat = XorString("mat_head");
				//if(pFrameWork->GetClientActor(&MyTa))
				if (entname = EnmTeamV1[i]->GetName()) 
				{
		            if (strstr(entname, XorString("Turret")))
					{
						mat = XorString("mat_hitpoint3");
						if (strstr(entname, XorString("TurretWall")))
						{
							mat = XorString("mat_hitpoint1");
						}
						else if (strstr(entname, XorString("TurretAPC")))
							mat = XorString("mat_metal");
						else;
					}
					else if (strstr(entname, XorString("IronMan")))
						mat = XorString("weaponPos_law");
					else if (strstr(entname, XorString("Drone"))) 
						mat = XorString("mat_metal");
					else if (strstr(entname, XorString("Flagsh")))
						goto j2;
					else if (strstr(entname, XorString("Drone_MG")))
						mat = XorString("mat_hitpoint1");
					else if (strstr(entname, "Helicopter")) {
						mat = XorString("weapon_bone");;
					}
					else
					continue;

					if (!MyTa->isDeadByH() && GetAsyncKeyState(VK_HOME)) {
						KillAura1(MyTa, EnmTeamV1[i], (DWORD)rand(), 0, GetPlayerPos(EnmTeamV1[i]), pFrameWork, mat, pSSGE, XorString("bullet"));
						goto j1;
					}

					j2:
					if (!MyTa->isDeadByH() && GetAsyncKeyState(VK_HOME)) {
						KillAura1(MyTa, EnmTeamV1[i], (DWORD)rand(), 0, GetPlayerPos(EnmTeamV1[i]), pFrameWork, mat, pSSGE, XorString("bullet"),738);
					}

					j1:;
				}
			
			}
		}
		catch (const std::out_of_range & oor) {};
	}
	if (autoAura == true && opt_Aura.Selected == 1) {
		
		try {
			for (__int8 i = 0; i < EnmTeamV.size(); i++)
			{
				autoAura = false;

				const char* entname;
				if (entname = EnmTeamV[i]->get_entity()->GetName()) {
					if (strstr(entname, XorString("Turret")) ||
						strstr(entname, XorString("Cyborg")) ||
						strstr(entname, XorString("Mantis")) ||
						strstr(entname, XorString("mech")) ||
						strstr(entname, XorString("Drone")) ||
						strstr(entname, XorString("Flagsh")))
						continue;
				}
				if (!MPlayer->isDeadByH() && !EnmTeamV[i]->isDeadByH()  && p != 11)
					if (int id = EnmTeamV[i]->get_entity()->GetCharacter(0)->GetISkeletonPose()->GetJointIDByNameParams(XorString("Bip01 Head")))
					{
						if (Distance(pren->GetViewCamera(), GetPlayerPos(EnmTeamV[i]->get_entity())) <= 3.0f)
							for (size_t j = 0; j < 10; j++)
								if (!MPlayer->isDeadByH() && !EnmTeamV[i]->isDeadByH())
									KillAura(MPlayer, EnmTeamV[i]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(EnmTeamV[i]->get_entity(), id), pFrameWork, pSSGE, XorString("melee"));
								else;
						else 
							KillAura(MPlayer, EnmTeamV[i]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(EnmTeamV[i]->get_entity(), id), pFrameWork, pSSGE);
						continue;
					}
				for (__int8 j = 0; j < 3; j++)
				{
					if (!MPlayer->isDeadByH() && !EnmTeamV[i]->isDeadByH() && p == 11)
						if (int id = EnmTeamV[i]->get_entity()->GetCharacter(0)->GetISkeletonPose()->GetJointIDByNameParams(XorString("Bip01 Nick")))
							KillAura1(MPlayer, EnmTeamV[i]->get_entity(), (DWORD)rand(), id, GetBonePositionByID(EnmTeamV[i]->get_entity(), id), pFrameWork, XorString("mat_torso"), pSSGE);
				}
			}
		}
		catch (const std::out_of_range & oor) {};
	}

};