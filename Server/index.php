<?php

$servername = "rdbms.strato.de";

$dbname 	= "dbs1773217";
$username 	= "dbu1299844";
$password 	= "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}

// $sql = "SELECT baromrelin, baromabsin, tempf, humidity, winddir, windspeedmph, windgustmph, rainratein, eventrainin, dailyrainin, weeklyrainin, monthlyrainin, yearlyrainin, totalrainin, solarradiation, uv, dateutc, id FROM `Weather Data` order by id desc limit 300";
$sql = "SELECT baromrelin, tempf, humidity, winddir, windspeedmph, windgustmph, rainratein, eventrainin, dailyrainin, weeklyrainin, solarradiation, uv, dateutc, id FROM `Weather Data` order by id desc limit 300";

$result = $conn->query($sql);

while ($data = $result->fetch_assoc())
	$sensor_data[] = $data;

$image_path 	= "/Images/" . max(array_reverse(array_column($sensor_data, "id"))) . ".jpg";

$dateutc		= json_encode(array_reverse(array_column($sensor_data, "dateutc")), JSON_NUMERIC_CHECK);
$baromrelin     = json_encode(array_reverse(array_column($sensor_data, "baromrelin")), JSON_NUMERIC_CHECK);
// $baromabsin     = json_encode(array_reverse(array_column($sensor_data, "baromabsin")), JSON_NUMERIC_CHECK);
$tempf     		= json_encode(array_reverse(array_column($sensor_data, "tempf")), JSON_NUMERIC_CHECK);
$humidity       = json_encode(array_reverse(array_column($sensor_data, "humidity")), JSON_NUMERIC_CHECK);
$winddir        = json_encode(array_reverse(array_column($sensor_data, "winddir")), JSON_NUMERIC_CHECK);
$windspeedmph   = json_encode(array_reverse(array_column($sensor_data, "windspeedmph")), JSON_NUMERIC_CHECK);
$windgustmph    = json_encode(array_reverse(array_column($sensor_data, "windgustmph")), JSON_NUMERIC_CHECK);
$rainratein     = json_encode(array_reverse(array_column($sensor_data, "rainratein")), JSON_NUMERIC_CHECK);
$eventrainin    = json_encode(array_reverse(array_column($sensor_data, "eventrainin")), JSON_NUMERIC_CHECK);
$dailyrainin    = json_encode(array_reverse(array_column($sensor_data, "dailyrainin")), JSON_NUMERIC_CHECK);
$weeklyrainin   = json_encode(array_reverse(array_column($sensor_data, "weeklyrainin")), JSON_NUMERIC_CHECK);
// $monthlyrainin  = json_encode(array_reverse(array_column($sensor_data, "monthlyrainin")), JSON_NUMERIC_CHECK);
// $yearlyrainin   = json_encode(array_reverse(array_column($sensor_data, "yearlyrainin")), JSON_NUMERIC_CHECK);
// $totalrainin    = json_encode(array_reverse(array_column($sensor_data, "totalrainin")), JSON_NUMERIC_CHECK);
$solarradiation = json_encode(array_reverse(array_column($sensor_data, "solarradiation")), JSON_NUMERIC_CHECK);
$uv				= json_encode(array_reverse(array_column($sensor_data, "uv")), JSON_NUMERIC_CHECK);

$result->free();
$conn->close();
?>

<!DOCTYPE html>
<html>

<head>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<title>Wengert Weather</title>

	<script src="https://code.highcharts.com/highcharts.js"></script>
	<script src="https://code.highcharts.com/highcharts-more.js"></script>
	<script src="https://code.highcharts.com/modules/data.js"></script>
	<script src="https://code.highcharts.com/modules/exporting.js"></script>
	<script src="https://code.highcharts.com/modules/export-data.js"></script>
	<script src="https://code.highcharts.com/modules/accessibility.js"></script>

	<style>
		body {
			min-width: 310px;
			max-width: 1280px;
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

		#image-container {
			width: 70%;
			margin: 0 auto;
		}

		#image-title {
			margin-top: 10px;
			text-align: center;
		}

		#image {
			border-width: medium;
			border-color: black;
			border-style: solid;
			width: 100%;
		}

		.chart {
			padding-top: 40px;
		}
	</style>
</head>

