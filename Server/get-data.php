<?php

$servername = "rdbms.strato.de";

$dbname = "DB3920559";
$username = "U3920559";
$password = "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) 
    die("Connection failed: " . $conn->connect_error);

$sql = "SELECT timeToWater FROM SensorData WHERE id = (SELECT MAX(id) FROM SensorData);";

echo $conn->query($sql)->fetch_object()->timeToWater;

$conn->close();
?>
