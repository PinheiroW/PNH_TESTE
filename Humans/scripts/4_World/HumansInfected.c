// Infected cannot acquire another infected-engine actor through their native senses.
// Supply only Human targets; normal player acquisition and native attack clips remain intact.
modded class ZombieBase
{
    Human_Base m_HumansTarget;
    float m_HumansScan;
    float m_HumansPathAt;
    float m_HumansLastSeen;
    bool m_HumansDriving;
    bool m_HumansContactDone;
    ref array<vector> m_HumansPath = {};
    int m_HumansWaypoint;
    ref PGFilter m_HumansFilter;

    void ZombieBase()
    {
        RegisterAnimationEvent("CombatHit", "HumansContactEvent");
    }

    void HumansContactEvent(int event_id, string event_user_string)
    {
        if ((!GetGame().IsServer() && GetGame().IsMultiplayer()) || !IsAlive() || m_HumansContactDone || !GetCommand_Attack()) return;
        Human_Base victim = Human_Base.Cast(m_ActualTarget);
        if (!victim || !victim.IsAlive() || !m_ActualAttackType || !victim.HumanSight(this)) return;
        if (vector.DistanceSq(GetPosition(), victim.GetPosition()) > m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance) return;
        m_HumansContactDone = true;
        DamageSystem.CloseCombatDamageName(this, victim, victim.GetHitComponentForAI(), m_ActualAttackType.m_AmmoType, victim.ModelToWorld(victim.GetDefaultHitPosition()));
    }

#ifdef VanillaSleepingZombies
    override void HZComboContact(int generation)
    {
        Human_Base victim = Human_Base.Cast(m_ActualTarget);
        if (!victim) { super.HZComboContact(generation); return; }
        if (m_HumansContactDone) return;
        if ((!GetGame().IsServer() && GetGame().IsMultiplayer()) || !IsAlive() || generation != m_HZAttackGeneration || !GetCommand_Attack()) return;
        if (m_HZSingleAttack && m_HZContactUsed) return;
        if (m_HZSingleAttack) m_HZContactUsed = true;
        if (!victim.IsAlive() || !m_ActualAttackType || !victim.HumanSight(this)) return;
        if (vector.DistanceSq(GetPosition(), victim.GetPosition()) > m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance) return;
        m_HumansContactDone = true;
        DamageSystem.CloseCombatDamageName(this, victim, victim.GetHitComponentForAI(), m_ActualAttackType.m_AmmoType, victim.ModelToWorld(victim.GetDefaultHitPosition()));
    }
#endif

    override bool ModCommandHandlerBefore(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
    {
        if (Human_Base.Cast(this) || (!GetGame().IsServer() && GetGame().IsMultiplayer())) return super.ModCommandHandlerBefore(pDt, pCurrentCommandID, pCurrentCommandFinished);
        DayZInfectedInputController input = GetInputController();
        if (!input) return false;
        float now = GetGame().GetTime() * 0.001;
        if (now >= m_HumansScan)
        {
            m_HumansScan = now + 0.5;
            float best = 30 * 30;
            EntityAI nativeTarget = input.GetTargetEntity();
            if (nativeTarget && nativeTarget.IsAlive()) best = Math.Min(best, vector.DistanceSq(GetPosition(), nativeTarget.GetPosition()));
            Human_Base selected;
            foreach (Human_Base human : Human_Base.HumanPopulation)
            {
                if (!human || !human.IsAlive() || human.HumanIsTrader()) continue;
                float distance = vector.DistanceSq(GetPosition(), human.GetPosition());
                if (distance <= best && human.HumanSight(this)) { selected = human; best = distance; }
            }
            if (selected) { m_HumansTarget = selected; m_HumansLastSeen = now; }
            else if (nativeTarget || now - m_HumansLastSeen > 5) m_HumansTarget = null;
        }
        if (!IsAlive() || m_IsCrawling || (m_HumansTarget && !m_HumansTarget.IsAlive())) m_HumansTarget = null;
        // Optional integration: let the installed sleeping mod play its get-up clip.
        // This conditional does not require that addon on standalone servers.
#ifdef VanillaSleepingZombies
        if (m_HumansTarget) m_HZWakeRequested = true;
#endif
        if (super.ModCommandHandlerBefore(pDt, pCurrentCommandID, pCurrentCommandFinished)) return true;
        if (!m_HumansTarget)
        {
            if (m_HumansDriving)
            {
                input.OverrideMovementSpeed(false, 0);
                input.OverrideHeading(false, 0);
                input.OverrideTurnSpeed(false, 0);
                input.OverrideAlertLevel(false, false, 0, 0);
                m_HumansDriving = false;
            }
            return false;
        }
        m_HumansDriving = true;
        input.OverrideAlertLevel(true, true, 3, 1);
        input.OverrideMovementSpeed(true, 0);
        if (pCurrentCommandID != DayZInfectedConstants.COMMANDID_MOVE) return false;
        vector goal = m_HumansTarget.GetPosition();
        vector point = goal;
        float range = vector.Distance(GetPosition(), goal);
        if (range > 1.3)
        {
            if (!m_HumansFilter)
            {
                m_HumansFilter = new PGFilter;
                m_HumansFilter.SetFlags(PGPolyFlags.WALK | PGPolyFlags.INSIDE | PGPolyFlags.DOOR, PGPolyFlags.DISABLED | PGPolyFlags.SWIM | PGPolyFlags.LADDER | PGPolyFlags.JUMP | PGPolyFlags.CLIMB, 0);
            }
            if (now >= m_HumansPathAt)
            {
                m_HumansPathAt = now + 1;
                m_HumansPath.Clear();
                m_HumansWaypoint = 1;
                GetGame().GetWorld().GetAIWorld().FindPath(GetPosition(), goal, m_HumansFilter, m_HumansPath);
            }
            while (m_HumansWaypoint < m_HumansPath.Count() && vector.DistanceSq(GetPosition(), m_HumansPath[m_HumansWaypoint]) < 0.16) m_HumansWaypoint++;
            if (m_HumansWaypoint >= m_HumansPath.Count()) return false;
            point = m_HumansPath[m_HumansWaypoint];
        }
        vector direction = point - GetPosition(); direction[1] = 0; direction.Normalize();
        input.OverrideTurnSpeed(true, Math.PI2);
        input.OverrideHeading(true, Math.Atan2(direction[0], direction[2]));
        if (range > 1.3) input.OverrideMovementSpeed(true, 2 * Math.Max(0, vector.Dot(direction, GetDirection())));
        return false;
    }

    override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
    {
        if (!m_HumansTarget || Human_Base.Cast(this)) return super.FightLogic(pCurrentCommandID, pInputController, pDt);
        // Native WasHit contact timing and damage processing use these same fields.
        if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
        {
#ifdef VanillaSleepingZombies
            return super.FightLogic(pCurrentCommandID, pInputController, pDt);
#endif
            if (!Human_Base.Cast(m_ActualTarget)) return super.FightLogic(pCurrentCommandID, pInputController, pDt);
            DayZInfectedCommandAttack attack = GetCommand_Attack();
            if (attack && attack.WasHit() && !m_HumansContactDone)
            {
                m_HumansContactDone = true;
                Human_Base victim = Human_Base.Cast(m_ActualTarget);
                if (victim.IsAlive() && victim.HumanSight(this) && m_ActualAttackType && vector.DistanceSq(GetPosition(), victim.GetPosition()) <= m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance)
                    DamageSystem.CloseCombatDamageName(this, victim, victim.GetHitComponentForAI(), m_ActualAttackType.m_AmmoType, victim.ModelToWorld(victim.GetDefaultHitPosition()));
            }
            return true;
        }
        if (pCurrentCommandID != DayZInfectedConstants.COMMANDID_MOVE) return false;
        m_AttackCooldownTime -= pDt * GameConstants.AI_ATTACKSPEED;
        float distance = vector.Distance(GetPosition(), m_HumansTarget.GetPosition());
        // CanAttackToPosition dereferences the native mind target, which is absent
        // for infected-engine Humans. Use geometry/range checks for this target.
        if (m_AttackCooldownTime > 0 || distance > 1.7 || !m_HumansTarget.HumanSight(this)) return true;
        vector direction = m_HumansTarget.GetPosition() - GetPosition(); direction.Normalize();
        if (vector.Dot(direction, GetDirection()) < 0.7) return true;
        m_ActualTarget = m_HumansTarget;
        m_ActualAttackType = GetDayZInfectedType().ChooseAttack(DayZInfectedAttackGroupType.FIGHT, distance, GetAttackPitch(m_ActualTarget));
        if (m_ActualAttackType)
        {
            m_HumansContactDone = false;
#ifdef VanillaSleepingZombies
            HZStartPatternAttack(m_ActualTarget);
#else
            StartCommand_Attack(m_ActualTarget, m_ActualAttackType.m_Type, m_ActualAttackType.m_Subtype);
#endif
            m_AttackCooldownTime = m_ActualAttackType.m_Cooldown;
        }
        return true;
    }
}
