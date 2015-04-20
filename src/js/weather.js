var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    // Construct URL
    var url = "http://forecast.weather.gov/MapClick.php?lat=" +
    pos.coords.latitude + "&lon=" + pos.coords.longitude + "&FcstType=json";
    
    // Send request to OpenWeatherMap
    xhrRequest(url, 'GET', function(responseText) {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(responseText);

            var temperature = Math.round(json.currentobservation.Temp);

            // Conditions
            var conditions = json.currentobservation.Weather;
            var conditionCode;
            if (conditions.indexOf("Sunny") > -1) {
                conditionCode = "sun";
            }
            else if (conditions.indexOf("Clear") > -1) {
                conditionCode = "clear";
            }
            else if ((conditions.indexOf("Rain\/Snow") > -1) || (conditions.indexOf("Wintry") > -1) || (conditions.indexOf("Sleet") > -1) || (conditions.indexOf("Showers"))) { //Is WintryMix possible?
                conditionCode = "mix";
            }
            else if (conditions.indexOf("Cloudy") > -1) {
                conditionCode = "cloud";
            }
            else if (conditions.indexOf("Rain") > -1) {
                conditionCode = "rain";
            }
            else if ((conditions.indexOf("Thunderstorms") > -1) || (conditions.indexOf("Sleet") > -1)) {
                conditionCode = "thunderstorm";
            }
            else if ((conditions.indexOf("Breezy") > -1) || (conditions.indexOf("Wind") > -1)) {
                conditionCode = "wind";
            }
            else if (conditions.indexOf("Snow") > -1) {
                conditionCode = "snow";
            }
            else {
                conditionCode = "clear";
            }
               
            //High/low
            var high_low_temp;
            if (json.time.tempLabel[0] == "Low")
            {
                high_low_temp = json.data.temperature[1] + "°/" + json.data.temperature[0] + "°";
            }
            else
            {
                high_low_temp = json.data.temperature[0] + "°/" + json.data.temperature[1] + "°";
            }
            
            // Wind
            var winds = json.currentobservation.Winds;
            var windd_raw = json.currentobservation.Windd;
            var windd = "mph";
            if (windd_raw === 0 && winds === 0)
            {
                windd = "mph";
            }
            else if (windd_raw >= 337.6 && windd_raw <= 22.5)
            {
                windd = "N";
            }
            else if (windd_raw >= 22.5 && windd_raw <= 67.5)
            {
                windd = "NE";
            }
            else if (windd_raw >= 67.5 && windd_raw <= 112.5)
            {
                windd = "E";
            }
            else if (windd_raw >= 112.5 && windd_raw <= 157.5)
            {
                windd = "SE";
            }
            else if (windd_raw >= 157.5 && windd_raw <= 202.5)
            {
                windd = "S";
            }
            else if (windd_raw >= 202.5 && windd_raw <= 247.5)
            {
                windd = "SW";
            }
            else if (windd_raw >= 247.5 && windd_raw <= 292.5)
            {
                windd = "W";
            }
            else if (windd_raw >= 247.5 && windd_raw <= 337.5)
            {
                windd = "NW";
            }

            var dictionary = {
                "KEY_TEMPERATURE": temperature,
                "KEY_HIGH_LOW": high_low_temp,
                "KEY_CONDITION": conditionCode,
                "KEY_WINDS": winds,
                "KEY_WINDD": windd
            };
           
            Pebble.sendAppMessage(dictionary,
                function (e) {
                    console.log("Data sent to Pebble");
                },
                function (e) {
                    console.log("Error sending data");
                });
       }
   );
}

function locationError(err) {
    console.log("Error requesting location!");
}

function getWeather() {
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {timeout: 15000, maximumAge: 60000}
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
    function(e) {
        getWeather();
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
        getWeather();
    }
);