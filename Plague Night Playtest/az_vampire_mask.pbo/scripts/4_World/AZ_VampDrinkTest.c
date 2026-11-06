// "/vamp drinktest" (admin): an NPC vampire runs the real drink-them-dry hold on you, so one person can see
// the victim's dim view (AZ-2569) without a second client. Bite from behind, knocked out after GRAB_MS, then
// held unconscious for DOWN_MS and let go. No turning and no blood drain: the test leaves you alive and human.
class AZ_VampDrinkTest
{
	static const int GRAB_MS = 6000;
	static const int DOWN_MS = 20000;
	static const int FADE_MS = 3000;

	static string Start(PlayerBase p)
	{
		if (!p || !p.IsAlive() || p.IsUnconscious())
			return "drinktest: you need to be alive and awake";
		if (p.m_AZVampHeld)
			return "drinktest: something already holds you";
		if (p.m_AZVampire)
			return "drinktest: run it as a human";
		vector dir = p.GetDirection();
		dir[1] = 0;
		dir.Normalize();
		vector back = p.GetPosition() - dir * AZ_VampFeed.ROOT_GAP;
		back[1] = p.GetPosition()[1];
		PlayerBase vamp = AZ_VampCryptLurkers.Dress(back, 0, 0);
		if (!vamp)
			return "drinktest: could not spawn the vampire";
		vamp.SetDirection(dir);
		vamp.SetHealth("", "Blood", 2500);
		ActionAZVampBite.SnapJoin(vamp, p, true);
		AZ_VampFeed.Hold(vamp, p);
		if (!p.m_AZVampHeld)
		{
			GetGame().ObjectDelete(vamp);
			return "drinktest: the hold was refused (sanctuary, daylight or a weak vampire?)";
		}
		p.m_AZVampHeldUntil = GetGame().GetTime() + GRAB_MS + DOWN_MS + 5000;
		AZ_VampFeedAnim.DummyFeedStart(vamp, true);
		Print("[AZ_Vamp] drinktest start " + AZ_VampState.Uid(p));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(KnockOut, GRAB_MS, false, vamp, p);
		return "drinktest: bitten from behind, knocked out in " + (GRAB_MS / 1000) + " s, held down for " + (DOWN_MS / 1000) + " s";
	}

	static bool Still(PlayerBase vamp, PlayerBase p)
	{
		return vamp && vamp.IsAlive() && p && p.IsAlive() && p.m_AZVampHeld && p.m_AZVampFeeder == vamp;
	}

	static void KnockOut(PlayerBase vamp, PlayerBase p)
	{
		if (!Still(vamp, p))
		{
			Print("[AZ_Vamp] drinktest broken before the knockout");
			Finish(vamp, p);
			return;
		}
		// Like AZ_VampFeed.Collapse, minus the turning roll.
		p.m_AZVampCollapsed = true;
		p.SetHealth("", "Shock", 0);
		Print("[AZ_Vamp] drinktest knockout " + AZ_VampState.Uid(p));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Finish, DOWN_MS, false, vamp, p);
	}

	static void Finish(PlayerBase vamp, PlayerBase p)
	{
		if (p && p.m_AZVampHeld && p.m_AZVampFeeder == vamp)
			AZ_VampFeed.Release(vamp, p);
		if (vamp)
		{
			AZ_VampFeedAnim.DummyFeedEnd(vamp);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Gone, FADE_MS, false, vamp);
		}
		bool down = p && p.IsUnconscious();
		Print("[AZ_Vamp] drinktest end " + AZ_VampState.Uid(p) + " unconscious " + down);
	}

	static void Gone(PlayerBase vamp)
	{
		if (vamp)
			GetGame().ObjectDelete(vamp);
	}
}
