






modded class AZ_VampConfig
{
	float portalVorkutaChance = 0.25;
}

class AZ_VampCryptRiverPortal
{
	static const float PORTAL_F = 39.0;
	static const float PORTAL_Y = 1.0;
	static const float TAKE_M = 1.3;
	static const float BANK_R = 2.4;
	static const float PROMPT_M = 2.0;
	static AZ_Crypt_River_Portal s_Portal;
	static AZ_Crypt_River_Portal s_ClientPortal;
	static int s_ClientLookAt;
	static ref map<string, int> s_HurtSaid;
	static bool s_ClientPrompt;

	static vector At()
	{
		return AZ_VampCrypt.Nave(PORTAL_F, 0, PORTAL_Y);
	}

	static vector Bank(float side)
	{
		return AZ_VampCrypt.Nave(PORTAL_F, BANK_R * side, 0);
	}



	static void Roll()
	{
		float chance = 0.25;
		if (AZ_VampState.Cfg())
			chance = AZ_VampState.Cfg().portalVorkutaChance;
		AZ_VampState.s_Data.cryptPortalVorkuta = 0;
		if (Math.RandomFloat01() < chance)
			AZ_VampState.s_Data.cryptPortalVorkuta = 1;
		Print("[AZ_Vamp] river portal: tonight it leads to " + Dest() + " (chance " + chance + ")");
	}

	static string Dest()
	{
		if (AZ_VampState.s_Data && AZ_VampState.s_Data.cryptPortalVorkuta == 1)
			return "vorkuta";
		return "a1";
	}


	static void Scan()
	{
		if (!s_Portal)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Scan, 250, false);
		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);
		foreach (Man m : men)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (!pb || !pb.IsAlive() || !AZ_VampCrypt.CanMove(pb))
				continue;
			if (vector.Distance(pb.GetPosition() + Vector(0, 1.1, 0), At()) > TAKE_M)
				continue;
			TakeIn(pb);
		}
	}

	static void Open(string why)
	{
		if (s_Portal)
			return;
		s_Portal = AZ_Crypt_River_Portal.Cast(AZ_VampCrypt.PutCrypt("AZ_Crypt_River_Portal", At(), AZ_VampCrypt.HALL_YAW));
		Print("[AZ_Vamp] river portal opened (" + why + "): " + (s_Portal != null) + " at hall " + AZ_VampCrypt.HallLocal(At()) + ", leads to " + Dest());
		if (s_Portal)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Scan, 250, false);
		if (!s_Portal || why != "the wake")
			return;

		array<Man> men = new array<Man>;
		GetGame().GetPlayers(men);
		foreach (Man m : men)
		{
			PlayerBase pb = PlayerBase.Cast(m);
			if (pb && pb.IsAlive() && AZ_VampCrypt.Inside(pb.GetPosition()))
				pb.Message("Behind you, the river tears open. A blood storm churns above it: walk into it.", "colorAction");
		}
	}


	static void Restore()
	{
		if (!AZ_VampCryptExit.OpenTonight())
			return;
		Open("restored after a restart, night " + AZ_VampState.s_Data.cryptExitNight);
	}


	static void Shut()
	{
		if (!s_Portal)
			return;
		GetGame().ObjectDelete(s_Portal);
		s_Portal = null;
		Print("[AZ_Vamp] river portal shut at dawn");
	}


	static AZ_Crypt_River_Portal Portal()
	{
		if (GetGame().IsServer())
			return s_Portal;
		if (GetGame().GetTime() >= s_ClientLookAt)
		{
			s_ClientLookAt = GetGame().GetTime() + 2000;
			s_ClientPortal = null;
			array<Object> objs = new array<Object>;
			GetGame().GetObjectsAtPosition(At(), 1.5, objs, null);
			foreach (Object o : objs)
			{
				AZ_Crypt_River_Portal rp = AZ_Crypt_River_Portal.Cast(o);
				if (rp)
					s_ClientPortal = rp;
			}
		}
		return s_ClientPortal;
	}


	static bool AtPortal(PlayerBase p)
	{
		if (!p || !AZ_VampCrypt.Site() || !Portal())
			return false;
		vector pos = p.GetPosition();
		bool near = false;
		for (int s = -1; s <= 1; s += 2)
		{
			float sf = s;
			vector to = Bank(sf) - pos;
			if (Math.AbsFloat(to[1]) > 1.2)
				continue;
			to[1] = 0;
			if (to.Length() <= PROMPT_M)
				near = true;
		}
		if (!near)
			return false;
		vector toP = At() - pos;
		toP[1] = 0;
		vector look = p.GetDirection();
		look[1] = 0;
		bool at = vector.Dot(look.Normalized(), toP.Normalized()) > 0.5;
		if (at && !s_ClientPrompt && GetGame().IsClient())
		{
			s_ClientPrompt = true;
			Print("[AZ_Vamp] river portal client: the Leave prompt is up");
		}
		return at;
	}

	static void Leave(PlayerBase p)
	{
		if (!AtPortal(p))
			return;
		TakeIn(p);
	}

	static void TakeIn(PlayerBase p)
	{
		if (!AZ_VampCrypt.CanMove(p))
			return;
		if (GetGame().GetTime() - p.m_AZLastHitAt < AZ_VampCryptLedge.LEAVE_HURT_MS)
		{

			if (!s_HurtSaid)
				s_HurtSaid = new map<string, int>;
			string uid = AZ_VampState.Uid(p);
			if (!s_HurtSaid.Contains(uid) || GetGame().GetTime() >= s_HurtSaid.Get(uid))
			{
				s_HurtSaid.Set(uid, GetGame().GetTime() + 5000);
				p.Message("Not with blood still running. Wait a moment, then try again.", "colorAction");
				Print("[AZ_Vamp] river portal: " + uid + " leave refused (hurt)");
			}
			return;
		}
		string where = Dest();
		p.m_AZCryptExitGrace = true;
		if (where == "vorkuta")
		{
			AZ_VampCrypt.MoveTo(p, AZ_VampCryptLedge.PickFrom(AZ_VampCryptExit.VorkutaSlots(), "portal to vorkuta"), AZ_VampCryptExit.VorkutaFwd(), "river_vorkuta_exit");
			Print("[AZ_Vamp] river portal: " + AZ_VampState.Uid(p) + " went through the storm to Vorkuta's edge");
			return;
		}
		if (!AZ_VampCryptA1.Ready())
		{
			p.m_AZCryptExitGrace = false;
			AZ_VampCrypt.MoveOut(p);
			Print("[AZ_Vamp] river portal: " + AZ_VampState.Uid(p) + " out (A1 not ready: the usual way)");
			return;
		}
		AZ_VampCrypt.MoveTo(p, AZ_VampCryptLedge.PickFrom(AZ_VampCryptA1.ExitSlots(false), "portal to A1"), AZ_VampCryptA1.Fwd(), "river_a1_exit");
		Print("[AZ_Vamp] river portal: " + AZ_VampState.Uid(p) + " went through the storm to the A1 crack");
	}
}




