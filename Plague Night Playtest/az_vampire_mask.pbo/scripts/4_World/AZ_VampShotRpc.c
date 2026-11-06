


class AZ_VampShotRpc
{
	static const int RPC = 4702140;
	static bool s_Pending;
	static string s_Label;
	static vector s_Cam;
	static vector s_Target;
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != AZ_VampShotRpc.RPC || !GetGame().IsClient())
			return;
		Param3<string, vector, vector> d = new Param3<string, vector, vector>("", vector.Zero, vector.Zero);
		if (!ctx.Read(d))
			return;
		AZ_VampShotRpc.s_Label = d.param1;
		AZ_VampShotRpc.s_Cam = d.param2;
		AZ_VampShotRpc.s_Target = d.param3;
		AZ_VampShotRpc.s_Pending = true;
	}
}
