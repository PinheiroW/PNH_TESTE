



modded class MissionServer
{
	static bool s_AZStakeShotsDone;
	static bool s_AZSilverShotsDone;

	override void HwStep(PlayerBase p, int step)
	{
		if (step == 15 && !s_AZStakeShotsDone && s_HwOnly && (s_HwOnly.Find("hw_stake") >= 0 || s_HwOnly.Find("hw_stake_ground") >= 0))
		{
			s_AZStakeShotsDone = true;
			AZStakeStep(p, 0);
			return;
		}
		if (step == 15 && !s_AZSilverShotsDone && s_HwOnly && s_HwOnly.Find("hw_silver") >= 0)
		{
			s_AZSilverShotsDone = true;
			AZSilverStep(p, 0);
			return;
		}
		super.HwStep(p, step);
	}

	void AZStakeStep(PlayerBase p, int i)
	{
		if (!p)
			return;
		vector at = p.GetPosition();
		switch (i)
		{
			case 0:
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				vector sk = Vector(7050, 0, 11122);
				HwGo(p, sk, sk + Vector(10, 0, 0));
				EntityAI skh = p.GetHumanInventory().CreateInHands("AZ_VampStake");
				vector skg = sk + Vector(1.6, 0, 1.2);
				skg[1] = GetGame().SurfaceY(skg[0], skg[2]);
				Object sko = GetGame().CreateObjectEx("AZ_VampStake", skg + Vector(0, 0.05, 0), ECE_PLACE_ON_SURFACE);
				Print("[AZ_Vamp] TEST stake in hands " + skh + " ground " + sko + " stake check " + PlayerBase.AZ_VampIsStake(skh));
				break;
			case 1:
				HwShot(p, "hw_stake", at + Vector(1.3, 1.4, -1.1), at + Vector(0.2, 0.95, -0.25));
				break;
			case 2:
				vector sg = at + Vector(1.6, 0, 1.2);
				sg[1] = GetGame().SurfaceY(sg[0], sg[2]);
				HwShot(p, "hw_stake_ground", sg + Vector(0.25, 0.55, -0.45), sg + Vector(0, 0.03, 0.1));
				break;
			default:
				HwStep(p, 15);
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZStakeStep, 7000, false, p, i + 1);
	}





	static PlayerBase s_AZSilverShooter;
	static float s_AZSilverTook;
	void AZSilverStep(PlayerBase p, int i)
	{
		if (!p)
			return;
		vector at = p.GetPosition();
		switch (i)
		{
			case 0:
				GetGame().GetWorld().SetDate(2011, 11, 5, 12, 0);
				vector sv = Vector(7050, 0, 11122);
				HwGo(p, sv, sv + Vector(10, 0, 0));
				vector sh = sv + Vector(6, 0, 0);
				sh[1] = GetGame().SurfaceY(sh[0], sh[2]);
				s_AZSilverShooter = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Boris", sh, false, true));
				vector sgr = sv + Vector(1.6, 0, 1.2);
				sgr[1] = GetGame().SurfaceY(sgr[0], sgr[2]);
				GetGame().CreateObjectEx("AZ_VampAmmoBox_357Silver_10Rnd", sgr + Vector(0, 0.05, 0), ECE_PLACE_ON_SURFACE);
				GetGame().CreateObjectEx("AZ_VampAmmo_357Silver", sgr + Vector(0.18, 0.05, 0.12), ECE_PLACE_ON_SURFACE);
				GetGame().CreateObjectEx("Ammo_357", sgr + Vector(-0.18, 0.05, 0.12), ECE_PLACE_ON_SURFACE);
				break;
			case 1:
				Print("[AZ_Vamp] TEST silver ammo cfg " + GetGame().ConfigIsExisting("CfgAmmo AZ_VampBullet_357Silver") + " health dmg " + GetGame().ConfigGetFloat("CfgAmmo AZ_VampBullet_357Silver DamageApplied Health damage"));
				s_AZSilverTook = AZSilverHit(p, "AZ_VampBullet_357Silver");
				break;
			case 2:
				float n0 = AZSilverHit(p, "Bullet_357");
				Print("[AZ_Vamp] TEST silver damage: normal .357 took " + n0 + " health, silver took " + s_AZSilverTook + " (vampire " + p.m_AZVampire + ", night " + GetGame().GetWorld().IsNight() + ") " + AZVerdict(s_AZSilverTook > n0 * 1.2));
				p.SetHealth("", "Health", 100);
				AZSilverGuns(p);
				break;
			case 3:
				vector sgg = at + Vector(1.6, 0, 1.2);
				sgg[1] = GetGame().SurfaceY(sgg[0], sgg[2]);
				HwShot(p, "hw_silver", sgg + Vector(0.3, 0.55, -0.45), sgg + Vector(0, 0.03, 0.1));
				break;
			default:
				if (s_AZSilverShooter)
					GetGame().ObjectDelete(s_AZSilverShooter);
				HwStep(p, 15);
				return;
		}
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AZSilverStep, 7000, false, p, i + 1);
	}

	float AZSilverHit(PlayerBase p, string ammo)
	{
		p.SetHealth("", "Health", 100);
		p.SetHealth("", "Shock", 100);
		float h0 = p.GetHealth("", "Health");
		p.ProcessDirectDamage(DamageType.FIRE_ARM, s_AZSilverShooter, "Torso", ammo, "0 1.2 0");
		float h1 = p.GetHealth("", "Health");
		Print("[AZ_Vamp] TEST silver hit " + ammo + " torso health " + h0 + " -> " + h1);
		return h0 - h1;
	}

	string AZVerdict(bool ok)
	{
		if (ok)
			return "PASS";
		return "FAIL";
	}

	void AZSilverGuns(PlayerBase p)
	{
		vector gp = p.GetPosition() + Vector(0, 0, 4);
		gp[1] = GetGame().SurfaceY(gp[0], gp[2]);
		Magazine silver = Magazine.Cast(GetGame().CreateObjectEx("AZ_VampAmmo_357Silver", gp, ECE_PLACE_ON_SURFACE));
		array<string> guns = {"Magnum", "SawedoffMagnum", "Deagle", "Derringer_Black", "Repeater"};
		int ok = 0;
		int all = 0;
		foreach (string g : guns)
		{
			Weapon_Base w = Weapon_Base.Cast(GetGame().CreateObjectEx(g, gp, ECE_PLACE_ON_SURFACE));
			if (!w || !silver)
				continue;
			string res = "";
			for (int m = 0; m < w.GetMuzzleCount(); m++)
			{
				all++;
				bool can = w.CanChamberFromMag(m, silver);
				if (can)
					ok++;
				res += " " + m + ":" + can;
			}
			Print("[AZ_Vamp] TEST silver chamber " + g + res);
			GetGame().ObjectDelete(w);
		}
		array<string> mags = {"Mag_357Speedloader_6Rnd", "Mag_Deagle_9rnd"};
		foreach (string mg : mags)
		{
			Magazine mag = Magazine.Cast(GetGame().CreateObjectEx(mg, gp, ECE_PLACE_ON_SURFACE));
			if (!mag)
				continue;
			bool fit = mag.IsCompatiableAmmo(ItemBase.Cast(silver));
			all++;
			if (fit)
				ok++;
			Print("[AZ_Vamp] TEST silver magazine " + mg + " takes it " + fit);
			GetGame().ObjectDelete(mag);
		}
		Print("[AZ_Vamp] TEST silver fits " + ok + "/" + all + " " + AZVerdict(ok == all && all > 0));
		if (silver)
			GetGame().ObjectDelete(silver);
	}
}
