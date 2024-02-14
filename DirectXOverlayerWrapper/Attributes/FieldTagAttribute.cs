using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DirectXOverlayer.Attributes
{
    [AttributeUsage(AttributeTargets.Field)]
    public class FieldTagAttribute : Attribute
    {
        public string Name { get; private set; }
        public bool NonPlayingAvailable { get; private set; }
        public FieldTagAttribute(string Name, bool NonPlayingAvailable = false) { 
            this.Name = Name;
            this.NonPlayingAvailable = NonPlayingAvailable;
        }

    }
}
