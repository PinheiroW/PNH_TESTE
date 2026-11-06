













class AZ_VampFeedAnim
{
	static const int SJ_FEED_ANIM = 146;
	static const int ANIM_START = 1;
	static const int ANIM_LIMP = 2;
	static const int ANIM_HISS = 9;
	static const int ANIM_STOP = 3;


	static const float OWNER_PIN_TOLERANCE = 0.30;
	static const float SERVER_PIN_TOLERANCE = 0.50;
	static const int LIMP_MS = 1500;
	static const int CMD_VICTIM = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET;

	static int s_Have = -1;

	static bool Available()
	{
		if (s_Have < 0)
		{
			s_Have = 0;
			if (GetGame().ConfigIsExisting("CfgPatches AZ_VampireAnims_Feed"))
				s_Have = 1;
		}
		return s_Have == 1;
	}



	static int s_HaveBound = -1;

	static bool BoundAvailable()
	{
		if (s_HaveBound < 0)
		{
			s_HaveBound = 0;
			if (GetGame().ConfigIsExisting("CfgPatches AZ_VampireAnims_Bound"))
				s_HaveBound = 1;
		}
		return s_HaveBound == 1;
	}



	static const int CMD_VAMP = DayZPlayerConstants.CMD_ACTIONFB_CLEANWOUNDSTARGET;

	static void DummyFeedStart(PlayerBase vamp, bool rear = false)
	{
		if (!vamp || !Available())
			return;
		vamp.m_AZVampDummyRear = rear;
		vamp.m_AZVampDummyReq = ANIM_START;
		vamp.m_AZVampDummyTries = 0;
	}

	static void DummyFeedEnd(PlayerBase vamp)
	{
		if (vamp)
			vamp.m_AZVampDummyReq = ANIM_LIMP;
	}

	static void Send(PlayerBase victim, int what)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(what);
		int ms = Math.Max(0, victim.m_AZVampFloorAt - GetGame().GetTime()) + 4000;
		ctx.Write(ms);
		int rear = 0;
		if (victim.m_AZVampRear)
			rear = 1;
		ctx.Write(rear);

		int hasPos = 0;
		if (what == ANIM_START && victim.m_AZVampPinned)
			hasPos = 1;
		ctx.Write(hasPos);
		ctx.Write(victim.m_AZVampPinPos);
		ctx.Write(victim.m_AZVampPinDir);
		if (!victim.GetIdentity())
		{
			victim.m_AZVampAnimReq = what;
			victim.m_AZVampAnimMs = ms;
			victim.m_AZVampAnimRear = rear;
			return;
		}
		victim.SendSyncJuncture(SJ_FEED_ANIM, ctx);
		if (hasPos == 1 && what == ANIM_START)
			Print("[AZ_Vamp] victim clip juncture " + AZ_VampState.Uid(victim) + " snap pos sent, victim now " + Math.Round(vector.Distance(victim.GetPosition(), victim.m_AZVampPinPos) * 100) + " cm off it");
	}
}

class AZ_VampVictimCB : HumanCommandActionCallback
{
	int m_CancelAt;
	bool m_AZDrinker;

	// Victim callbacks must never be mistaken for a continuing vampire clip.
	bool AZ_CanContinueDrink()
	{
		int phase = GetState();
		return m_AZDrinker && GetGame().GetTime() <= m_CancelAt && (phase != STATE_NONE && phase != STATE_LOOP_END && phase != STATE_LOOP_END2);
	}


	void Init(int ms)
	{
		m_CancelAt = GetGame().GetTime() + ms;
		EnableCancelCondition(true);
	}


	bool CancelCondition()
	{
		return GetGame().GetTime() > m_CancelAt;
	}
}

