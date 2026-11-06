// Local steering complements the native navmesh; recovery is completed before
// requesting another route to the enemy, so it cannot be cancelled every frame.
class HumansNavigation
{
    Human_Base Actor;
    ref array<vector> FailedPoints = {};
    ref array<float> FailedUntil = {};
    vector BypassGoal;
    float BypassUntil;
    void RememberBlocked(vector point)
    {
        if (FailedPoints.Count() >= 8) { FailedPoints.Remove(0); FailedUntil.Remove(0); }
        FailedPoints.Insert(point); FailedUntil.Insert(Now()+6);
    }
    float RecentFailureCost(vector point)
    {
        float cost;
        for (int i = FailedPoints.Count() - 1; i >= 0; i--)
        {
            if (Now()>=FailedUntil[i]) { FailedPoints.Remove(i); FailedUntil.Remove(i); continue; }
            if (HorizontalDistanceSq(point,FailedPoints[i])<2.25) cost+=6;
        }
        return cost;
    }
    bool LocalBypass(vector point, float speed)
    {
        vector start=Actor.GetPosition();
        vector forward=point-start; forward[1]=0; forward.Normalize();
        vector side=Vector(forward[2],0,-forward[0]);
        float best=100000; vector bestPoint; bool found;
        for (int choice=0;choice<12;choice++)
        {
            float angle=(25+Math.Floor(choice/2)*15)*Math.DEG2RAD;
            float sign=1; if (choice%2==1) sign=-1;
            vector direction=forward*Math.Cos(angle)+side*sign*Math.Sin(angle);
            vector sampled;
            if (!GroundPoint(start,start+direction*1.25,sampled,false,false)) continue;
            float score=vector.Distance(sampled,point)+angle*0.5+RecentFailureCost(sampled);
            if (score<best) { best=score; bestPoint=sampled; found=true; }
        }
        if (!found) return false;
        BypassGoal=bestPoint; BypassUntil=Now()+0.8;
        Steer(bestPoint,Math.Min(speed,1.5));
        return true;
    }
    ref array<vector> Recovery = {};
    int RecoveryIndex;
    int Attempts;
    float RetryAt;
    vector RecoveryGoal;
    vector MoveDirection;
    Building Door;
    int DoorIndex = -1;
    vector DoorCenter;
    vector DoorAnchor;
    vector DoorNormal;
    vector DoorEntry;
    vector DoorExit;
    float Joined;
    float Heartbeat;
    float LeaseUntil;
    float DoorScanAt;
    float DoorRetryAt;
    float PassedUntil;
    Building PassedDoor;
    int PassedIndex = -1;
    bool PortalReady;
    int PortalStage;
    vector DoorClearExit;
    float DoorProgressAt;
    vector DoorProgressPosition;

    void HumansNavigation(Human_Base actor) { Actor = actor; }

    float Now() { return GetGame().GetTime() * 0.001; }
    float HorizontalDistanceSq(vector from, vector to)
    {
        from[1] = 0; to[1] = 0;
        return vector.DistanceSq(from, to);
    }

    bool WalkClear(vector from, vector to, bool traffic = false)
    {
        return Actor.HumanClearSegment(from + "0 0.6 0", to + "0 0.6 0", 0.22, traffic) && Actor.HumanClearSegment(from + "0 1 0", to + "0 1 0", 0.27, traffic) && Actor.HumanClearSegment(from + "0 1.55 0", to + "0 1.55 0", 0.22, traffic);
    }

    bool GroundPoint(vector from, vector wanted, out vector sampled, bool escape = false, bool traffic = false)
    {
        AIWorld world = GetGame().GetWorld().GetAIWorld();
        if (!world.SampleNavmeshPosition(wanted, 0.4, Actor.m_HumanFilter, sampled)) return false;
        if (Math.AbsFloat(sampled[1] - from[1]) > 0.65 || vector.DistanceSq(sampled, wanted) > 0.25) return false;
        vector edge, normal;
        if (world.RaycastNavMesh(from, sampled, Actor.m_HumanFilter, edge, normal)) return false;
        vector begin = from;
        if (escape)
        {
            vector away = sampled - from; away[1] = 0; away.Normalize();
            begin = from + away * 0.2;
            if (!Actor.HumanClearSegment(from + "0 1 0", begin + "0 1 0", 0.1, traffic)) return false;
        }
        return WalkClear(begin, sampled, traffic);
    }

