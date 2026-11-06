

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		AZ_VampState.Init();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampDecor.Place, 15000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampLandmarks.Place, 20000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampDrop.Startup, 25000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampVillageSupplies.Startup, 27000, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampTorch.PlaceMore, 32000, false);

		string probe = AZ_VampState.DIR + "/probe_at.txt";
		if (FileExist(probe))
		{
			FileHandle fh = OpenFile(probe, FileMode.READ);
			string line;
			FGets(fh, line);
			CloseFile(fh);
			TStringArray xz = new TStringArray;
			line.Split(" ", xz);
			if (xz.Count() >= 2)
			{
				vector at = Vector(xz[0].ToFloat(), 0, xz[1].ToFloat());
				Print("[AZ_Vamp] probe requested at " + at);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampProbe.Run, 30000, false, at);

				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampProgress.ProbeNight, 45000, false, 0);
			}
		}
	}

	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReadyEvent(identity, player);
		if (player) player.AZ_HbSendConfig();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		AZ_VampCrypt.OnConnect(player);
		if (player)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(player.AZ_HbSendConfig, 3000, false);
		if (player && AZ_VampState.IsVampire(player))
		{
			AZ_VampBind.Bind(player);
			AZ_VampBlood.Restore(player);
			AZ_VampState.Restore(player);		// J01/F03/F05: the cooldowns, down count, weakness and hunger it logged out with
			AZ_VampRecord rebound = AZ_VampState.Find(AZ_VampState.Uid(player));
			string origin = "unknown";
			if (rebound && rebound.sickSource != "")
				origin = rebound.sickSource;
			AZ_VampAudit.Life(player, "VAMPIRE_REBOUND", origin);
		}
		else if (player)
		{

			EntityAI worn = player.FindAttachmentBySlotName("Mask");
			if (worn && worn.IsKindOf("AZ_VampireMask_Base"))
				GetGame().ObjectDelete(worn);
			if (AZ_VampState.GetSickLeft(player) > 0)
			{
				AZ_VampRecord sick = AZ_VampState.Find(AZ_VampState.Uid(player));
				string source = "unknown";
				if (sick && sick.sickSource != "")
					source = sick.sickSource;
				AZ_VampTurn.StartTurning(player, AZ_VampState.GetSickLeft(player), source);
			}
		}
	}

	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		if (player && player.m_AZVampTurning)
			AZ_VampAudit.Life(player, "DISCONNECT_SICK_PERSISTED", player.m_AZInfectionSource);
		super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		// Final logout only: preserve vanilla's timer, cancellation and body policy.
		// Settle actual drained blood before HandleBody can kill/delete the victim.
		if (player && player.m_AZVampHeld)
			AZ_VampFeed.Release(null, player);
		AZ_VampState.Checkpoint(player);		// F04 + J01/F03/F05: what the next login gets back, before vanilla saves the body
		AZ_VampShadow.Forget(player);		// J03: releases a held victim, drops the entity's shadow record
		super.PlayerDisconnected(player, identity, uid);
		// burned to ash during the logout window (AZ_VampAsh.Crumble kept and hid the body so vanilla could free the
		// UID above): now the corpse can go
		if (player && player.m_AZVampAshed)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(GetGame().ObjectDelete, 500, false, player);
	}

	override void OnMissionFinish()
	{
		// F04: an orderly shutdown checkpoints everyone still online, as their final logout would
		array<PlayerBase> list;
		AZ_VampState.OnlinePlayers(list);
		if (list)
		{
			foreach (PlayerBase p : list)
				AZ_VampState.Checkpoint(p, false);
			AZ_VampState.Save();
		}
		super.OnMissionFinish();
	}
}
