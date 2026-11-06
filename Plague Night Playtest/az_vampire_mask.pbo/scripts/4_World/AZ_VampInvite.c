





class AZ_VampInvite
{
	static const int RPC_INVITE = 4702260;
	static const int INVITE_MS = 30000;
	static const int ELDER_GONE_MS = 8000;
	static const float ELDER_AHEAD_M = 2.5;
	static const int ELDER_SEED = 2;

	static ref map<string, int> s_Until = new map<string, int>;
	static ref set<string> s_Seen = new set<string>;
	static ref set<string> s_Pulled = new set<string>;
	static int s_For;
	static int s_ClientUntil;


	static void Scan()
	{
		if (AZ_VampRitual.s_State != AZ_VampRitual.ST_LYING || !AZ_VampRitual.s_Site)
			return;
		bool first = s_For != AZ_VampRitual.s_LieAt;
		if (first)
		{
			s_For = AZ_VampRitual.s_LieAt;
			s_Until.Clear();
			s_Seen.Clear();
		}
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		foreach (PlayerBase h : online)
		{
			if (!h || !h.GetIdentity() || !h.IsAlive() || h.IsUnconscious() || h.m_AZVampire || h == AZ_VampRitual.s_Lying)
				continue;
			string uid = AZ_VampState.Uid(h);
			if (s_Seen.Find(uid) >= 0 || !AZ_VampCrypt.Inside(h.GetPosition()))
				continue;
			s_Seen.Insert(uid);
			if (first || AZ_VampRitual.s_Kneelers.Find(h) >= 0)
				continue;
			Invite(h);
		}
	}