    void Steer(vector point, float speed)
    {
        Actor.HumanFace(point);
        vector direction = point - Actor.GetPosition(); direction[1] = 0;
        float distance = direction.Length(); direction.Normalize();
        MoveDirection = direction;
        speed = Math.Min(speed, Math.Max(0.25, distance * 1.5));
        Actor.GetInputController().OverrideMovementSpeed(true, speed * Math.Max(0, vector.Dot(direction, Actor.GetDirection())));
    }

    void ResetProgress()
    {
        Actor.m_HumanProgressPosition = Actor.GetPosition();
        Actor.m_HumanProgressAt = Actor.m_HumanClock;
        Actor.m_HumanPathAt = 0;
    }

    void ClearRecovery()
    {
        Recovery.Clear(); RecoveryIndex = 0; Actor.m_HumanDetourUntil = 0;
        ResetProgress();
    }

    void Recover(vector goal)
    {
        if (Actor.m_HumanClock < RetryAt) return;
        RetryAt = Actor.m_HumanClock + 0.75;
        Recovery.Clear(); RecoveryIndex = 0;
        vector start = Actor.GetPosition();
        vector forward = MoveDirection;
        if (forward.LengthSq() < 0.1) forward = goal - start;
        forward[1] = 0; forward.Normalize();
        if (forward.LengthSq() < 0.1) forward = Actor.GetDirection();
        vector side = Vector(forward[2], 0, -forward[0]);
        float preferred = 1;
        if ((Actor.m_HumanSocialId + Attempts) % 2 == 0) preferred = -1;
        float width = 1.5 + Math.Min(Attempts, 3) * 0.75;
        Attempts++;
        vector back;
        bool haveBack;
        for (int retreat = 0; retreat < 3; retreat++)
        {
            vector away = -forward;
            if (retreat == 1) away = -forward + side * preferred * 0.65;
            if (retreat == 2) away = -forward - side * preferred * 0.65;
            away.Normalize();
            if (GroundPoint(start, start + away, back, true, true)) { haveBack = true; break; }
        }
        vector origin = start;
        if (haveBack) origin = back;
        vector bestSide, bestOut;
        bool found, withOut;
        float best = 100000;
        for (int choice = 0; choice < 4; choice++)
        {
            float sign = preferred;
            if (choice % 2 == 1) sign = -preferred;
            float offset = width;
            if (choice >= 2) offset += 1.25;
            vector lateral;
            if (!GroundPoint(origin, origin + side * sign * offset, lateral, !haveBack, true)) continue;
            vector beyond;
            bool clearBeyond = GroundPoint(lateral, lateral + forward * 3, beyond, false, true);
            float score = vector.Distance(lateral, goal) + offset * 0.2 + RecentFailureCost(lateral);
            if (clearBeyond) score = vector.Distance(beyond, goal) + offset * 0.2 - 1000 + RecentFailureCost(lateral);
            if (score >= best) continue;
            best = score; bestSide = lateral; bestOut = beyond; found = true; withOut = clearBeyond;
        }
        if (haveBack) Recovery.Insert(back);
        if (found) { Recovery.Insert(bestSide); if (withOut) Recovery.Insert(bestOut); }
        if (Recovery.Count() > 0)
        {
            RecoveryGoal = goal;
            Actor.m_HumanDetour = Recovery[0];
            Actor.m_HumanDetourUntil = Actor.m_HumanClock + 10;
            ResetProgress();
            return;
        }
        Actor.HumanCancelLoot(); Actor.m_HumanPatrolAt = 0; Actor.m_HumanStageEnd = Actor.m_HumanClock;
        ResetProgress();
    }

    bool RecoverTick(vector goal)
    {
        if (Recovery.Count() == 0) return false;
        if (Actor.m_HumanClock >= Actor.m_HumanDetourUntil || vector.DistanceSq(goal, RecoveryGoal) > 144) { ClearRecovery(); return false; }
        while (RecoveryIndex < Recovery.Count() && HorizontalDistanceSq(Actor.GetPosition(), Recovery[RecoveryIndex]) < 0.2) RecoveryIndex++;
        if (RecoveryIndex >= Recovery.Count()) { ClearRecovery(); RetryAt = Actor.m_HumanClock + 0.4; return false; }
        if (Actor.m_HumanClock - Actor.m_HumanProgressAt > 1.25)
        {
            bool stalled = vector.DistanceSq(Actor.GetPosition(), Actor.m_HumanProgressPosition) < 0.04;
            ResetProgress();
            if (stalled) { ClearRecovery(); RetryAt = Actor.m_HumanClock + 0.25; return true; }
        }
        Actor.m_HumanDetour = Recovery[RecoveryIndex];
        Steer(Actor.m_HumanDetour, 1);
        return true;
    }

