








class AZ_VampAltar
{
	static const float NEAR_M = 2.5;
	static const int MIN_HUMANS = 3;
	static const int ELDER_RETRY_MS = 120000;
	static const int ELDER_BITE_MS = 6000;
	static bool s_ElderPending;
	static bool s_ElderDue;
	static int s_ElderTryAt;
	static int s_Hour = -1;
	static bool s_NightArmed;
	static PlayerBase s_ElderVictim;
	static PlayerBase s_ElderActor;
	static int s_Attempt;
	static string s_LastReason;
	static int s_Starts;
	static int s_Interrupts;
	static int s_Infections;

	static int ActiveReservation()
	{
		if (s_ElderVictim)
			return 1;
		return 0;
	}

	static int SeedTarget()
	{
		return AZ_VampSeedPolicy.Target(AZ_VampState.Cfg().elderSeedTarget, AZ_VampState.CapNow());
	}

	static void BeginNight(string source)
	{
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		s_NightArmed = true;
		s_ElderDue = AZ_VampSeedPolicy.Due(GetGame().GetWorld().IsNight(), h);
		s_ElderPending = !s_ElderDue;
		s_ElderTryAt = 0;
		s_LastReason = "";
		AZ_VampAudit.Record("elder", "ARM", "source=" + source + " due=" + s_ElderDue + " configuredTarget=" + AZ_VampState.Cfg().elderSeedTarget + " retryMs=" + ELDER_RETRY_MS + " biteMs=" + ELDER_BITE_MS + " sicknessMin=" + AZ_VampState.Cfg().sicknessMin);
	}


	static int s_CheckAt;
	static vector s_CheckPos;
	static bool s_Near;
	static bool Near(vector pos)
	{
		int now = GetGame().GetTime();
		if (!GetGame().IsServer() && now < s_CheckAt && vector.Distance(pos, s_CheckPos) < 0.5)
			return s_Near;
		s_CheckAt = now + 1000;
		s_CheckPos = pos;
		s_Near = false;
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(pos, NEAR_M, objs, null);
		foreach (Object o : objs)
		{
			if (o && (o.IsKindOf("AZ_VampAltarCircle") || o.IsKindOf("AZ_VampAltar_Drum")))
			{
				s_Near = true;
				break;
			}
		}
		return s_Near;
	}


	static void EmptyDusk(array<PlayerBase> online)
	{
		if (!AZ_VampState.Cfg().ritualFallback)
			return;
		int humans = 0;
		foreach (PlayerBase p : online)
		{
			if (p.IsAlive() && !p.m_AZVampire)
				humans++;
		}
		if (humans < MIN_HUMANS)
			return;

		string bring = " Light 5 candles; one lies on the altar, the rest kneel.";
		if (AZ_VampState.Cfg().altarBloodBag)
			bring = " Bring a blade; light 5 candles. One lies on the altar, the rest kneel.";
		string where = "Ruined church east of Sebjan: the family vault in the churchyard. An altar waits below.";
		if (AZ_VampCryptA1.Ready())
			where = "Athena-1, beside the orb: the floor has cracked open. An altar waits below.";
		if (!AZ_VampState.Cfg().crypt)
			where = "The blood altars at Athena wait for an offering.";
		string warning = "";
		if (AZ_VampState.Cfg().elderFallback)
			warning = " Answer by midnight or an elder comes.";
		AZ_VampTurn.Announce("No vampire walks tonight. " + where + bring + warning);
		Print("[AZ_Vamp] empty dusk: altar hint, Elder fallback " + AZ_VampState.Cfg().elderFallback + " (" + humans + " humans)");
	}


	static void Watch()
	{
		bool night = GetGame().GetWorld().IsNight();
		bool enabled = AZ_VampEvent.Active() && AZ_VampState.Cfg().elderFallback;
		if (!night || !enabled)
		{
			s_NightArmed = false;
			s_ElderPending = false;
			s_ElderDue = false;
		}
		else if (!s_NightArmed)
			BeginNight("watch_recovery");
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		if (s_ElderPending && AZ_VampSeedPolicy.Due(night, h))
		{
			s_ElderPending = false;
			s_ElderDue = true;
			AZ_VampAudit.Record("elder", "MIDNIGHT_DUE");
		}
		s_Hour = h;
		array<PlayerBase> online;
		AZ_VampState.OnlinePlayers(online);
		int vampires = AZ_VampState.CountOnlineVampires();
		int turning = AZ_VampState.CountOnlineTurning();
		int humans = online.Count() - vampires - turning;
		int pending = s_Pending.Count();
		int reserved = AZ_VampSeedPolicy.Reserved(vampires, turning, pending, ActiveReservation());
		int cap = AZ_VampState.CapNow();
		int target = SeedTarget();
		int now = GetGame().GetTime();
		string reason = AZ_VampSeedPolicy.Reason(enabled, night, humans, ActiveReservation() > 0, AZ_VampRitual.s_State >= AZ_VampRitual.ST_LIGHTING, reserved, target, cap, s_ElderDue, now < s_ElderTryAt);
		if (AZ_VampSeedPolicy.Changed(s_LastReason, reason))
		{
			s_LastReason = reason;
			string details = "humans=" + humans + " vampires=" + vampires + " turning=" + turning;
			details += " pendingRitual=" + pending + " active=" + ActiveReservation() + " reserved=" + reserved;
			details += " target=" + target + " cap=" + cap + " retryAtMs=" + s_ElderTryAt;
			AZ_VampAudit.Record("elder", reason, details);
		}
		if (reason == "READY")
		{
			s_ElderTryAt = now + ELDER_RETRY_MS;
			Elder();
		}
	}

