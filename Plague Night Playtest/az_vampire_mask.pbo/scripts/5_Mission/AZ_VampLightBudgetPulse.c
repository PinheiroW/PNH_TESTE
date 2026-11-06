

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		AZ_VampLightBudget.Pulse(timeslice);
	}
}



modded class MissionServer
{
	protected float m_AZBudgetSendTime;
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		m_AZBudgetSendTime += timeslice;
		if (m_AZBudgetSendTime < 5)
			return;
		m_AZBudgetSendTime = 0;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		int cap = Math.Clamp(cfg.decorLightMax, 0, 8);
		if (!cfg.halloween)
			cap = 0;
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach (Man man : players)
		{
			PlayerBase player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
				GetGame().RPCSingleParam(player, AZ_VampLightBudget.RPC, new Param1<int>(cap), true, player.GetIdentity());
		}
	}
}