    void LeaveDoor()
    {
        Door = null; DoorIndex = -1; Joined = 0; LeaseUntil = 0; PortalReady = false; PortalStage = 0;
    }

    void JoinDoor(Building building, int index, vector center, vector normal)
    {
        if (Door == building && DoorIndex == index) return;
        LeaveDoor(); Door = building; DoorIndex = index; DoorCenter = center; DoorAnchor = center; DoorNormal = normal;
        Joined = Now(); Heartbeat = Joined;
    }

    Human_Base DoorOwner()
    {
        if (!Door) return null;
        Human_Base first;
        float oldest = 1000000000;
        foreach (Human_Base human : Human_Base.HumanPopulation)
        {
            if (!human || !human.IsAlive() || !human.m_HumanNavigation) continue;
            HumansNavigation nav = human.m_HumanNavigation;
            if (nav.Door != Door || nav.DoorIndex != DoorIndex || Now() - nav.Heartbeat > 1.5 || vector.DistanceSq(human.GetPosition(), nav.DoorCenter) > 64) continue;
            if (nav.Joined < oldest || (nav.Joined == oldest && (!first || human.m_HumanSocialId < first.m_HumanSocialId))) { first = human; oldest = nav.Joined; }
        }
        return first;
    }

    void FindDoor(vector goal, vector point)
    {
        if (Actor.m_HumanClock < DoorScanAt || Now() < DoorRetryAt) return;
        DoorScanAt = Actor.m_HumanClock + 0.35;
        ref array<Object> objects = {}; ref array<CargoBase> cargo = {};
        GetGame().GetObjectsAtPosition3D(Actor.GetPosition(), 6, objects, cargo);
        vector start = Actor.GetPosition();
        vector direction = point - start; direction[1] = 0;
        if (direction.LengthSq() < 0.1) return;
        direction.Normalize();
        Building bestBuilding; int bestDoor = -1; float best = 16;
        vector bestCenter, bestNormal;
        foreach (Object object : objects)
        {
            Building building = Building.Cast(object);
            if (!building || building.GetDoorCount() == 0) continue;
            for (int index = 0; index < building.GetDoorCount(); index++)
            {
                if (building.IsDoorLocked(index) || (building == PassedDoor && index == PassedIndex && Now() < PassedUntil)) continue;
                vector center = building.GetDoorSoundPos(index);
                if (Math.AbsFloat(center[1] - start[1]) > 2) continue;
                center[1] = start[1];
                float distance = vector.DistanceSq(center, start);
                if (distance >= best) continue;
                vector toDoor = center - start;
                float along = vector.Dot(toDoor, direction);
                if (along < -0.4 || along > 4 || (toDoor - direction * along).LengthSq() > 2.25) continue;
                vector normal = building.GetDirection(); normal[1] = 0; normal.Normalize();
                vector cross = Vector(normal[2], 0, -normal[0]);
                if (Math.AbsFloat(vector.Dot(cross, direction)) > Math.AbsFloat(vector.Dot(normal, direction))) normal = cross;
                if (vector.Dot(normal, direction) < 0) normal = -normal;
                if (vector.Dot(goal - center, normal) < 0.5 || vector.Dot(start - center, normal) > 0.6) continue;
                best = distance; bestBuilding = building; bestDoor = index; bestCenter = center; bestNormal = normal;
            }
        }
        if (bestBuilding) JoinDoor(bestBuilding, bestDoor, bestCenter, bestNormal);
    }

