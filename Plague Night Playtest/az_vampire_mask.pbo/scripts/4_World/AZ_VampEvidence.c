// AZ-2489: server-only observed values; never read or emit network addresses.
// Existing gameplay files only bracket mutations with calls into this helper.
class AZ_VampEvidence
{
	static ref array<ref AZ_VampEvidence> s_Open = new array<ref AZ_VampEvidence>;
	static bool s_Sweeping;
	static int s_Next;
	string eventKind;
	Object source;
	PlayerBase victim;
	string attackerLabel;
	string victimLabel;
	string moonBefore;
	float hp;
	float blood;
	float shock;
	float lastHp;
	float lastBlood;
	float lastShock;
	vector lastPos;
	int at;
	int lastAt;
	int serial;
	bool continuous;
	bool hold;
	bool identified;
	bool lastAlive;
	bool lastUnconscious;

	static string Clean(string value)
	{
		value.Replace("\n", " ");
		value.Replace("\r", " ");
		value.Replace("\t", " ");
		value.Replace("\"", "'");
		if (value.Length() > 64)
			value = value.Substring(0, 64);
		return value;
	}

	static string SteamHash(string steam)
	{
		int hash = steam.Hash();
		return hash.ToString();
	}

	static string Subject(Object obj)
	{
		PlayerBase p = PlayerBase.Cast(obj);
		EntityAI entity = EntityAI.Cast(obj);
		if (!p && entity)
			p = PlayerBase.Cast(entity.GetHierarchyRootPlayer());
		if (p)
		{
			PlayerIdentity identity = p.GetIdentity();
			if (identity)
			{
				string steam = identity.GetPlainId();
				string shortHash = "unknown";
				if (steam != "")
					shortHash = SteamHash(steam);
				return "\"" + Clean(identity.GetName()) + "\"#" + shortHash;
			}
			return "npc_or_disconnected#unknown";
		}
		if (obj)
			return Clean(obj.GetType()) + "#unknown";
		return "environment_or_unknown#unknown";
	}

	static string Moon()
	{
		return "night=" + GetGame().GetWorld().IsNight() + ",bloodMoon=" + AZ_VampProgress.s_BloodMoon;
	}

	static string Position(vector pos)
	{
		return "<" + Math.Round(pos[0]) + "," + Math.Round(pos[1]) + "," + Math.Round(pos[2]) + ">";
	}

	static int Find(string kind, Object actor, PlayerBase target)
	{
		for (int i = s_Open.Count() - 1; i >= 0; i--)
		{
			AZ_VampEvidence e = s_Open[i];
			if (e.eventKind == kind && e.source == actor && e.victim == target)
				return i;
		}
		return -1;
	}

