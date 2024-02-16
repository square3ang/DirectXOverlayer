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
        public object Dummy { get; private set; }
        public FieldTagAttribute(string Name, bool NonPlayingAvailable = false, object Dummy = null)
        {
            this.Name = Name;
            this.NonPlayingAvailable = NonPlayingAvailable;
            this.Dummy = Dummy;
        }

    }
}
