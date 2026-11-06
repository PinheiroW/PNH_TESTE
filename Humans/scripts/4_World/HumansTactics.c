// Perception memory stores observations, never a continuously updated hidden target.
class HumansMemory
{
 EntityAI Target;
 vector Position;
 float Expires;
 float ObservedAt;
}
class HumansTactics
{
 Human_Base Actor;
 ref array<ref HumansMemory> Memories = {};
 vector DodgeGoal;
 float DodgeUntil, DodgeAt, OrbitAt;
 vector OrbitGoal;
 vector CrowdGoal;
 float CrowdUntil;
 bool Sneaking;
 float AlertUntil;
 void HumansTactics(Human_Base actor) { Actor = actor; }
 static bool InPursuitRange(vector actor, vector target) { return vector.DistanceSq(actor,target) <= 6400; }
 HumansMemory Recall(EntityAI target)
 {
  for (int i = Memories.Count() - 1; i >= 0; i--)
  {
   HumansMemory entry=Memories[i];
   if (!entry.Target || !entry.Target.IsAlive() || Actor.m_HumanClock >= entry.Expires) { Memories.Remove(i); continue; }
   if (entry.Target==target) return entry;
  }
  return null;
 }
 void Remember(EntityAI target, vector position)
 {
  if (!target) return;
  HumansMemory entry=Recall(target);
  if (!entry)
  {
   if (Memories.Count()>=12) Memories.Remove(0);
   entry=new HumansMemory(); entry.Target=target; Memories.Insert(entry);
  }
  entry.Position=position; entry.ObservedAt=Actor.m_HumanClock; entry.Expires=Actor.m_HumanClock+600;
 }
 void DropFocus()
 {
  Actor.m_HumanEnemy=null; DodgeUntil=0; OrbitAt=0; Sneaking=false;
  Actor.m_HumanPathAt=0;
  if (Actor.m_HumanNavigation) { Actor.m_HumanNavigation.LeaveDoor(); Actor.m_HumanNavigation.ClearRecovery(); }
 }
 void UpdateFocus()
 {
  EntityAI enemy=Actor.m_HumanEnemy;
  if (!enemy) return;
  if (!Actor.HumanHostile(enemy) || !InPursuitRange(Actor.GetPosition(),enemy.GetPosition())) { DropFocus(); return; }
  // Search the last observed position rather than following a hidden entity.
  if (Actor.m_HumanClock-Actor.m_HumanSeen>12)
  {
   vector last=Actor.m_HumanLastKnown;
   DropFocus(); Actor.HumanHear(last);
  }
 }
 bool TryDodge()
 {
  if (Actor.m_HumanClock<DodgeAt || !Actor.GetCommand_Move() || Actor.m_HumanRestPhase!=3 || Actor.m_HumanDoorAction || Actor.m_HumanVaulting || Actor.m_HumanVaultPending || Actor.HumanIsBlocking()) return false;
  if (Actor.m_HumanNavigation && Actor.m_HumanNavigation.Door) return false;
  vector toward=Actor.m_HumanLastKnown-Actor.GetPosition(); toward[1]=0; toward.Normalize();
  vector side=Vector(toward[2],0,-toward[0]);
  float sign=1; if (Actor.m_HumanSocialId%2==0) sign=-1;
  for (int choice=0;choice<3;choice++)
  {
   vector offset=side*sign*1.25-toward*0.35;
   if (choice==1) offset=-side*sign*1.25-toward*0.35;
   if (choice==2) offset=-toward*1.2;
   vector sampled;
   if (!Actor.HumanNavigation().GroundPoint(Actor.GetPosition(),Actor.GetPosition()+offset,sampled,true,false)) continue;
   DodgeGoal=sampled; DodgeUntil=Actor.m_HumanClock+0.85; DodgeAt=Actor.m_HumanClock+Math.RandomFloatInclusive(3,5);
   Actor.m_HumanHitDone=true; Actor.m_HumanSwingSoundDone=true;
   return true;
  }
  DodgeAt=Actor.m_HumanClock+0.75;
  return false;
 }
 void React()
 {
  // Defence is deterministic and independent of the optional dodge timer.
  // Refresh while an actual melee command is active, release promptly after it.
  AlertUntil=Actor.m_HumanClock+60;
  Actor.HumanStartBlock();
 }
 bool DodgeTick()
 {
  if (Actor.m_HumanClock>=DodgeUntil || !Actor.GetCommand_Move()) return false;
  if (vector.DistanceSq(Actor.GetPosition(),DodgeGoal)<0.09) { DodgeUntil=0; return false; }
  if (!Actor.HumanNavigation().WalkClear(Actor.GetPosition(),DodgeGoal,false)) { DodgeUntil=0; return false; }
  Actor.HumanNavigation().Steer(DodgeGoal,2);
  return true;
 }
 bool CombatGoal(out vector goal, out float speed)
 {
  goal=Actor.m_HumanLastKnown; speed=2; Sneaking=false;
  PlayerBase player=PlayerBase.Cast(Actor.m_HumanEnemy);
  if (!player || !Actor.HumanSight(player,true)) return false;
  float distance=vector.Distance(Actor.GetPosition(),goal);
  if (distance<4 || player.IsRaised() || player.GetCommand_Melee2() || player.GetCommand_Melee()) AlertUntil=Actor.m_HumanClock+60;
  if (distance<5 && CrowdApproach(goal)) { goal=CrowdGoal; speed=2; return true; }
  if (Actor.HumanBlocked(player) && distance<3.8)
  {
   if (Actor.m_HumanClock>=OrbitAt)
   {
    OrbitAt=Actor.m_HumanClock+1.2;
    vector toward=goal-Actor.GetPosition(); toward[1]=0; toward.Normalize();
    vector side=Vector(toward[2],0,-toward[0]);
    float sign=1; if (Actor.m_HumanSocialId%2==0) sign=-1;
    vector sampled;
    if (Actor.HumanNavigation().GroundPoint(Actor.GetPosition(),Actor.GetPosition()+side*sign*1.4,sampled,true,false)) OrbitGoal=sampled;
    else if (Actor.HumanNavigation().GroundPoint(Actor.GetPosition(),Actor.GetPosition()-side*sign*1.4,sampled,true,false)) OrbitGoal=sampled;
    else OrbitGoal=Actor.GetPosition();
   }
   goal=OrbitGoal; speed=2; return true;
  }
  vector toActor=Actor.GetPosition()-goal; toActor[1]=0; toActor.Normalize();
  vector facing=player.GetDirection(); facing[1]=0; facing.Normalize();
  if (Actor.m_HumanClock>=AlertUntil && distance>4 && distance<20 && !player.IsRaised() && !player.GetCommand_Melee2() && !player.GetCommand_Melee() && vector.Dot(facing,toActor)<-0.2)
  {
   vector behind=goal-facing*2.4;
   vector sample;
   if (GetGame().GetWorld().GetAIWorld().SampleNavmeshPosition(behind,0.5,Actor.m_HumanFilter,sample) && Math.AbsFloat(sample[1]-goal[1])<0.65)
   {
    goal=sample; speed=1; Sneaking=true; return true;
   }
  }
  return false;
 }
 bool CrowdApproach(vector target)
 {
  if (Actor.m_HumanNavigation && Actor.m_HumanNavigation.Door) return false;
  if (Actor.m_HumanClock<CrowdUntil && vector.DistanceSq(Actor.GetPosition(),CrowdGoal)>0.09 && Actor.HumanNavigation().WalkClear(Actor.GetPosition(),CrowdGoal,false)) return true;
  vector forward=target-Actor.GetPosition();forward[1]=0;
  float range=forward.Length();if(range<0.1)return false;forward.Normalize();
  bool crowded;
  foreach(Human_Base other:Human_Base.HumanPopulation)
  {
   if(!other || other==Actor || !other.IsAlive() || other.m_HumanEnemy!=Actor.m_HumanEnemy)continue;
   vector delta=other.GetPosition()-Actor.GetPosition();delta[1]=0;
   float along=vector.Dot(delta,forward);
   if(along>0.1 && along<1.6 && along<range && (delta-forward*along).LengthSq()<0.49) {crowded=true;break;}
  }
  if(!crowded)return false;
  vector side=Vector(forward[2],0,-forward[0]);
  float sign=1;if(Actor.m_HumanSocialId%2==0)sign=-1;
  // Step out of a friend's approach lane before the generic stall timeout.
  // Every candidate checks navmesh and actual bodies; no position teleport.
  for(int choice=0;choice<4;choice++)
  {
   float direction=sign;if(choice%2==1)direction=-sign;
   float width=1.1;if(choice>=2)width=1.6;
   vector sample;
   if(!Actor.HumanNavigation().GroundPoint(Actor.GetPosition(),Actor.GetPosition()+side*direction*width,sample,true,false))continue;
   CrowdGoal=sample;CrowdUntil=Actor.m_HumanClock+1.2;return true;
  }
  return false;
 }
}


