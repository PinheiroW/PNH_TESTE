



class AZ_VampPumpkinHead extends PumpkinHelmet
{
	override void UpdateGlowState()
	{
		super.UpdateGlowState();
		if (!IsDamageDestroyed() && ZombieBase.Cast(GetHierarchyParent()))
			SetPilotLight(true);
	}
}

modded class ZombieBase
{
	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsServer() || !AZ_VampEvent.Active())
			return;
		if (!AZ_VampState.Cfg().halloween)
			return;
		float share = AZ_VampState.Cfg().pumpkinInfected;
		if (share > 0 && Math.RandomFloat01() < share && !GetInventory().FindAttachment(InventorySlots.HEADGEAR))
			GetInventory().CreateAttachment("AZ_VampPumpkinHead");
		AZCostume();
	}




	static int s_AZCostumes;

	void AZCostume()
	{
		float share = AZ_VampState.Cfg().costumeInfected;
		if (share <= 0 || Math.RandomFloat01() >= share)
			return;
		if (!IsMale() && !GetInventory().FindAttachment(InventorySlots.HEADGEAR))
		{
			string hats[4] = {"WitchHat", "WitchHood_Black", "WitchHood_Red", "WitchHood_Brown"};
			GetInventory().CreateAttachment(hats[Math.RandomInt(0, 4)]);
		}
		else if (Math.RandomInt(0, 2) == 0)
			GetInventory().CreateAttachment("EyeMask_Dead");
		else
			GetInventory().CreateAttachment("CrookedNose");
		s_AZCostumes++;
		if (s_AZCostumes <= 5 || s_AZCostumes % 50 == 0)
			Print("[AZ_Vamp] costume infected " + s_AZCostumes + ": " + GetType());
	}
}
