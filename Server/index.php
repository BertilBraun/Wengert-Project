<?php

$servername = "rdbms.strato.de";

$dbname 	= "dbs1773217";
$username 	= "dbu1299844";
$password 	= "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT baromrelin, baromabsin, tempf, humidity, winddir, windspeedmph, windgustmph, rainratein, eventrainin, dailyrainin, weeklyrainin, monthlyrainin, yearlyrainin, totalrainin, solarradiation, uv, dateutc, id FROM `Weather Data` order by id desc limit 40";

$result = $conn->query($sql);

while ($data = $result->fetch_assoc())
	$sensor_data[] = $data;

$image_path 	= "/Images/" . max(array_reverse(array_column($sensor_data, "id"))) . ".jpg";

$dateutc		= json_encode(array_reverse(array_column($sensor_data, "dateutc")), JSON_NUMERIC_CHECK);
$baromrelin     = json_encode(array_reverse(array_column($sensor_data, "baromrelin")), JSON_NUMERIC_CHECK);
$baromabsin     = json_encode(array_reverse(array_column($sensor_data, "baromabsin")), JSON_NUMERIC_CHECK);
$tempf     		= json_encode(array_reverse(array_column($sensor_data, "tempf")), JSON_NUMERIC_CHECK);
$humidity       = json_encode(array_reverse(array_column($sensor_data, "humidity")), JSON_NUMERIC_CHECK);
$winddir        = json_encode(array_reverse(array_column($sensor_data, "winddir")), JSON_NUMERIC_CHECK);
$windspeedmph   = json_encode(array_reverse(array_column($sensor_data, "windspeedmph")), JSON_NUMERIC_CHECK);
$windgustmph    = json_encode(array_reverse(array_column($sensor_data, "windgustmph")), JSON_NUMERIC_CHECK);
$rainratein     = json_encode(array_reverse(array_column($sensor_data, "rainratein")), JSON_NUMERIC_CHECK);
$eventrainin    = json_encode(array_reverse(array_column($sensor_data, "eventrainin")), JSON_NUMERIC_CHECK);
$dailyrainin    = json_encode(array_reverse(array_column($sensor_data, "dailyrainin")), JSON_NUMERIC_CHECK);
$weeklyrainin   = json_encode(array_reverse(array_column($sensor_data, "weeklyrainin")), JSON_NUMERIC_CHECK);
$monthlyrainin  = json_encode(array_reverse(array_column($sensor_data, "monthlyrainin")), JSON_NUMERIC_CHECK);
$yearlyrainin   = json_encode(array_reverse(array_column($sensor_data, "yearlyrainin")), JSON_NUMERIC_CHECK);
$totalrainin    = json_encode(array_reverse(array_column($sensor_data, "totalrainin")), JSON_NUMERIC_CHECK);
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
	<h2>Wengert Weather Station</h2>
	<div id="charts-container">
	</div>

	<div id="image-container">
		<h3 id="image-title"> Wengert Image </h3>
		<select id="mySelect"> </select>
		<img id="image" alt="Wengert Image">
	</div>
	<script>
		let dateutc = <?php echo $dateutc; ?>;
		dateutc = dateutc.map((e) => new Date(e).toLocaleTimeString("de-De", {
			hour: '2-digit',
			minute: '2-digit'
		}))

		const addChart = (dataValues, title, unit, color) => {

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
						text: title + " (" + unit + ")"
					}
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

		const addImageSelect = () => {

			const files = <?php echo json_encode(array_reverse(scandir("Images/"))); ?>.filter((e) => e.endsWith(".jpg"));

			const mySelect = document.getElementById("mySelect");
			for (const f of files) {
				const option = document.createElement("option");
				option.text = f;
				mySelect.add(option);
			}

			mySelect.onchange = function() {
				document.getElementById("image").src = "/Images/" + mySelect.value;
			}

			document.getElementById("image").src = "/Images/" + files[files.length - 1];
		}

		addChart(<?php echo $baromrelin; ?>, 'Baromrelin', '.', '#059e8a')

		addChart(<?php echo $baromabsin; ?>, 'Baromabsin', '.', '#059e8a')

		addChart(<?php echo $tempf; ?>, 'Tempf', '.', '#059e8a')

		addChart(<?php echo $humidity; ?>, 'Humidity', '.', '#059e8a')

		addChart(<?php echo $winddir; ?>, 'Winddir', '.', '#059e8a')

		addChart(<?php echo $windspeedmph; ?>, 'Windspeedmph', '.', '#059e8a')

		addChart(<?php echo $windgustmph; ?>, 'Windgustmph', '.', '#059e8a')

		addChart(<?php echo $rainratein; ?>, 'Rainratein', '.', '#059e8a')

		addChart(<?php echo $eventrainin; ?>, 'Eventrainin', '.', '#059e8a')

		addChart(<?php echo $dailyrainin; ?>, 'Dailyrainin', '.', '#059e8a')

		addChart(<?php echo $weeklyrainin; ?>, 'Weeklyrainin', '.', '#059e8a')

		addChart(<?php echo $monthlyrainin; ?>, 'Monthlyrainin', '.', '#059e8a')

		addChart(<?php echo $yearlyrainin; ?>, 'Yearlyrainin', '.', '#059e8a')

		addChart(<?php echo $totalrainin; ?>, 'Totalrainin', '.', '#059e8a')

		addChart(<?php echo $solarradiation; ?>, 'Solarradiation', '.', '#059e8a')

		addChart(<?php echo $uv; ?>, 'UV', '.', '#059e8a')

		addImageSelect()
	</script>
</body>

</html>