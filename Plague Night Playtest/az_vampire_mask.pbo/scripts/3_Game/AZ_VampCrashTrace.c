


class AZ_VampCrashTrace
{
	static int s_Enabled = -1;
	static int s_Count;
	static int s_Mission;

	static bool Enabled()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return false;
		if (s_Enabled < 0)
		{
			s_Enabled = 0;
			if (FileExist("$profile:AZ_Vampire/crashtrace.txt"))
				s_Enabled = 1;
		}
		return s_Enabled == 1;
	}

	static void BeginMission()
	{
		if (!Enabled())
			return;
		s_Mission++;
		s_Count = 0;
	}

	static void Mark(string phase)
	{
		if (!Enabled() || s_Count >= 2000)
			return;
		s_Count++;
		string line = "[AZ_CRASH] mission=" + s_Mission + " seq=" + s_Count + " ms=" + GetGame().GetTime() + " " + phase;
		if (s_Count == 2000)
			line = line + " TRACE LIMIT (until next mission)";
		Print(line);
		FileHandle traceFile = OpenFile("$profile:AZ_Vampire/crashtrace.log", FileMode.APPEND);
		if (traceFile)
		{
			FPrintln(traceFile, line);
			CloseFile(traceFile);
		}
	}
}

modded class DayZGame
{
	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{

		if (!AZ_VampCrashTrace.Enabled())
		{
			super.OnRPC(sender, target, rpc_type, ctx);
			return;
		}
		AZ_VampCrashTrace.Mark("RPC enter id=" + rpc_type + " targetPresent=" + (target != null));
		super.OnRPC(sender, target, rpc_type, ctx);
		AZ_VampCrashTrace.Mark("RPC exit id=" + rpc_type);
	}
}
