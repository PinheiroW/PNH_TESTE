
modded class ScriptedLightBase
{
	override void EOnInit(IEntity other, int extra)
	{
		if (!AZ_VampCrashTrace.Enabled())
		{
			super.EOnInit(other, extra);
			return;
		}
		string lightClass = Type().ToString();
		AZ_VampCrashTrace.Mark("light EOnInit enter class=" + lightClass);
		super.EOnInit(other, extra);
		AZ_VampCrashTrace.Mark("light EOnInit exit class=" + lightClass);
	}
}

modded class AZ_Crypt_Moon_Grate
{
	override void EEInit()
	{
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate EEInit enter");
		super.EEInit();
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate EEInit exit");
	}

	override void AZ_MistSpawn()
	{
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate AZ_MistSpawn enter");
		super.AZ_MistSpawn();
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate AZ_MistSpawn exit");
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate EEDelete enter");
		super.EEDelete(parent);
		AZ_VampCrashTrace.Mark("AZ_Crypt_Moon_Grate EEDelete exit");
	}
}
