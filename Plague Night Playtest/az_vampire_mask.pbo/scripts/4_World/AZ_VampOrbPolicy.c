class AZ_VampOrbPolicy
{

	static int PresentationState(bool night, bool bloodMoon, bool eventActive, bool goalEligible)
	{
		if (night && bloodMoon)
		{
			return 2;
		}
		if (night && eventActive && goalEligible)
		{
			return 1;
		}
		return 0;
	}

	static float DormantScale(float progress)
	{
		if (progress < 0)
		{
			return 0.4;
		}
		if (progress > 1)
		{
			return 2.6;
		}
		return 0.4 + 2.2 * progress;
	}

	static float FullScale(float growth)
	{
		if (growth < 0)
		{
			return 2.6;
		}
		if (growth > 1)
		{
			return 3.2;
		}
		return 2.6 + 0.6 * growth;
	}

	static float DormantRadius(float displayedProgress)
	{
		if (displayedProgress < 0)
		{
			return 3;
		}
		if (displayedProgress > 1)
		{
			return 12;
		}
		return 3 + 9 * displayedProgress;
	}

	static float DormantBrightness(float displayedProgress)
	{
		if (displayedProgress < 0)
		{
			return 0.3;
		}
		if (displayedProgress > 1)
		{
			return 1.6;
		}
		return 0.3 + 1.3 * displayedProgress;
	}

	static float ApproachScale(float current, float target, float step)
	{
		if (current < target)
		{
			if (current + step >= target)
			{
				return target;
			}
			return current + step;
		}
		if (current - step <= target)
		{
			return target;
		}
		return current - step;
	}

	static bool EntryOpen(bool night, bool bloodMoon)
	{
		return night && bloodMoon;
	}

	static bool Visible(bool bloodShell, bool nativeStorm)
	{
		return !bloodShell && !nativeStorm;
	}
}
