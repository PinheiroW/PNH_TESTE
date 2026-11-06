




modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("AZ_VampStake", "dz/anims/workspaces/player/player_main/weapons/player_main_1h_pipe.asi", pBehavior, "dz/anims/anm/player/ik/gear/akina.anm");
	}
}



class AZ_VampCarveStake extends RecipeBase
{
	override void Init()
	{
		m_Name = "Carve a hunter's stake";
		m_IsInstaRecipe = false;
		m_AnimationLength = 1.5;
		m_Specialty = 0.02;

		m_MinDamageIngredient[0] = -1;
		m_MaxDamageIngredient[0] = 3;
		m_MinQuantityIngredient[0] = 1;
		m_MaxQuantityIngredient[0] = -1;
		m_MinDamageIngredient[1] = -1;
		m_MaxDamageIngredient[1] = 3;
		m_MinQuantityIngredient[1] = -1;
		m_MaxQuantityIngredient[1] = -1;

		InsertIngredient(0, "LongWoodenStick");
		InsertIngredient(0, "Firewood");
		m_IngredientAddHealth[0] = 0;
		m_IngredientSetHealth[0] = -1;
		m_IngredientAddQuantity[0] = -1;
		m_IngredientDestroy[0] = 0;
		m_IngredientUseSoftSkills[0] = false;


		InsertIngredient(1, "KitchenKnife");
		InsertIngredient(1, "SteakKnife");
		InsertIngredient(1, "HuntingKnife");
		InsertIngredient(1, "CombatKnife");
		InsertIngredient(1, "KukriKnife");
		InsertIngredient(1, "FangeKnife");
		InsertIngredient(1, "StoneKnife");
		InsertIngredient(1, "BoneKnife");
		InsertIngredient(1, "Cleaver");
		InsertIngredient(1, "Machete");
		InsertIngredient(1, "CrudeMachete");
		InsertIngredient(1, "OrientalMachete");
		InsertIngredient(1, "Jambiya");
		InsertIngredient(1, "Akinaka");
		InsertIngredient(1, "AK_Bayonet");
		InsertIngredient(1, "M9A1_Bayonet");
		InsertIngredient(1, "Mosin_Bayonet");
		InsertIngredient(1, "SKS_Bayonet");
		m_IngredientAddHealth[1] = -10;
		m_IngredientSetHealth[1] = -1;
		m_IngredientAddQuantity[1] = 0;
		m_IngredientDestroy[1] = 0;
		m_IngredientUseSoftSkills[1] = true;

		AddResult("AZ_VampStake");
		m_ResultSetFullQuantity[0] = false;
		m_ResultSetQuantity[0] = -1;
		m_ResultSetHealth[0] = -1;
		m_ResultInheritsHealth[0] = -1;
		m_ResultInheritsColor[0] = -1;
		m_ResultToInventory[0] = -2;
		m_ResultUseSoftSkills[0] = false;
		m_ResultReplacesIngredient[0] = -1;
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		return true;
	}
}

modded class PluginRecipesManagerBase
{
	override void RegisterRecipies()
	{
		super.RegisterRecipies();
		RegisterRecipe(new AZ_VampCarveStake);
	}
}
