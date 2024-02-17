using DirectXOverlayer.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DirectXOverlayer.Tags
{
    public class GamePlayTags
    {
        [FieldTag("RealBpm", Dummy: 200)]
        public static double RealBpm;
        [FieldTag("TileBpm", Dummy: 100)]
        public static double TileBpm;
        [FieldTag("ReqKps", Dummy: 200d / 60d)]
        public static double ReqKps;
    }
}
