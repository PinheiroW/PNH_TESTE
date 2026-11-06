






const bool AZ_VAMP_RIG = false;
const bool AZ_VAMP_PLAGUE_TEST = false;


const bool AZ_VAMP_EVENT_TEST = false;



const bool AZ_VAMP_HALLOWEEN_TEST = false;

modded class MissionServer
{
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		if (!AZ_VAMP_RIG)
			return super.CreateCharacter(identity, pos, ctx, characterName);
		if (AZ_VAMP_EVENT_TEST)
			return super.CreateCharacter(identity, pos, ctx, "SurvivorM_Mirek");
		return super.CreateCharacter(identity, pos, ctx, "SurvivorF_Eva");
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if (AZ_VAMP_HALLOWEEN_TEST)
		{
			HwStep(player, 0);
			return;
		}
		if (!AZ_VAMP_RIG)
			return;
		if (AZ_VAMP_EVENT_TEST)
		{
			AZ_VampState.SetVampire(player, false);
			AZ_VampBind.Unbind(player);
			EntityAI old = player.FindAttachmentBySlotName("Mask");
			if (old)
				GetGame().ObjectDelete(old);
			AZ_VampState.s_Data.eventActive = false;
			int y, mo, d, h, mi;
			GetGame().GetWorld().GetDate(y, mo, d, h, mi);
			GetGame().GetWorld().SetDate(y, mo, d, 12, 0);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampEventDummy, 15000, false, player);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampEventCheck, 45000, false, player);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampEventCheck, 90000, false, player);
			return;
		}
		if (AZ_VAMP_PLAGUE_TEST)
		{

			AZ_VampState.SetVampire(player, false);
			AZ_VampBind.Unbind(player);
			EntityAI worn = player.FindAttachmentBySlotName("Mask");
			if (worn)
				GetGame().ObjectDelete(worn);
			AZ_VampState.SetSickLeft(player, 0);
			AZ_VampState.Cfg().sicknessMin = 2;
			AZ_VampState.s_Data.eventActive = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampPlagueBite, 30000, false, player);
			return;
		}
		if (player && !player.FindAttachmentBySlotName("Mask"))
			player.GetInventory().CreateAttachment("AZ_VampireMask");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampSwapFed, 75000, false, player);
	}



	void HwShot(PlayerBase p, string label, vector cam, vector target)
	{
		GetGame().RPCSingleParam(p, AZ_VampShotRpc.RPC, new Param3<string, vector, vector>(label, cam, target), true, p.GetIdentity());
		Print("[AZ_Vamp] TEST hw shot " + label + " cam " + cam + " target " + target);
	}

	void HwGo(PlayerBase p, vector at, vector look)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		p.SetPosition(at);
		vector d = look - at;
		d[1] = 0;
		p.SetDirection(d.Normalized());
	}


	void HwMouthShot(PlayerBase p, string label)
	{
		vector mh = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		vector mf = p.GetDirection();
		mf[1] = 0;
		mf.Normalize();
		vector mr = mf * vector.Up;
		vector mouth = mh + mf * 0.10;
		HwShot(p, label, mouth + mf * 0.42 + mr * 0.12 - "0 0.03 0", mouth);
	}

	static ref array<PlayerBase> s_HwEvActors;
	static PlayerBase s_HwEvVictim;
	static int s_HwEvFedIdx;

	PlayerBase HwEvFedSubject(PlayerBase p)
	{
		int n = 0;
		if (s_HwEvActors)
			n = s_HwEvActors.Count();
		if (s_HwEvFedIdx < n)
			return s_HwEvActors[s_HwEvFedIdx];
		if (s_HwEvFedIdx == n)
			return p;
		return null;
	}


	void HwEvidenceLook(PlayerBase tester, PlayerBase subj, string label)
	{
		vector mh = subj.GetBonePositionWS(subj.GetBoneIndexByName("Head"));
		vector mf = subj.GetDirection();
		mf[1] = 0;
		mf.Normalize();
		vector mr = mf * vector.Up;
		vector mouth = mh + mf * 0.10;
		EntityAI mk = subj.FindAttachmentBySlotName("Mask");
		string mtype = "none";
		if (mk)
			mtype = mk.GetType();
		Print("[AZ_Vamp] TEST ev look " + label + " subject " + subj.GetType() + " mask " + mtype + " fed left " + (subj.m_AZVampFedUntil - GetGame().GetTime()) + " ms");
		HwShot(tester, label, mouth + mf * 0.42 + mr * 0.12 - "0 0.03 0", mouth);
	}


	void HwEvidenceSpawn(PlayerBase p)
	{
		s_HwEvActors = new array<PlayerBase>;
		vector f = p.GetDirection();
		f[1] = 0;
		f.Normalize();
		vector r = f * vector.Up;
		array<string> types = {"SurvivorF_Eva", "SurvivorF_Frida", "SurvivorM_Mirek", "SurvivorM_Boris"};
		for (int i = 0; i < types.Count(); i++)
		{
			vector at = p.GetPosition() + r * (2.5 + 1.6 * i);
			at[1] = GetGame().SurfaceY(at[0], at[2]);
			PlayerBase a = PlayerBase.Cast(GetGame().CreateObject(types[i], at, false, true));
			if (!a)
			{
				Print("[AZ_Vamp] TEST ev spawn FAILED " + types[i]);
				continue;
			}
			a.SetDirection(f);
			AZ_VampState.SetVampire(a, true);
			AZ_VampBind.Bind(a);
			HwElderDress(a);
			a.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
			s_HwEvActors.Insert(a);
		}
		Print("[AZ_Vamp] TEST ev spawned " + s_HwEvActors.Count() + " AI vampires");
	}



	void HwEvidencePress(PlayerBase tester, PlayerBase subj, string label, string ang)
	{
		vector mh = subj.GetBonePositionWS(subj.GetBoneIndexByName("Head"));
		vector mf = subj.GetDirection();
		mf[1] = 0;
		mf.Normalize();
		vector mr = mf * vector.Up;
		vector mouth = mh + mf * 0.10;
		vector cam = mouth + mf * 0.42 + mr * 0.12 - "0 0.03 0";
		if (ang == "side")
			cam = mouth + mr * 0.40 + mf * 0.10;
		if (ang == "below")
			cam = mouth + mf * 0.30 - "0 0.22 0";
		EntityAI mk = subj.FindAttachmentBySlotName("Mask");
		string mtype = "none";
		if (mk)
			mtype = mk.GetType();
		Print("[AZ_Vamp] TEST ev press " + label + " subject " + subj.GetType() + " mask before " + mtype);
		HwShot(tester, label, cam, mouth);
		if (subj == tester)
		{
			tester.m_AZVampBareUntil = 0;
			GetGame().RPCSingleParam(tester, AZ_VampToggle.RPC_FANGS_TEST, new Param1<bool>(true), true, tester.GetIdentity());
		}
		else
		{
			AZ_VampReveal.BareAtWill(subj, 20);
			subj.m_AZVampDummyReq = AZ_VampFeedAnim.ANIM_HISS;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwEvidenceMask, 900, false, subj, label);
	}

	void HwEvidenceMask(PlayerBase subj, string label)
	{
		if (!subj)
			return;
		EntityAI mk = subj.FindAttachmentBySlotName("Mask");
		string mtype = "none";
		if (mk)
			mtype = mk.GetType();
		Print("[AZ_Vamp] TEST ev " + label + " mask at 0.9 s " + mtype + " bare left " + (subj.m_AZVampBareUntil - GetGame().GetTime()) + " ms");
	}

	void HwFangsAfter(PlayerBase p)
	{
		if (p)
			Print("[AZ_Vamp] TEST hw fangs after " + p.FindAttachmentBySlotName("Mask") + " bare left " + (p.m_AZVampBareUntil - GetGame().GetTime()) + " ms");
	}



	void HwElderDress(PlayerBase p)
	{
		EntityAI body = p.FindAttachmentBySlotName("Body");

		string want = "AZ_VampElder_GreatCoat";
		if (p.GetType() == "SurvivorF_Eva")
			want = "AZ_VampElder_VelvetGown";
		else if (p.IsMale() == false)
			want = "AZ_VampElder_GraveGown";
		if (body && body.GetType() == want)
			return;
		if (body)
			GetGame().ObjectDelete(body);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwElderWear, 300, false, p, want);
	}

	void HwElderWear(PlayerBase p, string want)
	{
		if (p && !p.GetInventory().CreateAttachment(want))
			Print("[AZ_Vamp] TEST hw elder dress: no Body slot for " + want + " on " + p.GetType());
	}

	static PlayerBase s_HwBiteVamp;
	static PlayerBase s_HwBiteVictim;
	void HwBiteTidy()
	{
		if (s_HwBiteVamp && s_HwBiteVictim)
			AZ_VampFeed.Release(s_HwBiteVamp, s_HwBiteVictim);
		if (s_HwBiteVamp)
			GetGame().ObjectDelete(s_HwBiteVamp);
		if (s_HwBiteVictim)
			GetGame().ObjectDelete(s_HwBiteVictim);
		s_HwBiteVamp = null;
		s_HwBiteVictim = null;
	}


	static PlayerBase s_HwDummy;
	static PlayerBase s_HwHuman;
	void HwMaleDummy(PlayerBase p)
	{
		vector side = p.GetDirection() * vector.Up;
		s_HwDummy = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", p.GetPosition() + side * 0.7, false, true));
		if (!s_HwDummy)
		{
			Print("[AZ_Vamp] TEST hw male dummy FAILED to spawn");
			return;
		}
		s_HwDummy.SetDirection(p.GetDirection());
		AZ_VampState.SetVampire(s_HwDummy, true);
		AZ_VampBind.Bind(s_HwDummy);
		s_HwDummy.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
		Print("[AZ_Vamp] TEST hw male dummy " + s_HwDummy);
	}


	void HwSunSpot(PlayerBase p)
	{
		vector c = p.GetPosition();
		for (int r = 0; r <= 400; r += 25)
		{
			for (int k = 0; k < 8; k++)
			{
				float a = k * Math.PI / 4;
				vector at = c + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
				at[1] = GetGame().SurfaceY(at[0], at[2]);
				if (GetGame().SurfaceIsSea(at[0], at[2]) || GetGame().SurfaceIsPond(at[0], at[2]))
					continue;
				vector head = at + "0 1.6 0";
				if (AZ_VampSun.ExposedFrom(head, head - GetGame().GetWorld().GetSunOrMoonDirection() * AZ_VampSun.LENGTH))
				{
					p.SetPosition(at);
					Print("[AZ_Vamp] TEST hw sun spot " + at + " (" + r + " m out)");
					return;
				}
			}
		}
		p.SetPosition(c);
		Print("[AZ_Vamp] TEST hw sun spot NONE within 400 m");
	}


	void HwClearInfected(PlayerBase p)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(p.GetPosition(), 80, near, null);
		foreach (Object o : near)
		{
			if (o.IsInherited(DayZInfected))
				GetGame().ObjectDelete(o);
		}
	}



	vector HwClearCam(vector target, float back, float up)
	{
		vector aim = target + "0 0.6 0";
		vector best = vector.Zero;
		for (int k = 0; k < 12; k++)
		{
			float a = k * Math.PI / 6;
			vector c = target + Vector(Math.Cos(a) * back, 0, Math.Sin(a) * back);
			c[1] = GetGame().SurfaceY(c[0], c[2]) + up;
			if (best == vector.Zero)
				best = c;
			vector hit, nrm;
			int comp;
			set<Object> res = new set<Object>;
			if (!DayZPhysics.RaycastRV(c, aim, hit, nrm, comp, res, null, null, false, false, ObjIntersectFire))
				return c;
			if (vector.Distance(hit, aim) < 2.0)
				return c;
		}
		Print("[AZ_Vamp] TEST no clear camera for " + target);
		return best;
	}


	vector HwCam(vector thing, vector from, float back, float up)
	{
		vector d = from - thing;
		d[1] = 0;
		d.Normalize();
		vector c = thing + d * back;
		c[1] = GetGame().SurfaceY(c[0], c[2]) + up;
		return c;
	}

	static const bool HW_MOON_RUN = true;



	static ref array<string> s_HwOnly;

	void HwLoadOnly()
	{
		s_HwOnly = null;
		FileHandle f = OpenFile("$profile:shot_only.txt", FileMode.READ);
		if (!f)
			return;
		s_HwOnly = new array<string>;
		string line;
		while (FGets(f, line) >= 0)
		{
			line = line.Trim();
			if (line != "")
				s_HwOnly.Insert(line);
		}
		CloseFile(f);
		Print("[AZ_Vamp] TEST hw jump list " + s_HwOnly.Count() + " shots");
	}


	string HwStepLabel(int step)
	{
		if (step == 2)
			return "hw_self_front";
		if (step == 3)
			return "hw_self_eyes";
		if (step == 4 || step == 5)
			return "hw_pumpkin_doorstep";
		if (step == 6 || step == 7)
			return "hw_campfire";
		if (step == 8 || step == 9)
			return "hw_graveyard_mist";
		if (step == 20 || step == 21)
			return "hw_bloodmoon_campfire";
		if (step == 22)
			return "hw_bloodmoon_self";
		if (step == 10 || step == 11)
			return "hw_sun_smoke|hw_sun_fire";
		if (step == 12)
			return "hw_sun_smoke";
		if (step == 13 || step == 14)
			return "hw_sun_fire";
		if (step == 30 || step == 31)
			return "hw_heartbeat";
		if (step >= 32 && step <= 34)
			return "hw_fangs_key";

		if (step >= 420 && step <= 422)
			return "hw_fangs_moon";
		if (step >= 423 && step <= 425)
			return "hw_bite_face";
		if (step >= 430 && step <= 470)
			return "hw_fangs_evidence";
		if (step == 35 || step == 36)
			return "hw_bell";
		if (step == 37 || step == 38)
			return "hw_shot_resist";
		if (step == 40)
			return "sky_n|sky_e|sky_s|sky_w|sky_zenith";
		if (step >= 41 && step <= 45)
			return HwSkyLabel(step - 41);
		if (step == 46)
			return "bloodmoon_orb|bloodmoon_orb_ground";
		if (step == 47)
			return "bloodmoon_orb";
		if (step == 48)
			return "bloodmoon_orb_ground";
		if (step == 600)
			return "a1orb_rim_night";
		if (step == 601)
			return "a1orb_rim_night";
		if (step == 602)
			return "a1orb_rim_blood|a1orb_close|a1orb_burst";
		if (step == 603)
			return "a1orb_rim_blood";
		if (step == 604)
			return "a1orb_close";
		if (step == 605)
			return "a1orb_burst";
		if (step >= 50 && step <= 52)
			return "props_tour";
		if (step >= 60 && step <= 62)
			return "hole_test";
		if (step == 63)
			return "pumpkin_zed";
		if (step == 64)
			return "costume_coat|costume_coat_back|costume_sick|costume_beauty|costume_ground";
		if (step >= 65 && step <= 69)
			return HwCostumeLabel(step - 65);
		if (step == 70)
			return "gcoat_front|gcoat_back|gcoat_close|gcoat_crouch|gcoat_prone|gcoat_female|gcoat_walk|gcoat_ground";
		if (step >= 71 && step <= 78)
			return HwGcoatLabel(step - 71);
		if (step == 79)
			return "gown_velvet_front|gown_velvet_close|gown_velvet_back|gown_grave_front|gown_grave_back|gown_crouch|gown_walk|gown_male|gown_ground";
		if (step >= 80 && step <= 88)
			return HwGownLabel(step - 80);
		return "";
	}



	string HwSkyLabel(int i)
	{
		string labels[5] = {"sky_n", "sky_e", "sky_s", "sky_w", "sky_zenith"};
		return labels[i];
	}


	static ref array<Object> s_HwCostumeDummies;
	static vector s_HwCostumeAt;
	string HwCostumeLabel(int i)
	{
		string labels[5] = {"costume_coat", "costume_coat_back", "costume_sick", "costume_beauty", "costume_ground"};
		return labels[i];
	}


	static ref array<PlayerBase> s_HwGcoat;
	static vector s_HwGcoatAt;
	string HwGcoatLabel(int i)
	{
		string labels[8] = {"gcoat_front", "gcoat_back", "gcoat_close", "gcoat_crouch", "gcoat_prone", "gcoat_female", "gcoat_walk", "gcoat_ground"};
		return labels[i];
	}



	static ref array<PlayerBase> s_HwGown;
	static vector s_HwGownAt;
	string HwGownLabel(int i)
	{
		string labels[9] = {"gown_velvet_front", "gown_velvet_close", "gown_velvet_back", "gown_grave_front", "gown_grave_back", "gown_crouch", "gown_walk", "gown_male", "gown_ground"};
		return labels[i];
	}

	void HwGownWalkShot(PlayerBase p, PlayerBase w, vector start)
	{
		if (!w)
			return;
		vector wp = w.GetPosition();
		Print("[AZ_Vamp] TEST gown walker moved " + vector.Distance(wp, start) + " m");
		HwShot(p, "gown_walk", wp + Vector(2.6, 1.1, -1.2), wp + Vector(0, 0.9, 0));
	}


	void HwGcoatStance(PlayerBase d, int stance)
	{
		if (!d)
			return;
		HumanCommandMove hcm = d.GetCommand_Move();
		if (hcm)
			hcm.ForceStance(stance);
		Print("[AZ_Vamp] TEST gcoat stance " + stance + " on " + d + " move " + hcm);
	}

	void HwGcoatWalkShot(PlayerBase p, PlayerBase w, vector start)
	{
		if (!w)
			return;
		vector wp = w.GetPosition();
		Print("[AZ_Vamp] TEST gcoat walker moved " + vector.Distance(wp, start) + " m");
		HwShot(p, "gcoat_walk", wp + Vector(0.4, 1.1, -3.2), wp + Vector(0, 0.9, 0));
	}

	float HwSkyAz(int i)
	{
		if (i == 4)
			return 0;
		return i * 90;
	}

	float HwSkyPitch(int i)
	{
		if (i == 4)
			return 89;
		return 30;
	}

	vector HwSkyDir(int i)
	{
		float az = HwSkyAz(i) * Math.DEG2RAD;
		float pitch = HwSkyPitch(i) * Math.DEG2RAD;
		return Vector(Math.Sin(az) * Math.Cos(pitch), Math.Sin(pitch), Math.Cos(az) * Math.Cos(pitch));
	}

	int HwNextStep(int step)
	{
		if (step == 14 && s_HwOnly)
			return 40;
		if (step == 45)
			return 46;
		if (step == 48)
			return 600;
		if (step == 605)
			return 50;
		if (step == 52)
			return 60;
		if (step == 62)
			return 63;
		if (step == 63)
		{
			if (s_HwOnly)
				return 64;
			return 15;
		}
		if (step == 69)
			return 70;
		if (step == 78)
			return 79;
		if (step == 88)
			return 15;
		if (step == 9 && HW_MOON_RUN)
			return 20;
		if (step == 34)
			return 420;
		if (step == 425)
			return 430;
		if (step == 459)
			return 35;
		if (step == 22)
			return 30;
		if (step == 38)
		{
			if (s_HwOnly)
				return 10;
			return 15;
		}
		return step + 1;
	}



	static bool s_HwGuard;
	void HwKeepAlive(PlayerBase p)
	{
		if (!s_HwGuard || !p || !p.IsAlive())
			return;
		if (p.GetHealth("", "Health") < 15)
			p.SetHealth("", "Health", 15);
		p.SetHealth("", "Shock", p.GetMaxHealth("", "Shock"));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwKeepAlive, 500, false, p);
	}


	bool HwWantsHunger()
	{
		if (!s_HwOnly)
			return true;
		return s_HwOnly.Find("hw_self_front") >= 0 || s_HwOnly.Find("hw_self_eyes") >= 0 || s_HwOnly.Find("hw_bloodmoon_self") >= 0 || s_HwOnly.Find("hw_heartbeat") >= 0;
	}

	bool HwWanted(int step)
	{
		string want = HwStepLabel(step);
		if (!s_HwOnly || want == "")
			return true;
		array<string> opts = new array<string>;
		want.Split("|", opts);
		foreach (string w : opts)
		{
			if (s_HwOnly.Find(w) >= 0)
				return true;
		}
		return false;
	}


	static ref array<string> s_HwPvLabels;
	static ref array<vector> s_HwPvTargets;
	static ref array<float> s_HwPvBack;
	static ref array<float> s_HwPvUp;
	static int s_HwPv;
	static Object s_HwRoom;

	void HwPropsList()
	{
		s_HwPvLabels = new array<string>;
		s_HwPvTargets = new array<vector>;
		s_HwPvBack = new array<float>;
		s_HwPvUp = new array<float>;
		if (AZ_VampProps.s_Scenes)
		{
			int graves = 0;
			int vork = 0;
			for (int i = 0; i < AZ_VampProps.s_Scenes.Count() && s_HwPvLabels.Count() < 10; i++)
			{
				string nm = AZ_VampProps.s_SceneNames[i];
				if (nm.IndexOf("graveyard") == 0)
				{
					graves++;
					if (graves > 2)
						continue;
				}
				if (nm.Contains("Vorkuta"))
				{
					vork++;
					if (vork > 2)
						continue;
				}
				nm.Replace(" ", "_");
				Print("[AZ_Vamp] TEST props tour pv_" + s_HwPvLabels.Count() + " = " + nm + " at " + AZ_VampProps.s_Scenes[i]);
				s_HwPvLabels.Insert("pv_" + s_HwPvLabels.Count());
				s_HwPvTargets.Insert(AZ_VampProps.s_Scenes[i]);
				s_HwPvBack.Insert(5.5);
				s_HwPvUp.Insert(1.7);
			}
		}
		if (AZ_VampLandmarks.s_Fires)
		{
			for (int f = 0; f < AZ_VampLandmarks.s_Fires.Count() && f < 1; f++)
			{
				if (!AZ_VampLandmarks.s_Fires[f])
					continue;
				Print("[AZ_Vamp] TEST props tour town_" + f + " = " + AZ_VampLandmarks.s_FireNames[f]);
				s_HwPvLabels.Insert("town_" + f);
				s_HwPvTargets.Insert(AZ_VampLandmarks.s_Fires[f].GetPosition());
				s_HwPvBack.Insert(150);
				s_HwPvUp.Insert(12);
				s_HwPvLabels.Insert("town_" + f + "_off");
				s_HwPvTargets.Insert(AZ_VampLandmarks.s_Fires[f].GetPosition());
				s_HwPvBack.Insert(150);
				s_HwPvUp.Insert(12);
			}
		}


		if (AZ_VampLandmarks.s_Fires && AZ_VampDecor.s_Lanterns)
		{
			int vh = 0;
			for (int vf = 0; vf < AZ_VampLandmarks.s_Fires.Count() && vh < 40; vf++)
			{
				Fireplace vfire = AZ_VampLandmarks.s_Fires[vf];
				if (!vfire)
					continue;
				vector vc = vfire.GetPosition();
				array<vector> picked = new array<vector>;
				foreach (Object vl : AZ_VampDecor.s_Lanterns)
				{
					if (!vl || picked.Count() >= 2)
						continue;
					vector vp = vl.GetPosition();
					float vd = vector.Distance(vp, vc);
					if (vd < 15 || vd > 220)
						continue;
					bool vnear = false;
					foreach (vector pk : picked)
					{
						if (vector.Distance(pk, vp) < 40)
							vnear = true;
					}
					if (vnear)
						continue;
					picked.Insert(vp);
					Print("[AZ_Vamp] TEST props tour vh_" + vh + " = " + AZ_VampLandmarks.s_FireNames[vf] + " at " + vp);
					s_HwPvLabels.Insert("vh_" + vh);
					s_HwPvTargets.Insert(vp);
					s_HwPvBack.Insert(7);
					s_HwPvUp.Insert(2.2);
					vh++;
				}
			}
		}
		Print("[AZ_Vamp] TEST props tour " + s_HwPvLabels.Count() + " shots, " + AZ_VampDecor.Report(vector.Zero));
	}

	void HwStep(PlayerBase p, int step)
	{
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		int next = 10000;
		int nextStep = step + 1;
		vector here = p.GetPosition();
		HwClearInfected(p);
		switch (step)
		{
			case 0:
				HwLoadOnly();
				s_HwGuard = true;
				HwKeepAlive(p);
				next = 35000;
				break;
			case 1:
				AZ_VampState.s_Data.eventActive = true;
				AZ_VampEvent.MakeVampire(p);


				if (HwWantsHunger())
					p.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
				else
					p.SetHealth("", "Blood", p.GetMaxHealth("", "Blood"));
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				Print("[AZ_Vamp] TEST hw start " + p.GetType() + " lanterns " + AZ_VampDecor.Report(here));
				HwMaleDummy(p);
				next = 15000;
				break;
			case 2:
				HwShot(p, "hw_self_front", "0 0 0", "0 0 0");
				next = 8000;
				break;
			case 3:
				HwShot(p, "hw_self_eyes", "0 0 0", "1 0 0");
				break;
			case 4:
				if (AZ_VampDecor.s_Lanterns && AZ_VampDecor.s_Lanterns.Count() > 0 && AZ_VampDecor.s_Lanterns[0])
				{
					vector l = AZ_VampDecor.s_Lanterns[0].GetPosition();
					HwGo(p, l + "6 0 6", l);
				}
				next = 12000;
				break;
			case 5:
				if (AZ_VampDecor.s_Lanterns && AZ_VampDecor.s_Lanterns.Count() > 0 && AZ_VampDecor.s_Lanterns[0])
				{
					vector lp = AZ_VampDecor.s_Lanterns[0].GetPosition();
					HwShot(p, "hw_pumpkin_doorstep", HwCam(lp, here, 2.5, 1.3), lp + "0 0.6 0");
				}
				break;
			case 6:
				if (AZ_VampLandmarks.s_Fires && AZ_VampLandmarks.s_Fires.Count() > 0 && AZ_VampLandmarks.s_Fires[0])
				{
					vector f = AZ_VampLandmarks.s_Fires[0].GetPosition();
					HwGo(p, f + "7 0 7", f);
				}
				next = 12000;
				break;
			case 7:
				if (AZ_VampLandmarks.s_Fires && AZ_VampLandmarks.s_Fires.Count() > 0 && AZ_VampLandmarks.s_Fires[0])
				{
					vector fp = AZ_VampLandmarks.s_Fires[0].GetPosition();
					HwShot(p, "hw_campfire", HwCam(fp, here, 6, 2.0), fp + "0 0.4 0");
				}
				break;
			case 8:
				if (AZ_VampLandmarks.s_Graveyards && AZ_VampLandmarks.s_Graveyards.Count() > 0)
				{
					vector g = AZ_VampLandmarks.s_Graveyards[0];
					HwGo(p, g + "12 0 12", g);
				}
				next = 12000;
				break;
			case 9:
				if (AZ_VampLandmarks.s_Graveyards && AZ_VampLandmarks.s_Graveyards.Count() > 0)
				{
					vector gp = AZ_VampLandmarks.s_Graveyards[0];
					HwShot(p, "hw_graveyard_mist", HwCam(gp, here, 16, 3.0), gp + "0 1 0");
				}
				nextStep = HwNextStep(step);
				break;
			case 20:
				AZ_VampProgress.s_BloodMoon = true;
				if (AZ_VampLandmarks.s_Fires && AZ_VampLandmarks.s_Fires.Count() > 0 && AZ_VampLandmarks.s_Fires[0])
				{
					vector mf = AZ_VampLandmarks.s_Fires[0].GetPosition();
					HwGo(p, mf + "7 0 7", mf);
				}
				next = 14000;
				break;
			case 21:
				if (AZ_VampLandmarks.s_Fires && AZ_VampLandmarks.s_Fires.Count() > 0 && AZ_VampLandmarks.s_Fires[0])
				{
					vector mfp = AZ_VampLandmarks.s_Fires[0].GetPosition();
					Print("[AZ_Vamp] TEST hw bloodmoon synced " + p.m_AZBloodMoon);
					HwShot(p, "hw_bloodmoon_campfire", HwCam(mfp, here, 6, 2.0), mfp + "0 0.4 0");
				}
				break;
			case 22:
				HwShot(p, "hw_bloodmoon_self", "0 0 0", "0 0 0");
				AZ_VampProgress.s_BloodMoon = false;
				nextStep = HwNextStep(step);
				break;

			case 30:
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
				if (!s_HwHuman)
					s_HwHuman = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Linda", p.GetPosition() + p.GetDirection() * 20, false, true));
				else
					s_HwHuman.SetPosition(p.GetPosition() + p.GetDirection() * 20);
				Print("[AZ_Vamp] TEST hw heartbeat human " + s_HwHuman + " vampire hunger " + p.m_AZVampHunger);
				next = 16000;
				break;
			case 31:
				Print("[AZ_Vamp] TEST hw heartbeat human at " + Math.Round(vector.Distance(s_HwHuman.GetPosition(), p.GetPosition())) + " m, hunger " + p.m_AZVampHunger);
				HwShot(p, "hw_heartbeat", "0 0 0", "0 0 0");
				break;
			case 32:
				HwElderDress(p);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampBareUntil = 0;
				p.m_AZVampFedUntil = 0;
				AZ_VampReveal.Update(p);
				next = 6000;
				break;
			case 33:
				Print("[AZ_Vamp] TEST hw fangs before " + p.FindAttachmentBySlotName("Mask"));
				HwMouthShot(p, "hw_fangs_key");
				next = 2100;
				break;
			case 34:
				GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_FANGS_TEST, new Param1<bool>(true), true, p.GetIdentity());
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwFangsAfter, 1000, false, p);
				nextStep = 420;
				next = 6000;
				break;




			case 430:
				HwElderDress(p);
				p.SetHealth("", "Blood", AZ_VampState.Cfg().fullBlood);
				p.m_AZVampFedUntil = 0;
				HwEvidenceSpawn(p);
				s_HwEvFedIdx = 0;
				next = 6000;
				break;
			case 431:
			case 432:
			case 433:
			case 434:
			case 435:
			{
				int en = step - 430;
				string ang = "front";
				if (en == 3)
					ang = "side";
				if (en == 4)
					ang = "below";
				HwEvidencePress(p, p, "ev_night" + en + "_" + ang, ang);
				next = 6500;
				break;
			}
			case 436:
				AZ_VampProgress.s_BloodMoon = true;
				p.m_AZVampBareUntil = 0;
				AZ_VampReveal.Update(p);
				next = 9000;
				break;
			case 437:
			case 438:
			case 439:
			{
				int em = step - 436;
				string angm = "front";
				if (em == 2)
					angm = "side";
				HwEvidencePress(p, p, "ev_moon" + em + "_" + angm, angm);
				next = 6500;
				break;
			}
			case 440:
				AZ_VampProgress.s_BloodMoon = false;
				next = 3000;
				break;
			case 441:
			case 442:
			case 443:
			case 444:
			{
				int ea = step - 441;
				if (s_HwEvActors && ea < s_HwEvActors.Count() && s_HwEvActors[ea])
				{
					string aang = "front";
					if (ea == 1)
						aang = "side";
					HwEvidencePress(p, s_HwEvActors[ea], "ev_ai" + ea + "_" + s_HwEvActors[ea].GetType(), aang);
				}
				next = 6500;
				break;
			}




			case 445:
			{
				PlayerBase fs = HwEvFedSubject(p);
				s_HwEvVictim = null;
				if (fs)
				{
					vector fsf = fs.GetDirection();
					fsf[1] = 0;
					fsf.Normalize();
					s_HwEvVictim = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Denis", fs.GetPosition() + fsf * AZ_VampFeed.ROOT_GAP, false, true));
					if (s_HwEvVictim)
					{
						s_HwEvVictim.SetDirection(fsf * -1);
						AZ_VampFeed.Hold(fs, s_HwEvVictim);
					}
				}
				next = 3500;
				break;
			}
			case 446:
			{
				PlayerBase fs2 = HwEvFedSubject(p);
				if (fs2 && s_HwEvVictim)
					AZ_VampFeed.Release(fs2, s_HwEvVictim);
				if (s_HwEvVictim)
					GetGame().ObjectDelete(s_HwEvVictim);
				s_HwEvVictim = null;
				next = 2500;
				break;
			}
			case 447:
			{
				PlayerBase fs3 = HwEvFedSubject(p);
				if (fs3)
					HwEvidenceLook(p, fs3, "ev_fed" + s_HwEvFedIdx + "_closed");
				next = 6000;
				break;
			}
			case 448:
			{
				PlayerBase fs4 = HwEvFedSubject(p);
				if (fs4)
				{
					fs4.m_AZVampFedUntil = Math.Max(fs4.m_AZVampFedUntil, GetGame().GetTime() + 15000);
					HwEvidencePress(p, fs4, "ev_fed" + s_HwEvFedIdx + "_hiss", "front");
				}
				next = 6500;
				break;
			}
			case 449:
				s_HwEvFedIdx++;
				if (HwEvFedSubject(p))
					nextStep = 445;
				else
					nextStep = 453;
				next = 500;
				break;
			case 453:
				if (s_HwEvActors)
				{
					foreach (PlayerBase evd : s_HwEvActors)
					{
						if (evd)
							GetGame().ObjectDelete(evd);
					}
				}
				s_HwEvActors = null;
				nextStep = 454;
				next = 1000;
				break;


			case 454:
				p.m_AZVampBareUntil = 0;
				AZ_VampReveal.BareAtWill(p, 20);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "ev_1pp_bared", vector.Zero, Vector(11, 0, 0));
				next = 7000;
				break;

			case 455:
				p.m_AZVampFedUntil = 0;
				p.m_AZVampBareUntil = 0;
				AZ_VampReveal.Update(p);
				Print("[AZ_Vamp] TEST blood before calm: bloodied " + p.m_AZVampBloodied);
				next = 4000;
				break;
			case 456:
				HwEvidenceLook(p, p, "ev_blood_calm");
				next = 5000;
				break;
			case 457:
				AZ_VampBlood.Set(p, false);
				next = 3500;
				break;
			case 458:
				HwEvidenceLook(p, p, "ev_blood_washed");
				next = 5000;
				break;
			case 459:
				nextStep = 35;
				next = 500;
				break;
			case 420:
				AZ_VampProgress.s_BloodMoon = true;
				p.m_AZVampBareUntil = 0;
				AZ_VampReveal.Update(p);
				next = 9000;
				break;
			case 421:
				Print("[AZ_Vamp] TEST hw fangs moon before " + p.FindAttachmentBySlotName("Mask") + " synced moon " + p.m_AZBloodMoon);
				HwMouthShot(p, "hw_fangs_moon");
				next = 2100;
				break;
			case 422:
				GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_FANGS_TEST, new Param1<bool>(true), true, p.GetIdentity());
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwFangsAfter, 1000, false, p);
				next = 6000;
				break;
			case 423:
			{
				AZ_VampProgress.s_BloodMoon = false;
				vector btF = p.GetDirection();
				btF[1] = 0;
				btF.Normalize();
				vector btSide = btF * vector.Up;
				vector btAt = here + btSide * 3.0;
				btAt[1] = GetGame().SurfaceY(btAt[0], btAt[2]);
				s_HwBiteVamp = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", btAt, false, true));
				s_HwBiteVictim = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Eva", btAt + btF * 0.9, false, true));
				if (s_HwBiteVamp && s_HwBiteVictim)
				{
					s_HwBiteVamp.SetDirection(btF);
					s_HwBiteVictim.SetDirection(btF * -1);
					AZ_VampState.SetVampire(s_HwBiteVamp, true);
					AZ_VampBind.Bind(s_HwBiteVamp);
					HwElderDress(s_HwBiteVamp);
					s_HwBiteVamp.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
					next = 4000;
				}
				else
					Print("[AZ_Vamp] TEST hw bite face: spawn FAILED");
				break;
			}
			case 424:
				if (s_HwBiteVamp && s_HwBiteVictim)
				{
					ActionAZVampBite.Snap(s_HwBiteVamp, s_HwBiteVictim);
					AZ_VampFeed.Hold(s_HwBiteVamp, s_HwBiteVictim);
					AZ_VampFeedAnim.DummyFeedStart(s_HwBiteVamp);
				}
				next = 2600;
				break;
			case 425:
			{
				if (s_HwBiteVamp)
				{
					vector btHead = s_HwBiteVamp.GetBonePositionWS(s_HwBiteVamp.GetBoneIndexByName("Head"));
					if (s_HwBiteVictim)
					{
						vector vnk = s_HwBiteVictim.GetBonePositionWS(s_HwBiteVictim.GetBoneIndexByName("Neck1"));
						vector vhd = s_HwBiteVictim.GetBonePositionWS(s_HwBiteVictim.GetBoneIndexByName("Head"));
						Print("[AZ_Vamp] TEST bite gap vamp Head -> victim Neck1 " + vector.Distance(btHead, vnk) + " m, -> victim Head " + vector.Distance(btHead, vhd) + " m, roots " + vector.Distance(s_HwBiteVamp.GetPosition(), s_HwBiteVictim.GetPosition()) + " m");
					}
					vector btFwd = s_HwBiteVamp.GetDirection();
					btFwd[1] = 0;
					btFwd.Normalize();
					vector btRight = btFwd * vector.Up;

					HwShot(p, "hw_bite_face", btHead - btRight * 0.45 + btFwd * 0.05 + "0 0.03 0", btHead + btFwd * 0.10);
				}
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwBiteTidy, 6000, false);
				nextStep = 430;
				next = 6500;
				break;
			}
			case 35:
				GetGame().GetWorld().SetDate(2011, 11, 5, 23, 59);
				next = 45000;
				break;
			case 36:
				int by, bmo, bd, bh, bmi;
				GetGame().GetWorld().GetDate(by, bmo, bd, bh, bmi);
				Print("[AZ_Vamp] TEST hw bell server clock " + bh + ":" + bmi);
				HwShot(p, "hw_bell", "0 0 0", "0 0 0");
				break;
			case 37:

				float hh0 = s_HwHuman.GetHealth("", "Health");
				s_HwHuman.ProcessDirectDamage(DamageType.FIRE_ARM, p, "Torso", "Bullet_762x39", "0 1.2 0");
				Print("[AZ_Vamp] TEST hw shot HUMAN torso health " + hh0 + " -> " + s_HwHuman.GetHealth("", "Health"));
				float h0 = p.GetHealth("", "Health");
				float s0 = p.GetHealth("", "Shock");
				p.ProcessDirectDamage(DamageType.FIRE_ARM, s_HwHuman, "Torso", "Bullet_762x39", "0 1.2 0");
				Print("[AZ_Vamp] TEST hw shot torso health " + h0 + " -> " + p.GetHealth("", "Health") + " shock " + s0 + " -> " + p.GetHealth("", "Shock") + " night " + GetGame().GetWorld().IsNight());
				p.SetHealth("", "Health", 100);
				p.ProcessDirectDamage(DamageType.FIRE_ARM, s_HwHuman, "LeftLeg", "Bullet_9x19", "0 0.4 0");
				p.ProcessDirectDamage(DamageType.FIRE_ARM, s_HwHuman, "LeftLeg", "Bullet_9x19", "0 0.4 0");
				Print("[AZ_Vamp] TEST hw shot 2x 9mm leg health now " + p.GetHealth("", "Health") + " leg " + p.GetHealth("LeftLeg", "Health"));
				next = 3000;
				break;
			case 38:
				Print("[AZ_Vamp] TEST hw shot leg: broken legs " + p.GetBrokenLegs() + " leg health " + p.GetHealth("LeftLeg", "Health"));
				HwShot(p, "hw_shot_resist", "0 0 0", "0 0 0");
				nextStep = HwNextStep(step);
				break;
			case 10:
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				next = 4000;
				break;
			case 11:
				HwSunSpot(p);
				next = 12000;
				break;
			case 12:
				HwClearInfected(p);
				Print("[AZ_Vamp] TEST hw sun_smoke exposed " + AZ_VampSun.Exposed(p) + " burn " + p.m_AZVampBurn);
				HwShot(p, "hw_sun_smoke", "0 0 0", "9 0 0");
				break;
			case 13:
				HwClearInfected(p);
				p.SetHealth("", "Health", AZ_VampSunAsh_FireHealth() - 5);
				p.SetHealth("", "Shock", p.GetMaxHealth("", "Shock"));
				next = 8000;
				break;
			case 14:
				p.SetHealth("", "Shock", p.GetMaxHealth("", "Shock"));
				Print("[AZ_Vamp] TEST hw sun_fire exposed " + AZ_VampSun.Exposed(p) + " burn " + p.m_AZVampBurn + " health " + p.GetHealth("", "Health") + " uncon " + p.IsUnconscious());
				HwShot(p, "hw_sun_fire", "0 0 0", "9 0 0");
				nextStep = HwNextStep(step);
				break;
			case 40:
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				AZ_VampProgress.s_BloodMoon = false;
				Weather w = GetGame().GetWeather();
				w.MissionWeather(true);
				w.GetOvercast().Set(0, 0, 3600);
				w.GetFog().Set(0, 0, 3600);
				w.GetRain().Set(0, 0, 3600);
				next = 12000;
				break;
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
				int si = step - 41;
				vector skyCam = here + "0 50 0";
				int sy, smo, sd, sh, smi;
				GetGame().GetWorld().GetDate(sy, smo, sd, sh, smi);
				Print("[AZ_Vamp] TEST sky " + HwSkyLabel(si) + " az " + HwSkyAz(si) + " pitch " + HwSkyPitch(si) + " server clock " + sh + ":" + smi + " overcast " + GetGame().GetWeather().GetOvercast().GetActual());
				HwShot(p, HwSkyLabel(si), skyCam, skyCam + HwSkyDir(si) * 100);
				nextStep = HwNextStep(step);
				break;
			case 46:
				GetGame().GetWorld().SetDate(2011, 11, 5, 19, 0);
				AZ_VampProgress.FullMoonDate();
				AZ_VampProgress.s_BloodMoon = true;
				Weather wm = GetGame().GetWeather();
				wm.MissionWeather(true);
				wm.GetOvercast().Set(0, 0, 3600);
				wm.GetFog().Set(0, 0, 3600);
				wm.GetRain().Set(0, 0, 3600);
				HwGo(p, Vector(8785, 0, 9855.4), Vector(8885, 0, 9835));
				next = 16000;
				break;
			case 47:
			case 48:
				vector moonDir = GetGame().GetWorld().GetSunOrMoonDirection();
				vector orbCam = here + Vector(moonDir[0] * 1.5, 2.0, moonDir[2] * 1.5);
				vector orbAim = moonDir;
				if (step == 48)
					orbAim = Vector(moonDir[0], moonDir[1] - 0.21, moonDir[2]).Normalized();
				Print("[AZ_Vamp] TEST bloodmoon orb dir " + moonDir + " elevation " + (Math.Asin(moonDir[1]) * Math.RAD2DEG) + " synced " + p.m_AZBloodMoon + " moon intensity " + GetGame().GetWorld().GetMoonIntensity());
				if (step == 47)
					HwShot(p, "bloodmoon_orb", orbCam, orbCam + orbAim * 100);
				else
					HwShot(p, "bloodmoon_orb_ground", orbCam, orbCam + orbAim * 100);
				if (step == 48 || !HwWanted(48))
					AZ_VampProgress.s_BloodMoon = false;
				nextStep = HwNextStep(step);
				break;
			case 600:
				GetGame().GetWorld().SetDate(2011, 11, 5, 23, 0);
				AZ_VampProgress.s_BloodMoon = false;
				AZ_VampCryptA1.s_TestOpen = true;
				AZ_VampCryptA1.Refresh();
				Weather wa = GetGame().GetWeather();
				wa.MissionWeather(true);
				wa.GetOvercast().Set(0, 0, 3600);
				wa.GetFog().Set(0, 0, 3600);
				wa.GetRain().Set(0, 0, 3600);
				HwGo(p, AZ_VampCryptA1.ORB + Vector(19, 0, -8), AZ_VampCryptA1.ORB);
				next = 12000;
				break;
			case 601:
			case 603:
				vector rimCam = AZ_VampCryptA1.ORB + Vector(17, 0, -7);
				rimCam[1] = GetGame().SurfaceY(rimCam[0], rimCam[2]) + 1.7;
				Print("[AZ_Vamp] TEST a1orb state " + AZ_VampCryptA1.s_OrbState + " blood " + AZ_VampProgress.s_BloodMoon + " synced " + p.m_AZBloodMoon);
				if (step == 601)
					HwShot(p, "a1orb_rim_night", rimCam, AZ_VampCryptA1.ORB - Vector(0, 2.5, 0));
				else
					HwShot(p, "a1orb_rim_blood", rimCam, AZ_VampCryptA1.ORB - Vector(0, 2.5, 0));
				nextStep = HwNextStep(step);
				break;
			case 602:
				AZ_VampProgress.FullMoonDate();
				AZ_VampProgress.s_BloodMoon = true;
				AZ_VampCryptA1.s_TestOpen = true;
				AZ_VampCryptA1.Refresh();
				HwGo(p, AZ_VampCryptA1.ORB + Vector(19, 0, -8), AZ_VampCryptA1.ORB);
				next = 14000;
				break;
			case 604:
				vector closeCam = AZ_VampCryptA1.ORB + Vector(3.5, -0.6, -1.5);
				HwShot(p, "a1orb_close", closeCam, AZ_VampCryptA1.ORB);
				nextStep = HwNextStep(step);
				break;
			case 605:
				vector burstCam = AZ_VampCryptA1.ORB + Vector(11, -1.5, -5);
				vector burstAt = AZ_VampCryptA1.ORB + Vector(2.4, -0.8, -1.0);
				HwShot(p, "a1orb_burst", burstCam, burstAt);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptA1.OrbBurst, 2500, false, burstAt);
				AZ_VampProgress.s_BloodMoon = false;
				AZ_VampCryptA1.s_TestOpen = false;
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptA1.Refresh, 10000, false);
				nextStep = HwNextStep(step);
				next = 12000;
				break;
			case 50:
				AZ_VampEvent.Cure(p);
				if (s_HwDummy)
					GetGame().ObjectDelete(s_HwDummy);
				p.SetHealth("", "Blood", p.GetMaxHealth("", "Blood"));
				p.SetHealth("", "Health", p.GetMaxHealth("", "Health"));
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				HwPropsList();
				s_HwPv = 0;
				next = 8000;
				nextStep = 51;
				if (s_HwPvLabels.Count() == 0)
					nextStep = 60;
				break;
			case 51:
				while (s_HwOnly && s_HwPv < s_HwPvLabels.Count() && s_HwOnly.Find(s_HwPvLabels[s_HwPv]) < 0)
					s_HwPv++;
				if (s_HwPv >= s_HwPvLabels.Count())
				{
					nextStep = 60;
					next = 100;
					break;
				}
				HwGo(p, s_HwPvTargets[s_HwPv] + "7 0 7", s_HwPvTargets[s_HwPv]);
				next = 12000;
				break;
			case 52:
				HwShot(p, s_HwPvLabels[s_HwPv], HwClearCam(s_HwPvTargets[s_HwPv], s_HwPvBack[s_HwPv], s_HwPvUp[s_HwPv]), s_HwPvTargets[s_HwPv] + "0 0.6 0");
				s_HwPv++;
				nextStep = 51;
				if (s_HwPv >= s_HwPvLabels.Count())
					nextStep = 60;
				break;
			case 60:
				vector hc = Vector(6905, 0, 10870);
				hc[1] = GetGame().SurfaceY(hc[0], hc[2]) - 8;
				s_HwRoom = GetGame().CreateObjectEx("Land_Underground_Storage_Prison", hc, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_NOLIFETIME);
				if (!s_HwRoom)
				{
					Print("[AZ_Vamp] TEST hole room FAILED to spawn");
					nextStep = 15;
					break;
				}
				s_HwRoom.SetPosition(hc);
				vector hBox[2];
				s_HwRoom.GetCollisionBox(hBox);
				vector hTop = hc + Vector(0, 2.0, 0);
				vector hHit, hNrm;
				int hComp;
				set<Object> hRes = new set<Object>;
				vector hFloor = hc + Vector(0, hBox[0][1] + 0.2, 0);
				if (DayZPhysics.RaycastRV(hTop, hc + Vector(0, hBox[0][1] - 0.5, 0), hHit, hNrm, hComp, hRes, null, null, false, false, ObjIntersectGeom))
					hFloor = hHit;
				p.SetPosition(hFloor + Vector(0, 0.1, 0));
				Print("[AZ_Vamp] TEST hole room at " + hc + " box " + hBox[0] + " / " + hBox[1] + " floor " + hFloor + " terrain " + GetGame().SurfaceY(hc[0], hc[2]));
				next = 6000;
				break;
			case 61:
				Print("[AZ_Vamp] TEST hole after 6 s tester y " + p.GetPosition()[1] + " terrain " + GetGame().SurfaceY(p.GetPosition()[0], p.GetPosition()[2]) + " fall " + p.IsFalling());
				p.SetPosition(p.GetPosition() + p.GetDirection() * 2.0);
				next = 6000;
				break;
			case 62:
				vector me = p.GetPosition();
				Print("[AZ_Vamp] TEST hole after 12 s tester y " + me[1] + " terrain " + GetGame().SurfaceY(me[0], me[2]) + " alive " + p.IsAlive() + " health " + p.GetHealth("", "Health"));
				HwShot(p, "hole_test", me + Vector(0, 1.6, 0) - p.GetDirection() * 2.5, me + Vector(0, 1.2, 0) + p.GetDirection() * 3);
				nextStep = HwNextStep(step);
				break;
			case 63:
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				vector zp = Vector(7050, 0, 11122);
				HwGo(p, zp + Vector(4, 0, 0), zp);
				zp[1] = GetGame().SurfaceY(zp[0], zp[2]);
				EntityAI pz = EntityAI.Cast(GetGame().CreateObjectEx("ZmbM_CitizenASkinny_Brown", zp, ECE_PLACE_ON_SURFACE | ECE_CREATEPHYSICS));
				if (pz)
				{
					pz.SetOrientation(Vector(90, 0, 0));
					EntityAI ph = pz.GetInventory().CreateAttachment("AZ_VampPumpkinHead");
					Print("[AZ_Vamp] TEST pumpkin zed " + pz + " head " + ph);
				}
				HwShot(p, "pumpkin_zed", zp + Vector(2.6, 1.5, 0.8), zp + Vector(0, 1.5, 0));
				nextStep = HwNextStep(step);
				break;
			case 64:
				AZ_VampEvent.Cure(p);
				if (s_HwDummy)
					GetGame().ObjectDelete(s_HwDummy);
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				vector cc = Vector(7050, 0, 11135);
				HwGo(p, cc + Vector(0, 0, -9), cc);
				s_HwCostumeAt = cc;
				string bodies[3] = {"SurvivorM_Boris", "SurvivorF_Linda", "SurvivorF_Eva"};
				string wear[3] = {AZ_VampCostumes.COAT, AZ_VampCostumes.DRESS_SICK, AZ_VampCostumes.DRESS_BEAUTY};
				s_HwCostumeDummies = new array<Object>;
				for (int ci = 0; ci < 3; ci++)
				{
					vector dp = cc + Vector((ci - 1) * 2.0, 0, 0);
					dp[1] = GetGame().SurfaceY(dp[0], dp[2]);
					PlayerBase cd = PlayerBase.Cast(GetGame().CreateObject(bodies[ci], dp, false, true));
					if (!cd)
					{
						Print("[AZ_Vamp] TEST costume dummy " + bodies[ci] + " FAILED to spawn");
						continue;
					}
					cd.SetDirection(Vector(0, 0, -1));
					EntityAI cw = cd.GetInventory().CreateAttachment(wear[ci]);
					s_HwCostumeDummies.Insert(cd);
					Print("[AZ_Vamp] TEST costume dummy " + bodies[ci] + " wears " + cw);
				}
				EntityAI cg = AZ_VampCostumes.Place(AZ_VampCostumes.COAT, cc + Vector(0, 0, -2.5), "30 0 0");
				AZ_VampCostumes.Place(AZ_VampCostumes.COAT, cc + Vector(0, 0, -2.5), "30 0 0");
				if (cg)
					s_HwCostumeDummies.Insert(cg);
				next = 12000;
				break;
			case 65:
			case 66:
			case 67:
			case 68:
			case 69:
				int ck = step - 65;
				vector ct = s_HwCostumeAt + Vector(-2.0, 0, 0);
				if (ck == 2)
					ct = s_HwCostumeAt;
				if (ck == 3)
					ct = s_HwCostumeAt + Vector(2.0, 0, 0);
				ct[1] = GetGame().SurfaceY(ct[0], ct[2]);
				vector ccam = ct + Vector(0.2, 1.1, -2.0);
				vector clook = ct + Vector(0, 1.0, 0);
				if (ck == 1)
					ccam = ct + Vector(-0.2, 1.2, 2.0);
				if (ck == 4)
				{
					ct = s_HwCostumeAt + Vector(0, 0, -2.5);
					ct[1] = GetGame().SurfaceY(ct[0], ct[2]);
					ccam = ct + Vector(0.6, 1.4, -1.2);
					clook = ct;
				}
				HwShot(p, HwCostumeLabel(ck), ccam, clook);
				next = 8000;
				nextStep = HwNextStep(step);
				break;
			case 70:
				AZ_VampEvent.Cure(p);
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				vector gc = Vector(7050, 0, 11143);
				HwGo(p, gc + Vector(0, 0, -10), gc);
				s_HwGcoatAt = gc;
				if (!s_HwCostumeDummies)
					s_HwCostumeDummies = new array<Object>;
				s_HwGcoat = new array<PlayerBase>;
				string gbodies[4] = {"SurvivorM_Boris", "SurvivorM_Boris", "SurvivorM_Boris", "SurvivorF_Eva"};
				float gx[4] = {-3.0, -1.0, 1.2, 3.6};
				for (int gi = 0; gi < 4; gi++)
				{
					vector gpos = gc + Vector(gx[gi], 0, 0);
					gpos[1] = GetGame().SurfaceY(gpos[0], gpos[2]);
					PlayerBase gd = PlayerBase.Cast(GetGame().CreateObject(gbodies[gi], gpos, false, true));
					s_HwGcoat.Insert(gd);
					if (!gd)
					{
						Print("[AZ_Vamp] TEST gcoat dummy " + gi + " FAILED to spawn");
						continue;
					}
					gd.SetDirection(Vector(0, 0, -1));
					EntityAI gw = gd.GetInventory().CreateAttachment(AZ_VampCostumes.GREATCOAT);
					if (gw)
						Print("[AZ_Vamp] TEST gcoat health " + gw.GetHealth("", "Health") + "/" + gw.GetMaxHealth("", "Health"));
					s_HwCostumeDummies.Insert(gd);
					Print("[AZ_Vamp] TEST gcoat dummy " + gi + " " + gbodies[gi] + " wears " + gw);
				}
				for (int gs = 0; gs < 3; gs++)
				{
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwGcoatStance, 2000 + gs * 3000, false, s_HwGcoat[1], DayZPlayerConstants.STANCEIDX_CROUCH);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwGcoatStance, 2000 + gs * 3000, false, s_HwGcoat[2], DayZPlayerConstants.STANCEIDX_PRONE);
				}
				EntityAI gg = AZ_VampCostumes.Place(AZ_VampCostumes.GREATCOAT, gc + Vector(0, 0, -2.5), "30 0 0");
				if (gg)
					s_HwCostumeDummies.Insert(gg);
				next = 12000;
				break;
			case 71:
			case 72:
			case 73:
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
				int gk = step - 71;
				vector gt = s_HwGcoatAt + Vector(-3.0, 0, 0);
				if (gk == 3)
					gt = s_HwGcoatAt + Vector(-1.0, 0, 0);
				if (gk == 4)
					gt = s_HwGcoatAt + Vector(1.2, 0, 0);
				if (gk == 5)
					gt = s_HwGcoatAt + Vector(3.6, 0, 0);
				gt[1] = GetGame().SurfaceY(gt[0], gt[2]);
				vector gcam = gt + Vector(0.3, 1.2, -2.7);
				vector glook = gt + Vector(0, 0.95, 0);
				if (gk == 1)
					gcam = gt + Vector(-0.3, 1.3, 2.7);
				if (gk == 2)
				{
					gcam = gt + Vector(0.25, 1.6, -1.0);
					glook = gt + Vector(0, 1.42, 0);
				}
				if (gk == 3)
				{
					gcam = gt + Vector(0.9, 1.0, -2.0);
					glook = gt + Vector(0, 0.6, 0);
				}
				if (gk == 4)
				{
					gcam = gt + Vector(1.4, 1.5, -1.6);
					glook = gt + Vector(0, 0.2, 0);
				}
				if (gk == 6)
				{
					vector ws = s_HwGcoatAt + Vector(-5.0, 0, 5.0);
					ws[1] = GetGame().SurfaceY(ws[0], ws[2]);
					PlayerBase wk = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", ws, false, true));
					if (wk)
					{
						wk.SetDirection(Vector(1, 0, 0));
						wk.GetInventory().CreateAttachment(AZ_VampCostumes.GREATCOAT);
						HumanInputController wkic = wk.GetInputController();
						Print("[AZ_Vamp] TEST walker input " + wkic);
						if (wkic)
						{
							wkic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 1);
							wkic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
						}
						s_HwCostumeDummies.Insert(wk);
						GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwGcoatWalkShot, 2500, false, p, wk, ws);
					}
					else
						Print("[AZ_Vamp] TEST gcoat walker FAILED to spawn");
					next = 9000;
					nextStep = HwNextStep(step);
					break;
				}
				if (gk == 7)
				{
					gt = s_HwGcoatAt + Vector(0, 0, -2.5);
					gt[1] = GetGame().SurfaceY(gt[0], gt[2]);
					gcam = gt + Vector(0.7, 1.5, -1.3);
					glook = gt;
				}
				HwShot(p, HwGcoatLabel(gk), gcam, glook);
				next = 8000;
				nextStep = HwNextStep(step);
				break;
			case 79:
				AZ_VampEvent.Cure(p);
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				vector wc = Vector(7050, 0, 11151);
				HwGo(p, wc + Vector(0, 0, -10), wc);
				s_HwGownAt = wc;
				if (!s_HwCostumeDummies)
					s_HwCostumeDummies = new array<Object>;
				s_HwGown = new array<PlayerBase>;
				string wbodies[4] = {"SurvivorF_Eva", "SurvivorF_Linda", "SurvivorF_Linda", "SurvivorM_Boris"};
				string wwear[4] = {AZ_VampCostumes.VELVET_GOWN, AZ_VampCostumes.GRAVE_GOWN, AZ_VampCostumes.GRAVE_GOWN, AZ_VampCostumes.VELVET_GOWN};
				float wx[4] = {-3.0, -0.8, 1.4, 3.6};
				for (int wi = 0; wi < 4; wi++)
				{
					vector wpos = wc + Vector(wx[wi], 0, 0);
					wpos[1] = GetGame().SurfaceY(wpos[0], wpos[2]);
					PlayerBase wd = PlayerBase.Cast(GetGame().CreateObject(wbodies[wi], wpos, false, true));
					s_HwGown.Insert(wd);
					if (!wd)
					{
						Print("[AZ_Vamp] TEST gown dummy " + wi + " FAILED to spawn");
						continue;
					}
					wd.SetDirection(Vector(0, 0, -1));
					EntityAI ww = wd.GetInventory().CreateAttachment(wwear[wi]);
					if (ww)
						Print("[AZ_Vamp] TEST gown health " + ww.GetHealth("", "Health") + "/" + ww.GetMaxHealth("", "Health"));
					s_HwCostumeDummies.Insert(wd);
					Print("[AZ_Vamp] TEST gown dummy " + wi + " " + wbodies[wi] + " wears " + ww);
				}
				for (int ws2 = 0; ws2 < 3; ws2++)
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwGcoatStance, 2000 + ws2 * 3000, false, s_HwGown[2], DayZPlayerConstants.STANCEIDX_CROUCH);
				EntityAI wg = AZ_VampCostumes.Place(AZ_VampCostumes.GRAVE_GOWN, wc + Vector(0, 0, -2.5), "30 0 0");
				if (wg)
					s_HwCostumeDummies.Insert(wg);
				next = 12000;
				break;
			case 80:
			case 81:
			case 82:
			case 83:
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
				int wk2 = step - 80;
				float wxs[9] = {-3.0, -3.0, -3.0, -0.8, -0.8, 1.4, 0, 3.6, 0};
				vector wt = s_HwGownAt + Vector(wxs[wk2], 0, 0);
				wt[1] = GetGame().SurfaceY(wt[0], wt[2]);
				vector wcam = wt + Vector(0.3, 1.2, -2.8);
				vector wlook = wt + Vector(0, 0.95, 0);
				if (wk2 == 1)
				{
					wcam = wt + Vector(0.2, 1.5, -0.9);
					wlook = wt + Vector(0, 1.38, 0);
				}
				if (wk2 == 2 || wk2 == 4)
					wcam = wt + Vector(-0.3, 1.3, 2.8);
				if (wk2 == 5)
				{
					wcam = wt + Vector(0.9, 1.0, -2.0);
					wlook = wt + Vector(0, 0.6, 0);
				}
				if (wk2 == 6)
				{
					vector wws = s_HwGownAt + Vector(-2.2, 0, -1.4);
					wws[1] = GetGame().SurfaceY(wws[0], wws[2]);
					PlayerBase wwk = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Eva", wws, false, true));
					if (wwk)
					{
						wwk.SetDirection(Vector(1, 0, 0));
						wwk.GetInventory().CreateAttachment(AZ_VampCostumes.VELVET_GOWN);
						HumanInputController wwkic = wwk.GetInputController();
						Print("[AZ_Vamp] TEST walker input " + wwkic);
						if (wwkic)
						{
							wwkic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 1);
							wwkic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
						}
						s_HwCostumeDummies.Insert(wwk);
						GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwGownWalkShot, 2500, false, p, wwk, wws);
					}
					else
						Print("[AZ_Vamp] TEST gown walker FAILED to spawn");
					next = 9000;
					nextStep = HwNextStep(step);
					break;
				}
				if (wk2 == 8)
				{
					wt = s_HwGownAt + Vector(0, 0, -2.5);
					wt[1] = GetGame().SurfaceY(wt[0], wt[2]);
					wcam = wt + Vector(0.7, 1.5, -1.3);
					wlook = wt;
				}
				HwShot(p, HwGownLabel(wk2), wcam, wlook);
				next = 8000;
				nextStep = HwNextStep(step);
				break;
			case 15:
				s_HwGuard = false;
				if (s_HwCostumeDummies)
				{
					foreach (Object cdo : s_HwCostumeDummies)
					{
						if (cdo)
							GetGame().ObjectDelete(cdo);
					}
					s_HwCostumeDummies = null;
				}
				AZ_VampProgress.s_BloodMoon = false;
				if (s_HwDummy)
					GetGame().ObjectDelete(s_HwDummy);
				if (s_HwHuman)
					GetGame().ObjectDelete(s_HwHuman);
				AZ_VampEvent.Cure(p);
				p.SetHealth("", "Health", p.GetMaxHealth("", "Health"));
				AZ_VampState.s_Data.eventActive = false;
				AZ_VampState.Save();
				GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
				HwShot(p, "done", "0 0 0", "-1 0 0");
				Print("[AZ_Vamp] TEST hw done, " + AZ_VampDecor.Report(p.GetPosition()));
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, nextStep);
	}

	float AZ_VampSunAsh_FireHealth()
	{
		return AZ_VampAsh.BURN_FIRE_HEALTH;
	}



	void VampPlagueBite(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return;
		vector pos = player.GetPosition() + player.GetDirection() * 1.2;
		PlayerBase vamp = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", pos, false, true));
		if (!vamp)
		{
			Print("[AZ_Vamp] TEST could not spawn the dummy vampire");
			return;
		}
		vamp.SetDirection(-player.GetDirection());
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		Print("[AZ_Vamp] TEST blood before " + player.GetHealth("", "Blood"));
		ActionAZVampBite.Apply(vamp, player);
		Print("[AZ_Vamp] TEST blood after " + player.GetHealth("", "Blood") + " turning " + player.m_AZVampTurning);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampPlagueCheck, 60000, false, player);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampPlagueCheck, 260000, false, player);
	}


	void VampPlagueCheck(PlayerBase player)
	{
		if (!player)
			return;
		EntityAI worn = player.FindAttachmentBySlotName("Mask");
		string item = "none";
		bool canDetach = false;
		if (worn)
		{
			item = worn.GetType();
			canDetach = worn.CanDetachAttachment(player) && player.CanReleaseAttachment(worn);
		}
		Print("[AZ_Vamp] TEST check vampire=" + AZ_VampState.IsVampire(player) + " sick=" + player.m_AZVampTurning + " stage=" + player.m_AZVampSickStage + " blood=" + player.GetHealth("", "Blood") + " uncon=" + player.IsUnconscious() + " synced=" + player.m_AZVampire + " mask=" + item + " canDetach=" + canDetach + " records=" + AZ_VampState.CountVampires());
	}


	void VampEventDummy(PlayerBase player)
	{
		if (!player)
			return;
		vector pos = player.GetPosition() + player.GetDirection() * 1.5;
		PlayerBase eva = PlayerBase.Cast(GetGame().CreateObject("SurvivorF_Eva", pos, false, true));
		if (!eva)
			return;
		eva.SetDirection(-player.GetDirection());
		eva.GetInventory().CreateAttachment("AZ_VampireMask");

		vector side = Vector(player.GetDirection()[2], 0, -player.GetDirection()[0]);
		PlayerBase man = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", pos + side * 1.2, false, true));
		if (man)
		{
			man.SetDirection(-player.GetDirection());
			man.GetInventory().CreateAttachment("AZ_VampireMask");
		}
		Print("[AZ_Vamp] TEST eva dummy spawned, tester health " + player.GetHealth("", "Health") + " underRoof " + MiscGameplayFunctions.IsUnderRoof(player) + " night " + GetGame().GetWorld().IsNight());
	}

	void VampEventCheck(PlayerBase player)
	{
		if (!player)
			return;
		Print("[AZ_Vamp] TEST event check health " + player.GetHealth("", "Health") + " vampire " + player.m_AZVampire + " event " + AZ_VampEvent.Active() + " underRoof " + MiscGameplayFunctions.IsUnderRoof(player) + " night " + GetGame().GetWorld().IsNight());
	}

	void VampSwapFed(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return;
		EntityAI old = player.FindAttachmentBySlotName("Mask");
		if (old)
			GetGame().ObjectDelete(old);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VampAttachFed, 500, false, player);
	}

	void VampAttachFed(PlayerBase player)
	{
		if (player && player.IsAlive())
		{
			player.GetInventory().CreateAttachment("AZ_VampireMask_Fed");
			Print("[AZ_Vamp] server swapped to Fed");
		}
	}
}