	static PlayerBase Invite(PlayerBase h)
	{
		vector dir = h.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector at = AZ_VampCryptLurkers.SafeFloor(h.GetPosition() + dir * ELDER_AHEAD_M, h.GetPosition()[1]);
		vector toHim = h.GetPosition() - at;
		float yaw = toHim.VectorToAngles()[0];
		PlayerBase e = AZ_VampCryptLurkers.Dress(at, yaw, ELDER_SEED);
		if (e)
		{
			GetGame().RPCSingleParam(e, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(at), true);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampRitual.ProtectGone, ELDER_GONE_MS, false, e);
		}
		string uid = AZ_VampState.Uid(h);
		s_Until.Set(uid, GetGame().GetTime() + INVITE_MS);
		GetGame().RPCSingleParam(h, RPC_INVITE, new Param2<vector, int>(at + Vector(0, 1.6, 0), INVITE_MS), true, h.GetIdentity());
		s_Pulled.Insert(uid);
		AZ_VampRitual.Note(h, "The altar", "The dead make room for you. Kneel, or watch.");
		Print("[AZ_Vamp] ritual invite " + uid + ": an elder at hall " + AZ_VampCrypt.HallLocal(at) + " (" + (e != null) + "), " + AZ_VampRitual.s_Kneelers.Count() + " kneeling");
		return e;
	}


	static void EndAll()
	{
		if (s_Pulled.Count() == 0)
			return;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		foreach (PlayerBase h : online)
		{
			if (h && h.GetIdentity() && s_Pulled.Find(AZ_VampState.Uid(h)) >= 0)
				GetGame().RPCSingleParam(h, RPC_INVITE, new Param2<vector, int>(vector.Zero, 0), true, h.GetIdentity());
		}
		Print("[AZ_Vamp] ritual invite: the waiting room lifts for " + s_Pulled.Count());
		s_Pulled.Clear();
	}

	static bool Invited(PlayerBase p)
	{
		if (!p || s_For != AZ_VampRitual.s_LieAt)
			return false;
		string uid = AZ_VampState.Uid(p);
		return s_Until.Contains(uid) && GetGame().GetTime() < s_Until.Get(uid);
	}

	static void Used(PlayerBase p)
	{
		if (p)
			s_Until.Remove(AZ_VampState.Uid(p));
	}


	static void Mark(PlayerBase p)
	{
		s_For = AZ_VampRitual.s_LieAt;
		s_Until.Set(AZ_VampState.Uid(p), GetGame().GetTime() + INVITE_MS);
	}

	static bool ClientInvited()
	{
		return s_ClientUntil > 0 && GetGame().GetTime() < s_ClientUntil;
	}


	static const string BED_SET = "AZ_VampInvite_Pull_SoundSet";
	static const string VOICE_SET = "AZ_VampLine_invite_SoundSet";
	static const string BEAT_SET = "AZ_VampStareBeat_SoundSet";
	static const float DUCK_S = 1.5;
	static const float UNDUCK_S = 2.0;
	static const float DUCK_TO = 0.0;
	static const string CALL_SET = "AZ_CryptCall_SoundSet";
	static const int HOLD_MAX_MS = 150000;



	static const int DUCK_AT_MS = 1000;
	static const int BEAT_AT_MS = 1500;
	static const int VOICE_MS = 3200;
	static const int PULL_MS = 11000;
	static const float BEAT_GAP0 = 850;
	static const float BEAT_GAP1 = 1450;
	static const float BEAT_GAP_GROW = 1.08;
	static const float BEAT_VOL0 = 0.6;
	static const float BEAT_VOL_GROW = 1.05;
	static int s_Gen;

	static void Pull(PlayerBase p, vector voiceAt, int ms)
	{
		if (ms <= 0)
		{
			Lift(s_Gen);
			return;
		}
		s_ClientUntil = GetGame().GetTime() + ms;
		s_Gen++;
		bool bed = false;
		if (GetGame().ConfigIsExisting("CfgSoundSets " + BED_SET))
		{
			EffectSound d = SEffectManager.PlaySound(BED_SET, p.GetPosition());
			if (d)
			{
				d.SetSoundAutodestroy(true);
				bed = true;
			}
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DuckIn, DUCK_AT_MS, false, s_Gen);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Lift, HOLD_MAX_MS, false, s_Gen);
		s_Who = p;
		s_Beat++;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Beat, BEAT_AT_MS, false, s_Beat, p, BEAT_GAP0, BEAT_VOL0);
		s_VoiceAt = voiceAt;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Voice, VOICE_MS, false, s_Gen);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Release, PULL_MS, false, s_Gen);
		Print("[AZ_Vamp] invite client: the pull, drone " + bed + ", kneel offered for " + ms + " ms");
	}

	static void DuckIn(int gen)
	{
		if (gen == s_Gen)
			AZ_VampWhisperLines.Duck(DUCK_TO, 1.0);
	}

	static void Beat(int gen, PlayerBase p, float gap, float vol)
	{
		if (gen != s_Beat || !p || !GetGame().ConfigIsExisting("CfgSoundSets " + BEAT_SET))
			return;
		EffectSound b = SEffectManager.PlaySound(BEAT_SET, p.GetPosition());
		if (b)
		{
			b.SetSoundVolume(vol);
			b.SetSoundAutodestroy(true);
		}
		if (vol <= BEAT_VOL0)
			Print("[AZ_Vamp] invite client: the slow heart starts");
		int wait = gap;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Beat, wait, false, gen, p, Math.Min(BEAT_GAP1, gap * BEAT_GAP_GROW), Math.Min(1.0, vol * BEAT_VOL_GROW));
	}

	static vector s_VoiceAt;

	static void Voice(int gen)
	{
		vector at = s_VoiceAt;
		if (gen != s_Gen || !GetGame().ConfigIsExisting("CfgSoundSets " + VOICE_SET))
			return;
		EffectSound v = SEffectManager.PlaySound(VOICE_SET, at);
		if (v)
			v.SetSoundAutodestroy(true);
		Print("[AZ_Vamp] invite client: the household speaks " + (v != null));
	}

	static void Release(int gen)
	{
		if (gen != s_Gen)
			return;
		s_Beat++;
		if (s_Who && GetGame().ConfigIsExisting("CfgSoundSets " + CALL_SET))
		{
			EffectSound c = SEffectManager.PlaySound(CALL_SET, s_Who.GetPosition() + s_Who.GetDirection() * 4.0 + Vector(0, 1.5, 0));
			if (c)
				c.SetSoundAutodestroy(true);
		}
		Print("[AZ_Vamp] invite client: the pull ends, the call, the silence holds");
	}

	static PlayerBase s_Who;
	static int s_Beat;

	static void Lift(int gen)
	{
		if (gen != s_Gen)
			return;
		s_Gen++;
		s_ClientUntil = 0;
		AZ_VampWhisperLines.Duck(1.0, UNDUCK_S);
		Print("[AZ_Vamp] invite client: the waiting room lifts, the world comes back");
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampInvite.RPC_INVITE || !GetGame().IsClient() || !IsControlledPlayer())
			return;
		Param2<vector, int> ip = new Param2<vector, int>(vector.Zero, 0);
		if (ctx.Read(ip))
			AZ_VampInvite.Pull(this, ip.param1, ip.param2);
	}
}
