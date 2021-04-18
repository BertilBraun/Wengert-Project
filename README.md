# Wengert-Project
Watering and data collection Arduino project

## Implementation

Weather Data gets collected by a Froggit weather station.
Images get taken by a TTGO T-Call.

Data and Images get saved every 10min and relayed over a mobile router to a server page hosted at [meinwengert.de](https://meinwengert.de/).

## Future

 - [x] cleanup Arducam implementation [ref here](https://randomnerdtutorials.com/esp32-cam-post-image-photo-server/)
 - [x] secure connection to not allow any random requests (API Key?)
 - [x] implement proper names and units on website [ref here](https://github.com/ambient-weather/api-docs/wiki/Device-Data-Specs)
