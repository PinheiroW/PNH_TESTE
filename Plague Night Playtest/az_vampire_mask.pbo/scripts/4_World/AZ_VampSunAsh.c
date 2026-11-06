





class AZ_VampAsh
{
	static const float BURN_FIRE_HEALTH = 40;
	static const int ASH_DELAY_MS = 1500;
	static const int ASH_LIFETIME_S = 1800;


	static void SetBurn(PlayerBase p, bool exposed)
	{
		int level = 0;
		if (exposed)
		{
			level = 1;
			if (p.GetHealth("", "Health") < BURN_FIRE_HEALTH)
				level = 2;
		}
		if (level != p.m_AZVampBurn)
		{
			p.m_AZVampBurn = level;
			p.SetSynchDirty();
		}
	}


	static void Crumble(PlayerBase body, bool sun = true, PlayerBase hunter = null)
	{
		if (!body)
			return;
		vector pos = body.GetPosition();

		AZ_VampState.SetVampire(body, false);
		AZ_VampState.SetSickLeft(body, 0);
		AZ_VampBind.Unbind(body);
		body.DropAllItems();
		EntityAI ash = EntityAI.Cast(GetGame().CreateObjectEx("AZ_VampAshPile", pos, ECE_PLACE_ON_SURFACE));
		if (ash)
		{
			ash.SetOrientation(Vector(body.GetOrientation()[0], 0, 0));
			ash.SetLifetime(ASH_LIFETIME_S);
		}

		vector side = Vector(Math.RandomFloat(-0.3, 0.3), 0, Math.RandomFloat(-0.3, 0.3));


		EntityAI skull = EntityAI.Cast(GetGame().CreateObjectEx("AZ_VampSkull_Trophy", pos - side, ECE_PLACE_ON_SURFACE));
		if (skull)
		{
			skull.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			skull.SetLifetime(ASH_LIFETIME_S);
		}
		if (hunter && hunter != body && !hunter.m_AZVampire && hunter.IsAlive())
		{
			AZ_VampProgress.OnSlain(hunter);
			hunter.Message("It bursts into ash. A skull lies in what is left.", "colorAction");
		}
		Print("[AZ_Vamp] burned to ash " + AZ_VampState.Uid(body) + " at " + pos + " sun " + sun + " hunter " + AZ_VampState.Uid(hunter));
		if (sun)
			AZ_VampProgress.OnAsh();
		// Allow the queued hands drop to finish before its owner is deleted.
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteBody, 500, false, body);
	}

	static void DeleteBody(PlayerBase body)
	{
		if (!body)
			return;
		// Check at deletion time: logout registration can lag behind the death/ash callback.
		// Keep the entity until vanilla releases its identity and finishes any pending logout.
		if (body.GetIdentity() || (GetGame().GetMission() && GetGame().GetMission().IsPlayerDisconnecting(body)))
		{
			// Synced: SetInvisible alone only hides the corpse on the server, so other players kept seeing it until respawn.
			if (!body.m_AZVampAshed)
			{
				body.m_AZVampAshed = true;
				body.SetSynchDirty();
			}
			body.SetInvisible(true);
			// Also clean up after respawn, when the identity leaves the old body.
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteBody, 500, false, body);
			return;
		}
		GetGame().ObjectDelete(body);
	}
}

class AZ_VampAshPile extends ItemBase
{
	Particle m_AZVampEmbers;

	void AZ_VampRemovePhysics()
	{
		if (dBodyIsSet(this))
			dBodyDestroy(this);
	}

