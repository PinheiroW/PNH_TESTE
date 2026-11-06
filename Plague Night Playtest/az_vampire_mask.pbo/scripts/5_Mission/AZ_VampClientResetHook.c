

modded class MissionGameplay
{
	override void OnMissionStart()
	{
		AZ_VampClientReset.Run("mission start");
		super.OnMissionStart();
	}

	override void OnMissionFinish()
	{
		AZ_VampClientReset.Run("mission finish");
		super.OnMissionFinish();
	}
}
