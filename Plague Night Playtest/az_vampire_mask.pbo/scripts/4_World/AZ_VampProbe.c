








class AZ_VampProbe
{
	static const int STEP_MS = 500;
	static bool s_HunterOk;
	static EntityAI s_Hunter0;
	static EntityAI s_Hunter1;
	static EntityAI s_HunterPartialItem;
	static EntityAI s_HunterDuplicate;
	static EntityAI s_HunterOrdinary;
	static EntityAI s_HunterMarker;
	static ref array<EntityAI> s_HunterFillers = new array<EntityAI>;
	static string s_HunterPartialFingerprint;
	static string s_HunterEmptyBefore;
	static string s_HunterOrdinaryBefore;

	static vector BonePos(PlayerBase p, string name)
	{
		int i = p.GetBoneIndexByName(name);
		if (i < 0)
			return p.GetPosition();
		return p.GetBonePositionWS(i);
	}

	static void Measure(PlayerBase vamp, PlayerBase victim, float t)
	{
		if (!vamp || !victim)
			return;
		vector d = victim.GetPosition() - vamp.GetPosition();
		d[1] = 0;
		vector eyes = (BonePos(vamp, "EyeLeft") + BonePos(vamp, "EyeRight")) * 0.5;
		vector vNeck = BonePos(victim, "Neck");
		int va = 0;
		int ta = 0;
		if (vamp.GetCommand_Action())
			va = 1;
		if (victim.GetCommand_Action())
			ta = 1;
		Print("[AZ_Vamp] probe t=" + t + " root=" + d.Length() + " head-neck=" + vector.Distance(BonePos(vamp, "Head"), vNeck) + " head-head=" + vector.Distance(BonePos(vamp, "Head"), BonePos(victim, "Head")) + " eyes-neck=" + vector.Distance(eyes, vNeck) + " vampAct=" + va + " victimAct=" + ta + " yawDiff=" + Math.AbsFloat(Math.DiffAngle(vamp.GetOrientation()[0], victim.GetOrientation()[0])));

		vector vHead = BonePos(victim, "Head");
		float g = victim.GetPosition()[1];
		Print("[AZ_Vamp] probe bones victim Neck z=" + (vNeck[1] - g) + " Head z=" + (vHead[1] - g) + " |Neck-Head|=" + vector.Distance(vNeck, vHead) + " neckIdx=" + victim.GetBoneIndexByName("Neck") + " vampire Head z=" + (BonePos(vamp, "Head")[1] - vamp.GetPosition()[1]) + AZ_ProbeSplit(BonePos(vamp, "Head"), vNeck) + " groundStep=" + (vamp.GetPosition()[1] - victim.GetPosition()[1]));
	}


	static string AZ_ProbeSplit(vector a, vector b)
	{
		vector d = b - a;
		float v = d[1];
		d[1] = 0;
		return " headNeckH=" + d.Length() + " headNeckV=" + v;
	}


	static vector Flattest(vector around)
	{
		vector best = around;
		float bestSlope = 1000;
		for (int ix = -3; ix <= 3; ix++)
		{
			for (int iz = -3; iz <= 3; iz++)
			{
				float x = around[0] + ix * 10;
				float z = around[2] + iz * 10;
				float y = GetGame().SurfaceY(x, z);
				float slope = Math.AbsFloat(GetGame().SurfaceY(x, z - 0.9) - y) + Math.AbsFloat(GetGame().SurfaceY(x + 0.5, z) - y) + Math.AbsFloat(GetGame().SurfaceY(x - 0.5, z) - y);
				if (slope < bestSlope && !GetGame().SurfaceIsSea(x, z) && !GetGame().SurfaceIsPond(x, z))
				{
					bestSlope = slope;
					best = Vector(x, y, z);
				}
			}
		}
		Print("[AZ_Vamp] probe: flattest spot " + best + " slope sum " + bestSlope);
		return best;
	}

