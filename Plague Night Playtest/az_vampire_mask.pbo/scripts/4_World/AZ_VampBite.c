








class AZ_VampRear
{
	static const int CMD_VAMP = DayZPlayerConstants.CMD_ACTIONFB_WASHFACEWELL;
	static const int CMD_VICTIM = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAG;
	static int s_Have = -1;

	static bool Available()
	{
		if (s_Have < 0)
		{
			s_Have = 0;
			if (GetGame().ConfigIsExisting("CfgPatches AZ_VampireAnims_Rear"))
				s_Have = 1;
		}
		return s_Have == 1;
	}


	static bool Behind(PlayerBase vamp, PlayerBase victim)
	{
		vector f = victim.GetDirection();
		f[1] = 0;
		vector d = vamp.GetPosition() - victim.GetPosition();
		d[1] = 0;
		if (f.Length() < 0.01 || d.Length() < 0.05)
			return false;
		return vector.Dot(f.Normalized(), d.Normalized()) < -0.5;
	}
}




class AZ_VampReach
{
	// AZ-2510: refuse excessive correction; never partially align a bite.
	static const float BITE_MAX_DY = 0.60;

	static bool LevelBite(PlayerBase victim, out vector pos)
	{
		if (!victim)
			return AZ_VampShadow.PathWhy("bite_height");
		vector victimPos = victim.GetPosition();
		// Equal terrain elevations: keep the original flat-ground root transform exactly.
		if (GetGame().SurfaceY(pos[0], pos[2]) == GetGame().SurfaceY(victimPos[0], victimPos[2]))
			return true;
		if (Math.AbsFloat(pos[1] - victimPos[1]) > BITE_MAX_DY)
			return AZ_VampShadow.PathWhy("bite_height");
		pos[1] = victimPos[1];
		return true;
	}

	static const float MAX_DY = 0.15;
	static const float MIN_DOT = 0.7;
	static const int MEASURE_MS = 2500;
	static const float START_RANGE = 1.0;

	static bool Ahead(PlayerBase vamp, PlayerBase victim)
	{
		vector fwd = vamp.GetDirection();
		fwd[1] = 0;
		vector d = victim.GetPosition() - vamp.GetPosition();
		d[1] = 0;
		if (fwd.Length() < 0.01 || d.Length() < 0.05)
			return false;
		return vector.Dot(fwd.Normalized(), d.Normalized()) >= MIN_DOT;
	}

	static float Dy(PlayerBase vamp, PlayerBase victim)
	{
		return Math.AbsFloat(vamp.GetPosition()[1] - victim.GetPosition()[1]);
	}
}

class ActionAZVampBiteCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(AZ_VampState.Cfg().drinkMaxSec);
	}
}

class ActionAZVampBite : ActionContinuousBase
{
	protected bool m_AZRear;

	void ActionAZVampBite()
	{
		m_CallbackClass = ActionAZVampBiteCB;


		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CLEANWOUNDSTARGET;
		m_FullBody = true;

		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Bite";
	}

