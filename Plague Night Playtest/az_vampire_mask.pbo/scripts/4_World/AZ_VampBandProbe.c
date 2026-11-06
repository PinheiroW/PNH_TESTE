





class AZ_VampBand
{
	static const int RPC = 4702301;
	static const string LIGHTING_G = "azo\\az_vampire_mask\\data\\lighting\\crypt_lighting_g.cfg";
	static string s_V = "";

	static bool Is(string v)
	{
		return s_V == v;
	}

	static void Send(PlayerBase p, string v)
	{
		GetGame().RPCSingleParam(p, RPC, new Param1<string>(v), true, p.GetIdentity());
		Print("[AZ_Vamp] TEST band variant " + v + " sent");
	}

	static void Apply(string v)
	{
		string was = s_V;
		s_V = v;
		bool moon = !Is("E");



		if ((v == "E" || was == "E") && AZ_Crypt_Moon_Grate.s_Grate)
			AZ_Crypt_Moon_Grate.s_Grate.AZ_MoonOn(moon);
		if (v == "K" || was == "K")
			AZ_Crypt_Moon_Grate.AZ_ApseShadow(!Is("K"));
		Print("[AZ_Vamp] TEST band variant " + v + " on the client: eye " + GetGame().GetWorld().GetEyeAccom() + " moon " + moon + " fog " + GetGame().GetWeather().GetFog().GetActual() + " volfog " + GetGame().GetWeather().GetDynVolFogDistanceDensity());
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampBand.RPC || !GetGame().IsClient())
			return;
		Param1<string> d = new Param1<string>("");
		if (ctx.Read(d))
			AZ_VampBand.Apply(d.param1);
	}
}
