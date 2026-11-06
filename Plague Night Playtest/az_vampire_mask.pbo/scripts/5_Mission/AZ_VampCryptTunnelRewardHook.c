
modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		AZ_VampCryptTunnelReward.Reset();
		AZ_VampCryptTunnel.s_OnCleared.Insert(AZ_VampCryptTunnelReward.Claim);
	}

	override void OnMissionFinish()
	{
		AZ_VampCryptTunnel.s_OnCleared.Remove(AZ_VampCryptTunnelReward.Claim);
		AZ_VampCryptTunnelReward.Shutdown();
		super.OnMissionFinish();
	}
}
