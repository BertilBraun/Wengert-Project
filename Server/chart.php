<?php

$servername = "rdbms.strato.de";

$dbname = "DB3920559";
$username = "U3920559";
$password = "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT humidity, ground_humidity, temperature, pressure, timeToWater, readingTime FROM SensorData order by readingTime desc limit 40";

$result = $conn->query($sql);

while ($data = $result->fetch_assoc())
    $sensor_data[] = $data;

$value1 		= json_encode(array_reverse(array_column($sensor_data, 'humidity')), JSON_NUMERIC_CHECK);
$value2 		= json_encode(array_reverse(array_column($sensor_data, 'ground_humidity')), JSON_NUMERIC_CHECK);
$value3 		= json_encode(array_reverse(array_column($sensor_data, 'temperature')), JSON_NUMERIC_CHECK);
$value4 		= json_encode(array_reverse(array_column($sensor_data, 'pressure')), JSON_NUMERIC_CHECK);
$value5 		= json_encode(array_reverse(array_column($sensor_data, 'timeToWater')), JSON_NUMERIC_CHECK);
$reading_time 	= json_encode(array_reverse(array_column($sensor_data, 'readingTime')), JSON_NUMERIC_CHECK);


$sql = "SELECT readingTime FROM SensorData WHERE id = (SELECT MAX(id) FROM SensorData);";
$image_path = 'Images/'.$conn->query($sql)->fetch_object()->readingTime.'.jpg';
	
$result->free();
$conn->close();
?>