	// AZ-2525: a raised weapon fails the condition mask before ActionCondition runs, so no message was ever shown.
	override bool CanBeUsedRaised()
	{
		return true;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTMan(AZ_VampReach.START_RANGE);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PlayerBase victim = PlayerBase.Cast(target.GetObject());
		if (!player || player.IsRestrained() || !player.m_AZVampire || !victim || !victim.IsAlive() || victim.m_AZVampire || victim.m_AZVampTurning)
			return false;
		// Conservative U8 fallback: front feeding requires a standing bound victim.
		// Rear feeding retains the supported bound kneeling route.
		if (!m_AZRear && victim.IsRestrained() && victim.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
		{
			int frontKneelNow = GetGame().GetTime();
			if (frontKneelNow >= player.m_AZVampBiteWhyAt)
			{
				player.m_AZVampBiteWhyAt = frontKneelNow + 4000;
				player.Message("They must stand before you can bite from the front.", "colorAction");
			}
			return false;
		}
		if (AZ_VampDown.RefuseBite(player))
			return false;
		if (GetGame().GetTime() < player.m_AZFeedCancelUntil)
			return false;
		if (victim.IsUnconscious())
			return false;
		if (!player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_RAISEDERECT))
			return false;
		// Bound feed supports a kneeling victim; the vampire must still stand.
		bool crouched = victim.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH);
		AZ_VampConfig crouchCfg = AZ_VampState.Cfg();
		bool crouchOk = crouched && !victim.IsRestrained() && (!crouchCfg || crouchCfg.biteCrouched);
		if (!victim.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_RAISEDERECT) && !crouchOk && !(victim.IsRestrained() && AZ_VampFeedAnim.BoundAvailable() && crouched))
			return false;
		// AZ-2525: tell the client why the prompt is gone (the server-side line is never reached once the client refuses).
		if (!GetGame().IsServer() && !victim.m_AZVampHeld && player.GetItemInHands())
		{
			int nowH = GetGame().GetTime();
			if (nowH >= player.m_AZVampBiteWhyAt)
			{
				player.m_AZVampBiteWhyAt = nowH + 4000;
				player.Message("Free your hands to bite.", "colorAction");
			}
			return false;
		}

		if (!victim.m_AZVampHeld)
		{
			bool wantRear = false;



			bool tiedOk = victim.IsRestrained() && AZ_VampFeedAnim.BoundAvailable();
			if (AZ_VampRear.Available() && (AZ_VampRear.Behind(player, victim) || (!tiedOk && victim.GetItemInHands())))
				wantRear = true;
			if (wantRear != m_AZRear)
				return false;
		}
		else
		{


			ActionBase running = player.GetActionManager().GetRunningAction();
			if (!running || running.Type() != Type())
			{
				bool behind = AZ_VampRear.Available() && AZ_VampRear.Behind(player, victim);
				if (behind != m_AZRear)
					return false;
			}
		}



		if (!GetGame().IsServer() && !victim.m_AZVampHeld && (victim.m_AZVampGraceSync || (victim.m_AZVampCoolSync && !player.m_AZBloodMoon)))
		{
			int nowC = GetGame().GetTime();
			if (nowC >= player.m_AZVampBiteWhyAt)
			{
				player.m_AZVampBiteWhyAt = nowC + 4000;
				if (victim.m_AZVampGraceSync)
					player.Message("They only just arrived. Not yet.", "colorAction");
				else
					player.Message("Their blood is too thin. Let them recover first.", "colorAction");
			}
			return false;
		}
		if (GetGame().IsServer())
		{
			string why;
			if (victim.m_AZVampHeld && victim.m_AZVampFeeder != player && victim.m_AZVampFeeder2 != player)
			{
				if (!AZ_VampFeed.Shared() || victim.m_AZVampCollapsed)
					return false;
				if (victim.m_AZVampFeeder2)
					why = "Two already drink from them.";
				else if (m_AZRear == victim.m_AZVampRear)
					why = "Another vampire already drinks from that side.";
			}

			if (why != "")
			{
			}
			else if (AZ_VampReach.Dy(player, victim) > AZ_VampReach.MAX_DY)
				why = "Not here. Get level with them.";
			else if (!victim.m_AZVampHeld && !AZ_VampReach.Ahead(player, victim))
				why = "Face them to bite.";
			else if (player.GetItemInHands())
				why = "Free your hands to bite.";
			else if (!AZ_VampEvent.Active())
				why = "The plague sleeps. Wait for Plague Night to begin.";
			else if (!AZ_VampFeed.CanFeedHere(player))
				why = "Not in direct sunlight. Find shelter to feed.";
			else if (!victim.m_AZVampHeld && !player.m_AZBloodMoon && player.GetHealth("", "Blood") > AZ_VampState.Cfg().fullBlood)
				why = "You are full. The hunger will come back.";

			else if (!victim.m_AZVampHeld && !player.m_AZBloodMoon && victim.GetHealth("", "Blood") < AZ_VampState.Cfg().bloodFloor + 150)
				why = "Their blood is too thin. Let them recover first.";
			else if (victim.m_AZVampHeld && !player.m_AZBloodMoon && player.GetHealth("", "Blood") >= player.GetMaxHealth("", "Blood") - 5)
				why = "You can drink no more. Let go.";
			else if (AZ_VampState.IsImmune(victim))
				why = "Their blood is still burning with the cure.";
			else if (!victim.m_AZVampHeld && AZ_VampCrypt.IsGuest(victim))
				why = "Not yet. The altar protects them.";
			else if (!victim.m_AZVampHeld && AZ_VampState.InSpawnGrace(victim))
				why = "They only just arrived. Not yet.";
			else if (!victim.m_AZVampHeld && !player.m_AZBloodMoon && AZ_VampState.InBiteCooldown(victim))
				why = "Their blood is too thin. Let them recover first.";
			if (why != "")
			{

				int now = GetGame().GetTime();
				if (now >= player.m_AZVampBiteWhyAt)
				{
					player.m_AZVampBiteWhyAt = now + 4000;
					player.Message(why, "colorAction");
					Print("[AZ_Vamp] bite refused " + AZ_VampState.Uid(player) + ": " + why);
				}
				return false;
			}
		}
		return true;
	}


	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		if (AZ_VampDown.RefuseBite(action_data.m_Player) || GetGame().GetTime() < action_data.m_Player.m_AZFeedCancelUntil)
		{
			action_data.m_Player.GetActionManager().Interrupt();
			return;
		}
		PlayerBase levelVictim = PlayerBase.Cast(action_data.m_Target.GetObject());
		vector levelPos = action_data.m_Player.GetPosition();
		if (!AZ_VampReach.LevelBite(levelVictim, levelPos))
		{
			AZ_VampShadow.NoPath(action_data.m_Player);
			action_data.m_Player.GetActionManager().Interrupt();
			return;
		}
		if (levelPos[1] != action_data.m_Player.GetPosition()[1])
		{
			AZ_VampSnap.To(action_data.m_Player, levelPos, action_data.m_Player.GetDirection());
			action_data.m_Player.AZ_BiteLevelStart(levelVictim);
		}
		AZ_VampReveal.BareAtWill(action_data.m_Player, AZ_VampState.Cfg().drinkMaxSec + 2);
		PlayerBase victim = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (victim && victim.m_AZVampHeld && victim.m_AZVampFeeder && victim.m_AZVampFeeder != action_data.m_Player)
		{


			if (victim.m_AZVampFeeder2 || m_AZRear == victim.m_AZVampRear)
				return;
			SnapJoin(action_data.m_Player, victim, m_AZRear);
			AZ_VampFeed.Join(action_data.m_Player, victim);
			action_data.m_Player.AZ_VampReachLater(victim, "join rear " + m_AZRear);
			return;
		}
		if (victim)
			victim.m_AZVampRear = m_AZRear;
		if (m_AZRear)
			SnapRear(action_data.m_Player, victim);
		else
			Snap(action_data.m_Player, victim);
		AZ_VampFeed.Hold(action_data.m_Player, victim);
		string tag = "front";
		if (m_AZRear)
			tag = "rear";
		action_data.m_Player.AZ_VampReachLater(victim, tag);
	}



	static void Snap(PlayerBase vamp, PlayerBase victim)
	{
		if (!vamp || !victim)
			return;
		vector fwd = vamp.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		vector d = victim.GetPosition() - vamp.GetPosition();
		d[1] = 0;
		float before = d.Length();


		if (before < 0.05 || vector.Dot(d.Normalized(), fwd) < 0.7)
		{
			Print("[AZ_Vamp] feed snap skipped " + AZ_VampState.Uid(victim) + " gap " + before + " m, not in front");
			return;
		}
		float gap = AZ_VampFeed.ROOT_GAP;
		if (vamp.IsMale())
			gap += AZ_VampState.Cfg().frontGapMale;
		vector pos = vamp.GetPosition() + fwd * gap;
		pos[1] = victim.GetPosition()[1];
		// AZ-2431: announce the bite snap to COT's movement check.
		AZ_VampSnap.AllowMove(victim);
		victim.SetPosition(pos);
		victim.SetDirection(-fwd);
		victim.AZ_VampPin(pos, -fwd);
		vector d2 = victim.GetPosition() - vamp.GetPosition();
		d2[1] = 0;
		Print("[AZ_Vamp] feed snap " + AZ_VampState.Uid(victim) + " gap " + before + " -> " + d2.Length() + " m");
	}


	static void SnapJoin(PlayerBase vamp, PlayerBase victim, bool rear)
	{
		if (!vamp || !victim)
			return;
		vector dir = victim.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		float gap = AZ_VampFeed.ROOT_GAP;
		if (!rear && vamp.IsMale())
			gap += AZ_VampState.Cfg().frontGapMale;
		vector pos = victim.GetPosition() + dir * gap;
		vector face = -dir;
		if (rear)
		{
			pos = victim.GetPosition() - dir * gap;
			face = dir;
		}
		pos[1] = vamp.GetPosition()[1];
		if (AZ_VampShadow.Blocked(victim, pos, vamp))
		{
			Print("[AZ_Vamp] feed join snap skipped " + AZ_VampState.Uid(vamp) + ": blocked");
			return;
		}
		// AZ-2431: announce the join snap to COT's movement check.
		AZ_VampSnap.AllowMove(vamp);
		vamp.SetPosition(pos);
		vamp.SetDirection(face);
		Print("[AZ_Vamp] feed join snap " + AZ_VampState.Uid(vamp) + " rear " + rear);
	}


	static void SnapRear(PlayerBase vamp, PlayerBase victim)
	{
		if (!vamp || !victim)
			return;
		vector fwd = vamp.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		vector d = victim.GetPosition() - vamp.GetPosition();
		d[1] = 0;
		float before = d.Length();
		if (before < 0.05 || vector.Dot(d.Normalized(), fwd) < 0.7)
		{
			Print("[AZ_Vamp] rear snap skipped " + AZ_VampState.Uid(victim) + " gap " + before + " m, not in front");
			return;
		}
		vector pos = vamp.GetPosition() + fwd * AZ_VampFeed.ROOT_GAP;
		pos[1] = victim.GetPosition()[1];
		// AZ-2431: announce the rear snap to COT's movement check.
		AZ_VampSnap.AllowMove(victim);
		victim.SetPosition(pos);
		victim.SetDirection(fwd);
		victim.AZ_VampPin(pos, fwd);
		Print("[AZ_Vamp] rear snap " + AZ_VampState.Uid(victim) + " gap " + before + " m");
	}


	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		PlayerBase victim;
		if (action_data.m_Target)
			victim = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (!victim)
			victim = action_data.m_Player.m_AZVampFeedingVictim;
		Print("[AZ_Vamp] bite action end state " + action_data.m_State + " victim " + victim);
		AZ_VampFeed.Release(action_data.m_Player, victim);
	}


	static void Apply(PlayerBase vamp, PlayerBase victim)
	{
		if (!vamp || !victim || !victim.IsAlive())
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		float taken = Math.Max(0, victim.GetHealth("", "Blood") - cfg.bloodFloor);
		AZ_VampEvidence.Begin("instant_bite", vamp, victim, false);
		victim.AddHealth("", "Blood", -taken);
		vamp.AddHealth("", "Blood", taken * cfg.feedGain);
		AZ_VampEvidence.End("instant_bite", vamp, victim);
		AZ_VampReveal.Fed(vamp);
		GetGame().RPCSingleParam(vamp, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
		AZ_VampFeed.Collapse(vamp, victim);
		Print("[AZ_Vamp] bite (instant) " + AZ_VampState.Uid(vamp) + " -> " + AZ_VampState.Uid(victim) + " took " + taken);
	}
}



class ActionAZVampDrainCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(AZ_VampState.Cfg().drinkMaxSec);
	}
}

class ActionAZVampDrain : ActionContinuousBase
{
	void ActionAZVampDrain()
	{
		m_CallbackClass = ActionAZVampDrainCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CHECKPULSE;


		if (GetGame().ConfigIsExisting("CfgPatches AZ_VampireAnims_Drain"))
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_BUILDROPELADDER;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Drink them dry";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PlayerBase victim = PlayerBase.Cast(target.GetObject());
		if (!player || player.IsRestrained() || !player.m_AZVampire || !victim || !victim.IsAlive() || victim.m_AZVampire)
			return false;
		if (AZ_VampDown.RefuseBite(player))
			return false;
		if (GetGame().GetTime() < player.m_AZFeedCancelUntil)
			return false;
		if (!player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
			return false;
		bool down = victim.IsUnconscious();


		if (!down && GetGame().IsServer() && !victim.GetIdentity())
			down = victim.GetHealth("", "Blood") <= AZ_VampState.Cfg().bloodFloor + 50;
		if (!down)
			return false;
		if (GetGame().IsServer())
		{
			if (!AZ_VampEvent.Active())
				return false;
			if (!AZ_VampFeed.CanFeedHere(player))
			{
				int nowSun = GetGame().GetTime();
				if (nowSun >= player.m_AZVampBiteWhyAt)
				{
					player.m_AZVampBiteWhyAt = nowSun + 4000;
					player.Message("Not in direct sunlight. Find shelter to feed.", "colorAction");
				}
				return false;
			}
			if (victim.m_AZVampHeld && victim.m_AZVampFeeder != player)
				return false;
			if (player.GetItemInHands())
			{
				int nowH = GetGame().GetTime();
				if (nowH >= player.m_AZVampBiteWhyAt)
				{
					player.m_AZVampBiteWhyAt = nowH + 4000;
					player.Message("Free your hands to drink.", "colorAction");
				}
				return false;
			}
			if (!player.m_AZBloodMoon && player.GetHealth("", "Blood") >= player.GetMaxHealth("", "Blood") - 5)
			{
				int now = GetGame().GetTime();
				if (now >= player.m_AZVampBiteWhyAt)
				{
					player.m_AZVampBiteWhyAt = now + 4000;
					player.Message("You can drink no more. Let go.", "colorAction");
				}
				return false;
			}
		}
		return true;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		PlayerBase victim = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (AZ_VampDown.RefuseBite(action_data.m_Player) || GetGame().GetTime() < action_data.m_Player.m_AZFeedCancelUntil)
		{
			action_data.m_Player.GetActionManager().Interrupt();
			return;
		}
		AZ_VampReveal.BareAtWill(action_data.m_Player, AZ_VampState.Cfg().drinkMaxSec + 2);
		AZ_VampFeed.Hold(action_data.m_Player, victim);
		if (victim)
		{
			if (m_CommandUID == DayZPlayerConstants.CMD_ACTIONFB_BUILDROPELADDER)
				AZ_DrainSnap(action_data.m_Player, victim);
			victim.m_AZVampCollapsed = true;
			victim.m_AZVampFeedPerSec = AZ_VampState.Cfg().finishPerSec;
		}
	}


	static void AZ_DrainSnap(PlayerBase vamp, PlayerBase victim)
	{
		int iNeck = victim.GetBoneIndexByName("Neck1");
		int iHead = victim.GetBoneIndexByName("Head");
		int iPelvis = victim.GetBoneIndexByName("Pelvis");
		if (iNeck < 0 || iHead < 0 || iPelvis < 0)
			return;
		vector neck = victim.GetBonePositionWS(iNeck);
		vector head = victim.GetBonePositionWS(iHead);
		float g = victim.GetPosition()[1];
		vector nh = neck - head;
		nh[1] = 0;


		vector dir = victim.GetBonePositionWS(iPelvis) - head;
		dir[1] = 0;
		Print("[AZ_Vamp] drain pose Neck1 z=" + (neck[1] - g) + " Head z=" + (head[1] - g) + " |Neck1-Head| horiz=" + nh.Length() + " head-pelvis horiz=" + dir.Length());
		if (dir.Length() < 0.15)
			return;
		dir.Normalize();
		vector pos = neck - dir * 0.62;
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
		if (vamp.GetPosition()[1] > pos[1] + 0.3 || vamp.GetPosition()[1] < pos[1] - 2)
			pos[1] = vamp.GetPosition()[1];
		// AZ-2431: announce the drain snap to COT's movement check.
		AZ_VampSnap.AllowMove(vamp);
		vamp.SetPosition(pos);
		vamp.SetDirection(dir);
		Print("[AZ_Vamp] drain snap " + Math.Round(vector.Distance(neck, pos) * 100) + " cm from the neck");
	}

	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		PlayerBase victim;
		if (action_data.m_Target)
			victim = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (!victim)
			victim = action_data.m_Player.m_AZVampFeedingVictim;
		AZ_VampFeed.Release(action_data.m_Player, victim);
	}
}

