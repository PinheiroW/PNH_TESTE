






















class AZ_VampRitualSite
{
	Object m_Altar;
	Object m_Slab;
	bool m_InCrypt;
	vector m_Center;
	vector m_Lie;
	vector m_LieDir;
	vector m_Head;
	vector m_Back;
	vector m_Right;
	float m_HalfLen = 0.9;
	ref array<vector> m_Kneel = new array<vector>;
	ref array<AZ_VampRitualCandle> m_Candles = new array<AZ_VampRitualCandle>;
}

class AZ_VampRitual
{
	static const string CAP_REACHED_TEXT = "The Countess is sated. Too many of her children walk tonight. Return later.";
	static const float SLAB_M = 3.0;
	static const float CANDLE_R = 1.7;
	static const float KNEEL_R = 2.2;
	static const float NEAR_M = 2.9;
	static const float LIE_DRIFT_M = 0.9;
	static const float KNEEL_DRIFT_M = 0.9;
	static const float KNEEL_SNAP_M = 3.5;
	static const float AREA_M = 30;
	static const int TICK_MS = 1000;
	static const int SETTLE_MS = 5000;



	static const int OFFER_HIDE_MS = 3500;
	static const int OFFER_BLACK_MS = 4200;
	static int s_OfferEndAt;



	static void SmokeLine(vector from, vector to)
	{
		vector a = from + Vector(0, 1.0, 0);
		vector b = to + Vector(0, 1.0, 0);
		if (GetGame().ConfigIsExisting("CfgSoundSets AZ_VampSwish_SoundSet"))
		{
			vector mid = (a + b) * 0.5;
			EffectSound w = SEffectManager.PlaySound("AZ_VampSwish_SoundSet", mid);
			if (w)
				w.SetSoundAutodestroy(true);
		}
		for (int i = 0; i < 6; i++)
		{
			float k = i / 5.0;
			int delay = k * 350;
			vector pt = a + (b - a) * k;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SmokePuff, delay, false, pt);
		}
	}

	static void SmokePuff(vector at)
	{
		Particle.PlayInWorld(ParticleList.AZ_VAMP_ASH_BURST, at);
	}


	static string SmokeToggle(string arg)
	{
		if (arg == "on")
			AZ_VampState.Cfg().offerSmoke = true;
		else if (arg == "off")
			AZ_VampState.Cfg().offerSmoke = false;
		Print("[AZ_Vamp] ritual offer smoke " + AZ_VampState.Cfg().offerSmoke);
		return "offer smoke " + AZ_VampState.Cfg().offerSmoke + " (usage: /vamp offersmoke on|off)";
	}
	static const int TELL_MS = 8000;
	static const int BLACK_AT_MS = 2000;
	static const int BLACK_MS = 1000;
	static const int ELDER_SHOW_MS = 3000;
	static const int WAKE_AFTER_BITE_MS = 10000;
	static const int EYES_FADE_MS = 60000;
	static const float ALONE_S = 90;
	static const float ONE_S = 60;
	static const float MANY_S = 40;
	static const int PEAK_MS = 20000;

	static const int MUS_DRONE = 1;
	static const int MUS_PULSE = 2;
	static const int MUS_CHOIR = 4;
	static const int MUS_PEAK = 8;

	static const int ST_SLEEP = 0;
	static const int ST_LIGHTING = 1;
	static const int ST_AWAKE = 2;
	static const int ST_POURED = 3;
	static const int ST_LYING = 4;
	static const int ST_CLIMAX = 5;

	static ref array<ref AZ_VampRitualSite> s_Sites = new array<ref AZ_VampRitualSite>;
	static AZ_VampRitualSite s_Site;
	static int s_State;
	static int s_AnswerGeneration;
	static int s_AnswersTonight;
	static bool s_Ending;
	static bool s_Looping;
	static PlayerBase s_Lying;
	static int s_LieAt;
	static float s_Progress;
	static int s_Hint;
	static int s_TellAt;
	static bool s_Told;
	static const float DIM_S = 15;
	static const int DIM_FLICK_MIN_MS = 1600;
	static const int DIM_FLICK_MAX_MS = 5000;
	static int s_DimFlickAt;
	static int s_PeakAt;
	static ref array<PlayerBase> s_Kneelers = new array<PlayerBase>;
	static ref array<int> s_KneelAt = new array<int>;
	static ref array<int> s_KneelSpot = new array<int>;


	static ref set<PlayerBase> s_EmoteKneel = new set<PlayerBase>;
	static const float EMOTE_SPOT_M = 1.4;


	static void Register(Object altar)
	{
		if (!GetGame().IsServer() || !altar)
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Setup, 2000, false, altar);
		if (!s_Looping)
		{
			s_Looping = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, TICK_MS, true);
		}
	}



	static vector SlabTop(Object slab, vector lie)
	{
		vector from = lie + Vector(0, 1.0, 0);
		vector to = lie - Vector(0, 1.5, 0);
		vector hit, normal;
		int comp;
		set<Object> results = new set<Object>;
		bool got = DayZPhysics.RaycastRV(from, to, hit, normal, comp, results, null, null, false, false, ObjIntersectGeom);
		bool onSlab = got && results.Find(slab) >= 0;
		Print("[AZ_Vamp] ritual slab top: box " + lie[1] + " ray " + got + " on slab " + onSlab + " at " + hit[1]);
		if (!onSlab)
			return lie;
		vector r = lie;
		r[1] = hit[1] + 0.01;
		return r;
	}

	static float FloorY(AZ_VampRitualSite s, vector at)
	{
		if (s.m_InCrypt)
			return s.m_Center[1];
		return GetGame().SurfaceY(at[0], at[2]);
	}

	static vector Ring(AZ_VampRitualSite s, float r, float deg)
	{

		float a = deg * Math.DEG2RAD;
		vector p = s.m_Center + s.m_Back * (r * Math.Cos(a));
		p = p + s.m_Right * (r * Math.Sin(a));
		float y = FloorY(s, p);
		p[1] = y;
		return p;
	}

	static void Setup(Object altar)
	{
		if (!altar)
			return;
		AZ_VampRitualSite s = new AZ_VampRitualSite;
		s.m_Altar = altar;
		s.m_InCrypt = AZ_VampCrypt.Inside(altar.GetPosition() + "0 0.5 0");
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(altar.GetPosition(), SLAB_M, objs, null);
		foreach (Object o : objs)
		{
			if (o && o.IsKindOf("AZ_VampAltar_Blood") && !s.m_Slab)
				s.m_Slab = o;
		}
		Object frame = altar;
		if (s.m_Slab)
			frame = s.m_Slab;
		vector back = frame.GetDirection();
		back[1] = 0;
		back.Normalize();
		s.m_Back = back;
		s.m_Right = Vector(back[2], 0, -back[0]);
		s.m_LieDir = -s.m_Right;
		if (s.m_Slab)
		{
			vector box[2];
			s.m_Slab.GetCollisionBox(box);
			float cx = (box[0][0] + box[1][0]) * 0.5;
			float cz = (box[0][2] + box[1][2]) * 0.5;
			vector top = Vector(cx, box[1][1] + 0.02, cz);
			s.m_HalfLen = (box[1][0] - box[0][0]) * 0.5;
			s.m_Lie = s.m_Slab.ModelToWorld(top);
			s.m_Lie = SlabTop(s.m_Slab, s.m_Lie);
			vector mid = Vector(cx, 0, cz);
			s.m_Center = s.m_Slab.ModelToWorld(mid);
			float sy = s.m_Slab.GetPosition()[1];
			if (s.m_Slab.IsKindOf("AZ_VampAltar_Drum"))
				sy = sy - AZ_VampCrypt.DRUM_TOP_F;
			if (!s.m_InCrypt)
				sy = GetGame().SurfaceY(s.m_Center[0], s.m_Center[2]);
			s.m_Center[1] = sy;
		}
		else
		{
			s.m_Center = altar.GetPosition();
			float cy = FloorY(s, s.m_Center);
			s.m_Center[1] = cy;
			s.m_Lie = s.m_Center + "0 0.02 0";
		}


		vector headAt = s.m_Center + s.m_Right * 1.9;
		headAt = headAt + s.m_Back * 0.4;
		float hy = FloorY(s, headAt);
		headAt[1] = hy;
		s.m_Head = headAt;


		array<Object> old = new array<Object>;
		float clearR = CANDLE_R + 1;
		if (s.m_InCrypt)
			clearR = 12;
		GetGame().GetObjectsAtPosition(s.m_Center, clearR, old, null);
		int oldCandles = 0;
		foreach (Object oc : old)
		{
			if (oc && oc.IsKindOf("AZ_VampRitualCandle"))
			{
				GetGame().ObjectDelete(oc);
				oldCandles++;
			}
		}
		Print("[AZ_Vamp] ritual site at " + s.m_Center + ": " + oldCandles + " saved altar candles removed before the new five");



		Object apse = null;
		if (s.m_InCrypt)
		{
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition(s.m_Center, 12, near, null);
			foreach (Object ao : near)
			{
				if (ao && ao.IsKindOf("AZ_Crypt_Apse") && ao.MemoryPointExists("candle_0"))
					apse = ao;
			}
		}
		Print("[AZ_Vamp] ritual site: candles in the apse sockets " + (apse != null));
		for (int i = 0; i < 5; i++)
		{
			vector cp = Ring(s, CANDLE_R, 36 + i * 72);
			if (apse && apse.MemoryPointExists("candle_" + i))
				cp = apse.ModelToWorld(apse.GetMemoryPointPos("candle_" + i));
			AZ_VampRitualCandle c = AZ_VampRitualCandle.Cast(GetGame().CreateObjectEx("AZ_VampRitualCandle", cp, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD));
			if (!c)
			{
				Print("[AZ_Vamp] ritual site: could not spawn a candle");
				continue;
			}
			c.SetPosition(cp);
			c.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			s.m_Candles.Insert(c);
		}
		for (int k = 0; k < 4; k++)
		{
			vector kp = Ring(s, KNEEL_R, 45 + k * 90);
			s.m_Kneel.Insert(kp);
		}
		s_Sites.Insert(s);
		string slab = "ground";
		if (s.m_Slab)
			slab = "slab";
		Print("[AZ_Vamp] ritual site at " + s.m_Center + " (" + slab + ", crypt " + s.m_InCrypt + ") lie " + s.m_Lie + " head " + s.m_Head + ", " + s.m_Candles.Count() + " candles");
	}

	static AZ_VampRitualSite SiteOf(Object candle)
	{
		foreach (AZ_VampRitualSite s : s_Sites)
		{
			if (s.m_Candles.Find(AZ_VampRitualCandle.Cast(candle)) >= 0)
				return s;
		}
		return null;
	}

	static AZ_VampRitualSite SiteNear(vector pos, float m)
	{
		foreach (AZ_VampRitualSite s : s_Sites)
		{
			if (vector.Distance(pos, s.m_Center) < m)
				return s;
		}
		return null;
	}



	static ref array<PlayerBase> s_Friends;

	static string Friends(PlayerBase admin, string arg)
	{
		if (!s_Friends)
			s_Friends = new array<PlayerBase>;
		if (arg == "clear")
		{
			int gone = 0;
			foreach (PlayerBase old : s_Friends)
			{
				if (old)
				{
					GetGame().ObjectDelete(old);
					gone++;
				}
			}
			s_Friends.Clear();
			return "friends cleared: " + gone;
		}
		if (!admin)
			return "friends: no admin";
		int n = arg.ToInt();
		if (n <= 0)
			n = 3;
		if (n > 8)
			n = 8;
		vector c = admin.GetPosition();
		AZ_VampRitualSite s = SiteNear(c, 60);
		if (s)
			c = s.m_Center;
		for (int i = 0; i < n; i++)
		{
			float a = (i * 360.0 / n + Math.RandomFloat(-15, 15)) * Math.DEG2RAD;
			int ring = i - (i / 3) * 3;
			float r = 3.0 + ring * 1.5;
			vector at = c + Vector(Math.Cos(a) * r, 0, Math.Sin(a) * r);
			vector stand = AZ_VampSpawn.Floor(at, c[1]);
			PlayerBase h = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Male(30 + i), stand, false, true));
			if (!h)
				continue;
			AZ_VampDress.Survivor(h, 30 + i);
			h.SetPosition(stand);
			vector look = c - stand;
			look[1] = 0;
			h.SetDirection(look.Normalized());
			s_Friends.Insert(h);
		}
		Print("[AZ_Vamp] test friends: " + s_Friends.Count() + " human dummies round " + c);
		return "friends: " + s_Friends.Count() + " humans round the altar (clear: /vamp friends clear)";
	}

	static bool InArea(AZ_VampRitualSite s, vector pos)
	{
		if (s.m_InCrypt)
			return AZ_VampCrypt.Inside(pos);
		return vector.Distance(pos, s.m_Center) < AREA_M;
	}

	static void Area(AZ_VampRitualSite s, out array<PlayerBase> outList)
	{
		outList = new array<PlayerBase>;
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		foreach (PlayerBase p : list)
		{
			if (p && p.IsAlive() && InArea(s, p.GetPosition()))
				outList.Insert(p);
		}
	}





	static void Note(PlayerBase p, string title, string text)
	{
		if (p && p.GetIdentity())
			NotificationSystem.SendNotificationToPlayerIdentityExtended(p.GetIdentity(), 7, title, text);
	}

	static void Say(AZ_VampRitualSite s, string text)
	{
		array<PlayerBase> list;
		Area(s, list);
		foreach (PlayerBase p : list)
		{
			if (p && p.GetIdentity())
				NotificationSystem.SendNotificationToPlayerIdentityExtended(p.GetIdentity(), 7, "The altar", text);
		}
	}


	static string Closed()
	{
		if (!AZ_VampEvent.Active())
			return "The wick will not take. The altar sleeps.";
		if (!GetGame().GetWorld().IsNight())
			return "The wick will not take while the sun is up.";
		if (s_AnswersTonight >= AZ_VampState.Cfg().ritualsPerNight)
			return "The wick will not take. The altar has given all its answers for tonight.";
		if (s_Ending)
			return "The wick will not take. The altar has not finished answering.";
		return "";
	}


	static int s_CheckAt;
	static vector s_CheckPos;
	static bool s_Near;
	static bool NearAltar(vector pos)
	{
		int now = GetGame().GetTime();
		if (now < s_CheckAt && vector.Distance(pos, s_CheckPos) < 0.5)
			return s_Near;
		s_CheckAt = now + 1000;
		s_CheckPos = pos;
		s_Near = false;
		s_Step = STEP_NONE;
		int lit = 0;
		int all = 0;
		bool poured = false;
		bool lying = false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(pos, NEAR_M, objs, null);
		foreach (Object o : objs)
		{
			AZ_VampRitualCandle c = AZ_VampRitualCandle.Cast(o);
			if (c)
			{
				s_Near = true;
				all++;
				if (c.AZ_IsLit())
					lit++;
				if (c.AZ_IsPoured())
					poured = true;
			}
			PlayerBase lp = PlayerBase.Cast(o);
			if (lp && lp.m_AZOnAltar && lp.IsAlive())
				lying = true;
		}

		if (lying)
			s_Step = STEP_LYING;
		else if (poured)
			s_Step = STEP_POURED;
		else if (all > 0 && lit >= all)
			s_Step = STEP_AWAKE;
		return s_Near;
	}


	static const int STEP_NONE = 0;
	static const int STEP_AWAKE = 1;
	static const int STEP_POURED = 2;
	static const int STEP_LYING = 3;
	static int s_Step;

	static int ClientStep(vector pos)
	{
		if (!NearAltar(pos))
			return STEP_NONE;
		return s_Step;
	}


	static void SetPoured(AZ_VampRitualSite s, bool on)
	{
		if (!s)
			return;
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c)
				c.AZ_SetPoured(on);
		}
	}

	static PlayerBase Holder(EntityAI item)
	{
		if (!item)
			return null;
		return PlayerBase.Cast(item.GetHierarchyRootPlayer());
	}


	static void CandleLit(AZ_VampRitualCandle c, EntityAI source)
	{
		PlayerBase p = Holder(source);
		AZ_VampRitualSite s = SiteOf(c);
		if (!s)
			return;
		string who = "?";
		if (p)
		{
			who = AZ_VampState.Uid(p);
			AZ_VampCrypt.MarkRitual(p);
		}
		string no = Closed();
		if (no == "" && s_Site && s_Site != s)
			no = "The wick will not take. Another altar is awake tonight.";
		if (no == "" && s_State >= ST_CLIMAX)
			no = "The wick will not take.";
		if (no != "")
		{
			if (p)
				AZ_VampRitual.Note(p, "The altar", no);
			Print("[AZ_Vamp] ritual refused candle " + who + ": " + no);
			return;
		}
		if (!AZ_VampAltar.CapWithPending())
		{
			Say(s, CAP_REACHED_TEXT);
			Print("[AZ_Vamp] ritual refused candle " + who + ": cap reached");
			return;
		}
		c.AZ_SetLit(true);
		s_Site = s;
		if (s_State == ST_SLEEP)
			s_State = ST_LIGHTING;
		if (p)
			AZ_VampCrypt.MarkRitual(p);
		int lit = Lit(s);
		Print("[AZ_Vamp] ritual candle " + who + " lit " + lit + "/" + s.m_Candles.Count());
		if (lit >= s.m_Candles.Count() && s_State < ST_AWAKE)
		{
			s_State = ST_AWAKE;
			string next = "Hold a blade on the slab and press F11 to offer yourself to her.";
			if (!AZ_VampState.Cfg().altarBloodBag)
			{
				s_State = ST_POURED;
				SetPoured(s, true);
				next = "Someone must lie on it. The others kneel.";
			}
			Say(s, "The altar is awake. " + next);
			Call(s);
			Print("[AZ_Vamp] ritual awake at " + s.m_Center);
		}
		else if (p)
			AZ_VampRitual.Note(p, "The altar", "The candle takes. " + lit + " of " + s.m_Candles.Count() + ".");
	}



	static AZ_VampRitualSite CryptSite()
	{
		foreach (AZ_VampRitualSite c : s_Sites)
		{
			if (c && c.m_InCrypt && c.m_Slab)
				return c;
		}
		return null;
	}

	static string Debug(PlayerBase admin, string what)
	{
		if (what != "awake" && what != "slab")
			return "ritual: awake (all 5 candles lit) | slab (awake + you at the slab with a knife, F11 offers); /vamp finale";
		AZ_VampRitualSite s = CryptSite();
		if (!s)
			return "no crypt altar";
		if (s_State >= ST_LYING)
			return "busy: someone is on the altar (state " + s_State + "); /vamp finale or wait";
		if (s_Site && s_Site != s)
			Reset();
		int n = 0;
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c && !c.AZ_IsLit())
			{
				CandleLit(c, null);
				n++;
			}
		}
		string r = "skipped: lit " + n + " candles, state " + s_State;
		if (!GetGame().GetWorld().IsNight() || !AZ_VampEvent.Active())
			r = r + " (WARNING: needs night and the event on, or the altar falls asleep at once)";
		if (what == "awake")
			return r;
		vector stand = s.m_Lie - s.m_Back * 1.2;
		float sy = FloorY(s, stand);
		stand[1] = sy;
		admin.SetPosition(stand);
		vector look = s.m_Lie - stand;
		look[1] = 0;
		admin.SetDirection(look.Normalized());
		string knife = "kept your blade";
		if (!AZ_VampAltar.IsBlade(admin.GetItemInHands()))
		{
			EntityAI held = admin.GetItemInHands();
			if (held && !admin.ServerTakeEntityToInventory(FindInventoryLocationType.ATTACHMENT | FindInventoryLocationType.CARGO, held))
				admin.ServerDropEntity(held);
			knife = "a FangeKnife in your hands";
			if (!admin.GetHumanInventory().CreateInHands("FangeKnife"))
				knife = "NO knife (hands busy: take one from the slab)";
		}
		return r + "; at the slab, " + knife + ": F11 offers (armed within 1 s)";
	}


	static string Finale(PlayerBase admin)
	{
		AZ_VampRitualSite s = CryptSite();
		if (!s)
			return "no crypt altar";
		if (admin.m_AZVampire)
			return "you are a vampire: /vamp cure first (the wake turns a human)";
		if (s_State != ST_SLEEP)
			Reset();
		s_Site = s;
		s_State = ST_CLIMAX;
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c)
				c.AZ_SetLit(true);
		}
		admin.SetPosition(s.m_Lie);
		Print("[AZ_Vamp] ritual finale shortcut for " + AZ_VampState.Uid(admin));
		Blackout(admin, AZ_VampState.Uid(admin));
		return "skipped: the candles, the offering, the lying and kneeling, the climax's gutter and eyes. Now: the blackout, the river feed on you, the wake (it turns you; /vamp cure after)";
	}

	static int Lit(AZ_VampRitualSite s)
	{
		int n = 0;
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c && c.AZ_IsLit())
				n++;
		}
		return n;
	}


	static void Pour(PlayerBase p)
	{
		if (!p || p.m_AZVampire || p.m_AZVampTurning)
			return;
		AZ_VampCrypt.MarkRitual(p);
		AZ_VampRitualSite s = SiteNear(p.GetPosition(), NEAR_M + 1);
		if (!s || s != s_Site || s_State < ST_AWAKE)
		{
			AZ_VampRitual.Note(p, "The altar", "The altar is asleep. Light its five candles first.");
			return;
		}
		if (s_State >= ST_POURED)
		{
			AZ_VampRitual.Note(p, "The altar", "The altar has its blood. Now it wants someone to lie on it.");
			return;
		}
		EntityAI held = p.GetItemInHands();
		bool cut = AZ_VampAltar.IsBlade(held);
		if (!cut && !AZ_VampAltar.IsFullBag(held))
		{
			AZ_VampRitual.Note(p, "The altar", "The altar wants your blood. Hold a blade and open your palm over it.");
			return;
		}
		if (cut)
			Cut(p);
		else
			GetGame().ObjectDelete(held);
		s_State = ST_POURED;
		SetPoured(s, true);
		AZ_VampCrypt.MarkRitual(p);
		Say(s, "The blood runs into the stone and is gone. The altar is waiting for someone to lie on it.");
		Print("[AZ_Vamp] ritual blood " + AZ_VampState.Uid(p) + " cut " + cut);
	}



	static const float CUT_BLOOD = 200;
	static const int CUT_BLEED_MS = 8000;
	static const float CUT_BLOOD_FLOOR = 3200;

	static void Cut(PlayerBase p)
	{
		// Never cut a low-blood player into death; leave room for the short bleed.
		float take = Math.Min(CUT_BLOOD, Math.Max(0, p.GetHealth("", "Blood") - CUT_BLOOD_FLOOR));
		AZ_VampEvidence.Begin("ritual_cut", p, p, false);
		p.AddHealth("", "Blood", -take);
		AZ_VampEvidence.End("ritual_cut", p, p);
		p.SetBloodyHands(true);
		BleedingSourcesManagerServer bm = p.GetBleedingManagerServer();
		if (!bm)
			return;
		int before = p.GetBleedingBits();
		bm.AttemptAddBleedingSourceBySelection("RightForeArmRoll");
		int added = p.GetBleedingBits() & ~before;
		if (added != 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CutStop, CUT_BLEED_MS, false, p, added);
	}

	static void CutStop(PlayerBase p, int bits)
	{
		if (p && p.IsAlive() && p.GetBleedingManagerServer())
			p.GetBleedingManagerServer().AZ_StopBits(bits);
	}


	static bool Lie(PlayerBase p, bool viaF11 = false)
	{
		if (!p)
			return false;
		string no = "";
		AZ_VampRitualSite s = SiteNear(p.GetPosition(), NEAR_M + 1);
		if (p.m_AZVampire)
			no = "The altar does not want the dead.";
		else if (p.m_AZVampTurning || p.m_AZVampHeld || p.IsRestrained() || p.IsUnconscious())
			no = "Not like this. Not while bitten, held, unconscious or restrained.";
		else if (!s || s != s_Site || s_State < ST_AWAKE)
			no = "The altar is asleep. Light its five candles first.";
		else if (s_State == ST_AWAKE && !AZ_VampAltar.IsBlade(p.GetItemInHands()))
			no = "The altar wants blood. Bring a blade.";
		else if (s_State >= ST_LYING)
			no = "Someone already lies there.";
		else if (Banned(p))
			no = "The altar remembers you. Not yet.";
		else if (Closed() != "")
			no = Closed();
		else if (!AZ_VampAltar.CapWithPending())
			no = CAP_REACHED_TEXT;
		bool stab = viaF11 || (StabKnife(p.GetItemInHands()) && AZ_VampState.Cfg() && AZ_VampState.Cfg().ritualStab);
		if (no == "" && p.GetItemInHands() && !stab && !(s_State == ST_AWAKE && AZ_VampAltar.IsBlade(p.GetItemInHands())))
			no = "Empty your hands first.";
		bool needBlood = s_State == ST_AWAKE;
		if (no != "")
		{
			if (no == CAP_REACHED_TEXT)
				Say(s, no);
			else
				AZ_VampRitual.Note(p, "The altar", no);
			Print("[AZ_Vamp] ritual refused lie " + AZ_VampState.Uid(p) + ": " + no);
			return false;
		}
		s_State = ST_LYING;
		s_Lying = p;
		s_LieAt = GetGame().GetTime();
		s_HoldPulls = 0;
		s_HoldPullAt = 0;
		s_HoldFailAt = 0;
		Print("[AZ_Vamp] altar hold " + AZ_VampState.Uid(p) + " held until the ritual ends");
		s_Progress = 0;
		s_Hint = 0;
		p.m_AZOnAltar = true;
		p.SetSynchDirty();



		if (!viaF11)
		{
			AZ_VampSnap.To(p, s.m_Lie, s.m_LieDir);
		}
		AZ_VampCrypt.MarkRitual(p);
		s_OfferNeedsBlood = false;
		if (needBlood && !stab)
			GiveBlood(p);
		if (!stab)
		{
			FreeHands(p, 0);
			Hold(p);
		}
		if (stab)
		{
			s_OfferNeedsBlood = needBlood;
			p.m_AZOffering = true;
			p.SetSynchDirty();
			s_OfferAt = s_LieAt;
			s_OfferLoopAt = 0;
			s_OfferStabAt = 0;
			s_OfferQuietUntil = s_LieAt + OFFER_MAX_MS + 8000;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OfferStep, OFFER_STEP_MS, false, p);
			AZ_VampRitual.Note(p, "The altar", "You set the blade to yourself on the cold stone.");
		}
		else
			AZ_VampRitual.Note(p, "The altar", "You lie back on the cold stone. Do not get up.");
		Say(s, "Someone lies on the altar. Kneel around it and the answer comes sooner.");
		Print("[AZ_Vamp] ritual lie " + AZ_VampState.Uid(p) + " at " + s.m_Lie);
		return true;
	}

	static bool StillLying(PlayerBase p)
	{
		return p && p == s_Lying && (s_State == ST_LYING || s_State == ST_CLIMAX);
	}

	static void GotUp(PlayerBase p)
	{
		if (!p || p != s_Lying || s_State != ST_LYING || p.m_AZOnAltar)
			return;
		s_Lying = null;
		ReleaseHold(p);
		p.m_AZOnAltar = false;
		p.SetSynchDirty();
		ClearOffer(p);
		Ban(p);
		Abort(AZ_VampState.Uid(p) + " got up");
	}



	static const int RELIE_BAN_MS = 300000;

	static PlayerBase s_Bound;

	static const bool ALTAR_BONDS = false;

	static void Hold(PlayerBase p)
	{
		if (!p || p != s_Lying || !p.IsAlive() || p.m_AZOffering)
			return;
		if (p.IsRestrained())
		{
			if (p == s_Bound && p.m_AZWillBonds && p.GetItemInHands() == p.m_AZWillBonds)
				return;
			Abort("the offering was bound by someone else");
			return;
		}
		if (!ALTAR_BONDS)
			return;
		if (AZ_VampTie.Bind(p, AZ_VampTie.COMPEL_BONDS))
		{
			s_Bound = p;
			s_HoldFailAt = 0;
			return;
		}



		int now = GetGame().GetTime();
		if (s_HoldFailAt == 0)
			s_HoldFailAt = now;
		if (now - s_HoldFailAt < HOLD_GRACE_MS)
		{
			Print("[AZ_Vamp] altar hold " + AZ_VampState.Uid(p) + ": hands busy, binding again next tick");
			return;
		}
		s_HoldFailAt = 0;
		Abort("the altar could not hold the offering");
	}
	static const int HOLD_GRACE_MS = 6000;
	static int s_HoldFailAt;

	static void ReleaseHold(PlayerBase p)
	{
		if (!p || p != s_Bound)
			return;
		EntityAI bonds = p.m_AZWillBonds;
		if (bonds && p.GetItemInHands() == bonds)
		{
			p.m_AZWillBonds = null;
			GetGame().ObjectDelete(bonds);
			p.SetRestrained(false);
			p.OnItemInHandsChanged();
		}
		s_Bound = null;
	}
	static const float ALTAR_PULL_M = 0.35;
	static int s_HoldPulls;
	static int s_HoldPullAt;
	static const int BAN_HIT_MS = 10000;
	static ref map<string, int> s_LieBan = new map<string, int>;

	static void Ban(PlayerBase p)
	{
		string uid = AZ_VampState.Uid(p);
		s_LieBan.Set(uid, GetGame().GetTime() + RELIE_BAN_MS);
		if (s_Site)
			Say(s_Site, "The one who rose from the stone is not welcome on it again tonight. Not soon.");
		Print("[AZ_Vamp] ritual: " + uid + " got up off the altar, no lie-down for " + (RELIE_BAN_MS / 1000) + " s");
	}

	static bool Banned(PlayerBase p)
	{
		string uid = AZ_VampState.Uid(p);
		return s_LieBan.Contains(uid) && GetGame().GetTime() < s_LieBan.Get(uid);
	}


	static void PullOff(PlayerBase by, PlayerBase target)
	{
		if (!by || !target || target != s_Lying || s_State != ST_LYING)
			return;
		AZ_VampRitual.Note(by, "The altar", "Cold hands close on yours.");
		Protect(target, by, "pulled at the one on the altar");
	}





	static const int PROTECT_GONE_MS = 7500;


	static const int PROTECT_COOLDOWN_MS = 15000;
	static ref map<string, int> s_ProtectAt = new map<string, int>;


	static void ProtectedStruck(PlayerBase striker)
	{
		int i = s_Kneelers.Find(striker);
		if (i < 0)
			return;
		AZ_VampRitual.Note(striker, "The altar", "Blood on your hands breaks your prayer.");
		DropKneeler(i, "struck someone");
		if (Running(striker, ActionAZKneel))
			Interrupt(striker);
	}

	static bool Protecting(PlayerBase who)
	{
		if (!who || !s_Site || s_State < ST_LYING)
			return false;
		return who == s_Lying || s_Kneelers.Find(who) >= 0;
	}

	static void Protect(PlayerBase who, PlayerBase by, string what)
	{


		if (!who || !by || by == who || by.m_AZElder || !by.IsAlive() || by.IsUnconscious() || by.m_AZVampHeld)
			return;
		string uid = AZ_VampState.Uid(by);
		int now = GetGame().GetTime();
		if (s_ProtectAt.Contains(uid) && now - s_ProtectAt.Get(uid) < PROTECT_COOLDOWN_MS)
			return;
		s_ProtectAt.Set(uid, now);
		vector dir = by.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector from = by.GetPosition() - dir * 3.0;
		PlayerBase e = AZ_VampCryptLurkers.Dress(from, 0, 11);
		if (!e)
			return;
		e.SetDirection(dir);
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(from), true);
		AZ_VampElderWrath.Hit(e, by);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProtectGone, PROTECT_GONE_MS, false, e);
		Print("[AZ_Vamp] ritual protected: " + AZ_VampState.Uid(by) + " " + what + "; an elder answers");
	}

	static void ProtectGone(PlayerBase e)
	{
		if (!e)
			return;
		GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(e.GetPosition()), true);
		GetGame().ObjectDelete(e);
	}


	static bool Kneel(PlayerBase p)
	{
		if (!p)
			return false;
		string no = "";
		AZ_VampRitualSite s = SiteNear(p.GetPosition(), KNEEL_R + KNEEL_SNAP_M);

		bool invited = s_State == ST_LYING && AZ_VampInvite.Invited(p) && s_Kneelers.Find(p) < 0 && AZ_VampCrypt.Inside(p.GetPosition());
		if (invited)
			s = s_Site;
		if (p.m_AZVampire)
			no = "Your knees will not bend. The altar is not for your kind.";
		else if (p == s_Lying || p.m_AZVampTurning || p.m_AZVampHeld || p.IsRestrained() || p.IsUnconscious())
			no = "Not like this. Not while bitten, held, unconscious or restrained.";
		else if (!s || s != s_Site || s_State < ST_AWAKE || s_State >= ST_CLIMAX)
			no = "Nothing listens yet. Wake the altar first.";
		else if (s_Kneelers.Find(p) >= 0)
			return true;
		if (no == "")
		{
			int best = -1;
			float bestD = KNEEL_SNAP_M;
			if (invited)
				bestD = 1000;
			for (int i = 0; i < s.m_Kneel.Count(); i++)
			{
				if (s_KneelSpot.Find(i) >= 0)
					continue;
				vector spot = s.m_Kneel[i];
				vector pp = p.GetPosition();
				float d = vector.Distance(pp, spot);
				if (d < bestD)
				{
					bestD = d;
					best = i;
				}
			}
			if (best < 0 && invited)
				no = "There is no room. Watch.";
			else if (best < 0)
				no = "There is no room left in the circle.";
		}
		if (no != "")
		{
			AZ_VampRitual.Note(p, "The altar", no);
			Print("[AZ_Vamp] ritual refused kneel " + AZ_VampState.Uid(p) + ": " + no);
			return false;
		}
		vector at = s.m_Kneel[best];
		vector face = s.m_Center - at;
		face[1] = 0;
		face.Normalize();
		vector was = p.GetPosition();
		if (invited && vector.Distance(was, at) > KNEEL_SNAP_M && p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_FADE, new Param1<int>(AZ_VampCrypt.FADE_MS), true, p.GetIdentity());
		if (invited)
		{
			AZ_VampInvite.Used(p);
			Print("[AZ_Vamp] ritual invite kneel " + AZ_VampState.Uid(p) + ": from hall " + AZ_VampCrypt.HallLocal(was) + " to spot " + best);
		}
		if (p.GetItemInHands())
			FreeHands(p, 0);
		AZ_VampSnap.To(p, at, face);
		s_Kneelers.Insert(p);
		s_KneelAt.Insert(GetGame().GetTime());
		s_KneelSpot.Insert(best);
		AZ_VampCrypt.MarkRitual(p);
		AZ_VampWhisperLines.Trigger(p, "altar");
		Print("[AZ_Vamp] ritual kneel " + AZ_VampState.Uid(p) + " spot " + best + ", " + s_Kneelers.Count() + " kneeling");
		return true;
	}

	static bool SitEmote(PlayerBase p)
	{
		if (!p || !p.GetEmoteManager() || !p.GetEmoteManager().IsEmotePlaying())
			return false;
		int id = p.GetEmoteManager().AZ_GestureID();
		return id == EmoteConstants.ID_EMOTE_SITA || id == EmoteConstants.ID_EMOTE_SITB;
	}


	static void EmoteKneelers(int now)
	{
		if (!s_Site || s_State != ST_LYING)
			return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		foreach (PlayerBase h : online)
		{
			if (!h || !h.IsAlive() || h.IsUnconscious() || h.m_AZVampire || h == s_Lying || s_Kneelers.Find(h) >= 0)
				continue;
			vector hp = h.GetPosition();
			if (vector.Distance(hp, s_Site.m_Center) > KNEEL_R + EMOTE_SPOT_M || !SitEmote(h))
				continue;
			int best = -1;
			float bestD = EMOTE_SPOT_M;
			for (int i = 0; i < s_Site.m_Kneel.Count(); i++)
			{
				if (s_KneelSpot.Find(i) >= 0)
					continue;
				vector spot = s_Site.m_Kneel[i];
				float d = vector.Distance(hp, spot);
				if (d < bestD)
				{
					bestD = d;
					best = i;
				}
			}
			if (best < 0)
				continue;
			s_Kneelers.Insert(h);
			s_KneelAt.Insert(now);
			s_KneelSpot.Insert(best);
			s_EmoteKneel.Insert(h);
			AZ_VampCrypt.MarkRitual(h);
			Print("[AZ_Vamp] ritual kneel (sit emote) " + AZ_VampState.Uid(h) + " spot " + best + " at " + bestD + " m, " + s_Kneelers.Count() + " kneeling");
		}
	}

	static bool StillKneeling(PlayerBase p)
	{
		return p && s_Kneelers.Find(p) >= 0;
	}

	static void Unkneel(PlayerBase p, string why)
	{
		int i = s_Kneelers.Find(p);
		if (i < 0)
			return;
		DropKneeler(i, why);
	}

	static void DropKneeler(int i, string why)
	{
		PlayerBase p = s_Kneelers[i];
		int ei = s_EmoteKneel.Find(p);
		if (ei >= 0)
			s_EmoteKneel.Remove(ei);
		s_Kneelers.Remove(i);
		s_KneelAt.Remove(i);
		s_KneelSpot.Remove(i);
		string who = "(gone)";
		if (p)
			who = AZ_VampState.Uid(p);
		Print("[AZ_Vamp] ritual unkneel " + who + " (" + why + "), " + s_Kneelers.Count() + " kneeling");
	}

	static void Interrupt(PlayerBase p)
	{
		if (p && p.GetActionManager())
			p.GetActionManager().Interrupt();
	}







	static const int SJ_OFFER = 148;
	static const int OFFER_STEP_MS = 250;
	static const int OFFER_LOOP_MS = 9000;
	static const int OFFER_HOLD_MS = 1200;
	static const int OFFER_STAB_MS = 2200;
	static const int OFFER_MAX_MS = 13000;
	static const float OFFER_HEALTH_FLOOR = 50;
	static int s_OfferAt;
	static int s_OfferLoopAt;
	static int s_OfferStabAt;
	static int s_OfferQuietUntil;
	static int s_OfferLogAt;
	static bool s_OfferNeedsBlood;



	static void GiveBlood(PlayerBase p)
	{
		s_OfferNeedsBlood = false;
		if (!p || !p.IsAlive())
			return;
		Cut(p);
		if (s_Site)
		{
			SetPoured(s_Site, true);
			Say(s_Site, "The blood runs into the stone and is gone.");
		}
		Print("[AZ_Vamp] ritual blood given by the offering " + AZ_VampState.Uid(p));
	}

	static bool StabKnife(EntityAI e)
	{
		return e && !e.IsRuined() && e.ConfigIsExisting("suicideAnim") && e.ConfigGetString("suicideAnim") == "onehanded";
	}


	static bool Offering(PlayerBase p, int now)
	{
		return p && (p.m_AZOffering || now < s_OfferQuietUntil);
	}

	static void OfferStep(PlayerBase p)
	{
		if (!p || !p.m_AZOffering)
			return;
		if (p != s_Lying || s_State != ST_LYING)
		{
			ClearOffer(p);
			return;
		}
		int now = GetGame().GetTime();
		EmoteManager em = p.GetEmoteManager();
		int state = -1;
		bool playing = false;
		if (em)
		{
			playing = em.IsEmotePlaying();
			if (em.m_Callback)
				state = em.m_Callback.GetState();
		}
		if (now >= s_OfferLogAt)
		{
			s_OfferLogAt = now + 1000;
			Print("[AZ_Vamp] ritual offering " + AZ_VampState.Uid(p) + ": server emote playing " + playing + " callback state " + state + " stabbed " + (s_OfferStabAt > 0));
		}
		if (s_OfferStabAt == 0)
		{
			if (state == HumanCommandActionCallback.STATE_LOOP_LOOP)
			{
				if (s_OfferLoopAt == 0)
					s_OfferLoopAt = now;
				if (now - s_OfferLoopAt >= OFFER_HOLD_MS)
				{
					s_OfferStabAt = now;
					OfferJuncture(p, 1);
					if (s_OfferNeedsBlood)
						GiveBlood(p);
					Print("[AZ_Vamp] ritual offering: the stab");
				}
			}
			else if (now - s_OfferAt > OFFER_LOOP_MS)
			{
				Print("[AZ_Vamp] ritual offering: no loop in " + OFFER_LOOP_MS + " ms, the plain lie-down instead");
				OfferEnd(p);
				return;
			}
		}
		else if (now - s_OfferStabAt >= OFFER_STAB_MS)
		{
			OfferEnd(p);
			return;
		}
		if (now - s_OfferAt > OFFER_MAX_MS)
		{
			OfferEnd(p);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OfferStep, OFFER_STEP_MS, false, p);
	}




	static void ClearOffer(PlayerBase p)
	{
		s_OfferQuietUntil = 0;
		if (!p || !p.m_AZOffering)
			return;
		OfferJuncture(p, 2);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OfferClear, 1500, false, p);
	}






	static const float ARM_M = 1.8;
	static ref array<PlayerBase> s_Armed = new array<PlayerBase>;

	static void Arm()
	{
		array<PlayerBase> want = new array<PlayerBase>;
		if (s_Site && s_Site.m_Slab && (s_State == ST_AWAKE || s_State == ST_POURED))
		{
			array<PlayerBase> list;
			Area(s_Site, list);
			foreach (PlayerBase p : list)
			{
				if (!p || !p.IsAlive() || p.m_AZVampire || p.m_AZVampTurning || p.m_AZOnAltar || !p.GetIdentity())
					continue;
				if (!AZ_VampAltar.IsBlade(p.GetItemInHands()))
					continue;
				vector d = p.GetPosition() - s_Site.m_Lie;
				d[1] = 0;
				if (d.Length() <= ARM_M)
					want.Insert(p);
			}
		}
		for (int i = s_Armed.Count() - 1; i >= 0; i--)
		{
			PlayerBase was = s_Armed[i];
			if (was && want.Find(was) < 0)
			{
				was.m_AZAltarArmed = false;
				was.SetSynchDirty();
			}
			if (!was || want.Find(was) < 0)
				s_Armed.Remove(i);
		}
		foreach (PlayerBase a : want)
		{
			if (s_Armed.Find(a) >= 0)
				continue;
			a.m_AZAltarArmed = true;
			a.SetSynchDirty();
			s_Armed.Insert(a);
			Print("[AZ_Vamp] ritual: " + AZ_VampState.Uid(a) + " armed at the slab (F11 offers)");
		}
	}


	static void F11Offer(PlayerBase p)
	{
		if (!p || p.m_AZOnAltar || p.m_AZOffering)
			return;
		Print("[AZ_Vamp] ritual: F11 at the slab by " + AZ_VampState.Uid(p));
		if (!Lie(p, true))

			Print("[AZ_Vamp] ritual: F11 offering refused (the emote plays out harmless)");
	}

	static void OfferClear(PlayerBase p)
	{
		if (!p || p == s_Lying)
			return;
		p.m_AZOffering = false;
		p.SetSynchDirty();
		Print("[AZ_Vamp] ritual offering flag cleared for " + AZ_VampState.Uid(p));
	}

	static void OfferJuncture(PlayerBase p, int what)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(what);
		p.SendSyncJuncture(SJ_OFFER, ctx);
	}


	static void OfferEnd(PlayerBase p)
	{
		if (!p)
			return;
		bool smoke = !AZ_VampState.Cfg() || AZ_VampState.Cfg().offerSmoke;
		int black = AZ_VampCrypt.FADE_MS;
		if (smoke)
			black = OFFER_BLACK_MS;
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampCrypt.RPC_FADE, new Param1<int>(black), true, p.GetIdentity());
		if (smoke)
			OfferJuncture(p, 3);
		OfferJuncture(p, 2);
		s_OfferEndAt = GetGame().GetTime();
		if (s_OfferNeedsBlood)
			GiveBlood(p);
		s_OfferQuietUntil = GetGame().GetTime() + 600 + SETTLE_MS + 2000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(OfferBack, 600, false, p);
	}

	static void OfferBack(PlayerBase p)
	{
		if (!p)
			return;
		if (p == s_Lying && s_Site && p.IsAlive())
		{
			AZ_VampSnap.To(p, s_Site.m_Lie, s_Site.m_LieDir);
		}
		FreeHands(p, 0);
		p.m_AZOffering = false;
		p.SetSynchDirty();
		Hold(p);
		Print("[AZ_Vamp] ritual offering done: " + AZ_VampState.Uid(p) + " back on the slab, alive " + p.IsAlive() + " health " + p.GetHealth("", "Health"));
	}



	static void FreeHands(PlayerBase p, int tries)
	{
		if (!p || !p.IsAlive())
			return;
		EntityAI item = p.GetItemInHands();
		if (!item || (p == s_Bound && item == p.m_AZWillBonds))
			return;
		string how = "stowed";
		if (!p.ServerTakeEntityToInventory(FindInventoryLocationType.ATTACHMENT | FindInventoryLocationType.CARGO, item))
		{
			how = "laid on the slab";
			if (!p.ServerDropEntity(item))
				how = "still in hand";
		}
		Print("[AZ_Vamp] ritual: " + item.GetType() + " " + how + " for the lie-down, try " + tries);
		if (how == "still in hand" && tries < 3)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FreeHands, 1000, false, p, tries + 1);
	}


	static void StopLying(PlayerBase p)
	{
		if (p && p.GetEmoteManager() && p.GetEmoteManager().IsEmotePlaying())
			p.GetEmoteManager().ServerRequestEmoteCancel();
	}

	static bool Running(PlayerBase p, typename t)
	{
		if (!p || !p.GetActionManager())
			return false;
		ActionBase a = p.GetActionManager().GetRunningAction();
		return a && a.Type() == t;
	}


	static const float TOOL_LIFETIME_S = 3888000;
	static const int MATCH_MIN = 5;
	static bool s_ToolWasNight;
	static ref array<string> s_ToolTypes = {"FangeKnife", "Matchbox"};
	static ref array<EntityAI> s_Tools = {null, null};
	static ref array<int> s_ToolGoneAt = {0, 0};
	static ref array<int> s_ToolHoardAt = {0, 0};

	static vector ToolHome(AZ_VampRitualSite s, int i)
	{
		float side = 0.18;
		if (i == 1)
			side = -0.18;
		return s.m_Lie + s.m_LieDir * (s.m_HalfLen - 0.22) + s.m_Back * side + Vector(0, 0.03, 0);
	}

	static void Tools()
	{
		AZ_VampRitualSite s = null;
		foreach (AZ_VampRitualSite c : s_Sites)
		{
			if (c && c.m_InCrypt && c.m_Slab)
				s = c;
		}
		if (!s)
			return;
		int now = GetGame().GetTime();
		float returnMs = Math.Max(1200, AZ_VampState.Cfg().ritualToolReturnSec) * 1000;
		bool night = GetGame().GetWorld().IsNight();
		bool dusk = night && !s_ToolWasNight;
		s_ToolWasNight = night;
		for (int i = 0; i < s_ToolTypes.Count(); i++)
		{
			EntityAI t = s_Tools[i];
			if (t && !t.IsRuined() && !t.GetHierarchyParent())
			{
				t.SetLifetimeMax(TOOL_LIFETIME_S);
				t.SetLifetime(TOOL_LIFETIME_S);
			}
			vector home = ToolHome(s, i);
			bool here = t && !t.IsRuined() && !t.GetHierarchyParent() && vector.Distance(t.GetPosition(), home) < 0.6;
			if (here)
			{
				s_ToolGoneAt[i] = 0;
				s_ToolHoardAt[i] = 0;


				ItemBase box = ItemBase.Cast(t);
				if (box && box.HasQuantity() && (dusk || box.GetQuantity() < MATCH_MIN) && box.GetQuantity() < box.GetQuantityMax())
				{
					float was = box.GetQuantity();
					box.SetQuantityMax();
					Print("[AZ_Vamp] ritual: " + t.GetType() + " on the slab filled " + was + " -> " + box.GetQuantity() + " (nightfall " + dusk + ")");
				}
				continue;
			}

			vector tpos = Vector(0, 0, 0);
			PlayerBase carrier = null;
			if (t)
			{
				tpos = t.GetPosition();
				carrier = PlayerBase.Cast(t.GetHierarchyRootPlayer());
				if (carrier)
					tpos = carrier.GetPosition();
			}
			if (carrier && carrier.IsAlive() && (AZ_VampCrypt.Inside(tpos) || vector.Distance(tpos, home) <= 50))
			{
				s_ToolGoneAt[i] = 0;
				s_ToolHoardAt[i] = 0;
				continue;
			}
			if (t && !t.IsRuined() && AZ_VampCrypt.Inside(tpos))
			{
				s_ToolGoneAt[i] = 0;
				if (s_ToolHoardAt[i] == 0)
					s_ToolHoardAt[i] = now;
				if (now - s_ToolHoardAt[i] < returnMs)
					continue;

				Print("[AZ_Vamp] ritual: " + t.GetType() + " kept off the slab for " + (returnMs / 60000) + " min inside the crypt, a fresh one is laid");
				t = null;
			}
			else
				s_ToolHoardAt[i] = 0;
			if (s_ToolGoneAt[i] == 0 && t)
			{
				s_ToolGoneAt[i] = now;
				continue;
			}
			if (t && now - s_ToolGoneAt[i] < returnMs)
				continue;
			if (t && t.GetHierarchyParent())
				t = null;
			if (t)
			{
				Print("[AZ_Vamp] ritual: the old " + t.GetType() + " at " + t.GetPosition() + " goes before a new one is laid");
				GetGame().ObjectDelete(t);
			}
			EntityAI n = EntityAI.Cast(GetGame().CreateObjectEx(s_ToolTypes[i], home, ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY));
			if (!n)
				continue;
			n.SetLifetimeMax(TOOL_LIFETIME_S);
			n.SetLifetime(TOOL_LIFETIME_S);
			n.SetPosition(home);
			n.SetOrientation(Vector(s.m_LieDir.VectorToAngles()[0] + 90, 0, 0));
			s_Tools[i] = n;
			s_ToolGoneAt[i] = 0;
			Print("[AZ_Vamp] ritual: " + s_ToolTypes[i] + " laid on the slab at " + home);
		}
	}


	static void Tick()
	{
		if (!GetGame().GetWorld())
			return;
		bool night = GetGame().GetWorld().IsNight();
		if (!night)
			s_AnswersTonight = 0;
		Tools();
		if (s_State == ST_SLEEP)
			return;
		DrumEyes(s_Site);
		if (s_State != ST_CLIMAX && (!night || !AZ_VampEvent.Active()))
		{
			string why = "dawn";
			if (!AZ_VampEvent.Active())
				why = "the event ended";
			Abort(why);
			return;
		}
		if (!s_Site)
		{
			Reset();
			return;
		}

		if (s_State < ST_CLIMAX && !AZ_VampAltar.CapWithPending())
		{
			Abort("cap reached during the ritual", CAP_REACHED_TEXT);
			return;
		}
		int now = GetGame().GetTime();
		Tell(now);
		CheckKneelers(now);
		EmoteKneelers(now);
		Music();
		Arm();
		AZ_VampInvite.Scan();
		if (s_Lying && !s_Lying.IsAlive())
		{
			Abort("the offering died");
			return;
		}
		if (s_Lying && !s_Lying.m_AZOffering)
			Hold(s_Lying);
		if (s_State != ST_LYING)
			return;


		PlayerBase p = s_Lying;
		string bad = "";
		if (!p)
			bad = "left the server";
		else if (!p.IsAlive())
			bad = "died";
		else if (p.IsUnconscious() || (p.IsRestrained() && p != s_Bound) || p.m_AZVampHeld || p.m_AZVampire)
			bad = "knocked out, tied or taken";


		else if (!Offering(p, now) && (vector.Distance(p.GetPosition(), s_Site.m_Lie) > ALTAR_PULL_M || (now - s_LieAt > SETTLE_MS && !p.IsRestrained() && !p.GetEmoteManager().IsEmotePlaying())))
		{


			s_HoldPulls++;
			AZ_VampSnap.To(p, s_Site.m_Lie, s_Site.m_LieDir);
			if (now - s_HoldPullAt > 8000)
			{
				s_HoldPullAt = now;
				AZ_VampRitual.Note(p, "The altar", "The stone will not let you rise.");
				Print("[AZ_Vamp] altar hold " + AZ_VampState.Uid(p) + " held, pull " + s_HoldPulls + " (" + ((now - s_LieAt) / 1000) + " s in)");
			}
		}
		if (bad == "" && p.m_AZOffering && p.GetHealth("", "Health") < OFFER_HEALTH_FLOOR)
			p.SetHealth("", "Health", OFFER_HEALTH_FLOOR);
		if (bad != "")
		{
			string who = "";
			if (p)
				who = AZ_VampState.Uid(p) + " ";


			if (p && (bad == "got up" || bad == "left the slab") && (p.m_AZLastPlayerHitAt == 0 || now - p.m_AZLastPlayerHitAt > BAN_HIT_MS))
				Ban(p);
			if (p)
				Print("[AZ_Vamp] altar hold " + who + "released " + bad + " after " + ((now - s_LieAt) / 1000) + " s, " + s_HoldPulls + " pulls");
			Abort(who + bad);
			return;
		}
		AZ_VampCrypt.MarkRitual(p);

		int n = s_Kneelers.Count();
		float need = ALONE_S;
		if (n == 1)
			need = ONE_S;
		else if (n >= 2)
			need = MANY_S;
		s_Progress += TICK_MS * 0.001 / need;
		if (s_Hint == 0 && s_Progress >= 0.5)
		{
			s_Hint = 1;
			Say(s_Site, "The stone under the one on the altar grows warm.");
			Print("[AZ_Vamp] ritual progress 50% (" + n + " kneeling)");
		}
		else if (s_Hint == 1 && s_Progress >= 0.85)
		{
			s_Hint = 2;
			Say(s_Site, "Something below the crypt is listening.");
			Print("[AZ_Vamp] ritual progress 85% (" + n + " kneeling)");
		}


		float toPeak = (1 - s_Progress) * need - PEAK_MS * 0.001;
		if (toPeak <= DIM_S && now >= s_DimFlickAt)
		{
			s_DimFlickAt = now + DIM_FLICK_MIN_MS + (DIM_FLICK_MAX_MS - DIM_FLICK_MIN_MS) * Math.Max(0, toPeak) / DIM_S;
			foreach (AZ_VampRitualCandle dc : s_Site.m_Candles)
			{
				if (dc)
				{
					dc.AZ_SetDim(true);
					if (Math.RandomFloat01() < 0.6)
						dc.AZ_Flicker();
				}
			}
		}
		if (s_PeakAt == 0 && (1 - s_Progress) * need * 1000 <= PEAK_MS)
		{
			if (!AZ_VampAltar.CapWithPending())
			{
				Abort("cap reached at the peak", CAP_REACHED_TEXT);
				return;
			}
			s_PeakAt = now;
			Music();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PeakDue, PEAK_MS - BLACK_AT_MS, false, now);
			Print("[AZ_Vamp] ritual peak (" + n + " kneeling)");
		}
	}


	static void PeakDue(int peakAt)
	{
		if (s_State == ST_LYING && s_PeakAt == peakAt && s_Lying)
			Climax();
	}

	static void CheckKneelers(int now)
	{
		for (int i = s_Kneelers.Count() - 1; i >= 0; i--)
		{
			PlayerBase k = s_Kneelers[i];
			string kb = "";
			vector kspot = s_Site.m_Kneel[s_KneelSpot[i]];
			vector kpos;
			if (k)
				kpos = k.GetPosition();
			if (!k || !k.IsAlive())
				kb = "gone";
			else if (k.IsUnconscious() || k.IsRestrained() || k.m_AZVampHeld || k.m_AZVampire)
				kb = "knocked out, tied or taken";
			else if (s_EmoteKneel.Find(k) >= 0)
			{
				if (vector.Distance(kpos, kspot) > EMOTE_SPOT_M)
					kb = "moved";
				else if (!SitEmote(k))
					kb = "stood up";
			}
			else if (vector.Distance(kpos, kspot) > KNEEL_DRIFT_M)
				kb = "moved";
			else if (now - s_KneelAt[i] > SETTLE_MS && !Running(k, ActionAZKneel))
				kb = "stopped";
			else if (s_State != ST_LYING && now - s_KneelAt[i] > KNEEL_IDLE_MS)
				kb = "knelt too long with nobody on the altar";
			if (kb != "")
			{
				if (k && kb == "struck")
					AZ_VampRitual.Note(k, "The altar", "The blow breaks your prayer. Kneel again.");
				if (k && kb == "knelt too long with nobody on the altar")
					AZ_VampRitual.Note(k, "The altar", "Your knees ache. No one lies on the stone.");
				DropKneeler(i, kb);
				if (Running(k, ActionAZKneel))
					Interrupt(k);
				continue;
			}
			if (s_State == ST_LYING)
				AZ_VampCrypt.MarkRitual(k);
		}
	}

	static const int KNEEL_IDLE_MS = 90000;


	static void Music()
	{
		if (s_State == ST_CLIMAX)
			return;
		int bits = 0;
		if (s_Site && s_PeakAt > 0)
			bits = MUS_PEAK;
		else if (s_Site && s_State >= ST_LIGHTING && s_State < ST_CLIMAX)
		{
			bits = MUS_DRONE;
			if (s_State >= ST_POURED)
				bits = bits | MUS_PULSE;
			if (s_Kneelers.Count() > 0)
				bits = bits | MUS_CHOIR;
		}
		SetMusic(s_Site, bits);
	}

	static void SetMusic(AZ_VampRitualSite s, int bits)
	{
		if (!s || s.m_Candles.Count() == 0 || !s.m_Candles[0])
			return;
		s.m_Candles[0].AZ_SetMusic(bits);
	}


	static void Call(AZ_VampRitualSite s)
	{
		if (s && s.m_Candles.Count() > 0 && s.m_Candles[0])
			s.m_Candles[0].AZ_Call();
	}


	static void Tell(int now)
	{
		if (now < s_TellAt || s_State >= ST_CLIMAX)
			return;
		s_TellAt = now + TELL_MS;
		array<PlayerBase> list;
		Area(s_Site, list);
		bool vamp = false;
		foreach (PlayerBase v : list)
		{
			if (v.m_AZVampire)
				vamp = true;
		}
		if (!vamp)
			return;
		Gutter(s_Site);
		foreach (PlayerBase h : list)
		{
			if (!h.m_AZVampire && h.GetIdentity())
				GetGame().RPCSingleParam(h, AZ_VampCrypt.RPC_WHISPER, new Param1<int>(1), true, h.GetIdentity());
		}
		if (!s_Told)
		{
			s_Told = true;
			Print("[AZ_Vamp] ritual tell: a vampire near the altar");
		}
	}

	static void Gutter(AZ_VampRitualSite s)
	{
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c)
				c.AZ_Flicker();
		}
	}


	static void Climax()
	{
		PlayerBase p = s_Lying;
		AZ_VampRitualSite s = s_Site;
		if (!AZ_VampAltar.CapWithPending())
		{
			Abort("cap reached at the climax", CAP_REACHED_TEXT);
			return;
		}
		s_State = ST_CLIMAX;
		s_AnswersTonight++;
		AZ_VampAltar.s_ElderPending = false;
		AZ_VampAltar.s_ElderDue = false;
		string uid = AZ_VampState.Uid(p);
		AZ_VampAltar.s_Pending.Set(uid, true);
		Gutter(s);
		AZ_VampCrypt.StatueEyes(s.m_Center, true);
		if (s.m_InCrypt)
			AZ_VampCrypt.FireFlare();
		if (s.m_InCrypt)
			Say(s, "The candles gutter. High above the altar, something opens its eyes.");
		else
			Say(s, "The candles gutter. The dark around the altar thickens.");
		Print("[AZ_Vamp] ritual climax " + uid + " with " + s_Kneelers.Count() + " kneeling");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Blackout, BLACK_AT_MS, false, p, uid);
	}

	static void Blackout(PlayerBase p, string uid)
	{
		AZ_VampRitualSite s = s_Site;
		if (!s)
			return;
		s_AnswerGeneration++;
		array<PlayerBase> list;
		Area(s, list);
		foreach (PlayerBase w : list)
		{
			if (w.GetIdentity())
				GetGame().RPCSingleParam(w, AZ_VampCrypt.RPC_FADE, new Param1<int>(BLACK_MS), true, w.GetIdentity());
		}
		foreach (AZ_VampRitualCandle c : s.m_Candles)
		{
			if (c)
				c.AZ_SetLit(false);
		}
		SetMusic(s, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Call, 1500, false, s);
		PlayerBase elder = null;
		if (s.m_InCrypt && p && p.IsAlive())
		{


			s_Lying = null;
			ReleaseHold(p);
			p.m_AZOnAltar = false;
			p.SetSynchDirty();
			StopLying(p);
			ClearOffer(p);
			vector river = AZ_VampCryptLurkers.RiverSpot();
			vector face = AZ_VampCryptLurkers.RiverFace();


			// AZ-2431: altar to river is a trusted in-crypt move; Snap.To grants the COT allowance and syncs the client.
			AZ_VampSnap.To(p, river, face.Normalized());
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptLurkers.RiverFeed, BLACK_MS, false, p, river, face);

			Print("[AZ_Vamp] ritual blackout: " + list.Count() + " there, the river feed on " + uid + " at " + river);

			AZ_VampCryptLurkers.s_RiverWakeUid = uid;
			AZ_VampCryptLurkers.s_RiverWakeLine = "They gave you to the night. Now they kneel within reach.";
			AZ_VampCryptLurkers.s_RiverWakeArmed = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptLurkers.RiverSafety, BLACK_MS + AZ_VampCryptLurkers.RiverSafetyMs(), false, p);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EyesOff, EYES_FADE_MS, false, s.m_Center, s_AnswerGeneration);
			return;
		}
		else
		{
			if (p && p.IsAlive())
				p.SetHealth("", "Blood", AZ_VampState.Cfg().bloodFloor);
			vector dir = s.m_Lie - s.m_Head;
			dir[1] = 0;
			dir.Normalize();
			elder = AZ_VampEvent.AIVampire(AZ_VampEvent.VampType(""), s.m_Head, dir);
			if (elder)
			{
				elder.SetPosition(s.m_Head);
				elder.SetDirection(dir);
			}
		}
		Print("[AZ_Vamp] ritual blackout: " + list.Count() + " there, the bite on " + uid + ", the Elder " + (elder != null) + " at " + s.m_Head);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderGone, BLACK_MS + ELDER_SHOW_MS, false, p, elder);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampAltar.Wake, WAKE_AFTER_BITE_MS, false, p, uid, "They gave you to the night. Now they kneel within reach.");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EyesOff, EYES_FADE_MS, false, s.m_Center, s_AnswerGeneration);
	}

	static void ElderGone(PlayerBase p, PlayerBase elder)
	{
		if (elder)
		{
			GetGame().RPCSingleParam(elder, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(elder.GetPosition()), true);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 1500, false, elder);
		}
		if (p && p.IsAlive())
		{
			p.SetHealth("", "Shock", 0);
			AZ_VampRitual.Note(p, "The altar", "Cold lips at your throat. The dark takes you.");
		}
		if (s_Site && s_Site.m_InCrypt)
			AZ_VampCrypt.BloodRise();
		Print("[AZ_Vamp] ritual elder gone in smoke");

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampCryptLurkers.RiverSafety);
		s_Ending = true;
		Reset();
		FinishAnswer();
	}


	static void FinishAnswer()
	{
		if (AZ_VampAltar.s_Pending.Count() > 0 || AZ_VampCryptLurkers.s_RiverWakeArmed || AZ_VampCryptLurkers.s_River || AZ_VampCryptLurkers.s_Audience)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FinishAnswer, 1000, false);
			return;
		}
		s_Ending = false;
	}

	static void EyesOff(vector at, int generation)
	{
		if (generation != s_AnswerGeneration && s_State == ST_CLIMAX && s_Site && s_Site.m_Center == at)
			return;
		AZ_VampCrypt.StatueEyes(at, false);
	}


	static void Abort(string why, string text = "The candles die. The altar sleeps again.")
	{
		if (s_Site)
		{
			Say(s_Site, text);
			foreach (AZ_VampRitualCandle c : s_Site.m_Candles)
			{
				if (c)
					c.AZ_SetLit(false);
			}
		}
		Print("[AZ_Vamp] ritual abort (" + why + ")");
		Reset();
	}



	static void DrumEyes(AZ_VampRitualSite s)
	{
		AZ_VampAltar_Drum drum = null;
		if (s)
			drum = AZ_VampAltar_Drum.Cast(s.m_Slab);
		else if (s_Site)
			drum = AZ_VampAltar_Drum.Cast(s_Site.m_Slab);
		if (!drum)
			return;
		int level = 0;
		if (s)
		{
			int lit = Lit(s);
			if (lit >= 1)
				level = 1;
			if (lit >= 3)
				level = 2;
			if (s_PeakAt > 0 && lit > 0)
				level = 3;
		}
		drum.AZ_SetEyeLevel(level);
	}

	static void Reset()
	{
		AZ_VampInvite.EndAll();
		PlayerBase p = s_Lying;
		s_Lying = null;
		if (p)
		{
			ReleaseHold(p);
			p.m_AZOnAltar = false;
			p.SetSynchDirty();
			StopLying(p);
			ClearOffer(p);
		}

		foreach (PlayerBase armed : s_Armed)
		{
			if (armed)
			{
				armed.m_AZAltarArmed = false;
				armed.SetSynchDirty();
			}
		}
		s_Armed.Clear();
		for (int i = s_Kneelers.Count() - 1; i >= 0; i--)
		{
			PlayerBase k = s_Kneelers[i];
			DropKneeler(i, "the ritual ended");
			if (Running(k, ActionAZKneel))
				Interrupt(k);
		}
		SetMusic(s_Site, 0);
		SetPoured(s_Site, false);
		DrumEyes(null);
		s_PeakAt = 0;
		s_DimFlickAt = 0;
		s_State = ST_SLEEP;
		s_Site = null;
		s_Progress = 0;
		s_Hint = 0;
		s_Told = false;
		Print("[AZ_Vamp] ritual reset");
	}
}


