



modded class MissionServer
{
	static int s_AZSlotWait;
	protected ref array<PlayerBase> m_AZSlotDummies;

	override void OnInit()
	{
		super.OnInit();


		if (!AZ_VAMP_HALLOWEEN_TEST || !FileExist("$profile:AZ_Vampire\\slottest.txt"))
			return;
		s_AZSlotWait = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSlotStart, 15000, false);
	}

	void AZSlotStart()
	{
		if (!AZ_VampCrypt.s_Built)
		{
			s_AZSlotWait++;
			if (s_AZSlotWait < 60)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSlotStart, 5000, false);
			return;
		}
		m_AZSlotDummies = new array<PlayerBase>;
		vector at = AZ_VampCrypt.Mouth();
		for (int i = 0; i < 6; i++)
		{
			PlayerBase d = PlayerBase.Cast(GetGame().CreateObjectEx("SurvivorM_Mirek", at + Vector(0.3 * i, 0, 0), ECE_PLACE_ON_SURFACE));
			if (d)
				m_AZSlotDummies.Insert(d);
		}
		foreach (PlayerBase e : m_AZSlotDummies)
			AZ_VampCrypt.EnterFrom(e, 0);
		Print("[AZ_Vamp] slot test: " + m_AZSlotDummies.Count() + " dummies sent down");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSlotReport, 4000, false, "down", true);
	}

	void AZSlotReport(string leg, bool next)
	{
		float closest = 1000;
		int n = m_AZSlotDummies.Count();
		for (int i = 0; i < n; i++)
		{
			vector a = m_AZSlotDummies[i].GetPosition();
			Print("[AZ_Vamp] slot test " + leg + " " + i + " at " + a + " inside " + AZ_VampCrypt.Inside(a));
			for (int j = i + 1; j < n; j++)
				closest = Math.Min(closest, vector.Distance(a, m_AZSlotDummies[j].GetPosition()));
		}
		Print("[AZ_Vamp] slot test " + leg + ": closest pair " + closest + " m");
		if (next)
		{
			foreach (PlayerBase e : m_AZSlotDummies)
			{
				e.m_AZCryptAt = 0;
				AZ_VampCrypt.MoveOut(e);
			}
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZSlotReport, 4000, false, "up", false);
			return;
		}
		foreach (PlayerBase g : m_AZSlotDummies)
			GetGame().ObjectDelete(g);
		Print("[AZ_Vamp] slot test done");

	}



	protected PlayerBase m_AZShIn;
	protected PlayerBase m_AZShOut;

	void AZShortcutTest()
	{
		m_AZShIn = PlayerBase.Cast(GetGame().CreateObjectEx("SurvivorM_Mirek", AZ_VampCrypt.Hall(-14.0, -9.6, 0.1), ECE_KEEPHEIGHT));
		m_AZShOut = PlayerBase.Cast(GetGame().CreateObjectEx("SurvivorM_Boris", AZ_VampCrypt.Hall(-14.0, -5.2, 0.1), ECE_KEEPHEIGHT));
		if (!m_AZShIn || !m_AZShOut)
		{
			Print("[AZ_Vamp] shortcut test: no dummies");
			return;
		}
		Print("[AZ_Vamp] shortcut test: sides in " + AZ_VampCryptGallery.Side(m_AZShIn.GetPosition()) + " out " + AZ_VampCryptGallery.Side(m_AZShOut.GetPosition()) + ", fresh push allowed " + AZ_VampCryptGallery.CanPush(m_AZShOut) + " (want false), lift allowed " + AZ_VampCryptGallery.CanLift(m_AZShIn) + " (want true)");
		AZ_VampCryptGallery.Lift(m_AZShIn);
		Print("[AZ_Vamp] shortcut test: after lift open " + AZ_VampCryptGallery.SlabOpen() + ", lifter unlocked " + AZ_VampCryptGallery.Unlocked(m_AZShIn) + " flag " + m_AZShIn.m_AZShortcut);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZShortcutTest2, 23000, false);
	}

	void AZShortcutTest2()
	{
		Print("[AZ_Vamp] shortcut test: 23 s later open " + AZ_VampCryptGallery.SlabOpen() + " (want false)");
		m_AZShIn.SetPosition(AZ_VampCrypt.Hall(-14.0, -5.2, 0.1));
		m_AZShOut.SetPosition(AZ_VampCrypt.Hall(-16.0, -2.0, 0.1));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZShortcutTest3, 1000, false);
	}

	void AZShortcutTest3()
	{
		Print("[AZ_Vamp] shortcut test: unlocked push allowed " + AZ_VampCryptGallery.CanPush(m_AZShIn) + " (want true)");
		AZ_VampCryptGallery.Push(m_AZShIn);
		Print("[AZ_Vamp] shortcut test: after push open " + AZ_VampCryptGallery.SlabOpen() + ", unlock file count " + AZ_VampCryptGallery.s_Data.uids.Count());
		GetGame().ObjectDelete(m_AZShIn);
		GetGame().ObjectDelete(m_AZShOut);
		Print("[AZ_Vamp] shortcut test done");
	}
}
