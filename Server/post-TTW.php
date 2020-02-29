<?php

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

$servername = "rdbms.strato.de";

$dbname = "DB3920559";
$username = "U3920559";
$password = "(L3mb3rg3rLand!)";

$api_key_value = "tPmAT5Ab3j7F9";

if (test_input($_POST["api_key"]) == $api_key_value) {
	
	$timeToWater 	= test_input($_POST["timeToWater"]);
	
	$conn = new mysqli($servername, $username, $password, $dbname);
	if ($conn->connect_error)
		die("Connection failed: " . $conn->connect_error);
	
	$id = $conn->query("SELECT id FROM SensorData WHERE id = (SELECT MAX(id) FROM SensorData)")->fetch_object()->id;
	
	if ($conn->query("UPDATE SensorData SET timeToWater = " . $timeToWater . " WHERE id = " . $id))
		echo "Updated successfully";
	else 
		echo "Error: " . $conn->error;

	$conn->close();
}
else 
	echo "Wrong API Key provided.";

?>