class AZ_VampRitualCandle extends Candle
{
	protected bool m_AZLit;
	protected int m_AZFlicker;
	protected int m_AZFlickerSeen;
	protected bool m_AZDim;
	protected bool m_AZPoured;
	protected bool m_AZDimSeen;
	protected AZ_VampCandleLight m_AZLight;
	protected int m_AZMusic;
	protected int m_AZCall;
	protected int m_AZCallSeen;
	protected ref array<EffectSound> m_AZSnd;
	protected bool m_AZHushing;
	protected bool m_AZHushed;



	static bool s_AZHear;
	static ref array<AZ_VampRitualCandle> s_AZMusicCandles;
	static const float PULSE_FADE_S = 3.0;


	static void AZ_ApplyDuck(float level)
	{
		if (!s_AZMusicCandles)
			return;
		foreach (AZ_VampRitualCandle c : s_AZMusicCandles)
		{
			if (!c || !c.m_AZSnd)
				continue;
			foreach (EffectSound e : c.m_AZSnd)
			{
				if (e)
					e.SetSoundVolume(level);
			}
		}
	}


	static void AZ_ApplyPitch(float freq)
	{
		if (!s_AZMusicCandles)
			return;
		foreach (AZ_VampRitualCandle c : s_AZMusicCandles)
		{
			if (!c || !c.m_AZSnd || !c.m_AZSnd[0])
				continue;
			AbstractWave w;
			EnScript.GetClassVar(c.m_AZSnd[0], "m_SoundWaveObject", 0, w);
			if (w)
				w.SetFrequency(freq);
		}
	}