class ActionAZVampBiteRear : ActionAZVampBite
{
	void ActionAZVampBiteRear()
	{
		m_CommandUID = AZ_VampRear.CMD_VAMP;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_AZRear = true;
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZVampBite);
		actions.Insert(ActionAZVampBiteRear);
		actions.Insert(ActionAZVampDrain);
	}
}

modded class PlayerBase
{
	int m_AZVampBiteWhyAt;
	int m_AZShadowWhyAt;
	bool m_AZVampRear;
	int m_AZVampAnimRear;



	bool m_AZVampAnimHasPos;
	bool m_AZVampOwnerPin;
	vector m_AZVampAnimPos;
	vector m_AZVampAnimDir;


	bool m_AZVampBitten;
	int m_AZVampBittenUntil;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZVampBitten");
	}


	void AZ_VampMarkBitten()
	{
		m_AZVampBittenUntil = GetGame().GetTime() + AZ_VampState.Cfg().biteMarkMin * 60000;
		if (!m_AZVampBitten)
		{
			m_AZVampBitten = true;
			SetSynchDirty();
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampBiteMarkFade, AZ_VampState.Cfg().biteMarkMin * 60000 + 500, false);
	}



	vector m_AZVampPinPos;
	vector m_AZVampPinDir;
	bool m_AZVampPinned;
	int m_AZVampPinFixes;

	void AZ_VampPin(vector pos, vector dir)
	{
		m_AZVampPinPos = pos;
		m_AZVampPinDir = dir;
		m_AZVampPinned = true;
		m_AZVampPinFixes = 0;
	}




	void AZ_VampReachLater(PlayerBase victim, string tag)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampReachLog, AZ_VampReach.MEASURE_MS, false, victim, tag);
	}

	void AZ_VampReachLog(PlayerBase victim, string tag)
	{
		if (!victim)
			return;
		int ih = GetBoneIndexByName("Head");
		int inn = victim.GetBoneIndexByName("Neck1");
		if (ih < 0 || inn < 0)
			return;
		vector roots = victim.GetPosition() - GetPosition();
		float dy = roots[1];
		roots[1] = 0;
		string sex = "f";
		if (IsMale())
			sex = "m";


		vector head = GetBonePositionWS(ih);
		vector neck = victim.GetBonePositionWS(inn);
		vector f = GetDirection();
		f[1] = 0;
		f.Normalize();
		vector mouth = head + f * 0.10;
		vector mn = neck - mouth;
		float ahead = vector.Dot(mn, f);
		vector fs = f * vector.Up;
		float aside = vector.Dot(mn, fs);
		Print("[AZ_Vamp] reach " + tag + " " + sex + ": mouth -> neck " + Math.Round(vector.Distance(mouth, neck) * 1000) / 10 + " cm (ahead " + Math.Round(ahead * 1000) / 10 + ", aside " + Math.Round(aside * 1000) / 10 + ", up " + Math.Round(mn[1] * 1000) / 10 + "), head -> neck " + Math.Round(vector.Distance(head, neck) * 1000) / 10 + " cm, roots " + roots.Length() + " m, dy " + dy + " m, held " + victim.m_AZVampHeld + ", pin fixes " + victim.m_AZVampPinFixes);
		AZ_VampClipLog(victim, tag);
	}




	void AZ_VampClipLog(PlayerBase victim, string tag)
	{
		array<string> bones = {"Head", "Neck1", "Spine3"};
		string line = "[AZ_Vamp] bite clip " + tag + ":";
		string flags = "";
		foreach (string a : bones)
		{
			int ia = GetBoneIndexByName(a);
			if (ia < 0)
				continue;
			vector pa = GetBonePositionWS(ia);
			foreach (string b : bones)
			{
				int ib = victim.GetBoneIndexByName(b);
				if (ib < 0)
					continue;
				float d = vector.Distance(pa, victim.GetBonePositionWS(ib));
				line += " " + a + "-" + b + " " + Math.Round(d * 100) + "cm";
				float lim = 0.14;
				if (a == "Head" && b == "Head")
					lim = 0.16;
				else if (a == "Spine3" && b == "Spine3")
					lim = 0.22;
				else if (a == "Head" && b == "Neck1")
					lim = 0;
				if (d < lim)
					flags += " " + a + "-" + b;
			}
		}
		if (flags != "")
			line += " | CLIP?" + flags;
		Print(line);
	}

	void AZ_VampBiteMarkFade()
	{
		if (!m_AZVampBitten || GetGame().GetTime() < m_AZVampBittenUntil)
			return;
		m_AZVampBitten = false;
		SetSynchDirty();
	}

	override void SetActionsRemoteTarget(out TInputActionMap InputActionMap)
	{
		AddAction(ActionAZVampBite, InputActionMap);
		AddAction(ActionAZVampBiteRear, InputActionMap);
		AddAction(ActionAZVampDrain, InputActionMap);
		super.SetActionsRemoteTarget(InputActionMap);
	}
}

