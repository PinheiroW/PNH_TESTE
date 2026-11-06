

class AZ_VampCryptTunnelReward
{
	static const int MAX_ATTEMPTS = 3;
	static const int RETRY_MS = 1000;
	static const int ROOT_COUNT = 7;
	static const int CHILD_COUNT = 6;
	static const string GUN_SPEC = "FAL|Mag_FAL_20Rnd|ACOGOptic+Fal_OeBttstck|ImprovisedSuppressor|AmmoBox_308Win_20Rnd";
	static bool s_Claimed;
	static bool s_Complete;
	static bool s_Preserved;
	static bool s_Failed;
	static int s_Attempts;
	static EntityAI s_Chest;
	static bool s_ChestMade;
	static ref array<EntityAI> s_Roots;
	static ref array<bool> s_RootMade;
	static ref array<EntityAI> s_Children;
	static ref array<EntityAI> s_ChildParents;
	static ref array<bool> s_ChildMade;

	static vector Position()
	{
		return AZ_VampCrypt.Nave(25.4, 10.3, 0.05);
	}


	static void Reset()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(TryFill);
		s_Claimed = false;
		s_Complete = false;
		s_Preserved = false;
		s_Failed = false;
		s_Attempts = 0;
		s_Chest = null;
		s_ChestMade = false;
		s_Roots = new array<EntityAI>;
		s_RootMade = new array<bool>;
		for (int ri = 0; ri < ROOT_COUNT; ri++)
		{
			s_Roots.Insert(null);
			s_RootMade.Insert(false);
		}
		s_Children = new array<EntityAI>;
		s_ChildParents = new array<EntityAI>;
		s_ChildMade = new array<bool>;
		for (int ci = 0; ci < CHILD_COUNT; ci++)
		{
			s_Children.Insert(null);
			s_ChildParents.Insert(null);
			s_ChildMade.Insert(false);
		}
	}

	static void Shutdown()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(TryFill);
	}


	static void Claim(PlayerBase player)
	{
		if (!GetGame().IsServer() || s_Claimed || !AZ_VampCryptTunnel.s_Cleared)
			return;
		s_Claimed = true;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!cfg || !cfg.cryptLoot)
		{
			Print("[AZ_Vamp] tunnel reward: disabled by cryptLoot");
			return;
		}
		if (!s_Roots)
		{

			Reset();
			s_Claimed = true;
		}
		array<Object> near = new array<Object>;
		GetGame().GetObjectsAtPosition(Position(), 1.25, near, null);
		int count;
		foreach (Object object : near)
		{
			if (!object || object.GetType() != "SeaChest")
				continue;
			count++;
			s_Chest = EntityAI.Cast(object);
		}
		if (count > 1)
		{
			s_Failed = true;
			s_Chest = null;
			Print("[AZ_Vamp] tunnel reward: multiple existing chests; preserved, no additional reward");
			return;
		}
		if (s_Chest)
		{
			s_ChestMade = true;
			GameInventory existing = s_Chest.GetInventory();
			if (!existing)
			{
				s_Failed = true;
				Print("[AZ_Vamp] tunnel reward: existing chest has no inventory; preserved");
				return;
			}
			if (existing.CountInventory() > 1)
			{
				s_Preserved = true;
				Print("[AZ_Vamp] tunnel reward: existing cargo preserved, no refill this boot");
				return;
			}
		}
		TryFill();
	}

	static EntityAI FindAttached(EntityAI holder, string classname)
	{
		if (!holder || !holder.GetInventory())
			return null;
		GameInventory inventory = holder.GetInventory();
		for (int i = 0; i < inventory.AttachmentCount(); i++)
		{
			EntityAI attached = inventory.GetAttachmentFromIndex(i);
			if (attached && attached.GetType() == classname)
				return attached;
		}
		return null;
	}

	static EntityAI Root(int index, string classname)
	{
		if (s_Roots[index])
			return s_Roots[index];
		EntityAI item = s_Chest.GetInventory().CreateInInventory(classname);
		if (item)
		{
			s_Roots[index] = item;
			s_RootMade[index] = true;
		}
		return item;
	}

	static EntityAI Child(int index, EntityAI holder, string classname)
	{
		if (!holder || !holder.GetInventory())
			return null;
		EntityAI item = s_Children[index];
		if (!item)
			item = FindAttached(holder, classname);
		if (!item)
			item = holder.GetInventory().CreateAttachment(classname);
		if (item)
		{
			s_Children[index] = item;
			s_ChildParents[index] = holder;
			s_ChildMade[index] = true;
		}
		return item;
	}


	static bool Retained()
	{
		for (int ri = 0; ri < ROOT_COUNT; ri++)
		{
			if (s_RootMade[ri] && (!s_Roots[ri] || s_Roots[ri].GetHierarchyParent() != s_Chest))
				return false;
		}
		for (int ci = 0; ci < CHILD_COUNT; ci++)
		{
			if (s_ChildMade[ci] && (!s_Children[ci] || s_Children[ci].GetHierarchyParent() != s_ChildParents[ci]))
				return false;
		}
		return true;
	}

	static void TryFill()
	{
		if (!GetGame().IsServer() || !s_Claimed || s_Complete || s_Preserved || s_Failed)
			return;
		AZ_VampConfig cfg = AZ_VampState.Cfg();
		if (!cfg || !cfg.cryptLoot)
			return;
		s_Attempts++;
		if (!s_Chest && !s_ChestMade)
		{
			s_Chest = EntityAI.Cast(GetGame().CreateObjectEx("SeaChest", Position(), ECE_KEEPHEIGHT | ECE_CREATEPHYSICS | ECE_DYNAMIC_PERSISTENCY));
			if (s_Chest)
			{
				s_ChestMade = true;
				s_Chest.SetOrientation(Vector(AZ_VampCrypt.HALL_YAW, 0, 0));
				AZ_VampLootPlace.Settle(s_Chest, "dig tomb clearance reward");
			}
		}
		if (!s_Chest || !s_Chest.GetInventory())
		{
			if (s_ChestMade)
			{
				s_Failed = true;
				Print("[AZ_Vamp] tunnel reward: distributed chest disappeared; no replacement");
				return;
			}
			RetryOrStop("chest unavailable");
			return;
		}
		if (!Retained())
		{
			s_Failed = true;
			Print("[AZ_Vamp] tunnel reward: distributed inventory changed during fill; no replacement");
			return;
		}
		Weapon_Base gun = Weapon_Base.Cast(Root(0, "FAL"));
		EntityAI optic = Child(0, gun, "ACOGOptic");
		EntityAI stock = Child(1, gun, "Fal_OeBttstck");
		EntityAI suppressor = Child(2, gun, "ImprovisedSuppressor");
		if (gun && !s_Children[3])
		{
			Magazine magazine = Magazine.Cast(FindAttached(gun, "Mag_FAL_20Rnd"));
			if (!magazine)
				magazine = gun.SpawnAttachedMagazine("Mag_FAL_20Rnd");
			if (magazine)
			{
				s_Children[3] = magazine;
				s_ChildParents[3] = gun;
				s_ChildMade[3] = true;
			}
		}
		EntityAI ammo1 = Root(1, "AmmoBox_308Win_20Rnd");
		EntityAI ammo2 = Root(2, "AmmoBox_308Win_20Rnd");
		EntityAI headset = Root(3, "NVGHeadstrap");
		EntityAI goggles = Child(4, headset, "NVGoggles");
		EntityAI battery = Child(5, goggles, "Battery9V");
		EntityAI saline = Root(4, "SalineBagIV");
		EntityAI bandage1 = Root(5, "BandageDressing");
		EntityAI bandage2 = Root(6, "BandageDressing");
		s_Complete = gun && optic && stock && suppressor && s_Children[3] && ammo1 && ammo2 && headset && goggles && battery && saline && bandage1 && bandage2;
		if (s_Complete)
		{

			AZ_VampCryptLoot.Wear(gun);
			AZ_VampCryptLoot.Wear(optic);
			AZ_VampCryptLoot.Wear(stock);
			AZ_VampCryptLoot.Wear(suppressor);
			Print("[AZ_Vamp] tunnel reward: guaranteed FAL/NVG/medical kit ready, two .308 boxes; one clearance reward this boot");
			return;
		}
		RetryOrStop("incomplete kit");
	}

	static void RetryOrStop(string reason)
	{
		if (s_Attempts >= MAX_ATTEMPTS)
		{
			s_Failed = true;
			Print("[AZ_Vamp] tunnel reward: " + reason + " after three attempts; no reroll/refill");
			return;
		}
		Print("[AZ_Vamp] tunnel reward: " + reason + ", retry " + (s_Attempts + 1));
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TryFill, RETRY_MS, false);
	}
}
