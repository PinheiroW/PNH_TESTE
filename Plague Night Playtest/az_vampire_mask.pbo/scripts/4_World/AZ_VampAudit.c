
class AZ_VampAudit
{
	static ref map<string, int> s_Counts = new map<string, int>;
	static void Record(string system, string reason, string details = "")
	{
		if (!GetGame().IsServer() || !GetGame().GetWorld())
			return;
		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);
		string stamp = "[AZ_VampOps] up=" + GetGame().GetTime() / 1000;
		stamp += " game=" + y + "/" + mo + "/" + d + " " + h + ":" + mi;
		stamp += " night=" + AZ_VampProgress.s_NightNo + " system=" + system + " reason=" + reason;
		Print(stamp + " " + details);
	}


	static string MoonClock()
	{
		World w = GetGame().GetWorld();
		int y, mo, d, h, mi;
		w.GetDate(y, mo, d, h, mi);
		return "date=" + y + "/" + mo + "/" + d + " clock=" + h + ":" + mi + " physicalNight=" + w.IsNight();
	}

	static bool s_MoonDayMismatch;
	static int s_MoonDayLastLog;
	static void WatchMoonDay(bool active)
	{
		bool physicalNight = GetGame().GetWorld().IsNight();
		bool mismatch = active && !physicalNight;
		int now = GetGame().GetTime();
		if (mismatch)
		{
			if (!s_MoonDayMismatch || now < s_MoonDayLastLog || now - s_MoonDayLastLog >= 60000)
			{
				string reason = "ACTIVE_PHYSICAL_DAY_REPEAT";
				if (!s_MoonDayMismatch)
					reason = "ACTIVE_PHYSICAL_DAY_ENTER";
				Record("moon", reason, "active=" + active + " " + MoonClock());
				s_MoonDayLastLog = now;
			}
		}
		else if (s_MoonDayMismatch)
			Record("moon", "ACTIVE_PHYSICAL_DAY_RECOVERED", "active=" + active + " " + MoonClock());
		s_MoonDayMismatch = mismatch;
	}

	static void Life(PlayerBase p, string action, string source)
	{
		if (!p || !p.GetIdentity())
			return;
		if (source == "")
			source = "unknown";
		string key = source + ":" + action;
		int count = s_Counts.Get(key) + 1;
		s_Counts.Set(key, count);
		Record("life", action, "source=" + source + " sessionCount=" + count);
	}

	static void Summary(string boundary)
	{
		Record("summary", boundary, "scope=process_cumulative reset=server_boot");
		foreach (string key, int count : s_Counts)
			Record("summary", "COUNT", "sourceOutcome=" + key + " count=" + count);
	}
}
