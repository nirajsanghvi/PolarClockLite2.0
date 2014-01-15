var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  var saveOptions = JSON.parse(window.localStorage.getItem("options"));
	
  var url = "http://niraj.com/pebble/polar_clock_lite_config.php?v=1.0";
	
  if(saveOptions != null) {
    url += "&time=" + (saveOptions["polarclockL0"] ? encodeURIComponent(saveOptions["polarclockL0"]) : "") + 
           "&date=" + (saveOptions["polarclockL1"] ? encodeURIComponent(saveOptions["polarclockL1"]) : "") +
		   "&rowd=" + (saveOptions["polarclockL2"] ? encodeURIComponent(saveOptions["polarclockL2"]) : "") +
		   "&invt=" + (saveOptions["polarclockL3"] ? encodeURIComponent(saveOptions["polarclockL3"]) : "") +
		   "&four=" + (saveOptions["polarclockL4"] ? encodeURIComponent(saveOptions["polarclockL4"]) : "");
  }
	
  console.log("Showing configuration: " + url);
  Pebble.openURL(url);
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
	if(e.response && e.response.length>5) {
			var options = JSON.parse(decodeURIComponent(e.response));
			
			console.log("Options = " + JSON.stringify(options));
		
			var saveOptions = {
				"polarclockL0": options["0"],
				"polarclockL1": options["1"],
				"polarclockL2": options["2"],
				"polarclockL3": options["3"],
				"polarclockL4": options["4"]
			};
		
			window.localStorage.setItem("options", JSON.stringify(saveOptions));
			
			Pebble.sendAppMessage(options,
				function(e) {
					console.log("Successfully sent options to Pebble");
				},
				function(e) {
					console.log("Failed to send options to Pebble.\nError: " + e.error.message);
				}
			);
	} else {
		
		console.log("Error with JS Config options received.");	
	}
});