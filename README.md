# Solar powered Particle Photon environment monitor

Here is the Photon code and hardware details to build a solar powered environment monitor. It uses a combo module of 3 separate sensors that are read via I2C protocol to periodically measure and record:

  * Temperature
  * Humidity
  * Light level
  * Air pressure
  * Battery charge

The 3 sensors are:

  * [HTU21D](https://www.adafruit.com/datasheets/1899_HTU21D.pdf) - Temperature and humidity sensor.
  * [Bosch BMP180](https://www.bosch-sensortec.com/en/homepage/products_3/environmental_sensors_1/bmp180_1/bmp180) - Digital barometric pressure and temperature sensor.
  * [Rohm BH1750FVI](http://rohmfs.rohm.com/en/products/databook/datasheet/ic/sensor/light/bh1750fvi-e.pdf) - Ambient light sensor.

After the measurements are taken the values are sent to [Dweet](http://dweet.io/). The Photon then measure its own battery level and will deep sleep for a variable amount of time between measurements in order to conserve battery life. The sleep time will be longer when the battery gets low.

The following libraries are used:

  * https://github.com/amcewen/HttpClient
  * https://github.com/sparkfun/Photon_Battery_Shield
  * https://github.com/adafruit/Adafruit-BMP085-Library
  * https://github.com/sparkfun/SparkFun_HTU21D_Breakout_Arduino_Library/

## Hardware

  * [Particle Photon](https://docs.particle.io/datasheets/photon-datasheet/).
  * [SparkFun Photon Battery Shield](https://www.sparkfun.com/products/13626): Used to charge the battery from the solar panel and read the battery charge level.
  * 5V 500mA 2.5W 140x140mm Solar panel: Purchased from eBay.
  * HTU21D/BMP180/BH1750FVI combo sensor: Purchased from AliExpress. [Some examples here](http://www.aliexpress.com/wholesale?SearchText=HTU21D+BMP180+BH1750FVI).
  * Polymer Lithium Ion Battery 400mAh.

The pin connections are very simple as there is only one way to connect the battery, solar panel and sensor (just uses standard I2C pins).

## Photos

![Dweet](../master/images/dweet.jpg?raw=true)

![photon-env-monitor1](../master/images/photon-env-monitor1.jpg?raw=true)

![photon-env-monitor2](../master/images/photon-env-monitor2.jpg?raw=true)
