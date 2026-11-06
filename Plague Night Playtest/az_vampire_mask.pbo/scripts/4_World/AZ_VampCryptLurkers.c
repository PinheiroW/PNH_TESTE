














modded class AZ_VampConfig
{
	bool cryptLurkers = true;
	bool cryptLurkersTurn = false;
	int riverStareS = 8;
}


class AZ_LurkerLieCB : HumanCommandActionCallback
{
}

class AZ_VampCryptLurkers
{
	static const int SHHH_MS = 1200;
	static const int HOLD_MS = 4000;
	static const int FADE_MS = 4000;
	static const int COOLDOWN_MS = 120000;
	static const float ELDER_CHANCE = 0.3;
	static const int KEEP_MS = 3000;
	static const int RISE_MS = 2500;


	static const string COFFIN_CLASS = "AZ_Crypt_Coffin_Rest";
	static ref array<Object> s_Coffins;
	static const string BEDS = "-12.3 -11.95 180 0, 9.6 -7.35 180 0.9, -8.5 1.0 180 0 room";
	static const int RPC_SHHH = 4702238;

	static ref array<PlayerBase> s_Sleepers;
	static ref array<vector> s_Beds;
	static ref array<float> s_BedY;



	static const int LIE_TRIES = 3;
	static ref array<int> s_LieTries;
	static ref map<string, bool> s_Done;
	static int s_ReadyAt;
	static bool s_Busy;
	static int s_Gen;
	static int s_Replaced = -60000;
	static int s_Keeps;




	static const int TEASE_SHOW_MS = 2000;
	static const int TEASE_TO_BITE_MS = 1200;
	static const int TASTE_MS = 1800;
	static const float TASTE_ML = 400;
	static const int VANISH_MS = 600;
	static const int HUNT_MS = 180000;
	static const int TASTE_COOLDOWN_MS = 30000;
	static const int RPC_SAY = 4702239;
	static ref map<string, int> s_TastedAt;
	static ref map<string, int> s_TasterBed;
	static PlayerBase s_Show;
	static float s_TasteFrom;
	static bool s_CageTaste;
	static bool s_CageFeed;
	static const int OPEN_MS = 3000;
	static const int STEP_IN_MS = 600;
	static const int CAGE_LIFT_MS = 1500;
	static bool s_ProbeElder;

