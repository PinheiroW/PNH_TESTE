






modded class MissionServer
{
	string HwCryptLabel(int step)
	{
		if (step == 89)
			return "crypt_reveal";
		if (step == 90)
			return "crypt_nave";
		if (step == 91)
			return "crypt_altar";
		if (step == 92)
			return "a1_crack|a1_orb|a1_flood|a1_crack_bm";
		if (step == 93 || step == 94)
			return A1Labels()[step - 93];
		if (step == 95)
			return "a1_flood|a1_crack_bm";
		if (step == 96 || step == 97)
			return A1Labels()[step - 94];
		if (step == 150)
			return "crypt_ante";
		if (step == 151)
			return "crypt_ante_room";
		if (step == 152)
			return "crypt_deep";
		if (step == 153)
			return "a3_exit";
		if (step == 154)
			return "crypt_blood";
		if (step == 155)
			return "crypt_seal_l";
		if (step == 156)
			return "crypt_seal_r";
		if (step == 157)
			return "crypt_seal_v";
		if (step == 310)
			return "side_gallery";
		if (step == 311)
			return "side_gallery_exit";
		if (step == 312)
			return "side_shortcut";
		if (step == 313)
			return "side_tomb";
		if (step == 314)
			return "side_chapel";
		if (step == 315)
			return "side_shortcut_in";
		if (step == 317)
			return "fresco_moon";
		if (step == 318)
			return "fresco_court";
		if (step == 322)
			return "bm_altar";
		if (step == 323)
			return "bm_sky";
		if (step == 321)
			return "countess_eyes";
		if (step == 320)
			return "countess_guide";
		if (step == 319)
			return "lore_probe";
		if (step == 324)
			return "river_close|river_wide|river_low|river_clip|brazier_stair";
		if (step == 325)
			return "ritual_sockets|ritual_cut|ritual_drip";
		if (step == 326)
			return "look_h_arrival_w|look_v_arrival_w|look_statue_hold_00|look_moon_off|look_leave_gate|look_entry_05|ledge_spawn|ledge_leave|seal_breach|look_h_altar_w|look_h_altar_c|look_v_altar_w|look_h_nave_w|look_h_stairs_w|look_h_ossuary_w|look_h_chapel_door|diag_pre|apse_his_A|apse_his_L|apse_his_M|apse_his_N";
		if (step == 327)
			return "finale_slab|finale_offer|protect_bite|kneel_mark|loot_a0|loot_a1|loot_a2|loot_a3|loot_a4|loot_a5|invite_swish|invite_kneel|elder_face|offer_onlooker|offer_onlooker_2s|finale_wait|river_feed_alone|river_feed_dais|river_feed_dais_top|river_stare|stare_embers|river_feed_banks|river_feed_swish|river_feed_taste|river_feed_side|river_feed_wide|river_feed_down|river_feed_wake|wake_kneel|river_feed_wake_altar|river_feed_withdraw|after_howto|after_bare|after_feed|after_turned|after_letgo|after_river_stand|after_steps|after_stairs_walk|after_gate|exit_open|exit_door|exit_landed|portal_front_night|gallery_arrival|gallery_from_hall|leave_tomb_prompt|elder_wrath";
		if (step == 316)
			return "band";
		return "";
	}



	int RiverRun(PlayerBase p)
	{
		MoonVariant(p, "off");
		p.SetPosition(AZ_VampCrypt.Nave(12.0, -5.4, 0.1));
		float w = -AZ_VampCrypt.NAVE_DEPTH + AZ_VampCrypt.s_Blood;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 6000, false, p, "river_close", AZ_VampCrypt.Nave(24.0, -2.9, 0.9), AZ_VampCrypt.Nave(27.0, 0.3, w));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 12000, false, p, "river_wide", AZ_VampCrypt.Nave(13.0, -3.0, 1.6), AZ_VampCrypt.Nave(42.0, 0, w + 0.6));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 18000, false, p, "river_low", AZ_VampCrypt.Nave(30.0, -2.6, w + 0.45), AZ_VampCrypt.Nave(44.0, 0.4, w));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 24000, false, p, "river_clip", AZ_VampCrypt.Nave(25.0, -2.9, 1.1), AZ_VampCrypt.Nave(31.0, 0.2, w));

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 32000, false, p, "river_swap", AZ_VampCrypt.Nave(13.0, -3.0, 1.6), AZ_VampCrypt.Nave(42.0, 0, w + 0.6));

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 40000, false, p, "brazier_stair", AZ_VampCrypt.Nave(5.6, 3.2, 1.6), AZ_VampCrypt.Nave(1.4, 5.6, 1.3));
		return 48000;
	}








	static int s_FeedStart;
	static int s_FeedRiverSeen;



	static const int LOOT_SHOT_MS = 6000;



	static ref array<PlayerBase> s_Burst;

	void BurstTest()
	{
		s_Burst = new array<PlayerBase>;
		vector park = AZ_VampCrypt.Nave(20.0, 0, 0);
		for (int i = 0; i < 10; i++)
		{
			PlayerBase d = PlayerBase.Cast(GetGame().CreateObject(AZ_VampDress.Any(40 + i), park + Vector(i * 1.5, 0, 0), false, true));
			if (!d)
				continue;
			s_Burst.Insert(d);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampCryptLedge.Arrive, i * 200, false, d, 0);
		}
		Print("[AZ_Vamp] TEST burst: " + s_Burst.Count() + " arrivals within 2 s");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BurstCheck, 11000, false);
	}

	void BurstCheck()
	{
		float worst = 1000;
		for (int i = 0; i < s_Burst.Count(); i++)
		{
			PlayerBase a = s_Burst[i];
			if (!a)
				continue;
			if (AZ_VampCrypt.HallLocal(a.GetPosition())[0] > 8)
			{
				Print("[AZ_Vamp] TEST burst " + i + ": still waiting");
				continue;
			}
			float nearest = 1000;
			for (int j = 0; j < s_Burst.Count(); j++)
			{
				PlayerBase b = s_Burst[j];
				if (!b || j == i)
					continue;
				float dd = vector.Distance(a.GetPosition(), b.GetPosition());
				if (dd < nearest)
					nearest = dd;
			}
			if (nearest < worst)
				worst = nearest;
			Print("[AZ_Vamp] TEST burst " + i + ": hall " + AZ_VampCrypt.HallLocal(a.GetPosition()) + ", nearest other " + nearest + " m");
		}
		string verdict = "PASS";
		if (worst < 1.0)
			verdict = "FAIL";
		Print("[AZ_Vamp] TEST burst: closest pair " + worst + " m, " + verdict);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BurstClear, 40000, false);
	}

	void BurstClear()
	{
		if (!s_Burst)
			return;
		foreach (PlayerBase g : s_Burst)
		{
			if (g)
				GetGame().ObjectDelete(g);
		}
		s_Burst = null;
	}

	void FeedRun(PlayerBase p)
	{
		GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);


		p.SetPosition(AZ_VampCryptLedge.Floor(8.0, 3.5) + Vector(0, 0.05, 0));
		BurstTest();
		AZ_VampLootPlace.Audit();
		int n = AZ_VampLootPlace.s_AuditShow.Count();
		if (n > 6)
			n = 6;
		for (int i = 0; i < n; i++)
		{
			EntityAI it = AZ_VampLootPlace.s_AuditShow[i];
			if (!it)
				continue;
			vector c = it.GetPosition();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (i + 1) * LOOT_SHOT_MS, false, p, "loot_a" + i, c + Vector(0.9, 0.7, 0.9), c);
			Print("[AZ_Vamp] TEST loot frame loot_a" + i + ": " + it.GetType() + " at " + c);
		}
		AZ_VampRitualSite ks = AZ_VampRitual.CryptSite();
		if (ks && ks.m_Kneel.Count() > 0)
		{
			vector spot = ks.m_Kneel[0];
			vector away = spot - ks.m_Center;
			away[1] = 0;
			away.Normalize();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000, false, p, "kneel_mark", spot + away * 2.0 + Vector(0, 1.6, 0), spot);
		}

		vector wellAt = AZ_VampCrypt.Nave(0.4, 0, -0.9);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 1) * LOOT_SHOT_MS, false, p, "ledge_overflow", AZ_VampCrypt.Nave(1.0, -4.6, 1.7), wellAt);


		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 2) * LOOT_SHOT_MS, false, p, "gibbet_a", AZ_VampCrypt.Nave(16.3, -2.8, 1.7), AZ_VampCrypt.Nave(18.0, -6.6, 4.6));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 3) * LOOT_SHOT_MS, false, p, "gibbet_ledge", AZ_VampCrypt.Nave(2.2, 1.0, 1.7), AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 2.5));

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 4) * LOOT_SHOT_MS, false, p, "lt_camp_f13", AZ_VampCrypt.Hall(13.0, -3.0, 1.65), AZ_VampCrypt.Hall(2.0, -3.0, 2.5));


		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 5) * LOOT_SHOT_MS, false, p, "basin_stair", AZ_VampCrypt.Nave(50.2, 0.9, 0.6), AZ_VampCrypt.Nave(53.6, 0, 0.0));

		Print("[AZ_Vamp] TEST portal before the wake: " + (AZ_VampCryptRiverPortal.s_Portal != null) + " (true is a FAIL)");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000 + (n + 6) * LOOT_SHOT_MS, false, p, "river_portal_closed", AZ_VampCrypt.Nave(2.2, 0, 1.65), AZ_VampCryptRiverPortal.At());
		int wait = 2000 + (n + 7) * LOOT_SHOT_MS;
		if (wait < 13000)
			wait = 13000;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedRunMain, wait, false, p);
	}

	void FeedRunMain(PlayerBase p)
	{
		MoonVariant(p, "off");
		AZ_VampState.s_Data.eventActive = true;
		GetGame().GetWorld().SetDate(2011, 11, 5, 22, 0);
		if (p.m_AZVampire)
			AZ_VampEvent.Cure(p);
		s_HwGuard = false;
		AZ_VampRecord rec = AZ_VampState.Find(AZ_VampState.Uid(p));
		if (rec)
		{
			rec.howToShown = false;
			rec.shadowHintShown = false;
		}
		AZ_VampRitual.Friends(null, "clear");
		string slab = AZ_VampRitual.Debug(p, "slab");
		string friends = AZ_VampRitual.Friends(p, "3");
		Print("[AZ_Vamp] TEST finale probe: " + slab + " | " + friends);
		s_FeedStart = GetGame().GetTime();
		s_FeedRiverSeen = 0;
		vector slabCam = AZ_VampCrypt.Nave(53.0, 2.6, 1.6);
		vector slabAt = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 0.9);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2500, false, p, "finale_slab", slabCam, slabAt);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedOffer, 4000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OfferWatch, 4500, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProtectTest, 24000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(InviteTest, 18000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MatchTest, 2000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 9000, false, p, "finale_offer", AZ_VampCrypt.Nave(55.0, 1.8, 1.7), slabAt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 30000, false, p, "finale_wait", slabCam, slabAt);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(FeedWatch, 1000, false, p);
	}


	void OfferWatch(PlayerBase p)
	{
		if (AZ_VampRitual.s_OfferEndAt > s_FeedStart)
		{
			vector cam = AZ_VampCrypt.Nave(55.0, 1.8, 1.7);
			vector at = AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 0.9);
			HwShot(p, "offer_onlooker", cam, at);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2000, false, p, "offer_onlooker_2s", cam, at);
			Print("[AZ_Vamp] TEST finale probe: the offering's collapse at +" + (AZ_VampRitual.s_OfferEndAt - s_FeedStart) + " ms");
			return;
		}
		if (GetGame().GetTime() - s_FeedStart < 60000)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OfferWatch, 200, false, p);
	}



	void ProtectTest(PlayerBase p)
	{
		PlayerBase fr = null;
		if (AZ_VampRitual.s_Friends && AZ_VampRitual.s_Friends.Count() > 2)
			fr = AZ_VampRitual.s_Friends[2];
		Print("[AZ_Vamp] TEST protect: state " + AZ_VampRitual.s_State + ", protecting the one lying " + AZ_VampRitual.Protecting(p) + ", friend " + (fr != null));
		if (!fr)
			return;
		AZ_VampRitual.Protect(p, fr, "probe: a friend struck the one on the altar");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "protect_bite", fr.GetPosition() + Vector(2.0, 1.6, 1.0), fr.GetPosition() + Vector(0, 1.0, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ProtectAfter, 7000, false, p, fr);
	}




	void InviteTest(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST invite: state " + AZ_VampRitual.s_State + ", " + AZ_VampRitual.s_Kneelers.Count() + " kneeling");
		PlayerBase e = AZ_VampInvite.Invite(p);
		vector eye = p.GetPosition() + Vector(0, 1.7, 0);
		if (e)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 3000, false, p, "elder_face", e.GetPosition() + Vector(0, 1.62, 0) + e.GetDirection() * 0.7, e.GetPosition() + Vector(0, 1.6, 0));
		if (e)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1200, false, p, "invite_swish", e.GetPosition() + e.GetDirection() * 2.6 + Vector(0, 1.7, 0), e.GetPosition() + Vector(0, 1.2, 0));
		PlayerBase fr = null;
		if (AZ_VampRitual.s_Friends && AZ_VampRitual.s_Friends.Count() > 1)
			fr = AZ_VampRitual.s_Friends[1];
		if (!fr)
			return;
		AZ_VampRitual.Unkneel(fr, "probe: the invited friend");
		vector back = AZ_VampCrypt.Nave(-6.0, 0, 0) - AZ_VampCrypt.Nave(0, 0, 0);
		fr.SetPosition(fr.GetPosition() + back);
		AZ_VampInvite.Mark(fr);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(InviteKneel, 2000, false, p, fr);
	}


	void MatchTest(PlayerBase p)
	{
		ItemBase box = ItemBase.Cast(AZ_VampRitual.s_Tools[1]);
		if (!box)
		{
			Print("[AZ_Vamp] TEST matches: no matchbox on the slab");
			return;
		}
		box.SetQuantity(1);
		Print("[AZ_Vamp] TEST matches: emptied to " + box.GetQuantity());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MatchAfter, 4000, false, box);
	}

	void MatchAfter(ItemBase box)
	{
		if (box)
			Print("[AZ_Vamp] TEST matches after: " + box.GetQuantity() + " of " + box.GetQuantityMax() + " (PASS = full)");
	}

	void InviteKneel(PlayerBase p, PlayerBase fr)
	{
		if (!p || !fr)
			return;
		vector was = AZ_VampCrypt.HallLocal(fr.GetPosition());
		bool ok = AZ_VampRitual.Kneel(fr);
		Print("[AZ_Vamp] TEST invite kneel: from hall " + was + " ok " + ok + ", now hall " + AZ_VampCrypt.HallLocal(fr.GetPosition()) + ", " + AZ_VampRitual.s_Kneelers.Count() + " kneeling (PASS = ok and at the circle)");
		HwShot(p, "invite_kneel", AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F - 4.0, 2.5, 1.8), AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 0.6));
	}

	void ProtectAfter(PlayerBase p, PlayerBase fr)
	{
		if (!fr)
			return;
		Print("[AZ_Vamp] TEST protect after: ritual state " + AZ_VampRitual.s_State + " (still 4 or 5 = PASS), the friend unconscious " + fr.IsUnconscious() + " blood " + fr.GetHealth("", "Blood"));
	}

	void FeedOffer(PlayerBase p)
	{
		bool ok = AZ_VampRitual.Lie(p);
		Print("[AZ_Vamp] TEST finale probe: the offering at +" + (GetGame().GetTime() - s_FeedStart) + " ms, lie " + ok + ", knife " + p.GetItemInHands());
	}


	void FeedWatch(PlayerBase p)
	{
		int now = GetGame().GetTime();
		if (AZ_VampCryptLurkers.s_RiverVictim == p && AZ_VampCryptLurkers.s_RiverStartAt > s_FeedStart)
		{
			int late = now - AZ_VampCryptLurkers.s_RiverStartAt;
			Print("[AZ_Vamp] TEST finale probe: the river feed began " + (AZ_VampCryptLurkers.s_RiverStartAt - s_FeedStart) + " ms after the start");
			FeedViews(p, -late);
			return;
		}
		if (now - s_FeedStart > 240000)
		{
			Print("[AZ_Vamp] TEST finale probe FAIL: no river feed within 240 s (ritual state " + AZ_VampRitual.s_State + ")");
			FeedRunEnd(p);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(FeedWatch, 500, false, p);
	}


	void FeedViews(PlayerBase p, int d)
	{
		vector spot = AZ_VampCryptLurkers.RiverSpot();
		float f = AZ_VampCryptLurkers.RIVER_SPOT_F;
		int st = AZ_VampCryptLurkers.StareMs();
		int bite = AZ_VampCryptLurkers.RIVER_PLAY_MS + 400 + d + st;
		vector side = AZ_VampCrypt.Nave(f, 2.3, 0.75);
		vector head = AZ_VampCrypt.Nave(f, 0, 0.5);
		vector wideCam = AZ_VampCrypt.Nave(f - 3.5, 1.8, 1.4);
		vector wideAt = AZ_VampCrypt.Nave(f + 3.0, 0, 0.3);
		ScriptCallQueue sq = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);
		ScriptCallQueue gq = GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		sq.CallLater(HwShot, 2500 + d, false, p, "river_feed_alone", wideCam, wideAt);
		sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + 3000 + d, false, p, "river_feed_banks", wideCam, wideAt);
		gq.CallLater(AZ_VampCryptLurkers.RiverCensus, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + 5500 + d, false);
		sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + 5500 + d, false, p, "river_feed_dais", AZ_VampCrypt.Nave(f + 1.5, 1.2, 1.0), AZ_VampCrypt.Nave(59.5, 0, 1.6));

		sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + 8500 + d, false, p, "river_feed_dais_top", AZ_VampCrypt.Nave(58.4, 0, 7.0), AZ_VampCrypt.Nave(58.9, 0, 0.6));


		vector eye = spot + Vector(0, 1.6, 0);
		vector gaze = AZ_VampCryptLurkers.RiverFace();
		if (st > 0)
			sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + AZ_VampCryptLurkers.STARE_AFTER_ARRIVE_MS + 4000 - 3000 + d, false, p, "river_stare", eye - gaze * 0.3, AZ_VampCryptLurkers.StatueFace());

		if (st > 0)
			sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_ARRIVE_MS + AZ_VampCryptLurkers.STARE_AFTER_ARRIVE_MS + 5000 - 3000 + d, false, p, "stare_embers", AZ_VampCrypt.Nave(f - 3.0, 2.0, 1.6), AZ_VampCrypt.Nave(f + 8.0, 0, 1.5));
		sq.CallLater(HwShot, AZ_VampCryptLurkers.RIVER_B_TASTE - 650 + 175 - 3000 + d + st, false, p, "river_feed_swish", AZ_VampCrypt.Nave(f + 2.0, 2.2, 1.6), AZ_VampCrypt.Nave(f + 4.5, 0, 1.0));
		int taste = AZ_VampCryptLurkers.RIVER_A_TASTE + 1500 + d + st;
		gq.CallLater(AZ_VampCryptLurkers.RiverMeasure, taste, false, "taste");
		sq.CallLater(HwShot, taste, false, p, "river_feed_taste", side, head);
		gq.CallLater(AZ_VampCryptLurkers.RiverMeasure, bite + 2500, false, "full");
		sq.CallLater(HwShot, bite + 2500, false, p, "river_feed_side", side, head);
		sq.CallLater(HwShot, bite + 5000, false, p, "river_feed_wide", wideCam, AZ_VampCrypt.Nave(f + 0.5, 0, 0.3));
		sq.CallLater(HwShot, bite + 7500, false, p, "river_feed_down", AZ_VampCrypt.Nave(f, 2.0, 1.8), AZ_VampCrypt.Nave(f, 0, 0.2));
		gq.CallLater(AZ_VampCryptLurkers.RiverMeasure, bite + AZ_VampCryptLurkers.RIVER_DRAIN_MS + 1500, false, "end");
		int wake = bite + AZ_VampCryptLurkers.RIVER_DRAIN_MS + AZ_VampCryptLurkers.RIVER_WAKE_MS + 4000;
		sq.CallLater(HwShot, wake, false, p, "river_feed_wake", wideCam, wideAt);

		int kneelAt = bite + AZ_VampCryptLurkers.RIVER_DRAIN_MS + AZ_VampCryptLurkers.RIVER_WAKE_MS + 2000 - 3000;
		sq.CallLater(HwShot, kneelAt, false, p, "wake_kneel", AZ_VampCrypt.Nave(f + 1.0, 1.6, 1.5), AZ_VampCrypt.Nave(59.0, 0, 1.6));

		sq.CallLater(HwShot, wake + 3000, false, p, "river_feed_wake_altar", AZ_VampCrypt.Nave(f + 1.5, -1.2, 1.2), AZ_VampCrypt.Nave(AZ_VampCrypt.ALTAR_F, 0, 0.9));

		gq.CallLater(ExitCheck, wake + 1500, false, "wake");
		sq.CallLater(HwShot, wake + 5500, false, p, "exit_open", AZ_VampCrypt.Nave(56.8, AZ_VampCryptExit.EXIT_R + 1.6, 1.9), AZ_VampCrypt.Nave(AZ_VampCryptExit.EXIT_F, AZ_VampCryptExit.EXIT_R + 0.4, 1.7));
		sq.CallLater(HwShot, wake + AZ_VampAltar.HOWTO_MS - 2000, false, p, "after_howto", AZ_VampCrypt.Nave(f - 1.5, 1.0, 1.0), AZ_VampCrypt.Nave(f + 2.0, 0, 0.8));
		int leave = wake + AZ_VampCryptLurkers.RIVER_STAY_S * 1000 + 2000;
		sq.CallLater(HwShot, leave, false, p, "river_feed_withdraw", wideCam, wideAt);
		gq.CallLater(FeedAfter, leave + 12000, false, p);
	}





	static PlayerBase s_AfterFriend;
	static int s_AfterAt;

	PlayerBase AfterFriend(int i)
	{
		if (!AZ_VampRitual.s_Friends || i >= AZ_VampRitual.s_Friends.Count())
			return null;
		return AZ_VampRitual.s_Friends[i];
	}

	void FeedAfter(PlayerBase p)
	{
		s_AfterAt = GetGame().GetTime();
		AZ_VampRecord rec = AZ_VampState.Find(AZ_VampState.Uid(p));
		bool shown = rec && rec.howToShown;
		Print("[AZ_Vamp] TEST after: vampire " + p.m_AZVampire + " unconscious " + p.IsUnconscious() + " how-to shown " + shown);
		PlayerBase fr = AfterFriend(0);
		if (!fr)
		{
			Print("[AZ_Vamp] TEST after FAIL: no friend left to feed on");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterShadow, 500, false, p);
			return;
		}
		vector fd = fr.GetDirection();
		fd[1] = 0;
		fd.Normalize();
		vector stand = fr.GetPosition() - fd * 1.2;
		p.SetPosition(stand);
		p.SetDirection(fd);
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_FANGS_TEST, new Param1<bool>(true), true, p.GetIdentity());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "after_bare", fr.GetPosition() + fd * 1.6 + Vector(0.8, 1.6, 0), fr.GetPosition() + Vector(0, 1.3, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterFeed, 3500, false, p, 0);
	}


	void AfterFeed(PlayerBase p, int i)
	{
		PlayerBase fr = AfterFriend(i);
		if (!fr || !p)
		{
			AfterShadow(p);
			return;
		}
		s_AfterFriend = fr;
		AZ_VampCryptLurkers.RiverSnap(p, fr, true);
		AZ_VampFeed.Hold(p, fr);
		Print("[AZ_Vamp] TEST after: feeding on friend " + i + " " + AZ_VampState.Uid(fr) + " blood " + fr.GetHealth("", "Blood"));
		if (i == 0)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2500, false, p, "after_feed", fr.GetPosition() + fr.GetDirection() * 1.8 + Vector(1.0, 1.5, 0), fr.GetPosition() + Vector(0, 1.3, 0));
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterWatch, 1000, false, p, 0);
		}
		else
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterLetGo, 3000, false, p, fr);
	}

	void AfterWatch(PlayerBase p, int sec)
	{
		PlayerBase fr = s_AfterFriend;
		if (fr && (fr.m_AZVampCollapsed || !fr.m_AZVampHeld || sec >= 45))
		{
			Print("[AZ_Vamp] TEST after: friend 0 after " + sec + " s: collapsed " + fr.m_AZVampCollapsed + " turning " + fr.m_AZVampTurning + " blood " + fr.GetHealth("", "Blood") + " alive " + fr.IsAlive());
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 500, false, p, "after_turned", fr.GetPosition() + Vector(1.6, 1.4, 1.0), fr.GetPosition() + Vector(0, 0.4, 0));
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterFeed, 5000, false, p, 1);
			return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterWatch, 1000, false, p, sec + 1);
	}

	void AfterLetGo(PlayerBase p, PlayerBase fr)
	{
		AZ_VampFeed.Release(p, fr);
		Print("[AZ_Vamp] TEST after: let friend 1 go after 3 s: turning " + fr.m_AZVampTurning + " collapsed " + fr.m_AZVampCollapsed + " blood " + fr.GetHealth("", "Blood"));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000, false, p, "after_letgo", fr.GetPosition() + fr.GetDirection() * 2.0 + Vector(0, 1.6, 0), fr.GetPosition() + Vector(0, 1.3, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterShadow, 4000, false, p);
	}

	void AfterShadow(PlayerBase p)
	{
		if (AZ_VampProgress.s_BloodMoon)
			AZ_VampAltar.ShadowHint(p);
		else
			Print("[AZ_Vamp] TEST after: the shadow-step hint skipped (no blood moon tonight)");

		p.SetPosition(AZ_VampCryptLurkers.RiverSpot());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000, false, p, "after_river_stand", AZ_VampCrypt.Nave(46.0, 1.5, 1.2), AZ_VampCryptLurkers.RiverSpot() + Vector(0, 1.0, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterPush, 5000, false, p);
	}

	void AfterPush(PlayerBase p)
	{
		float drift = vector.Distance(p.GetPosition(), AZ_VampCryptLurkers.RiverSpot());
		Print("[AZ_Vamp] TEST after: 5 s standing in the river, drift " + drift + " m (a push > 0.5 m is a FAIL)");
		AfterClimb(p, 0);
	}





	void AfterClimb(PlayerBase p, int i)
	{
		array<vector> at = {AZ_VampCrypt.Nave(54.1, 0, -1.3), AZ_VampCrypt.Nave(50.0, 1.95, -1.0), AZ_VampCrypt.Nave(50.0, -1.95, -1.0)};
		array<vector> toward = {AZ_VampCrypt.Nave(55.1, 0, -1.3), AZ_VampCrypt.Nave(50.0, 2.95, -1.0), AZ_VampCrypt.Nave(50.0, -2.95, -1.0)};
		array<string> what = {"basin floor -> altar-side rim", "nave river -> right bank wall", "nave river -> left bank wall"};
		if (i > 0)
		{
			SHumanCommandClimbResult res = new SHumanCommandClimbResult;
			HumanCommandClimb.DoClimbTest(p, res, 0);
			Print("[AZ_Vamp] TEST after climb " + what[i - 1] + ": standing at hall " + AZ_VampCrypt.HallLocal(p.GetPosition()) + " climb " + res.m_bIsClimb + " climb over " + res.m_bIsClimbOver + " height " + res.m_fClimbHeight + " finish with fall " + res.m_bFinishWithFall);
		}
		if (i >= at.Count())
		{
			float rim = AZ_VampCrypt.APSE_F + 1.25 + 2.5;
			float f = AZ_VampCryptLurkers.RIVER_SPOT_F;
			float g = AZ_VampFeed.ROOT_GAP;
			Print("[AZ_Vamp] TEST after spots: the feed at hall " + AZ_VampCrypt.HallLocal(AZ_VampCryptLurkers.RiverSpot()) + " (nave f " + f + ", r 0, y -1.0); rear feeder f " + (f - g) + ", front f " + (f + g) + "; the basin sill f " + (AZ_VampCrypt.APSE_F + 1.25) + ", its altar-side rim f " + rim + ": the front feeder " + (rim - f - g) + " m from the rim, the victim " + (rim - f) + " m");
			AfterSteps(p);
			return;
		}
		p.SetPosition(at[i]);
		vector d = toward[i] - at[i];
		d[1] = 0;
		p.SetDirection(d.Normalized());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterClimb, 1500, false, p, i + 1);
	}





	static ref array<float> s_StepF;
	static ref array<float> s_StepH;

	float FloorAt(float f)
	{
		vector from = AZ_VampCrypt.Nave(f, 0, 2.0);
		vector to = AZ_VampCrypt.Nave(f, 0, -3.0);
		vector pos, dir;
		int comp;
		set<Object> hits = new set<Object>;
		if (!DayZPhysics.RaycastRV(from, to, pos, dir, comp, hits, null, null, false, false, ObjIntersectGeom))
			return -9.0;
		vector base = AZ_VampCrypt.Nave(f, 0, 0);
		return pos[1] - base[1];
	}

	void AfterSteps(PlayerBase p)
	{
		s_StepF = new array<float>;
		s_StepH = new array<float>;
		string prof = "";
		for (int k = 0; k <= 30; k++)
		{
			float f = 51.0 + k * 0.1;
			float h = FloorAt(f);
			s_StepF.Insert(f);
			s_StepH.Insert(h);
			prof += " " + f + ":" + h;
		}
		Print("[AZ_Vamp] TEST steps profile (nave f:floor y, r 0):" + prof);

		int bottom = -1;
		float dirF = 1.0;
		for (int i = 0; i + 1 < s_StepH.Count(); i++)
		{
			float rise = s_StepH[i + 1] - s_StepH[i];
			if (rise > 0.12 && rise < 0.5)
			{
				bottom = i;
				break;
			}
		}
		if (bottom < 0)
		{
			dirF = -1.0;
			for (int j = s_StepH.Count() - 1; j > 0; j--)
			{
				float riseB = s_StepH[j - 1] - s_StepH[j];
				if (riseB > 0.12 && riseB < 0.5)
				{
					bottom = j;
					break;
				}
			}
		}
		if (bottom < 0)
		{


			dirF = 1.0;
			bottom = 0;
			for (int kb = 1; kb < s_StepH.Count(); kb++)
			{
				if (s_StepH[kb] < s_StepH[bottom])
					bottom = kb;
			}
			Print("[AZ_Vamp] TEST steps: no riser on the axis (a ramp), walking from the lowest point f " + s_StepF[bottom]);
		}
		float stand = s_StepF[bottom] - dirF * 0.25;
		p.SetPosition(AZ_VampCrypt.Nave(stand, 0, s_StepH[bottom] + 0.05));
		p.SetDirection(AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW) * dirF);
		Print("[AZ_Vamp] TEST steps: bottom riser at f " + s_StepF[bottom] + " floor " + s_StepH[bottom] + ", going " + dirF + "; standing at f " + stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1000, false, p, "after_steps", AZ_VampCrypt.Nave(stand - dirF * 2.0, 1.4, 1.0), AZ_VampCrypt.Nave(stand + dirF * 1.0, 0, -0.6));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterStepTest, 7000, false, p);
	}

	void AfterStepTest(PlayerBase p)
	{
		SHumanCommandClimbResult res = new SHumanCommandClimbResult;
		HumanCommandClimb.DoClimbTest(p, res, 0);
		Print("[AZ_Vamp] TEST steps climb test at the bottom step: standing at hall " + AZ_VampCrypt.HallLocal(p.GetPosition()) + " climb " + res.m_bIsClimb + " climb over " + res.m_bIsClimbOver + " height " + res.m_fClimbHeight);


		s_WalkY0 = p.GetPosition()[1];
		s_WalkMax = 0;
		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_WALK_TEST, new Param1<int>(WALK_MS), true, p.GetIdentity());

		HumanInputController shic = p.GetInputController();
		if (shic)
		{
			shic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 2);
			shic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(WalkServerEnd, WALK_MS, false, p);
		}
		Print("[AZ_Vamp] TEST stairs walk: server input " + (shic != null));
		WalkLog(p, 0);
	}

	static const int WALK_MS = 10000;

	void WalkServerEnd(PlayerBase p)
	{
		HumanInputController shic = p.GetInputController();
		if (!shic)
			return;
		shic.OverrideMovementSpeed(HumanInputControllerOverrideType.DISABLED, 0);
		shic.OverrideMovementAngle(HumanInputControllerOverrideType.DISABLED, 0);
	}
	static float s_WalkY0;
	static float s_WalkMax;

	void WalkLog(PlayerBase p, int i)
	{
		vector l = AZ_VampCrypt.HallLocal(p.GetPosition());
		float up = p.GetPosition()[1] - s_WalkY0;
		if (up > s_WalkMax)
			s_WalkMax = up;
		Print("[AZ_Vamp] TEST stairs walk t " + (i * 500) + " ms: hall " + l + " gained " + up + " m");
		if (i * 500 < WALK_MS + 1500)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(WalkLog, 500, false, p, i + 1);
			return;
		}
		StairRays(p);
		bool pass = l[0] > 53.0 && l[2] > -0.15;
		string verdict = "FAIL: did not reach the nave floor";
		if (pass)
			verdict = "PASS: walked out of the basin";
		Print("[AZ_Vamp] TEST stairs walk end: hall " + l + ", gained " + s_WalkMax + " m of 1.3, " + verdict);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 300, false, p, "after_stairs_walk", p.GetPosition() + Vector(0, 1.7, 0) - p.GetDirection() * 2.5, p.GetPosition() + Vector(0, 1.0, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AfterGate, 6500, false, p);
	}


	void StairRays(PlayerBase p)
	{
		vector fwd = AZ_VampCrypt.Nave(1.0, 0, 0) - AZ_VampCrypt.Nave(0, 0, 0);
		fwd[1] = 0;
		fwd.Normalize();
		array<float> ups = {0.2, 0.5, 1.0, 1.7};
		array<int> kinds = {ObjIntersectGeom, ObjIntersectView};
		foreach (int kind : kinds)
		{
			foreach (float up : ups)
			{
				vector from = p.GetPosition() + Vector(0, up, 0);
				vector to = from + fwd * 1.5;
				vector hit;
				vector hitDir;
				int comp;
				set<Object> objs = new set<Object>;
				string what = "nothing";
				if (DayZPhysics.RaycastRV(from, to, hit, hitDir, comp, objs, null, p, true, false, kind))
				{
					what = "ground or terrain";
					if (objs.Count() > 0 && objs[0])
						what = objs[0].GetType() + " (" + objs[0].GetModelName() + ")";
					what = what + " at hall " + AZ_VampCrypt.HallLocal(hit) + ", " + vector.Distance(from, hit) + " m";
				}
				Print("[AZ_Vamp] TEST stairs ray " + kind + " at +" + up + " m: " + what);
			}
		}
	}

	void AfterGate(PlayerBase p)
	{
		string tp = AZ_VampCryptLedge.AdminTp(p, "gate");
		Print("[AZ_Vamp] TEST after: the gate: " + tp + " at hall " + AZ_VampCrypt.HallLocal(p.GetPosition()));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2500, false, p, "after_gate", p.GetPosition() + p.GetDirection() * -2.0 + Vector(0, 1.7, 0), p.GetPosition() + p.GetDirection() * 3.0 + Vector(0, 1.2, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ExitTest, 8000, false, p);
		Print("[AZ_Vamp] TEST after: the rest of the night took " + (GetGame().GetTime() - s_AfterAt) + " ms");
	}



	void ExitCheck(string at)
	{
		AZ_Crypt_Exit_Slab s = AZ_VampCryptExit.s_Slab;
		bool open = s && s.IsOpen();
		Print("[AZ_Vamp] TEST exit " + at + ": slab " + (s != null) + " open " + open + " night " + AZ_VampState.s_Data.cryptExitNight + " tonight " + AZ_VampCryptExit.NightKey() + " openTonight " + AZ_VampCryptExit.OpenTonight());
	}

	void ExitTest(PlayerBase p)
	{
		ExitCheck("before restart");
		if (AZ_VampCryptExit.s_Slab)
			AZ_VampCryptExit.s_Slab.SetOpen(false);
		AZ_VampCryptExit.Restore();
		ExitCheck("after restart");
		vector dir = AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW);
		p.SetPosition(AZ_VampCryptExit.Front(0.5));
		p.SetDirection(dir);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "exit_door", AZ_VampCryptExit.Front(2.5) + Vector(0, 1.7, 0), AZ_VampCryptExit.Front(-0.6) + Vector(0, 1.6, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ExitLeave, 8000, false, p);
	}

	void ExitLeave(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST exit: at the door " + AZ_VampCryptExit.AtDoor(p) + " can move " + AZ_VampCrypt.CanMove(p) + " hall " + AZ_VampCrypt.HallLocal(p.GetPosition()));
		AZ_VampCryptExit.Leave(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ExitLanded, 5000, false, p);
	}

	void ExitLanded(PlayerBase p)
	{
		float best = 9999;
		array<vector> slots = AZ_VampCryptA1.ExitSlots();
		if (AZ_VampState.Cfg().exitLanding != "a1")
			slots = AZ_VampCryptExit.VorkutaSlots();
		foreach (vector v : slots)
			best = Math.Min(best, vector.Distance(v, p.GetPosition()));
		Print("[AZ_Vamp] TEST exit: landed " + p.GetPosition() + " (landing " + AZ_VampState.Cfg().exitLanding + "), " + best + " m from the nearest landing slot (over 1.5 m is a FAIL), inside " + AZ_VampCrypt.Inside(p.GetPosition()));

		vector l2 = p.GetPosition() + Vector(0, 1.7, 0);
		vector fw = AZ_VampCryptExit.VorkutaFwd();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 500, false, p, "exit_landed", l2 - fw * 1.5, l2 + fw * 20.0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PortalTest, 3000, false, p);
	}


	void PortalTest(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST portal open after the wake: " + (AZ_VampCryptRiverPortal.s_Portal != null) + " openTonight " + AZ_VampCryptExit.OpenTonight() + ", rolled destination " + AZ_VampCryptRiverPortal.Dest() + " (cryptPortalVorkuta " + AZ_VampState.s_Data.cryptPortalVorkuta + ")");
		vector bank = AZ_VampCryptRiverPortal.Bank(-1);
		vector toP = AZ_VampCryptRiverPortal.At() - bank;
		toP[1] = 0;
		p.SetPosition(bank);
		p.SetDirection(toP.Normalized());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2000, false, p, "river_portal_open", bank - toP.Normalized() * 1.5 + Vector(0, 1.7, 0), AZ_VampCryptRiverPortal.At());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PortalLeave, 8000, false, p);
	}

	void PortalLeave(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST portal: at the portal " + AZ_VampCryptRiverPortal.AtPortal(p) + " can move " + AZ_VampCrypt.CanMove(p));
		AZ_VampCryptRiverPortal.Leave(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PortalLanded, 5000, false, p);
	}

	void PortalLanded(PlayerBase p)
	{
		float best = 9999;
		string at = "A1";
		array<vector> a1s = AZ_VampCryptA1.ExitSlots();
		array<vector> vks = AZ_VampCryptExit.VorkutaSlots();
		foreach (vector v : a1s)
			best = Math.Min(best, vector.Distance(v, p.GetPosition()));
		foreach (vector vk : vks)
		{
			float dk = vector.Distance(vk, p.GetPosition());
			if (dk < best)
			{
				best = dk;
				at = "Vorkuta";
			}
		}
		Print("[AZ_Vamp] TEST portal: landed " + p.GetPosition() + " at " + at + " (Vorkuta chance " + AZ_VampState.Cfg().portalVorkutaChance + "), " + best + " m from the nearest slot (over 1.5 m is a FAIL), inside " + AZ_VampCrypt.Inside(p.GetPosition()));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ExitDawn, 3000, false, p);
	}

	void ExitDawn(PlayerBase p)
	{
		AZ_VampCryptExit.Dawn();
		ExitCheck("after dawn");
		Print("[AZ_Vamp] TEST portal after dawn: " + (AZ_VampCryptRiverPortal.s_Portal != null) + " (true is a FAIL)");
		p.SetPosition(AZ_VampCryptLedge.Floor(8.0, 3.5) + Vector(0, 0.05, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 3500, false, p, "river_portal_shut", AZ_VampCrypt.Nave(2.2, 0, 1.65), AZ_VampCryptRiverPortal.At());
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombTest, 12000, false, p);
	}



	void TombTest(PlayerBase p)
	{
		vector away = -AZ_VampCrypt.Fwd(AZ_VampCrypt.DOOR_YAW);
		vector m = AZ_VampCrypt.Mouth();
		p.SetPosition(m);
		p.SetDirection(-away);
		Print("[AZ_Vamp] TEST tomb: at the door " + AZ_VampCrypt.AtDoor(p.GetPosition()) + ", tomb on " + AZ_VampCryptTomb.On() + ", key needed " + AZ_VampCryptTomb.NeedsKey() + ", keys on the dais " + AZ_VampCryptKeys.KeyCount() + " (expect 0)");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "portal_front_night", m + away * 7.0 + Vector(0, 2.2, 0), AZ_VampCrypt.DoorOrigin() + Vector(0, 2.0, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombEnter, 8000, false, p);
	}

	void TombEnter(PlayerBase p)
	{
		AZ_VampCrypt.Enter(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombInside, 5000, false, p);
	}

	void TombInside(PlayerBase p)
	{
		vector l = AZ_VampCrypt.HallLocal(p.GetPosition());
		bool ok = AZ_VampCryptGallery.InGallery(p.GetPosition()) && l[0] < -13.0;
		Print("[AZ_Vamp] TEST tomb: in at hall " + l + ", gallery west end " + ok + " (false is a FAIL)");
		vector eye = p.GetPosition() + Vector(0, 1.6, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "gallery_arrival", eye - p.GetDirection() * 0.6, eye + p.GetDirection() * 4.0);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 7500, false, p, "gallery_from_hall", AZ_VampCrypt.Hall(2.0, -11.0, 1.6), AZ_VampCrypt.Hall(-14.0, -11.0, 0.6));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombToLeave, 14000, false, p);
	}

	void TombToLeave(PlayerBase p)
	{
		p.SetPosition(AZ_VampCrypt.Hall(AZ_VampCryptTomb.LEAVE_F, AZ_VampCryptTomb.LEAVE_R, 0.05));
		p.SetDirection(AZ_VampCrypt.Side(AZ_VampCrypt.HALL_YAW));
		vector eye = p.GetPosition() + Vector(0, 1.6, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 1500, false, p, "leave_tomb_prompt", eye - p.GetDirection() * 1.8 + Vector(0, 0.2, 0), AZ_VampCrypt.Hall(AZ_VampCryptTomb.VOTIVE_F, AZ_VampCryptTomb.VOTIVE_R, 0.4));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombLeave, 8000, false, p);
	}

	void TombLeave(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST tomb: at the leave " + AZ_VampCryptTomb.AtLeave(p) + ", can move " + AZ_VampCrypt.CanMove(p));
		AZ_VampCryptTomb.Leave(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombOut, 5000, false, p);
	}

	void TombOut(PlayerBase p)
	{
		float d = vector.Distance(p.GetPosition(), AZ_VampCrypt.Mouth());
		vector away = -AZ_VampCrypt.Fwd(AZ_VampCrypt.DOOR_YAW);
		float faces = vector.Dot(p.GetDirection(), away);
		Print("[AZ_Vamp] TEST tomb: out at " + p.GetPosition() + ", " + d + " m from the mouth (over 3 m is a FAIL), facing away " + faces + " (under 0.5 is a FAIL), inside " + AZ_VampCrypt.Inside(p.GetPosition()));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombOffTest, 3000, false, p);
	}



	void TombOffTest(PlayerBase p)
	{
		AZ_VampState.Cfg().tombDoor = false;
		vector m = AZ_VampCrypt.Mouth();
		Print("[AZ_Vamp] TEST tomb off: Enter at the door " + AZ_VampCrypt.AtDoor(m) + " (true is a FAIL)");
		p.m_AZCryptFrom = 0;
		p.SetPosition(AZ_VampCryptLedge.Slots()[0]);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombOffLeave, 2000, false, p);
	}

	void TombOffLeave(PlayerBase p)
	{
		AZ_VampCrypt.MoveOut(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TombOffOut, 5000, false, p);
	}

	void TombOffOut(PlayerBase p)
	{
		float best = 9999;
		array<vector> slots = AZ_VampCryptA1.ExitSlots();
		foreach (vector v : slots)
			best = Math.Min(best, vector.Distance(v, p.GetPosition()));
		Print("[AZ_Vamp] TEST tomb off: a way-0 leaver came up " + best + " m from an A1 exit slot (over 1.5 m is a FAIL)");
		AZ_VampState.Cfg().tombDoor = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderAttack, 3000, false, p);
	}



	static PlayerBase s_WrathElder;

	void ElderAttack(PlayerBase p)
	{
		if (p.m_AZVampire)
			AZ_VampEvent.Cure(p);
		vector dir = AZ_VampCrypt.Fwd(AZ_VampCrypt.HALL_YAW);
		p.SetPosition(AZ_VampCrypt.Nave(40.0, 0, 0));
		p.SetDirection(dir);
		p.SetHealth("", "Blood", 5000);
		p.SetHealth("", "Shock", 100);
		s_WrathElder = AZ_VampCryptLurkers.Dress(AZ_VampCrypt.Nave(42.5, 0, 0), 0, 3);
		if (!s_WrathElder)
		{
			Print("[AZ_Vamp] TEST elder wrath FAIL: no elder");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedRunEnd, 1000, false, p);
			return;
		}
		s_WrathElder.SetDirection(-dir);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderStrike, 2000, false, p);
	}

	void ElderStrike(PlayerBase p)
	{
		if (!p || !s_WrathElder)
			return;
		Print("[AZ_Vamp] TEST elder wrath: the player strikes the elder (smoke " + AZ_VampState.Cfg().eldersSmoke + ", elder " + s_WrathElder.m_AZElder + ")");
		s_WrathElder.ProcessDirectDamage(DamageType.CLOSE_COMBAT, p, "Torso", "MeleeFist", "0 1 0", 1.0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderFallback, 1000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2000, false, p, "elder_wrath", AZ_VampCrypt.Nave(38.0, 2.2, 1.6), AZ_VampCrypt.Nave(40.0, 0, 1.0));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ElderAfter, AZ_VampElderWrath.BITE_MS + 2500, false, p);
	}



	void ElderFallback(PlayerBase p)
	{
		if (!p || !s_WrathElder || p.m_AZVampHeld)
			return;
		Print("[AZ_Vamp] TEST elder wrath: no bite from the hit path, calling AZ_VampElderWrath.Hit directly");
		AZ_VampElderWrath.Hit(s_WrathElder, p);
	}

	void ElderAfter(PlayerBase p)
	{
		Print("[AZ_Vamp] TEST elder wrath after: alive " + p.IsAlive() + " unconscious " + p.IsUnconscious() + " blood " + p.GetHealth("", "Blood") + " held " + p.m_AZVampHeld + " (a death or blood under 2500 is a FAIL)");
		if (s_WrathElder)
			GetGame().ObjectDelete(s_WrathElder);
		s_WrathElder = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FeedRunEnd, 4000, false, p);
	}

	void FeedRunEnd(PlayerBase p)
	{
		FeedRunDone(p);
	}


	void FeedRunDone(PlayerBase p)
	{
		if (p && p.m_AZVampire)
			AZ_VampEvent.Cure(p);
		AZ_VampRitual.Friends(null, "clear");
		s_HwGuard = true;
		if (p)
			HwKeepAlive(p);
		Print("[AZ_Vamp] TEST finale probe end at +" + (GetGame().GetTime() - s_FeedStart) + " ms: cured, friends cleared, god mode back");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 2000, false, p, HwNextStep(327));
	}






	int LookFrames(array<string> names, array<vector> stands, array<vector> cams, array<vector> looks)
	{
		float ap = AZ_VampCrypt.APSE_F;
		float w = -AZ_VampCrypt.NAVE_DEPTH + AZ_VampCrypt.s_Blood;
		vector trapStand = AZ_VampCrypt.Hall(-12.4, -3.4, 0.1);
		names.Insert("arrival_w"); stands.Insert(AZ_VampCrypt.Hall(-19.0, -3.0, 0.1)); cams.Insert(AZ_VampCrypt.Hall(-20.3, 0, 1.65)); looks.Insert(AZ_VampCrypt.Hall(-16.1, 0, 1.1));
		names.Insert("arrival_c"); stands.Insert(AZ_VampCrypt.Hall(-19.0, -3.0, 0.1)); cams.Insert(AZ_VampCrypt.Hall(-19.2, -4.0, 1.65)); looks.Insert(AZ_VampCrypt.Hall(-15.5, 4.5, 1.2));
		names.Insert("trap_w"); stands.Insert(trapStand); cams.Insert(AZ_VampCrypt.Hall(-13.2, -4.6, 1.7)); looks.Insert(AZ_VampCrypt.Hall(-5.0, -0.5, 1.0));
		names.Insert("trap_c"); stands.Insert(trapStand); cams.Insert(AZ_VampCrypt.Hall(-9.8, -0.6, 2.4)); looks.Insert(AZ_VampCrypt.Hall(-7.4, -5.85, 0.0));
		names.Insert("cage_w"); stands.Insert(trapStand); cams.Insert(AZ_VampCrypt.Hall(-13.0, -3.0, 2.2)); looks.Insert(AZ_VampCrypt.Hall(-9.6, -0.5, 1.0));
		names.Insert("cage_c"); stands.Insert(trapStand); cams.Insert(AZ_VampCrypt.Hall(-11.6, -1.6, 1.6)); looks.Insert(AZ_VampCrypt.Hall(-9.6, -0.5, 0.9));
		names.Insert("stairs_w"); stands.Insert(AZ_VampCrypt.Hall(0.15, 0.8, 0.1)); cams.Insert(AZ_VampCrypt.Hall(0.15, 0, 1.6)); looks.Insert(AZ_VampCrypt.Nave(40.0, 0, 4.0));
		names.Insert("stairs_c"); stands.Insert(AZ_VampCrypt.Hall(0.15, 0.8, 0.1)); cams.Insert(AZ_VampCrypt.Nave(5.6, 3.2, 1.6)); looks.Insert(AZ_VampCrypt.Nave(1.4, 5.6, 1.3));
		names.Insert("gallery_w"); stands.Insert(LookBeside(AZ_VampCrypt.Hall(1.4, -11.6, 1.55), AZ_VampCrypt.Hall(-12.0, -11.6, 0.8), AZ_VampCrypt.Hall(1.2, -11.6, 0.1))); cams.Insert(AZ_VampCrypt.Hall(1.4, -11.6, 1.55)); looks.Insert(AZ_VampCrypt.Hall(-12.0, -11.6, 0.8));
		names.Insert("gallery_c"); stands.Insert(LookBeside(AZ_VampCrypt.Hall(-14.0, -9.9, 1.6), AZ_VampCrypt.Hall(-14.0, -6.3, 1.1), AZ_VampCrypt.Hall(-14.0, -9.6, 0.1))); cams.Insert(AZ_VampCrypt.Hall(-14.0, -9.9, 1.6)); looks.Insert(AZ_VampCrypt.Hall(-14.0, -6.3, 1.1));
		names.Insert("nave_w"); stands.Insert(AZ_VampCrypt.Nave(14.0, -5.0, 0.1)); cams.Insert(AZ_VampCrypt.Nave(13.0, -3.0, 1.6)); looks.Insert(AZ_VampCrypt.Nave(50.0, 0, 1.5));
		names.Insert("nave_c"); stands.Insert(AZ_VampCrypt.Nave(22.0, -5.4, 0.1)); cams.Insert(AZ_VampCrypt.Nave(24.0, -2.9, 0.9)); looks.Insert(AZ_VampCrypt.Nave(27.0, 0.3, w));
		names.Insert("ossuary_w"); stands.Insert(LookBeside(AZ_VampCrypt.Nave(32.0, -11.4, 1.6), AZ_VampCrypt.Nave(40.2, -12.4, 1.6), AZ_VampCrypt.Nave(32.4, -11.2, 0.1))); cams.Insert(AZ_VampCrypt.Nave(32.0, -11.4, 1.6)); looks.Insert(AZ_VampCrypt.Nave(40.2, -12.4, 1.6));
		names.Insert("ossuary_c"); stands.Insert(LookBeside(AZ_VampCrypt.Nave(32.0, -11.4, 1.6), AZ_VampCrypt.Nave(40.2, -12.4, 1.6), AZ_VampCrypt.Nave(32.4, -11.2, 0.1))); cams.Insert(AZ_VampCrypt.Nave(32.0, -11.4, 1.6)); looks.Insert(AZ_VampCrypt.Nave(36.0, -12.4, 0.8));

		names.Insert("chapel_door"); stands.Insert(AZ_VampCrypt.Nave(36.0, -8.2, 0.1)); cams.Insert(AZ_VampCrypt.Nave(36.0, -9.4, 1.6)); looks.Insert(AZ_VampCrypt.Nave(36.0, -13.5, 1.2));
		names.Insert("tomb_w"); stands.Insert(LookBeside(AZ_VampCrypt.Nave(22.0, 11.2, 1.5), AZ_VampCrypt.Nave(27.5, 12.8, 0.5), AZ_VampCrypt.Nave(22.2, 11.3, 0.1))); cams.Insert(AZ_VampCrypt.Nave(22.0, 11.2, 1.5)); looks.Insert(AZ_VampCrypt.Nave(27.5, 12.8, 0.5));
		names.Insert("aisle_c"); stands.Insert(LookBeside(AZ_VampCrypt.Nave(9.2, -6.4, 1.6), AZ_VampCrypt.Nave(6.0, -9.5, 1.0), AZ_VampCrypt.Nave(9.5, -6.6, 0.1))); cams.Insert(AZ_VampCrypt.Nave(9.2, -6.4, 1.6)); looks.Insert(AZ_VampCrypt.Nave(6.0, -9.5, 1.0));
		names.Insert("altar_w"); stands.Insert(AZ_VampCrypt.Nave(50.0, 2.6, 0.1)); cams.Insert(AZ_VampCrypt.Nave(50.5, 1.8, 1.4)); looks.Insert(AZ_VampCrypt.Nave(ap + 8.6, 0, 3.4));
		names.Insert("altar_c"); stands.Insert(AZ_VampCrypt.Nave(ap + 4.6, 0.8, AZ_VampCrypt.DAIS_Y + 0.1)); cams.Insert(AZ_VampCrypt.Nave(ap + 5.2, 0.6, 3.6)); looks.Insert(AZ_VampCrypt.Nave(ap + 8.6, 0, 5.2));
		return names.Count();
	}


	vector LookBeside(vector cam, vector look, vector floor)
	{
		vector back = look - cam;
		back[1] = 0;
		back.Normalize();
		vector stand = cam + Vector(back[2], 0, -back[0]) * 0.6;
		stand[1] = floor[1];
		return stand;
	}

	void LookPlace(PlayerBase p, vector stand, vector look)
	{
		if (!p)
			return;
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
	}

	void LookPass(PlayerBase p, bool vampire)
	{
		if (!p)
			return;
		if (vampire)
			AZ_VampBind.Bind(p);
		else
			AZ_VampBind.Unbind(p);
		Print("[AZ_Vamp] TEST look pass vampire " + vampire + " (flag " + p.m_AZVampire + ")");
	}



	bool LookWanted(string label)
	{
		return !s_HwOnly || s_HwOnly.Find(label) >= 0;
	}

	bool LookWantedAny(string a, string b = "", string c = "", string d = "")
	{
		return LookWanted(a) || (b != "" && LookWanted(b)) || (c != "" && LookWanted(c)) || (d != "" && LookWanted(d));
	}



	void DiagPiece(PlayerBase p, int k)
	{
		if (!p)
			return;
		if (k == 1)
			AZ_VampBand.Send(p, "A");
		else if (k == 2)
			GetGame().GetWorld().SetDate(2011, 11, 20, 23, 30);
		else if (k == 3)
		{
			AZ_VampProgress.s_BloodMoon = false;
			p.m_AZBloodMoon = false;
			p.SetSynchDirty();
		}
		Print("[AZ_Vamp] TEST diag piece " + k);
	}

	int LookRun(PlayerBase p)
	{
		int t0 = 0;
		if (LookWanted("diag_pre"))
		{
			array<string> dn = new array<string>;
			array<vector> ds = new array<vector>;
			array<vector> dc = new array<vector>;
			array<vector> dl = new array<vector>;
			LookFrames(dn, ds, dc, dl);
			int dnave = dn.Find("nave_w");
			ScriptCallQueue dq = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);
			dq.CallLater(LookPlace, 0, false, p, ds[dnave], dl[dnave]);
			dq.CallLater(HwShot, 8000, false, p, "diag_pre", dc[dnave], dl[dnave]);
			dq.CallLater(DiagPiece, 14000, false, p, 1);
			dq.CallLater(HwShot, 22000, false, p, "diag_band", dc[dnave], dl[dnave]);
			dq.CallLater(DiagPiece, 28000, false, p, 2);
			dq.CallLater(HwShot, 36000, false, p, "diag_date", dc[dnave], dl[dnave]);
			dq.CallLater(DiagPiece, 42000, false, p, 3);
			dq.CallLater(HwShot, 50000, false, p, "diag_bm", dc[dnave], dl[dnave]);
			t0 = 57000;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MoonVariant, t0, false, p, "E");
		array<string> names = new array<string>;
		array<vector> stands = new array<vector>;
		array<vector> cams = new array<vector>;
		array<vector> looks = new array<vector>;
		int n = LookFrames(names, stands, cams, looks);
		ScriptCallQueue q = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);
		int t = t0 + 2000;


		int nave = names.Find("nave_w");
		if (LookWantedAny("look_moon_off", "look_moon_e"))
		{
			q.CallLater(LookPlace, t, false, p, stands[nave], looks[nave]);
			q.CallLater(MoonVariant, t, false, p, "off");
			q.CallLater(HwShot, t + 8000, false, p, "look_moon_off", cams[nave], looks[nave]);
			q.CallLater(MoonVariant, t + 14500, false, p, "E");
			q.CallLater(HwShot, t + 22500, false, p, "look_moon_e", cams[nave], looks[nave]);
			t += 29000;
		}


		vector outStand, outCam, outLook;
		if (LookWantedAny("look_entry_05", "look_entry_15", "look_entry_30", "look_entry_60") && AZ_VampCrypt.ShotSpot(0, outStand, outCam, outLook))
		{
			outStand[1] = GetGame().SurfaceY(outStand[0], outStand[2]);
			q.CallLater(LookPlace, t, false, p, outStand, outLook);
			t += 20000;
			q.CallLater(LookPlace, t, false, p, stands[nave], looks[nave]);
			q.CallLater(HwShot, t + 5000, false, p, "look_entry_05", cams[nave], looks[nave]);
			q.CallLater(HwShot, t + 15000, false, p, "look_entry_15", cams[nave], looks[nave]);
			q.CallLater(HwShot, t + 30000, false, p, "look_entry_30", cams[nave], looks[nave]);
			q.CallLater(HwShot, t + 60000, false, p, "look_entry_60", cams[nave], looks[nave]);
			t += 67000;
		}



		if (!AZ_VampCrypt.EntryOn() && AZ_VampCrypt.ShotSpot(0, outStand, outCam, outLook))
		{
			if (LookWanted("ledge_spawn"))
			{
				outStand[1] = GetGame().SurfaceY(outStand[0], outStand[2]);
				q.CallLater(LookPlace, t, false, p, outStand, outLook);
				q.CallLater(AZ_VampCrypt.EnterFrom, t + 4000, false, p, 0);
				array<vector> ledgeSlots = AZ_VampCryptLedge.Slots();
				vector slot0 = ledgeSlots[0];
				q.CallLater(HwShot, t + 12000, false, p, "ledge_spawn", slot0 + Vector(0, 1.6, 0), AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 3.0));
				t += 18500;
			}
			vector exitAt = AZ_VampCrypt.Hall(AZ_VampCryptLedge.EXIT_F + 0.6, AZ_VampCryptLedge.EXIT_R + 0.3, 1.0);
			if (LookWanted("ledge_leave"))
			{
				q.CallLater(LookPlace, t, false, p, exitAt, AZ_VampCrypt.Hall(-3.0, AZ_VampCryptLedge.EXIT_R, 1.0));
				q.CallLater(HwShot, t + 6000, false, p, "ledge_leave", AZ_VampCrypt.Hall(AZ_VampCryptLedge.EXIT_F + 2.0, AZ_VampCryptLedge.EXIT_R + 1.2, 1.7), AZ_VampCrypt.Hall(-0.5, AZ_VampCryptLedge.EXIT_R, 1.0));
				t += 12500;
			}
			array<string> sealN = new array<string>;
			sealN.Insert("seal_breach");
			sealN.Insert("seal_nook");
			sealN.Insert("seal_slab");
			sealN.Insert("seal_tomb");
			sealN.Insert("seal_skull");
			array<vector> sealS = new array<vector>;


			sealS.Insert(AZ_VampCrypt.Nave(3.0, 1.0, 0.1));
			sealS.Insert(AZ_VampCrypt.Hall(-10.6, -11.6, 0.1));
			sealS.Insert(AZ_VampCrypt.Hall(-15.6, -11.6, 0.1));
			sealS.Insert(AZ_VampCrypt.Nave(18.8, 8.6, 0.1));
			sealS.Insert(AZ_VampCrypt.Nave(16.4, 5.9, 0.1));
			array<vector> sealC = new array<vector>;
			sealC.Insert(AZ_VampCrypt.Nave(2.6, 0, 1.6));
			sealC.Insert(AZ_VampCrypt.Hall(-9.0, -12.2, 1.5));
			sealC.Insert(AZ_VampCrypt.Hall(-14.0, -12.2, 1.5));
			sealC.Insert(AZ_VampCrypt.Nave(20.5, 9.2, 1.3));
			sealC.Insert(AZ_VampCrypt.Nave(18.0, 5.5, 1.6));
			array<vector> sealL = new array<vector>;
			sealL.Insert(AZ_VampCrypt.Hall(-1.0, 0, 1.2));
			sealL.Insert(AZ_VampCrypt.Hall(-9.0, -10.4, 1.0));
			sealL.Insert(AZ_VampCrypt.Hall(-14.0, -10.4, 1.0));
			sealL.Insert(AZ_VampCrypt.Nave(20.5, 11.3, 0.8));
			sealL.Insert(AZ_VampCrypt.Nave(18.0, 8.5, 1.2));
			for (int sn = 0; sn < sealN.Count(); sn++)
			{
				if (!LookWanted(sealN[sn]))
					continue;
				q.CallLater(LookPlace, t, false, p, sealS[sn], sealL[sn]);
				q.CallLater(HwShot, t + 6000, false, p, sealN[sn], sealC[sn], sealL[sn]);
				t += 12500;
			}
		}


		vector gate = AZ_VampCrypt.Hall(-AZ_VampCrypt.HALF_LEN - 0.4, 0, 1.2);
		vector gateStand = AZ_VampCrypt.Hall(-AZ_VampCrypt.HALF_LEN + 1.4, 0.3, 0.1);
		if (LookWanted("look_leave_gate"))
		{
			q.CallLater(LookPlace, t, false, p, gateStand, gate);
			q.CallLater(HwShot, t + 6000, false, p, "look_leave_gate", AZ_VampCrypt.Hall(-AZ_VampCrypt.HALF_LEN + 1.6, -0.6, 1.65), gate);
			t += 12500;
		}


		vector hisStand = Vector(7646.8, 25.4, 8813.7);
		vector hisFace = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 6.5);
		array<string> hisBands = {"A", "L", "M", "N"};
		bool hisAny = false;
		foreach (string hb : hisBands)
		{
			if (!LookWanted("apse_his_" + hb))
				continue;
			if (!hisAny)
			{
				q.CallLater(LookPlace, t, false, p, hisStand, hisFace);
				t += 4000;
				hisAny = true;
			}
			q.CallLater(AZ_VampBand.Send, t, false, p, hb);
			q.CallLater(HwShot, t + 8000, false, p, "apse_his_" + hb, hisStand + Vector(0, 1.6, 0), hisFace);
			t += 14500;
		}
		if (hisAny)
			q.CallLater(AZ_VampBand.Send, t, false, p, "A");
		for (int pass = 0; pass < 2; pass++)
		{
			bool vamp = pass == 1;
			string pre = "look_h_";
			if (vamp)
				pre = "look_v_";
			bool passWanted = false;
			for (int w = 0; w < n; w++)
			{
				if (LookWantedAny(pre + names[w], pre + names[w] + "_vu") || (!vamp && names[w] == "altar_c" && LookWanted("look_statue_hold_00")))
					passWanted = true;
			}
			if (!passWanted)
				continue;
			q.CallLater(LookPass, t, false, p, vamp);
			t += 10000;
			for (int i = 0; i < n; i++)
			{
				string label = pre + names[i];
				bool holdWanted = !vamp && names[i] == "altar_c" && LookWanted("look_statue_hold_00");
				if (!LookWantedAny(label, label + "_vu") && !holdWanted)
					continue;
				q.CallLater(LookPlace, t, false, p, stands[i], looks[i]);
				int settle = 6000;
				q.CallLater(HwShot, t + settle, false, p, label, cams[i], looks[i]);
				t += settle + 6500;
				if (!vamp && names[i].IndexOf("_w") > 0 && LookWanted(label + "_vu"))
				{
					q.CallLater(AZ_VampBand.Send, t, false, p, "C");
					q.CallLater(HwShot, t + 4000, false, p, label + "_vu", cams[i], looks[i]);
					q.CallLater(AZ_VampBand.Send, t + 10000, false, p, "A");
					t += 10500;
				}
				if (holdWanted)
				{

					for (int h = 0; h < 10; h++)
						q.CallLater(HwShot, t + 1000 * h, false, p, "look_statue_hold_0" + h.ToString(), cams[i], looks[i]);
					t += 16000;
				}
			}
		}
		Print("[AZ_Vamp] TEST look tour: " + n + " frames a pass, " + t + " ms");
		return t + 2000;
	}

	array<string> A1Labels()
	{
		array<string> l = {"a1_crack", "a1_orb", "a1_flood", "a1_crack_bm"};
		return l;
	}

	override string HwStepLabel(int step)
	{
		if ((step >= 89 && step <= 97) || (step >= 150 && step <= 157) || (step >= 310 && step <= 327))
			return HwCryptLabel(step);
		return super.HwStepLabel(step);
	}

	override int HwNextStep(int step)
	{
		if (step == 88 && s_HwOnly)
			return 89;
		if (step >= 89 && step <= 96)
			return step + 1;
		if (step == 97)
			return 150;
		if (step == 150)
			return 151;
		if (step >= 151 && step <= 156)
			return step + 1;
		if (step == 157)
			return 310;
		if (step >= 310 && step <= 320)
			return step + 1;
		if (step == 321 || step == 322)
			return step + 1;
		if (step == 323)
			return 324;
		if (step == 324)
			return 325;
		if (step == 325)
			return 326;
		if (step == 326)
			return 327;
		if (step == 327)
			return super.HwNextStep(88);
		return super.HwNextStep(step);
	}

	override void HwStep(PlayerBase p, int step)
	{
		if ((step < 89 || step > 97) && (step < 150 || step > 157) && (step < 310 || step > 327))
		{
			super.HwStep(p, step);
			return;
		}
		if (!p)
			return;
		if (!HwWanted(step))
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		if (step == 327)
		{
			FeedRun(p);
			return;
		}
		if (step == 326)
		{
			int lookMs = LookRun(p);
			int lookNext = HwNextStep(step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, lookMs, false, p, lookNext);
			return;
		}
		if (step == 325)
		{
			int cutrunMs = CutRun(p);
			int cutrunNext = HwNextStep(step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, cutrunMs, false, p, cutrunNext);
			return;
		}
		if (step == 324)
		{
			int riverrunMs = RiverRun(p);
			int riverrunNext = HwNextStep(step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, riverrunMs, false, p, riverrunNext);
			return;
		}
		if (step == 322 || step == 323)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, MoonAB(p, step), false, p, HwNextStep(step));
			return;
		}
		if (step == 316)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, BandRun(p), false, p, HwNextStep(step));
			return;
		}
		if (step == 321)
		{
			float ap = AZ_VampCrypt.APSE_F;
			p.SetPosition(AZ_VampCrypt.Nave(ap + 4.6, 0.8, AZ_VampCrypt.DAIS_Y + 0.1));
			AZ_VampCrypt.StatueEyes(vector.Zero, true);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "countess_eyes", AZ_VampCrypt.Nave(ap + 5.2, 0.6, 3.6), AZ_VampCrypt.Nave(ap + 8.6, 0, 4.4));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCrypt.StatueEyes, 9500, false, vector.Zero, false);

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampBand.Send, 9500, false, p, "K");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampBand.Send, 16900, false, p, "A");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 10500, false, p, "countess_eyes_off", AZ_VampCrypt.Nave(ap + 5.2, 0.6, 3.6), AZ_VampCrypt.Nave(ap + 8.6, 0, 4.4));

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 16500, false, p, "countess_eyes_near", AZ_VampCrypt.Nave(ap + 7.13, 0.26, 3.2), AZ_VampCrypt.Nave(ap + 8.6, 0, 2.9));

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 23000, false, p, "countess_eyes_far", AZ_VampCrypt.Nave(ap + 2.32, 1.11, 2.91), AZ_VampCrypt.Nave(ap + 8.6, 0, 4.4));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 29500, false, p, HwNextStep(step));
			return;
		}
		if (step == 320)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, GuideRun(p), false, p, HwNextStep(step));
			return;
		}
		if (step == 319)
		{
			vector note = AZ_VampCrypt.Hall(-15.2, 2.0, 0.03);
			Object probe = GetGame().CreateObjectEx("AZ_VampLore_Probe", note + Vector(0.5, 0, 0), ECE_KEEPHEIGHT | ECE_CREATEPHYSICS);
			if (probe)
				probe.SetPosition(note + Vector(0.5, 0, 0));
			p.SetPosition(AZ_VampCrypt.Hall(-13.6, 2.0, 0.1));
			Print("[AZ_Vamp] TEST crypt lore_probe: probe " + (probe != null) + " beside note 3 at " + note);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "lore_probe", note + Vector(0.25, 1.1, -0.35), note + Vector(0.25, 0, 0));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
			return;
		}
		if (step == 317 || step == 318)
		{
			FrescoShot(p, step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
			return;
		}
		if (step >= 310)
		{
			GalleryShot(p, step);
			int gnext = 10000;
			if (step == 313 || step == 314)
				gnext = 16000;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, gnext, false, p, HwNextStep(step));
			return;
		}
		if (step >= 155)
		{
			SealShot(p, step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
			return;
		}
		if (step == 154)
		{
			AZ_VampCryptBlood.Restock();
			p.SetPosition(AZ_VampCrypt.Hall(-10.4, 1.2, 0.1));
			Print("[AZ_Vamp] TEST crypt crypt_blood");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, "crypt_blood", AZ_VampCrypt.Hall(-9.8, -0.6, 2.4), AZ_VampCrypt.Hall(-7.4, -5.85, 0.0));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
			return;
		}
		if (step == 152 || step == 153)
		{
			DeepShot(p, step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
			return;
		}
		if (step == 150 || step == 151)
		{
			AnteShot(p, step);
			int next = 10000;
			if (step == 151)
				next = 16000;
			if (step == 150)
				next = 23500;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, next, false, p, HwNextStep(step));
			return;
		}
		if (step >= 92)
		{
			int wait = A1Step(p, step);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, wait, false, p, HwNextStep(step));
			return;
		}

		vector stand, cam, look;
		if (!AZ_VampCrypt.ShotSpot(step - 88, stand, cam, look))
		{
			Print("[AZ_Vamp] TEST crypt shot spot " + (step - 88) + " missing");
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 100, false, p, HwNextStep(step));
			return;
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand + " inside " + AZ_VampCrypt.Inside(stand));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, label, cam, look);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwStep, 10000, false, p, HwNextStep(step));
	}



	int A1Step(PlayerBase p, int step)
	{
		vector crack = AZ_VampCryptA1.Crack();
		vector orb = AZ_VampCryptA1.ORB;
		vector cam;
		vector look;
		switch (step)
		{
			case 92:
				GetGame().GetWorld().SetDate(2011, 11, 5, 23, 0);
				AZ_VampProgress.s_BloodMoon = false;
				AZ_VampCryptA1.s_TestOpen = true;
				AZ_VampCryptA1.Refresh();
				p.SetPosition(AZ_VampCryptA1.ExitPoint());
				Print("[AZ_Vamp] TEST a1 at " + AZ_VampCryptA1.ExitPoint() + " crack " + crack + " at crack " + AZ_VampCryptA1.AtCrack(crack));
				return 8000;
			case 95:
				AZ_VampProgress.s_BloodMoon = true;
				AZ_VampCryptA1.Refresh();
				return 8000;
			case 93:
			case 97:
				cam = crack + Vector(-3.2, 1.7, 2.4);
				cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.7;
				look = crack + Vector(0.4, -0.1, 0);
				break;
			case 94:
			case 96:
				cam = orb + "14 -2.5 -7";
				cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.7;
				look = orb - "0 4 0";
				break;
		}
		int li = step - 93;
		if (step >= 96)
			li = step - 94;
		HwShot(p, A1Labels()[li], cam, look);
		if (step == 97 || (step == 96 && !HwWanted(97)))
		{
			AZ_VampProgress.s_BloodMoon = false;
			AZ_VampCryptA1.s_TestOpen = false;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptA1.Refresh, 3000, false);
		}
		return 7000;
	}



	static const int MOON_AB_MS = 15000;
	int MoonAB(PlayerBase p, int step)
	{
		vector cam = Vector(7648.7, 26.7, 8807.6);
		vector look = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 4.0);
		vector stand = Vector(7648.7, 25.1, 8807.6);
		if (step == 323)
		{
			stand = AZ_VampCryptA1.ExitPoint();
			cam = stand + Vector(0, 1.7, 0);
			look = cam + Vector(0, 25, -30);
		}

		vector fwd = look - cam;
		fwd[1] = 0;
		fwd.Normalize();
		stand = stand + Vector(fwd[2], 0, -fwd[0]) * 2.0;
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);

		array<string> vars = new array<string>;
		vars.Insert("A");
		vars.Insert("B");
		vars.Insert("C");
		if (step == 322)
		{
			vars.Insert("E");
			vars.Insert("F");
		}

		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition3D(cam, 9, near, null);
		string names = "";
		foreach (Object o : near)
		{
			if (o && !o.IsInherited(PlayerBase))
				names = names + " " + o.GetType() + "@" + vector.Distance(o.GetPosition(), cam).ToString();
		}
		Print("[AZ_Vamp] TEST moon A/B objects within 9 m of " + label + ":" + names);
		int nv = vars.Count();
		for (int i = 0; i < nv; i++)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MoonVariant, i * MOON_AB_MS, false, p, vars[i]);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, i * MOON_AB_MS + 9000, false, p, label + "_" + vars[i], cam, look);
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MoonVariant, nv * MOON_AB_MS, false, p, "off");
		return nv * MOON_AB_MS + 1000;
	}


	void MoonVariant(PlayerBase p, string v)
	{
		World w = GetGame().GetWorld();
		bool moon = v == "A" || v == "C" || v == "F";
		bool fullDate = v == "A" || v == "C" || v == "E";
		AZ_VampProgress.s_BloodMoon = moon;
		if (fullDate)
		{
			w.SetDate(2011, 11, 5, 23, 30);
			AZ_VampProgress.FullMoonDate();
		}
		else
			w.SetDate(2011, 11, 20, 23, 30);
		if (p)
		{
			p.m_AZBloodMoon = moon;
			p.SetSynchDirty();
			string band = "A";
			if (v == "C")
				band = "T";
			AZ_VampBand.Send(p, band);
		}
		Print("[AZ_Vamp] TEST moon A/B variant " + v + " blood moon " + moon + " full-moon date " + fullDate);
	}




	void GalleryShot(PlayerBase p, int step)
	{
		vector stand, cam, look;
		if (step == 310)
		{
			stand = AZ_VampCrypt.Hall(1.2, -11.6, 0.1);
			cam = AZ_VampCrypt.Hall(1.4, -11.6, 1.55);
			look = AZ_VampCrypt.Hall(-12.0, -11.6, 0.8);
		}
		else if (step == 311)
		{
			stand = AZ_VampCrypt.Nave(9.5, -6.6, 0.1);
			cam = AZ_VampCrypt.Nave(9.2, -6.4, 1.6);
			look = AZ_VampCrypt.Nave(6.0, -9.5, 1.0);
		}
		else if (step == 315)
		{
			if (AZ_VampCryptGallery.s_Slab && AZ_VampCryptGallery.s_Slab.IsOpen())
				AZ_VampCryptGallery.s_Slab.SetOpen(false);
			stand = AZ_VampCrypt.Hall(-14.0, -9.6, 0.1);
			cam = AZ_VampCrypt.Hall(-14.0, -9.9, 1.6);
			look = AZ_VampCrypt.Hall(-14.0, -6.3, 1.1);
		}
		else if (step == 314)
		{
			stand = AZ_VampCrypt.Nave(32.4, -11.2, 0.1);
			cam = AZ_VampCrypt.Nave(32.0, -11.4, 1.6);
			look = AZ_VampCrypt.Nave(40.2, -12.4, 1.6);
		}
		else if (step == 313)
		{
			stand = AZ_VampCrypt.Nave(22.2, 11.3, 0.1);
			cam = AZ_VampCrypt.Nave(22.0, 11.2, 1.5);
			look = AZ_VampCrypt.Nave(27.5, 12.8, 0.5);
		}
		else
		{
			if (AZ_VampCryptGallery.s_Slab)
				AZ_VampCryptGallery.s_Slab.OpenFor(30000);
			stand = AZ_VampCrypt.Hall(-16.8, -2.6, 0.1);
			cam = AZ_VampCrypt.Hall(-16.6, -3.0, 1.6);
			look = AZ_VampCrypt.Hall(-14.0, -8.5, 1.0);
		}


		vector back = look - cam;
		back[1] = 0;
		back.Normalize();
		float sy = stand[1];
		stand = cam + Vector(back[2], 0, -back[0]) * 0.6;
		stand[1] = sy;
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand + " slab open " + AZ_VampCryptGallery.SlabOpen());
		int settle = 4000;
		if (step == 313 || step == 314)
			settle = 10000;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, settle, false, p, label, cam, look);
	}






	protected bool m_AZCutVamp;

	int CutRun(PlayerBase p)
	{
		AZ_VampRitualSite site = null;
		foreach (AZ_VampRitualSite rs : AZ_VampRitual.s_Sites)
		{
			if (rs && rs.m_InCrypt)
				site = rs;
		}
		if (!site)
		{
			Print("[AZ_Vamp] TEST ritual_cut: no crypt altar site");
			return 100;
		}
		MoonVariant(p, "off");
		m_AZCutVamp = p.m_AZVampire;
		p.m_AZVampire = false;
		vector side = site.m_Center + Vector(1.0, 0.05, 0);
		p.SetPosition(side);
		vector face = site.m_Center - side;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		foreach (AZ_VampRitualCandle c : site.m_Candles)
		{
			if (c && !c.AZ_IsLit())
				AZ_VampRitual.CandleLit(c, null);
		}
		if (p.GetItemInHands())
			p.GetHumanInventory().LocalDestroyEntity(p.GetItemInHands());
		EntityAI knife = p.GetHumanInventory().CreateInHands("KitchenKnife");
		Print("[AZ_Vamp] TEST ritual_cut: lit " + AZ_VampRitual.Lit(site) + "/" + site.m_Candles.Count() + " step " + AZ_VampRitual.s_State + " knife " + knife + " event " + AZ_VampEvent.Active() + " night " + GetGame().GetWorld().IsNight());

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 2500, false, p, "ritual_sockets", site.m_Center + Vector(0, 3.2, 0) - site.m_Back * 2.6, site.m_Center);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CutNow, 3000, false, p);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 5000, false, p, "ritual_cut", site.m_Center + Vector(-1.4, 1.6, 1.2), side + Vector(0, 0.9, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 13000, false, p, "ritual_drip", site.m_Center + Vector(-1.4, 1.6, 1.2), side + Vector(-0.4, 0.2, 0));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CutEnd, 26000, false, p);
		return 30000;
	}

	void CutNow(PlayerBase p)
	{
		if (!p)
			return;
		float blood = p.GetHealth("", "Blood");
		AZ_VampRitual.Pour(p);
		Print("[AZ_Vamp] TEST ritual_cut: blood " + blood + " -> " + p.GetHealth("", "Blood") + " bleeding bits " + p.GetBleedingBits() + " step " + AZ_VampRitual.s_State);
	}

	void CutEnd(PlayerBase p)
	{
		if (p)
			p.m_AZVampire = m_AZCutVamp;
		if (p)
			Print("[AZ_Vamp] TEST ritual_cut: after 23 s bleeding bits " + p.GetBleedingBits() + " blood " + p.GetHealth("", "Blood"));
		AZ_VampRitual.Abort("test ritual_cut done");
	}

	int GuideRun(PlayerBase p)
	{
		AZ_VampRitualSite site = null;
		foreach (AZ_VampRitualSite rs : AZ_VampRitual.s_Sites)
		{
			if (rs && rs.m_InCrypt)
				site = rs;
		}
		if (!site)
		{
			Print("[AZ_Vamp] TEST countess_guide: no crypt altar site");
			return 100;
		}
		vector side = site.m_Center + Vector(1.6, 0.05, 0);
		p.SetPosition(side);
		Print("[AZ_Vamp] TEST countess_guide: beside the dark altar at " + side + " night " + GetGame().GetWorld().IsNight());
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GuideLie, 8000, false, p, site.m_Lie, true);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GuideLie, 34000, false, p, site.m_Lie, false);
		return 36000;
	}

	void GuideLie(PlayerBase p, vector lie, bool on)
	{
		if (!p)
			return;
		if (on)
			p.SetPosition(lie);
		p.m_AZOnAltar = on;
		p.SetSynchDirty();
		Print("[AZ_Vamp] TEST countess_guide: on the altar " + on);
	}

	void FrescoShot(PlayerBase p, int step)
	{
		vector stand = AZ_VampCrypt.Nave(39.6, -3.0, 0.1);
		vector cam = AZ_VampCrypt.Nave(39.8, -2.8, 1.7);
		vector look = AZ_VampCrypt.Nave(42.0, -8.0, 4.4);
		if (step == 318)
		{
			stand = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 3.0, 2.6, AZ_VampCrypt.DAIS_Y + 0.1);
			cam = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 3.2, 2.4, AZ_VampCrypt.DAIS_Y + 1.7);
			look = AZ_VampCrypt.Nave(AZ_VampCrypt.APSE_F + 6.6, -8.0, 3.5);
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, label, cam, look);
	}



	static const string BAND_VARS = "ABCDEFGIJH";
	static const int BAND_SETTLE_MS = 4000;
	static const int BAND_FRAME_MS = 6000;
	static const int BAND_FRAMES = 4;

	int BandRun(PlayerBase p)
	{
		int per = BAND_SETTLE_MS + BAND_FRAMES * BAND_FRAME_MS;
		for (int v = 0; v < BAND_VARS.Length(); v++)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(BandVariant, v * per, false, p, BAND_VARS.Get(v));
		return BAND_VARS.Length() * per + 2000;
	}

	void BandVariant(PlayerBase p, string v)
	{
		if (!p)
			return;
		vector stand = "7648.7 25.0 8807.6";
		vector aim = AZ_VampCrypt.Nave(AZ_VampCrypt.CountessF(), 0, 3.6);
		float sweep = 15;
		if (v == "H")
		{
			vector top = "4987.5 31.5 6618.5";
			vector hit, nrm;
			int comp;
			if (DayZPhysics.RaycastRV(top, top - "0 20 0", hit, nrm, comp, null, null, p, false, false, ObjIntersectGeom))
				stand = hit + "0 0.05 0";
			else
				stand = top;
			aim = stand + "10 0.5 0";
			sweep = 72;
		}
		p.SetPosition(stand);
		vector face = aim - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		AZ_VampBand.Send(p, v);
		Print("[AZ_Vamp] TEST band " + v + " at " + stand + " inside " + AZ_VampCrypt.Inside(stand));
		for (int k = 0; k < BAND_FRAMES; k++)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(BandFrame, BAND_SETTLE_MS + k * BAND_FRAME_MS, false, p, v, k, stand, aim, sweep);
	}

	void BandFrame(PlayerBase p, string v, int k, vector stand, vector aim, float sweep)
	{
		if (!p)
			return;
		vector cam = stand + "0 1.6 0";
		vector d = aim - cam;
		float a = (k - 2) * sweep * Math.DEG2RAD;
		vector look = cam + Vector(d[0] * Math.Cos(a) - d[2] * Math.Sin(a), d[1], d[0] * Math.Sin(a) + d[2] * Math.Cos(a));
		HwShot(p, "band_" + v + "_" + k, cam, look);
		if (v == "H" && k == BAND_FRAMES - 1)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampBand.Send, 5500, false, p, "");
	}


	void SealShot(PlayerBase p, int step)
	{
		float sr = -4.5;
		vector look = AZ_VampCrypt.Nave(25.0, -8.0, 9.0);
		if (step == 156)
		{
			sr = 4.5;
			look = AZ_VampCrypt.Nave(25.0, 8.0, 9.0);
		}
		float sf = 9.0;
		if (step == 157)
		{
			sf = 33.0;
			look = AZ_VampCrypt.Nave(43.0, 0.0, 14.0);
		}
		vector stand = AZ_VampCrypt.Nave(sf, 0, -0.9);
		vector cam = AZ_VampCrypt.Nave(sf, sr * 0.3, 0.7);
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, label, cam, look);
	}

	void AnteShot(PlayerBase p, int step)
	{
		vector stand = AZ_VampCrypt.Hall(-15.0, 4.8, 0.1);
		vector cam = AZ_VampCrypt.Hall(-16.0, 4.2, 1.7);
		vector look = AZ_VampCrypt.Hall(-20.4, -1.0, 1.6);
		if (step == 151)
		{
			stand = AZ_VampCrypt.Hall(-12.4, -3.4, 0.1);
			cam = AZ_VampCrypt.Hall(-13.2, -4.6, 1.7);
			look = AZ_VampCrypt.Hall(-5.0, -0.5, 1.0);
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " ante " + AZ_VampCrypt.AnteOn() + " at " + stand);
		int wait = 4000;
		if (step == 151)
			wait = 10000;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, wait, false, p, label, cam, look);
		if (step != 150)
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 10500, false, p, "crypt_ante_gate", AZ_VampCrypt.Hall(-20.3, 0, 1.65), AZ_VampCrypt.Hall(-16.1, 0, 1.1));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 17000, false, p, "crypt_ante_arrival", AZ_VampCrypt.Hall(-19.2, -4.0, 1.65), AZ_VampCrypt.Hall(-15.5, 4.5, 1.2));
	}


	void DeepShot(PlayerBase p, int step)
	{
		vector stand;
		vector cam;
		vector look;
		if (step == 152)
		{
			AZ_VampCryptA3.s_TestOpen = true;
			AZ_VampCryptA3.Refresh();
			stand = AZ_VampCrypt.Nave(AZ_VampCryptA3.DEEP_F - 3.5, AZ_VampCryptA3.DEEP_R + 0.6, 0.1);
			cam = AZ_VampCrypt.Nave(AZ_VampCryptA3.DEEP_F - 2.6, AZ_VampCryptA3.DEEP_R + 0.3, 1.7);
			look = AZ_VampCryptA3.DeepPos();
		}
		else
		{
			stand = AZ_VampCryptA3.ExitPoint();
			cam = AZ_VampCryptA3.ExitPoint() + AZ_VampCryptA3.s_ExitDir * 4.0;
			cam[1] = GetGame().SurfaceY(cam[0], cam[2]) + 1.9;
			look = AZ_VampCryptA3.ExitPoint() - AZ_VampCryptA3.s_ExitDir * 2.0;
			AZ_VampCryptA3.s_TestOpen = false;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZ_VampCryptA3.Refresh, 3000, false);
		}
		p.SetPosition(stand);
		vector face = look - stand;
		face[1] = 0;
		p.SetDirection(face.Normalized());
		string label = HwCryptLabel(step);
		Print("[AZ_Vamp] TEST crypt " + label + " at " + stand + " deep open " + AZ_VampCryptA3.Open() + " at deep " + AZ_VampCryptA3.AtDeep(stand));
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HwShot, 4000, false, p, label, cam, look);
	}
}
