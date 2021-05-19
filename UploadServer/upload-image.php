<?php

function log_to_file($msg)
{
    $result = $msg . "\n";
    file_put_contents("log.txt", $result . file_get_contents("log.txt"));
    echo $result;
}

$ERROR_CODES = array(
    0 => "There is no error, the file uploaded with success",
    1 => "The uploaded file exceeds the upload_max_filesize directive in php.ini",
    2 => "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form",
    3 => "The uploaded file was only partially uploaded",
    4 => "No file was uploaded",
    6 => "Missing a temporary folder"
);

$path = date("Y/m/d H:i") . '.jpg';

log_to_file("Image Upload " . $path);

$pass_key_value = "58B6E6B64C7A088FA18CAB6A84668F9E";
$file = $_FILES[$pass_key_value];

if ($file['error'] === UPLOAD_ERR_OK && move_uploaded_file($file['tmp_name'], $path)) {
    log_to_file("The file has been uploaded");
} else {
    log_to_file("There was an error uploading the file: " . $ERROR_CODES[$file['error']]);
}

?>