modded class PlayerBase
{
	// Clip visibility only: inventory ownership and empty-hands eligibility stay unchanged.
	bool m_AZBiteHandsHidden;
	int m_AZVampOwnerPinStarted;
	int m_AZVampAnimReq;
	int m_AZVampAnimMs;
	int m_AZVampAnimPhase;
	bool m_AZVampDummyRear;
	int m_AZVampDummyReq;
	int m_AZVampDummyTries;

	override void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		super.OnSyncJuncture(pJunctureID, pCtx);
		if (pJunctureID > 26 && pJunctureID != 200 && pJunctureID != AZ_VampFeedAnim.SJ_FEED_ANIM && pJunctureID != AZ_VampShadow.SJ_SHADOW_BITE)
			Print("[AZ_Vamp] unknown sync juncture " + pJunctureID + " server " + GetGame().IsServer());
		if (pJunctureID != AZ_VampFeedAnim.SJ_FEED_ANIM)
			return;
		int what, ms, rear;
		if (pCtx.Read(what) && pCtx.Read(ms))
		{
			m_AZVampAnimReq = what;
			m_AZVampAnimMs = ms;
			m_AZVampAnimRear = 0;
			if (pCtx.Read(rear))
				m_AZVampAnimRear = rear;
			int hasPos;
			vector sPos, sDir;
			m_AZVampAnimHasPos = false;
			if (pCtx.Read(hasPos) && pCtx.Read(sPos) && pCtx.Read(sDir) && hasPos == 1)
			{
				m_AZVampAnimHasPos = true;
				m_AZVampAnimPos = sPos;
				m_AZVampAnimDir = sDir;
			}
		}
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		AZ_VampVictimCB drinkClip = AZ_VampVictimCB.Cast(GetCommand_Action());
		ActionBase biteAction;
		if (GetActionManager())
			biteAction = GetActionManager().GetRunningAction();
		bool biteHands = IsAlive() && !IsUnconscious() && ((drinkClip && drinkClip.AZ_CanContinueDrink()) || (biteAction && biteAction.IsInherited(ActionAZVampBite)));
		if (biteHands && GetItemInHands())
		{
			if (!m_AZBiteHandsHidden)
				TryHideItemInHands(true);
			m_AZBiteHandsHidden = true;
		}
		else if (m_AZBiteHandsHidden)
		{
			TryHideItemInHands(false);
			m_AZBiteHandsHidden = false;
		}


		if (GetGame().IsServer() && AZ_VampFeedAnim.Available())
			AZ_VampAnimServerTick();

		if (m_AZVampPinned && GetGame().IsServer())
			AZ_VampPinTick();



		if (m_AZVampOwnerPin && !GetGame().IsServer())
		{
			if (!IsAlive() || (!m_AZVampHeld && GetGame().GetTime() - m_AZVampOwnerPinStarted > 1000))
			{
				m_AZVampOwnerPin = false;
			}
			else
			{
				vector ownOff = GetPosition() - m_AZVampAnimPos;
				ownOff[1] = 0;
				if (ownOff.Length() > AZ_VampFeedAnim.OWNER_PIN_TOLERANCE)
				{
					vector ownTo = m_AZVampAnimPos;
					ownTo[1] = GetPosition()[1];
					SetPosition(ownTo);
				}
			}
		}

		if (m_AZVampAnimReq != 0)
			AZ_VampAnimApply(pCurrentCommandID);

		if (m_AZVampDummyReq != 0 && GetGame().IsServer())
			AZ_VampDummyApply(pCurrentCommandID);
	}

	protected void AZ_VampDummyApply(int pCurrentCommandID)
	{
		if (m_AZVampDummyReq == AZ_VampFeedAnim.ANIM_HISS)
		{
			m_AZVampDummyReq = 0;
			HumanCommandActionCallback hcb = AddCommandModifier_Action(DayZPlayerConstants.CMD_GESTUREMOD_TAUNTKISS, AZ_VampVictimCB);
			string hres = "FAILED";
			if (hcb)
				hres = "ok";
			Print("[AZ_Vamp] AI hiss gesture " + hres + " " + GetType());
			return;
		}
		AZ_VampVictimCB cb = AZ_VampVictimCB.Cast(GetCommand_Action());
		if (m_AZVampDummyReq == AZ_VampFeedAnim.ANIM_START)
		{
			if (cb && cb.AZ_CanContinueDrink())
			{
				cb.Init(AZ_VampState.Cfg().drinkMaxSec * 1000 + 4000);
				m_AZVampDummyReq = 0;
				return;
			}


			if (pCurrentCommandID != DayZPlayerConstants.COMMANDID_MOVE || GetCommand_Action() || GetCommandModifier_Action())
			{
				m_AZVampDummyTries++;
				if (m_AZVampDummyTries > 120)
					m_AZVampDummyReq = 0;
				return;
			}
			m_AZVampDummyReq = 0;
			int ms = AZ_VampState.Cfg().drinkMaxSec * 1000 + 4000;
			int cmd = AZ_VampFeedAnim.CMD_VAMP;
			if (m_AZVampDummyRear && AZ_VampRear.Available())
				cmd = AZ_VampRear.CMD_VAMP;
			cb = AZ_VampVictimCB.Cast(StartCommand_Action(cmd, AZ_VampVictimCB, DayZPlayerConstants.STANCEMASK_ERECT));
			if (cb)
			{
				cb.m_AZDrinker = true;
				cb.Init(ms);
				Print("[AZ_Vamp] feedme dummy: feed anim started");
			}
			else
				Print("[AZ_Vamp] feedme dummy: feed anim FAILED to start");
			return;
		}
		if (cb)
		{
			cb.m_AZDrinker = false;
			cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
		}
		m_AZVampDummyReq = 0;
	}


	protected void AZ_VampPinTick()
	{
		if (!m_AZVampHeld || !IsAlive())
		{
			if (m_AZVampPinFixes > 0)
				Print("[AZ_Vamp] feed pin released " + AZ_VampState.Uid(this) + " after " + m_AZVampPinFixes + " fixes");
			m_AZVampPinned = false;
			return;
		}
		vector off = GetPosition() - m_AZVampPinPos;
		off[1] = 0;
		if (off.Length() > AZ_VampFeedAnim.SERVER_PIN_TOLERANCE)
		{
			vector to = m_AZVampPinPos;
			to[1] = GetPosition()[1];
			AZ_VampSnap.AllowMove(this);
			SetPosition(to);
			m_AZVampPinFixes++;
		}
	}

	protected void AZ_VampAnimServerTick()
	{
		if (m_AZVampHeld && m_AZVampAnimPhase == 0 && IsUnconscious())
		{

		}
		else if (m_AZVampHeld)
		{
			int endAt = m_AZVampFloorAt;
			if (m_AZVampAnimPhase == 0)
			{
				m_AZVampAnimPhase = 1;
				AZ_VampFeedAnim.Send(this, AZ_VampFeedAnim.ANIM_START);
			}
			else if (m_AZVampAnimPhase == 1 && GetGame().GetTime() >= endAt - AZ_VampFeedAnim.LIMP_MS)
			{
				m_AZVampAnimPhase = 2;
				AZ_VampFeedAnim.Send(this, AZ_VampFeedAnim.ANIM_LIMP);
			}
		}
		else if (m_AZVampAnimPhase != 0)
		{
			if (m_AZVampAnimPhase == 1)
				AZ_VampFeedAnim.Send(this, AZ_VampFeedAnim.ANIM_STOP);
			m_AZVampAnimPhase = 0;
		}
	}

	protected void AZ_VampAnimApply(int pCurrentCommandID)
	{
		AZ_VampVictimCB cb = AZ_VampVictimCB.Cast(GetCommand_Action());
		if (m_AZVampAnimReq == AZ_VampFeedAnim.ANIM_START)
		{
			if (cb)
			{
				m_AZVampAnimReq = 0;
				return;
			}
			if (!m_AZVampHeld && GetGame().IsServer())
			{
				m_AZVampAnimReq = 0;
				return;
			}

			if (m_AZVampAnimHasPos && !GetGame().IsServer())
			{
				vector to = m_AZVampAnimPos;
				to[1] = GetPosition()[1];
				Print("[AZ_Vamp] victim owner snap " + Math.Round(vector.Distance(GetPosition(), to) * 100) + " cm");
				SetPosition(to);
				SetDirection(m_AZVampAnimDir);
				m_AZVampOwnerPinStarted = GetGame().GetTime();
				m_AZVampOwnerPin = true;
			}
			m_AZVampAnimHasPos = false;
			HumanCommandActionCallback other = GetCommand_Action();
			if (!other)
				other = GetCommandModifier_Action();
			if (other)
			{
				other.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
				return;
			}
			int stance = m_MovementState.m_iStanceIdx;

			bool tied = IsRestrained() && AZ_VampFeedAnim.BoundAvailable();
			// A bound victim can still report ACTION after the callback above was interrupted.
			bool ok = (pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE || (tied && pCurrentCommandID == DayZPlayerConstants.COMMANDID_ACTION)) && (!GetItemInHands() || tied) && !IsUnconscious();
			ok = ok && stance != DayZPlayerConstants.STANCEIDX_PRONE && stance != DayZPlayerConstants.STANCEIDX_RAISEDPRONE;
			m_AZVampAnimReq = 0;
			if (!ok)
			{
				if (GetGame().IsServer())
				{
					string why = "cmd " + pCurrentCommandID.ToString() + " stance " + stance.ToString();
					if (GetItemInHands() && !tied)
						why += " item in hands";
					if (IsUnconscious())
						why += " unconscious";
					Print("[AZ_Vamp] victim anim skipped: " + why);
				}
				return;
			}
			int cmd = AZ_VampFeedAnim.CMD_VICTIM;
			if (m_AZVampAnimRear == 1 && AZ_VampRear.Available())
				cmd = AZ_VampRear.CMD_VICTIM;


			cb = AZ_VampVictimCB.Cast(StartCommand_Action(cmd, AZ_VampVictimCB, DayZPlayerConstants.STANCEMASK_ERECT));
			if (cb)
				cb.Init(m_AZVampAnimMs);
			if (GetGame().IsServer())
			{
				string res = "FAILED";
				if (cb)
					res = "ok";
				Print("[AZ_Vamp] victim anim start " + res + " rear " + m_AZVampAnimRear + " yaw " + GetOrientation()[0].ToString());
			}
			return;
		}
		if (cb)
		{
			if (m_AZVampAnimReq == AZ_VampFeedAnim.ANIM_LIMP)
				cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			else
				cb.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
		m_AZVampAnimReq = 0;
	}
}
