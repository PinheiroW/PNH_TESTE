











class AZ_VampFace
{
	static const string DIR = "azo\\az_vampire_mask\\data\\faces\\";
	static const string HEADS = "azo\\az_vampire_mask\\data\\faces\\heads\\";
	static const string EVA = "dz\\characters\\heads\\data\\f_eva_2\\";


	static bool Vanilla(PlayerBase p, out string tex, out string mat)
	{
		tex = p.m_AZVampBaseTex;
		mat = p.m_AZVampBaseMat;
		if (mat == "")
		{
			PluginLifespan ls = PluginLifespan.Cast(GetPlugin(PluginLifespan));
			if (ls)
			{
				tex = ls.GetCurrentHeadTexture(p);
				mat = ls.GetCurrentHeadMaterial(p);
			}
		}
		if (mat == "")
		{
			GetGame().ConfigGetText("CfgVehicles " + p.GetType() + " BloodyHands mat_normal", mat);
			tex = mat;
			tex.Replace(".rvmat", "_co.paa");
		}

		if (mat.Contains("_beard.rvmat"))
		{
			mat.Replace("_beard.rvmat", "_body_bearded.rvmat");
			tex.Replace("_beard_ca.paa", "_body_bearded_co.paa");
		}
		return mat != "";
	}


	static bool Paths(PlayerBase p, string state, out string tex, out string mat)
	{
		if (p.GetType() == "SurvivorF_Eva")
		{
			tex = DIR + "hhl_f_eva_body_co.paa";
			mat = DIR + "az_vampire_eva_" + state + ".rvmat";
			if (state == "vamp")
				mat = DIR + "az_vampire_eva.rvmat";
			if (state == "sick" || state == "blood")
				tex = EVA + "hhl_f_eva_body_co.paa";
			return true;
		}
		string vmat;
		if (!Vanilla(p, tex, vmat))
			return false;
		array<string> parts = new array<string>;
		vmat.Split("\\", parts);
		string stem = parts[parts.Count() - 1];
		stem.ToLower();
		stem.Replace(".rvmat", "");
		mat = HEADS + stem + "_" + state + ".rvmat";
		return true;
	}



	static const int FADE_STEP_MS = 20000;

	// Face sync (AZ-2279/AZ-2589): every client paints the face from synced state only:
	// m_AZVampire, m_AZVampBloodied, m_AZVampBitten/Turning, the Mask slot (or the server's
	// pending swap while the slot is briefly empty), alive and corpse state.
	// The warm fade is only a client-side transition between two looks, never an input.
	static const int MASK_NONE = 0;
	static const int MASK_BASE = 1;
	static const int MASK_BARED = 2;
	static const int MASK_FED = 3;
	static const int MASK_FRENZY = 4;

	static const int REFRESH_SYNC = 0;		// re-key only when the look changed (every net sync)
	static const int REFRESH_PAINT = 1;		// also repaint the current frame (attach, lifespan, load)
	static const int REFRESH_AGAIN = 2;		// also repaint a restored vanilla face (Reassert)

	static int s_Gen;

	static int TypeKind(string type)
	{
		if (type == "AZ_VampireMask_Frenzy")
			return MASK_FRENZY;
		if (type == "AZ_VampireMask_Fed")
			return MASK_FED;
		if (type == "AZ_VampireMask_Bared")
			return MASK_BARED;
		if (type == "AZ_VampireMask")
			return MASK_BASE;
		return MASK_NONE;
	}

	static int MaskKind(PlayerBase p)
	{
		EntityAI mask = p.FindAttachmentBySlotName("Mask");
		if (!mask)
		{
			// Server swap in flight (delete, re-attach 100 ms later): keep the look it is swapping to.
			if (p.m_AZVampire)
				return p.m_AZVampLookPending;
			return MASK_NONE;
		}
		// Frenzy extends Fed in config: test it first.
		if (mask.IsKindOf("AZ_VampireMask_Frenzy"))
			return MASK_FRENZY;
		if (mask.IsKindOf("AZ_VampireMask_Fed"))
			return MASK_FED;
		if (mask.IsKindOf("AZ_VampireMask_Bared"))
			return MASK_BARED;
		if (mask.IsKindOf("AZ_VampireMask_Base"))
			return MASK_BASE;
		return MASK_NONE;
	}

