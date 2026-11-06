// Namalsk's LEHS space suit is a human-only tool: vampires can't put it on, and a turned human drops it.
// U10c fix (AZ-2590, 4 Nov): the block keys on m_AZVampire alone. It used to also need AZ_VampEvent.Active(),
// but that state only exists on the server (AZ_VampState.s_Data loads under IsServer), so the client never
// blocked, and outside the event (vampires keep the curse between events) the server did not either.
// Humans still need Namalsk's own rule: Body, Vest, Belt, Legs, Back, Gloves, Feet, Melee and Shoulder empty.
// U10c fix 2 (Oliver, 4 Nov): grandfather existing wearers. Strip runs only at the moment of turning
// (AZ_VampTurn.Turn, AZ_VampEvent.MakeVampire when not yet a vampire), never on login, restart or a tick.
class AZ_VampLEHS
{
	static bool Blocked(EntityAI parent)
	{
		PlayerBase p = PlayerBase.Cast(parent);
		return p && p.m_AZVampire;
	}

	static void Strip(PlayerBase p)
	{
#ifdef NAMALSK_SURVIVAL
		if (!GetGame().IsServer() || !p || !p.m_AZVampire || !p.IsAlive())
			return;
		bool dropped = false;
		// Helmet first: Namalsk will not release the suit while the helmet is on.
		EntityAI helmet = p.FindAttachmentBySlotName("Headgear");
		if (helmet && helmet.IsKindOf("dzn_lehs_helmet") && p.ServerDropEntity(helmet))
			dropped = true;
		EntityAI suit = p.FindAttachmentBySlotName("LEHS");
		if (suit && suit.IsKindOf("dzn_lehs") && p.ServerDropEntity(suit))
			dropped = true;
		if (dropped)
		{
			p.Message("The suit will not seal around a vampire. You cast it off.", "colorAction");
			Print("[AZ_Vamp] lehs stripped " + AZ_VampState.Uid(p));
		}
#endif
	}
}

#ifdef NAMALSK_SURVIVAL
modded class dzn_lehs
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (AZ_VampLEHS.Blocked(parent))
			return false;
		return super.CanPutAsAttachment(parent);
	}
}

modded class dzn_lehs_helmet
{
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (AZ_VampLEHS.Blocked(parent))
			return false;
		return super.CanPutAsAttachment(parent);
	}
}

modded class PlayerBase
{
	// Namalsk's PlayerBase returns true for the suit early (bare body) without calling super: refuse first.
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if (m_AZVampire && attachment && (attachment.IsKindOf("dzn_lehs") || attachment.IsKindOf("dzn_lehs_helmet")))
			return false;
		return super.CanReceiveAttachment(attachment, slotId);
	}

	// Namalsk keeps a suit on while its helmet, battery or O2 tank is fitted, and a pressurised helmet on:
	// a vampire must always be able to shed them (Strip), so skip that rule for vampires only.
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (m_AZVampire && attachment && (attachment.IsKindOf("dzn_lehs") || attachment.IsKindOf("dzn_lehs_helmet")))
			return true;
		return super.CanReleaseAttachment(attachment);
	}
}
#endif