modded class PlayerBase
{
	PlayerBase m_AZBiteLevelVictim;
	float m_AZBiteLevelY;
	int m_AZBiteLevelAt;
	int m_AZBiteLevelUntil;

	void AZ_BiteLevelStart(PlayerBase victim)
	{
		m_AZBiteLevelVictim = victim;
		m_AZBiteLevelY = victim.GetPosition()[1];
		m_AZBiteLevelAt = GetGame().GetTime() + 500;
		m_AZBiteLevelUntil = GetGame().GetTime() + AZ_VampState.Cfg().drinkMaxSec * 1000 + 4000;
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsServer() || !m_AZBiteLevelVictim)
			return;
		PlayerBase victim = m_AZBiteLevelVictim;
		int now = GetGame().GetTime();
		if (!IsAlive() || IsUnconscious() || now >= m_AZBiteLevelUntil || !victim.m_AZVampHeld || (victim.m_AZVampFeeder != this && victim.m_AZVampFeeder2 != this))
		{
			m_AZBiteLevelVictim = null;
			return;
		}
		if (now < m_AZBiteLevelAt)
			return;
		vector pos = GetPosition();
		if (Math.AbsFloat(pos[1] - m_AZBiteLevelY) > 0.02)
		{
			pos[1] = m_AZBiteLevelY;
			AZ_VampSnap.To(this, pos, GetDirection());
		}
		m_AZBiteLevelAt = now + 500;
	}
}