	static void AZ_SetHear(bool hear)
	{
		if (hear == s_AZHear)
			return;
		s_AZHear = hear;
		if (!s_AZMusicCandles)
			return;
		foreach (AZ_VampRitualCandle c : s_AZMusicCandles)
		{
			if (c)
				c.AZ_ApplyMusic();
		}
	}

	void AZ_VampRitualCandle()
	{
		RegisterNetSyncVariableBool("m_AZLit");
		RegisterNetSyncVariableBool("m_AZDim");
		RegisterNetSyncVariableBool("m_AZPoured");
		RegisterNetSyncVariableInt("m_AZFlicker", 0, 15);
		RegisterNetSyncVariableInt("m_AZMusic", 0, 15);
		RegisterNetSyncVariableInt("m_AZCall", 0, 15);
		m_AZCallSeen = -1;
	}

	void AZ_SetMusic(int bits)
	{
		if (m_AZMusic == bits)
			return;
		m_AZMusic = bits;
		SetSynchDirty();
	}

	void AZ_Call()
	{
		m_AZCall = (m_AZCall + 1) % 16;
		SetSynchDirty();
	}


	void AZ_ApplyMusic()
	{
		if (!m_AZSnd)
		{
			m_AZSnd = new array<EffectSound>;
			for (int i = 0; i < 4; i++)
				m_AZSnd.Insert(null);
		}
		if (m_AZMusic != 0)
		{
			if (!s_AZMusicCandles)
				s_AZMusicCandles = new array<AZ_VampRitualCandle>;
			if (s_AZMusicCandles.Find(this) < 0)
				s_AZMusicCandles.Insert(this);
		}
		if (m_AZMusic == 0)
			m_AZHushed = false;
		array<string> sets = {"AZ_RitualDrone_SoundSet", "AZ_RitualPulse_SoundSet", "AZ_RitualChoir_SoundSet", "AZ_RitualPeak_SoundSet"};
		for (int b = 0; b < 4; b++)
		{
			bool want = s_AZHear && (m_AZMusic & (1 << b)) != 0;
			EffectSound snd = m_AZSnd[b];
			if (want && !snd && b == 0 && !m_AZHushed && m_AZMusic == 1)
			{

				m_AZHushed = true;
				m_AZHushing = true;
				AZ_VampWhisperLines.Duck(0.0, 0.4);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_HushEnd, 3000, false);
				continue;
			}
			if (want && !snd && b == 0 && m_AZHushing)
				continue;
			if (want && !snd)
			{
				bool loop = b < 3;
				m_AZSnd[b] = SEffectManager.PlaySoundOnObject(sets[b], this, 0, 0.3, loop);
			}
			else if (!want && snd)
			{
				if (b == 1)
					snd.SetSoundFadeOut(PULSE_FADE_S);
				snd.SetSoundAutodestroy(true);
				snd.SoundStop();
				m_AZSnd[b] = null;
			}
		}
	}

	void AZ_HushEnd()
	{
		m_AZHushing = false;
		AZ_VampWhisperLines.Duck(1.0, 4.0);
		AZ_ApplyMusic();
	}

	bool AZ_IsLit()
	{
		return m_AZLit;
	}

	void AZ_SetLit(bool on)
	{
		m_AZLit = on;
		m_AZDim = false;
		SetSynchDirty();
	}

	bool AZ_IsPoured()
	{
		return m_AZPoured;
	}

	void AZ_SetPoured(bool on)
	{
		if (m_AZPoured == on)
			return;
		m_AZPoured = on;
		SetSynchDirty();
	}

	void AZ_SetDim(bool on)
	{
		if (m_AZDim == on)
			return;
		m_AZDim = on;
		SetSynchDirty();
	}

	protected float AZ_Base()
	{
		float s = AZ_VampCryptLights.RitualScale(GetPosition());
		if (m_AZDim)
			return 1.1 * s;
		return 2.2 * s;
	}

	void AZ_Flicker()
	{
		m_AZFlicker = (m_AZFlicker + 1) % 16;
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_AZLit && !m_AZLight)
		{
			SetObjectTexture(0, "dz\\gear\\tools\\data\\candle_flame_ca.paa");
			SetObjectMaterial(0, "dz\\gear\\tools\\data\\candle_flame.rvmat");
			m_AZLight = AZ_VampCandleLight.Cast(AZ_VampLightGate.Light(AZ_VampCandleLight, GetPosition()));
			if (m_AZLight)
			{
				m_AZLight.AttachOnObject(this, "0 0.17 0");
				AZ_VampCryptLights.RitualCandle(m_AZLight, GetPosition());
				m_AZLight.SetBrightnessTo(AZ_Base());
			}
		}
		else if (!m_AZLit && m_AZLight)
		{
			SetObjectTexture(0, "");
			m_AZLight.Destroy();
			m_AZLight = null;
		}
		if (m_AZDim != m_AZDimSeen)
		{
			m_AZDimSeen = m_AZDim;
			if (m_AZLight)
				m_AZLight.SetBrightnessTo(AZ_Base());
		}
		AZ_ApplyMusic();
		if (m_AZCall != m_AZCallSeen)
		{
			if (m_AZCallSeen >= 0)
				SEffectManager.PlaySoundOnObject("AZ_CryptCall_SoundSet", this);
			m_AZCallSeen = m_AZCall;
		}
		if (m_AZFlicker != m_AZFlickerSeen)
		{
			m_AZFlickerSeen = m_AZFlicker;


			if (m_AZLit && m_AZLight)
				Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, Vector(0, 0.2, 0));
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (m_AZLight)
			m_AZLight.Destroy();
		if (m_AZSnd)
		{
			foreach (EffectSound es : m_AZSnd)
			{
				if (es)
				{
					es.SetSoundAutodestroy(true);
					es.SoundStop();
				}
			}
		}
		super.EEDelete(parent);
	}

	override bool IsIgnited()
	{
		return m_AZLit;
	}

	override bool CanBeIgnitedBy(EntityAI igniter = null)
	{
		return false;
	}

	override bool CanIgniteItem(EntityAI ignite_target = null)
	{
		return false;
	}

	override bool IsThisIgnitionSuccessful(EntityAI item_source = null)
	{
		return true;
	}

	override void OnIgnitedThis(EntityAI fire_source)
	{
		if (GetGame().IsServer())
			AZ_VampRitual.CandleLit(this, fire_source);
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool IsTakeable()
	{
		return false;
	}
}