    bool ResolvePortal()
    {
        vector side = Vector(DoorNormal[2], 0, -DoorNormal[0]);
        for (int attempt = 0; attempt < 9; attempt++)
        {
            float offset;
            if (attempt > 0) { offset = Math.Ceil(attempt / 2.0) * 0.2; if (attempt % 2 == 0) offset = -offset; }
            vector center = DoorAnchor + side * offset;
            vector entry, exit;
            if (!GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(center - DoorNormal * 0.9, 0.45, Actor.m_HumanFilter, entry)) continue;
            if (!GroundPoint(entry, center + DoorNormal * 1.3, exit, false, true)) continue;
            // A gap must cross a wall, not merely run alongside its outside.
            if (WalkClear(entry + side * 0.9, exit + side * 0.9, true) || WalkClear(entry - side * 0.9, exit - side * 0.9, true)) continue;
            DoorEntry = entry; DoorExit = exit; DoorClearExit = exit;
            DoorCenter = (entry + exit) * 0.5;
            float spread = 0.8; if (Actor.m_HumanSocialId % 2 == 0) spread = -spread;
            vector clearExit;
            if (GroundPoint(exit, exit + side * spread + DoorNormal * 0.4, clearExit, false, true)) DoorClearExit = clearExit;
            else if (GroundPoint(exit, exit - side * spread + DoorNormal * 0.4, clearExit, false, true)) DoorClearExit = clearExit;
            PortalReady = true; return true;
        }
        return false;
    }

    bool DoorTick(vector goal, vector point)
    {
        if (!Door) FindDoor(goal, point);
        if (!Door) return false;
        if (vector.DistanceSq(Actor.GetPosition(), DoorCenter) > 64) { LeaveDoor(); return false; }
        Heartbeat = Now();
        if (vector.Dot(goal - DoorCenter, DoorNormal) < 0.3 && PortalStage == 0) { LeaveDoor(); return false; }
        Human_Base owner = DoorOwner();
        if (owner != Actor)
        {
            if (owner && owner.m_HumanNavigation && owner.m_HumanNavigation.PortalReady)
            {
                HumansNavigation head = owner.m_HumanNavigation;
                DoorCenter = head.DoorCenter; DoorEntry = head.DoorEntry; DoorExit = head.DoorExit;
                DoorClearExit = head.DoorClearExit; PortalReady = true;
            }
            int rank;
            foreach (Human_Base other : Human_Base.HumanPopulation)
            {
                if (!other || !other.IsAlive() || !other.m_HumanNavigation || other == Actor) continue;
                HumansNavigation otherNav = other.m_HumanNavigation;
                if (otherNav.Door == Door && otherNav.DoorIndex == DoorIndex && Now() - otherNav.Heartbeat < 1.5 && (otherNav.Joined < Joined || (otherNav.Joined == Joined && other.m_HumanSocialId < Actor.m_HumanSocialId))) rank++;
            }
            vector side = Vector(DoorNormal[2], 0, -DoorNormal[0]);
            float spread = 0.5; if (Actor.m_HumanSocialId % 2 == 0) spread = -spread;
            vector waiting = DoorCenter - DoorNormal * (1.8 + Math.Min(rank, 4) * 0.9) + side * spread;
            vector sampled;
            if (vector.DistanceSq(Actor.GetPosition(), waiting) > 0.25 && GroundPoint(Actor.GetPosition(), waiting, sampled, true, true)) Steer(sampled, 1);
            else Actor.HumanFace(DoorCenter);
            ResetProgress();
            return true;
        }
        if (LeaseUntil == 0) { LeaseUntil = Now() + 8; DoorProgressAt = Now(); DoorProgressPosition = Actor.GetPosition(); }
        if (PortalStage == -1)
        {
            if (HorizontalDistanceSq(Actor.GetPosition(), DoorEntry) < 0.25 || Now() >= LeaseUntil)
            {
                LeaveDoor(); DoorRetryAt = Now() + 1.5; ResetProgress(); return false;
            }
            Steer(DoorEntry, 1); return true;
        }
        bool blocked;
        if (Now() - DoorProgressAt > 1.5)
        {
            blocked = vector.DistanceSq(Actor.GetPosition(), DoorProgressPosition) < 0.04;
            DoorProgressAt = Now(); DoorProgressPosition = Actor.GetPosition();
        }
        if (Now() >= LeaseUntil || blocked)
        {
            vector retreat;
            if (GroundPoint(Actor.GetPosition(), Actor.GetPosition() - DoorNormal * 1.2, retreat, true, true))
            {
                DoorEntry = retreat; PortalStage = -1; LeaseUntil = Now() + 2.5; Steer(retreat, 1); return true;
            }
            LeaveDoor(); DoorRetryAt = Now() + 1.5; Recover(goal); return RecoverTick(goal);
        }
        if (!Door.IsDoorOpen(DoorIndex))
        {
            if (!Actor.HumanInteractDoor(Door, DoorIndex))
            {
                vector approach;
                if (GroundPoint(Actor.GetPosition(), DoorCenter - DoorNormal * 1.5, approach, false, true)) Steer(approach, 1);
            }
            return true;
        }
        if (!PortalReady && !ResolvePortal()) return true;
        if (PortalStage == 0 && HorizontalDistanceSq(Actor.GetPosition(), DoorEntry) < 0.0225) PortalStage = 1;
        if (PortalStage == 1 && (HorizontalDistanceSq(Actor.GetPosition(), DoorExit) < 0.09 || vector.Dot(Actor.GetPosition() - DoorExit, DoorNormal) > 0)) PortalStage = 2;
        if (PortalStage == 2 && HorizontalDistanceSq(Actor.GetPosition(), DoorClearExit) < 0.09)
        {
            PassedDoor = Door; PassedIndex = DoorIndex; PassedUntil = Now() + 3;
            LeaveDoor(); ResetProgress(); return false;
        }
        if (PortalStage == 0) Steer(DoorEntry, 1);
        else if (PortalStage == 1) Steer(DoorExit, 1);
        else Steer(DoorClearExit, 1);
        return true;
    }

