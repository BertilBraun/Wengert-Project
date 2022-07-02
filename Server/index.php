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

$basePath = "cam/Wengert-Cam/Snapshot/";

$folders = array_reverse(scandir($basePath));
// folders is an array of dates in the form YYYY-MM-DD, get the 10 most recent ones
$folders = array_slice($folders, 0, min(10, count($folders)));

$files = [];
foreach ($folders as $folder) {
	// for each folder, get all the files in the subfolder "001"
	$folderFiles = array_reverse(scandir($basePath . $folder . "/001"));
	// add the files to the array with the complete path
	foreach ($folderFiles as $file) {
		$files[] = $basePath . $folder . "/001/" . $file;
	}
}

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

		#image:hover {
			opacity: 0.7;
		}

		/* The Modal (background) */
		.modal {
			display: none;
			/* Hidden by default */
			position: fixed;
			/* Stay in place */
			z-index: 1;
			/* Sit on top */
			padding-top: 100px;
			/* Location of the box */
			left: 0;
			top: 0;
			width: 100%;
			/* Full width */
			height: 100%;
			/* Full height */
			overflow: auto;
			/* Enable scroll if needed */
			background-color: rgb(0, 0, 0);
			/* Fallback color */
			background-color: rgba(0, 0, 0, 0.9);
			/* Black w/ opacity */
		}

		/* Modal Content (Image) */
		.modal-content {
			margin: auto;
			width: 80%;
			max-width: 1000px;

			/* align child items in a row */
			display: flex;
			flex-direction: row;
			justify-content: space-between;
			align-items: center;
		}

		/* Modal Image */
		#img01 {
			width: 100%;
			height: auto;
		}

		/* Caption of Modal Image (Image Text) - Same Width as the Image */
		#caption {
			margin: auto;
			display: block;
			width: 80%;
			max-width: 700px;
			text-align: center;
			color: #ccc;
			padding: 10px 0;
			height: 150px;
		}

		/* Add Animation - Zoom in the Modal */
		.modal-content,
		#caption {
			animation-name: zoom;
			animation-duration: 0.6s;
		}

		@keyframes zoom {
			from {
				transform: scale(0)
			}

			to {
				transform: scale(1)
			}
		}

		/* The Close Button */
		.close {
			position: absolute;
			top: 15px;
			right: 35px;
			color: #f1f1f1;
			font-size: 40px;
			font-weight: bold;
			transition: 0.3s;
		}

		.close:hover,
		.close:focus {
			color: #bbb;
			text-decoration: none;
			cursor: pointer;
		}

		/* 100% Image Width on Smaller Screens */
		@media only screen and (max-width: 700px) {
			.modal-content {
				width: 100%;
			}
		}

		.chart {
			padding-top: 40px;
		}
	</style>
</head>

