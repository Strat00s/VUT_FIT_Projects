using System;
using Newtonsoft.Json;

namespace WPFWeather.Services.OpenWeatherMapModels
{
    public class Clouds
    {
        [JsonProperty("all")] public long All { get; set; }
    }
}