modded class MissionGameplay
{
	protected Camera m_VampCam;
	protected AZ_VampFaceLight m_AZFaceLight;
	protected int m_VampStep = -1;
	protected float m_VampWait = 0;
	protected bool m_VampBaseTalk;

	protected float m_AZFpsT;
	protected int m_AZFpsN;
	protected string m_AZFpsLabel;
	protected float m_AZFpsWorst;
	protected int m_AZFpsSlow;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (m_AZFpsLabel != "")
		{
			m_AZFpsT += timeslice;
			m_AZFpsN++;
			if (timeslice > m_AZFpsWorst)
				m_AZFpsWorst = timeslice;
			if (timeslice > 0.033)
				m_AZFpsSlow++;
		}
		if (AZ_VampShotRpc.s_Pending)
		{
			AZ_VampShotRpc.s_Pending = false;
			VampHwShot(AZ_VampShotRpc.s_Label, AZ_VampShotRpc.s_Cam, AZ_VampShotRpc.s_Target);
		}
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!AZ_VAMP_RIG || !p || m_VampStep >= 99)
			return;
		m_VampWait += timeslice;
		if (AZ_VAMP_EVENT_TEST)
		{
			VampEventClient(p, timeslice);
			return;
		}
		if (m_VampStep < 0 && AZ_VAMP_PLAGUE_TEST && VampSickPhase(p))
			return;
		if (m_VampStep < 0 && AZ_VAMP_PLAGUE_TEST)
		{

			if (m_VampWait < 25 || !p.m_AZVampire || p.IsUnconscious() || !p.FindAttachmentBySlotName("Mask"))
				return;
			Print("[AZ_Vamp] TEST client sees turned " + p.GetType());
			m_VampStep = 0;
			m_VampWait = 99;
		}
		if (AZ_VAMP_PLAGUE_TEST && m_VampStep >= 0 && m_VampStep < 99)
		{
			if (m_VampWait < 7)
				return;
			m_VampWait = 0;
			if (m_VampStep == 4)
			{
				p.m_AZVampTalk = false;
				if (m_VampCam)
					m_VampCam.SetActive(false);
				Print("[AZ_Vamp] done");
				m_VampStep = 99;
				return;
			}
			string turned[4] = {"turned_front", "turned_eyes", "turned_mouth", "turned_talk_mouth"};
			int frame[4] = {0, 1, 2, 2};
			p.m_AZVampTalk = (m_VampStep == 3);
			VampShot(p, frame[m_VampStep], turned[m_VampStep]);
			m_VampStep++;
			return;
		}
		if (m_VampStep < 0)
		{
			if (m_VampWait < 25 || !p.FindAttachmentBySlotName("Mask"))
				return;
			Print("[AZ_Vamp] character " + p.GetType());
			m_VampStep = 0;
			m_VampWait = 99;
		}
		if (m_VampWait < 7)
			return;
		if (m_VampStep == 4 && !m_VampBaseTalk)
		{
			m_VampBaseTalk = true;
			p.m_AZVampTalk = true;
			m_VampWait = 0;
			VampShot(p, 2, "base_talk_mouth");
			return;
		}
		if (m_VampStep == 4)
		{
			p.m_AZVampTalk = false;
			EntityAI mask = p.FindAttachmentBySlotName("Mask");
			if (!mask || !mask.IsKindOf("AZ_VampireMask_Fed"))
				return;
		}
		if (m_VampStep == 8)
		{
			p.m_AZVampTalk = true;
			m_VampWait = 0;
			VampShot(p, 2, "fed_talk_mouth");
			m_VampStep++;
			return;
		}
		if (m_VampStep == 9)
		{
			p.m_AZVampTalk = false;
			m_VampWait = 0;
			VampShot(p, 3, "fed_kiss_emote");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VampKiss, 1500, false, p);
			m_VampStep++;
			return;
		}
		if (m_VampStep >= 10)
		{
			if (m_VampCam)
				m_VampCam.SetActive(false);
			Print("[AZ_Vamp] done");
			m_VampStep = 99;
			return;
		}
		m_VampWait = 0;
		VampShot(p, m_VampStep);
		m_VampStep++;
	}

	protected float m_VampT;
	protected int m_VampSent;
	protected int m_VampSick = -1;
	protected bool m_VampSickSeenDown;
	protected int m_VampSickAwake;



	bool VampSickPhase(PlayerBase p)
	{
		if (m_VampSick >= 5)
			return false;
		if (m_VampSick < 0)
		{


			if (!p.m_AZVampTurning)
				return false;
			if (p.IsUnconscious())
				m_VampSickSeenDown = true;
			if (!m_VampSickSeenDown || p.IsUnconscious())
			{
				m_VampSickAwake = 0;
				return false;
			}
			m_VampSickAwake += 1;
			if (m_VampSickAwake < 150)
				return false;
			AZ_VampSunDebug.s_On = true;
			Print("[AZ_Vamp] TEST client sees sick, stage " + p.m_AZVampSickStage);
			m_VampSick = 0;
			m_VampWait = 99;
		}
		if (m_VampWait < 7)
			return true;
		m_VampWait = 0;
		if (m_VampSick <= 1)
			VampShotAt(p, 90 - 180 * m_VampSick, 0.42, -0.13, -0.15, 0.40, "sick_neck_" + m_VampSick);
		else if (m_VampSick <= 3)
		{
			PlayerBase dummy = VampFindDummy(p, "SurvivorM_Mirek");
			if (dummy)
				VampShot(dummy, m_VampSick - 2, "dummy_" + (m_VampSick - 2));
			else
				Print("[AZ_Vamp] TEST client found no dummy vampire");
		}
		else
			VampShotAt(p, 90, 4.0, 1.2, 0.6, 0.9, "sun_wide");
		m_VampSick++;
		if (m_VampSick >= 5)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VampCamOff, 6000, false);
		return true;
	}

	void VampCamOff()
	{
		if (m_VampCam)
			m_VampCam.SetActive(false);
	}


	void VampShotAt(PlayerBase p, float yawDeg, float dist, float hgt, float aim, float fov, string label)
	{
		vector head = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		vector fwd = p.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		float a = yawDeg * Math.DEG2RAD;
		vector dir = Vector(fwd[0] * Math.Cos(a) - fwd[2] * Math.Sin(a), 0, fwd[0] * Math.Sin(a) + fwd[2] * Math.Cos(a));
		vector cp = head + dir * dist + Vector(0, hgt, 0);
		if (!m_VampCam)
			Class.CastTo(m_VampCam, GetGame().CreateObject("staticcamera", cp, true));
		if (!m_VampCam)
			return;
		m_VampCam.SetPosition(cp);
		m_VampCam.LookAt(head + Vector(0, aim, 0));
		m_VampCam.SetFOV(fov);
		m_VampCam.SetActive(true);
		GetGame().GetWorld().SetEyeAccom(1.0);
		VampShotLine(label, cp, head + Vector(0, aim, 0));
	}


	void VampShotLine(string label, vector cam, vector target)
	{
		Print("[AZ_Vamp] client start " + label);
		Print("[SHOT] " + label + " cam " + cam + " target " + target);
	}


	void VampSend(PlayerBase p, string line)
	{
		GetGame().RPCSingleParam(p, AZ_VampEvent.RPC_CMD, new Param1<string>(line), true);
		Print("[AZ_Vamp] TEST client sent " + line);
	}

	void VampEventClient(PlayerBase p, float dt)
	{
		m_VampT += dt;
		if (m_VampSent == 0 && m_VampT > 20)
		{
			VampSend(p, "/vamp status");
			m_VampSent = 1;
		}
		if (m_VampSent == 1 && m_VampT > 23)
		{
			VampSend(p, "/vamp start");
			m_VampSent = 2;
		}
		if (m_VampStep < 0)
		{
			if (m_VampSent < 2 || !p.m_AZVampire || !p.FindAttachmentBySlotName("Mask"))
				return;
			Print("[AZ_Vamp] TEST client sees patient zero " + p.GetType());
			m_VampStep = 0;
			m_VampWait = 99;
		}
		if (m_VampStep >= 99 || m_VampWait < 7)
			return;
		m_VampWait = 0;
		if (m_VampStep < 2)
		{
			string own[2] = {"tester_front", "tester_eyes"};
			VampShot(p, m_VampStep, own[m_VampStep]);
		}
		else if (m_VampStep < 6)
		{
			string want = "SurvivorF_Eva";
			if (m_VampStep >= 4)
				want = "SurvivorM_Mirek";
			PlayerBase dummy = VampFindDummy(p, want);
			if (dummy)
			{
				string dn[4] = {"eva_front", "eva_eyes", "male_front", "male_eyes"};
				VampShot(dummy, (m_VampStep - 2) % 2, dn[m_VampStep - 2]);
			}
			else
				Print("[AZ_Vamp] TEST client found no dummy " + want);
		}
		else
		{
			if (m_VampCam)
				m_VampCam.SetActive(false);
			VampSend(p, "/vamp stop");
			VampSend(p, "/vamp status");
			Print("[AZ_Vamp] done");
			m_VampStep = 99;
			return;
		}
		m_VampStep++;
	}


	void VampHwShot(string label, vector cam, vector target)
	{

		if (m_AZFpsLabel != "" && m_AZFpsT > 0.5)
			Print("[AZ_Vamp] TEST fps " + m_AZFpsLabel + " " + Math.Round(m_AZFpsN / m_AZFpsT) + " (" + Math.Round(m_AZFpsT) + " s), worst frame " + Math.Round(m_AZFpsWorst * 1000) + " ms, frames over 33 ms " + m_AZFpsSlow);
		m_AZFpsLabel = label;
		m_AZFpsT = 0;
		m_AZFpsN = 0;
		m_AZFpsWorst = 0;
		m_AZFpsSlow = 0;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;

		if (m_AZFaceLight)
		{
			m_AZFaceLight.Destroy();
			m_AZFaceLight = null;
		}
		if (cam != vector.Zero && (label.IndexOf("ev_") == 0 || label.IndexOf("hw_fangs") == 0 || label == "hw_bite_face"))
			m_AZFaceLight = AZ_VampFaceLight.Cast(ScriptedLightBase.CreateLight(AZ_VampFaceLight, vector.Lerp(target, cam, 0.6) + "0 0.05 0"));
		if (label == "done")
		{
			if (m_VampCam)
				m_VampCam.SetActive(false);
			Print("[AZ_Vamp] done");
			Print("[SHOT] done");
			return;
		}
		if (cam == vector.Zero)
		{
			int i = Math.Round(target[0]);
			if (i == 9)
				VampShotAt(p, 150, 3.2, 0.4, -0.5, 0.9, label);
			else if (i == 11)
				VampFirstPersonShot(p, label);
			else
				VampShot(p, i, label);
			return;
		}
		if (label.Contains("_off"))
		{
			array<Object> lofs = new array<Object>;
			GetGame().GetObjectsAtPosition(target, 400, lofs, null);
			int offs = 0;
			foreach (Object lo : lofs)
			{
				PumpkinHelmet ph = PumpkinHelmet.Cast(lo);
				if (ph)
				{
					ph.SetPilotLight(false);
					offs++;
				}
			}
			Print("[AZ_Vamp] TEST pilot lights off: " + offs);
		}
		if (!m_VampCam)
			Class.CastTo(m_VampCam, GetGame().CreateObject("staticcamera", cam, true));
		if (!m_VampCam)
			return;
		m_VampCam.SetPosition(cam);
		m_VampCam.LookAt(target);
		m_VampCam.SetFOV(0.9);
		m_VampCam.SetActive(true);
		VampShotLine(label, cam, target);
	}


	void VampFirstPersonShot(PlayerBase p, string label)
	{
		if (!p)
			p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		if (m_VampCam)
			m_VampCam.SetActive(false);

		Print("[AZ_Vamp] TEST 1pp third person " + p.IsInThirdPerson());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VampFirstPersonShotLine, 800, false, p, label);
	}

	void VampFirstPersonShotLine(PlayerBase p, string label)
	{
		if (!p)
			return;
		Print("[AZ_Vamp] TEST 1pp own eye view " + AZ_VampFP.OwnEyeView(p) + " cam to head " + vector.Distance(GetGame().GetCurrentCameraPosition(), p.GetBonePositionWS(p.GetBoneIndexByName("Head"))));
		vector hd = p.GetBonePositionWS(p.GetBoneIndexByName("Head"));
		VampShotLine(label, hd, hd + p.GetDirection());
	}

	PlayerBase VampFindDummy(PlayerBase p, string type)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(p.GetPosition(), 5, objs, null);
		foreach (Object o : objs)
		{
			PlayerBase pb = PlayerBase.Cast(o);
			if (pb && pb != p && pb.GetType() == type)
				return pb;
		}
		return null;
	}

	void VampKiss(PlayerBase p)
	{
		if (p && p.GetEmoteManager())
		{
			p.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_TAUNTKISS);
			Print("[AZ_Vamp] kiss emote started");
		}
	}



	void VampShot(PlayerBase p, int i, string label = "")
	{
		string names[8] = {"base_front", "base_eyes", "base_mouth", "base_left34", "fed_front", "fed_eyes", "fed_mouth", "fed_left34"};
		if (label == "")
			label = names[i];
		float yaw[8] = {0, 0, 0, 40, 0, 0, 0, 40};
		float dist[8] = {0.55, 0.28, 0.30, 0.55, 0.55, 0.28, 0.30, 0.55};
		float hgt[8] = {0.0, -0.01, -0.06, 0.0, 0.0, -0.01, -0.06, 0.0};
		float aim[8] = {0.0, -0.01, -0.09, 0.0, 0.0, -0.01, -0.09, 0.0};
		float fov[8] = {0.45, 0.25, 0.25, 0.45, 0.45, 0.25, 0.25, 0.45};
		vector head = p.GetBonePositionWS(p.GetBoneIndexByName("Head")) + Vector(0, 0.07, 0);
		vector fwd = p.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		float a = yaw[i] * Math.DEG2RAD;
		vector dir = Vector(fwd[0] * Math.Cos(a) - fwd[2] * Math.Sin(a), 0, fwd[0] * Math.Sin(a) + fwd[2] * Math.Cos(a));
		vector cp = head + dir * dist[i] + Vector(0, hgt[i], 0);
		if (!m_VampCam)
			Class.CastTo(m_VampCam, GetGame().CreateObject("staticcamera", cp, true));
		if (!m_VampCam)
		{
			Print("[AZ_Vamp] could not create a staticcamera");
			return;
		}
		m_VampCam.SetPosition(cp);
		m_VampCam.LookAt(head + Vector(0, aim[i], 0));
		m_VampCam.SetFOV(fov[i]);
		m_VampCam.SetActive(true);
		GetGame().GetWorld().SetEyeAccom(1.0);
		VampShotLine(label, cp, head + Vector(0, aim[i], 0));
	}
}

