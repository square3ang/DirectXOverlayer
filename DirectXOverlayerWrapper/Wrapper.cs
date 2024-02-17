using System;
using System.CodeDom;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityModManagerNet;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
namespace DirectXOverlayer
{

    internal class Wrapper
    {
        public static Regex tagRegex = new Regex(@"&\[(.*?)\]");

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
        public delegate string ApplyTagsAPI(string str, bool simulateImGame);
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
        static string ApplyTags(string str, bool simulateInGame)
        {
            var strc = str;

            var mc = tagRegex.Matches(str);

            foreach (Match m in mc)
            {
                var d = m.Result("$1");
                var spl = d.Split(':');

                if (Main.tags.ContainsKey(spl[0]))
                {

                    

                    var tagname = spl[0];

                    

                    var tag = Main.tags[tagname];

                    

                    if (!tag.Item2 && (!Main.IsPlaying && !simulateInGame)) continue;

                    var val = isSetting ? tag.Item3 : tag.Item1();




                    var fmtstr = "";

                    if (spl.Length > 1 && val is double or float)
                    {
                        var f = spl[1];
                        if (!int.TryParse(f, out var i)) continue;
                        if (i > 0)
                        {
                            fmtstr = "0." + new string('#', i);
                        }
                        else if (i == 0)
                        {
                            fmtstr = "0";
                        }
                    }


                    if (fmtstr != "")
                    {
                        if (val is double)
                        {
                            strc = strc.Replace(m.Value, ((double)val).ToString(fmtstr));
                        }
                        else if (val is float)
                        {
                            strc = strc.Replace(m.Value, ((float)val).ToString(fmtstr));
                        }
                    } 
                    else
                    {
                        strc = strc.Replace(m.Value, val.ToString());
                    }
                    
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
