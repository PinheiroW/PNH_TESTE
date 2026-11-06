



class AZ_VampNightLighting
{
	static const int ID = 225;
	static const string PATH = "azo\\az_vampire_mask\\data\\lighting\\az_lighting_night.txt";
	static const string NAMALSK_BRIGHT = "nst\\namalsk\\data\\lighting\\lighting_namalsk_bright.txt";
	static const string VANILLA_DEFAULT = "dz\\data\\lighting\\lighting_default.txt";
}

modded class WorldLighting
{
	override void SetGlobalLighting(int lightingID)
	{
		super.SetGlobalLighting(lightingID);
		if (lightingID == AZ_VampNightLighting.ID)
		{
			GetGame().GetWorld().LoadNewLightingCfg(AZ_VampNightLighting.PATH);
			Print("[AZ_Vamp] night lighting " + AZ_VampNightLighting.ID + ": " + AZ_VampNightLighting.PATH);
		}
	}
}