	override void OnCreatePhysics()
	{
		super.OnCreatePhysics();
		// Remove recreated bodies on every peer, after the native creation callback returns.
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampRemovePhysics);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(AZ_VampRemovePhysics);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(AZ_VampRemovePhysics);
		if (m_AZVampEmbers)
			m_AZVampEmbers.Stop();
		super.EEDelete(parent);
	}

	override void EEInit()
	{
		super.EEInit();
		// Keep the fireplace ash visual and smoke, without its item collision body.
		AZ_VampRemovePhysics();
		if (!GetGame().IsDedicatedServer())
		{


			Particle.PlayInWorld(ParticleList.AZ_VAMP_ASH_BURST, GetPosition() + Vector(0, 0.9, 0));
			m_AZVampEmbers = Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, "0 0.02 0"); // AZ-2588: burning smoke, not the ash embers
			if (m_AZVampEmbers)
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_AZVampEmbers.Stop, 120000, false);
		}
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}
}

modded class PlayerBase
{
	int m_AZVampBurn;
	int m_AZVampBurnShown;
	bool m_AZVampAshed;		// burned to ash while logging out: the hidden corpse waits for vanilla's logout end
	Particle m_AZVampBurnSmoke;
	Particle m_AZVampBurnFire;
	EffectSound m_AZVampBurnSound;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableInt("m_AZVampBurn", 0, 2);
		RegisterNetSyncVariableBool("m_AZVampAshed");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (m_AZVampBurn != m_AZVampBurnShown)
			AZ_VampBurnVisual(m_AZVampBurn);
		if (m_AZVampAshed)
		{
			SetInvisible(true);
			if (m_AZVampBurnShown != 0)
				AZ_VampBurnVisual(0);
		}
	}

	void AZ_VampBurnVisual(int level)
	{
		m_AZVampBurnShown = level;
		if (m_AZVampBurnSmoke)
		{
			m_AZVampBurnSmoke.Stop();
			m_AZVampBurnSmoke = null;
		}
		if (m_AZVampBurnFire)
		{
			m_AZVampBurnFire.Stop();
			m_AZVampBurnFire = null;
		}
		AZ_VampBurnSoundStop();
		if (level == 1)
			m_AZVampBurnSound = SEffectManager.PlaySoundOnObject("Food_Burning_SoundSet", this, 0.5, 0.5, true);
		else if (level >= 2)
			m_AZVampBurnSound = SEffectManager.PlaySoundOnObject("LightFire_SoundSet", this, 0.5, 0.5, true);

		if (level >= 1)
			m_AZVampBurnSmoke = Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, "0 1.35 0");
		if (level >= 2)
			m_AZVampBurnFire = Particle.PlayOnObject(ParticleList.AZ_VAMP_SUN_BURN, this, "0 1.0 0");
	}

	override void EEKilled(Object killer)
	{
		bool vampDied = GetGame().IsServer() && m_AZVampire;
		bool burning = vampDied && m_AZVampBurn > 0;
		PlayerBase hunter;
		EntityAI killerEnt = EntityAI.Cast(killer);
		if (vampDied && killerEnt)
			hunter = PlayerBase.Cast(killerEnt.GetHierarchyRootPlayer());
		if (GetGame().IsServer() && m_AZVampire)
			Print("[AZ_Vamp] vampire died " + AZ_VampState.Uid(this) + " blood " + GetHealth("", "Blood") + " health " + GetHealth("", "Health") + " shock " + GetHealth("", "Shock") + " uncon " + IsUnconscious() + " killer " + killer);
		super.EEKilled(killer);


		if (vampDied)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampAsh.Crumble, AZ_VampAsh.ASH_DELAY_MS, false, this, burning, hunter);
	}

	void AZ_VampBurnSoundStop()
	{
		if (m_AZVampBurnSound)
		{
			m_AZVampBurnSound.SoundStop();
			SEffectManager.DestroyEffect(m_AZVampBurnSound);
			m_AZVampBurnSound = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		AZ_VampBurnSoundStop();
		if (m_AZVampBurnSmoke)
			m_AZVampBurnSmoke.Stop();
		if (m_AZVampBurnFire)
			m_AZVampBurnFire.Stop();
		super.EEDelete(parent);
	}
}
