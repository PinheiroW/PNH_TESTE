














modded class AZ_VampConfig
{
	bool stalkWhispers = false;
}

class AZ_VampWhisperLines
{
	static const int RPC_LINE = 4702243;
	static const int RPC_CIRCLE = 4702244;
	static const int RPC_RITUAL = 4702245;





	static int s_QuietUntil;
	static bool s_QuietDucked;

	static bool Quiet()
	{
		return GetGame().GetTime() < s_QuietUntil || s_DuckLevel < 0.5;
	}

	static float s_DuckLevel = 1.0;
	static float s_DuckTarget = 1.0;
	static float s_DuckRate = 1.0;

	static void Duck(float level, float seconds)
	{
		s_DuckTarget = Math.Clamp(level, 0, 1);
		s_DuckRate = Math.AbsFloat(s_DuckTarget - s_DuckLevel) / Math.Max(seconds, 0.05);
	}


	static bool DuckTick(float dt)
	{
		if (s_DuckLevel == s_DuckTarget)
			return s_DuckLevel < 1.0;
		float step = s_DuckRate * dt;
		if (s_DuckLevel < s_DuckTarget)
			s_DuckLevel = Math.Min(s_DuckTarget, s_DuckLevel + step);
		else
			s_DuckLevel = Math.Max(s_DuckTarget, s_DuckLevel - step);
		return true;
	}

	static const int RIT_OFF = 0;
	static const int RIT_LIT = 1;
	static const int RIT_NEAR = 2;
	static const int RIT_PEAK = 3;
	static const float RIT_NEAR_S = 60;
	static const float RIT_AREA_M = 40;
	static const float CHANCE = 0.3;
	static const int MAX_PER_NIGHT = 2;
	static const int STALK_TICK_MS = 5000;
	static const float STALK_M = 20;
	static const int STALK_COOLDOWN_MS = 105000;

	static ref map<string, ref array<string>> s_Heard;
	static ref map<string, int> s_Night;
	static ref map<string, int> s_StalkAt;
	static bool s_StalkOn;

	static array<string> Pool(string moment)
	{
		array<string> pool = new array<string>;
		pool.Insert(moment + "_1");
		pool.Insert(moment + "_2");
		return pool;
	}

	static string NightKey()
	{
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		if (h < 12)
			d = d - 1;
		return y.ToString() + "-" + mo.ToString() + "-" + d.ToString();
	}

