// Client-local voice budget. Never throttles server shots or damage.
class HumansGunAudio
{
 static const int MAX_SHOT_VOICES=3;
 static const float CLUSTER_WINDOW=0.08;
 static const float CLUSTER_RADIUS_SQ=144;
 static ref array<ref EffectSound> Shots={};
 static ref array<float> Times={};
 static ref EffectSound Tail;
 static float LastClusterAt=-100;
 static vector LastClusterPosition;
 static float TailAt;
 static int Requests;
 static int Played;
 static int Coalesced;
 static void PlayShot(Object source)
 {
  if(GetGame().IsDedicatedServer() || !source)return;
  float now=GetGame().GetTime()*0.001;Requests++;
  for(int i=Shots.Count() - 1;i>=0;i--)
  {
   if(!Shots[i] || !Shots[i].IsSoundPlaying() || now - Times[i]>2)
   {
    if(Shots[i] && Shots[i].IsSoundPlaying())Shots[i].SoundStop();
    Shots.Remove(i);Times.Remove(i);
   }
  }
  bool merge=now - LastClusterAt<CLUSTER_WINDOW && vector.DistanceSq(source.GetPosition(),LastClusterPosition)<CLUSTER_RADIUS_SQ;
  if(merge)Coalesced++;
  else
  {
   while(Shots.Count()>=MAX_SHOT_VOICES)
   {
    if(Shots[0])Shots[0].SoundStop();Shots.Remove(0);Times.Remove(0);
   }
   EffectSound shot=SEffectManager.PlaySoundOnObject("Humans_GlockShot_SoundSet",source,0,0.05);
   if(shot){shot.SetAutodestroy(true);Shots.Insert(shot);Times.Insert(now);Played++;}
   LastClusterAt=now;LastClusterPosition=source.GetPosition();
  }
  // One rolling environmental echo, rather than one long tail per NPC/shot.
  if(now>=TailAt)
  {
   if(Tail)Tail.SoundStop();
   Tail=SEffectManager.PlaySoundOnObject("Humans_GlockTail_SoundSet",source,0,0.2);
   if(Tail)Tail.SetAutodestroy(true);
   TailAt=now+0.6;
  }
#ifdef DIAG_DEVELOPER
  if(Requests%20==0)Print("[HumansAudio] requests="+Requests.ToString()+" played="+Played.ToString()+" merged="+Coalesced.ToString()+" activeShotVoices="+Shots.Count().ToString()+" maximum=3 tailMaximum=1");
#endif
 }
}

