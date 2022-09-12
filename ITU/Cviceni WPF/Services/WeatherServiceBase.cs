using System;
using System.Threading.Tasks;
using WPFWeather.Models;
using WPFWeather.Services.OpenWeatherMapModels;

namespace WPFWeather.Services
{
    public abstract class WeatherServiceBase : IWeatherService
    {
        public async Task<WeatherInfoModel> DownloadWeatherAsync(string city)
        {
            var jsonString = await GetWeatherAsync(city.ToLower());

            if (string.IsNullOrEmpty(jsonString)) throw new ArgumentOutOfRangeException(nameof(city));

            var weatherFromJson = Openweathermap.FromJson(jsonString);

            return WeatherInfoMapper.Map(weatherFromJson);
        }

        protected abstract Task<string> GetWeatherAsync(string city);
    }
}