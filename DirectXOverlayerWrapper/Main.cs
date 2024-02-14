using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using UnityModManagerNet;
using HarmonyLib;
using System.Reflection;
using UnityEngine;
using Newtonsoft.Json.Linq;
using DirectXOverlayer.Attributes;
using MonoMod.Utils;
using DirectXOverlayer.Tags;

namespace DirectXOverlayer
{

    public class Main
    {
        [DllImport("kernel32", SetLastError = true)]
        static extern IntPtr LoadLibraryW([MarshalAs(UnmanagedType.LPWStr)] string lpFileName);

        public static UnityModManager.ModEntry entry;

        public static Texture2D logo;

        public static Dictionary<string, Dictionary<string, string>> translations = new();
        public static string language = "KOREAN";

        public static Dictionary<HitMargin, double> hitmarginCount = new();

        public static Dictionary<string, (Func<object>, bool)> tags = new();

        public static bool IsPlaying
        {
            get
            {
                var cdt = scrConductor.instance;
                if (cdt != null)
                    return cdt.isGameWorld;
                return false;
            }
        }
        
        public static void HitMarginCountReset()
        {
            foreach (var hm in Enum.GetValues(typeof(HitMargin)))
            {
                hitmarginCount[(HitMargin)hm] = 0;
            }
        }

        public static void Load(UnityModManager.ModEntry entry)
        {
            var dllpath = Path.Combine(entry.Path, "DirectXOverlayer.dll");
            entry.Logger.Log(dllpath);
            var handle = LoadLibraryW(dllpath);
            if (handle == IntPtr.Zero)
            {
                throw new Exception("Handle Is Null!");
            }
            Main.entry = entry;

            entry.Logger.Log("Loading Translation");
            translations["KOREAN"] = JObject.Parse(File.ReadAllText(Path.Combine(entry.Path, "KOREAN.language"))).ToObject<Dictionary<string, string>>();
            translations["ENGLISH"] = JObject.Parse(File.ReadAllText(Path.Combine(entry.Path, "ENGLISH.language"))).ToObject<Dictionary<string, string>>();

            // Add Hit Margins
            HitMarginCountReset();

            // Load Tags
            LoadTag<HexCodes>();
            LoadTag<JudgeTags>();
            

            Wrapper.Load();


            var harmony = new Harmony(entry.Info.Id);
            harmony.PatchAll(Assembly.GetExecutingAssembly());

            logo = new Texture2D(2, 2);
            logo.LoadImage(File.ReadAllBytes(Path.Combine(entry.Path, "Logo.png")));

            entry.OnGUI = OnGUI;
            entry.OnUpdate = OnUpdate;
        }

        public static void LoadTag<T>()
        {
            LoadTag(typeof(T));
        }
        public static void LoadTag(Type type)
        {
            foreach (var method in type.GetMethods())
            {
                var attr = method.GetCustomAttribute<TagAttribute>();
                if (attr != null)
                {
                    tags[attr.Name] = (() => method.Invoke(null, new object[] { }), attr.NonPlayingAvailable);
                }
            }
            foreach (var field in type.GetFields())
            {
                var attr = field.GetCustomAttribute<FieldTagAttribute>();
                if (attr != null)
                {
                    tags[attr.Name] = (() => field.GetValue(null), attr.NonPlayingAvailable);
                }
            }
        }

        public static void OnGUI(UnityModManager.ModEntry entry)
        {

            GUILayout.Label(logo, GUILayout.Height(80), GUILayout.Width(80 * logo.width / logo.height));

            GUILayout.Space(10);

            GUILayout.BeginHorizontal();
            if (GUILayout.Button("한국어")) language = "KOREAN";
            if (GUILayout.Button("English")) language = "ENGLISH";
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();

            GUILayout.Space(10);

            if (!Wrapper.isInitialized)
            {
                GUILayout.Label($"<color=yellow>{translations[language]["FontBrokenAlert"]}</color>");
                GUILayout.Space(10);
            }


            GUILayout.BeginHorizontal();
            if (GUILayout.Button(translations[language]["OpenSettings"]))
            {
                Wrapper.isSetting = true;
            }
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();
        }

        public static void OnUpdate(UnityModManager.ModEntry entry, float delta)
        {
            if (IsPlaying) return;
            if (Input.GetKeyDown(KeyCode.F3))
            {
                UnityModManager.UI.Instance.ToggleWindow(true);
                Wrapper.isSetting = true;
            }
        }

        [HarmonyPatch(typeof(Event), "current", MethodType.Getter)]
        public static class currentPatch
        {
            public static void Postfix(ref Event __result)
            {
                if (Wrapper.isSetting && __result.type is EventType.MouseDown or EventType.MouseUp)
                {
                    __result.type = EventType.Ignore;
                }
            }
        }

        [HarmonyPatch(typeof(UnityModManager.UI), "OnGUI")]
        public static class OnGUIPatch
        {
            public static bool Prefix()
            {
                return !Wrapper.isSetting;
            }
        }

        [HarmonyPatch(typeof(Input), "GetKeyDown", typeof(KeyCode))]
        public static class GetKeyDownPatch
        {
            public static void Postfix(ref bool __result)
            {
                if (Wrapper.isSetting) __result = false;   
            }
        }

        [HarmonyPatch(typeof(Input), "GetKeyUp", typeof(KeyCode))]
        public static class GetKeyUpPatch
        {
            public static void Postfix(ref bool __result)
            {
                if (Wrapper.isSetting) __result = false;
            }
        }

        [HarmonyPatch(typeof(scrController), "CountValidKeysPressed")]
        public static class CountValidKeysPressedPatch
        {
            public static void Postfix(ref int __result)
            {
                if (Wrapper.isSetting) __result = 0;
            }
        }

        [HarmonyPatch(typeof(scrMisc), "GetHitMargin")]
        public static class GetHitMarginPatch
        {
            public static void Postfix(ref HitMargin __result)
            {
                hitmarginCount[__result]++;
            }
        }

        [HarmonyPatch(typeof(scrController), "Awake_Rewind")]
        public static class Resetter
        {
            public static void Prefix(scrController __instance) => Reset(__instance);
            public static void Reset(scrController __instance)
            {
                HitMarginCountReset();
            }
        }
        [HarmonyPatch(typeof(scrController), "Start")]
        public static class Resetter2
        {
            public static void Postfix(scrController __instance) => Resetter.Reset(__instance);
        }

    }
}
