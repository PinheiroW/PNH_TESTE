// Independent, server-authoritative melee controller. No HAH runtime dependency.
class Human_Base extends ZombieMaleBase
{
    ref HumansTactics m_HumanTactics;

    AnimalBase m_HumanAnimalLastSource;
    float m_HumanAnimalLastHit=-100;
    EntityAI m_HumanEnemy;
    EntityAI m_HumanSwingTarget;
    ref PGFilter m_HumanFilter;
    ref array<vector> m_HumanPath = {};
    vector m_HumanHome;
    vector m_HumanGoal;
    vector m_HumanLastKnown;
    float m_HumanClock;
    float m_HumanScanAt;
    float m_HumanSeen;
    float m_HumanPathAt;
    vector m_HumanPathGoal;
    vector m_HumanProgressPosition;
    vector m_HumanDetour;
    float m_HumanProgressAt;
    float m_HumanTravelAt;
    float m_HumanDetourUntil;
    float m_HumanUnsafeUntil;
    vector m_HumanHeardPosition;
    float m_HumanInvestigateUntil;
    float m_HumanHearingAt;
    float m_HumanVaultAt;
    float m_HumanVaultStarted;
    bool m_HumanVaulting;
    bool m_HumanVaultPending;
    vector m_HumanVaultLanding;
    int m_HumanVaultType = 1;
    ref HumansNavigation m_HumanNavigation;
    ref HumansVehicleClimb m_HumanVehicle;
    float m_HumanStageEnd;
    float m_HumanRestTime;
    float m_HumanHeading;
    float m_HumanAttackAt;
    float m_HumanBlockUntil;
    float m_HumanBlockAt;
    bool m_HumanThreatAttacking;
    EntityAI m_HumanBlockThreat;
    float m_HumanHitAt;
    int m_HumanPathIndex;
    int m_HumanStage;
    int m_HumanRestPhase = 3;
    int m_HumanRestStyle;
    int m_HumanAttackCue;
    int m_HumanPainCue;
    int m_HumanLastAttackCue;
    int m_HumanLastPainCue;
    bool m_HumanReady;
    bool m_HumanHitDone = true;
    bool m_HumanWake;
    // Stable saved IDs: -1 unset; 0 fists; 1 legacy crowbar; 2 bat; 3-13 variety.
    int m_HumanLoadout = -1;
    int m_HumanSwingKind;
    ItemBase m_HumanWeapon;
    int m_HumanEquipAttempts;
    float m_HumanSoundAt;
    bool m_HumanSwingSoundDone = true;
    static const int HUMAN_SWING_RPC = 874130;
    static ref array<Human_Base> HumanPopulation = {};
    static int HumanNextSocialId;
    int m_HumanSocialId;
    Human_Base m_HumanLeader;
    float m_HumanGroupAt;
    float m_HumanPatrolAt;
    float m_HumanGroupWaitUntil;
    bool m_HumanWasGrouped;
    vector m_HumanPatrolGoal;
    ItemBase m_HumanLootTarget;
    ItemBase m_HumanIgnoredLoot;
    float m_HumanLootAt;
    float m_HumanLootUntil;
    float m_HumanLootPickupAt;
    float m_HumanIgnoreLootUntil;
    Building m_HumanDoorBuilding;
    int m_HumanDoorIndex = -1;
    bool m_HumanDoorAction;
    float m_HumanDoorOpenAt;
    float m_HumanDoorScanAt;

    void Human_Base()
    {
        HumanNextSocialId++;
        m_HumanSocialId = HumanNextSocialId;
        HumanPopulation.Insert(this);
        RegisterNetSyncVariableInt("m_HumanAttackCue");
        RegisterNetSyncVariableInt("m_HumanPainCue");
        RegisterNetSyncVariableInt("m_HumanLoadout", -1, HumansWeapons.MAX_KIND);
        m_HumanFilter = new PGFilter;
        m_HumanFilter.SetFlags(PGPolyFlags.WALK | PGPolyFlags.INSIDE | PGPolyFlags.DOOR, PGPolyFlags.DISABLED | PGPolyFlags.SWIM | PGPolyFlags.LADDER | PGPolyFlags.JUMP | PGPolyFlags.CLIMB, 0);
    }