<body>
	<!-- The Modal -->
	<div id="myModal" class="modal">

		<!-- The Close Button -->
		<span class="close">&times;</span>

		<!-- A Div, containing a Button to go left, then the img content, then a Button to go right -->
		<div class="modal-content">
			<!-- The Button to go left -->
			<button id="prev">&#10094;</button>

			<!-- The img content -->
			<img id="img01" src="">

			<!-- The Button to go right -->
			<button id="next">&#10095;</button>

		</div>

		<!-- Modal Caption (Image Text) -->
		<div id="caption"></div>
	</div>
	<h2>Wengert Wetter Station</h2>

	<div id="image-container">
		<select id="mySelect"> </select>
		<img id="image" alt="Wengert Image">
	</div>

	<div id="charts-container" />

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

		addChart(<?php echo $baromrelin; ?>.map(x => x * 33.863).map(x => Math.round(x * 10) / 10), 'Luftdruck', 'hPa', '#059e8a')

		addChart(<?php echo $humidity; ?>, 'Luftfeuchtigkeit', '%', '#059e8a', 0, 100)

		addChart(<?php echo $rainratein; ?>.map(x => Math.round(x * 25.4)), 'Niederschlagsmenge', 'mm', '#059e8a', 0)

		// addChart(<?php echo $eventrainin; ?>.map(x => Math.round(x * 25.4)), 'Regenschauer', 'mm', '#059e8a', 0)

		addChart(<?php echo $dailyrainin; ?>.map(x => Math.round(x * 25.4)), 'Tagesregenmenge', 'mm', '#059e8a', 0)

		addChart(<?php echo $weeklyrainin; ?>.map(x => Math.round(x * 25.4)), 'Wochenregenmenge', 'mm', '#059e8a', 0)

		addWindRose(<?php echo $winddir; ?>, <?php echo $windspeedmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10))

		addChart(<?php echo $winddir; ?>, 'Windrichtung', 'Grad', '#059e8a', 0, 360)

		addChart(<?php echo $windspeedmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10), 'Windgeschwindigkeit', 'km/h', '#059e8a')

		addChart(<?php echo $windgustmph; ?>.map(x => x * 1.609).map(x => Math.round(x * 10) / 10), 'Windboen', 'km/h', '#059e8a')

		// addChart(<?php //echo $baromabsin; 
					?>, 'Baromabsin', 'hPa', '#059e8a')

		addChart(<?php echo $solarradiation; ?>, 'Sonneneinstrahlung', 'W/m&sup2;', '#059e8a')

		addChart(<?php echo $uv; ?>, 'UV index', '', '#059e8a')

	</script>
	<script>

		const basePath = <?php echo json_encode($basePath); ?>;

		const files = <?php echo json_encode($files); ?>
						.filter((e) => e.endsWith(".jpg"))
						.sort()
						.reverse();

		// Get the select
		const mySelect = document.getElementById("mySelect");
		// Get the modal
		const modal = document.getElementById("myModal");

		// Get the image and insert it inside the modal - use its "alt" text as a caption
		const img = document.getElementById("image");
		const modalImg = document.getElementById("img01");
		const captionText = document.getElementById("caption");

		// Get the <span> element that closes the modal
		const span = document.getElementsByClassName("close")[0];

		const prev = document.getElementById("prev");
		const next = document.getElementById("next");

		const getImageName = (path) => {
			// path is of the form "cam/Wengert-Cam/Snapshot/YYYY-MM-DD/001/hh.mm.ss-hh.mm.ss.jpg"
			// return "YYYY-MM-DD : hh.mm.ss"
			const date = path.split("/")[3];
			const time = path.split("/")[5].split("-")[0].split(".");
			// modify hh by adding one hour 
			const hh = parseInt(time[0]);
			const modifiedTime = hh.toString().padStart(2, "0") + "." + time[1] + "." + time[2];

			return date + " : " + modifiedTime;
		}

		const setImage = (file) => {
			img.src = file;
			img.alt = getImageName(file);
		}
	
		for (const file of files) {
			const option = document.createElement("option");
			// set the text to only the filename without extension and path
			option.text = getImageName(file);
			option.value = file;
			mySelect.add(option);
		}

		mySelect.onchange = () => setImage(mySelect.value);
		mySelect.value = files[0];

		const updateModal = () => {
			modal.style.display = "block";
			modalImg.src = img.src;
			captionText.innerHTML = img.alt;
		}

		img.onclick = () => updateModal();
		setImage(files[0])

		// When the user clicks on <span> (x), close the modal
		span.onclick = () => modal.style.display = "none";

		next.onclick = () => {
			const index = files.indexOf(mySelect.value);
			if (index > 0) {
				mySelect.value = files[index - 1];
				setImage(files[index - 1]);
				updateModal();
			}
		}

		prev.onclick = () => {
			const index = files.indexOf(mySelect.value);
			if (index < files.length - 1) {
				mySelect.value = files[index + 1];
				setImage(files[index + 1]);
				updateModal();
			}
		}

		// When the user clicks anywhere outside of the modal, close it
		window.onclick = (event) => {
			if (event.target === modal) 
				modal.style.display = "none";
		}
	</script>
</body>

</html>
