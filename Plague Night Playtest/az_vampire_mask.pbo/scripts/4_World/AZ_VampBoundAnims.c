






modded class ModItemRegisterCallbacks
{
	override void RegisterRestrained(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterRestrained(pType, pBehavior);
		if (!GetGame().ConfigIsExisting("CfgPatches AZ_VampireAnims_Bound"))
			return;
		string asi = "azo/az_vampire_anims/player_vamp_restrained.asi";
		pType.AddItemInHandsProfileIK("HandcuffsLocked", asi, pBehavior, "dz/anims/anm/player/ik/gear/handcuffslocked.anm");
		pType.AddItemInHandsProfileIK("RopeLocked", asi, pBehavior, "dz/anims/anm/player/ik/gear/bp_rope_tied.anm");
		pType.AddItemInHandsProfileIK("DuctTapeLocked", asi, pBehavior, "dz/anims/anm/player/ik/gear/DuctTape_tied.anm");
		pType.AddItemInHandsProfileIK("MetalWireLocked", asi, pBehavior, "dz/anims/anm/player/ik/gear/Metalwire_tied.anm");
		pType.AddItemInHandsProfileIK("BarbedWireLocked", asi, pBehavior, "dz/anims/anm/player/ik/gear/Barbed_wire_tied.anm");
	}
}
