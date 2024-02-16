﻿using DirectXOverlayer.Attributes;
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
        [Tag("Fps", true)]
        public static float Fps() => 1 / Time.unscaledDeltaTime;
    }
}