	static void Trigger(PlayerBase h, string moment)
	{
		if (!GetGame().IsServer() || !h || !h.IsAlive() || h.m_AZVampire || !h.GetIdentity())
			return;
		if (!s_Heard)
		{
			s_Heard = new map<string, ref array<string>>;
			s_Night = new map<string, int>;
		}
		string uid = h.GetIdentity().GetId();
		string nk = uid + "|" + NightKey();
		if (s_Night.Get(nk) >= MAX_PER_NIGHT)
			return;
		if (Math.RandomFloat01() > CHANCE)
		{
			Print("[AZ_Vamp] whisper line " + moment + " skipped (chance) for " + uid);
			return;
		}
		array<string> heard = s_Heard.Get(uid);
		if (!heard)
		{
			heard = new array<string>;
			s_Heard.Set(uid, heard);
		}
		array<string> left = new array<string>;


		array<string> pool = Pool(moment);
		if (!pool)
			return;
		foreach (string n : pool)
		{
			if (heard.Find(n) < 0)
				left.Insert(n);
		}
		if (left.Count() == 0)
			return;
		string line = left.GetRandomElement();
		heard.Insert(line);
		s_Night.Set(nk, s_Night.Get(nk) + 1);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Send, Math.RandomIntInclusive(2000, 8000), false, h, line);
	}

	static void Send(PlayerBase h, string line)
	{
		if (!h || !h.IsAlive() || !h.GetIdentity())
			return;
		GetGame().RPCSingleParam(h, RPC_LINE, new Param1<string>(line), true, h.GetIdentity());
		Print("[AZ_Vamp] whisper line " + line + " for " + h.GetIdentity().GetId());
	}


	static void Circle(PlayerBase h, vector center, vector toward, int count = 3)
	{
		if (!GetGame().IsServer() || !h || !h.GetIdentity() || h.m_AZVampire)
			return;
		GetGame().RPCSingleParam(h, RPC_CIRCLE, new Param3<vector, vector, int>(center, toward, count), true, h.GetIdentity());
	}


	static void StartStalk()
	{
		if (s_StalkOn || !GetGame().IsServer())
			return;
		s_StalkOn = true;
		s_StalkAt = new map<string, int>;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StalkTick, STALK_TICK_MS, true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RitualTick, 1000, true);
	}



	static int s_RitStage;
	static int s_RitKneel;
	static int s_RitQuarter;
	static int s_RitSentAt;

	static void RitualTick()
	{
		AZ_VampRitualSite site = AZ_VampRitual.s_Site;
		int stage = RIT_OFF;
		int kneel = 0;
		int quarter = 0;
		if (site && AZ_VampRitual.s_State >= AZ_VampRitual.ST_LIGHTING)
		{
			stage = RIT_LIT;
			kneel = AZ_VampRitual.s_Kneelers.Count();
			if (AZ_VampRitual.s_PeakAt > 0 || AZ_VampRitual.s_State >= AZ_VampRitual.ST_CLIMAX)
				stage = RIT_PEAK;
			else if (AZ_VampRitual.s_State == AZ_VampRitual.ST_LYING)
			{
				float need = AZ_VampRitual.ALONE_S;
				if (kneel == 1)
					need = AZ_VampRitual.ONE_S;
				else if (kneel >= 2)
					need = AZ_VampRitual.MANY_S;
				quarter = Math.Clamp(Math.Floor(AZ_VampRitual.s_Progress * 4), 0, 3);
				float left = (1 - AZ_VampRitual.s_Progress) * need - AZ_VampRitual.PEAK_MS * 0.001;
				if (left <= RIT_NEAR_S)
					stage = RIT_NEAR;
			}
		}
		int now = GetGame().GetTime();
		if (stage == s_RitStage && kneel == s_RitKneel && quarter == s_RitQuarter && (stage == RIT_OFF || now - s_RitSentAt < 4000))
			return;
		if (stage != s_RitStage)
			Print("[AZ_Vamp] ritual sound stage " + stage + " (" + kneel + " kneeling)");
		s_RitStage = stage;
		s_RitKneel = kneel;
		s_RitQuarter = quarter;
		s_RitSentAt = now;
		if (!site)
			return;
		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);
		foreach (Man m : men)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (pb && pb.GetIdentity() && vector.Distance(pb.GetPosition(), site.m_Center) < RIT_AREA_M)
				GetGame().RPCSingleParam(pb, RPC_RITUAL, new Param4<vector, int, int, int>(site.m_Center, stage, kneel, quarter), true, pb.GetIdentity());
		}
	}

	static void StalkTick()
	{
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!cfg || !cfg.stalkWhispers || !GetGame().GetWorld().IsNight())
			return;
		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);


		array<PlayerBase> vampires = new array<PlayerBase>;
		foreach (Man candidate : men)
		{
			PlayerBase vampire = PlayerBase.Cast(candidate);
			if (vampire && vampire.m_AZVampire && vampire.IsAlive())
				vampires.Insert(vampire);
		}
		if (vampires.Count() == 0)
			return;
		int now = GetGame().GetTime();
		foreach (Man m : men)
		{
			PlayerBase h = PlayerBase.Cast(m);
			if (!h || !h.IsAlive() || h.m_AZVampire || !h.m_AZEventOn || !h.GetIdentity() || h.IsUnconscious())
				continue;
			string uid = h.GetIdentity().GetId();
			if (now < s_StalkAt.Get(uid))
				continue;
			if (AZ_VampRepel.LightReach(h, 12) > 0)
				continue;
			vector hp = h.GetPosition();
			vector face = h.GetDirection();
			PlayerBase near;
			int count;
			foreach (PlayerBase v : vampires)
			{
				vector vp = v.GetPosition();
				float d = vector.Distance(vp, hp);
				if (d < 6 || d > STALK_M)
					continue;
				vector to = vp - hp;
				to.Normalize();
				if (vector.Dot(face, to) > 0.5)
					continue;
				count++;
				near = v;
			}
			if (!near)
				continue;
			vector toward = near.GetPosition() - hp;
			toward[1] = 0;
			toward.Normalize();
			Circle(h, hp, toward, Math.RandomIntInclusive(2, 3));
			float gap = STALK_COOLDOWN_MS * Math.RandomFloat(0.85, 1.15);
			if (count > 1)
				gap = gap * 0.7;
			int next = now + gap;
			s_StalkAt.Set(uid, next);
			Print("[AZ_Vamp] stalk whisper " + uid + " vampires " + count + " nearest " + Math.Round(vector.Distance(near.GetPosition(), hp)) + " m");
		}
	}


	static ref array<string> s_ClientLines;
	static ref array<ref Param3<vector, vector, int>> s_ClientCircles;
	static vector s_ClientRitCenter;
	static int s_ClientRitStage;
	static int s_ClientRitKneel;
	static int s_ClientRitQuarter;
	static int s_ClientRitAt;
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (!GetGame().IsClient())
			return;
		if (rpc_type == AZ_VampWhisperLines.RPC_LINE)
		{
			Param1<string> l = new Param1<string>("");
			if (!ctx.Read(l))
				return;
			if (!AZ_VampWhisperLines.s_ClientLines)
				AZ_VampWhisperLines.s_ClientLines = new array<string>;
			AZ_VampWhisperLines.s_ClientLines.Insert(l.param1);
		}
		else if (rpc_type == AZ_VampWhisperLines.RPC_RITUAL)
		{
			Param4<vector, int, int, int> rs = new Param4<vector, int, int, int>(vector.Zero, 0, 0, 0);
			if (!ctx.Read(rs))
				return;
			AZ_VampWhisperLines.s_ClientRitCenter = rs.param1;
			AZ_VampWhisperLines.s_ClientRitStage = rs.param2;
			AZ_VampWhisperLines.s_ClientRitKneel = rs.param3;
			AZ_VampWhisperLines.s_ClientRitQuarter = rs.param4;
			AZ_VampWhisperLines.s_ClientRitAt = GetGame().GetTime();
		}
		else if (rpc_type == AZ_VampWhisperLines.RPC_CIRCLE)
		{
			Param3<vector, vector, int> c = new Param3<vector, vector, int>(vector.Zero, vector.Zero, 0);
			if (!ctx.Read(c))
				return;
			if (!AZ_VampWhisperLines.s_ClientCircles)
				AZ_VampWhisperLines.s_ClientCircles = new array<ref Param3<vector, vector, int>>;
			AZ_VampWhisperLines.s_ClientCircles.Insert(c);
		}
	}
}


modded class BloodBagFull
{
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		if (!GetGame().IsServer() || oldLoc.GetType() != InventoryLocationType.GROUND || newLoc.GetType() == InventoryLocationType.GROUND)
			return;
		PlayerBase h = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (h && AZ_VampCrypt.Inside(h.GetPosition()))
			AZ_VampWhisperLines.Trigger(h, "bag");
	}
}