	// The look the synced state asks for. Same mapping as before; corpses keep it while fresh.
	static string Look(PlayerBase p)
	{
		int kind = MaskKind(p);
		// U10c fix 2: the clean flag can arrive before the server's mask swap, as for Fed below.
		if (p.m_AZVampire && kind == MASK_FRENZY)
		{
			if (p.m_AZVampBloodied)
				return "frenzy";
			return "vamp";
		}
		if (p.m_AZVampire && kind == MASK_FED)
		{
			// The clean flag can arrive before the server's mask replacement.
			if (p.m_AZVampBloodied)
				return "fed";
			return "vamp";
		}
		// U10c fix (4 Nov): bared fangs on an unwashed mouth keep the blood ("fed" = vamp eyes + blood) until a wash.
		if (p.m_AZVampire && kind == MASK_BARED)
		{
			if (p.m_AZVampBloodied)
				return "fed";
			return "vamp";
		}
		if (p.m_AZVampBloodied && p.m_AZVampire)
			return "blood";
		if (p.m_AZVampTurning || p.m_AZVampBitten)
			return "sick";
		return "";
	}

	static bool Painted(PlayerBase p)
	{
		return p.IsAlive() || p.m_CorpseState <= PlayerConstants.CORPSE_STATE_FRESH;
	}

	static void Apply(PlayerBase p)
	{
		Refresh(p, REFRESH_PAINT);
	}

