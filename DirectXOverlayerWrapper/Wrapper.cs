using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityModManagerNet;
namespace DirectXOverlayer
{

    internal class Wrapper
    {


        [DllImport("DirectXOverlayer.dll", CharSet = CharSet.Ansi)]
        public static extern void RegisterAPI(IntPtr addr, string APIName);

        [DllImport("DirectXOverlayer.dll")]
        public static extern void Start();

        [DllImport("DirectXOverlayer.dll")]
        public static extern void SetStringWithReference(IntPtr reference, string str);

        
        [DllImport("DirectXOverlayer.dll")]
        public static extern IntPtr GetStringWithReference(IntPtr reference);

        [DllImport("DirectXOverlayer.dll")]
        public static extern IntPtr Save();

        [DllImport("DirectXOverlayer.dll")]
        public static extern void FreeMemory(IntPtr ptr);

        [DllImport("DirectXOverlayer.dll")]
        public static extern void LoadSave(string json);

        public static bool isSetting = false;
        public static bool isInitialized = false;

       



        public delegate void HelloWorldAPI();
        public delegate string GetModPathAPI();
        public delegate bool GetIsSettingAPI();
        public delegate void EndSettingAPI();
        public delegate void LogAPI(string msg);
        public delegate void SetInitializedAPI(bool value);
        public delegate bool GetIsPlayingAPI();
        public delegate string GetTranslationAPI(string key);
        public delegate string ApplyTagsAPI(string str);
        public delegate void OpenEditTextAPI(IntPtr txt);
        public delegate bool GetIsEditingTextAPI();

        static void HelloWorld()
        {
            Main.entry.Logger.Log("Hello, World from c++!");
        }
        static string GetModPath()
        {
            return Main.entry.Path;
        }
        static bool GetIsSetting()
        {
            return isSetting;
        }
        static void EndSetting()
        {
            isSetting = false;
            //Input.imeCompositionMode = IMECompositionMode.Auto;
            UnityModManager.UI.Instance.ToggleWindow(false);
        }
        static void Log(string msg)
        {
            Main.entry.Logger.Log(msg);
        }
        static void SetInitialized(bool value)
        {
            isInitialized = value;
        }
        static bool GetIsPlaying()
        {
            return Main.IsPlaying;
        }
        static string GetTranslation(string key)
        {
            return Main.translations[Main.language][key];
        }
        static string ApplyTags(string str)
        {
            var strc = str;
            foreach (var tag in Main.tags)
            {
                if (!tag.Value.Item2 && !Main.IsPlaying || !str.Contains($"&[{tag.Key}]")) continue;
                var val = isSetting ? tag.Value.Item3 : tag.Value.Item1();
                try
                {
                    strc = strc.Replace($"&[{tag.Key}]", val.ToString());
                }
                catch
                {
                    continue;
                }
            }
            return strc;
        }

        static void OpenEditText(IntPtr txt)
        {
            Main.entry.Logger.Log("Addr: " + txt);
            Main.curEditingText = txt;
            Main.isEditingText = true;
            
        }

        static bool GetIsEditingText()
        {
            return Main.isEditingText;
        }


        public static void Load()
        {
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new HelloWorldAPI(HelloWorld)), "HelloWorld");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new GetModPathAPI(GetModPath)), "GetModPath");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new GetIsSettingAPI(GetIsSetting)), "GetIsSetting");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new EndSettingAPI(EndSetting)), "EndSetting");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new LogAPI(Log)), "Log");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new SetInitializedAPI(SetInitialized)), "SetInitialized");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new GetIsPlayingAPI(GetIsPlaying)), "GetIsPlaying");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new GetTranslationAPI(GetTranslation)), "GetTranslation");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new ApplyTagsAPI(ApplyTags)), "ApplyTags");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new OpenEditTextAPI(OpenEditText)), "OpenEditText");
            RegisterAPI(Marshal.GetFunctionPointerForDelegate(new GetIsEditingTextAPI(GetIsEditingText)), "GetIsEditingText");

            Start();
        }
    }
}
