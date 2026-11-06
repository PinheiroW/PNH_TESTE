#ifdef NAMALSK_SURVIVAL




modded class EVRStorm
{
	override int GetEventCooldown()
	{
		return Math.RandomIntInclusive(10800, 21600);
	}
}
#endif
