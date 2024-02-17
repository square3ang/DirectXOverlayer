using DirectXOverlayer.Tags;
using HarmonyLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace DirectXOverlayer
{
    public class BpmUpdater
    {
        public static float pitch;
        public static float bpm;
        public static float playbackSpeed;
        public static bool beforedt;
        public static double beforebpm;
        public static FieldInfo curSpd = typeof(GCS).GetField("currentSpeedTrial", AccessTools.all);

        [HarmonyPatch(typeof(scnGame), "Play")]
        public static class BpmUpdaterPatch1
        {
            public static void Postfix()
            {
                scrController instance = scrController.instance;
                if (instance == null || !instance.gameworld)
                {
                    return;
                }
                if (scnGame.instance == null)
                {
                    return;
                }
                Init();
            }
        }
        [HarmonyPatch(typeof(scrPressToStart), "ShowText")]
        public static class BpmUpdaterPatch2
        {
            public static void Postfix()
            {
                if (!scrController.instance.gameworld)
                {
                    return;
                }
                if (scnGame.instance != null)
                {
                    return;
                }
                Init();
            }
        }
        [HarmonyPatch(typeof(scrPlanet), "MoveToNextFloor")]
        public static class BpmUpdaterPatch3
        {
            public static void Postfix(scrFloor floor)
            {
                if (!scrController.instance.gameworld)
                {
                    return;
                }
                if (floor.nextfloor == null)
                {
                    return;
                }
                double num = GetRealBpm(floor, bpm) * playbackSpeed * pitch;
                int num2 = 0;
                GamePlayTags.TileBpm = bpm * scrController.instance.speed;
                if (num2 != 0 || beforedt)
                {
                    num = beforebpm;
                }
                GamePlayTags.RealBpm = num;
                GamePlayTags.ReqKps = num / 60.0;
                beforedt = num2 != 0;
                beforebpm = num;
            }
            public static double GetRealBpm(scrFloor floor, float bpm)
            {
                if (floor == null)
                {
                    return (double)bpm;
                }
                if (floor.nextfloor == null)
                {
                    return scrController.instance.speed * (double)bpm;
                }
                return 60.0 / (floor.nextfloor.entryTime - floor.entryTime);
            }
        }
        public static void Init()
        {
            if (scnGame.instance != null)
            {
                pitch = scnGame.instance.levelData.pitch / 100f;
                if (ADOBase.isCLSLevel)
                {
                    pitch *= (float)curSpd.GetValue(null);
                }
                bpm = scnGame.instance.levelData.bpm * playbackSpeed * pitch;
            }
            else
            {
                pitch = scrConductor.instance.song.pitch;
                bpm = scrConductor.instance.bpm * pitch;
            }
            scnEditor instance = scnEditor.instance;
            playbackSpeed = (instance != null) ? instance.playbackSpeed : 1f;

            float num2 = bpm;
            if (scrController.instance.currentSeqID != 0)
            {
                double speed = scrController.instance.speed;
                num2 = (float)(bpm * speed);
            }


            GamePlayTags.TileBpm = num2;
            GamePlayTags.RealBpm = num2;
            GamePlayTags.ReqKps = 0;
        }
    }
}
