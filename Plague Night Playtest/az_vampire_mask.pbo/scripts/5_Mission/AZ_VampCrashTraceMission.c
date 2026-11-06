modded class MissionGameplay
{
	protected bool m_AZCrashFirstFrame;
	protected bool m_AZLifeFirstFrame;

	override void OnInit()
	{
		AZ_VampCrashTrace.BeginMission();
		AZ_VampClientLife.Mark("mission_init_enter");
		AZ_VampCrashTrace.Mark("MissionGameplay OnInit enter");
		super.OnInit();
		AZ_VampCrashTrace.Mark("MissionGameplay OnInit exit");
		AZ_VampClientLife.Mark("mission_init_exit");
	}

	override void OnMissionStart()
	{
		AZ_VampCrashTrace.Mark("MissionGameplay OnMissionStart enter");
		AZ_VampClientLife.Mark("mission_start_enter");
		super.OnMissionStart();
		AZ_VampCrashTrace.Mark("MissionGameplay OnMissionStart exit");
		AZ_VampClientLife.Mark("mission_start_exit");
	}

	override void OnMissionFinish()
	{
		AZ_VampCrashTrace.Mark("MissionGameplay OnMissionFinish enter");
		AZ_VampClientLife.Mark("mission_finish_enter", "scope=mission_only", true);
		super.OnMissionFinish();
		AZ_VampCrashTrace.Mark("MissionGameplay OnMissionFinish exit");
		AZ_VampClientLife.Mark("mission_finish_exit", "scope=mission_only", true);
	}

	override void OnUpdate(float timeslice)
	{
		if (!m_AZLifeFirstFrame)
		{
			m_AZLifeFirstFrame = true;
			AZ_VampClientLife.Mark("mission_first_update");
		}
		if (!m_AZCrashFirstFrame && AZ_VampCrashTrace.Enabled())
		{
			m_AZCrashFirstFrame = true;
			AZ_VampCrashTrace.Mark("MissionGameplay first OnUpdate enter");
			super.OnUpdate(timeslice);
			AZ_VampCrashTrace.Mark("MissionGameplay first OnUpdate exit");
			return;
		}
		super.OnUpdate(timeslice);
	}
}
