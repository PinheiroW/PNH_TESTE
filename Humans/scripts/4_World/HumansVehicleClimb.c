// Reachable, stationary vehicle surfaces only. Every climb and descent uses
// swept body clearance; roof movement is confined to surfaces of the same car.
class HumansVehicleClimb
{
    Human_Base Actor;
    CarScript Vehicle;
    vector Anchor;
    float CheckAt;
    float LastTick;
    string Reason;

    void HumansVehicleClimb(Human_Base actor) { Actor = actor; }

    bool ProbeVehicleSurface(vector from, vector to, out CarScript vehicle, out vector position)
    {
        ref array<Object> ignore = {Actor};
        if (Actor.m_HumanWeapon) ignore.Insert(Actor.m_HumanWeapon);
        if (Actor.m_HumanEnemy) ignore.Insert(Actor.m_HumanEnemy);
        ref array<ref RaycastRVResult> hits = {};
        RaycastRVParams ray = new RaycastRVParams(from, to, Actor);
        ray.type = ObjIntersectGeom;
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0) return false;
        vehicle = CarScript.Cast(hits[0].obj);
        if (!vehicle) vehicle = CarScript.Cast(hits[0].parent);
        if (!vehicle) return false;
        position = hits[0].pos;
        return true;
    }

    CarScript TargetCar()
    {
        PlayerBase player = PlayerBase.Cast(Actor.m_HumanEnemy);
        if (!player || !player.IsAlive() || player.GetCommand_Vehicle()) return null;
        CarScript vehicle; vector floor;
        if (!ProbeVehicleSurface(player.GetPosition() + "0 0.15 0", player.GetPosition() - "0 1.4 0", vehicle, floor)) return null;
        return vehicle;
    }

    static vector Position(vector start, vector end, float t, bool descending = false)
    {
        if (descending) return Position(end, start, 1 - t, false);
        float rise = Math.Clamp(t / 0.45, 0, 1); rise = rise * rise * (3 - 2 * rise);
        float travel = Math.Clamp((t - 0.35) / 0.5, 0, 1); travel = travel * travel * (3 - 2 * travel);
        float settle = Math.Clamp((t - 0.85) / 0.15, 0, 1);
        vector result = start + (end - start) * travel;
        result[1] = start[1] + (end[1] - start[1] + 0.2) * rise - 0.2 * settle;
        return result;
    }

    bool Clear(vector start, vector end, bool descending = false)
    {
        vector previous = start;
        for (int step = 1; step <= 16; step++)
        {
            vector next = Position(start, end, step / 16.0, descending);
            if (!Actor.HumanClearSegment(previous + "0 0.35 0", next + "0 0.35 0", 0.22, true)) return false;
            if (!Actor.HumanClearSegment(previous + "0 1 0", next + "0 1 0", 0.27, true)) return false;
            if (!Actor.HumanClearSegment(previous + "0 1.65 0", next + "0 1.65 0", 0.22, true)) return false;
            previous = next;
        }
        return true;
    }

    void Begin(vector landing, int type)
    {
        Actor.HumanNavigation().LeaveDoor();
        Actor.HumanNavigation().ClearRecovery();
        Actor.m_HumanVaultLanding = landing;
        Actor.m_HumanVaultType = type;
        Actor.m_HumanVaultPending = true;
        Actor.m_HumanVaultStarted = Actor.m_HumanClock;
        Actor.m_HumanVaultAt = Actor.m_HumanClock + 4;
    }

    bool TryClimb(vector goal)
    {
        if (Vehicle || Actor.m_HumanClock < CheckAt || Actor.m_HumanVaulting || Actor.m_HumanVaultPending || Actor.m_HumanDoorAction || Actor.m_HumanRestPhase != 3) return false;
        CheckAt = Actor.m_HumanClock + 0.4;
        CarScript vehicle = TargetCar();
        Reason = "target/moving/range";
        if (!vehicle || GetVelocity(vehicle).Length() > 0.5 || vector.DistanceSq(Actor.GetPosition(), vehicle.GetPosition()) > 49) return false;
        vector start = Actor.GetPosition();
        vector forward = goal - start; forward[1] = 0; forward.Normalize();
        CarScript landingCar; vector landing;
        vector wanted = goal - forward * 0.65;
        Reason = "landing probe";
        if (!ProbeVehicleSurface(wanted + "0 0.8 0", wanted - "0 2.2 0", landingCar, landing) || landingCar != vehicle) return false;
        landing[1] = landing[1] + 0.03;
        float height = landing[1] - start[1];
        Reason = "height " + height.ToString();
        if (height < 0.45 || height > 1.9) return false;
        // Locate the near side before the AI's ground path turns around the car.
        ref array<Object> ignore = {Actor};
        if (Actor.m_HumanWeapon) ignore.Insert(Actor.m_HumanWeapon);
        if (Actor.m_HumanEnemy) ignore.Insert(Actor.m_HumanEnemy);
        ref array<ref RaycastRVResult> hits = {};
        vector nearEnd = landing; nearEnd[1] = start[1] + 0.6;
        RaycastRVParams ray = new RaycastRVParams(start + "0 0.6 0", nearEnd, Actor);
        ray.type = ObjIntersectGeom;
        Reason = "approach ray";
        if (!DayZPhysics.RaycastRVProxy(ray, hits, ignore) || hits.Count() == 0) return false;
        CarScript nearCar = CarScript.Cast(hits[0].obj);
        if (!nearCar) nearCar = CarScript.Cast(hits[0].parent);
        if (nearCar != vehicle) return false;
        Reason = "launch navmesh";
        vector launch = hits[0].pos - forward * 0.65; launch[1] = start[1];
        vector launchSample;
        if (!GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(launch, 0.45, Actor.m_HumanFilter, launchSample)) return false;
        vector across = landing - launchSample; across[1] = 0;
        Reason = "capsule path";
        if (across.LengthSq() > 6.25 || !Clear(launchSample, landing)) return false;
        if (vector.DistanceSq(start, launchSample) > 0.0625)
        {
            Reason = "approach clearance";
            if (!Actor.HumanNavigation().WalkClear(start, launchSample, true)) return false;
            Actor.HumanNavigation().Steer(launchSample, 1); CheckAt = Actor.m_HumanClock + 0.1; Reason = "approaching"; return true;
        }
        if (vector.Dot(forward, Actor.GetDirection()) < 0.95)
        {
            Actor.HumanFace(goal); CheckAt = Actor.m_HumanClock + 0.1; Reason = "aligning"; return true;
        }
        Reason = "final capsule";
        if (!Clear(start, landing)) return false;
        Vehicle = vehicle; Anchor = vehicle.WorldToModel(landing);
        int type = 2; if (height > 1.45) type = 3;
        Begin(landing, type);
        Reason = "climbing";
        return true;
    }

    bool Tick(float dt)
    {
        if (!Vehicle || Actor.m_HumanVaulting || Actor.m_HumanVaultPending) return false;
        vector current = Vehicle.ModelToWorld(Anchor);
        Actor.SetPosition(current);
        if (TargetCar() != Vehicle || !Actor.m_HumanEnemy)
        {
            if (Actor.m_HumanClock < CheckAt) return true;
            CheckAt = Actor.m_HumanClock + 0.5;
            vector start = Vehicle.ModelToWorld(Anchor);
            for (int attempt = 0; attempt < 8; attempt++)
            {
                float angle = attempt * Math.PI / 4;
                vector wanted = start + Vector(Math.Sin(angle) * 2.5, -1, Math.Cos(angle) * 2.5);
                wanted[1] = GetGame().SurfaceY(wanted[0], wanted[2]);
                vector landing;
                if (!GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(wanted, 0.6, Actor.m_HumanFilter, landing)) continue;
                if (start[1] - landing[1] < 0.3 || start[1] - landing[1] > 2.1 || !Clear(start, landing, true)) continue;
                Vehicle = null; Begin(landing, 0); return true;
            }
            Actor.SetPosition(start);
            return true;
        }
        Actor.HumanFace(Actor.m_HumanEnemy.GetPosition());
        if (Actor.HumanIsBlocking() || Actor.GetCommand_Attack() || !Actor.GetCommand_Move()) return true;
        vector toward = Actor.m_HumanEnemy.GetPosition() - current; toward[1] = 0;
        if (toward.Length() < Actor.HumanReach(Actor.HumanActiveWeapon()) - 0.15) return true;
        toward.Normalize();
        vector wantedStep = current + toward * Math.Min(dt, 0.15);
        CarScript floorCar; vector roof;
        if (!ProbeVehicleSurface(wantedStep + "0 0.45 0", wantedStep - "0 0.45 0", floorCar, roof) || floorCar != Vehicle || Math.AbsFloat(roof[1] - current[1]) > 0.3) return true;
        roof[1] = roof[1] + 0.03;
        if (!Actor.HumanNavigation().WalkClear(current, roof, true)) return true;
        Anchor = Vehicle.WorldToModel(roof);
        Actor.SetPosition(roof);
        Actor.GetInputController().OverrideMovementSpeed(true, 1);
        return true;
    }
}