class AZ_VampAltarCircle extends AZ_VampCandleCircle
{
	override void EEInit()
	{
		super.EEInit();
		AZ_VampRitual.Register(this);
	}
}




class ActionAZLightCandleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.FIREPLACE_IGNITE);
	}
}

class ActionAZLightCandle : ActionContinuousBase
{
	static const float REACH_M = 2.4;

	void ActionAZLightCandle()
	{
		m_CallbackClass = ActionAZLightCandleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_STARTFIRE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Light the altar candle";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	static AZ_VampRitualCandle FindCandle(PlayerBase p)
	{
		if (!p || (!GetGame().IsServer() && !AZ_VampRitual.NearAltar(p.GetPosition())))
			return null;
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(p.GetPosition(), REACH_M, near, null);
		vector fwd = p.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		AZ_VampRitualCandle best = null;
		float bestScore = 1000;
		foreach (Object o : near)
		{
			AZ_VampRitualCandle c = AZ_VampRitualCandle.Cast(o);
			if (!c || c.AZ_IsLit())
				continue;
			vector to = c.GetPosition() - p.GetPosition();
			to[1] = 0;
			float d = to.Length();
			to.Normalize();
			float facing = vector.Dot(to, fwd);
			float score = d - facing;
			if (score < bestScore)
			{
				bestScore = score;
				best = c;
			}
		}
		return best;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.m_AZVampire || player.m_AZOnAltar)
			return false;
		ItemBase fire = ItemBase.Cast(player.GetItemInHands());
		if (!fire)
			return false;
		AZ_VampRitualCandle c = FindCandle(player);
		return c && fire.CanIgniteItem(c);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase p = action_data.m_Player;
		ItemBase fire = ItemBase.Cast(p.GetItemInHands());
		AZ_VampRitualCandle c = FindCandle(p);
		if (!fire || !c || !fire.CanIgniteItem(c))
			return;
		fire.OnIgnitedTarget(c);
		c.OnIgnitedThis(fire);
	}
}



