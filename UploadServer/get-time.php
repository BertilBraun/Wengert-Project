<?php
	$hour = (int) date('H');

	if ($hour >= 8 && $hour <= 20) {
		echo 1;
  	} else {
		echo 0;
	}
?>