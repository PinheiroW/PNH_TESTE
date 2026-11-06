






class AZ_VampClientReset
{
	static void Run(string when)
	{

		AZ_VampCryptLights.s_Trap = null;
		AZ_VampCryptLights.s_Nave = null;
		AZ_VampCryptLights.s_Rest = null;
		AZ_VampCryptLights.s_Dais = null;
		AZ_VampCryptLights.s_Bowls = null;
		AZ_VampCryptLights.s_River = null;
		AZ_VampCryptLights.s_Slab = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampCryptLights.Tick);
		AZ_VampCryptLights.s_Ticking = false;

		AZ_VampLightBudget.s_All = null;
		AZ_VampLightBudget.s_Lit = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampLightBudget.Tick);
		AZ_VampLightBudget.s_Ticking = false;

		AZ_VampHallVotive.s_All = null;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampHallVotive.ClientTick);
		AZ_VampHallVotive.s_Ticking = false;

		AZ_VampCryptGallery.s_Votive = null;
		AZ_VampCryptGallery.s_Lamp = null;
		AZ_VampCryptGallery.s_Bones = null;
		AZ_VampCryptGallery.s_BonesPool = null;
		AZ_VampCryptGallery.s_Pools = null;
		AZ_VampCryptGallery.s_FrescoPools = null;
		AZ_VampCryptGallery.s_Eye = null;
		AZ_VampCryptGallery.s_EyeRed = false;

		AZ_VampEmbers.s_Was = null;
		AZ_VampEmbers.s_Gen++;
		AZ_VampStareBeat.s_Snd = null;
		AZ_VampStareBeat.s_Who = null;
		AZ_VampStareBeat.s_Gen++;
		AZ_VampStareBeat.s_Until = 0;
		AZ_VampInvite.s_Who = null;
		AZ_VampInvite.s_Gen++;
		AZ_VampInvite.s_ClientUntil = 0;

		AZ_VampRitualCandle.s_AZMusicCandles = null;
		AZ_VampCryptExit.s_ClientSlab = null;
		Print("[AZ_Vamp] client statics reset (" + when + "): crypt light registries, tickers and client-local objects forgotten");
	}
}
