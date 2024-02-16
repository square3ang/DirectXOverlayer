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
        [Tag("CurTE", Dummy: 1)]
        public static double TE() => GetCurDiffCount(HitMargin.TooEarly);
        [Tag("CurVE", Dummy: 3)]
        public static double VE() => GetCurDiffCount(HitMargin.VeryEarly);
        [Tag("CurEP", Dummy: 101)]
        public static double EP() => GetCurDiffCount(HitMargin.EarlyPerfect);
        [Tag("CurP", Dummy: 1250)]
        public static double P() => GetCurDiffCount(HitMargin.Perfect);
        [Tag("CurLP", Dummy: 81)]
        public static double LP() => GetCurDiffCount(HitMargin.LatePerfect);
        [Tag("CurVL", Dummy: 1)]
        public static double VL() => GetCurDiffCount(HitMargin.VeryLate);
        [Tag("CurTL", Dummy: 0)]
        public static double TL() => GetCurDiffCount(HitMargin.TooLate);
        [Tag("CurMiss", Dummy: 0)]
        public static double Miss() => GetCurDiffCount(HitMargin.FailMiss);
        [Tag("CurOverload", Dummy: 0)]
        public static double Overload() => GetCurDiffCount(HitMargin.FailOverload);

        public static double GetCurDiffCount(HitMargin margin)
        {
            return scrController.instance.mistakesManager.GetHits(margin);
        }
    }
}
