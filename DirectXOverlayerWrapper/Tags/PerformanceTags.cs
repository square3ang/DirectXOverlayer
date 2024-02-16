using DirectXOverlayer.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace DirectXOverlayer.Tags
{
    public class PerformanceTags
    {
        [FieldTag("Fps", true, Dummy: 144.3712)]
        public static float Fps = 0;
    }
}
