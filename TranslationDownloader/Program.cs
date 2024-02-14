// See https://aka.ms/new-console-template for more information
using Newtonsoft.Json.Linq;
using System.Net;

Console.WriteLine("TranslationDownloader for DirectXOverlayer");



string GetUrl(int gid) =>
            $"https://docs.google.com/spreadsheets/d/1aOfrMoBgV-ZPZ1K-h3-wrF4Lguvnp9GlYHa4gWgAQTI/gviz/tq?tqx=out:json&tq&gid={gid}";
var cl = new HttpClient();
foreach (var language in Enum.GetValues(typeof(GID)))
{

    var resp = await cl.GetAsync(GetUrl((int)language));
    var json = await resp.Content.ReadAsStringAsync();

    var dir = string.Join(' ', args);
    json = json.Substring(47, json.Length - 49);

    var ps = JObject.Parse(json);

    var nj = new JObject();

    foreach (var j in ps["table"]["rows"].ToArray())
    {
        var key = j["c"][0]["v"];
        var value = j["c"][1]["v"];

        nj[key.ToString()] = value.ToString();
    }


    File.WriteAllText(Path.Combine(dir, Enum.GetName(typeof(GID), language) + ".language"), nj.ToString());
}

public enum GID : int
{
    KOREAN = 0,
    ENGLISH = 1718932483
}



