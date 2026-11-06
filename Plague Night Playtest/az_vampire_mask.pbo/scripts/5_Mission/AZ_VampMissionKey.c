


modded class MissionServer
{
	static int s_AZKeyWait;

	override void OnInit()
	{
		super.OnInit();
		s_AZKeyWait = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZKeyStart, 12000, false);
	}

	void AZKeyStart()
	{
		if (!AZ_VampCrypt.s_Built)
		{
			s_AZKeyWait++;
			if (s_AZKeyWait < 60)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZKeyStart, 5000, false);
			else
				Print("[AZ_Vamp] crypt key: no crypt after 5 min, no key laid");
			return;
		}
		AZ_VampCryptKeys.s_WasNight = GetGame().GetWorld().IsNight();
		AZ_VampCryptKeys.Restock();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptKeys.Tick, 60000, true);
		if (FileExist(AZ_VampState.DIR + "/key_probe.txt"))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptKeys.Probe, 20000, false, 0);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptLurkers.ProbeCorpseStart, 21000, false);
		}
	}
}