	static void Refresh(PlayerBase p, int mode = 1)
	{
		if (!p || !Painted(p))
			return;
		string key = Look(p);
		string prev = p.m_AZVampLookKey;
		if (key == prev)
		{
			// Nothing changed: keep any running fade, just put the current frame back if asked.
			if (mode != REFRESH_SYNC)
				Paint(p, mode == REFRESH_AGAIN);
			return;
		}
		s_Gen++;
		p.m_AZVampFadeGen = s_Gen;
		p.m_AZVampLookKey = key;
		p.m_AZVampFadeStep = 0;
		if (p.IsAlive() && (prev == "fed" || prev == "frenzy" || prev == "vamp") && (key == "" || key == "blood") && p.m_AZVampire && p.GetType() != "SurvivorF_Eva")
		{
			FadeStep(p, 1, p.m_AZVampFadeGen);
			return;
		}
		Paint(p, false);
		string what = key;
		if (!p.m_AZVampFace)
			what = "off";
		Print("[AZ_Vamp] face " + what + " " + p.m_AZVampFaceMatNow + " tex " + p.m_AZVampFaceTexNow + " base " + p.m_AZVampBaseMat + " alive " + p.IsAlive() + " t " + GetGame().GetTime());
		if (GetGame().IsClient())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Reassert, REASSERT_MS, false, p, p.m_AZVampFadeGen, 0);
	}

	static void SetFace(PlayerBase p, string tex, string mat, bool vamp)
	{
		p.SetFaceTexture(tex);
		p.SetFaceMaterial(mat);
		p.m_AZVampFace = vamp;
		p.m_AZVampFaceTexNow = tex;
		p.m_AZVampFaceMatNow = mat;
	}

	// Paints the current frame (fade step or target look) from the current base face.
	static void Paint(PlayerBase p, bool again)
	{
		string tex, mat;
		if (p.m_AZVampFadeStep > 0)
		{
			string warm = "warm";
			if (p.m_AZVampBloodied)
				warm = "bwarm";
			if (!Paths(p, warm + p.m_AZVampFadeStep, tex, mat))
				return;
			SetFace(p, tex, mat, true);
			BodyMat(p, mat, tex);
			p.m_AZVampBody = true;
			return;
		}
		string key = p.m_AZVampLookKey;
		if (key != "" && Paths(p, key, tex, mat))
			SetFace(p, tex, mat, true);
		else if ((p.m_AZVampFace || (again && p.m_AZVampFaceMatNow != "")) && Vanilla(p, tex, mat))
			SetFace(p, tex, mat, false);
		Body(p, again);
	}

	static const int REASSERT_MS = 2000;




	static const int REASSERT_LATE_MS = 6000;
	static const int REASSERT_LAST_MS = 20000;

	static void Reassert(PlayerBase p, int gen, int pass = 0)
	{
		if (!p || gen != p.m_AZVampFadeGen || !Painted(p))
			return;
		if (pass == 0)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Reassert, REASSERT_LATE_MS, false, p, gen, 1);
		else if (pass == 1)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Reassert, REASSERT_LAST_MS, false, p, gen, 2);
		// Recomputed from synced state, never a cached frame: a missed event re-keys here.
		Refresh(p, REFRESH_AGAIN);
		Print("[AZ_Vamp] face reassert " + pass + " " + p.GetType() + " " + p.m_AZVampLookKey + " " + p.m_AZVampFaceMatNow + " body " + p.m_AZVampBodyMatNow);
	}

	static void FadeStep(PlayerBase p, int step, int gen)
	{
		if (!p || gen != p.m_AZVampFadeGen)
			return;
		// The fade was scheduled for a look that is no longer wanted: start over from the synced state.
		if (!p.IsAlive() || Look(p) != p.m_AZVampLookKey)
		{
			p.m_AZVampFadeStep = 0;
			Refresh(p, REFRESH_PAINT);
			return;
		}
		if (step > 2)
		{
			p.m_AZVampFadeStep = 0;
			// AZ-2589: SetFace records the restored face, so a later Reassert never re-paints a vampire frame.
			Paint(p, false);
			if (GetGame().IsClient())
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Reassert, REASSERT_MS, false, p, gen, 0);
			return;
		}
		p.m_AZVampFadeStep = step;
		Paint(p, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FadeStep, FADE_STEP_MS, false, p, step + 1, gen);
	}




	static void BodyMat(PlayerBase p, string mat, string tex, bool vanilla = false)
	{
		array<string> slots = {"Body", "Legs", "Feet", "Gloves"};
		foreach (string slot : slots)
		{
			EntityAI part = p.GetInventory().FindPlaceholderForSlot(InventorySlots.GetSlotIdFromString(slot));
			if (part)
			{
				part.SetObjectTexture(0, tex);
				part.SetObjectMaterial(0, mat);
			}
		}
		p.m_AZVampBodyTexNow = tex;
		p.m_AZVampBodyMatNow = mat;
		p.m_AZVampSkinTex = "";
		p.m_AZVampSkinMat = "";
		if (!vanilla)
		{
			p.m_AZVampSkinTex = tex;
			p.m_AZVampSkinMat = mat;
		}
		SkinWear(p);
	}




	static bool SkinLook(PlayerBase p, out string tex, out string mat)
	{
		tex = p.m_AZVampSkinTex;
		mat = p.m_AZVampSkinMat;
		if (mat == "" && p.m_EmptyGloves)
		{
			mat = p.m_EmptyGloves.GetHiddenSelectionsMaterials().Get(0);
			tex = p.m_EmptyGloves.GetHiddenSelectionsTextures().Get(0);
		}
		return mat != "";
	}

	static void SkinWear(PlayerBase p)
	{
		string tex, mat;
		if (!p || !SkinLook(p, tex, mat))
			return;
		array<string> slots = {"Body", "Legs", "Feet", "Gloves"};
		foreach (string slot : slots)
		{
			EntityAI item = p.GetInventory().FindAttachment(InventorySlots.GetSlotIdFromString(slot));
			if (!item)
				continue;
			int idx = item.GetHiddenSelectionIndex("personality");
			if (idx < 0)
				continue;
			item.SetObjectMaterial(idx, mat);
			item.SetObjectTexture(idx, tex);
		}
	}


	static void Body(PlayerBase p, bool again = false)
	{
		string tex, mat;
		int kind = MaskKind(p);
		if (p.m_AZVampire && (kind == MASK_BARED || kind == MASK_FED || kind == MASK_FRENZY))
		{
			if (!Paths(p, "pale", tex, mat))
				return;
			p.m_AZVampBody = true;
			BodyMat(p, mat, tex);
			return;
		}
		if (!p.m_AZVampBody && !(again && p.m_AZVampBodyMatNow != ""))
			return;
		if (!Vanilla(p, tex, mat))
			return;
		p.m_AZVampBody = false;
		BodyMat(p, mat, tex, true);
	}
}

