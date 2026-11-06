
class AZ_VampSeedPolicy
{
	static int Target(int configured, int cap)
	{
		if (configured < 0 || cap < 0)
		{
			return 0;
		}
		if (configured > cap)
		{
			return cap;
		}
		return configured;
	}

	static int Reserved(int vampires, int turning, int rituals, int active)
	{
		return vampires + turning + rituals + active;
	}

	static bool Due(bool night, int hour)
	{
		return night && hour < 12;
	}

	static string Reason(bool enabled, bool night, int humans, bool attempt, bool ritual, int reserved, int target, int cap, bool due, bool cooldown)
	{
		if (!enabled) { return "INACTIVE"; }
		if (!night) { return "DAY"; }
		if (humans < 3) { return "POPULATION"; }
		if (attempt) { return "ATTEMPT_ACTIVE"; }
		if (ritual) { return "RITUAL_ACTIVE"; }
		if (reserved >= cap) { return "CAP"; }
		if (reserved >= target) { return "TARGET_MET"; }
		if (!due) { return "BEFORE_MIDNIGHT"; }
		if (cooldown) { return "COOLDOWN"; }
		return "READY";
	}

	static string Outcome(bool completed, bool infected)
	{
		if (!completed) { return "INTERRUPTED"; }
		if (!infected) { return "BITE_NO_INFECTION"; }
		return "INFECTED";
	}

	static bool Changed(string previous, string current)
	{
		return previous != current;
	}

	static string MoonReason(int need, int score, int rest, int gap)
	{
		if (need <= 0) { return "DISABLED"; }
		if (score < need) { return "SCORE_LOW"; }
		if (rest < gap) { return "GAP"; }
		return "EARNED";
	}
}