<!DOCTYPE html>
<html>
<meta name="viewport" content="width=device-width, initial-scale=1">
	<script src="https://code.highcharts.com/highcharts.js"></script>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.1.0/jquery.min.js"></script>
	<style>
	body {
		min-width: 310px;
		max-width: 1280px;
		height: 500px;
		margin: 0 auto;
	}
	h2 {
		font-family: Arial;
		font-size: 2.5rem;
		text-align: center;
	}
	h3 {
		font-family: Arial;
		font-size: 1.7rem;
	}
	#image-title {
		text-align: center;
	}
	#image {
		width: 70%;
		border-width: medium;
		border-color: black;
		border-style: solid;
		margin: 15%;
		margin-top: 10px;
	}	
	.slider-container {
		margin-top: 50px;
		margin-bottom: 100px;
	}
	.slider {
		-webkit-appearance: none;
		width: 100%;
		height: 15px;
		border-radius: 5px;  
		background: #d3d3d3;
		outline: none;
		opacity: 0.7;
		-webkit-transition: .2s;
		transition: opacity .2s;
	}
	.slider::-webkit-slider-thumb {
		-webkit-appearance: none;
		appearance: none;
		width: 25px;
		height: 25px;
		border-radius: 50%; 
		background: #4CAF50;
		cursor: pointer;
	}
	.slider::-moz-range-thumb {
		width: 25px;
		height: 25px;
		border-radius: 50%;
		background: #4CAF50;
		cursor: pointer;
	}
	</style>
	<body>
		<h2>ESP Weather Station</h2>
		<div id="chart-temperature" class="container"></div>
		<div id="chart-humidity" class="container"></div>
		<div id="chart-ground-humidity" class="container"></div>
		<div id="chart-pressure" class="container"></div>
		<div id="chart-timeToWater" class="container"></div>
		<div class="slider-container">
			<h3 id="TTW-header"/>
			<h3 style="float: left;  margin: auto; padding-right: 30px;">0</h3>
			<input type="range" min="0" max="300" value="0" class="slider" id="myRange" style="float: left; width: 80%; margin: auto;">
			<h3 style="float: left;  margin: auto; padding-left: 30px;">300</h3>
		</div>
		<div id="image-container">
			<h3 id="image-title"> Wengert Image </h3>
			<img id="image" alt="Wengert Image">
		</div>
	<script>
	var value1 = <?php echo $value1; ?>;
	var value2 = <?php echo $value2; ?>;
	var value3 = <?php echo $value3; ?>;
	var value4 = <?php echo $value4; ?>;
	var value5 = <?php echo $value5; ?>;
	var reading_time = <?php echo $reading_time; ?>;
	var image_path = "<?php echo $image_path; ?>";
	
	var chartT = new Highcharts.Chart({
	  chart:{ renderTo : 'chart-temperature' },
	  title: { text: 'Temperature' },
	  series: [{
		showInLegend: false,
		data: value3
	  }],
	  plotOptions: {
		line: { animation: false,
		  dataLabels: { enabled: true }
		},
		series: { color: '#059e8a' }
	  },
	  xAxis: { 
		type: 'datetime',
		categories: reading_time
	  },
	  yAxis: {
		title: { text: 'Temperature (Celsius)' }
	  },
	  credits: { enabled: false }
	});

	var chartH = new Highcharts.Chart({
	  chart:{ renderTo:'chart-humidity' },
	  title: { text: 'Humidity' },
	  series: [{
		showInLegend: false,
		data: value1
	  }],
	  plotOptions: {
		line: { animation: false,
		  dataLabels: { enabled: true }
		}
	  },
	  xAxis: {
		type: 'datetime',
		//dateTimeLabelFormats: { second: '%H:%M:%S' },
		categories: reading_time
	  },
	  yAxis: {
		title: { text: 'Humidity (%)' }
	  },
	  credits: { enabled: false }
	});

	var chartGH = new Highcharts.Chart({
	  chart:{ renderTo:'chart-ground-humidity' },
	  title: { text: 'Ground Humidity' },
	  series: [{
		showInLegend: false,
		data: value2
	  }],
	  plotOptions: {
		line: { animation: false,
		  dataLabels: { enabled: true }
		}
	  },
	  xAxis: {
		type: 'datetime',
		//dateTimeLabelFormats: { second: '%H:%M:%S' },
		categories: reading_time
	  },
	  yAxis: {
		title: { text: 'Ground Humidity (%)' }
	  },
	  credits: { enabled: false }
	});

	var chartP = new Highcharts.Chart({
	  chart:{ renderTo:'chart-pressure' },
	  title: { text: 'Pressure' },
	  series: [{
		showInLegend: false,
		data: value4
	  }],
	  plotOptions: {
		line: { animation: false,
		  dataLabels: { enabled: true }
		},
		series: { color: '#18009c' }
	  },
	  xAxis: {
		type: 'datetime',
		categories: reading_time
	  },
	  yAxis: {
		title: { text: 'Pressure (hPa)' }
	  },
	  credits: { enabled: false }
	});

	var createTTW = function() {
		return new Highcharts.Chart({
			chart:{ renderTo:'chart-timeToWater' },
			title: { text: 'Time To Water' },
			series: [{
			showInLegend: false,
			data: value5
			}],
			plotOptions: {
			line: { animation: false,
			  dataLabels: { enabled: true }
			},
			series: { color: ' #4CAF50' }
			},
			xAxis: {
			type: 'datetime',
			categories: reading_time
			},
			yAxis: {
			title: { text: 'Time To Water (min)' }
			},
			credits: { enabled: false }
		});
	}

	var chartTTW = createTTW();
	
	var slider = document.getElementById("myRange");
	var get = new XMLHttpRequest();
	get.open('GET', 'get-data.php', true);
	get.onload = function() {
		slider.value = parseInt(get.response, 10);
		slider.oninput();
	};
	get.send();
	slider.oninput = function() {
		document.getElementById("TTW-header").innerText = "Time to Water: " + slider.value + "min";
	}
	slider.onchange = function() {
		var post = new XMLHttpRequest();
		post.open('POST', 'post-TTW.php', true);
		post.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
		post.send('api_key=tPmAT5Ab3j7F9&timeToWater=' + slider.value);
		value4[value4.length - 1] = parseInt(slider.value, 10);
		chartTTW = createTTW();
	}
	
	document.getElementById("image").src = "/<?php echo $image_path; ?>";
	
	</script>
</body>
</html>