modded class PlayerBase
{
	bool m_AZVampFace;
	bool m_AZVampBody;
	int m_AZVampFadeGen;
	int m_AZVampFadeStep;
	string m_AZVampLookKey;
	string m_AZVampBaseTex;
	string m_AZVampBaseMat;
	string m_AZVampFaceTexNow;
	string m_AZVampFaceMatNow;
	string m_AZVampBodyTexNow;
	string m_AZVampBodyMatNow;
	string m_AZVampSkinTex;
	string m_AZVampSkinMat;

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		if (slot_name == "Mask")
			AZ_VampFace.Apply(this);
		else if (m_AZVampBody && AZ_VampFaceIsBodySlot(slot_name))
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampFaceApply, 100, false);
		else if (m_AZVampSkinMat != "" && AZ_VampFaceIsBodySlot(slot_name))
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampSkinWear, 100, false);
	}

	void AZ_VampSkinWear()
	{
		AZ_VampFace.SkinWear(this);
	}

	bool AZ_VampFaceIsBodySlot(string slot_name)
	{
		return slot_name == "Body" || slot_name == "Legs" || slot_name == "Feet" || slot_name == "Gloves";
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (slot_name == "Mask" || (m_AZVampBody && AZ_VampFaceIsBodySlot(slot_name)))
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampFaceApply, 100, false);
	}

	void AZ_VampFaceApply()
	{
		AZ_VampFace.Apply(this);
	}

	// Any synced change on this player (or the first sync after it streams into the bubble) re-derives the look.
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		AZ_VampFace.Refresh(this, AZ_VampFace.REFRESH_SYNC);
	}

	// Vanilla re-applies its own face once the character is loaded; put ours back now and once more 2 s later,
	// as vanilla does for corpse visuals ("sometimes it takes a while to load in").
	override void OnPlayerLoaded()
	{
		super.OnPlayerLoaded();
		if (!GetGame().IsClient())
			return;
		AZ_VampFace.Refresh(this, AZ_VampFace.REFRESH_PAINT);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampFaceApply, 2000, false);
	}



	bool m_AZVampTalk;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (m_AZVampTalk && GetCommandModifier_Additives())
			GetCommandModifier_Additives().SetTalking(true);
	}
}


modded class PluginLifespan
{
	override protected void SetPlayerLifespanLevel(PlayerBase player, LifespanLevel level)
	{
		super.SetPlayerLifespanLevel(player, level);
		if (!player || !level)
			return;
		bool changed = player.m_AZVampBaseMat != level.GetMaterialName();
		player.m_AZVampBaseTex = level.GetTextureName();
		player.m_AZVampBaseMat = level.GetMaterialName();
		// Vanilla calls this on every net sync of the player; log only real beard changes.
		if (GetGame().IsClient() && changed)
			Print("[AZ_Vamp] face lifespan " + player.GetType() + " " + player.m_AZVampBaseMat + " tex " + player.m_AZVampBaseTex + " vamp " + player.m_AZVampFace + " t " + GetGame().GetTime());
		// Vanilla just painted its face over ours: put the current frame back (no re-key, a running fade keeps going).
		if (player.m_AZVampFace || player.m_AZVampFadeStep > 0 || player.m_AZVampLookKey != "")
			AZ_VampFace.Refresh(player, AZ_VampFace.REFRESH_PAINT);
	}
}



modded class Clothing
{
	override void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null)
	{
		super.SwitchItemSelectionTextureEx(context, par);
		Param1<PlayerBase> data = Param1<PlayerBase>.Cast(par);
		if (!data || !data.param1 || data.param1.m_AZVampSkinMat == "" || data.param1.m_CorpseState > PlayerConstants.CORPSE_STATE_FRESH)
			return;
		int idx = GetHiddenSelectionIndex("personality");
		if (idx < 0)
			return;
		SetObjectMaterial(idx, data.param1.m_AZVampSkinMat);
		SetObjectTexture(idx, data.param1.m_AZVampSkinTex);
	}
}
