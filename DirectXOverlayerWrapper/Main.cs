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
    public class StringWrapper
    {
        public static string a;
    }
    public class Main
    {
        [DllImport("kernel32", SetLastError = true)]
        static extern IntPtr LoadLibraryW([MarshalAs(UnmanagedType.LPWStr)] string lpFileName);

        public static UnityModManager.ModEntry entry;

        public static Texture2D logo;

        public static Dictionary<string, Dictionary<string, string>> translations = new();
        public static string language = "KOREAN";

        public static Dictionary<string, (Func<object>, bool)> tags = new();

        public static bool isEditingText = false;
        public static IntPtr curEditingText;

        public static bool IsPlaying
        {
            get
            {
                var cdt = scrConductor.instance;
                var ctl = scrController.instance;
                if (ctl != null && cdt != null)
                    return !ctl.paused && cdt.isGameWorld;
                return false;
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

            // Load Tags
            LoadTag<HexCodes>();
            LoadTag<JudgeTags>();
            LoadTag<PerformanceTags>();
            
            

            Wrapper.Load();

            // Load Save
            var savpath = Path.Combine(entry.Path, "save.json");
            if (File.Exists(savpath))
            {
                var json = File.ReadAllText(savpath);
                var jo = JObject.Parse(json);
                language = jo["language"].ToString();
                Wrapper.LoadSave(json);
            }



            var harmony = new Harmony(entry.Info.Id);
            harmony.PatchAll(Assembly.GetExecutingAssembly());

            logo = new Texture2D(2, 2);
            logo.LoadImage(File.ReadAllBytes(Path.Combine(entry.Path, "Logo.png")));

            //HangulPatch.SetHook();

            entry.OnGUI = OnGUI;
            entry.OnUpdate = OnUpdate;
            entry.OnSaveGUI = OnSaveGUI;
        }

        public static void OnSaveGUI(UnityModManager.ModEntry entry)
        {
            var savstr_ = Wrapper.Save();

            var savstr = Marshal.PtrToStringAnsi(savstr_);

            var jo = JObject.Parse(savstr);
            jo["language"] = language;

            File.WriteAllText(Path.Combine(entry.Path, "save.json"), jo.ToString());
            Wrapper.FreeMemory(savstr_);
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
                //Input.imeCompositionMode = IMECompositionMode.On;
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
                //Input.imeCompositionMode = IMECompositionMode.On;
                Wrapper.isSetting = true;
            }
        }

        [HarmonyPatch(typeof(Event), "current", MethodType.Getter)]
        public static class currentPatch
        {
            public static void Postfix(ref Event __result)
            {
                if (isEditingText) return;
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
                if (isEditingText)
                {
                    var str = Marshal.PtrToStringAnsi(Wrapper.GetStringWithReference(curEditingText));
                    
                    Wrapper.SetStringWithReference(curEditingText, GUI.TextArea(new Rect(Screen.width / 2 - 250 * 900 / Screen.height, Screen.height / 2 - 250 * 900 / Screen.height, 500 * 900 / Screen.height, 500 * 900 / Screen.height), str));
                    if (GUI.Button(new Rect(Screen.width / 2 - 50 * 900 / Screen.height, Screen.height / 2 + 300 * 900 / Screen.height, 100 * 900 / Screen.height, 50 * 900 / Screen.height), translations[language]["Done"]))
                    {
                        isEditingText = false;
                    }
                    
                }
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

    }
}
