


class AZ_VampStanceTest
{
	static const int RPC = 4702530;
	static const int FIRST = 530;
	static const int LAST = 551;
	static PlayerBase s_Dummy;

	static string Stance(PlayerBase p)
	{
		if (!p)
			return "none";
		if (p.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_RAISEDERECT))
			return "erect";
		if (p.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
			return "crouch";
		if (p.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE | DayZPlayerConstants.STANCEMASK_RAISEDPRONE))
			return "prone";
		return "other";
	}

	static string Running(PlayerBase p)
	{
		if (!p || !p.GetActionManager() || !p.GetActionManager().GetRunningAction())
			return "none";
		return p.GetActionManager().GetRunningAction().Type().ToString();
	}

	static void Log(string tag, PlayerBase p)
	{
		Print("[AZ_Vamp] TEST stance " + tag + ": feeder " + Stance(p) + " running " + Running(p) + " cmd " + p.GetCurrentCommandID() + ", victim " + Stance(s_Dummy));
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampStanceTest.RPC || !GetGame().IsClient() || !IsControlledPlayer())
			return;
		Param1<int> m = new Param1<int>(0);
		if (!ctx.Read(m))
			return;
		ActionManagerClient amc = ActionManagerClient.Cast(GetActionManager());
		if (!amc)
			return;
		if (m.param1 == 9)
		{
			amc.EndActionInput();
			amc.SetIgnoreAutomaticInputEnd(false);
			Print("[AZ_Vamp] TEST stance client: end, " + AZ_VampStanceTest.Stance(this));
			return;
		}


		m_AZStTestStance = DayZPlayerConstants.STANCEIDX_ERECT;
		if (m.param1 == 2)
			m_AZStTestStance = DayZPlayerConstants.STANCEIDX_PRONE;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_StanceTestRelease, 2800, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_StanceTestPress, 3000, false, m.param1);
	}

	int m_AZStTestStance = -2;

	void AZ_StanceTestRelease()
	{
		m_AZStTestStance = -1;
	}

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (m_AZStTestStance == -2 || !((GetGame().IsClient() && IsControlledPlayer()) || (GetGame().IsServer() && !GetIdentity())))
			return;
		HumanCommandMove hcm = GetCommand_Move();
		if (!hcm)
			return;
		hcm.ForceStance(m_AZStTestStance);
		if (m_AZStTestStance == -1)
			m_AZStTestStance = -2;
	}


	void AZ_StanceTestPress(int mode)
	{
		ActionManagerClient amc = ActionManagerClient.Cast(GetActionManager());
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(GetPosition(), 3, near, null);
		PlayerBase victim;
		foreach (Object o : near)
		{
			PlayerBase pb = PlayerBase.Cast(o);
			if (pb && pb != this && !pb.m_AZVampire)
			{
				victim = pb;
				break;
			}
		}
		if (!amc || !victim)
		{
			Print("[AZ_Vamp] TEST stance client: no victim near");
			return;
		}
		ActionTarget t = new ActionTarget(victim, null, -1, vector.Zero, -1);
		ActionBase bite = amc.GetAction(ActionAZVampBite);
		ActionBase drain = amc.GetAction(ActionAZVampDrain);
		bool canBite = bite && bite.Can(this, t, null);
		bool canDrain = drain && drain.Can(this, t, null);
		Print("[AZ_Vamp] TEST stance client mode " + mode + ": feeder " + AZ_VampStanceTest.Stance(this) + ", victim " + AZ_VampStanceTest.Stance(victim) + ", can bite " + canBite + " drain " + canDrain);
		ActionBase rear = amc.GetAction(ActionAZVampBiteRear);
		bool canRear = rear && rear.Can(this, t, null);
		Print("[AZ_Vamp] TEST stance client mode " + mode + ": can rear " + canRear);
		ActionBase go = bite;
		bool can = canBite;
		if (mode == 3)
		{
			go = drain;
			can = true;
		}
		if (mode == 4)
		{
			go = rear;
			can = canRear;
		}
		if (!can)
			return;
		amc.SetIgnoreAutomaticInputEnd(true);
		amc.PerformActionStart(go, t, null);
	}
}
