






class AZ_VampFeed
{
	static const int RPC_FEED = 4702129;
	static const int SAFETY_MS = 3000;


	static const float ROOT_GAP = 0.62;

	// Server authority: independent of stamina, action/animation phase and victim pose.
	static bool Cancel(PlayerBase vamp)
	{
		if (!vamp || !GetGame().IsServer())
			return false;
		PlayerBase victim = vamp.m_AZVampFeedingVictim;
		if (!victim)
			victim = vamp.m_AZVampTimedVictim;
		ActionManagerBase am = vamp.GetActionManager();
		ActionBase action;
		if (am)
			action = am.GetRunningAction();
		bool biteAction = action && (action.IsInherited(ActionAZVampBite) || action.IsInherited(ActionAZVampDrain));
		AZ_VampShadowState st = AZ_VampShadow.s_State.Get(vamp);
		int now = GetGame().GetTime();
		bool shadowBusy = st && (st.victim || st.compelling || now < st.busyUntil);
		if (!victim && !biteAction && !shadowBusy)
			return false;
		// AZ-2621: under the blood moon the frenzy holds the vampire too: no letting go while the victim is still awake.
		// A hit, a knock-out or death still breaks the feed (the hold's own checks), which is the humans' counterplay
		if (vamp.m_AZBloodMoon && AZ_VampState.Cfg().moonNoLetGo && victim && victim.IsAlive() && !victim.IsUnconscious() && victim.m_AZVampHeld && (victim.m_AZVampFeeder == vamp || victim.m_AZVampFeeder2 == vamp))
		{
			if (now >= vamp.m_AZShadowWhyAt)
			{
				vamp.m_AZShadowWhyAt = now + 3000;
				vamp.Message("The blood moon will not let you stop.", "colorAction");
			}
			return true;
		}
		// Keep normal credit/victim cooldowns, and forbid cancelling into an instant re-bite.
		vamp.m_AZFeedCancelUntil = now + Math.Max(1, AZ_VampState.Cfg().shadowCooldownSec) * 1000;
		AZ_VampShadow.Abort(vamp);
		if (victim && victim.m_AZVampHeld && (victim.m_AZVampFeeder == vamp || victim.m_AZVampFeeder2 == vamp))
			Release(vamp, victim);
		if (biteAction)
		{
			am.EndOrInterruptCurrentAction();
			am.Interrupt();
		}
		AZ_VampShadow.BiteAnim(vamp, 2, 0, 0);
		vamp.Message("You let go.", "colorAction");
		Print("[AZ_Vamp] feed cancelled by vampire " + AZ_VampState.Uid(vamp));
		return true;
	}

	static void ReleaseHint(PlayerBase vamp)
	{
		if (vamp && vamp.GetIdentity())
			vamp.Message("Press your Fangs key ({AZ_FANGS}) to let go.", "colorAction");
	}