	static bool On()
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		return cfg && cfg.cryptTraps && cfg.cryptLurkers;
	}

	static void Build()
	{
		s_Sleepers = new array<PlayerBase>;
		s_Beds = new array<vector>;
		s_BedY = new array<float>;
		s_LieTries = new array<int>;
		s_Done = new map<string, bool>;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg && !cfg.cryptLurkers)
		{
			Print("[AZ_Vamp] crypt lurkers: off (cryptLurkers false), none placed");
			return;
		}







		array<string> beds = new array<string>;
		BEDS.Split(",", beds);
		foreach (string bed : beds)
		{
			array<string> v = new array<string>;
			bed.Trim().Split(" ", v);
			if (v.Count() < 4)
				continue;
			if (v.Count() > 4 && v[4] == "room" && AZ_VampCryptTraps.s_RoomSealed)
				continue;
			s_Beds.Insert(Vector(v[0].ToFloat(), v[1].ToFloat(), v[2].ToFloat()));
			s_BedY.Insert(v[3].ToFloat());
			s_LieTries.Insert(0);
		}
		s_Coffins = new array<Object>;
		bool coffins = GetGame().ConfigIsExisting("CfgVehicles " + COFFIN_CLASS);
		for (int c = 0; c < s_Beds.Count(); c++)
		{
			Object cof;
			if (coffins)
				cof = AZ_VampCrypt.PutCrypt(COFFIN_CLASS, CoffinAt(c), AZ_VampCrypt.HALL_YAW + s_Beds[c][2]);
			s_Coffins.Insert(cof);
		}
		Print("[AZ_Vamp] crypt lurkers: coffins " + coffins);
		for (int i = 0; i < s_Beds.Count(); i++)
			s_Sleepers.Insert(Sleep(i));
		s_Gen++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Keep, KEEP_MS, false, s_Gen);
		Print("[AZ_Vamp] crypt lurkers asleep: " + s_Sleepers.Count() + ", on " + On());
	}



	static PlayerBase Dress(vector at, float yaw, int seed, bool bared = true)
	{
		PlayerBase v = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Unique(AZ_VampDress.Any(seed + 1)), at, false, true));
		if (!v)
			return null;
		AZ_VampDress.Note(v);
		v.SetPosition(at);
		v.SetOrientation(Vector(yaw, 0, 0));
		AZ_VampDress.Elder(v, seed);
		v.m_AZElder = true;




		if (bared)
			v.GetInventory().CreateAttachment("AZ_VampireMask_Bared");
		AZ_VampState.SetVampire(v, true);
		AZ_VampBind.Bind(v);
		return v;
	}






	static const float SAFE_UP = 0.4;
	static const float SAFE_DOWN = 0.6;

	static vector SafeFloor(vector at, float refY)
	{
		vector from = Vector(at[0], refY + SAFE_UP, at[2]);
		vector to = Vector(at[0], refY - SAFE_DOWN, at[2]);
		vector hit, nrm;
		int comp;
		set<Object> res = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, hit, nrm, comp, res, null, null, false, false, ObjIntersectGeom))
			return Vector(at[0], refY, at[2]);
		if (Math.AbsFloat(hit[1] - refY) > 0.15)
		{
			string what = "?";
			foreach (Object o : res)
			{
				if (o)
					what = o.GetType();
			}
			Print("[AZ_Vamp] crypt floor snap at hall " + AZ_VampCrypt.HallLocal(at) + ": " + (hit[1] - refY) + " m off the expected height, on " + what);
		}
		return Vector(at[0], hit[1], at[2]);
	}


	static vector CoffinAt(int i)
	{
		vector b = s_Beds[i];
		vector at = AZ_VampCrypt.Hall(b[0], b[1], s_BedY[i]);
		return SafeFloor(at, at[1]);
	}



	static void Bare(PlayerBase v)
	{
		if (v && v.IsAlive() && !v.FindAttachmentBySlotName("Mask"))
			v.GetInventory().CreateAttachment("AZ_VampireMask_Bared");
	}

	static void Rise(int bed)
	{
		PlayerBase v = s_Sleepers[bed];
		s_Sleepers[bed] = null;
		if (!v || !v.IsAlive())
		{
			if (v)
				GetGame().ObjectDelete(v);
			return;
		}
		HumanCommandActionCallback lie = v.GetCommand_Action();
		if (lie)
			lie.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		v.SetHealth("", "Shock", 100);
		EntityAI calm = v.FindAttachmentBySlotName("Mask");
		if (calm && !calm.IsKindOf("AZ_VampireMask_Bared"))
		{
			GetGame().ObjectDelete(calm);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Bare, 100, false, v);
		}
		Print("[AZ_Vamp] crypt lurker " + bed + " rises from its bed");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiseGone, RISE_MS - 100, false, v);
	}

	static void RiseGone(PlayerBase v)
	{
		if (v)
			GetGame().ObjectDelete(v);
	}

	static PlayerBase Sleep(int i)
	{
		vector b = s_Beds[i];



		vector at;
		Object cof;
		if (s_Coffins && i < s_Coffins.Count())
			cof = s_Coffins[i];
		if (cof)
		{
			at = cof.GetPosition() + "0 0.06 0";
			if (cof.MemoryPointExists("body"))
				at = cof.ModelToWorld(cof.GetMemoryPointPos("body"));
		}
		else
		{
			vector floor = AZ_VampCrypt.Hall(b[0], b[1], s_BedY[i]);
			at = SafeFloor(floor, floor[1]);
			at[1] = at[1] + 0.05;
		}

		PlayerBase v = Dress(at, AZ_VampCrypt.HALL_YAW + b[2] + 180, i, false);
		if (v)
		{
			if (s_LieTries[i] >= LIE_TRIES)
				v.SetHealth("", "Shock", 0);
			else
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(LieDown, 1000, false, v, i);
			EntityAI body = v.FindAttachmentBySlotName("Body");
			string worn = "none";
			if (body)
				worn = body.GetType();
			Print("[AZ_Vamp] crypt lurker " + i + " asleep: " + v.GetType() + " in " + worn + ", " + v.GetInventory().AttachmentCount() + " items worn");
		}
		return v;
	}

	static void LieDown(PlayerBase v, int i)
	{
		if (!v || !v.IsAlive() || v.GetCommand_Action())
			return;
		HumanCommandActionCallback cb = v.StartCommand_Action(DayZPlayerConstants.CMD_GESTUREFB_LYINGDOWN, AZ_LurkerLieCB, DayZPlayerConstants.STANCEMASK_CROUCH);
		Print("[AZ_Vamp] crypt lurker " + i + " lies down: started " + (cb != null));
	}


	static void Keep(int gen)
	{
		if (gen != s_Gen)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Keep, KEEP_MS, false, gen);
		s_Keeps++;
		if (s_Keeps == 10)
		{
			for (int j = 0; j < s_Sleepers.Count(); j++)
			{
				if (s_Sleepers[j])
					Print("[AZ_Vamp] crypt lurker " + j + " lies at hall " + AZ_VampCrypt.HallLocal(s_Sleepers[j].GetPosition()) + " (bed " + s_Beds[j] + " y " + s_BedY[j] + ") alive " + s_Sleepers[j].IsAlive() + " shock " + s_Sleepers[j].GetHealth("", "Shock"));
			}
		}
		for (int i = 0; i < s_Sleepers.Count(); i++)
		{
			PlayerBase v = s_Sleepers[i];
			if (!v)
				continue;
			if (!v.IsAlive())
			{
				int t = GetGame().GetTime();
				if (t - s_Replaced < 60000)
					continue;
				s_Replaced = t;
				Print("[AZ_Vamp] crypt lurker " + i + " died in its bed at " + v.GetPosition() + " (bed " + AZ_VampCrypt.Hall(s_Beds[i][0], s_Beds[i][1], s_BedY[i]) + "): replaced");
				GetGame().ObjectDelete(v);
				PlayerBase n = Sleep(i);
				if (n)
					s_Sleepers[i] = n;
				continue;
			}
			if (s_LieTries[i] >= LIE_TRIES)
			{
				if (v.GetHealth("", "Shock") > 5)
					v.SetHealth("", "Shock", 0);
			}
			else if (!v.GetCommand_Action())
			{
				s_LieTries[i] = s_LieTries[i] + 1;
				if (s_LieTries[i] >= LIE_TRIES)
				{
					v.SetHealth("", "Shock", 0);
					Print("[AZ_Vamp] crypt lurker " + i + " would not lie down: unconscious instead");
				}
				else
					LieDown(v, i);
			}
			if (v.GetHealth("", "Health") < 80)
				v.SetHealth("", "Health", 100);
		}
	}



	static void ProbeCorpseStart()
	{
		if (!s_Sleepers || s_Sleepers.Count() == 0 || !s_Sleepers[0])
			return;
		PlayerBase v = s_Sleepers[0];
		v.SetHealth("", "", 0);
		Print("[AZ_Vamp] lurker probe: sleeper 0 killed, alive " + v.IsAlive());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProbeCorpseCheck, 75000, false, v);
	}

	static void ProbeCorpseCheck(PlayerBase old)
	{
		if (!s_Sleepers || s_Sleepers.Count() == 0)
			return;
		PlayerBase cur = s_Sleepers[0];
		bool replaced = cur && cur.IsAlive() && cur != old;
		string verdict = "FAIL";
		if (replaced)
			verdict = "PASS";
		Print("[AZ_Vamp] lurker probe: bed 0 has a living new sleeper " + replaced + " " + verdict + " (expect the corpse replaced)");
	}

	static void OnTrapFail(PlayerBase p, string trap)
	{
		int bed = Claim(p, trap);
		if (bed < 0)
			return;
		if (Hunted(p))
		{
			Print("[AZ_Vamp] crypt lurker: the hunt, " + AZ_VampState.Uid(p) + " was tasted " + ((GetGame().GetTime() - s_TastedAt.Get(AZ_VampState.Uid(p))) / 1000) + " s ago: the full feed");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Strike, RISE_MS, false, p, bed);
			return;
		}
		s_ReadyAt = GetGame().GetTime() + TASTE_COOLDOWN_MS;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tease, RISE_MS, false, p, bed);
	}


	static bool Hunted(PlayerBase p)
	{
		if (!s_TastedAt || !p)
			return false;
		string uid = AZ_VampState.Uid(p);
		return s_TastedAt.Contains(uid) && GetGame().GetTime() - s_TastedAt.Get(uid) < HUNT_MS;
	}


	static string PlayerVampireInside()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase v : list)
		{
			if (v && v.IsAlive() && (v.m_AZVampire || AZ_VampState.IsVampire(v)) && AZ_VampCrypt.Inside(v.GetPosition()))
				return AZ_VampState.Uid(v);
		}
		return "";
	}


	static void Tease(PlayerBase p, int bed)
	{
		if (!p || !p.IsAlive())
		{
			s_Busy = false;
			return;
		}
		vector spot;
		if (TeaseSpot(p, spot))
		{
			PlayerBase old = s_Sleepers[bed];
			if (old)
				GetGame().ObjectDelete(old);
			s_Sleepers[bed] = null;
			s_Show = Dress(spot, 0, bed);
			if (s_Show)
			{
				vector look = p.GetPosition() - spot;
				look[1] = 0;
				s_Show.SetDirection(look.Normalized());
			}
			Say(p, "AZ_VampLurkLaugh_SoundSet", spot + "0 1.6 0");
			Print("[AZ_Vamp] crypt lurker " + bed + " shows itself to " + AZ_VampState.Uid(p) + " " + Math.Round(vector.Distance(spot, p.GetPosition())) + " m off");
		}
		else
			Print("[AZ_Vamp] crypt lurker: no clear spot 6-10 m from " + AZ_VampState.Uid(p) + " to show itself, straight to the taste");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeaseGone, TEASE_SHOW_MS, false, p, bed);
	}



	static bool TeaseSpot(PlayerBase p, out vector spot)
	{
		vector pos = p.GetPosition();
		vector fwd = p.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		vector eye = pos + "0 1.6 0";
		array<float> dists = {8.0, 7.0, 9.0, 6.0, 10.0};
		array<float> turns = {0, 30, -30, 60, -60, 90, -90, 120, -120, 150, -150, 180};
		foreach (float d : dists)
		{
			foreach (float t : turns)
			{
				float a = t * Math.DEG2RAD;
				vector dir = Vector(fwd[0] * Math.Cos(a) - fwd[2] * Math.Sin(a), 0, fwd[0] * Math.Sin(a) + fwd[2] * Math.Cos(a));
				vector c = SafeFloor(pos + dir * d, pos[1]);
				if (Math.AbsFloat(c[1] - pos[1]) > 0.6 || !AZ_VampCrypt.Inside(c))
					continue;
				if (Blocked(eye, c + "0 1.5 0", p) || Blocked(c + "0 0.4 0", c + "0 1.8 0", p))
					continue;
				spot = c;
				return true;
			}
		}
		return false;
	}

	static bool Blocked(vector from, vector to, Object ignore)
	{
		vector hit, normal;
		int comp;
		set<Object> results = new set<Object>;
		return DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, ignore, false, false, ObjIntersectGeom);
	}

	static void TeaseGone(PlayerBase p, int bed)
	{
		if (s_Show)
			GetGame().ObjectDelete(s_Show);
		s_Show = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Taste, TEASE_TO_BITE_MS, false, p, bed);
	}


	static void Taste(PlayerBase p, int bed)
	{
		if (p)
			s_TasteFrom = p.GetHealth("", "Blood");
		StrikeWith(p, bed, null, false);
	}

	static void TasteEnd(PlayerBase vamp, PlayerBase p, int bed, int serial = 0)
	{
		if (p && p.IsAlive() && !BiteOwned(vamp, p, serial))
			Print("[AZ_Vamp] crypt lurker taste of " + AZ_VampState.Uid(p) + " was broken before its end: no blood taken");
		else if (p && p.IsAlive())
		{
			AZ_VampEvidence.Begin("lurker_taste", vamp, p, false);
			float target = Math.Max(s_TasteFrom - TASTE_ML, 1000);
			if (p.GetHealth("", "Blood") > target)
				p.SetHealth("", "Blood", target);
			AZ_VampEvidence.End("lurker_taste", vamp, p);
			AZ_VampFeed.Release(vamp, p);
			if (!s_TastedAt)
				s_TastedAt = new map<string, int>;
			s_TastedAt.Set(AZ_VampState.Uid(p), GetGame().GetTime());
			if (!s_TasterBed)
				s_TasterBed = new map<string, int>;
			s_TasterBed.Set(AZ_VampState.Uid(p), bed);
			Say(p, "AZ_VampLine4_play_2_SoundSet", p.GetPosition());
			Print("[AZ_Vamp] crypt lurker tasted " + AZ_VampState.Uid(p) + ": blood " + s_TasteFrom + " -> " + p.GetHealth("", "Blood") + ", let go; the hunt is on " + (HUNT_MS / 1000) + " s");
			if (s_CageTaste)
				AZ_VampCryptCage.WatchLinger(p);
		}
		s_CageTaste = false;
		AZ_VampFeedAnim.DummyFeedEnd(vamp);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Vanish, VANISH_MS, false, vamp, bed);
	}

	static void Vanish(PlayerBase vamp, int bed)
	{
		if (vamp)
			GetGame().ObjectDelete(vamp);
		s_Busy = false;


		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Wake, TASTE_COOLDOWN_MS, false, bed, s_Gen);
	}


	static void RefillBeds()
	{
		if (!s_Sleepers)
			return;
		for (int i = 0; i < s_Sleepers.Count(); i++)
		{
			if (!s_Sleepers[i])
				s_Sleepers[i] = Sleep(i);
		}
	}


	static void Say(PlayerBase p, string soundSet, vector at)
	{
		if (p && p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_SAY, new Param2<string, vector>(soundSet, at), true, p.GetIdentity());
	}



	static int Claim(PlayerBase p, string trap)
	{

		if (!p)
			return -1;
		string who = AZ_VampState.Uid(p);
		if (!On() || !s_Sleepers)
		{
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + ", lurkers off (on " + On() + ", built " + (s_Sleepers != null) + ")");
			return -1;
		}
		if (!AZ_VampCryptTraps.IsHuman(p))
		{
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + ", not a human (vampire, turning or dead)");
			return -1;
		}
		if (p.m_AZVampHeld)
		{
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + ", already held by a vampire");
			return -1;
		}
		string key = who + "|" + trap;
		int now = GetGame().GetTime();
		if (s_Done.Contains(key))
		{
			AZ_VampWhisperLines.Trigger(p, "trap");
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + " again, already ambushed for it");
			return -1;
		}
		if (s_Busy || now < s_ReadyAt)
		{
			AZ_VampWhisperLines.Trigger(p, "trap");
			Print("[AZ_Vamp] crypt lurker: " + AZ_VampState.Uid(p) + " failed " + trap + ", none ready (busy " + s_Busy + ", cooldown " + ((s_ReadyAt - now) / 1000) + " s)");
			return -1;
		}
		string answer = PlayerVampireInside();
		if (answer != "")
		{
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + ", a vampire player is in the crypt (" + answer + "), the lurkers leave it to them");
			return -1;
		}
		int bed = Nearest(p.GetPosition());
		if (bed < 0 && Hunted(p) && s_TasterBed && s_TasterBed.Contains(who))
			bed = s_TasterBed.Get(who);
		if (bed < 0)
		{
			Print("[AZ_Vamp] crypt lurker: " + who + " failed " + trap + ", no bed free");
			return -1;
		}
		s_Done.Set(key, true);
		s_Busy = true;
		s_ReadyAt = now + COOLDOWN_MS;
		if (p.GetIdentity())
		{
			GetGame().RPCSingleParam(p, RPC_SHHH, new Param1<int>(1), true, p.GetIdentity());
			GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_WHISPER, new Param1<int>(1), true, p.GetIdentity());
		}
		Print("[AZ_Vamp] crypt lurker ambush " + AZ_VampState.Uid(p) + " after " + trap + " (sleeper " + bed + ")");
		AZ_VampWhisperLines.Trigger(p, "lurk");
		Rise(bed);
		return bed;
	}






	static PlayerBase s_CageVamp;
	static bool s_CageFull;
	static const int CIRCLE_MS = 4000;
	static const int CIRCLE_STEPS = 40;

	static void CageAmbush(PlayerBase p)
	{
		int bed = Claim(p, "cage");
		if (bed < 0)
			return;
		bool full = Hunted(p);
		if (!full)
			s_ReadyAt = GetGame().GetTime() + TASTE_COOLDOWN_MS;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageArrive, RISE_MS, false, p, bed, full);
	}


	static void CageHunt(PlayerBase p)
	{
		int bed = Claim(p, "cage hunt");
		if (bed >= 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageArrive, RISE_MS, false, p, bed, true);
	}

	static void CageArrive(PlayerBase p, int bed, bool full)
	{
		PlayerBase old = s_Sleepers[bed];
		if (old)
			GetGame().ObjectDelete(old);
		s_Sleepers[bed] = null;
		s_CageFull = full;
		bool open = AZ_VampCryptCage.IsDoorOpen();
		vector at = AZ_VampCryptCage.DoorOutside();
		if (open)
			at = AZ_VampCryptCage.DoorInside();
		else if (!full)
			at = AZ_VampCryptCage.CirclePoint(0);
		PlayerBase vamp = Dress(SafeFloor(at, at[1]), 0, bed);
		if (!vamp)
		{
			s_Sleepers[bed] = Sleep(bed);
			s_Busy = false;
			return;
		}
		vamp.SetHealth("", "Blood", 2500);
		s_CageVamp = vamp;
		Print("[AZ_Vamp] crypt lurker " + bed + " at the cage for " + AZ_VampState.Uid(p) + ", full " + full + " door open " + open);
		if (open)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageBite, STEP_IN_MS, false, p, bed);
			return;
		}
		if (full)
		{
			CageAtDoor(p, bed);
			return;
		}
		CageCircle(p, bed, 0);
	}


	static void CageCircle(PlayerBase p, int bed, int step)
	{
		PlayerBase vamp = s_CageVamp;
		if (!vamp)
			return;
		if (step > CIRCLE_STEPS)
		{
			CageAtDoor(p, bed);
			return;
		}
		vector at = AZ_VampCryptCage.CirclePoint((step * 1.0) / CIRCLE_STEPS);
		vamp.SetPosition(SafeFloor(at, at[1]));
		if (p)
		{
			vector look = p.GetPosition() - vamp.GetPosition();
			look[1] = 0;
			vamp.SetDirection(look.Normalized());
		}
		if (step == CIRCLE_STEPS / 2 && p)
			Say(p, "AZ_VampWhisperShhh_SoundSet", vamp.GetPosition() + "0 1.6 0");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageCircle, CIRCLE_MS / CIRCLE_STEPS, false, p, bed, step + 1);
	}

	static void CageAtDoor(PlayerBase p, int bed)
	{
		PlayerBase vamp = s_CageVamp;
		if (!vamp)
			return;
		vector door = AZ_VampCryptCage.DoorOutside();
		vamp.SetPosition(SafeFloor(door, door[1]));
		vamp.SetDirection(AZ_VampCryptCage.InwardDir());
		vamp.m_AZCageOpenReq = true;
		vamp.m_AZCageOpenTries = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageOpened, OPEN_MS, false, p, bed);
	}

	static void CageOpened(PlayerBase p, int bed)
	{
		PlayerBase vamp = s_CageVamp;
		if (!vamp || !p || !p.IsAlive() || !AZ_VampCryptCage.HoldsHuman(p) || p.m_AZVampHeld)
		{

			Print("[AZ_Vamp] crypt lurker at the cage: the victim is gone, it withdraws");
			if (vamp)
				GetGame().ObjectDelete(vamp);
			s_CageVamp = null;
			s_Busy = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Wake, COOLDOWN_MS, false, bed, s_Gen);
			return;
		}
		vamp.m_AZCageOpenEnd = true;
		AZ_VampCryptCage.OpenDoor("a crypt lurker", !s_CageFull);
		vector inside = AZ_VampCryptCage.DoorInside();
		vamp.SetPosition(SafeFloor(inside, inside[1]));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CageBite, STEP_IN_MS, false, p, bed);
	}

	static void CageBite(PlayerBase p, int bed)
	{
		PlayerBase vamp = s_CageVamp;
		s_CageVamp = null;
		if (s_CageFull)
			s_CageFeed = true;
		else
		{
			s_CageTaste = true;
			if (p)
				s_TasteFrom = p.GetHealth("", "Blood");
		}
		StrikeWith(p, bed, vamp, s_CageFull);
	}

	static int Nearest(vector pos)
	{
		int best = -1;
		float bd = 1e9;
		for (int i = 0; i < s_Sleepers.Count(); i++)
		{
			if (!s_Sleepers[i])
				continue;
			vector b = s_Beds[i];
			float d = vector.Distance(AZ_VampCrypt.Hall(b[0], b[1], s_BedY[i]), pos);
			if (d < bd)
			{
				bd = d;
				best = i;
			}
		}
		return best;
	}



	static void Strike(PlayerBase p, int bed)
	{
		StrikeWith(p, bed, null, true);
	}


	static void StrikeWith(PlayerBase p, int bed, PlayerBase vamp, bool full)
	{
		if (!p || !p.IsAlive())
		{
			if (vamp)
				GetGame().ObjectDelete(vamp);
			s_CageFeed = false;
			s_CageTaste = false;
			s_Busy = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Wake, COOLDOWN_MS, false, bed, s_Gen);
			return;
		}
		PlayerBase old = s_Sleepers[bed];
		if (old)
			GetGame().ObjectDelete(old);
		s_Sleepers[bed] = null;
		vector dir = p.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector back = p.GetPosition() - dir * AZ_VampFeed.ROOT_GAP;
		back[1] = p.GetPosition()[1];
		if (vamp)
			vamp.SetPosition(back);
		else
			vamp = Dress(back, 0, bed);
		if (!vamp)
		{
			s_Sleepers[bed] = Sleep(bed);
			s_Busy = false;
			return;
		}
		vamp.SetDirection(dir);
		vamp.SetHealth("", "Blood", 2500);
		ActionAZVampBite.SnapJoin(vamp, p, true);
		PlayerBase elder;
		AZ_VampFeed.Hold(vamp, p);
		AZ_VampFeedAnim.DummyFeedStart(vamp, true);
		int serial = NextBite(p);
		if (!full)
		{
			Print("[AZ_Vamp] crypt lurker takes a taste of " + AZ_VampState.Uid(p) + " from behind");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TasteEnd, TASTE_MS, false, vamp, p, bed, serial);
			return;
		}
		if (s_ProbeElder || Math.RandomFloat01() < ELDER_CHANCE)
		{
			vector front = p.GetPosition() + dir * AZ_VampFeed.ROOT_GAP;
			elder = Dress(front, 0, bed + 5);
			if (elder)
			{
				elder.SetHealth("", "Blood", 2500);
				elder.SetDirection(-dir);
				ActionAZVampBite.SnapJoin(elder, p, false);
				AZ_VampFeed.Join(elder, p);
				AZ_VampFeedAnim.DummyFeedStart(elder, false);
			}
		}
		Print("[AZ_Vamp] crypt lurker bites " + AZ_VampState.Uid(p) + " from behind" + ElderNote(elder));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Collapse, HOLD_MS, false, vamp, elder, p, bed, serial);
	}

	// F02: a bite's delayed end must still own its victim when it fires: the lurker alive and awake, the hold still
	// its own (a rescue releases it), and no newer bite on that victim since. The actor cleanup runs either way
	static int s_BiteSerial;
	static ref map<string, int> s_BiteSerialOf;

	static int NextBite(PlayerBase p)
	{
		if (!s_BiteSerialOf)
			s_BiteSerialOf = new map<string, int>;
		s_BiteSerial++;
		s_BiteSerialOf.Set(AZ_VampState.Uid(p), s_BiteSerial);
		return s_BiteSerial;
	}

	static bool BiteOwned(PlayerBase vamp, PlayerBase p, int serial)
	{
		if (!vamp || !vamp.IsAlive() || vamp.IsUnconscious() || !p || !p.m_AZVampHeld || (p.m_AZVampFeeder != vamp && p.m_AZVampFeeder2 != vamp))
			return false;
		return s_BiteSerialOf && s_BiteSerialOf.Get(AZ_VampState.Uid(p)) == serial;
	}

	static string ElderNote(PlayerBase elder)
	{
		if (elder)
			return ", an elder from the front";
		return "";
	}

	static void Collapse(PlayerBase vamp, PlayerBase elder, PlayerBase p, int bed, int serial = 0)
	{
		if (p && p.IsAlive() && !BiteOwned(vamp, p, serial))
			Print("[AZ_Vamp] crypt lurker bite on " + AZ_VampState.Uid(p) + " was broken before its end: no collapse");
		else if (p && p.IsAlive())
		{
			AZ_VampEvidence.Begin("lurker_collapse", vamp, p, false);
			p.m_AZVampCollapsed = true;
			p.SetHealth("", "Shock", 0);
			AZ_VampEvidence.End("lurker_collapse", vamp, p);
			AZ_VampConfig cfg = AZ_VampState.Cfg();
			if (cfg && cfg.cryptLurkersTurn && !p.m_AZVampTurning && AZ_VampState.CapAllowsTurn())
				AZ_VampTurn.StartTurning(p, -1, "crypt_lurker");
			AZ_VampFeed.Release(vamp, p);
			Print("[AZ_Vamp] crypt lurker done with " + AZ_VampState.Uid(p) + ": unconscious " + p.IsUnconscious() + " shock " + p.GetHealth("", "Shock") + " turning " + p.m_AZVampTurning);
		}
		if (s_CageFeed)
		{
			s_CageFeed = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptCage.Rise, CAGE_LIFT_MS, false, "a lurker fed");
		}
		AZ_VampFeedAnim.DummyFeedEnd(vamp);
		if (elder)
			AZ_VampFeedAnim.DummyFeedEnd(elder);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Fade, FADE_MS, false, vamp, elder, bed);
	}

	static void Fade(PlayerBase vamp, PlayerBase elder, int bed)
	{
		if (vamp)
			GetGame().ObjectDelete(vamp);
		if (elder)
			GetGame().ObjectDelete(elder);
		s_Busy = false;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Wake, COOLDOWN_MS, false, bed, s_Gen);
	}


	static void Wake(int bed, int gen)
	{
		if (gen != s_Gen || !s_Sleepers || bed >= s_Sleepers.Count() || s_Sleepers[bed])
			return;
		s_Sleepers[bed] = Sleep(bed);
	}










	static const int RIVER_DRAIN_MS = 27000;
	static const int RIVER_CAP_MS = 33000;
	static const int RIVER_DRAIN_STEP_MS = 500;
	static const float RIVER_BLOOD_FLOOR = 3400;
	static const int RIVER_WAKE_MS = 4000;
	static float s_RiverBlood0;
	static bool s_RiverEnough;
	static bool s_RiverForced;
	static const int RIVER_ENOUGH_MS = 25500;
	static const string ENOUGH_SET = "AZ_VampLine_enough_SoundSet";

	static bool s_RiverWakeArmed;
	static string s_RiverWakeUid;
	static string s_RiverWakeLine;
	static const int RIVER_FADE_MS = 4000;
	static ref array<PlayerBase> s_River;



	static const int FEED_MUSIC = 5;
	static const int WAKE_MUSIC = 7;
	static const int WAKE_MUSIC_MS = 40000;
	static AZ_VampRitualSite s_RiverSite;
	static const int RPC_BEAT = 4702248;
	static const string BEAT_SET = "AZ_VampVictimBeat_SoundSet";
	static const string BEAT_WEAK_SET = "AZ_VampVictimBeatWeak_SoundSet";
	static const float BEAT_WEAK_BPM = 70;
	static const int BEAT_RISE_MS = 2500;
	static const int BEAT_MAX_MS = 36000;
	static const float BEAT_REST_BPM = 72;


	static float BeatBpm(int t)
	{
		if (t < BEAT_RISE_MS)
			return 75 + 55.0 * t / BEAT_RISE_MS;
		float k = Math.Clamp((t - BEAT_RISE_MS) * 1.0 / (RIVER_DRAIN_MS - BEAT_RISE_MS), 0, 1);
		return 130 - 90 * k;
	}


	static void Beat(PlayerBase p, int mode)
	{
		if (p && p.GetIdentity())
			GetGame().RPCSingleParam(p, RPC_BEAT, new Param1<int>(mode), true, p.GetIdentity());
	}

















	static const int RIVER_ARRIVE_MS = 4500;
	static const int RIVER_CIRCLE_MS = 6000;
	static const int RIVER_APPROACH_MS = 3000;
	static const int RIVER_TASTE_MS = 4000;
	static const int RIVER_DASH_MS = 300;
	static const float RIVER_CIRCLE_R = 1.4;
	static const float RIVER_ENTRY_R = 2.2;
	static const float RIVER_CLOSE_R = 1.1;
	static const int RIVER_A_AT = 12500;
	static const int RIVER_A_TASTE = 19300;
	static const int RIVER_A_HOME = 23400;
	static const int RIVER_B_AT = 31700;
	static const int RIVER_B_TASTE = 35200;
	static const int RIVER_B_HOME = 39300;
	static const int RIVER_CLOSE_AT = 41500;
	static const int RIVER_PLAY_MS = 45800;
	static const float RIVER_TASTE_ML = 300;
	static float s_RiverTasteFrom;
	static int s_RiverPinUntil;
	static ref array<vector> s_RiverHome;






	static const float RIVER_SPOT_F = 50.0;

	static vector RiverSpot()
	{
		return AZ_VampCrypt.Nave(RIVER_SPOT_F, 0, -1.0);
	}

	static vector RiverFace()
	{
		vector f = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 0) - RiverSpot();
		f[1] = 0;
		return f.Normalized();
	}




	static const int KNEEL_MS = 4000;

	static void RiverKneel(PlayerBase p)
	{
		if (!p)
			return;
		array<PlayerBase> all = new array<PlayerBase>;
		if (s_Audience)
			all.InsertAll(s_Audience);
		if (s_River)
			all.InsertAll(s_River);
		int n = 0;
		foreach (PlayerBase e : all)
		{
			if (!e || !e.IsAlive() || e.GetCommand_Action())
				continue;
			vector look = p.GetPosition() - e.GetPosition();
			look[1] = 0;
			if (look.Length() > 0.1)
				e.SetDirection(look.Normalized());
			HumanCommandActionCallback cb = e.StartCommand_Action(DayZPlayerConstants.CMD_GESTUREFB_KNEEL, AZ_LurkerLieCB, DayZPlayerConstants.STANCEMASK_CROUCH);
			if (cb)
				n++;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverKneelEnd, KNEEL_MS, false, e);
		}
		Print("[AZ_Vamp] ritual river feed: " + n + " of " + all.Count() + " elders kneel to the new one");
	}

	static void RiverKneelEnd(PlayerBase e)
	{
		if (!e || !e.IsAlive())
			return;
		HumanCommandActionCallback cb = e.GetCommand_Action();
		if (cb)
			cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
	}

	static int RiverTotalMs()
	{
		return RIVER_PLAY_MS + RIVER_DRAIN_MS + StareMs();
	}





	static const int STARE_AFTER_ARRIVE_MS = 1500;

	static int StareMs()
	{
		if (!AZ_VampState.Cfg())
			return 8000;
		return Math.Clamp(AZ_VampState.Cfg().riverStareS, 0, 30) * 1000;
	}

	static void RiverStare(PlayerBase p)
	{
		if (!p)
			return;
		array<PlayerBase> all = new array<PlayerBase>;
		if (s_Audience)
			all.InsertAll(s_Audience);
		if (s_River)
			all.InsertAll(s_River);
		int n = 0;
		foreach (PlayerBase e : all)
		{
			if (!e || !e.IsAlive())
				continue;
			vector look = RiverVictimPos(p) - e.GetPosition();
			look[1] = 0;
			if (!RiverFinite(look) || look.Length() < 0.1)
				continue;
			e.SetDirection(look.Normalized());
			n++;
		}

		vector face = StatueFace();
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, PlayerBase.RPC_STARE, new Param2<vector, int>(face, StareMs()), true, p.GetIdentity());


		GetGame().RPCSingleParam(p, PlayerBase.RPC_EMBERS, new Param1<int>(StareMs()), true);

		int held = 0;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		foreach (PlayerBase h : online)
		{
			if (!h || h == p || !h.IsAlive() || h.m_AZVampire || !h.GetIdentity())
				continue;
			if (!AZ_VampCrypt.Inside(h.GetPosition()))
				continue;
			GetGame().RPCSingleParam(h, PlayerBase.RPC_COMPEL, new Param2<vector, int>(face, StareMs()), true, h.GetIdentity());
			held++;
		}
		Print("[AZ_Vamp] ritual river feed: the stare compels " + held + " more humans in the hall");
		Print("[AZ_Vamp] ritual river feed: the stare, " + n + " elders turned to him for " + StareMs() + " ms, his eyes drawn to her face " + face);
	}


	static vector StatueFace()
	{
		vector at = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, AZ_VampCrypt.CountessFeet());
		float h = 2.4;
		if (AZ_VampCrypt.s_Countess)
		{
			vector mm[2];
			AZ_VampCrypt.s_Countess.ClippingInfo(mm);
			vector mx = mm[1];
			float top = mx[1];
			if (top > 1.0)
				h = top - 0.3;
		}
		at[1] = at[1] + h;
		return at;
	}


	static int RiverSafetyMs()
	{
		return RIVER_PLAY_MS + RIVER_CAP_MS + 5000 + StareMs();
	}





	static const float DAIS_GAP_M = 0.7;
	static ref array<vector> s_DaisFree;
	static ref array<vector> s_DaisUsed;

	static string DaisBlocked(vector at, array<Object> skip)
	{
		vector c = at + Vector(0, 0.95, 0);
		array<Object> hit = new array<Object>;
		if (!GetGame().IsBoxCollidingGeometry(c, vector.Zero, Vector(0.8, 1.7, 0.8), ObjIntersectGeom, ObjIntersectGeom, skip, hit))
			return "";


		foreach (Object o : hit)
		{
			if (o && !ItemBase.Cast(o))
				return o.GetType();
		}
		return "";
	}

	static void DaisBuild()
	{
		s_DaisFree = new array<vector>;
		s_DaisUsed = new array<vector>;
		array<float> fr = {60.0, -1.5, 60.0, 1.5, 59.4, -2.6, 59.4, 2.6, 58.4, -2.8, 58.4, 2.8, 60.4, -2.4, 60.4, 2.4, 58.9, -3.4, 58.9, 3.4, 58.0, -2.2, 58.0, 2.2, 57.6, -3.0, 57.6, 3.0};
		for (int i = 0; i + 1 < fr.Count(); i += 2)
		{
			vector at = AZ_VampCrypt.Nave(fr[i], fr[i + 1], AZ_VampCrypt.DAIS_Y);
			string near = DaisBlocked(at, null);
			Print("[AZ_Vamp] crypt dais spot f " + fr[i] + " r " + fr[i + 1] + " clear " + (near == "") + " nearest " + near);
			if (near == "")
				s_DaisFree.Insert(at);
		}
		Print("[AZ_Vamp] crypt dais: " + s_DaisFree.Count() + " free spots");
	}


	static void DaisTake(array<vector> dest, int n, string role)
	{
		if (!s_DaisFree)
			DaisBuild();
		foreach (vector c : s_DaisFree)
		{
			if (n <= 0)
				return;
			bool close = false;
			foreach (vector u : s_DaisUsed)
			{
				if (vector.Distance(u, c) < DAIS_GAP_M)
					close = true;
			}
			if (close)
				continue;
			s_DaisUsed.Insert(c);
			dest.Insert(c);
			n--;
			Print("[AZ_Vamp] crypt dais " + role + " placed at hall " + AZ_VampCrypt.HallLocal(c));
		}
		if (n > 0)
			Print("[AZ_Vamp] crypt dais: " + n + " " + role + "(s) without a free spot");
	}


	static void RiverFeeders(vector at)
	{
		s_River = new array<PlayerBase>;
		s_RiverHome = new array<vector>;
		array<vector> spots = new array<vector>;


		DaisTake(spots, 2, "feeder");
		for (int i = 0; i < spots.Count(); i++)
		{
			vector sp = spots[i];
			vector home = sp;
			PlayerBase v = Dress(home, 0, 10 + 2 * i);
			if (!v)
				continue;
			v.SetHealth("", "Blood", 2500);
			vector look = at - home;
			look[1] = 0;
			v.SetDirection(look.Normalized());
			s_River.Insert(v);
			AZ_VampElderWrath.Smoke(v);
			s_RiverHome.Insert(home);
		}
	}

	static PlayerBase s_RiverVictim;
	static int s_RiverStartAt;


	static void RiverMeasure(string tag)
	{
		if (!s_River || !s_RiverVictim)
		{
			Print("[AZ_Vamp] river measure " + tag + ": no feed");
			return;
		}
		Print("[AZ_Vamp] river measure " + tag + ": victim at hall " + AZ_VampCrypt.HallLocal(s_RiverVictim.GetPosition()) + " unconscious " + s_RiverVictim.IsUnconscious());
		foreach (PlayerBase v : s_River)
		{
			if (v)
				AZ_VampProbe.Measure(v, s_RiverVictim, 0);
		}
	}

	static void RiverFeed(PlayerBase p, vector at, vector face)
	{
		if (!p || !p.IsAlive())
			return;
		s_RiverVictim = p;
		s_RiverStartAt = GetGame().GetTime();
		face[1] = 0;
		face.Normalize();
		AZ_VampSnap.AllowMove(p);
		p.SetPosition(at);
		p.SetDirection(face);
		s_RiverAt = at;
		s_RiverFace = face;
		int st = StareMs();
		s_RiverPinUntil = GetGame().GetTime() + RIVER_PLAY_MS + RIVER_CAP_MS + st;
		RiverPin(p, at);
		if (AZ_VampRitual.s_Site)
			s_RiverSite = AZ_VampRitual.s_Site;
		Beat(p, 1);
		Print("[AZ_Vamp] ritual river feed on " + AZ_VampState.Uid(p) + " at " + at + ": alone first, the elders at +" + RIVER_ARRIVE_MS + " ms");
		ScriptCallQueue q = GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.CallLater(RiverArrive, RIVER_ARRIVE_MS, false, p);
		q.CallLater(RiverLaugh, RIVER_ARRIVE_MS + 1500, false, p);
		if (st > 0)
			q.CallLater(RiverStare, RIVER_ARRIVE_MS + STARE_AFTER_ARRIVE_MS, false, p);



		q.CallLater(AppearFeed, RIVER_A_TASTE - 650 + st, false, 0, p, true);
		q.CallLater(RiverVocalI, RIVER_A_TASTE - 300 + st, false, p, 0, "AZ_VampVocal_scent_SoundSet", SCENT_SEX);
		q.CallLater(RiverTasteI, RIVER_A_TASTE + st, false, 0, p, true);
		q.CallLater(RiverTasteEndI, RIVER_A_HOME - 100 + st, false, 0, p);
		q.CallLater(RiverVocalI, RIVER_A_HOME - 50 + st, false, p, 0, "AZ_VampVocal_sated_SoundSet", SATED_SEX);
		q.CallLater(DashHome, RIVER_A_HOME + st, false, 0, at);
		q.CallLater(AppearFeed, RIVER_B_TASTE - 650 + st, false, 1, p, false);
		q.CallLater(RiverVocalI, RIVER_B_TASTE - 300 + st, false, p, 1, "AZ_VampVocal_hiss_SoundSet", HISS_SEX);
		q.CallLater(RiverTasteI, RIVER_B_TASTE + st, false, 1, p, false);
		q.CallLater(RiverTasteEndI, RIVER_B_HOME - 100 + st, false, 1, p);
		q.CallLater(DashHome, RIVER_B_HOME + st, false, 1, at);
		q.CallLater(AppearFeed, RIVER_PLAY_MS - 800 + st, false, 0, p, true);
		q.CallLater(AppearFeed, RIVER_PLAY_MS - 500 + st, false, 1, p, false);
		q.CallLater(RiverBiteI, RIVER_PLAY_MS + st, false, p);
		Print("[AZ_Vamp] ritual river feed: the stare " + st + " ms, A at +" + (RIVER_A_TASTE + st) + ", B at +" + (RIVER_B_TASTE + st) + ", the full bite at +" + (RIVER_PLAY_MS + st) + " ms");
	}

	static vector s_RiverAt;
	static vector s_RiverFace;


	static vector RiverRing(float a, float r)
	{
		vector right = Vector(s_RiverFace[2], 0, -s_RiverFace[0]);
		vector pos = s_RiverAt + (s_RiverFace * Math.Cos(a) + right * Math.Sin(a)) * r;
		pos[1] = s_RiverAt[1];
		return pos;
	}

	static PlayerBase Feeder(int i)
	{
		if (!s_River || i >= s_River.Count())
			return null;
		return s_River[i];
	}


	static void RiverArrive(PlayerBase p)
	{
		if (s_DaisUsed)
			s_DaisUsed.Clear();
		if (!s_River || s_River.Count() < 2)
			RiverAudience(s_RiverAt);
		array<PlayerBase> all = new array<PlayerBase>;
		if (s_Audience)
			all.InsertAll(s_Audience);
		if (s_River)
			all.InsertAll(s_River);
		foreach (PlayerBase e : all)
		{
			if (e)
				GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(e.GetPosition()), true);
		}
		if (s_RiverSite)
			AZ_VampRitual.SetMusic(s_RiverSite, FEED_MUSIC);
		if (p && p.IsAlive())
			AZ_VampWhisperLines.Circle(p, s_RiverAt, s_RiverFace, 3);
		Print("[AZ_Vamp] ritual river feed: " + all.Count() + " elders appear on the banks");
	}


	static void AppearFeed(int i, PlayerBase p, bool rear)
	{
		PlayerBase v = Feeder(i);
		if (!v || !v.IsAlive() || !p)
			return;
		vector at = RiverVictimPos(p);
		vector dir = RiverVictimDir(p);
		vector land = at + dir * AZ_VampFeed.ROOT_GAP;
		if (rear)
			land = at - dir * AZ_VampFeed.ROOT_GAP;
		GetGame().RPCSingleParam(v, AZ_VampShadow.RPC_STREAK, new Param2<vector, vector>(v.GetPosition(), land), true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AppearLand, SWISH_MS, false, i, p, rear);
		Print("[AZ_Vamp] ritual river feed: feeder " + i + " swishes to its bite pose, rear " + rear);
	}

	static void AppearLand(int i, PlayerBase p, bool rear)
	{
		PlayerBase v = Feeder(i);
		if (!v || !v.IsAlive() || !p)
			return;
		RiverSnap(v, p, rear);
		LevelWith(v, p);
		GetGame().RPCSingleParam(v, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(v.GetPosition()), true);
	}

	static void DashRing(int i, float a, float r)
	{
		PlayerBase v = Feeder(i);
		if (v)
			Dash(v, RiverRing(a, r), s_RiverAt);
	}


	static void GlideI(int i, float a0, float a1, float r0, float r1, int ms, int n)
	{
		PlayerBase v = Feeder(i);
		if (!v || !v.IsAlive())
			return;
		int steps = Math.Max(1, ms / 100);
		float k = Math.Clamp(n * 1.0 / steps, 0, 1);
		vector pos = RiverRing(a0 + (a1 - a0) * k, r0 + (r1 - r0) * k);
		v.SetPosition(pos);
		vector look = s_RiverAt - pos;
		look[1] = 0;
		if (look.Length() > 0.05)
			v.SetDirection(look.Normalized());
		if (n < steps)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GlideI, 100, false, i, a0, a1, r0, r1, ms, n + 1);
	}

	static void RiverVocalI(PlayerBase p, int i, string soundSet, string sex)
	{
		RiverVocal(p, Feeder(i), soundSet, sex);
	}

	static void RiverTasteI(int i, PlayerBase p, bool rear)
	{
		RiverTaste(Feeder(i), p, rear);
	}

	static void RiverTasteEndI(int i, PlayerBase p)
	{
		RiverTasteEnd(Feeder(i), p);
	}

	static void RiverBiteI(PlayerBase p)
	{
		RiverBite(p, Feeder(0), Feeder(1));
	}



	static void Voice(PlayerBase p, string soundSet, string who, string text, vector at)
	{
		if (!p || !p.IsAlive())
			return;
		if (GetGame().ConfigIsExisting("CfgSoundSets " + soundSet))
			Say(p, soundSet, at);
		else
			AZ_VampRitual.Note(p, who, text);
	}







	static const string LAUGH_SEX = "m";
	static const string SCENT_SEX = "f";
	static const string SATED_SEX = "f";
	static const string HISS_SEX = "f";
	static const string ENOUGH_SEX = "m";


	static PlayerBase Speaker(PlayerBase p, string sex)
	{
		array<PlayerBase> all = new array<PlayerBase>;
		if (s_River)
			all.InsertAll(s_River);
		if (s_Audience)
			all.InsertAll(s_Audience);
		PlayerBase best = null;
		float bestD = 1000;
		foreach (PlayerBase e : all)
		{
			if (!e || !e.IsAlive())
				continue;
			if (sex == "m" && !e.IsMale())
				continue;
			if (sex == "f" && e.IsMale())
				continue;
			float d = vector.Distance(e.GetPosition(), p.GetPosition());
			if (d < bestD)
			{
				bestD = d;
				best = e;
			}
		}
		return best;
	}


	static vector SpeakerAt(PlayerBase p, string sex)
	{
		PlayerBase e = Speaker(p, sex);
		if (e)
			return Carry(p, e.GetPosition() + Vector(0, 1.6, 0));
		return p.GetPosition() + Vector(0, 1.6, 0);
	}



	static const float VOICE_CARRY_M = 6.0;

	static vector Carry(PlayerBase p, vector at)
	{
		vector head = p.GetPosition() + Vector(0, 1.6, 0);
		vector d = at - head;
		if (d.Length() <= VOICE_CARRY_M)
			return at;
		return head + d.Normalized() * VOICE_CARRY_M;
	}



	static void RiverVocal(PlayerBase p, PlayerBase v, string soundSet, string sex)
	{
		if (!p || !p.IsAlive() || !GetGame().ConfigIsExisting("CfgSoundSets " + soundSet))
			return;
		vector at = SpeakerAt(p, sex);
		if (v && v.IsAlive() && (sex == "any" || (sex == "m") == v.IsMale()))
			at = Carry(p, v.GetPosition() + Vector(0, 1.6, 0));
		Say(p, soundSet, at);
	}


	static void RiverLaugh(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		if (GetGame().ConfigIsExisting("CfgSoundSets AZ_VampVocal_laugh_SoundSet"))
		{
			PlayerBase e = null;
			if (s_Audience && s_Audience.Count() > 0)
				e = s_Audience[0];
			RiverVocal(p, e, "AZ_VampVocal_laugh_SoundSet", LAUGH_SEX);
			return;
		}
		if (s_Audience && s_Audience.Count() > 0 && s_Audience[0])
			Say(p, "AZ_VampLurkLaugh_SoundSet", s_Audience[0].GetPosition() + Vector(0, 1.6, 0));
	}





	static const int SWISH_MS = 350;

	static void Dash(PlayerBase v, vector to, vector victimAt)
	{
		if (!v || !v.IsAlive())
			return;
		vector from = v.GetPosition();
		Print("[AZ_Vamp] crypt dash " + v.GetType() + " hall " + AZ_VampCrypt.HallLocal(from) + " -> " + AZ_VampCrypt.HallLocal(to));
		GetGame().RPCSingleParam(v, AZ_VampShadow.RPC_STREAK, new Param2<vector, vector>(from, to), true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DashStep, SWISH_MS, false, v, from, to, victimAt, 3);
	}

	static void DashStep(PlayerBase v, vector from, vector to, vector victimAt, int n)
	{
		if (!v)
			return;
		float k = n / 3.0;
		vector pos = from + (to - from) * k;
		v.SetPosition(pos);
		if (n < 3)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DashStep, RIVER_DASH_MS / 3, false, v, from, to, victimAt, n + 1);
			return;
		}
		vector look = victimAt - to;
		look[1] = 0;
		if (look.Length() > 0.05)
			v.SetDirection(look.Normalized());
		GetGame().RPCSingleParam(v, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(to), true);
	}


	static void DashHome(int i, vector victimAt)
	{
		if (!s_River || !s_RiverHome || i >= s_River.Count() || i >= s_RiverHome.Count())
			return;
		Dash(s_River[i], s_RiverHome[i], victimAt);
	}


	static void RiverPin(PlayerBase p, vector at)
	{
		if (!p || !p.IsAlive() || GetGame().GetTime() > s_RiverPinUntil)
			return;
		vector have = p.GetPosition();
		if (!RiverFinite(have) && RiverFinite(at))
		{
			Print("[AZ_Vamp] river guard: victim position " + have + " is not finite, put back on " + at);
			AZ_VampSnap.AllowMove(p);
			p.SetPosition(at);
		}
		else if (vector.Distance(have, at) > 0.35 && !p.m_AZVampHeld)
		{
			AZ_VampSnap.AllowMove(p);
			p.SetPosition(at);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverPin, 300, false, p, at);
	}


	static void LevelWith(PlayerBase v, PlayerBase p)
	{
		if (!v || !p)
			return;
		vector pos = v.GetPosition();
		float py = RiverVictimPos(p)[1];
		float dy = pos[1] - py;
		pos[1] = py;
		if (!RiverFinite(pos))
		{
			Print("[AZ_Vamp] river guard: level skipped, bad feeder position " + pos);
			return;
		}
		v.SetPosition(pos);
		Print("[AZ_Vamp] ritual river feed: feeder levelled with the victim (was " + dy + " m off)");
	}

	static void RiverTaste(PlayerBase v, PlayerBase p, bool rear)
	{
		if (!v || !p || !p.IsAlive())
			return;
		s_RiverTasteFrom = p.GetHealth("", "Blood");
		RiverSnap(v, p, rear);
		LevelWith(v, p);
		AZ_VampFeed.Hold(v, p);
		AZ_VampFeedAnim.DummyFeedStart(v, rear);
		Print("[AZ_Vamp] ritual river feed: a taste, rear " + rear);
	}

	static void RiverTasteEnd(PlayerBase v, PlayerBase p)
	{
		if (p && p.IsAlive())
		{
			AZ_VampEvidence.Begin("river_taste", v, p, false);
			float target = Math.Max(s_RiverTasteFrom - RIVER_TASTE_ML, 3500);
			if (p.GetHealth("", "Blood") > target)
				p.SetHealth("", "Blood", target);
			AZ_VampEvidence.End("river_taste", v, p);
			AZ_VampFeed.Release(v, p);
		}
		if (v)
			AZ_VampFeedAnim.DummyFeedEnd(v);
	}


	static void RiverBite(PlayerBase p, PlayerBase rear, PlayerBase front)
	{
		if (!p || !p.IsAlive())
			return;
		if (rear)
		{
			RiverSnap(rear, p, true);
			LevelWith(rear, p);
			AZ_VampFeed.Hold(rear, p);
			AZ_VampFeedAnim.DummyFeedStart(rear, true);
		}
		if (front)
		{
			RiverSnap(front, p, false);
			LevelWith(front, p);
			AZ_VampFeed.Join(front, p);
			AZ_VampFeedAnim.DummyFeedStart(front, false);
		}
		s_RiverBiteAt = GetGame().GetTime();
		s_RiverBlood0 = p.GetHealth("", "Blood");
		s_RiverEnough = false;
		s_RiverForced = false;
		Beat(p, 2);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverKeep, RIVER_KEEP_MS, false, p);
		Print("[AZ_Vamp] ritual river feed: the full bite on " + AZ_VampState.Uid(p) + ", until they go under; the pulse hands over to the heart");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverDrain, RIVER_DRAIN_STEP_MS, false, p);
	}


	static void RiverDrain(PlayerBase p)
	{
		if (!p || !p.IsAlive())
		{
			RiverDone(p);
			return;
		}
		int t = GetGame().GetTime() - s_RiverBiteAt;
		float k = Math.Clamp((t * 1.0) / RIVER_DRAIN_MS, 0, 1);
		p.m_AZVampFloorAt = GetGame().GetTime() + 60000;
		float shock = 100 * (1 - k);
		if (p.GetHealth("", "Shock") > shock)
			p.SetHealth("", "Shock", shock);


		if (!s_RiverEnough && t >= RIVER_ENOUGH_MS)
		{
			s_RiverEnough = true;
			Voice(p, ENOUGH_SET, "An elder", "Enough.", SpeakerAt(p, ENOUGH_SEX));
		}
		float minBlood = RIVER_BLOOD_FLOOR;
		if (AZ_VampState.Cfg())
			minBlood = Math.Max(minBlood, AZ_VampState.Cfg().bloodFloor + 400);
		float blood = Math.Max(minBlood, s_RiverBlood0 - (s_RiverBlood0 - minBlood) * k);
		if (p.GetHealth("", "Blood") > blood)
			p.SetHealth("", "Blood", blood);
		if (p.GetHealth("", "Blood") < minBlood)
			p.SetHealth("", "Blood", minBlood);



		if (t >= RIVER_DRAIN_MS && !p.IsUnconscious() && !s_RiverForced)
		{
			s_RiverForced = true;
			AZ_VampEvidence.Begin("river_blackout", p.m_AZVampFeeder, p, false);
			p.SetHealth("", "Shock", 0);
			DayZPlayerSyncJunctures.SendPlayerUnconsciousness(p, true);
			AZ_VampEvidence.End("river_blackout", p.m_AZVampFeeder, p);
			Print("[AZ_Vamp] ritual river feed: the blackout sent at " + t + " ms (shock " + p.GetHealth("", "Shock") + ")");
		}
		if (p.IsUnconscious() || t >= RIVER_CAP_MS)
		{
			Print("[AZ_Vamp] ritual river feed: under after " + t + " ms, unconscious " + p.IsUnconscious() + " blood " + p.GetHealth("", "Blood"));
			RiverDone(p);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverDrain, RIVER_DRAIN_STEP_MS, false, p);
	}




	static const int RIVER_KEEP_MS = 500;
	static const float RIVER_KEEP_YAW = 20;
	static const float RIVER_KEEP_M = 0.15;



	static const float RIVER_REAR_GAP = 0.62;


	static bool RiverFinite(vector v)
	{
		for (int c = 0; c < 3; c++)
		{
			float x = v[c];
			if (!(x > -100000 && x < 100000))
				return false;
		}
		return true;
	}

	static vector RiverVictimPos(PlayerBase p)
	{
		vector have = p.GetPosition();
		if (RiverFinite(have) || !RiverFinite(s_RiverAt))
			return have;
		Print("[AZ_Vamp] river guard: victim position " + have + " is not finite, using the river spot " + s_RiverAt);
		return s_RiverAt;
	}

	static vector RiverVictimDir(PlayerBase p)
	{
		vector d = p.GetDirection();
		d[1] = 0;
		if (RiverFinite(d) && d.Length() > 0.1)
			return d.Normalized();
		vector f = s_RiverFace;
		f[1] = 0;
		if (RiverFinite(f) && f.Length() > 0.1)
			return f.Normalized();
		return Vector(0, 0, 1);
	}





	static void RiverSnap(PlayerBase v, PlayerBase p, bool rear)
	{
		if (!v || !p)
			return;
		vector at = RiverVictimPos(p);
		vector dir = RiverVictimDir(p);
		vector pos = at + dir * AZ_VampFeed.ROOT_GAP;
		vector face = -dir;
		if (rear)
		{
			pos = at - dir * RIVER_REAR_GAP;
			face = dir;
		}
		pos[1] = at[1];
		if (!RiverFinite(pos) || !RiverFinite(face))
		{
			Print("[AZ_Vamp] river guard: snap skipped, bad transform pos " + pos + " face " + face);
			return;
		}
		v.SetPosition(pos);
		v.SetDirection(face);
	}


	static void RiverCensus()
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(AZ_VampCrypt.Nave(56.0, 0, 0.6), 14, near, null);
		int n = 0;
		foreach (Object o : near)
		{
			PlayerBase f = PlayerBase.Cast(o);
			if (!f)
				continue;
			string role = "other";
			if (s_Audience && s_Audience.Find(f) >= 0)
				role = "watcher";
			if (s_River && s_River.Find(f) >= 0)
				role = "feeder";
			if (f == s_RiverVictim)
				role = "victim";
			if (f.GetIdentity())
				role = "player";
			Print("[AZ_Vamp] river census " + role + " " + f.GetType() + " at hall " + AZ_VampCrypt.HallLocal(f.GetPosition()) + " vampire " + f.m_AZVampire + AZ_VampDress.Worn(f));
			n++;
		}
		Print("[AZ_Vamp] river census: " + n + " figures within 14 m of the dais");
	}

	static int s_RiverKeepLogAt;

	static void RiverKeep(PlayerBase p)
	{
		if (!p || !p.IsAlive() || p.IsUnconscious() || s_RiverDoneAt > s_RiverBiteAt || !p.m_AZVampHeld)
			return;
		int nowK = GetGame().GetTime();
		if (nowK >= s_RiverKeepLogAt)
		{

			s_RiverKeepLogAt = nowK + 1000;
			string line = "[AZ_Vamp] river keep t=" + (nowK - s_RiverBiteAt) + " drift=" + vector.Distance(p.GetPosition(), s_RiverAt);
			vector neck = AZ_VampProbe.BonePos(p, "Neck");
			for (int j = 0; j < 2; j++)
			{
				PlayerBase fj = Feeder(j);
				if (fj)
					line += " f" + j + " headNeck=" + vector.Distance(AZ_VampProbe.BonePos(fj, "Head"), neck) + " yaw=" + Math.AbsFloat(Math.DiffAngle(fj.GetOrientation()[0], p.GetOrientation()[0]));
			}
			Print("" + line);
		}
		for (int i = 0; i < 2; i++)
		{
			PlayerBase v = Feeder(i);
			if (!v || !v.IsAlive())
				continue;
			bool rear = i == 0;
			float want = 0;
			if (!rear)
				want = 180;
			float yaw = Math.AbsFloat(Math.DiffAngle(v.GetOrientation()[0], p.GetOrientation()[0]));
			vector d = v.GetPosition() - p.GetPosition();
			d[1] = 0;
			float gapWant = AZ_VampFeed.ROOT_GAP;
			if (rear)
				gapWant = RIVER_REAR_GAP;
			float gapOff = Math.AbsFloat(d.Length() - gapWant);
			if (Math.AbsFloat(yaw - want) > RIVER_KEEP_YAW || gapOff > RIVER_KEEP_M)
			{
				RiverSnap(v, p, rear);
				vector pos = v.GetPosition();
				pos[1] = p.GetPosition()[1];
				v.SetPosition(pos);
				Print("[AZ_Vamp] ritual river feed: feeder " + i + " back on its spot (yaw " + yaw + " gap off " + gapOff + ")");
			}
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverKeep, RIVER_KEEP_MS, false, p);
	}

	static int s_RiverBiteAt;
	static int s_RiverDoneAt;

	static void RiverDone(PlayerBase p)
	{
		if (s_RiverWakeArmed)
		{
			s_RiverWakeArmed = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampRitual.ElderGone, 500, false, p, null);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverWakeUp, RIVER_WAKE_MS - 300, false, p);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampAltar.Wake, RIVER_WAKE_MS, false, p, s_RiverWakeUid, s_RiverWakeLine);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverKneel, RIVER_WAKE_MS + 600, false, p);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(WakeMusic, RIVER_WAKE_MS, false, s_RiverSite, true);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptExit.OpenAtWake, RIVER_WAKE_MS + AZ_VampCryptExit.OPEN_AFTER_WAKE_MS, false, p);
		}
		PlayerBase feeder;
		Beat(p, 0);
		if (p && p.IsAlive())
		{
			feeder = p.m_AZVampFeeder;
			s_RiverDoneAt = GetGame().GetTime();
			AZ_VampEvidence.Begin("river_done_ko", feeder, p, false);
			p.m_AZVampCollapsed = true;
			p.SetHealth("", "Shock", 0);
			AZ_VampEvidence.End("river_done_ko", feeder, p);
			AZ_VampFeed.Release(feeder, p);
			Print("[AZ_Vamp] ritual river feed done: " + AZ_VampState.Uid(p) + " blood " + p.GetHealth("", "Blood") + " held " + p.m_AZVampHeld);
		}
		if (s_River)
		{
			foreach (PlayerBase v : s_River)
			{
				if (v)
					AZ_VampFeedAnim.DummyFeedEnd(v);
			}
		}


		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DashHome, 1500, false, 0, s_RiverAt);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DashHome, 1900, false, 1, s_RiverAt);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverWithdrawWatch, RIVER_WAKE_MS, false, p, 0);
	}



	static void WakeMusic(AZ_VampRitualSite s, bool on)
	{
		if (!s || (AZ_VampRitual.s_Site == s))
			return;
		if (on)
		{
			AZ_VampRitual.SetMusic(s, WAKE_MUSIC);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(WakeMusic, WAKE_MUSIC_MS, false, s, false);
		}
		else
			AZ_VampRitual.SetMusic(s, 0);
	}



	static void RiverSafety(PlayerBase p)
	{
		if (!s_RiverWakeArmed)
			return;
		Print("[AZ_Vamp] ritual river feed: safety, RiverDone never came");
		RiverDone(p);
	}



	static const int RIVER_STAY_S = 20;
	static const float RIVER_LEAVE_M = 2.5;
	static const int RIVER_WITHDRAW_MS = 1500;




	static void RiverWakeUp(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		p.SetHealth("", "Shock", p.GetMaxHealth("", "Shock"));
		DayZPlayerSyncJunctures.SendPlayerUnconsciousness(p, false);
		Print("[AZ_Vamp] ritual river feed: he comes to (unconscious " + p.IsUnconscious() + ")");
	}

	static void RiverWithdrawWatch(PlayerBase p, int sec)
	{
		bool gone = !p || !p.IsAlive();
		bool moved = false;
		if (!gone && !p.IsUnconscious())
			moved = vector.Distance(p.GetPosition(), s_RiverAt) > RIVER_LEAVE_M;
		if (gone || moved || sec >= RIVER_STAY_S)
		{
			Print("[AZ_Vamp] ritual river feed: the elders withdraw after " + sec + " s of the wake (moved " + moved + ")");
			RiverWithdraw(0);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverWithdrawWatch, 1000, false, p, sec + 1);
	}

	static void RiverVanish(PlayerBase e)
	{
		if (!e)
			return;
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(e.GetPosition()), true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 1500, false, e);
	}

	static void RiverWithdraw(int n)
	{
		array<PlayerBase> order = new array<PlayerBase>;
		if (s_Audience)
			order.InsertAll(s_Audience);
		if (s_River)
		{
			for (int r = s_River.Count() - 1; r >= 0; r--)
				order.Insert(s_River[r]);
		}
		while (n < order.Count() && !order[n])
			n++;
		if (n >= order.Count())
		{
			s_Audience = null;
			s_River = null;
			Print("[AZ_Vamp] ritual river feed: the last elder is gone");
			return;
		}
		PlayerBase e = order[n];

		float side = 0.6;
		if (n % 2 == 1)
			side = -0.6;
		vector behind = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 9.4, side, AZ_VampCrypt.DAIS_Y);
		string near = DaisBlocked(behind, null);
		Print("[AZ_Vamp] crypt dais spot withdraw hall " + AZ_VampCrypt.HallLocal(behind) + " clear " + (near == "") + " nearest " + near);
		if (near == "")
		{
			Dash(e, behind, AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, AZ_VampCrypt.DAIS_Y));
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverVanish, 700, false, e);
		}
		else
			RiverVanish(e);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverWithdraw, RIVER_WITHDRAW_MS, false, n + 1);
	}

	static void RiverFade()
	{
		if (s_Audience)
		{
			foreach (PlayerBase a : s_Audience)
			{
				if (a)
				{
					GetGame().RPCSingleParam(a, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(a.GetPosition()), true);
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 1500, false, a);
				}
			}
			s_Audience = null;
		}
		if (!s_River)
			return;
		foreach (PlayerBase v : s_River)
		{
			if (v)
			{
				GetGame().RPCSingleParam(v, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(v.GetPosition()), true);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 1500, false, v);
			}
		}
		s_River = null;
	}





	static ref array<PlayerBase> s_Audience;

	static void RiverAudience(vector at)
	{
		RiverFeeders(at);
		s_Audience = new array<PlayerBase>;
		array<vector> spots = new array<vector>;






		DaisTake(spots, 4, "watcher");
		for (int i = 0; i < spots.Count(); i++)
		{
			vector sp = spots[i];
			vector stand = sp;
			PlayerBase e = Dress(stand, 0, 14 + i);
			if (!e)
				continue;
			vector look = at - stand;
			look[1] = 0;
			e.SetDirection(look.Normalized());
			s_Audience.Insert(e);
			AZ_VampElderWrath.Smoke(e);
		}
		string where = "";
		foreach (PlayerBase w : s_Audience)
			where += " " + AZ_VampCrypt.HallLocal(w.GetPosition());
		Print("[AZ_Vamp] ritual river feed: " + s_Audience.Count() + " elders watch from the banks, hall f r y:" + where);
	}


	static void RiverProbe(vector at, vector face)
	{
		PlayerBase h = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(21), at, false, true));
		if (!h)
		{
			Print("[AZ_Vamp] river probe: FAIL spawn");
			return;
		}
		AZ_VampDress.Survivor(h, 21);
		s_RiverBiteAt = 0;
		s_RiverDoneAt = 0;
		RiverFeed(h, at, face);
		bool early = !h.m_AZVampHeld && h.GetHealth("", "Shock") > 50;
		Print("[AZ_Vamp] river probe: at the call (the fade is over) -> held " + h.m_AZVampHeld + " shock " + h.GetHealth("", "Shock") + " " + RV(early) + " (expect not yet bitten: they are still rising)");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverProbeCheck, RIVER_PLAY_MS + 2000, false, h, at, 1);
	}

	static string RV(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}

	static void RiverProbeCheck(PlayerBase h, vector at, int step)
	{
		bool ok;
		if (step == 1)
		{
			int near = 0;
			if (s_River)
			{
				foreach (PlayerBase v : s_River)
				{
					if (v && vector.Distance(v.GetPosition(), h.GetPosition()) < 1.2)
						near++;
				}
			}
			ok = h.m_AZVampHeld && h.m_AZVampFeeder && h.m_AZVampFeeder2 && near >= 2 && vector.Distance(h.GetPosition(), at) < 0.8;
			Print("[AZ_Vamp] river probe: 2 s in -> held " + h.m_AZVampHeld + " feeders " + (h.m_AZVampFeeder != null) + "/" + (h.m_AZVampFeeder2 != null) + " in range " + near + " off the spot " + vector.Distance(h.GetPosition(), at) + " m " + RV(ok) + " (expect two feeding, victim on the spot)");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RiverProbeCheck, RIVER_CAP_MS, false, h, at, 2);
			return;
		}
		ok = !h.m_AZVampHeld && h.GetHealth("", "Shock") < 25 && h.GetHealth("", "Blood") < 5000 && !h.m_AZVampTurning && s_RiverBiteAt > 0 && s_RiverDoneAt > s_RiverBiteAt;
		Print("[AZ_Vamp] river probe: after the hold -> held " + h.m_AZVampHeld + " shock " + h.GetHealth("", "Shock") + " blood " + h.GetHealth("", "Blood") + " turning " + h.m_AZVampTurning + " " + RV(ok) + " (expect released, down, drained; the turning is the ritual's Wake)");
		GetGame().ObjectDelete(h);
		Print("[AZ_Vamp] river probe: done");
	}


	static void Reset()
	{
		s_ReadyAt = 0;
		s_Busy = false;
		s_CageFeed = false;
		s_CageTaste = false;
		if (s_Done)
			s_Done.Clear();
	}
}


