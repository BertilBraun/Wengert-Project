<?php

function log_to_file($msg) {
    $result = $msg . "\n";
    file_put_contents("log.txt", $result, FILE_APPEND);
}

$servername = "rdbms.strato.de";

$dbname 	= "dbs1773217";
$username 	= "dbu1299844";
$password 	= "(L3mb3rg3rLand!)";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) 
	die("Connection failed: " . $conn->connect_error);

$sql = "SELECT id FROM `Weather Data` WHERE id = (SELECT MAX(id) FROM `Weather Data`);";
$path = '../Images/'.$conn->query($sql)->fetch_object()->id.'.jpg';

$conn->close();

log_to_file("Image Upload " . $path);
	
$pass_key_value = "58B6E6B64C7A088FA18CAB6A84668F9E";

if (move_uploaded_file($_FILES[$pass_key_value]['tmp_name'], $path)) {
    log_to_file("The file has been uploaded");
    echo "The file has been uploaded";
} else {
    log_to_file("There was an error uploading the file, please try again! " . error_get_last());
    echo "There was an error uploading the file, please try again!";
}

?>