class ActionAZOfferSelf : ActionInteractBase
{
	void ActionAZOfferSelf()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Offer yourself";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.m_AZVampire || player.m_AZVampTurning || !GetGame().GetWorld().IsNight())
			return false;
		if (player.GetItemInHands() && !AZ_VampAltar.IsBlade(player.GetItemInHands()))
			return false;
		if (GetGame().IsServer())
			return true;
		if (player.m_AZOnAltar)
			return false;
		int step = AZ_VampRitual.ClientStep(player.GetPosition());
		if (step == AZ_VampRitual.STEP_AWAKE)
			return AZ_VampAltar.IsBlade(player.GetItemInHands());
		return step == AZ_VampRitual.STEP_POURED;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampRitual.Lie(action_data.m_Player);
	}
}


class ActionAZKneelCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(3600);
	}
}

class ActionAZKneel : ActionContinuousBase
{
	void ActionAZKneel()
	{
		m_CallbackClass = ActionAZKneelCB;
		m_CommandUID = DayZPlayerConstants.CMD_GESTUREFB_KNEEL;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Kneel at the altar";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTAZAny;
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.m_AZVampire || player.m_AZOnAltar || !GetGame().GetWorld().IsNight())
			return false;
		if (GetGame().IsServer())
			return true;

		PlayerBase t = PlayerBase.Cast(target.GetObject());
		if (t && t.m_AZOnAltar)
			return false;
		if (AZ_VampInvite.ClientInvited() && AZ_VampCrypt.Inside(player.GetPosition()))
			return true;
		return AZ_VampRitual.ClientStep(player.GetPosition()) == AZ_VampRitual.STEP_LYING;
	}

	override string GetText()
	{
		if (AZ_VampInvite.ClientInvited())
			return "Kneel with them";
		return "Kneel at the altar";
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		if (!GetGame().IsServer())
			return true;
		return AZ_VampRitual.StillKneeling(action_data.m_Player);
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		if (!AZ_VampRitual.Kneel(action_data.m_Player))
			action_data.m_Player.GetActionManager().Interrupt();
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		AZ_VampRitual.Unkneel(action_data.m_Player, "got up");
	}
}


