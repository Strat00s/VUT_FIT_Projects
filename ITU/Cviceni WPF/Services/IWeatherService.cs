using System;
using System.Threading.Tasks;
using WPFWeather.Models;

namespace WPFWeather.Services
{
    public interface IWeatherService
    {
        Task<WeatherInfoModel> DownloadWeatherAsync(string city);
    }
}