	static bool CanFeedHere(PlayerBase vamp)
	{
		if (!vamp || AZ_VampDown.Weak(vamp))
			return false;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!cfg)
			return false;
		return !cfg.nightOnly || !AZ_VampSun.Exposed(vamp);
	}




	static bool KeepSheltered(PlayerBase victim)
	{
		if (!victim || !victim.m_AZVampHeld)
			return false;
		if (victim.m_AZVampFeeder2 && !CanFeedHere(victim.m_AZVampFeeder2))
			Release(victim.m_AZVampFeeder2, victim);
		if (victim.m_AZVampHeld && victim.m_AZVampFeeder && !CanFeedHere(victim.m_AZVampFeeder))
			Release(victim.m_AZVampFeeder, victim);
		return victim.m_AZVampHeld;
	}


	static void Hold(PlayerBase vamp, PlayerBase victim, bool hiss = true)
	{
		if (!vamp || !victim || AZ_VampDown.RefuseBite(vamp))
			return;
		AZ_VampEvidence.Begin("feed_hold", vamp, victim, false, true);
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		int now = GetGame().GetTime();
		bool down = victim.IsUnconscious();
		victim.m_AZVampHeld = true;
		victim.AZ_VampMarkBitten();
		victim.m_AZVampFeeder = vamp;
		vamp.m_AZVampFeedingVictim = victim;
		ReleaseHint(vamp);
		victim.m_AZVampFeedTotal = 0;
		victim.m_AZVampTimedFeed = false;
		victim.m_AZVampCollapsed = down;
		victim.m_AZVampFeedPerSec = cfg.drinkPerSec;
		if (down)
			victim.m_AZVampFeedPerSec = cfg.finishPerSec;
		victim.m_AZVampHeldUntil = now + cfg.drinkMaxSec * 1000 + SAFETY_MS;

		victim.m_AZVampFloorAt = now + 1000 * Math.Max(0, victim.GetHealth("", "Blood") - cfg.bloodFloor) / Math.Max(1, cfg.drinkPerSec);
		victim.SetSynchDirty();
		if (!down && hiss)
			GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
		GetGame().RPCSingleParam(vamp, RPC_FEED, new Param1<bool>(true), true);
		Print("[AZ_Vamp] feed hold " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " blood " + victim.GetHealth("", "Blood") + " down " + down);
	}


	static const float MIN_J_FEED_SEC = 6;

	// minSec: the floor of the hold; the blood-moon chain passes its own (AZ-2627), the strike keeps MIN_J_FEED_SEC
	static void HoldTimed(PlayerBase vamp, PlayerBase victim, float seconds, float minSec = MIN_J_FEED_SEC)
	{
		if (!vamp || !victim || AZ_VampDown.RefuseBite(vamp))
			return;
		Hold(vamp, victim, false);
		seconds = Math.Max(minSec, seconds);
		victim.m_AZVampTimedFeed = true;
		vamp.m_AZVampTimedVictim = victim;
		victim.m_AZVampFeedPerSec = Math.Max(0, victim.GetHealth("", "Blood") - AZ_VampState.Cfg().bloodFloor) / seconds;
		victim.m_AZVampFloorAt = GetGame().GetTime() + seconds * 1000;
		victim.m_AZVampHeldUntil = victim.m_AZVampFloorAt + SAFETY_MS;
	}




	static bool s_SharedTest;

	static bool Shared()
	{
		return s_SharedTest || AZ_VampState.Cfg().sharedFeed;
	}

	static void Join(PlayerBase vamp, PlayerBase victim)
	{
		if (!vamp || !victim || AZ_VampDown.RefuseBite(vamp) || !victim.m_AZVampHeld || victim.m_AZVampFeeder2)
			return;
		AZ_VampEvidence.Begin("feed_hold", vamp, victim, false, true);
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		int now = GetGame().GetTime();
		victim.m_AZVampFeeder2 = vamp;
		vamp.m_AZVampFeedingVictim = victim;
		ReleaseHint(vamp);
		victim.m_AZVampFeedTotal2 = 0;
		victim.m_AZVampHeldUntil = Math.Max(victim.m_AZVampHeldUntil, now + cfg.drinkMaxSec * 1000 + SAFETY_MS);
		if (!victim.m_AZVampCollapsed)
			victim.m_AZVampFloorAt = now + 1000 * Math.Max(0, victim.GetHealth("", "Blood") - cfg.bloodFloor) / Math.Max(1, 2 * victim.m_AZVampFeedPerSec);
		GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
		GetGame().RPCSingleParam(vamp, RPC_FEED, new Param1<bool>(true), true);
		Print("[AZ_Vamp] feed join " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " rear " + !victim.m_AZVampRear);
	}


	static void Credit(PlayerBase vamp, PlayerBase victim, float total)
	{
		if (!vamp || total <= 0)
			return;
		if (total >= AZ_VampState.Cfg().victimCooldownMl)
			AZ_VampState.SetBiteCooldown(victim);
		AZ_VampReveal.Fed(vamp);
		AZ_VampProgress.OnFeed(vamp, total);
		if (!AZ_VampShadow.s_InChain)
			GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_SIGH, new Param1<bool>(true), true);
		Print("[AZ_Vamp] feed end " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " drank " + total + " ml, victim blood " + victim.GetHealth("", "Blood") + " alive " + victim.IsAlive() + " vampire blood " + vamp.GetHealth("", "Blood"));
	}


	static void LetGoOne(PlayerBase vamp, PlayerBase victim)
	{
		AZ_VampEvidence.End("feed_hold", vamp, victim);
		if (vamp && vamp.m_AZVampFeedingVictim == victim)
			vamp.m_AZVampFeedingVictim = null;
		if (vamp && vamp.m_AZVampTimedVictim == victim)
		{
			vamp.m_AZVampTimedVictim = null;
			if (!AZ_VampShadow.s_InChain)
				AZ_VampShadow.BiteAnim(vamp, 2, 0, 0);
			victim.m_AZVampTimedFeed = false;
		}
		if (vamp == victim.m_AZVampFeeder2)
		{
			Credit(vamp, victim, victim.m_AZVampFeedTotal2);
		}
		else
		{
			Credit(vamp, victim, victim.m_AZVampFeedTotal);
			victim.m_AZVampFeeder = victim.m_AZVampFeeder2;
			victim.m_AZVampFeedTotal = victim.m_AZVampFeedTotal2;
			victim.m_AZVampRear = !victim.m_AZVampRear;
		}
		victim.m_AZVampFeeder2 = null;
		victim.m_AZVampFeedTotal2 = 0;
		if (!victim.m_AZVampCollapsed)
			victim.m_AZVampFloorAt = GetGame().GetTime() + 1000 * Math.Max(0, victim.GetHealth("", "Blood") - AZ_VampState.Cfg().bloodFloor) / Math.Max(1, victim.m_AZVampFeedPerSec);
		if (vamp)
			GetGame().RPCSingleParam(vamp, RPC_FEED, new Param1<bool>(false), true);
	}




	static void Release(PlayerBase vamp, PlayerBase victim)
	{
		if (vamp && vamp.m_AZVampFeedingVictim == victim)
			vamp.m_AZVampFeedingVictim = null;

		if (vamp && victim && victim.m_AZVampHeld && vamp != victim.m_AZVampFeeder && vamp != victim.m_AZVampFeeder2)
		{
			GetGame().RPCSingleParam(vamp, RPC_FEED, new Param1<bool>(false), true);
			return;
		}
		if (victim && victim.m_AZVampHeld && victim.m_AZVampFeeder2 && vamp && (vamp == victim.m_AZVampFeeder || vamp == victim.m_AZVampFeeder2) && victim.IsAlive() && !victim.m_AZVampCollapsed)
		{
			LetGoOne(vamp, victim);
			return;
		}
		PlayerBase second;
		float total2;
		if (victim && victim.m_AZVampHeld)
		{
			if (!vamp || vamp == victim.m_AZVampFeeder2)
				vamp = victim.m_AZVampFeeder;
			second = victim.m_AZVampFeeder2;
			total2 = victim.m_AZVampFeedTotal2;
			AZ_VampEvidence.EndHolds(victim);
			victim.m_AZVampHeld = false;
			victim.m_AZVampRear = false;
			victim.m_AZVampFeedPerSec = 0;
			victim.m_AZVampTimedFeed = false;
			victim.m_AZVampFeeder = null;
			victim.m_AZVampFeeder2 = null;
			victim.m_AZVampFeedTotal2 = 0;
			victim.SetSynchDirty();
		}
		if (vamp && victim && victim.m_AZVampFeedTotal > 0)
			Credit(vamp, victim, victim.m_AZVampFeedTotal);
		if (victim)
			victim.m_AZVampFeedTotal = 0;
		if (vamp)
			GetGame().RPCSingleParam(vamp, RPC_FEED, new Param1<bool>(false), true);
		if (second)
		{
			if (second.m_AZVampFeedingVictim == victim)
				second.m_AZVampFeedingVictim = null;
			Credit(second, victim, total2);
			GetGame().RPCSingleParam(second, RPC_FEED, new Param1<bool>(false), true);
		}
		if (vamp && vamp.m_AZVampTimedVictim == victim)
		{
			vamp.m_AZVampTimedVictim = null;
			if (!AZ_VampShadow.s_InChain)
				AZ_VampShadow.BiteAnim(vamp, 2, 0, 0);
		}
		if (vamp && victim)
			AZ_VampShadow.Released(vamp, victim);
		// Release(null, victim) resolves its primary feeder above.
		if (vamp && vamp.m_AZVampFeedingVictim == victim)
			vamp.m_AZVampFeedingVictim = null;
	}


	static void Collapse(PlayerBase vamp, PlayerBase victim)
	{
		if (!victim || victim.m_AZVampCollapsed)
			return;
		AZ_VampEvidence.Begin("bite_collapse", vamp, victim, false);
		victim.m_AZVampCollapsed = true;
		victim.SetHealth("", "Shock", 0);
		AZ_VampEvidence.End("bite_collapse", vamp, victim);

		bool elderAttempt = victim == AZ_VampAltar.s_ElderVictim && vamp == AZ_VampAltar.s_ElderActor;
		bool capOk = AZ_VampState.CapAllowsTurn(!elderAttempt);
		string source = "player_bite";
		if (vamp && !vamp.GetIdentity())
			source = "npc_bite";
		if (elderAttempt)
		{
			source = "elder";
			int reserved = AZ_VampState.CountOnlineVampires() + AZ_VampState.CountOnlineTurning() + AZ_VampAltar.s_Pending.Count();
			capOk = capOk && reserved < AZ_VampAltar.SeedTarget();
		}
		if (capOk)
			AZ_VampTurn.StartTurning(victim, -1, source);
		else
		{
			AZ_VampAudit.Life(victim, "INFECTION_BLOCKED_CAP_OR_TARGET", source);
			// The collapse hides the HUD; deliver the refusal when this victim wakes.
			victim.m_AZVampCapWakeMessage = true;
		}
		if (vamp)
		{
			if (capOk)
				vamp.Message("They collapse. Let go to spare them; drink more and they die.", "colorAction");
			else
				vamp.Message("They collapse, but they will not turn: too many of us walk tonight.", "colorAction");
			AZ_VampTie.Hint(vamp, "bind", "With rope, tape or wire: the Bind hands key ({AZ_TIE}) beside them binds their hands.");
		}
		Print("[AZ_Vamp] bite collapse " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " turning " + victim.m_AZVampTurning + " capOk " + capOk);
		Release(vamp, victim);
	}



	// AZ-2516: a victim who drops out of the drain (logout, crash, alt-F4) is still dying: vanilla would kill an unconscious
	// logout anyway. Count the disconnect as the finished feed for the vampire (the rest of the blood, up to what they can
	// hold, and the drained-dry kill) instead of the body dying on its own with nobody credited.
	static string DcHash(PlayerBase p)
	{
		if (p && p.GetIdentity() && p.GetIdentity().GetPlainId() != "")
			return AZ_VampEvidence.SteamHash(p.GetIdentity().GetPlainId());
		return "unknown";
	}

	static bool DisconnectFinish(PlayerBase victim)
	{
		if (!victim || !GetGame().IsServer() || !victim.IsAlive() || !victim.m_AZVampHeld || !victim.m_AZVampCollapsed || victim.m_AZVampTimedFeed)
			return false;
		if (!GetGame().GetMission() || !GetGame().GetMission().IsPlayerDisconnecting(victim))
			return false;
		PlayerBase vamp = victim.m_AZVampFeeder;
		if (!vamp || !vamp.IsAlive() || vamp.IsUnconscious())
			return false;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		float left = Math.Max(0, victim.GetHealth("", "Blood") - cfg.deathBlood);
		if (!vamp.m_AZBloodMoon)
			left = Math.Min(left, Math.Max(0, vamp.GetMaxHealth("", "Blood") - vamp.GetHealth("", "Blood")) / Math.Max(0.01, cfg.feedGain));
		if (left > 0)
		{
			victim.AddHealth("", "Blood", -left);
			victim.m_AZVampFeedTotal += left;
			vamp.AddHealth("", "Blood", left * cfg.feedGain);
			AZ_VampReveal.StarveReset(vamp);
		}
		Print("[AZ_Vamp] drink-dry disconnect credited " + DcHash(vamp) + " -> " + DcHash(victim) + " drank " + victim.m_AZVampFeedTotal + " ml (" + left + " ml on disconnect)");
		DrainedToDeath(vamp, victim);
		return true;
	}


	static void DrainedToDeath(PlayerBase vamp, PlayerBase victim)
	{
		if (!victim || !victim.IsAlive())
			return;
		if (victim.m_AZVampTurning)
		{
			victim.m_AZVampTurning = false;
			AZ_VampState.SetSickLeft(victim, 0);
			victim.SetSynchDirty();
		}
		Print("[AZ_Vamp] drained to death " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim));
		AZ_VampEvidence.Begin("drained_to_death", vamp, victim, false);
		victim.SetHealth("", "", 0);
		AZ_VampEvidence.End("drained_to_death", vamp, victim);
		if (vamp)
			vamp.Message("Drained dry.", "colorAction");
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (cfg.ghoulRiseSec > 0 && LiveGhouls() < GHOUL_CAP)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Ghoul, cfg.ghoulRiseSec * 1000, false, victim);
	}

	static const int GHOUL_CAP = 40;
	static ref array<Object> s_Ghouls = new array<Object>;


	static int LiveGhouls()
	{
		for (int i = s_Ghouls.Count() - 1; i >= 0; i--)
		{
			EntityAI g = EntityAI.Cast(s_Ghouls[i]);
			if (!g || !g.IsAlive())
				s_Ghouls.Remove(i);
		}
		return s_Ghouls.Count();
	}



	static void DawnFall()
	{
		int n = DropAll(s_Ghouls) + DropAll(AZ_VampGraves.s_Risen);
		if (n > 0)
			Print("[AZ_Vamp] dawn: " + n + " ghouls and grave risen fell");
	}

	static int DropAll(array<Object> list)
	{
		int n = 0;
		foreach (Object o : list)
		{
			EntityAI e = EntityAI.Cast(o);
			if (e && e.IsAlive())
			{
				e.SetHealth("", "", 0);
				n++;
			}
		}
		list.Clear();
		return n;
	}




	static void Ghoul(PlayerBase corpse)
	{
		if (!corpse || corpse.IsAlive() || !AZ_VampEvent.Active() || LiveGhouls() >= GHOUL_CAP)
			return;
		vector pos = corpse.GetPosition();
		bool male = corpse.IsMale();
		float yaw = corpse.GetOrientation()[0];
		array<EntityAI> drop = new array<EntityAI>;
		for (int i = 0; i < corpse.GetInventory().AttachmentCount(); i++)
			drop.Insert(corpse.GetInventory().GetAttachmentFromIndex(i));
		if (corpse.GetItemInHands())
			drop.Insert(corpse.GetItemInHands());
		foreach (EntityAI it : drop)
		{
			if (it)
				corpse.GetInventory().DropEntity(InventoryMode.SERVER, corpse, it);
		}
		GetGame().ObjectDelete(corpse);
		array<string> women = {"ZmbF_CitizenANormal_Brown", "ZmbF_CitizenBSkinny", "ZmbF_HikerSkinny_Grey", "ZmbF_JoggerSkinny_Red", "ZmbF_Clerk_Normal_White"};
		array<string> men = {"ZmbM_CitizenASkinny_Blue", "ZmbM_CitizenBFat_Red", "ZmbM_HikerSkinny_Green", "ZmbM_JournalistSkinny", "ZmbM_MechanicSkinny_Grey"};
		string cls = women.GetRandomElement();
		if (male)
			cls = men.GetRandomElement();
		Object z = GetGame().CreateObjectEx(cls, pos, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS);
		if (!z)
			return;
		z.SetOrientation(Vector(yaw, 0, 0));
		s_Ghouls.Insert(z);
		Print("[AZ_Vamp] the drained rise: " + cls + " at " + pos + ", " + drop.Count() + " items dropped (" + s_Ghouls.Count() + " ghouls)");
	}
}


