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
            console.log("Temperature is " + temperature);

            // Conditions
            var conditions = json.currentobservation.Weather;
            console.log("Conditions are " + conditions);
               
            //High/low
           var high_low_temp = json.data.temperature[0] + "°/" + json.data.temperature[1] + "°";
            
           var dictionary = {
                "KEY_TEMPERATURE": temperature,
                "KEY_CONDITIONS": conditions,
                "KEY_HIGH_LOW": high_low_temp,
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
        console.log("AppMessage received!");
        getWeather();
    }
);