

modded class MissionServer
{
	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		PlayerBase p = super.OnClientNewEvent(identity, pos, ctx);
		if (p && identity)
			AZ_VampState.SetSpawnGrace(identity.GetId());
		return p;
	}
}