class AZ_VampLurkerOpenCB : HumanCommandActionCallback
{
}

modded class PlayerBase
{
	bool m_AZCageOpenReq;
	bool m_AZCageOpenEnd;
	int m_AZCageOpenTries;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if ((!m_AZCageOpenReq && !m_AZCageOpenEnd) || !GetGame().IsServer())
			return;
		AZ_VampLurkerOpenCB cb = AZ_VampLurkerOpenCB.Cast(GetCommand_Action());
		if (m_AZCageOpenEnd)
		{
			m_AZCageOpenReq = false;
			m_AZCageOpenEnd = false;
			if (cb)
				cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			return;
		}
		if (cb)
		{
			m_AZCageOpenReq = false;
			return;
		}

		if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_MOVE || GetCommand_Action() || GetCommandModifier_Action())
		{
			m_AZCageOpenTries++;
			if (m_AZCageOpenTries > 120)
			{
				m_AZCageOpenReq = false;
				Print("[AZ_Vamp] crypt lurker: the door pose never started (not in the move command)");
			}
			return;
		}
		m_AZCageOpenReq = false;
		cb = AZ_VampLurkerOpenCB.Cast(StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_INTERACT, AZ_VampLurkerOpenCB, DayZPlayerConstants.STANCEMASK_ERECT));
		string res = "FAILED";
		if (cb)
			res = "ok";
		Print("[AZ_Vamp] crypt lurker door pose " + res);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (!GetGame().IsClient())
			return;
		if (rpc_type == AZ_VampCryptLurkers.RPC_SAY)
		{
			AZ_LurkSay(ctx);
			return;
		}
		if (rpc_type == AZ_VampCryptLurkers.RPC_BEAT)
		{
			Param1<int> beat = new Param1<int>(0);
			if (!ctx.Read(beat))
				return;
			m_AZBeatGen++;
			m_AZBeatMode = beat.param1;
			AZ_FeedBand(beat.param1 > 0);
			if (beat.param1 > 0)
			{
				m_AZBeatFrom = GetGame().GetTime();
				AZ_BeatTick(m_AZBeatGen);
			}
			Print("[AZ_Vamp] victim heartbeat " + beat.param1);
			return;
		}
		if (rpc_type != AZ_VampCryptLurkers.RPC_SHHH)
			return;
		EffectSound shhh = SEffectManager.PlaySoundOnObject("AZ_VampCompel_SoundSet", this);
		if (shhh)
			shhh.SetSoundAutodestroy(true);
	}







	bool m_AZFeedBand;

	bool AZ_FeedBandOn()
	{
		return m_AZFeedBand;
	}

	void AZ_FeedBand(bool on)
	{
		if (on == m_AZFeedBand)
			return;
		m_AZFeedBand = on;
		AZ_PPEFeedBand r = AZ_PPEFeedBand.Get();
		if (r)
		{
			if (on)
				r.SetBand(1);
			else
				r.Stop();
		}
		Print("[AZ_Vamp] river feed band " + on + " (+" + AZ_PPEFeedBand.FEED_EV + " EV, no blood-loss grey)");
	}

	protected int m_AZBeatGen;
	protected int m_AZBeatFrom;
	protected int m_AZBeatMode;

	void AZ_BeatTick(int gen)
	{
		if (gen != m_AZBeatGen || !IsAlive() || IsUnconscious())
			return;
		int t = GetGame().GetTime() - m_AZBeatFrom;
		if (t > AZ_VampCryptLurkers.BEAT_MAX_MS * 3)
			return;
		float bpm = AZ_VampCryptLurkers.BEAT_REST_BPM;
		if (m_AZBeatMode == 2)
		{
			if (t > AZ_VampCryptLurkers.BEAT_MAX_MS)
				return;
			bpm = AZ_VampCryptLurkers.BeatBpm(t);
		}
		string beatSet = AZ_VampCryptLurkers.BEAT_SET;
		if (bpm < AZ_VampCryptLurkers.BEAT_WEAK_BPM && GetGame().ConfigIsExisting("CfgSoundSets " + AZ_VampCryptLurkers.BEAT_WEAK_SET))
			beatSet = AZ_VampCryptLurkers.BEAT_WEAK_SET;
		EffectSound snd;
		if (AZ_VampStareBeat.Playing())
			snd = null;
		else if (GetGame().ConfigIsExisting("CfgSoundSets " + beatSet))
			snd = SEffectManager.PlaySound(beatSet, GetPosition() + Vector(0, 1.6, 0));
		else
			snd = SEffectManager.PlaySoundOnObject("AZ_VampHeartbeat_SoundSet", this);
		if (snd)
			snd.SetSoundAutodestroy(true);
		int gap = 60000 / bpm;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_BeatTick, gap, false, gen);
	}


	void AZ_LurkSay(ParamsReadContext ctx)
	{
		Param2<string, vector> say = new Param2<string, vector>("", Vector(0, 0, 0));
		if (!ctx.Read(say))
			return;
		EffectSound snd = SEffectManager.PlaySound(say.param1, say.param2);
		if (snd)
			snd.SetSoundAutodestroy(true);
	}
}


class AZ_PPEFeedBand extends PPERequester_GameplayBase
{
	static const int L_EXPOSURE = 610;
	static const float FEED_EV = 1.6;

	void SetBand(float k)
	{
		SetTargetValueFloat(PPEExceptions.EXPOSURE, PPEExposureNative.PARAM_INTENSITY, false, FEED_EV * k, L_EXPOSURE, PPOperators.ADD);
	}

	static AZ_PPEFeedBand Get()
	{
		AZ_PPEFeedBand r = AZ_PPEFeedBand.Cast(PPERequesterBank.GetRequester(AZ_PPEFeedBand));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEFeedBand);
			r = AZ_PPEFeedBand.Cast(PPERequesterBank.GetRequester(AZ_PPEFeedBand));
		}
		return r;
	}
}


modded class BloodLoss
{
	bool m_AZFeedCapped;

	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
		if (player && player.AZ_FeedBandOn())
		{
			if (!m_AZFeedCapped)
			{
				m_AZFeedCapped = true;
				if (m_RequesterBloodLoss)
					m_RequesterBloodLoss.Stop();
			}
			m_BloodSet = -1;
			return;
		}
		m_AZFeedCapped = false;
		super.OnUpdateClient(player, deltatime);
	}
}