class ActionAZPullOff : ActionInteractBase
{
	void ActionAZPullOff()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Pull them off the altar";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PlayerBase t = PlayerBase.Cast(target.GetObject());
		return player && t && t != player && t.m_AZOnAltar && t.IsAlive();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		AZ_VampRitual.PullOff(action_data.m_Player, PlayerBase.Cast(action_data.m_Target.GetObject()));
	}
}

modded class PlayerBase
{
	bool m_AZOnAltar;
	bool m_AZOffering;
	bool m_AZAltarArmed;
	protected int m_AZAltarSeen;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZOnAltar");
		RegisterNetSyncVariableBool("m_AZOffering");
		RegisterNetSyncVariableBool("m_AZAltarArmed");
	}


	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		int key = 0;
		if (m_AZOnAltar)
			key = 1;
		if (m_AZOnAltar && m_AZOffering)
			key = 2;
		if (key == m_AZAltarSeen)
			return;
		m_AZAltarSeen = key;
		if (!IsControlledPlayer())
			return;
		if (key == 2)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_OfferEmote, 900, false, 0);
		else if (key == 1)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_LieDown, 900, false, 0);
	}


	void AZ_OfferEmote(int tries)
	{
		if (!m_AZOffering || !IsAlive() || !GetEmoteManager() || GetEmoteManager().IsEmotePlaying())
			return;
		GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SUICIDE);
		Print("[AZ_Vamp] ritual offering: emote asked, try " + tries);
		if (tries < 3)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_OfferEmote, 1000, false, tries + 1);
	}

	override void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		super.OnSyncJuncture(pJunctureID, pCtx);
		if (pJunctureID != AZ_VampRitual.SJ_OFFER)
			return;
		int what;
		if (!pCtx.Read(what))
			return;
		if (what == 3)
		{
			AZ_OfferVanish();
			return;
		}
		if (GetEmoteManager())
			GetEmoteManager().AZ_OfferJuncture(what);
	}


	void AZ_OfferVanish()
	{
		if (!GetGame().IsClient())
			return;
		vector at = GetPosition();
		vector side = GetDirection() * vector.Up;
		side[1] = 0;
		if (side.Length() < 0.1)
			side = Vector(1, 0, 0);
		side.Normalize();
		vector from = at - side * 0.9;
		vector to = at + side * 0.9;
		AZ_VampRitual.SmokeLine(from, to);
		AZ_OfferHide(true);
		Print("[AZ_Vamp] ritual offering: gone in smoke (client)");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_OfferAppear, AZ_VampRitual.OFFER_HIDE_MS, false);
	}


	void AZ_OfferHide(bool hide)
	{
		SetInvisible(hide);
		SetInvisibleRecursive(hide);
		EntityAI held = GetItemInHands();
		if (held)
			held.SetInvisible(hide);
	}

	void AZ_OfferAppear()
	{
		AZ_OfferHide(false);
		vector at = GetPosition();
		Particle.PlayInWorld(ParticleList.AZ_VAMP_ASH_BURST, at + Vector(0, 0.5, 0));
		Print("[AZ_Vamp] ritual offering: back out of the smoke on the slab (client)");
	}


	void AZ_LieDown(int tries)
	{
		if (!m_AZOnAltar || m_AZOffering || !IsAlive() || !GetEmoteManager())
			return;

		if (!IsRestrained() && !GetEmoteManager().IsEmotePlaying())
		{
			GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_LYINGDOWN);
			if (tries < 5 || tries % 10 == 0)
				Print("[AZ_Vamp] ritual lie: emote asked, try " + tries);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_LieDown, 500, false, tries + 1);
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZLightCandle, InputActionMap);
		AddAction(ActionAZOfferSelf, InputActionMap);
		AddAction(ActionAZKneel, InputActionMap);
		AddAction(ActionAZPullOff, InputActionMap);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZLightCandle);
		actions.Insert(ActionAZOfferSelf);
		actions.Insert(ActionAZKneel);
		actions.Insert(ActionAZPullOff);
	}
}