class AZ_PPEVampDrain extends PPERequester_GameplayBase
{
	static const int L_VIGNETTE = 450;

	void SetPulse(float v)
	{
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, v, L_VIGNETTE, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {0.4 * v, 0.0, 0.0, 0.0}, L_VIGNETTE, PPOperators.ADD);
	}

	static AZ_PPEVampDrain Get()
	{
		AZ_PPEVampDrain r = AZ_PPEVampDrain.Cast(PPERequesterBank.GetRequester(AZ_PPEVampDrain));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEVampDrain);
			r = AZ_PPEVampDrain.Cast(PPERequesterBank.GetRequester(AZ_PPEVampDrain));
		}
		return r;
	}
}




class AZ_PPEVampCompel extends PPERequester_GameplayBase
{
	static const int L_VIG = 460;
	static const int L_OTHER = 120;

	void SetTrance(float v, float t)
	{
		float pulse = 0.5 + 0.5 * Math.Sin(t * 2.2);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTE, false, v * (0.7 + 0.25 * pulse), L_VIG, PPOperators.HIGHEST);
		SetTargetValueColor(PostProcessEffectType.Glow, PPEGlow.PARAM_VIGNETTECOLOR, {0.32 * v, 0.0, 0.03 * v, 0.0}, L_VIG, PPOperators.ADD);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_SATURATION, false, 1 - 0.6 * v, L_OTHER, PPOperators.LOWEST);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur, PPERadialBlur.PARAM_POWERX, false, 0.08 * v, L_OTHER, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur, PPERadialBlur.PARAM_POWERY, false, 0.08 * v, L_OTHER, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur, PPERadialBlur.PARAM_OFFSETX, false, 0.45, L_OTHER, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.RadialBlur, PPERadialBlur.PARAM_OFFSETY, false, 0.45, L_OTHER, PPOperators.SET);
		SetTargetValueFloat(PostProcessEffectType.Glow, PPEGlow.PARAM_LENSDISTORT, false, 0.1 * v * pulse, L_OTHER, PPOperators.SET);
	}

	static AZ_PPEVampCompel Get()
	{
		AZ_PPEVampCompel r = AZ_PPEVampCompel.Cast(PPERequesterBank.GetRequester(AZ_PPEVampCompel));
		if (!r)
		{
			PPERequesterBank.RegisterRequester(AZ_PPEVampCompel);
			r = AZ_PPEVampCompel.Cast(PPERequesterBank.GetRequester(AZ_PPEVampCompel));
		}
		return r;
	}
}