    void Travel(vector goal, float speed)
    {
        if (Actor.m_HumanClock - Actor.m_HumanTravelAt > 0.5) ResetProgress();
        Actor.m_HumanTravelAt = Actor.m_HumanClock;
        if (RecoverTick(goal)) return;
        if (Now() < DoorRetryAt) { Actor.HumanFace(goal); return; }
        if (!Door && vector.DistanceSq(Actor.GetPosition(), goal) < 0.16) return;
        if (Door && DoorTick(goal, goal)) return;
        if (Actor.HumanTryVehicleClimb(goal)) return;
        if (Actor.HumanTryVault(goal)) return;
        if (!Door && Now()<BypassUntil && HorizontalDistanceSq(Actor.GetPosition(),BypassGoal)>0.09 && WalkClear(Actor.GetPosition(),BypassGoal,false)) { Steer(BypassGoal,Math.Min(speed,1.5)); return; }
        if (Actor.m_HumanClock - Actor.m_HumanProgressAt > 1.1)
        {
            bool stalled = vector.DistanceSq(Actor.GetPosition(), Actor.m_HumanProgressPosition) < 0.0625 && vector.DistanceSq(Actor.GetPosition(), goal) > 0.49;
            ResetProgress();
            if (stalled && !Door) { Recover(goal); if (RecoverTick(goal)) return; }
        }
        if (Actor.m_HumanClock >= Actor.m_HumanPathAt || vector.DistanceSq(goal, Actor.m_HumanPathGoal) > 4)
        {
            Actor.m_HumanPathAt = Actor.m_HumanClock + 1.5; Actor.m_HumanPathGoal = goal;
            Actor.m_HumanPath.Clear(); Actor.m_HumanPathIndex = 1;
            GetGame().GetWorld().GetAIWorld().FindPath(Actor.GetPosition(), goal, Actor.m_HumanFilter, Actor.m_HumanPath);
        }
        while (Actor.m_HumanPathIndex < Actor.m_HumanPath.Count() && vector.DistanceSq(Actor.GetPosition(), Actor.m_HumanPath[Actor.m_HumanPathIndex]) < 0.16) Actor.m_HumanPathIndex++;
        vector point = goal;
        if (Actor.m_HumanPathIndex < Actor.m_HumanPath.Count()) point = Actor.m_HumanPath[Actor.m_HumanPathIndex];
        if (DoorTick(goal, point) || Actor.HumanTryDoor(point)) return;
        if (Actor.m_HumanPathIndex >= Actor.m_HumanPath.Count()) { Recover(goal); RecoverTick(goal); return; }
        vector direction = point - Actor.GetPosition(); direction[1] = 0;
        float length = direction.Length(); direction.Normalize();
        MoveDirection = direction;
        vector probe = Actor.GetPosition() + direction * Math.Min(length, 0.8);
        if (vector.Dot(direction, Actor.GetDirection()) > 0.5 && !WalkClear(Actor.GetPosition(), probe, true))
        {
            RememberBlocked(probe);
            if (LocalBypass(point,speed)) return;
            Recover(goal); RecoverTick(goal); return;
        }
        Steer(point, speed);
    }
}


