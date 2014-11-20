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
    xhrRequest(url, 'GET',
   function(responseText) {
       // responseText contains a JSON object with weather info
       var json = JSON.parse(responseText);
       
       // Temperature in Kelvin requires adjustment
       var temperature = Math.round(json.currentobservation.Temp);
       console.log("Temperature is " + temperature);
       
       // Conditions
       var conditions = json.currentobservation.Weather;
       console.log("Conditions are " + conditions);
   } /,
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
        console.log("PebbleKit JS ready!");

        // Get the initial weather
        getWeather();
    }
);