	static bool Prey(PlayerBase p)
	{
		return PreyReason(p) == "ELIGIBLE";
	}

	static string PreyReason(PlayerBase p)
	{
		if (!p || !p.IsAlive() || p.IsUnconscious() || p.m_AZVampire || p.m_AZVampTurning || p.m_AZVampHeld || p.IsInVehicle())
			return "STATE";
		if (AZ_VampState.IsImmune(p) || p.GetHealth("", "Blood") <= AZ_VampState.Cfg().bloodFloor + 50)
			return "PROTECTED";
		if (AZ_VampCrypt.Inside(p.GetPosition()))
			return "CRYPT";
		if (AZ_VampShadow.LightNear(p.GetPosition(), AZ_VampState.Cfg().strikeLightM))
			return "LIGHT";
		return "ELIGIBLE";
	}


	static bool Elder()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		array<PlayerBase> pool = new array<PlayerBase>;
		int stateReject, protectionReject, cryptReject, lightReject, geometryReject;
		foreach (PlayerBase p : list)
		{
			string no = PreyReason(p);
			if (no == "ELIGIBLE")
				pool.Insert(p);
			else if (no == "STATE") stateReject++;
			else if (no == "PROTECTED") protectionReject++;
			else if (no == "CRYPT") cryptReject++;
			else if (no == "LIGHT") lightReject++;
		}
		while (pool.Count() > 0)
		{
			int i = Math.RandomInt(0, pool.Count());
			PlayerBase victim = pool[i];
			pool.Remove(i);
			vector fwd = victim.GetDirection();
			fwd[1] = 0;
			fwd.Normalize();
			vector at = victim.GetPosition() - fwd * AZ_VampFeed.ROOT_GAP;
			at[1] = GetGame().SurfaceY(at[0], at[2]);
			if (AZ_VampShadow.Blocked(victim, at))
			{
				geometryReject++;
				continue;
			}
			PlayerBase elder = AZ_VampEvent.AIVampire(AZ_VampEvent.VampType(""), at, fwd);
			if (!elder)
			{
				AZ_VampAudit.Record("elder", "SPAWN_FAILED", "retryAtMs=" + s_ElderTryAt);
				return false;
			}
			s_ElderActor = elder;
			s_ElderVictim = victim;
			s_Attempt++;
			s_Starts++;
			victim.m_AZInfectionSource = "elder";
			GetGame().RPCSingleParam(elder, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(at), true);
			victim.m_AZVampRear = true;
			AZ_VampFeed.Hold(elder, victim);
			AZ_VampFeedAnim.DummyFeedStart(elder, true);
			AZ_VampRitual.Note(victim, "The altar", "Cold hands close on you from behind. Something very old is drinking.");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderEnd, ELDER_BITE_MS, false, elder, victim);
			AZ_VampAudit.Record("elder", "ATTEMPT_START", "attempt=" + s_Attempt + " sessionStarts=" + s_Starts + " retryAtMs=" + s_ElderTryAt);
			return true;
		}
		AZ_VampAudit.Record("elder", "NO_ELIGIBLE_TARGET", "state=" + stateReject + " protected=" + protectionReject + " crypt=" + cryptReject + " light=" + lightReject + " geometry=" + geometryReject + " retryAtMs=" + s_ElderTryAt);
		return false;
	}

	static void ElderEnd(PlayerBase elder, PlayerBase victim)
	{
		bool infected = victim && victim.m_AZVampTurning && victim.m_AZInfectionSource == "elder";
		bool completed = elder && elder.IsAlive() && !elder.IsUnconscious() && victim && victim.IsAlive() && victim.m_AZVampHeld && victim.m_AZVampFeeder == elder && AZ_VampEvent.Active() && GetGame().GetWorld().IsNight();
		if (completed)
		{
			float take = Math.Max(0, victim.GetHealth("", "Blood") - AZ_VampState.Cfg().bloodFloor);
			AZ_VampEvidence.Begin("elder_floor_drain", elder, victim, false);
			victim.AddHealth("", "Blood", -take);
			AZ_VampEvidence.End("elder_floor_drain", elder, victim);
			AZ_VampFeed.Collapse(elder, victim);
			infected = victim.m_AZVampTurning;
			AZ_VampTurn.Announce("Something older than the night fed at midnight. It is gone again.");
		}
		else if (victim)
			AZ_VampFeed.Release(elder, victim);
		if (infected)
			s_Infections++;
		else if (!completed)
			s_Interrupts++;
		AZ_VampAudit.Record("elder", AZ_VampSeedPolicy.Outcome(completed || infected, infected), "attempt=" + s_Attempt + " sessionInfections=" + s_Infections + " sessionInterrupts=" + s_Interrupts + " retryAtMs=" + s_ElderTryAt);
		if (victim && !victim.m_AZVampTurning)
			victim.m_AZInfectionSource = "";
		s_ElderVictim = null;
		s_ElderActor = null;
		if (elder)
		{
			AZ_VampFeedAnim.DummyFeedEnd(elder);
			GetGame().RPCSingleParam(elder, AZ_VampShadow.RPC_SHADOW, new Param1<vector>(elder.GetPosition()), true);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 1500, false, elder);
		}
	}



	static ref map<string, bool> s_Pending = new map<string, bool>;



	static void Offer(PlayerBase p)
	{
		if (!p || p.m_AZVampire || p.m_AZVampTurning || !AZ_VampEvent.Active() || !GetGame().GetWorld().IsNight())
			return;
		AZ_VampRitual.Pour(p);
	}


	static bool IsFullBag(EntityAI e)
	{
		return e && (e.IsKindOf("BloodBagFull") || e.IsKindOf("BloodBagIV")) && !e.IsRuined();
	}


	static bool IsBlade(EntityAI e)
	{
		if (!e || e.IsRuined())
			return false;
		array<string> blades = {"KitchenKnife", "SteakKnife", "HuntingKnife", "CombatKnife", "StoneKnife", "BoneKnife", "KukriKnife", "FangeKnife", "Machete", "CrudeMachete", "OrientalMachete", "Cleaver", "Sickle", "Sword", "Hatchet", "Akinaka", "DonerKnife"};
		foreach (string b : blades)
		{
			if (e.IsKindOf(b))
				return true;
		}
		return false;
	}


	static bool CapWithPending()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		int cap = list.Count() * AZ_VampState.Cfg().capPercent / 100;
		if (cap < AZ_VampState.Cfg().capMin)
			cap = AZ_VampState.Cfg().capMin;
		return AZ_VampState.CountOnlineVampires() + AZ_VampState.CountOnlineTurning() + s_Pending.Count() + ActiveReservation() < cap;
	}






	static const float WAKE_FEED_M = 30;
	static const int WAKE_CHOICE_AT_MS = 0;
	static const int WAKE_CHOICE_LEN_MS = 6520;
	static const int WAKE_DRAIN_GAP_MS = 700;

	static void WakeFeed(PlayerBase p)
	{
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(p.GetPosition(), WAKE_FEED_M, near, null);
		int humans = 0;
		foreach (Object o : near)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (h && h != p && h.IsAlive() && !h.m_AZVampire)
				humans++;
		}
		if (humans == 0)
			return;
		ScriptCallQueue q = GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.CallLater(WakeChoice, WAKE_CHOICE_AT_MS, false, p);
		q.CallLater(WakeDrain, WAKE_CHOICE_AT_MS + WAKE_CHOICE_LEN_MS + WAKE_DRAIN_GAP_MS, false, p);
		Print("[AZ_Vamp] ritual wake: " + humans + " humans within " + WAKE_FEED_M + " m, the choice");
	}




	static const int HOWTO_MS = 12000;

	static void HowTo(PlayerBase p)
	{
		if (!p || !p.IsAlive() || !p.m_AZVampire || !p.GetIdentity())
			return;
		AZ_VampRecord r = AZ_VampState.Find(AZ_VampState.Uid(p));
		if (!r || r.howToShown)
			return;
		r.howToShown = true;
		r.shadowHintShown = true;
		AZ_VampState.Save();
		NotificationSystem.SendNotificationToPlayerIdentityExtended(p.GetIdentity(), 14, "You are a vampire", "The Fangs key ({AZ_FANGS}) bares your fangs. Empty hands, hold interact on a human to feed.");
		Print("[AZ_Vamp] how-to shown to " + AZ_VampState.Uid(p));
	}


	static void ShadowHint(PlayerBase p)
	{
		if (!p || !p.IsAlive() || !p.m_AZVampire || !p.GetIdentity())
			return;
		AZ_VampRecord r = AZ_VampState.Find(AZ_VampState.Uid(p));
		if (!r || r.shadowHintShown)
			return;
		r.shadowHintShown = true;
		AZ_VampState.Save();
		NotificationSystem.SendNotificationToPlayerIdentityExtended(p.GetIdentity(), 12, "Blood moon", "Fangs key ({AZ_FANGS}) at a human ahead: the shadow step. You are on them in a blink; press again to chain.");
		Print("[AZ_Vamp] shadow-step hint shown to " + AZ_VampState.Uid(p));
	}


	static void VoiceNear(PlayerBase p, string soundSet, string who, string text, vector at)
	{
		AZ_VampCryptLurkers.Voice(p, soundSet, who, text, at);
		array<Object> nearAll = new array<Object>;
		GetGame().GetObjectsAtPosition3D(p.GetPosition(), WAKE_FEED_M, nearAll, null);
		foreach (Object no : nearAll)
		{
			PlayerBase np = PlayerBase.Cast(no);
			if (np && np != p && np.IsAlive() && np.GetIdentity())
				AZ_VampCryptLurkers.Voice(np, soundSet, who, text, at);
		}
	}

	static void WakeChoice(PlayerBase p)
	{
		if (p && p.IsAlive())
			VoiceNear(p, "AZ_VampLine_wake_SoundSet", "The Countess", "Feed on them... and they will rise, as you did. Or let them stay warm.", p.GetPosition() + Vector(0, 1.6, 0));
	}

	static void WakeDrain(PlayerBase p)
	{
		if (!p || !p.IsAlive())
			return;
		PlayerBase e = AZ_VampCryptLurkers.Speaker(p, "m");
		vector at = p.GetPosition() + Vector(0, 1.6, 0);
		string from = "the ear";
		if (e && vector.Distance(e.GetPosition(), p.GetPosition()) < 15)
		{
			at = e.GetPosition() + Vector(0, 1.6, 0);
			from = "a male elder";
		}
		VoiceNear(p, "AZ_VampLine_drinkdry_SoundSet", "An elder", "Or drink them dry.", at);
		Print("[AZ_Vamp] ritual wake: the elder's answer from " + from);
	}

	static void Wake(PlayerBase p, string uid, string line = "You wake cold. The altar answered. Feed after dark.")
	{
		s_Pending.Remove(uid);
		if (!p || !p.IsAlive() || p.m_AZVampire)
		{
			Print("[AZ_Vamp] ritual wake " + uid + " skipped: gone, dead or already turned");
			return;
		}
		if (!AZ_VampState.CapAllowsTurn())
		{
			Print("[AZ_Vamp] ritual wake " + uid + " refused: cap");
			AZ_VampRitual.Note(p, "The Countess", "You wake weak and still warm. The altar took your blood and gave nothing back.");
			return;
		}
		AZ_VampEvent.MakeVampire(p, "ritual");
		WakeFeed(p);
		AZ_VampTurn.Teach(p);
		AZ_VampTurn.Announce("Someone on this server answered the blood altar. Prepare for a vampire.");
		Print("[AZ_Vamp] ritual wake " + uid + " turned");
	}
}

class ActionAZOfferBloodCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(10);
	}
}

class ActionAZOfferBlood : ActionContinuousBase
{
	void ActionAZOfferBlood()
	{
		m_CallbackClass = ActionAZOfferBloodCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BANDAGE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Cut your palm over the altar";
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || player.m_AZVampire || player.m_AZVampTurning || !GetGame().GetWorld().IsNight())
			return false;
		if (GetGame().IsServer())
			return AZ_VampEvent.Active();
		return HasOffering(player.GetItemInHands()) && AZ_VampRitual.ClientStep(player.GetPosition()) == AZ_VampRitual.STEP_AWAKE;
	}

	bool HasOffering(EntityAI e)
	{
		return false;
	}

	override bool IsLockTargetOnUse()
	{
		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampAltar.Offer(action_data.m_Player);
	}
}


class ActionAZOfferBag : ActionAZOfferBlood
{
	void ActionAZOfferBag()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_Text = "Pour the blood bag over the altar";
	}

	override bool HasOffering(EntityAI e)
	{
		return AZ_VampAltar.IsFullBag(e);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZOfferBlood, InputActionMap);
		AddAction(ActionAZOfferBag, InputActionMap);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZOfferBlood);
		actions.Insert(ActionAZOfferBag);
	}
}