	static void Run(vector at)
	{
		at = Flattest(at);
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		vector fwd = "0 0 1";
		PlayerBase victim = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at, false, true));
		PlayerBase vamp = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at - fwd * 0.9, false, true));
		if (!victim || !vamp)
		{
			Print("[AZ_Vamp] probe: spawn failed");
			return;
		}
		victim.SetDirection(-fwd);
		vamp.SetDirection(fwd);
		AZ_VampState.SetVampire(vamp, true);
		AZ_VampBind.Bind(vamp);
		vamp.SetHealth("", "Blood", AZ_VampState.Cfg().vampDrainFloor);
		Print("[AZ_Vamp] probe: spawned at " + at + ", feeding in 5 s");
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Start, 5000, false, vamp, victim);
		vector off = "15 0 0";
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Trophy, 8000, false, at + off);
		vector off2 = "-20 0 0";
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Trail, 9000, false, at + off2);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Chest, 10000, false);
	}



	static void Chest()
	{
		s_HunterOk = true;
		AZ_VampDrop.Startup();
		vector anchor0 = AZ_VampDrop.CachePosition(0);
		vector anchor1 = AZ_VampDrop.CachePosition(1);
		s_Hunter0 = AZ_VampDrop.CacheAt(0);
		s_Hunter1 = AZ_VampDrop.CacheAt(1);
		s_HunterOk = s_HunterOk && s_Hunter0 && s_Hunter1;
		if (!s_HunterOk)
		{
			HunterFinish();
			return;
		}
		s_HunterOk = s_HunterOk && Math.AbsFloat(s_Hunter0.GetPosition()[0] - anchor0[0]) < 0.01 && Math.AbsFloat(s_Hunter0.GetPosition()[2] - anchor0[2]) < 0.01;
		s_HunterOk = s_HunterOk && Math.AbsFloat(s_Hunter1.GetPosition()[0] - anchor1[0]) < 0.01 && Math.AbsFloat(s_Hunter1.GetPosition()[2] - anchor1[2]) < 0.01;
		s_HunterOk = s_HunterOk && HunterCargoOk(s_Hunter0) && HunterCargoOk(s_Hunter1);
		s_HunterOk = s_HunterOk && AZ_VampLootPlace.Audit().IndexOf("0 fail") >= 0;

		array<EntityAI> roots0 = new array<EntityAI>;
		array<EntityAI> roots1 = new array<EntityAI>;
		HunterRoots(s_Hunter0, roots0);
		HunterRoots(s_Hunter1, roots1);
		array<string> fingerprints0 = new array<string>;
		array<string> fingerprints1 = new array<string>;
		HunterFingerprints(roots0, fingerprints0);
		HunterFingerprints(roots1, fingerprints1);
		AZ_VampDrop.Startup();
		s_HunterOk = s_HunterOk && s_Hunter0 == AZ_VampDrop.CacheAt(0) && s_Hunter1 == AZ_VampDrop.CacheAt(1);
		s_HunterOk = s_HunterOk && HunterSameRoots(s_Hunter0, roots0, fingerprints0) && HunterSameRoots(s_Hunter1, roots1, fingerprints1);

		for (int partialDelete = roots0.Count() - 1; partialDelete >= 1; partialDelete--)
			GetGame().ObjectDelete(roots0[partialDelete]);
		s_HunterPartialItem = roots0[0];
		s_HunterPartialFingerprint = HunterFingerprint(s_HunterPartialItem);
		for (int emptyDelete = roots1.Count() - 1; emptyDelete >= 0; emptyDelete--)
			GetGame().ObjectDelete(roots1[emptyDelete]);
		s_HunterEmptyBefore = HunterFingerprint(s_Hunter1);
		AZ_VampDrop.ResetStartupLatchForProbe();
		AZ_VampDrop.Startup();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(HunterRestartCheck, 500, false);
	}

	static void HunterRestartCheck()
	{
		EntityAI partial = AZ_VampDrop.CacheAt(0);
		EntityAI refilled = AZ_VampDrop.CacheAt(1);
		array<EntityAI> partialRoots = new array<EntityAI>;
		HunterRoots(partial, partialRoots);
		s_HunterOk = s_HunterOk && partial == s_Hunter0 && refilled == s_Hunter1;
		s_HunterOk = s_HunterOk && partialRoots.Count() == 1 && partialRoots[0] == s_HunterPartialItem;
		s_HunterOk = s_HunterOk && HunterFingerprint(s_HunterPartialItem) == s_HunterPartialFingerprint;
		s_HunterOk = s_HunterOk && s_HunterEmptyBefore != HunterFingerprint(refilled) && HunterCargoOk(refilled);

		array<EntityAI> sameProcessRoots = new array<EntityAI>;
		HunterRoots(refilled, sameProcessRoots);
		array<string> sameProcessFingerprints = new array<string>;
		HunterFingerprints(sameProcessRoots, sameProcessFingerprints);
		AZ_VampDrop.Startup();
		s_HunterOk = s_HunterOk && HunterSameRoots(refilled, sameProcessRoots, sameProcessFingerprints);

		s_HunterFillers.Clear();
		for (int fillerIndex = 0; fillerIndex < 100; fillerIndex++)
		{
			EntityAI filler = partial.GetInventory().CreateInInventory("DuctTape");
			if (!filler)
				break;
			s_HunterFillers.Insert(filler);
		}
		vector duplicateAt = AZ_VampDrop.CachePosition(0) + Vector(0.5, 0, 0.5);
		s_HunterDuplicate = EntityAI.Cast(GetGame().CreateObjectEx("AZ_VampHunterChest", duplicateAt, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
		if (s_HunterDuplicate && s_HunterDuplicate.GetInventory())
			s_HunterMarker = s_HunterDuplicate.GetInventory().CreateInInventory("DuctTape");
		vector ordinaryAt = AZ_VampDrop.CachePosition(0) + Vector(-0.5, 0, 0.5);
		s_HunterOrdinary = EntityAI.Cast(GetGame().CreateObjectEx("SeaChest", ordinaryAt, ECE_KEEPHEIGHT | ECE_CREATEPHYSICS));
		if (s_HunterOrdinary && s_HunterOrdinary.GetInventory())
			s_HunterOrdinary.GetInventory().CreateInInventory("DuctTape");
		s_HunterOrdinaryBefore = HunterFingerprint(s_HunterOrdinary);
		s_HunterOk = s_HunterOk && s_HunterDuplicate && s_HunterMarker && s_HunterOrdinary;
		AZ_VampDrop.ResetStartupLatchForProbe();
		AZ_VampDrop.Startup();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(HunterDuplicateCheck, 500, false);
	}

	static void HunterDuplicateCheck()
	{
		vector at = AZ_VampDrop.CachePosition(0);
		s_HunterOk = s_HunterOk && HunterManagedCount(at) == 1;
		s_HunterOk = s_HunterOk && s_HunterOrdinary && HunterFingerprint(s_HunterOrdinary) == s_HunterOrdinaryBefore;
		s_HunterOk = s_HunterOk && HunterMarkerSurvived(AZ_VampDrop.CacheAt(0), s_HunterMarker, at);
		if (s_HunterMarker)
			GetGame().ObjectDelete(s_HunterMarker);
		foreach (EntityAI fillerToDelete : s_HunterFillers)
		{
			if (fillerToDelete)
				GetGame().ObjectDelete(fillerToDelete);
		}
		s_HunterFillers.Clear();
		if (s_HunterOrdinary)
			GetGame().ObjectDelete(s_HunterOrdinary);
		if (s_HunterDuplicate && s_HunterDuplicate != AZ_VampDrop.CacheAt(0))
			GetGame().ObjectDelete(s_HunterDuplicate);
		HunterFinish();
	}

	static void HunterFinish()
	{
		string status = AZ_VampDrop.Status();
		if (s_HunterOk)
			Print("[AZ_Vamp] hunter chest probe: PASS");
		else
			Print("[AZ_Vamp] hunter chest probe: FAIL | " + status);
	}

	static void HunterRoots(EntityAI chest, out array<EntityAI> roots)
	{
		roots.Clear();
		if (!chest || !chest.GetInventory())
			return;
		CargoBase cargo = chest.GetInventory().GetCargo();
		if (!cargo)
			return;
		for (int rootIndex = 0; rootIndex < cargo.GetItemCount(); rootIndex++)
		{
			EntityAI rootItem = cargo.GetItem(rootIndex);
			if (rootItem)
				roots.Insert(rootItem);
		}
	}

	static void HunterFingerprints(array<EntityAI> roots, out array<string> fingerprints)
	{
		fingerprints.Clear();
		foreach (EntityAI fingerprintRoot : roots)
			fingerprints.Insert(HunterFingerprint(fingerprintRoot));
	}

	static bool HunterSameRoots(EntityAI chest, array<EntityAI> beforeRoots, array<string> beforeFingerprints)
	{
		array<EntityAI> afterRoots = new array<EntityAI>;
		HunterRoots(chest, afterRoots);
		if (afterRoots.Count() != beforeRoots.Count() || afterRoots.Count() != beforeFingerprints.Count())
			return false;
		for (int compareIndex = 0; compareIndex < afterRoots.Count(); compareIndex++)
		{
			if (afterRoots[compareIndex] != beforeRoots[compareIndex] || HunterFingerprint(afterRoots[compareIndex]) != beforeFingerprints[compareIndex])
				return false;
		}
		return true;
	}

	static string HunterFingerprint(EntityAI item)
	{
		if (!item)
			return "null";
		string fingerprint = item.GetType() + "|h=" + item.GetHealth("", "");
		if (item.HasQuantity())
			fingerprint += "|q=" + item.GetQuantity();
		Magazine magazineItem = Magazine.Cast(item);
		if (magazineItem)
			fingerprint += "|a=" + magazineItem.GetAmmoCount();
		GameInventory itemInventory = item.GetInventory();
		if (!itemInventory)
			return fingerprint;
		for (int attachmentIndex = 0; attachmentIndex < itemInventory.AttachmentCount(); attachmentIndex++)
			fingerprint += "|att{" + HunterFingerprint(itemInventory.GetAttachmentFromIndex(attachmentIndex)) + "}";
		CargoBase childCargo = itemInventory.GetCargo();
		if (childCargo)
		{
			for (int cargoIndex = 0; cargoIndex < childCargo.GetItemCount(); cargoIndex++)
				fingerprint += "|cargo{" + HunterFingerprint(childCargo.GetItem(cargoIndex)) + "}";
		}
		return fingerprint;
	}

	static bool HunterCargoOk(EntityAI chest)
	{
		array<EntityAI> cargoRoots = new array<EntityAI>;
		HunterRoots(chest, cargoRoots);
		int n = cargoRoots.Count();
		int weapon = 0;
		int silver = 0;
		int stake = 0;
		int holy = 0;
		int knife = 0;
		int bandage = 0;
		int practical = 0;
		foreach (EntityAI cargoItem : cargoRoots)
		{
			string cargoType = cargoItem.GetType();
			if (cargoType == "Repeater" || cargoType == "Magnum") weapon++;
			else if (cargoType == "AZ_VampAmmoBox_357Silver_10Rnd") silver++;
			else if (cargoType == "AZ_VampStake") stake++;
			else if (cargoType == "AZ_VampHolyWater") holy++;
			else if (cargoType == "HuntingKnife") knife++;
			else if (cargoType == "BandageDressing") bandage++;
			else if (cargoType == "Binoculars" || cargoType == "Canteen") practical++;
			else return false;
		}
		return n == 8 && weapon == 1 && silver == 2 && stake == 1 && holy == 1 && knife == 1 && bandage == 1 && practical == 1;
	}

	static int HunterManagedCount(vector at)
	{
		array<Object> nearby = new array<Object>;
		GetGame().GetObjectsAtPosition(at, 1.5, nearby, null);
		int managed = 0;
		foreach (Object nearbyObject : nearby)
		{
			if (nearbyObject && nearbyObject.IsKindOf("AZ_VampHunterChest"))
				managed++;
		}
		return managed;
	}

	static bool HunterMarkerSurvived(EntityAI keep, EntityAI marker, vector at)
	{
		if (!marker)
			return false;
		if (marker.GetHierarchyParent() == null)
			return vector.Distance(marker.GetPosition(), at) <= 3.0;
		return false;
	}



	static void Trail(vector at)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		PlayerBase v = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at, false, true));
		if (!v)
		{
			Print("[AZ_Vamp] trail probe: spawn failed");
			return;
		}
		AZ_VampState.SetVampire(v, true);
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		int start = AZ_VampTrail.s_Drops.Count();
		AZ_VampTrail.Start(v);
		int now = GetGame().GetTime();
		vector pos;
		for (int i = 1; i <= 5; i++)
		{
			pos = at + Vector(0, 0, i * 6);
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			v.SetPosition(pos);
			AZ_VampTrail.Tick(v, now + i);
		}
		int drops = AZ_VampTrail.s_Drops.Count() - start;
		v.m_AZScorchAt = 0;
		for (int k = 1; k <= 3; k++)
		{
			pos = at + Vector(8, 0, k * 6);
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			v.SetPosition(pos);
			AZ_VampTrail.Scorch(v, now + k * (cfg.scorchStepSec * 1000 + 1));
		}
		int scorch = AZ_VampTrail.s_Drops.Count() - start - drops;
		float lift = 0;
		int n = AZ_VampTrail.s_Drops.Count();
		if (n > 0)
		{
			Object last = AZ_VampTrail.s_Drops[n - 1];
			vector lp = last.GetPosition();
			lift = lp[1] - GetGame().SurfaceY(lp[0], lp[2]);
		}
		Print("[AZ_Vamp] trail probe: blood drops " + drops + "/5, scorch " + scorch + "/3, last lift " + lift + " m, total " + n + " cap " + cfg.trailCap);
		GetGame().ObjectDelete(v);
	}



	static void Trophy(vector at)
	{
		at[1] = GetGame().SurfaceY(at[0], at[2]);
		vector at2 = at + "4 0 0";
		at2[1] = GetGame().SurfaceY(at2[0], at2[2]);
		PlayerBase v1 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at, false, true));
		PlayerBase v2 = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at2, false, true));
		PlayerBase hunter = PlayerBase.Cast(GetGame().CreateObject("SurvivorM_Mirek", at2 + "0 0 3", false, true));
		if (!v1 || !v2 || !hunter)
		{
			Print("[AZ_Vamp] trophy probe: spawn failed");
			return;
		}
		AZ_VampState.SetVampire(v1, true);
		AZ_VampState.SetVampire(v2, true);
		AZ_VampRecord r = AZ_VampProgress.Rec(hunter);
		int before = 0;
		if (r)
			before = r.slain;

		v1.m_AZVampire = true;
		v2.m_AZVampire = true;
		v1.SetHealth("", "", 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampAsh.Crumble, 100, false, v2, false, hunter);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TrophyCheck, 4000, false, at, at2, hunter, before);
	}

	static void TrophyCheck(vector at, vector at2, PlayerBase hunter, int before)
	{
		Print("[AZ_Vamp] trophy probe: killed " + TrophyCount(at) + " | crumbled with hunter " + TrophyCount(at2));
		int after = -1;
		AZ_VampRecord r = AZ_VampProgress.Rec(hunter);
		if (r)
			after = r.slain;
		Print("[AZ_Vamp] trophy probe: hunter slain " + before + " -> " + after + " (-1 = an AI hunter has no record)");
		if (hunter)
			GetGame().ObjectDelete(hunter);
	}

	static string TrophyCount(vector at)
	{
		array<Object> objs = new array<Object>;
		GetGame().GetObjectsAtPosition(at, 1.5, objs, null);
		int ash = 0;
		int fangs = 0;
		int skull = 0;
		int bodies = 0;
		foreach (Object o : objs)
		{
			if (o.IsKindOf("AZ_VampAshPile"))
				ash++;
			else if (o.IsKindOf("AZ_VampFangs"))
				fangs++;
			else if (o.IsKindOf("AZ_VampSkull_Human"))
				skull++;
			else if (PlayerBase.Cast(o))
				bodies++;
		}
		return "ash " + ash + " fangs " + fangs + " skull " + skull + " bodies " + bodies;
	}

	static void Start(PlayerBase vamp, PlayerBase victim)
	{
		if (!vamp || !victim)
			return;
		Measure(vamp, victim, -1);
		ActionAZVampBite.Snap(vamp, victim);
		AZ_VampFeed.Hold(vamp, victim);
		AZ_VampFeedAnim.DummyFeedStart(vamp);
		Print("[AZ_Vamp] probe: feed started");
	}
}