class AZ_Crypt_River_Portal extends House
{
	static const float CORE = 2.2;
	protected Particle m_AZVortex;
	protected Particle m_AZDrip;
	protected Particle m_AZHeart;
	protected EffectSound m_AZHum;
	protected float m_AZYaw;
	protected int m_AZHintAt;

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSpin, 50, true);
	}

	int AZVortexId()
	{
		return ParticleList.AZ_VAMP_PORTAL_HALL;
	}

	void AZSpin()
	{
		if (!AZ_VampOrbThing.Ready())
			return;
		if (!m_AZVortex)
		{
			m_AZVortex = Particle.PlayInWorld(AZVortexId(), GetPosition());
			Print("[AZ_Vamp] river portal client: the storm plays at " + GetPosition());
		}
		if (!m_AZDrip)
		{

			m_AZDrip = Particle.PlayInWorld(ParticleList.AZ_VAMP_ORB_DRIP, AZ_VampDarkOrb.Under(GetPosition(), CORE));
		}
		if (GetGame().GetTime() >= m_AZHintAt)
		{
			m_AZHintAt = GetGame().GetTime() + 1000;
			PlayerBase nearStorm = PlayerBase.Cast(GetGame().GetPlayer());
			if (nearStorm && nearStorm.IsAlive() && !nearStorm.IsUnconscious() && vector.Distance(nearStorm.GetPosition(), GetPosition()) < 8)
			{
				m_AZHintAt = GetGame().GetTime() + 60000;
				nearStorm.Message("A blood storm hangs above the river, drawing you toward the bank. Walk into it to leave.", "colorAction");
			}
		}
		m_AZYaw = m_AZYaw + 0.6;
		SetOrientation(Vector(m_AZYaw, 12, 0));
		SetScale(CORE);
		if (!m_AZHum)
			m_AZHum = AZ_VampDarkOrb.Hum(this);
		if (!m_AZHeart)
			m_AZHeart = AZ_VampDarkOrb.Heart(GetPosition(), CORE);
		AZ_VampDarkOrb.Keep(m_AZHeart, GetPosition(), CORE);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZSpin);
		if (m_AZVortex)
			m_AZVortex.Stop();
		if (m_AZDrip)
			m_AZDrip.Stop();
		if (m_AZHeart)
			m_AZHeart.Stop();
		AZ_VampDarkOrb.HumStop(m_AZHum);
	}
}

class ActionAZCryptPortalLeaveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}


class ActionAZCryptPortalLeave : ActionContinuousBase
{
	void ActionAZCryptPortalLeave()
	{
		m_CallbackClass = ActionAZCryptPortalLeaveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "Give yourself to the storm";
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return AZ_VampCryptRiverPortal.AtPortal(player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		AZ_VampCryptRiverPortal.Leave(action_data.m_Player);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionAZCryptPortalLeave);
	}
}

modded class PlayerBase
{
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionAZCryptPortalLeave, InputActionMap);
	}
}
