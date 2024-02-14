using DirectXOverlayer.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DirectXOverlayer.Tags
{
    public class HexCodes
    {
        [FieldTag("TEHex", true)]
        public static string TEHex = "FF0000FF";
        [FieldTag("VEHex", true)]
        public static string VEHex = "FF6F4EFF";
        [FieldTag("EPHex", true)]
        public static string EPHex = "A0FF4EFF";
        [FieldTag("PHex", true)]
        public static string PHex = "60FF4EFF";
        [FieldTag("LPHex", true)]
        public static string LPHex = "A0FF4EFF";
        [FieldTag("VLHex", true)]
        public static string VLHex = "FF6F4EFF";
        [FieldTag("TLHex", true)]
        public static string TLHex = "FF0000FF";
        [FieldTag("MPHex", true)]
        public static string MPHex = "00FFEDFF";
        [FieldTag("FMHex", true)]
        public static string FMHex = "D958FFFF";
        [FieldTag("FOHex", true)]
        public static string FOHex = "D958FFFF";
    }
}