modded class BleedingSourcesManagerServer
{
	void AZ_StopBits(int bits)
	{
		if ((m_Player.GetBleedingBits() & bits) != 0)
			RemoveBleedingSource(bits);
	}
}

modded class AZ_VampConfig
{
	bool offerSmoke = true;
	int ritualsPerNight = 3;
	bool ritualStab = true;
}



modded class EmoteManager
{
	void AZ_OfferJuncture(int what)
	{
		if (!m_Callback)
			return;
		if (what == 1 && m_Callback.GetState() == HumanCommandActionCallback.STATE_LOOP_LOOP)
			CommitSuicide();
		else if (what == 2)
			InterruptCallbackCommand();
	}

	int AZ_GestureID()
	{
		return m_CurrentGestureID;
	}

	protected bool AZ_Safe()
	{
		return m_Player && (m_Player.m_AZOffering || m_Player.m_AZOnAltar || m_Player.m_AZAltarArmed || AZ_VampCrypt.Inside(m_Player.GetPosition()));
	}

	override bool PlayEmote(int id)
	{
		bool played = super.PlayEmote(id);
		if (played && id == EmoteConstants.ID_EMOTE_SUICIDE && GetGame().IsServer() && m_Player && m_Player.m_AZAltarArmed && !m_Player.m_AZOffering)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampRitual.F11Offer, 0, false, m_Player);
		return played;
	}



	static int s_AZNoSuicideMsgAt;

	void AZ_Why(string text)
	{
		if (!GetGame().IsClient() || GetGame().GetTime() < s_AZNoSuicideMsgAt)
			return;
		s_AZNoSuicideMsgAt = GetGame().GetTime() + 3000;
		NotificationSystem.AddNotificationExtended(5, "The Countess", text);
	}

	string AZ_WhyNot()
	{
		if (m_Player.m_AZVampire)
			return "The altar does not want the dead.";
		if (m_Player.m_AZVampTurning || m_Player.m_AZVampHeld)
			return "Not like this. Not while bitten or held.";
		int step = AZ_VampRitual.ClientStep(m_Player.GetPosition());
		if (step == AZ_VampRitual.STEP_NONE)
			return "The altar is asleep. Light all five candles first.";
		if (step == AZ_VampRitual.STEP_LYING)
			return "Someone already lies on the altar.";
		if (!AZ_VampAltar.IsBlade(m_Player.GetItemInHands()))
			return "The altar wants blood. Hold a blade, stand at the slab.";
		return "Step up to the slab with the blade in hand.";
	}

	override bool CanPlayEmote(int id)
	{
		if (id == EmoteConstants.ID_EMOTE_SUICIDE && m_Player && !m_Player.m_AZOffering && AZ_VampCrypt.Inside(m_Player.GetPosition()))
		{
			if (!m_Player.m_AZAltarArmed)
			{
				AZ_Why(AZ_WhyNot());
				return false;
			}
			bool ok = super.CanPlayEmote(id);
			if (!ok && !IsEmotePlaying())
				AZ_Why("Ready the blade first: crouch for a knife, stand for a sword or axe.");
			return ok;
		}
		return super.CanPlayEmote(id);
	}

	override protected void CommitSuicide()
	{
		if (AZ_Safe())
		{
			if (!m_Callback)
				return;
			if (Weapon_Base.Cast(m_Player.GetItemInHands()))
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			else
				m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			return;
		}
		super.CommitSuicide();
	}

	override void KillPlayer()
	{
		if (AZ_Safe())
		{
			Print("[AZ_Vamp] ritual: a suicide kill on the altar blocked");
			return;
		}
		super.KillPlayer();
	}

	override void CreateBleedingEffect(int Callback_ID)
	{
		if (AZ_Safe())
			return;
		super.CreateBleedingEffect(Callback_ID);
	}
}

modded class EmoteCB
{
	override void OnAnimationEvent(int pEventID)
	{
		if (m_player && (m_player.m_AZOffering || m_player.m_AZOnAltar || m_player.m_AZAltarArmed || AZ_VampCrypt.Inside(m_player.GetPosition())))
			return;
		super.OnAnimationEvent(pEventID);
	}
}

modded class ActionUnrestrainSelf
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player && player.m_AZOnAltar)
			return false;
		return super.ActionCondition(player, target, item);
	}
}
