using DirectXOverlayer.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DirectXOverlayer.Tags
{
    public class JudgeTags
    {
        [Tag("CurTE")]
        public static double TE() => GetCurDiffCount(HitMargin.TooEarly);
        [Tag("CurVE")]
        public static double VE() => GetCurDiffCount(HitMargin.VeryEarly);
        [Tag("CurEP")]
        public static double EP() => GetCurDiffCount(HitMargin.EarlyPerfect);
        [Tag("CurP")]
        public static double P() => GetCurDiffCount(HitMargin.Perfect);
        [Tag("CurLP")]
        public static double LP() => GetCurDiffCount(HitMargin.LatePerfect);
        [Tag("CurVL")]
        public static double VL() => GetCurDiffCount(HitMargin.VeryLate);
        [Tag("CurTL")]
        public static double TL() => GetCurDiffCount(HitMargin.TooLate);

        public static double GetCurDiffCount(HitMargin margin)
        {
            return scrController.instance.mistakesManager.GetHits(margin);
        }
    }
}
