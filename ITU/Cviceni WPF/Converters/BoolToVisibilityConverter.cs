using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace WPFWeather.Converters
{
    public class BoolToVisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var boolValue = (bool) value;
            var boolParameter = parameter != null ? bool.Parse((string) parameter) : false;

            if (boolParameter) return boolValue ? Visibility.Hidden : Visibility.Visible;
            return boolValue ? Visibility.Visible : Visibility.Hidden;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}