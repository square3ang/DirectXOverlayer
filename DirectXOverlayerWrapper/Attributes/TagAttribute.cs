using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DirectXOverlayer.Attributes
{
    [AttributeUsage(AttributeTargets.Method)]
    public class TagAttribute : Attribute
    {
        public string Name { get; private set; }
        public bool NonPlayingAvailable { get; private set; }
        public TagAttribute(string Name, bool NonPlayingAvailable = false)
        {
            this.Name = Name;
            this.NonPlayingAvailable = NonPlayingAvailable;
        }
    }
}