	static void Begin(string kind, Object actor, PlayerBase target, bool ticks = false, bool held = false)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (!GetGame() || !GetGame().IsServer() || !target)
			return;
		if (ticks && !target.IsAlive())
			return;
		int found = Find(kind, actor, target);
		if (found >= 0)
		{
			s_Open[found].lastAt = GetGame().GetTime();
			return;
		}
		AZ_VampEvidence e = new AZ_VampEvidence;
		e.eventKind = kind;
		e.source = actor;
		e.victim = target;
		e.attackerLabel = Subject(actor);
		e.victimLabel = Subject(target);
		e.identified = target.GetIdentity() != null;
		e.moonBefore = Moon();
		e.hp = target.GetHealth("", "Health");
		e.blood = target.GetHealth("", "Blood");
		e.shock = target.GetHealth("", "Shock");
		e.lastHp = e.hp;
		e.lastBlood = e.blood;
		e.lastShock = e.shock;
		e.lastPos = target.GetPosition();
		e.lastAlive = target.IsAlive();
		e.lastUnconscious = target.IsUnconscious();
		e.at = GetGame().GetTime();
		e.lastAt = e.at;
		e.continuous = ticks;
		e.hold = held;
		s_Next++;
		e.serial = s_Next;
		s_Open.Insert(e);
		if (ticks || held)
			Write(e, "start");
		if (!s_Sweeping)
		{
			s_Sweeping = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Sweep, 5000, false);
		}
	}

	static void Sample(AZ_VampEvidence e)
	{
		if (!e.victim)
			return;
		e.lastHp = e.victim.GetHealth("", "Health");
		e.lastBlood = e.victim.GetHealth("", "Blood");
		e.lastShock = e.victim.GetHealth("", "Shock");
		e.lastPos = e.victim.GetPosition();
		e.lastAlive = e.victim.IsAlive();
		e.lastUnconscious = e.victim.IsUnconscious();
	}

	static void Write(AZ_VampEvidence e, string phase)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		string line = "[AZ_Vamp] evidence id=" + e.serial + " event=" + e.eventKind + "/" + phase;
		line += " attacker=" + e.attackerLabel + " victim=" + e.victimLabel + " pos=" + Position(e.lastPos);
		line += " hp=" + e.hp + "->" + e.lastHp + " blood=" + e.blood + "->" + e.lastBlood + " shock=" + e.shock + "->" + e.lastShock;
		line += " alive=" + e.lastAlive + " unconscious=" + e.lastUnconscious;
		line += " moon=" + e.moonBefore + "->" + Moon() + " ms=" + (e.lastAt - e.at);
		Print(line);
		GetGame().AdminLog(line);
	}

	static void End(string kind, Object actor, PlayerBase target, string phase = "end")
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (!GetGame() || !GetGame().IsServer())
			return;
		int i = Find(kind, actor, target);
		if (i < 0)
			return;
		AZ_VampEvidence e = s_Open[i];
		Sample(e);
		e.lastAt = GetGame().GetTime();
		if (e.continuous && target && target.IsAlive())
			return;
		if (kind != "stake_finisher" || e.hp != e.lastHp)
			Write(e, phase);
		s_Open.Remove(i);
	}

	static void Discard(string kind, Object actor, PlayerBase target)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (!GetGame() || !GetGame().IsServer())
			return;
		int i = Find(kind, actor, target);
		if (i >= 0)
			s_Open.Remove(i);
	}

	static string BurnKind(bool exposed, bool holy, bool fire)
	{
		if (exposed)
			return "sun_burn";
		if (fire)
			return "sanctuary_fire";
		return "holy_ground";
	}

	static void BeginBurn(PlayerBase target, bool exposed, bool holy, bool fire)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		Begin(BurnKind(exposed, holy, fire), null, target, true);
	}

	static void EndBurn(PlayerBase target, bool exposed, bool holy, bool fire)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		End(BurnKind(exposed, holy, fire), null, target);
	}

	static void InterceptEnd(string kind, Object actor, PlayerBase target, bool alreadyDown)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (alreadyDown)
			End(kind, actor, target, "held_down");
		else
			End(kind, actor, target, "knockout");
	}

	static void EndDrain(PlayerBase target, bool draining)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (draining)
			End("passive_blood_drain", null, target);
	}

	static void BeginDrain(PlayerBase target, bool draining)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (draining)
			Begin("passive_blood_drain", null, target, true);
	}

	static void EndHolds(PlayerBase target)
	{
		if (!AZ_VampState.Cfg().combatEvidence)
			return;
		if (!GetGame() || !GetGame().IsServer())
			return;
		for (int i = s_Open.Count() - 1; i >= 0; i--)
		{
			AZ_VampEvidence e = s_Open[i];
			if (e.hold && e.victim == target)
				End(e.eventKind, e.source, target);
		}
	}

	static void Sweep()
	{
		if (!AZ_VampState.Cfg().combatEvidence)
		{
			s_Open.Clear();
			s_Sweeping = false;
			return;
		}
		int now = GetGame().GetTime();
		for (int i = s_Open.Count() - 1; i >= 0; i--)
		{
			AZ_VampEvidence e = s_Open[i];
			bool gone = !e.victim;
			if (e.victim)
				gone = !e.victim.IsAlive() || (e.identified && !e.victim.GetIdentity());
			if (gone || (!e.hold && now - e.lastAt >= 5000))
			{
				// Tick summaries use the last observed post-mutation values, not later healing.
				if (e.hold)
					Sample(e);
				Write(e, "end");
				s_Open.Remove(i);
			}
		}
		s_Sweeping = s_Open.Count() > 0;
		if (s_Sweeping)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Sweep, 5000, false);
	}
}
