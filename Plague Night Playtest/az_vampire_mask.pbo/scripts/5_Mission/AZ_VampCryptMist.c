



modded class MissionServer
{
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if (player && identity)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZMistSend, 6000, false, player);
	}

	void AZMistSend(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;
		float rate = AZ_VampState.Cfg().cryptMist;
		GetGame().RPCSingleParam(player, AZ_VampLightTune.RPC, new Param1<string>("mist:" + rate), true, player.GetIdentity());
	}
}
