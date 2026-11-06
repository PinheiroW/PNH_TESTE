

modded class MissionServer
{
	static int s_AZMapWait;

	override void OnInit()
	{
		super.OnInit();
		s_AZMapWait = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZMapStart, 20000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampSurvey.Start, 30000, false);
	}


	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if (player && identity)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZMapSend, 6000, false, player);
	}

	void AZMapSend(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		array<string> lines = AZ_VampOldMap.Lines();
		string all = "";
		foreach (string l : lines)
		{
			if (all != "")
				all += "\n";
			all += l;
		}
		GetGame().RPCSingleParam(player, AZ_VampOldMap.RPC_MARKS, new Param1<string>(all), true, player.GetIdentity());
	}

	void AZMapStart()
	{
		if (!AZ_VampState.Cfg().oldMapEnabled)
		{
			Print("[AZ_Vamp] old map: off (oldMapEnabled false)");
			return;
		}
		if (!AZ_VampCrypt.s_Built)
		{
			s_AZMapWait++;
			if (s_AZMapWait < 60)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZMapStart, 5000, false);
			else
				Print("[AZ_Vamp] old map: no crypt after 5 min, none laid");
			return;
		}
		AZ_VampOldMaps.Plan();
		AZ_VampOldMaps.Restock();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampOldMaps.Tick, 60000, true);
	}
}