    bool HumanIsTrader(){return false;}
    bool HumanAuthority() { return GetGame().IsServer() || !GetGame().IsMultiplayer(); }
    override void EOnInit(IEntity other, int extra)
    {
        super.EOnInit(other, extra);
        if (HumanAuthority())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HumanInitLoadout, 100, false);
            m_StanceVariation = 2;
            DayZInfectedCommandMove move = GetCommand_Move();
            if (move) { move.SetStanceVariation(2); move.SetIdleState(2); }
        }
    }
    static int HumanRollLoadout()
    {
        if (Math.RandomFloat(0, 100) >= HumansSettings.Get().MeleeSpawnChancePercent) return 0;
        return Math.RandomIntInclusive(5, HumansWeapons.MAX_KIND);
    }
    void HumanInitLoadout()
    {
        if (!HumanAuthority() || !IsAlive()) return;
        if (m_HumanLoadout < 0) m_HumanLoadout = HumanRollLoadout();
        HumanEquip(m_HumanLoadout);
    }
    string HumanWeaponType(int kind)
    {
        return HumansWeapons.Types[HumansWeapons.ClampKind(kind)];
    }
    void HumanEquip(int kind)
    {
        if (!HumanAuthority()) return;
        kind = HumansWeapons.ClampKind(kind);
        // Bat loadout IDs remain reserved for persistence; replace saved bats.
        if (kind >= 2 && kind <= 4) kind = 12;
        if (m_HumanWeapon && m_HumanLoadout == kind) return;
        if (m_HumanWeapon) { RemoveChild(m_HumanWeapon); GetGame().ObjectDelete(m_HumanWeapon); m_HumanWeapon = null; }
        m_HumanLoadout = kind;
        if (kind > 0)
        {
            int hand = GetBoneIndexByName("RightHand_Dummy");
            if (hand >= 0)
            {
                m_HumanWeapon = ItemBase.Cast(GetGame().CreateObjectEx(HumanWeaponType(kind), "0 0 0", ECE_CREATEPHYSICS | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY));
                if (m_HumanWeapon)
                {
                    vector grip[4];
                    // Vanilla CreateInHands calibration on RightHand_Dummy:
                    // measured axes (-1,-.0097,-.0016),(-.0016,-.0016,1),(-.0097,1,.0016).
                    // Snap the tiny evaluation-time drift to the orthonormal hand basis.
                    grip[0] = "-1 0 0";
                    grip[1] = "0 0 1";
                    grip[2] = "0 1 0";
                    grip[3] = "0.02 0 0.01";
                    // The infected hand rig presents the one-handed tools in
                    // reverse with the bat grip. Turn them 90 degrees about
                    // their local X axis, keeping the hand anchor unchanged.
                    // Both weapon families use the same 90-degree correction.
                    if (HumansWeapons.Family(kind) == 1 || HumansWeapons.Family(kind) == 2)
                    {
                        grip[1] = "0 -1 0";
                        grip[2] = "0 0 1";
                    }
                    m_HumanWeapon.SetTransform(grip);
                    if (AddChild(m_HumanWeapon, hand))
                    {
                        m_HumanWeapon.SetTakeable(false);
                        m_HumanWeapon.SetAllowDamage(false);
                        GetGame().RemoteObjectTreeDelete(m_HumanWeapon);
                        GetGame().RemoteObjectTreeCreate(m_HumanWeapon);
                    }
                    else { GetGame().ObjectDelete(m_HumanWeapon); m_HumanWeapon = null; }
                }
            }
            if (!m_HumanWeapon)
            {
                m_HumanEquipAttempts++;
                Print("[Humans] Failed to attach " + HumanWeaponType(kind) + ", attempt " + m_HumanEquipAttempts.ToString());
                // Keep the chosen weapon; a temporarily unavailable hand bone
                // must not turn an armed spawn into a permanent unarmed roll.
                if (m_HumanEquipAttempts < 5) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HumanInitLoadout, 1000, false);
            }
            else m_HumanEquipAttempts = 0;
        }

        SetSynchDirty();
    }
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_HumanLoadout);
    }
    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version)) return false;
        if (!ctx.Read(m_HumanLoadout)) m_HumanLoadout = -1;
        // An unset saved value must still receive its first random loadout.
        // Explicitly saved fists (0) remain a deliberate unarmed loadout.
        if (m_HumanLoadout >= 0) m_HumanLoadout = HumansWeapons.ClampKind(m_HumanLoadout);
        return true;
    }
    override void EEDelete(EntityAI parent)
    {
        HumanPopulation.RemoveItem(this);
        m_HumanLootTarget = null;
        m_HumanLeader = null;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(HumanInitLoadout);
        if (HumanAuthority() && m_HumanWeapon) { RemoveChild(m_HumanWeapon); GetGame().ObjectDelete(m_HumanWeapon); m_HumanWeapon = null; }
        super.EEDelete(parent);
    }
    HumansTactics HumanTacticalBrain()
    {
        if (!m_HumanTactics) m_HumanTactics = new HumansTactics(this);
        return m_HumanTactics;
    }
    int HumanActiveWeapon()
    {
        if (m_HumanLoadout > 0 && (!HumanAuthority() || m_HumanWeapon)) return m_HumanLoadout;
        return 0;
    }
    float HumanReach(int kind)
    {
        if (kind == 0) return GetGame().ConfigGetFloat("CfgVehicles SurvivorBase MeleeModes Fist range");
        return GetGame().ConfigGetFloat("CfgVehicles " + HumanWeaponType(kind) + " MeleeModes Default range");
    }
    string HumanAmmo(int kind)
    {
        if (kind == 0) return GetGame().ConfigGetTextOut("CfgVehicles SurvivorBase MeleeModes Fist ammo");
        return GetGame().ConfigGetTextOut("CfgVehicles " + HumanWeaponType(kind) + " MeleeModes Default ammo");
    }
    bool HumanBlocked(EntityAI target)
    {
        PlayerBase player = PlayerBase.Cast(target);
        if (!player || !player.GetMeleeFightLogic() || !player.GetMeleeFightLogic().IsInBlock()) return false;
        vector towardAttacker = GetPosition() - player.GetPosition(); towardAttacker[1] = 0; towardAttacker.Normalize();
        vector forward = player.GetDirection(); forward[1] = 0; forward.Normalize();
        return vector.Dot(forward, towardAttacker) > 0.5;
    }
    bool HumanIsBlocking()
    {
        return IsAlive() && m_HumanRestPhase == 3 && m_HumanClock < m_HumanBlockUntil && GetCommand_Move() != null;
    }
    bool HumanStartBlock()
    {
        if (!HumanAuthority() || !IsAlive() || m_HumanRestPhase != 3 || m_HumanDoorAction || m_HumanVaulting || m_HumanVaultPending) return false;
        if (!HumanHostile(m_HumanEnemy) || vector.DistanceSq(GetPosition(), m_HumanEnemy.GetPosition()) > 9 || !HumanSight(m_HumanEnemy)) return false;
        // Interrupt our swing into the existing movement guard, never restart a
        // vault, hit reaction or door interaction. The cancelled swing cannot hit.
        if (!GetCommand_Move())
        {
            if (!GetCommand_Attack()) return false;
            StartCommand_Move();
        }
        if (!GetCommand_Move()) return false;
        m_HumanBlockUntil = m_HumanClock + 0.22;
        m_HumanBlockAt = 0; // Retained field for diagnostics; no block cooldown.
        m_HumanHitDone = true;
        m_HumanSwingSoundDone = true;
        HumanTacticalBrain().DodgeUntil = 0;
        HumanFace(m_HumanEnemy.GetPosition());
        return true;
    }
    void HumanUpdateBlock()
    {
        if (m_HumanEnemy != m_HumanBlockThreat)
        {
            m_HumanBlockThreat = m_HumanEnemy; m_HumanThreatAttacking = false;
            if (HumanHostile(m_HumanEnemy))
            {
                // Do not keep steering along an old patrol/stuck detour when
                // a fresh close threat requires turning and fighting now.
                if (m_HumanNavigation && !m_HumanNavigation.Door) m_HumanNavigation.ClearRecovery();
            }
        }
        if (!HumanHostile(m_HumanEnemy) || vector.DistanceSq(GetPosition(), m_HumanEnemy.GetPosition()) > 9)
        {
            m_HumanBlockUntil = 0;
            m_HumanThreatAttacking = false;
            return;
        }
        PlayerBase player = PlayerBase.Cast(m_HumanEnemy);
        ZombieBase infected = ZombieBase.Cast(m_HumanEnemy);
        // Vanilla light/heavy punches and weapon swings use MELEE2. Legacy
        // MELEE alone misses ordinary player attacks, including chained combos.
        AnimalBase animal = AnimalBase.Cast(m_HumanEnemy);
        bool attacking = (animal && animal.HumansAnimalAttacking()) || (player && (player.GetCommand_Melee2() || player.GetCommand_Melee())) || (infected && infected.GetCommand_Attack());
        // Combo range marks recovery rather than the next windup. Do not
        // consume WasHit(): vanilla melee owns that one-shot contact event.
        if (player && player.GetCommand_Melee2() && player.GetCommand_Melee2().IsInComboRange()) attacking = false;
        // Our infected bridge records contact without consuming it a second
        // time here. Counter during recovery once that strike has resolved.
        if (infected && infected.m_HumansContactDone) attacking = false;
        if (attacking)
        {
            vector toward = m_HumanEnemy.GetPosition() - GetPosition(); toward[1] = 0; toward.Normalize();
            if (vector.Dot(GetDirection(), toward) > 0.5) HumanTacticalBrain().React();
        }
        m_HumanThreatAttacking = attacking;
    }
    override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        AnimalBase animalSource=AnimalBase.Cast(source);
        if (HumanAuthority() && animalSource && animalSource==m_HumanAnimalLastSource && GetGame().GetTime()*0.001-m_HumanAnimalLastHit<0.65) return false;
        if (HumanAuthority() && HumanIsBlocking() && damageType == DamageType.CLOSE_COMBAT && source)
        {
            EntityAI attacker = source.GetHierarchyRootPlayer();
            if (!attacker) attacker = source;
            vector towardAttacker = attacker.GetPosition() - GetPosition(); towardAttacker[1] = 0; towardAttacker.Normalize();
            vector forward = GetDirection(); forward[1] = 0; forward.Normalize();
            if (vector.Dot(forward, towardAttacker) > 0.5) return false;
        }
        return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
    }
    // Also used by the isolated diagnostic mission; never trusts a client RPC.
    bool HumanApplyMeleeHit(EntityAI target, int kind)
    {
        if (!HumanAuthority() || !IsAlive() || !HumanHostile(target)) return false;
        float reach = HumanReach(kind);
        if (vector.DistanceSq(GetPosition(), target.GetPosition()) > reach * reach || !HumanSight(target) || HumanBlocked(target)) return false;
        vector towardTarget = target.GetPosition() - GetPosition(); towardTarget[1] = 0; towardTarget.Normalize();
        // Fists track the committed target: animation/root turning must not
        // discard an otherwise valid contact. Weapons retain their swing cone.
        if (kind != 0 && vector.Dot(GetDirection(), towardTarget) < 0.5) return false;
        string ammo = HumanAmmo(kind);
        if (ammo == "") return false;
        // CloseCombatDamageName dispatches the ammo's native contact sound/effects
        // and damage/armor processing. Do not layer a second synthetic hit sound.
        string hitComponent = target.GetHitComponentForAI();
        // Normal body punches, using the survivor's light-fist ammo and range.
        // Do not randomly award a headshot multiplier to an unaimed AI punch.
        if (kind == 0)
        {
            hitComponent = "Torso";
            if (PlayerBase.Cast(target))
            {
                hitComponent = "dmgZone_torso";
                ammo = "HumansPlayer_MeleeFist";
            }
        }
        PlayerBase punchedPlayer = PlayerBase.Cast(target);
        float shockBefore, healthBefore;
        if (punchedPlayer) { shockBefore = punchedPlayer.GetHealth("", "Shock"); healthBefore = punchedPlayer.GetHealth("", "Health"); }
        DamageSystem.CloseCombatDamageName(this, target, hitComponent, ammo, target.ModelToWorld(target.GetDefaultHitPosition()));
        // Only the damaging fist strike crossing the knockout threshold adds
        // cuts. Blocked, invulnerable, fatal and already unconscious hits do not.
        if (kind == 0 && punchedPlayer && punchedPlayer.IsAlive() && punchedPlayer.GetHealth("", "Health") < healthBefore && shockBefore >= PlayerConstants.UNCONSCIOUS_THRESHOLD && punchedPlayer.GetHealth("", "Shock") < PlayerConstants.UNCONSCIOUS_THRESHOLD)
        {
            BleedingSourcesManagerServer bleeding = punchedPlayer.GetBleedingManagerServer();
            if (bleeding)
            {
                array<string> cuts = {"LeftForeArmRoll", "RightForeArmRoll", "LeftArm", "RightArm"};
                foreach (string selection : cuts)
                {
                    if (bleeding.GetBleedingSourcesCount() >= 2) break;
                    bleeding.AttemptAddBleedingSourceBySelection(selection);
                }
            }
        }
        return true;
    }
    // HumanFight owns contact damage and weapon audio. Inherited infected event
    // handlers (including sleeping-zombie addons) must not add another hit.
    override void OnDamageEvent(int event_id, string event_user_string) {}
    override void OnSoundEvent(int event_id, string event_user_string) {}
    override void HumansContactEvent(int event_id, string event_user_string)
    {
        if (!HumanAuthority() || !IsAlive() || m_HumanHitDone || m_HumanDoorAction || !GetCommand_Attack()) return;
        m_HumanHitDone = true;
        if (m_HumanSwingKind == HumanActiveWeapon()) HumanApplyMeleeHit(m_HumanSwingTarget, m_HumanSwingKind);
    }
    void HumanPlaySwing(int kind)
    {
        if (GetGame().IsDedicatedServer()) return;
        string sound = HumansWeapons.Sounds[HumansWeapons.ClampKind(kind)];
        EffectSound effect = SEffectManager.PlaySoundOnObject(sound, this);
        if (effect) effect.SetAutodestroy(true);
    }
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
        if (rpc_type != HUMAN_SWING_RPC || HumanAuthority()) return;
        Param1<int> payload;
        if (ctx.Read(payload)) HumanPlaySwing(payload.param1);
    }
    override bool IsZombie() { return false; }
    override bool IsDanger() { return true; }
    override bool CanBeBackstabbed() { return false; }
    override bool HandleVault(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
    {
        if (pCurrentCommandID != DayZInfectedConstants.COMMANDID_VAULT) return false;
        // Small native vaults terminate on the animation graph's exit state.
        // Never enter the infected high-vault/knuckle-run branch.
        if (m_HumanClock - m_HumanVaultStarted > 4) StartCommand_Move();
        return true;
    }
    override bool EvaluateCrawlTransitionAnimation(EntityAI pSource, string pComponent, string pAmmoType, out int pAnimType) { pAnimType = -1; return false; }
    override protected void HandleSoundEvents() {}
    override void OnSoundVoiceEvent(int event_id, string event_user_string) {}
    override void OnStepEvent(int event_id, string event_user_string) {} // Player clips use different event IDs.
    override bool CanReceiveAttachment(EntityAI attachment, int slotId) { return false; }
    override bool CanReleaseCargo(EntityAI cargo) { return !IsAlive() && super.CanReleaseCargo(cargo); }

    int HumanFollowerCount()
    {
        int count;
        foreach (Human_Base human : HumanPopulation)
            if (human && human.IsAlive() && human.m_HumanLeader == this) count++;
        return count;
    }

    void HumanUpdateGroup()
    {
        if (!HumanAuthority() || !IsAlive()) return;
        if (m_HumanLeader)
        {
            if (m_HumanLeader.IsAlive() && !m_HumanLeader.m_HumanLeader && vector.DistanceSq(GetPosition(), m_HumanLeader.GetPosition()) < 1600) return;
            m_HumanLeader = null;
        }
        Human_Base leader;
        int bestId = m_HumanSocialId;
        foreach (Human_Base candidate : HumanPopulation)
        {
            if (!candidate || candidate == this || !candidate.IsAlive() || candidate.HumanIsTrader() || candidate.m_HumanLeader) continue;
            if (candidate.m_HumanSocialId >= bestId || candidate.HumanFollowerCount() >= 5) continue;
            if (vector.DistanceSq(GetPosition(), candidate.GetPosition()) > 256 || !HumanSight(candidate, true)) continue;
            leader = candidate;
            bestId = candidate.m_HumanSocialId;
        }
        // Leaders with followers stay put rather than creating leader chains.
        if (leader && HumanFollowerCount() == 0) m_HumanLeader = leader;
    }

    vector HumanFormationGoal()
    {
        if (!m_HumanLeader) return GetPosition();
        int slot;
        foreach (Human_Base mate : HumanPopulation)
        {
            if (mate == this) break;
            if (mate && mate.IsAlive() && mate.m_HumanLeader == m_HumanLeader) slot++;
        }
        vector forward = m_HumanLeader.GetDirection(); forward[1] = 0; forward.Normalize();
        vector right = Vector(forward[2], 0, -forward[0]);
        float side = -1.8;
        if (slot % 2 == 1) side = 1.8;
        int row = slot / 2;
        return m_HumanLeader.GetPosition() - forward * (3 + row * 2.5) + right * side;
    }

    void HumanGroupPatrol()
    {
        if (m_HumanLeader)
        {
            vector formation = HumanFormationGoal();
            float gap = vector.Distance(GetPosition(), formation);
            if (gap > 1.5)
            {
                float pace = 1;
                if (gap > 8) pace = 2;
                if (gap > 18) pace = 3;
                HumanTravel(formation, pace);
            }
            else HumanFace(GetPosition() + m_HumanLeader.GetDirection() * 5);
            return;
        }
        // Pause briefly for stragglers, but never wait forever for a stuck member.
        bool straggler;
        foreach (Human_Base follower : HumanPopulation)
            if (follower && follower.IsAlive() && follower.m_HumanLeader == this && vector.DistanceSq(GetPosition(), follower.GetPosition()) > 144) straggler = true;
        if (!straggler) m_HumanGroupWaitUntil = 0;
        if (straggler && m_HumanGroupWaitUntil == 0) m_HumanGroupWaitUntil = m_HumanClock + 6;
        if (straggler && m_HumanClock < m_HumanGroupWaitUntil) return;
        if (m_HumanClock >= m_HumanPatrolAt)
        {
            m_HumanPatrolGoal = m_HumanHome + Vector(Math.RandomFloatInclusive(-30, 30), 0, Math.RandomFloatInclusive(-30, 30));
            m_HumanPatrolGoal[1] = GetGame().SurfaceY(m_HumanPatrolGoal[0], m_HumanPatrolGoal[2]);
            m_HumanPatrolAt = m_HumanClock + 25;
            m_HumanPathAt = 0;
        }
        if (vector.DistanceSq(GetPosition(), m_HumanPatrolGoal) > 4) HumanTravel(m_HumanPatrolGoal, 1);
        else if (m_HumanPatrolAt > m_HumanClock + 5) m_HumanPatrolAt = m_HumanClock + 5;
    }

    bool HumanLootEligible(ItemBase item)
    {
        if (!item || item == m_HumanWeapon || item.IsSetForDeletion() || item.IsRuined()) return false;
        if (!item.IsTakeable() || item.IsHologram() || item.IsBeingPlaced()) return false;
        if (item.GetParent() || item.GetHierarchyParent()) return false;
        if (!item.CanPutInCargo(this)) return false;
        InventoryLocation destination = new InventoryLocation;
        return GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.CARGO, destination);
    }

    bool HumanLootVisible(ItemBase item)
    {
        if (!item) return false;
        ref array<Object> ignore = {this};
        if (m_HumanWeapon) ignore.Insert(m_HumanWeapon);
        ref array<ref RaycastRVResult> hits = {};
        RaycastRVParams ray = new RaycastRVParams(GetPosition() + "0 1.2 0", item.GetPosition() + "0 0.1 0", this);
        ray.flags = CollisionFlags.NEARESTCONTACT;
        ray.type = ObjIntersectView;
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0) return true;
        return hits[0].obj == item || hits[0].parent == item;
    }

    bool HumanLootClaimed(ItemBase item)
    {
        foreach (Human_Base human : HumanPopulation)
            if (human && human != this && human.IsAlive() && human.m_HumanLootTarget == item && human.m_HumanClock < human.m_HumanLootUntil) return true;
        return false;
    }

    bool HumanCollectLoot(ItemBase item)
    {
        if (!HumanAuthority() || !IsAlive() || m_HumanEnemy || m_HumanRestPhase != 3) return false;
        if (!HumanLootEligible(item) || HumanLootClaimed(item)) return false;
        if (vector.DistanceSq(GetPosition(), item.GetPosition()) > 2.56 || !HumanLootVisible(item)) return false;
        // Move the existing entity, preserving quantity, condition, attachments and cargo.
        // No spawning/deleting a replacement and no access to another entity's inventory.
        InventoryMode mode = InventoryMode.SERVER;
        if (!GetGame().IsMultiplayer()) mode = InventoryMode.LOCAL;
        return GetInventory().TakeEntityToTargetCargo(mode, this, item);
    }

    void HumanCancelLoot(bool ignoreItem = false)
    {
        if (ignoreItem) { m_HumanIgnoredLoot = m_HumanLootTarget; m_HumanIgnoreLootUntil = m_HumanClock + 30; }
        m_HumanLootTarget = null;
        m_HumanLootPickupAt = 0;
        m_HumanLootAt = m_HumanClock + 4;
    }

    bool HumanLootTick()
    {
        if (m_HumanLeader && vector.DistanceSq(GetPosition(), m_HumanLeader.GetPosition()) > 100) { HumanCancelLoot(); return false; }
        if (m_HumanLootTarget && (m_HumanClock > m_HumanLootUntil || !HumanLootEligible(m_HumanLootTarget))) HumanCancelLoot(true);
        if (!m_HumanLootTarget && m_HumanClock >= m_HumanLootAt)
        {
            m_HumanLootAt = m_HumanClock + 4;
            ref array<Object> objects = {};
            ref array<CargoBase> cargo = {};
            GetGame().GetObjectsAtPosition3D(GetPosition(), 6, objects, cargo);
            float best = 36;
            foreach (Object object : objects)
            {
                ItemBase item = ItemBase.Cast(object);
                if (!HumanLootEligible(item) || HumanLootClaimed(item)) continue;
                if (item == m_HumanIgnoredLoot && m_HumanClock < m_HumanIgnoreLootUntil) continue;
                float distance = vector.DistanceSq(GetPosition(), item.GetPosition());
                if (distance < best && HumanLootVisible(item)) { m_HumanLootTarget = item; best = distance; }
            }
            if (m_HumanLootTarget) { m_HumanLootUntil = m_HumanClock + 12; m_HumanPathAt = 0; }
        }
        if (!m_HumanLootTarget) return false;
        if (vector.DistanceSq(GetPosition(), m_HumanLootTarget.GetPosition()) > 2.25)
        {
            m_HumanLootPickupAt = 0;
            HumanTravel(m_HumanLootTarget.GetPosition(), 1);
        }
        else
        {
            HumanFace(m_HumanLootTarget.GetPosition());
            if (m_HumanLootPickupAt == 0) m_HumanLootPickupAt = m_HumanClock + 0.65;
            if (m_HumanClock >= m_HumanLootPickupAt)
            {
                bool collected = HumanCollectLoot(m_HumanLootTarget);
                HumanCancelLoot(!collected);
            }
        }
        return true;
    }

    bool HumanCanOpenDoor(Building building, int door)
    {
        if (!building || door < 0 || door >= building.GetDoorCount()) return false;
        if (building.IsDoorOpen(door) || building.IsDoorLocked(door)) return false;
        if (vector.DistanceSq(GetPosition(), building.GetDoorSoundPos(door)) > 6.25) return false;
        DoorManipulationParams params = new DoorManipulationParams;
        params.m_DoorIndex = door;
        params.m_CheckIfLocked = true;
        params.m_Caller = this;
        return building.CanDoorBeOpened(params);
    }

    bool HumanOpenDoor(Building building, int door)
    {
        if (!HumanAuthority() || !IsAlive() || !HumanCanOpenDoor(building, door)) return false;
        building.OpenDoor(door); // Native door replication, sound and pathgraph update.
        m_HumanPath.Clear();
        m_HumanPathAt = 0;
        return building.IsDoorOpen(door);
    }

    bool HumanTryDoor(vector goal)
    {
        if (!PlayerBase.Cast(m_HumanEnemy) || m_HumanDoorAction || m_HumanClock < m_HumanDoorScanAt) return false;
        m_HumanDoorScanAt = m_HumanClock + 0.5;
        vector direction = goal - GetPosition(); direction[1] = 0;
        if (direction.LengthSq() < 0.01) return false;
        direction.Normalize();
        ref array<Object> ignore = {this};
        if (m_HumanWeapon) ignore.Insert(m_HumanWeapon);
        ref array<ref RaycastRVResult> hits = {};
        vector start = GetPosition() + "0 1 0";
        RaycastRVParams ray = new RaycastRVParams(start, start + direction * 2.4, this);
        ray.flags = CollisionFlags.NEARESTCONTACT;
        ray.type = ObjIntersectView;
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0) return false;
        Building building = Building.Cast(hits[0].obj);
        if (!building) building = Building.Cast(hits[0].parent);
        if (!building) return false;
        int door = building.GetDoorIndex(hits[0].component);
        return HumanInteractDoor(building, door);
    }

    bool HumanInteractDoor(Building building, int door)
    {
        if (!HumanAuthority() || !IsAlive() || m_HumanDoorAction || !HumanCanOpenDoor(building, door)) return false;
        m_HumanDoorBuilding = building;
        m_HumanDoorIndex = door;
        m_HumanDoorAction = true;
        m_HumanDoorOpenAt = m_HumanClock + 0.45;
        m_HumanHitDone = true;
        m_HumanSwingSoundDone = true;
        HumanFace(building.GetDoorSoundPos(door));
        GetInputController().OverrideMovementSpeed(true, 0);
        StartCommand_Attack(building, 3, 1); // Dedicated player door-opening clip, no damage.
        return true;
    }

    bool HumanHostile(EntityAI target)
    {
        if (!target || target == this || !target.IsAlive() || Human_Base.Cast(target)) return false;
        return PlayerBase.Cast(target) != null || ZombieBase.Cast(target) != null || AnimalBase.Cast(target) != null;
    }

    bool HumanSight(EntityAI target, bool groupScan = false)
    {
        if (!target) return false;
        ref array<Object> ignore = {this};
        if (m_HumanWeapon) ignore.Insert(m_HumanWeapon);
        if (groupScan)
        {
            foreach (Human_Base friend : HumanPopulation)
            {
                if (!friend || friend == target || friend == this) continue;
                ignore.Insert(friend);
                if (friend.m_HumanWeapon) ignore.Insert(friend.m_HumanWeapon);
            }
        }
        ref array<ref RaycastRVResult> hits = {};
        RaycastRVParams ray = new RaycastRVParams(GetPosition() + "0 1.5 0", target.GetPosition() + "0 1.1 0", this);
        ray.flags = CollisionFlags.NEARESTCONTACT;
        ray.type = ObjIntersectView;
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0) return true;
        return hits[0].obj == target || hits[0].parent == target;
    }

    void HumanScan()
    {
        ref array<Object> objects = {};
        ref array<CargoBase> cargo = {};
        ref array<Man> players = {};
        GetGame().GetObjectsAtPosition3D(GetPosition(), 80, objects, cargo);
        GetGame().GetPlayers(players);
        foreach (Man player : players) { if (objects.Find(player) < 0) objects.Insert(player); }
        Object localPlayer = GetGame().GetPlayer();
        if (localPlayer && objects.Find(localPlayer) < 0) objects.Insert(localPlayer);
        float best = 80 * 80;
        EntityAI nearest;
        foreach (Object object : objects)
        {
            EntityAI candidate = EntityAI.Cast(object);
            if (!HumanHostile(candidate)) continue;
            float distance = vector.DistanceSq(GetPosition(), candidate.GetPosition());
            if (distance < 30 * 30) m_HumanUnsafeUntil = m_HumanClock + 20;
            HumansMemory memory = HumanTacticalBrain().Recall(candidate);
            float visualRange = 45; if (memory || candidate == m_HumanEnemy) visualRange = 80;
            vector toward = candidate.GetPosition() - GetPosition(); toward[1] = 0; toward.Normalize();
            bool inView = vector.Dot(GetDirection(), toward) > -0.25 || distance < 9 || memory != null;
            float score = distance; if (candidate == m_HumanEnemy) score *= 0.7; else if (memory) score *= 0.85;
            if (distance <= visualRange * visualRange && inView && score < best && HumanSight(candidate)) { nearest = candidate; best = score; }
        }
        if (nearest)
        {
            m_HumanEnemy = nearest;
            m_HumanSeen = m_HumanClock;
            m_HumanLastKnown = nearest.GetPosition();
            HumanTacticalBrain().Remember(nearest, m_HumanLastKnown);
        }
        if (!nearest && m_HumanClock >= m_HumanHearingAt)
        {
            float heardDistance = 20 * 20;
            PlayerBase heardPlayer;
            foreach (Man listenerCandidate : players)
            {
                PlayerBase movingPlayer = PlayerBase.Cast(listenerCandidate);
                if (!movingPlayer || !movingPlayer.IsAlive() || movingPlayer.GetCommand_Vehicle()) continue;
                HumanMovementState state = new HumanMovementState;
                movingPlayer.GetMovementState(state);
                float playerDistance = vector.DistanceSq(GetPosition(), movingPlayer.GetPosition());
                if (HumanAudibleMovement(state.m_iStanceIdx, state.m_iMovement, playerDistance) && playerDistance <= heardDistance)
                {
                    heardPlayer = movingPlayer;
                    heardDistance = playerDistance;
                }
            }
            if (heardPlayer) { HumanHear(heardPlayer.GetPosition()); HumanTacticalBrain().Remember(heardPlayer, heardPlayer.GetPosition()); }
        }
    }

    static bool HumanAudibleMovement(int stance, int movement, float distanceSq)
    {
        if (distanceSq > 400 || movement <= DayZPlayerConstants.MOVEMENTIDX_IDLE) return false;
        if (stance == DayZPlayerConstants.STANCEIDX_PRONE || stance == DayZPlayerConstants.STANCEIDX_RAISEDPRONE) return false;
        bool crouched = stance == DayZPlayerConstants.STANCEIDX_CROUCH || stance == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH;
        return !crouched || movement > DayZPlayerConstants.MOVEMENTIDX_WALK;
    }

    void HumanHear(vector position)
    {
        m_HumanHeardPosition = position;
        m_HumanInvestigateUntil = m_HumanClock + 12;
        m_HumanHearingAt = m_HumanClock + 0.4;
        m_HumanUnsafeUntil = m_HumanClock + 20;
        m_HumanWake = true;
        HumanCancelLoot();
    }

    void HumanFace(vector point)
    {
        vector delta = point - GetPosition();
        delta[1] = 0;
        if (delta.LengthSq() < 0.01) return;
        GetInputController().OverrideTurnSpeed(true, Math.PI2 * 2);
        GetInputController().OverrideHeading(true, Math.Atan2(delta[0], delta[2]));
        GetInputController().OverrideLookAt(true, delta.Normalized());
    }

    bool HumanCanRest()
    {
        if (m_HumanEnemy || m_HumanClock < m_HumanUnsafeUntil || m_HumanWasGrouped) return false;
        if (GetGame().GetWeather().GetRain().GetActual() > 0.01) return false;
        vector pos = GetPosition();
        string surface;
        GetGame().SurfaceGetType3D(pos[0], pos[1], pos[2], surface);
        surface.ToLower();
        if (surface.Contains("road") || surface.Contains("asphalt") || surface.Contains("concrete") || surface.Contains("pavement")) return false;
        return true;
    }

    bool HumanClearSegment(vector start, vector end, float radius = 0.25, bool ignoreTraffic = false)
    {
        ref array<Object> ignore = {this};
        if (m_HumanWeapon) ignore.Insert(m_HumanWeapon);
        if (ignoreTraffic)
        {
            foreach (Human_Base human : HumanPopulation)
            {
                if (!human || human == this) continue;
                ignore.Insert(human);
                if (human.m_HumanWeapon) ignore.Insert(human.m_HumanWeapon);
            }
            if (m_HumanEnemy) ignore.Insert(m_HumanEnemy);
        }
        ref array<ref RaycastRVResult> hits = {};
        RaycastRVParams ray = new RaycastRVParams(start, end, this, radius);
        ray.type = ObjIntersectGeom;
        return !DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0;
    }

    bool HumanVaultObstacle(Object obstacle)
    {
        if (!obstacle || obstacle == this) return false;
        EntityAI entity = EntityAI.Cast(obstacle);
        if (entity && entity.GetHierarchyRoot() == this) return false;
        if (Man.Cast(obstacle) || DayZCreatureAI.Cast(obstacle) || Transport.Cast(obstacle)) return false;
        if (ItemBase.Cast(obstacle) && !BaseBuildingBase.Cast(obstacle)) return false;
        return true;
    }

    bool HumanTryVault(vector goal)
    {
        if (m_HumanClock < m_HumanVaultAt || m_HumanRestPhase != 3 || m_HumanDoorAction || m_HumanVaulting || m_HumanVaultPending || !GetCommand_Move()) return false;
        m_HumanVaultAt = m_HumanClock + 0.35;
        vector start = GetPosition();
        vector forward = goal - start; forward[1] = 0;
        if (forward.LengthSq() < 4.84) return false;
        forward.Normalize();
        ref array<Object> ignore = {this};
        if (m_HumanWeapon) ignore.Insert(m_HumanWeapon);
        // Attached weapons are visual children, not map obstacles.
        IEntity held = GetChildren();
        while (held) { Object heldObject = Object.Cast(held); if (heldObject) ignore.Insert(heldObject); held = held.GetSibling(); }
        ref array<ref RaycastRVResult> hits = {};
        RaycastRVParams ray = new RaycastRVParams(start + "0 0.45 0", start + forward * 3 + "0 0.45 0", this, 0.12);
        ray.type = ObjIntersectGeom;
        ray.flags = CollisionFlags.NEARESTCONTACT;
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0 || !hits[0].obj) return false;
        Object obstacle = hits[0].obj;
        if (!HumanVaultObstacle(obstacle)) return false;
        float obstacleDistance = vector.Dot(hits[0].pos - start, forward);
        if (obstacleDistance < 0.3 || obstacleDistance > 2.8) return false;
        // Require the same solid obstacle above shin height; loose items,
        // other creatures and tiny ground clutter must never start a vault.
        array<ref RaycastRVResult> upperHits = {};
        RaycastRVParams upperRay = new RaycastRVParams(start + "0 0.65 0", start + forward * 3 + "0 0.65 0", this, 0.08);
        upperRay.type = ObjIntersectGeom; upperRay.flags = CollisionFlags.NEARESTCONTACT;
        if (!DayZPhysics.RaycastRVProxy(upperRay, upperHits, ignore) || upperHits.Count() == 0) return false;
        if (upperHits[0].obj != obstacle && upperHits[0].parent != obstacle) return false;
        if (HumanNavigation().WalkClear(start, start + forward * Math.Min(obstacleDistance + 0.25, 3), true)) return false;
        vector launch = hits[0].pos - forward * 0.7; launch[1] = start[1];
        if (vector.Dot(launch - start, forward) < 0) launch = start;
        vector launchSample;
        if (!GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(launch, 0.35, m_HumanFilter, launchSample)) return false;
        if (Math.AbsFloat(launchSample[1] - start[1]) > 0.3) return false;
        launch = launchSample;
        // Check the entire crossing before approaching, so a tall wall cannot
        // lure the actor away from a usable route around it.
        if (!HumanClearSegment(launch + "0 1.25 0", launch + forward * 2.2 + "0 1.25 0", 0.18)) return false;
        vector landing = launch + forward * 2.2;
        vector sampled;
        if (!GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(landing, 0.4, m_HumanFilter, sampled)) return false;
        if (Math.AbsFloat(sampled[1] - start[1]) > 0.3 || vector.DistanceSq(sampled, landing) > 0.16) return false;
        if (!HumanClearSegment(sampled + "0 0.35 0", sampled + "0 1.8 0", 0.3)) return false;
        if (!HumanClearSegment(launch + "0 1.9 0", sampled + "0 1.9 0", 0.3)) return false;
        if (!HumanVaultClear(launch, sampled)) return false;
        if (vector.DistanceSq(start, launch) > 0.0625)
        {
            if (!HumanNavigation().WalkClear(start, launch, true)) return false;
            HumanNavigation().Steer(launch, 1);
            m_HumanVaultAt = m_HumanClock + 0.1;
            return true;
        }
        if (vector.Dot(forward, GetDirection()) < 0.95)
        {
            HumanFace(start + forward);
            m_HumanVaultAt = m_HumanClock + 0.1;
            return true;
        }
        if (!HumanVaultClear(start, sampled)) return false;
        m_HumanVaultAt = m_HumanClock + 6;
        m_HumanVaultStarted = m_HumanClock;
        m_HumanVaultLanding = sampled;
        m_HumanVaultType = 1;
        m_HumanVaultPending = true;
        return true;
    }

    static vector HumanVaultPosition(vector start, vector end, float t)
    {
        // Lift before crossing the near face, then land beyond the far face.
        float travel = Math.Clamp((t - 0.2) / 0.6, 0, 1);
        travel = travel * travel * (3 - 2 * travel);
        float rise = Math.Clamp(t / 0.25, 0, 1);
        float fall = Math.Clamp((1 - t) / 0.25, 0, 1);
        rise = rise * rise * (3 - 2 * rise);
        fall = fall * fall * (3 - 2 * fall);
        vector position = start + (end - start) * travel;
        position[1] = position[1] + Math.Min(rise, fall) * 1.2;
        return position;
    }

    bool HumanVaultClear(vector start, vector end)
    {
        vector previous = start;
        for (int step = 1; step <= 12; step++)
        {
            vector next = HumanVaultPosition(start, end, step / 12.0);
            // Sweep the lower body, torso and head along the actual trajectory.
            if (!HumanClearSegment(previous + "0 0.35 0", next + "0 0.35 0", 0.25)) return false;
            if (!HumanClearSegment(previous + "0 1 0", next + "0 1 0", 0.3)) return false;
            if (!HumanClearSegment(previous + "0 1.65 0", next + "0 1.65 0", 0.25)) return false;
            previous = next;
        }
        return true;
    }

    HumansNavigation HumanNavigation()
    {
        if (!m_HumanNavigation) m_HumanNavigation = new HumansNavigation(this);
        return m_HumanNavigation;
    }

    bool HumanTryVehicleClimb(vector goal)
    {
        if (!PlayerBase.Cast(m_HumanEnemy)) return false;
        if (!m_HumanVehicle) m_HumanVehicle = new HumansVehicleClimb(this);
        return m_HumanVehicle.TryClimb(goal);
    }

    void HumanRecover(vector goal)
    {
        HumanNavigation().Recover(goal);
    }

    bool HumanRangedTick(float dt, int command) { return false; }

    void HumanTravel(vector goal, float speed)
    {
        HumanNavigation().Travel(goal, speed);
    }

    override bool ModCommandHandlerBefore(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        if (!HumanAuthority() || !IsAlive()) return false;
        DayZInfectedInputController pInputController = GetInputController();
        if (!pInputController) return false;
        m_HumanClock += pDt;
        if (m_HumanNavigation && m_HumanNavigation.Door) m_HumanNavigation.Heartbeat = m_HumanNavigation.Now();
        if (m_HumanVaulting && pCurrentCommandID == DayZInfectedConstants.COMMANDID_SCRIPT) return false;
        if (m_HumanVaulting) { m_HumanVaulting = false; m_HumanPathAt = 0; m_HumanProgressAt = m_HumanClock; m_HumanProgressPosition = GetPosition(); }
        if (m_HumanVaultPending && pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE)
        {
            m_HumanVaultPending = false;
            m_HumanVaulting = true;
            StartCommand_Script(new HumansVaultCommand(this));
            return true;
        }
        pInputController.OverrideMovementSpeed(true, 0);
        pInputController.OverrideJump(true, 0);
        pInputController.OverrideLookAt(false, vector.Zero);
        if (!m_HumanReady)
        {
            m_HumanReady = true;
            m_HumanHome = GetPosition();
            m_HumanStageEnd = m_HumanClock + Math.RandomFloatInclusive(15, 30);
            m_HumanGroupAt = m_HumanClock + Math.RandomFloatInclusive(0, 2);
            m_HumanLootAt = m_HumanClock + Math.RandomFloatInclusive(1, 4);
        }
        if (m_HumanClock >= m_HumanGroupAt)
        {
            HumanUpdateGroup();
            m_HumanGroupAt = m_HumanClock + 2;
            bool nextGrouped = m_HumanLeader != null || HumanFollowerCount() > 0;
            if (nextGrouped && !m_HumanWasGrouped) { m_HumanPatrolAt = 0; m_HumanPathAt = 0; }
            m_HumanWasGrouped = nextGrouped;
        }
        bool grouped = m_HumanWasGrouped;
        if (grouped) m_HumanWake = true;
        if (m_HumanClock >= m_HumanScanAt) { HumanScan(); m_HumanScanAt = m_HumanClock + 0.15; }
        HumanTacticalBrain().UpdateFocus();
        HumanUpdateBlock();
        if (HumanRangedTick(pDt, pCurrentCommandID)) return false;
        if (HumanTacticalBrain().DodgeTick()) return false;
        if (m_HumanVehicle && m_HumanVehicle.Tick(pDt)) return false;
        if (HumanIsBlocking())
        {
            HumanFace(m_HumanEnemy.GetPosition());
            return false;
        }
        if (m_HumanRestPhase < 3 && !HumanCanRest()) m_HumanWake = true;
        if (m_HumanEnemy)
        {
            HumanCancelLoot();
            m_HumanWake = true;
            m_HumanStage = 0;
            m_HumanStageEnd = m_HumanClock + 20;
        }
        m_HumanRestTime += pDt;
        float enterTime = 62.0 / 30.0 + 0.45;
        float exitTime = 52.0 / 30.0 + 0.45;
        if (m_HumanRestStyle == 0) { enterTime = 87.0 / 30.0 + 0.45; exitTime = 64.0 / 30.0 + 0.45; }
        if (m_HumanRestPhase == 1 && m_HumanRestTime >= enterTime) { m_HumanRestPhase = 0; m_HumanRestTime = 0; }
        if (m_HumanRestPhase == 0 && (m_HumanWake || m_HumanClock >= m_HumanStageEnd)) { m_HumanRestPhase = 2; m_HumanRestTime = 0; }
        if (m_HumanRestPhase == 2 && m_HumanRestTime >= exitTime)
        {
            m_HumanRestPhase = 3;
            pInputController.OverrideTurnSpeed(false, 0);
            pInputController.OverrideHeading(false, 0);
        }
        if (m_HumanRestPhase < 3)
        {
            pInputController.OverrideTurnSpeed(true, 0);
            pInputController.OverrideHeading(true, m_HumanHeading);
            return false;
        }
        if (pCurrentCommandID != DayZInfectedConstants.COMMANDID_MOVE) return false;
        if (m_HumanNavigation && m_HumanNavigation.Door)
        {
            vector trafficGoal = m_HumanPathGoal;
            if (m_HumanEnemy) trafficGoal = m_HumanLastKnown;
            if (m_HumanNavigation.DoorTick(trafficGoal, trafficGoal)) return false;
        }
        if (m_HumanEnemy)
        {
            vector combatGoal; float combatSpeed;
            bool tactical = HumanTacticalBrain().CombatGoal(combatGoal, combatSpeed);
            if (!tactical) combatGoal = m_HumanLastKnown;
            HumanFace(combatGoal);
            float distance = vector.Distance(GetPosition(), combatGoal);
            float stopRange = HumanReach(HumanActiveWeapon()) - 0.2;
            if (tactical) stopRange = 0.3;
            if (distance > stopRange)
            {
                float speed = 3;
                if (distance < 12) speed = 2;
                if (distance < 3) speed = 2;
                if (tactical) speed = combatSpeed;
                HumanTravel(combatGoal, speed);
            }
        }
        else
        {
            if (m_HumanClock < m_HumanInvestigateUntil)
            {
                if (vector.DistanceSq(GetPosition(), m_HumanHeardPosition) > 2.25) HumanTravel(m_HumanHeardPosition, 1);
                else HumanFace(m_HumanHeardPosition);
                return false;
            }
            if (HumanLootTick()) return false;
            if (grouped) { HumanGroupPatrol(); return false; }
            if (m_HumanClock >= m_HumanStageEnd)
            {
                m_HumanStage = (m_HumanStage + 1) % 4;
                m_HumanStageEnd = m_HumanClock + Math.RandomFloatInclusive(20, 35);
                if (m_HumanStage == 1 || m_HumanStage == 2)
                {
                    if (!HumanCanRest()) { m_HumanStage = 0; return false; }
                    m_HumanRestStyle = 2 - m_HumanStage;
                    m_HumanRestPhase = 1;
                    m_HumanRestTime = 0;
                    m_HumanWake = false;
                    m_HumanHeading = pInputController.GetHeading();
                }
                if (m_HumanStage == 3) m_HumanGoal = m_HumanHome + Vector(Math.RandomFloatInclusive(-12, 12), 0, Math.RandomFloatInclusive(-12, 12));
            }
            if (m_HumanStage == 3) HumanTravel(m_HumanGoal, 1);
        }
        return false;
    }

    override bool HandleMindStateChange(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
    {
        DayZInfectedCommandMove move = GetCommand_Move();
        if (move)
        {
            m_StanceVariation = 2;
            if (HumansWeapons.Family(HumanActiveWeapon()) == 1) m_StanceVariation = 0;
            if (HumansWeapons.Family(HumanActiveWeapon()) == 2) m_StanceVariation = 1;
            int idle = 2;
            if (m_HumanRestPhase < 3) m_StanceVariation = m_HumanRestStyle;
            if (m_HumanRestPhase < 2) idle = 0;
            if (m_HumanRestPhase == 2) idle = 1;
            if (HumanIsBlocking())
            {
                m_StanceVariation = 3;
                idle = 0;
                if (HumansWeapons.Family(HumanActiveWeapon()) == 2) idle = 2;
            }
    
            move.SetStanceVariation(m_StanceVariation);
            move.SetIdleState(idle);
        }
        return false;
    }

    override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
    {
        if (m_HumanVaulting || HumanIsBlocking() || m_HumanClock < HumanTacticalBrain().DodgeUntil) return true;
        if (m_HumanNavigation && m_HumanNavigation.Door && m_HumanNavigation.LeaseUntil > 0 && !m_HumanDoorAction) return true;
        if (!HumanAuthority()) return true;
        if (m_HumanDoorAction)
        {
            // StartCommand_Attack may have replaced MOVE during the pre-handler;
            // the command id argument still describes the previous command this tick.
            if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE && GetCommand_Attack()) return true;
            if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
            {
                if (m_HumanClock >= m_HumanDoorOpenAt && m_HumanDoorBuilding)
                {
                    HumanOpenDoor(m_HumanDoorBuilding, m_HumanDoorIndex);
                    m_HumanDoorBuilding = null;
                    m_HumanDoorScanAt = m_HumanClock + 1;
                }
                return true;
            }
            m_HumanDoorAction = false;
            m_HumanDoorBuilding = null;
        }
        if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
        {
            if (!m_HumanSwingSoundDone && m_HumanClock >= m_HumanSoundAt)
            {
                m_HumanSwingSoundDone = true;
                if (GetGame().IsMultiplayer()) GetGame().RPCSingleParam(this, HUMAN_SWING_RPC, new Param1<int>(m_HumanSwingKind), true);
                else HumanPlaySwing(m_HumanSwingKind);
            }
            if (!m_HumanHitDone && m_HumanClock >= m_HumanHitAt)
            {
                m_HumanHitDone = true;
                if (m_HumanSwingKind == HumanActiveWeapon()) HumanApplyMeleeHit(m_HumanSwingTarget, m_HumanSwingKind);
            }
            return true;
        }
        m_HumanHitDone = true;
        m_HumanSwingSoundDone = true;
        float reach = HumanReach(HumanActiveWeapon());
        if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE && m_HumanRestPhase == 3 && HumanHostile(m_HumanEnemy) && m_HumanClock >= m_HumanAttackAt && vector.DistanceSq(GetPosition(), m_HumanEnemy.GetPosition()) < reach * reach && HumanSight(m_HumanEnemy) && !HumanBlocked(m_HumanEnemy))
        {
            HumanFace(m_HumanEnemy.GetPosition());
            pInputController.OverrideMovementSpeed(true, 0);
            m_HumanSwingTarget = m_HumanEnemy;
            m_HumanSwingKind = HumanActiveWeapon();
            // Stable graph families: 0=fists, 1=vanilla 1h-pipe, 2=vanilla 2h-bat.
            StartCommand_Attack(m_HumanSwingTarget, HumansWeapons.Family(m_HumanSwingKind), 1);
            // Native command completion still governs recovery; this timer no
            // longer adds a long pause after the light punch has finished.
            m_HumanAttackAt = m_HumanClock + 0.7;
            if (m_HumanSwingKind != 0) m_HumanAttackAt = m_HumanClock + 0.85;
            m_HumanHitAt = m_HumanClock + HumansWeapons.HitTimes[m_HumanSwingKind];
            m_HumanSoundAt = m_HumanClock + HumansWeapons.SoundTimes[m_HumanSwingKind];
            m_HumanSwingSoundDone = false;
            m_HumanHitDone = false;
            m_HumanAttackCue++;
            SetSynchDirty();
            if (!GetGame().IsMultiplayer()) HumanVoice("Attack");
        }
        return true;
    }

    override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
    {
        super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
        if (!HumanAuthority()) return;
        AnimalBase animalSource=AnimalBase.Cast(source);
        if(animalSource){m_HumanAnimalLastSource=animalSource;m_HumanAnimalLastHit=GetGame().GetTime()*0.001;}
        m_HumanBlockUntil = 0; // An unguarded hit interrupts the defensive pose.
        if (m_DamageHitToProcess && !m_DamageHitHeavy) m_ShockDamage = 100;
        if (m_HumanRestPhase < 3) { m_DamageHitToProcess = false; m_HumanWake = true; }
        EntityAI attacker;
        if (source) { attacker = source.GetHierarchyRootPlayer(); if (!attacker) attacker = source; }
        if (HumanHostile(attacker)) { m_HumanEnemy = attacker; m_HumanLastKnown = attacker.GetPosition(); m_HumanSeen = m_HumanClock; HumanTacticalBrain().Remember(attacker, m_HumanLastKnown); HumanTacticalBrain().AlertUntil = m_HumanClock + 60; }
        m_HumanPainCue++;
        SetSynchDirty();
        if (!GetGame().IsMultiplayer()) HumanVoice("Pain");
    }

    void HumanVoice(string cue)
    {
        if (GetGame().IsDedicatedServer()) return;
        string sex = "Male";
        if (ConfigGetBool("woman")) sex = "Female";
        ProcessVoiceFX("Humans_" + sex + cue + "_SoundSet");
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        if (m_HumanAttackCue != m_HumanLastAttackCue) HumanVoice("Attack");
        if (m_HumanPainCue != m_HumanLastPainCue) HumanVoice("Pain");
        m_HumanLastAttackCue = m_HumanAttackCue;
        m_HumanLastPainCue = m_HumanPainCue;
    }
    override void EEKilled(Object killer)
    {
        m_HumanLootTarget = null;
        m_HumanLeader = null;
        super.EEKilled(killer);
        HumanDropWeapon();
        HumanVoice("Death");
    }
    void HumanDropWeapon()
    {
        if (!HumanAuthority() || IsAlive() || !m_HumanWeapon) return;
        ItemBase weapon = m_HumanWeapon;
        GetGame().RemoteObjectTreeDelete(weapon);
        RemoveChild(weapon, false);
        vector transform[4];
        Math3D.MatrixIdentity4(transform);
        transform[3] = GetPosition() + "0 0.15 0";
        weapon.SetTransform(transform);
        weapon.PlaceOnSurface();
        weapon.SetTakeable(true);
        weapon.SetAllowDamage(true);
        weapon.SetLifetimeMax(14400);
        weapon.SetLifetime(14400);
        m_HumanWeapon = null; // Corpse cleanup must never delete the released loot.
        GetGame().RemoteObjectTreeCreate(weapon);
    }

}

class HumanM_HunterOld_Summer extends Human_Base {}
class HumanM_Jacket_grey extends Human_Base {}
class HumanM_JournalistSkinny extends Human_Base {}
class HumanF_SurvivorNormal_Blue extends Human_Base {}
class HumanM_SoldierNormal extends Human_Base
{
    override void HumanInitLoadout()
    {
        if (HumanAuthority() && IsAlive()) HumanEquip(12);
    }
}
class HumanM_SoldierNormal_Female extends Human_Base
{
    override void HumanInitLoadout()
    {
        if (HumanAuthority() && IsAlive()) HumanEquip(12);
    }
}





