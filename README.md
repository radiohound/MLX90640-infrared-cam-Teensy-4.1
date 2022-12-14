# MLX90640-infrared-cam-Teensy-4.1
This project displays heat related image from a Melexis MLX90640 ir thermal camera to a TFT display and locates the highest temp. It was based off Adafruit's MLX90640 code from <a href="https://github.com/adafruit/Adafruit_MLX90640/blob/master/examples/MLX90640_arcadaCam/MLX90640_arcadaCam.ino">here.</a> This project uses a 1.8" TFT display module 160x128 pixels (a ST7735 display) and a Teensy 4.1 from www.pjrc.com.

It surprised me that such a small infrared imaging sensor could get a fairly reasonable image of with only a 32x24 array. I can also see the heat from a small wifi mesh device about 14 feet away, which appears as a few pixels wide. Although it identifies the hot pixel in each of the photos, its easiest to see in the image of one of my wifi routers. This is the last of the three photos. 


<img src="https://github.com/radiohound/MLX90640-infrared-cam-Teensy-4.1/blob/main/coffee_mug.jpg" width=25% height=25%>

<img src="https://github.com/radiohound/MLX90640-infrared-cam-Teensy-4.1/blob/main/face_hand.jpg" width=25% height=25%>

<img src="https://github.com/radiohound/MLX90640-infrared-cam-Teensy-4.1/blob/main/wifi-14ft.jpg" width=25% height=25%>

<iframe width="420" height="315"
src="https://www.youtube.com/embed/UcrW1sQX-8s?playlist=UcrW1sQX-8s&loop=1">
</iframe>