<body>
	<h2>Wengert Wetter Station</h2>
	<div id="charts-container">
	</div>

	<div id="image-container">
		<h3 id="image-title"> Wengert Image </h3>
		<select id="mySelect"> </select>
		<img id="image" alt="Wengert Image">
	</div>

	<script>
		let dateutc = <?php echo $dateutc; ?>;
		dateutc = dateutc.map((e) => {
			const date = new Date(e);

			if ((date.getHours() == 0 || date.getHours() == 12) && date.getMinutes() < 10) {
				return date.toLocaleDateString("de-De", {
					year: '2-digit',
					month: 'short',
					day: '2-digit',
					hour: '2-digit',
					minute: '2-digit'
				})
			} else {
				return date.toLocaleTimeString("de-De", {
					hour: '2-digit',
					minute: '2-digit'
				})
			}
		})

		const addWindRose = (direction, speed) => {

			const elemId = "wind-rose-container";

			document.getElementById("charts-container").innerHTML += '<div class="chart" id="' + elemId + '"></div>';

			const dir = direction[direction.length - 1]
			const spp = speed[speed.length - 1]

			const dirs = []

			for (let i = 1; i <= 360; i++)
				dirs.push(Math.abs(i - dir) <= 1 ? spp : 0)

			const cats = []
			const labels = ['N', 'NO', 'O', 'SO', 'S', 'SW', 'W', 'NW']

			for (let i = 0; i < 360; i++)
				cats.push(i % 45 == 0 ? labels[Math.floor(i / 45)] : i.toString() + '&deg;')

			setTimeout(() => Highcharts.chart(elemId, {
				series: [{
					type: 'column',
					pointPlacement: 'between',
					name: "Windgeschwindigkeit",
					showInLegend: false,
					data: dirs,
					color: '#FF0000',
				}],

				chart: {
					polar: true,
				},

				title: {
					text: 'Wind'
				},

				pane: {
					size: '85%'
				},

				xAxis: {
					tickmarkPlacement: 'on',
					min: 0,
					max: 360,
					tickInterval: 45,
					categories: cats
				},

				yAxis: {
					endOnTick: false,
					labels: {
						enabled: false,
					},
					min: 0,
					max: spp,
					gridLineWidth: 0
				},

				tooltip: {
					valueSuffix: 'km/h'
				},

				plotOptions: {
					column: {
						pointPadding: 0,
						groupPadding: 0
					}
				},
			}), 0);
		}

		const addChart = (dataValues, title, unit, color, minimum = null, maximum = null) => {

			const elemId = title + "-container";

			document.getElementById("charts-container").innerHTML += '<div class="chart" id="' + elemId + '"></div>';

			setTimeout(() => Highcharts.chart(elemId, {
				chart: {
					type: 'line',
					zoomType: 'x'
				},
				title: {
					text: title
				},
				xAxis: {
					categories: dateutc
				},
				yAxis: {
					title: {
						text: (!unit) ? title : title + " (" + unit + ")"
					},
					max: maximum,
					min: minimum
				},
				plotOptions: {
					line: {
						dataLabels: {
							enabled: false
						},
						color: color
					}
				},
				series: [{
					name: title,
					showInLegend: false,
					data: dataValues
				}]
			}), 0);
		}

		addChart(<?php echo $tempf; ?>.map(x => (x - 32) / 1.8).map(x => Math.round(x * 10) / 10), 'Temperatur', '&deg;C', '#059e8a')

		addChart(<?php echo $humidity; ?>, 'Luftfeuchtigkeit', '%', '#059e8a', 0, 100)

		addWindRose(<?php echo $winddir; ?>, <?php echo $windspeedmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10))

		addChart(<?php echo $winddir; ?>, 'Windrichtung', 'Grad', '#059e8a', 0, 360)

		addChart(<?php echo $windspeedmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10), 'Windgeschwindigkeit', 'km/h', '#059e8a')

		addChart(<?php echo $windgustmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10), 'Windboen', 'km/h', '#059e8a')

		addChart(<?php echo $baromrelin; ?>.map(x => x * 33.863).map(x => Math.round(x * 10) / 10), 'Luftdruck', 'hPa', '#059e8a')

		// addChart(<?php //echo $baromabsin; 
					?>, 'Baromabsin', 'hPa', '#059e8a')

		addChart(<?php echo $solarradiation; ?>, 'Sonneneinstrahlung', 'W/m&sup2;', '#059e8a')

		addChart(<?php echo $uv; ?>, 'UV index', '', '#059e8a')

		addChart(<?php echo $rainratein; ?>.map(x => Math.round(x * 25.4)), 'Niederschlagsmenge', 'mm', '#059e8a', 0)

		addChart(<?php echo $eventrainin; ?>.map(x => Math.round(x * 25.4)), 'Regenschauer', 'mm', '#059e8a', 0)

		addChart(<?php echo $dailyrainin; ?>.map(x => Math.round(x * 25.4)), 'Tagesregenmenge', 'mm', '#059e8a', 0)

		addChart(<?php echo $weeklyrainin; ?>.map(x => Math.round(x * 25.4)), 'Wochenregenmenge', 'mm', '#059e8a', 0)

		// addChart(<?php // echo $monthlyrainin; 
					?>, 'Monthlyrainin', 'mm', '#059e8a')

		// addChart(<?php // echo $yearlyrainin; 
					?>, 'Yearlyrainin', 'mm', '#059e8a')

		// addChart(<?php // echo $totalrainin; 
					?>, 'Totalrainin', 'mm', '#059e8a')

		const addImageSelect = () => {

			const files = <?php
							$arr = array_reverse(scandir("Images/"));
							$arr = array_slice($arr, 0, min(100, count($arr)));
							echo json_encode($arr);
							?>.filter((e) => e.endsWith(".jpg"));

			files.sort();

			const mySelect = document.getElementById("mySelect");
			for (const f of files) {
				const option = document.createElement("option");
				option.text = f;
				mySelect.add(option);
			}

			mySelect.onchange = function() {
				document.getElementById("image").src = "/Images/" + mySelect.value;
			}

			mySelect.value = files[0]

			document.getElementById("image").src = "/Images/" + files[0];
		}

		addImageSelect()
	</script>
</body>

</html>