modded class PlayerBase
{


	override bool CanJump()
	{
		// AZ-2621: the blood moon frenzy holds the feeding vampire too; jumping must not cut the feed short.
		if (m_AZVampHeld || m_AZFrenzySync)
		{
			return false;
		}
		return super.CanJump();
	}

	override bool CanClimb(int climbType, SHumanCommandClimbResult climbRes)
	{
		if (m_AZVampHeld)
		{
			return false;
		}
		return super.CanClimb(climbType, climbRes);
	}

	bool m_AZVampHeld;
	bool m_AZVampCompelled;
	int m_AZCompelFreeAt;
	bool m_AZVampTimedFeed;
	PlayerBase m_AZVampTimedVictim;
	PlayerBase m_AZVampFeedingVictim;
	bool m_AZFrenzySync;
	int m_AZFeedCancelUntil;
	float m_AZVampFeedPerSec;
	bool m_AZVampGraceSync;
	bool m_AZVampCoolSync;
	float m_AZVampFeedTotal;
	int m_AZVampHeldUntil;
	int m_AZVampFloorAt;
	bool m_AZVampCollapsed;
	bool m_AZVampCapWakeMessage;

	PlayerBase m_AZVampFeeder;
	PlayerBase m_AZVampFeeder2;
	float m_AZVampFeedTotal2;







	protected float m_AZDummyLogAt;
	protected float m_AZDummyOutFor;

	void AZ_VampDummySim(float dt, int cmd)
	{


		HumanCommandUnconscious hcu = GetCommand_Unconscious();
		if (m_ShouldBeUnconscious && !hcu && cmd != DayZPlayerConstants.COMMANDID_DEATH && cmd != DayZPlayerConstants.COMMANDID_FALL)
		{
			StartCommand_Unconscious(0);
			m_IsUnconscious = true;
			m_AZDummyOutFor = 0;
			SetSynchDirty();
		}
		if (hcu)
			m_AZDummyOutFor += dt;
		if (!m_ShouldBeUnconscious && hcu && m_AZDummyOutFor > 2 && !PhysicsIsFalling(false))
		{
			hcu.WakeUp(DayZPlayerConstants.STANCEIDX_ERECT);
			m_IsUnconscious = false;
			SetSynchDirty();
		}
		float shock = GetHealth("", "Shock");
		if (!m_ShouldBeUnconscious)
		{
			if (shock < PlayerConstants.UNCONSCIOUS_THRESHOLD)
			{
				m_ShouldBeUnconscious = true;
				DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, true);
				Print("[AZ_Vamp] dummy " + AZ_VampState.Uid(this) + " goes unconscious (shock " + Math.Round(shock) + ")");
				return;
			}
			if (shock < 100 && !m_AZVampHeld)
				AddHealth("", "Shock", PlayerConstants.SHOCK_REFILL_CONSCIOUS_SPEED * dt);
			return;
		}
		float rate = PlayerConstants.SHOCK_REFILl_UNCONSCIOUS_SPEED;
		if (GetHealth("", "Blood") < PlayerConstants.SL_BLOOD_CRITICAL)
			rate *= PlayerConstants.SHOCK_REFILL_UNCON_CRITICAL_BLOOD_MLTP;
		AddHealth("", "Shock", rate * dt);
		if (GetHealth("", "Shock") > PlayerConstants.CONSCIOUS_THRESHOLD)
		{
			m_ShouldBeUnconscious = false;
			DayZPlayerSyncJunctures.SendPlayerUnconsciousness(this, false);
			Print("[AZ_Vamp] dummy " + AZ_VampState.Uid(this) + " wakes up");
		}
	}
	int m_AZVampProbeAt;
	float m_AZVampPulse;
	bool m_AZVampPulseOn;
	ref EffectSound m_AZVampFeedSound;
	ref EffectSound m_AZVampHumSound;

	bool m_AZTranceOn;
	int m_AZCompelSeenAt;
	static const int STARE_LOCK_MS = 4000;

	bool m_AZForceSoundOn;



	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (!GetGame().IsClient())
			return;
		if (m_AZVampCompelled && !m_AZForceSoundOn)
		{
			m_AZForceSoundOn = true;
			EffectSound compel = SEffectManager.PlaySoundOnObject("AZ_VampCompel_SoundSet", this);
			if (compel)
				compel.SetSoundAutodestroy(true);
		}
		else if (!m_AZVampCompelled)
			m_AZForceSoundOn = false;
	}

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZVampHeld");
		RegisterNetSyncVariableBool("m_AZVampGraceSync");
		RegisterNetSyncVariableBool("m_AZVampCoolSync");
		RegisterNetSyncVariableBool("m_AZVampCompelled");
		RegisterNetSyncVariableBool("m_AZFrenzySync");
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (GetGame().IsServer() && !GetIdentity() && IsAlive())
			AZ_VampDummySim(pDt, pCurrentCommandID);
		if (GetGame().IsServer())
		{
			// Same conditions as the let-go refusal in AZ_VampFeed.Cancel.
			PlayerBase me = this;
			PlayerBase fv = m_AZVampFeedingVictim;
			bool frenzy = me.m_AZBloodMoon && fv && AZ_VampState.Cfg().moonNoLetGo && fv.IsAlive() && !fv.IsUnconscious() && fv.m_AZVampHeld && (fv.m_AZVampFeeder == this || fv.m_AZVampFeeder2 == this);
			if (frenzy != m_AZFrenzySync)
			{
				m_AZFrenzySync = frenzy;
				SetSynchDirty();
			}
		}

		if (!m_AZVampCompelled)
			m_AZCompelSeenAt = 0;
		if (m_AZVampCompelled && !m_AZVampHeld)
		{



			HumanInputController hicC = GetInputController();
			if (hicC)
			{
				hicC.OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 0);
				hicC.OverrideRaise(HumanInputControllerOverrideType.ONE_FRAME, false);
				hicC.OverrideMeleeEvade(HumanInputControllerOverrideType.ONE_FRAME, false);
				hicC.OverrideAimChangeX(HumanInputControllerOverrideType.ONE_FRAME, 0);
				int nowC = GetGame().GetTime();
				if (m_AZCompelSeenAt == 0)
					m_AZCompelSeenAt = nowC;
				if (nowC - m_AZCompelSeenAt < STARE_LOCK_MS)
					hicC.OverrideAimChangeY(HumanInputControllerOverrideType.ONE_FRAME, 0);
			}
			if (GetGame().IsClient() && IsControlledPlayer())
			{
				int since = GetGame().GetTime() - m_AZCompelSeenAt;
				m_AZTranceOn = true;
				float v = Math.Clamp(since / 800.0, 0, 1);
				if (since > STARE_LOCK_MS)
					v = Math.Max(0.5, 1 - (since - STARE_LOCK_MS) / 1500.0);
				AZ_PPEVampCompel tr = AZ_PPEVampCompel.Get();
				if (tr)
					tr.SetTrance(v, GetGame().GetTime() / 1000.0);
			}
		}
		else if (m_AZTranceOn && GetGame().IsClient())
		{
			m_AZTranceOn = false;
			AZ_PPEVampCompel trOff = AZ_PPEVampCompel.Get();
			if (trOff)
				trOff.Stop();
		}

		if (m_AZVampHeld)
		{
			HumanInputController hic = GetInputController();
			if (hic)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 0);
				hic.OverrideRaise(HumanInputControllerOverrideType.ONE_FRAME, false);
				hic.OverrideMeleeEvade(HumanInputControllerOverrideType.ONE_FRAME, false);
				hic.OverrideAimChangeX(HumanInputControllerOverrideType.ONE_FRAME, 0);
			}
			if (GetGame().IsServer() && AZ_VampFeed.KeepSheltered(this))
			{
				AZ_VampConfig cfg = AZ_VampState.Cfg();
				PlayerBase feeder = m_AZVampFeeder;


				bool lost = !feeder || !feeder.IsAlive() || feeder.IsUnconscious();
				float rate = m_AZVampFeedPerSec;
				if (lost)
					rate = 0;
				float blood = GetHealth("", "Blood");
				float take = rate * pDt;


				if (!m_AZVampCollapsed)
					take = Math.Min(take, Math.Max(0, blood - cfg.bloodFloor));
				if (feeder && !feeder.m_AZBloodMoon)
					take = Math.Min(take, Math.Max(0, feeder.GetMaxHealth("", "Blood") - feeder.GetHealth("", "Blood")) / Math.Max(0.01, cfg.feedGain));
				if (take > 0)
				{
					AddHealth("", "Blood", -take);
					m_AZVampFeedTotal += take;
					if (feeder)
					{
						feeder.AddHealth("", "Blood", take * cfg.feedGain);
						AZ_VampReveal.StarveReset(feeder);
					}
				}
				PlayerBase feeder2 = m_AZVampFeeder2;
				if (feeder2 && (!feeder2.IsAlive() || feeder2.IsUnconscious()))
				{
					AZ_VampFeed.LetGoOne(feeder2, this);
					feeder2 = null;
				}
				if (feeder2 && !lost)
				{
					blood = GetHealth("", "Blood");
					float take2 = m_AZVampFeedPerSec * pDt;
					if (!m_AZVampCollapsed)
						take2 = Math.Min(take2, Math.Max(0, blood - cfg.bloodFloor));
					if (!feeder2.m_AZBloodMoon)
						take2 = Math.Min(take2, Math.Max(0, feeder2.GetMaxHealth("", "Blood") - feeder2.GetHealth("", "Blood")) / Math.Max(0.01, cfg.feedGain));
					if (take2 > 0)
					{
						AddHealth("", "Blood", -take2);
						m_AZVampFeedTotal2 += take2;
						feeder2.AddHealth("", "Blood", take2 * cfg.feedGain);
						AZ_VampReveal.StarveReset(feeder2);
					}
				}
				if (!lost)
					AZ_VampFeed.DisconnectFinish(this);
				blood = GetHealth("", "Blood");
				if (lost && m_AZVampFeeder2 && IsAlive())
				{

					AZ_VampFeed.LetGoOne(feeder, this);
					lost = false;
				}
				else if (lost)
				{
				}
				else if (!m_AZVampTimedFeed && !m_AZVampCollapsed && blood <= cfg.bloodFloor + 1)
					AZ_VampFeed.Collapse(feeder, this);
				else if (!m_AZVampTimedFeed && m_AZVampCollapsed && blood <= cfg.deathBlood + 1)
					AZ_VampFeed.DrainedToDeath(feeder, this);
				int nowMs = GetGame().GetTime();
				if (cfg.alignProbe && nowMs >= m_AZVampProbeAt)
				{
					m_AZVampProbeAt = nowMs + AZ_VampProbe.STEP_MS;
					AZ_VampProbe.Measure(feeder, this, (nowMs - (m_AZVampHeldUntil - cfg.drinkMaxSec * 1000 - AZ_VampFeed.SAFETY_MS)) / 1000.0);
				}
				if (lost || GetGame().GetTime() > m_AZVampHeldUntil || !IsAlive())
					AZ_VampFeed.Release(null, this);
			}
		}


		if (GetGame().IsClient() && IsControlledPlayer())
		{
			if (m_AZVampHeld)
			{
				m_AZVampPulse += pDt * 7.3;
				float beat = Math.Pow(Math.AbsFloat(Math.Sin(m_AZVampPulse * 0.5)), 6);
				AZ_PPEVampDrain req = AZ_PPEVampDrain.Get();
				if (req)
					req.SetPulse(0.35 + 0.45 * beat);
				m_AZVampPulseOn = true;
			}
			else if (m_AZVampPulseOn)
			{
				m_AZVampPulseOn = false;
				AZ_PPEVampDrain r = AZ_PPEVampDrain.Get();
				if (r)
					r.Stop();
			}
		}
	}



	float m_AZCamMin = 99;
	int m_AZCamStartAt;
	bool m_AZCamMidLogged;

	void AZ_VampCamDebug(bool on)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampCamTick);
		if (on)
		{
			m_AZCamMin = 99;
			m_AZCamStartAt = GetGame().GetTime();
			m_AZCamMidLogged = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCamTick, 100, true);
			return;
		}
		if (m_AZCamMin < 99)
		{
			string flag = "";
			if (m_AZCamMin < 0.12)
				flag = " | CAMERA IN HEAD";
			Print("[AZ_Vamp] bite camera min " + Math.Round(m_AZCamMin * 100) + " cm over the feed" + flag);
		}
		m_AZCamMin = 99;
	}

	void AZ_VampCamTick()
	{
		vector cam = GetGame().GetCurrentCameraPosition();
		array<Object> objs = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		GetGame().GetObjectsAtPosition3D(cam, 2.0, objs, proxies);
		float best = 99;
		foreach (Object o : objs)
		{
			PlayerBase h = PlayerBase.Cast(o);
			if (!h || h == this || !h.m_AZVampHeld)
				continue;
			int ih = h.GetBoneIndexByName("Head");
			if (ih >= 0)
				best = Math.Min(best, vector.Distance(cam, h.GetBonePositionWS(ih)));
		}
		if (best >= 99)
			return;
		m_AZCamMin = Math.Min(m_AZCamMin, best);
		if (!m_AZCamMidLogged && GetGame().GetTime() - m_AZCamStartAt >= 2500)
		{
			m_AZCamMidLogged = true;
			string flag = "";
			if (best < 0.12)
				flag = " | CAMERA IN HEAD";
			Print("[AZ_Vamp] bite camera mid-drink: camera -> victim Head " + Math.Round(best * 100) + " cm" + flag);
		}
	}


	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		// Only a hit from someone or something else breaks the feed: falls and own bleeding are self-sourced, environment has no source.
		bool foreign = source && source != this && source.GetHierarchyRoot() != this;
		if (GetGame().IsServer() && foreign && damageResult && (damageResult.GetDamage("", "Health") > 0 || damageResult.GetDamage("", "Shock") > 0))
		{
			AZ_VampShadowState hitChain = AZ_VampShadow.s_State.Get(this);
			if (hitChain && (hitChain.victim || hitChain.animWaiting))
				AZ_VampShadow.Abort(this);
			else if (m_AZVampTimedVictim)
				AZ_VampFeed.Release(this, m_AZVampTimedVictim);
		}
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampFeed.RPC_FEED || !GetGame().IsClient())
			return;
		Param1<bool> p = new Param1<bool>(false);
		if (!ctx.Read(p))
			return;
		if (PlayerBase.Cast(GetGame().GetPlayer()) == this)
			AZ_VampCamDebug(p.param1);
		AZ_VampStopFeedSounds();
		if (p.param1)
		{
			m_AZVampFeedSound = SEffectManager.PlaySoundOnObject("AZ_VampFeed_SoundSet", this);
			string hum = "AZ_VampHumF_SoundSet";
			if (IsMale())
				hum = "AZ_VampHumM_SoundSet";
			m_AZVampHumSound = SEffectManager.PlaySoundOnObject(hum, this, 0.3, 0.3, true);
		}
	}

	// F06: the two retained feed sounds are destroyed, not just stopped, when replaced, stopped or the feeder goes:
	// a stopped sound stays registered with the effect manager until the mission ends
	void AZ_VampStopFeedSounds()
	{
		if (m_AZVampFeedSound)
		{
			SEffectManager.DestroyEffect(m_AZVampFeedSound);
			m_AZVampFeedSound = null;
		}
		if (m_AZVampHumSound)
		{
			SEffectManager.DestroyEffect(m_AZVampHumSound);
			m_AZVampHumSound = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (!GetGame().IsServer())
			AZ_VampStopFeedSounds();
		super.EEDelete(parent);
	}
}
