






class AZ_VampSchedule
{
	static int s_NextCheck;


	static int DateKey(string s)
	{
		if (s.Length() != 10)
			return 0;
		int y = s.Substring(0, 4).ToInt();
		int m = s.Substring(5, 2).ToInt();
		int d = s.Substring(8, 2).ToInt();
		if (y < 2000 || m < 1 || m > 12 || d < 1 || d > 31)
			return 0;
		return y * 10000 + m * 100 + d;
	}

	static int Today()
	{
		int y, m, d;
		GetYearMonthDay(y, m, d);
		return y * 10000 + m * 100 + d;
	}

	static void Tick()
	{
		if (!AZ_VampState.s_Data || GetGame().GetTime() < s_NextCheck)
			return;
		s_NextCheck = GetGame().GetTime() + 60000;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		int start = DateKey(cfg.eventStart);
		int end = DateKey(cfg.eventEnd);
		int today = Today();
		bool active = AZ_VampState.s_Data.eventActive;


		bool inWindow = start > 0 && today >= start && (end == 0 || today < end);
		if (!inWindow && AZ_VampState.s_Data.stoppedByAdmin)
		{
			AZ_VampState.s_Data.stoppedByAdmin = false;
			AZ_VampState.Save();
		}
		if (!active && start > 0 && today >= start && (end == 0 || today < end) && !AZ_VampState.s_Data.stoppedByAdmin)
		{
			Print("[AZ_Vamp] schedule: " + cfg.eventStart + " reached, event starts");
			AZ_VampEvent.StartEvent();
		}
		else if (active && end > 0 && today >= end && !GetGame().GetWorld().IsNight())
		{
			Print("[AZ_Vamp] schedule: " + cfg.eventEnd + " reached, event ends");
			End();
		}
	}

	static void End()
	{
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		if (AZ_VampState.Cfg().finale)
		{
			AZ_VampTurn.Announce("The last dawn of Plague Night. " + AZ_VampProgress.Titles(list));
			AZ_VampTurn.Announce("The curse lifts. Namalsk is quiet again.");
		}
		else
			AZ_VampTurn.Announce("The curse has lifted from Namalsk.");
		foreach (PlayerBase p : list)
		{
			if (p.m_AZVampire)
				AZ_VampEvent.Cure(p);
			else if (p.m_AZVampTurning)
				AZ_VampTurn.ClearSick(p);
		}
		foreach (AZ_VampRecord r : AZ_VampState.s_Data.players)
		{
			r.vampire = false;
			r.sickLeftSec = 0;
		}
		AZ_VampState.s_Data.eventActive = false;
		AZ_VampState.s_Data.stoppedByAdmin = false;
		AZ_VampState.s_Data.bloodMoonTonight = false;
		AZ_VampState.Save();
	}
}
