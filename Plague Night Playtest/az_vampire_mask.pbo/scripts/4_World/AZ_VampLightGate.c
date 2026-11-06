


class AZ_VampLightGate
{
	static int s_Off = -1;

	static bool Off()
	{
		if (s_Off < 0)
		{
			s_Off = 0;
			if (FileExist("$profile:AZ_Vampire\\nolights.txt"))
				s_Off = 1;
			if (s_Off == 1)
				Print("[AZ_Vamp] light gate: $profile:AZ_Vampire\\nolights.txt found, no script lights this session");
		}
		return s_Off == 1;
	}




	static ScriptedLightBase CryptDay(ScriptedLightBase light, typename name, vector pos)
	{
		if (light && name != AZ_VampEyeLight && AZ_VampCrypt.Inside(pos))
			light.SetVisibleDuringDaylight(true);
		return light;
	}

	static ScriptedLightBase Light(typename name, vector global_pos = "0 0 0", float fade_in_time_in_s = 0)
	{
		if (Off())
			return null;
		if (!AZ_VampCrashTrace.Enabled())
			return CryptDay(ScriptedLightBase.CreateLight(name, global_pos, fade_in_time_in_s), name, global_pos);
		AZ_VampCrashTrace.Mark("CreateLight enter class=" + name.ToString());
		ScriptedLightBase light = ScriptedLightBase.CreateLight(name, global_pos, fade_in_time_in_s);
		AZ_VampCrashTrace.Mark("CreateLight exit present=" + (light != null));
		return CryptDay(light, name, global_pos);
	}

	static ScriptedLightBase AtPoint(typename name, notnull Object target, string memory_point_start, string memory_point_target = "", vector global_pos = "0 0 0", float fade_in_time_in_s = 0)
	{
		if (Off())
			return null;
		if (!AZ_VampCrashTrace.Enabled())
			return CryptDay(ScriptedLightBase.CreateLightAtObjMemoryPoint(name, target, memory_point_start, memory_point_target, global_pos, fade_in_time_in_s), name, target.GetPosition());
		AZ_VampCrashTrace.Mark("CreateLightAtObjMemoryPoint enter class=" + name.ToString());
		ScriptedLightBase attachedLight = ScriptedLightBase.CreateLightAtObjMemoryPoint(name, target, memory_point_start, memory_point_target, global_pos, fade_in_time_in_s);
		AZ_VampCrashTrace.Mark("CreateLightAtObjMemoryPoint exit present=" + (attachedLight != null));
		return CryptDay(attachedLight, name, target.GetPosition());
	}
}
